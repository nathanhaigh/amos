#include "foundation_AMOS.hh"
#include "delcher.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace HASHMAP;
using namespace AMOS;

using namespace std;

typedef vector<bool> Mer_t;

int Kmer_Len = 30;

const char * bintoascii = "ACGT";

//  Return the binary equivalent of  ch .
static unsigned  Char_To_Binary (char ch)
{
  switch  (tolower (ch))
  {
    case  'a' : return  0;
    case  'c' : return  1;
    case  'g' : return  2;
    case  't' : return  3;
    default: return 0;
  };

  return  0;
}

void InitMer(Mer_t & mer)
{
  mer.clear();
  mer.resize(Kmer_Len*2);
}

static void MerToAscii(Mer_t mer, string & s)
{
  s.erase();

  int l = mer.size()/2;

  s.resize(l);

  for (int i = 0; i < l; i++)
  {
    char m = 0;
    m |= mer[i*2+1] << 1;
    m |= mer[i*2];

    s[i] = bintoascii[m];
  }
}

static void  Forward_Add_Ch(Mer_t & mer, char ch)
{
  // delete the first character from kmer
  for (int i = 0; i < (Kmer_Len-1)*2; i++)
  {
    mer[i] = mer[i+2];
  }

  // append to end
  ch = Char_To_Binary(ch);
  mer[Kmer_Len*2-1] = ch & 2;
  mer[Kmer_Len*2-2] = ch & 1;
}

// Return the first len characters of mer
Mer_t prefix(const Mer_t & mer, int len)
{
  Mer_t retval(mer);
  retval.resize(len*2);

  if (0)
  {
    string z,y;
    MerToAscii(mer, z);
    MerToAscii(retval, y);
    cerr << "Prefix of " << z << " is " << y << endl;
  }

  return retval;
}

// Return the last len characters of mer
Mer_t suffix(const Mer_t & mer, int len)
{
  Mer_t retval;
  retval.resize(len*2);

  int ol = mer.size();
  for (int i = 0; i < len*2; i++)
  {
    retval[i] = mer[ol-len*2+i];
  }

  if (0)
  {
    string z,y;
    MerToAscii(mer, z);
    MerToAscii(retval, y);
    cerr << "Suffix of " << z << " is " << y << endl;
  }

  return retval;
}

void extendMer(Mer_t & mer, const Mer_t & extend)
{
  int DEBUG = 0;
  if (DEBUG)
  {
    string u,v;
    MerToAscii(mer, u);
    MerToAscii(extend, v);

    cerr << "Extending " << u << " with " << v << endl;
  }

  Mer_t suff (suffix(extend, extend.size()/2 - (Kmer_Len-2)));
  mer.insert(mer.end(), suff.begin(), suff.end());

  if (DEBUG)
  {
    string z;
    MerToAscii(mer, z);

    cerr << ": " << z << endl;
  }
}



int NC=0;

class MerVertex_t
{
public:
  MerVertex_t(Mer_t s) : mer_m(s), node_m(NC++), dead(false) {}
  Mer_t mer_m;
  int   node_m;
  bool  dead;

  vector<MerVertex_t *> out_m;
  vector<MerVertex_t *> in_m;

  bool compressable()
  {
    if (out_m.empty() || (out_m[0] == this))
    {
      return false;
    }

    int ol = out_m.size();
    for (int i = 1; i < ol; i++)
    {
      if (out_m[i] != out_m[0])
      {
        return false;
      }
    }

    int il = out_m[0]->in_m.size();

    if (ol != il)
    {
      return false;
    }

    for (int i = 0; i < il; i++)
    {
      if (out_m[0]->in_m[i] != this)
      {
        return false;
      }
    }


    return true;
  }

  void collapse()
  {
    MerVertex_t * buddy = out_m[0];
    //cerr << "Collapse " << node_m << " and " << buddy->node_m << endl;
    out_m = buddy->out_m;
    buddy->dead = true;
    extendMer(mer_m, buddy->mer_m);

    for (int i = 0; i < out_m.size(); i++)
    {
      MerVertex_t * bo = out_m[i];

      for (int j = 0; j < bo->in_m.size(); j++)
      {
        if (bo->in_m[j] == buddy)
        {
          bo->in_m[j] = this;
          break;
        }
      }
    }

  }

  string str()
  {
    string r;
    MerToAscii(mer_m, r);

    return r;
  }
};

namespace HASHMAP
{
  template<> struct hash< Mer_t >
  {
    size_t operator()( const Mer_t& x ) const
    {
      size_t p = 0;
      int l = x.size();
      for (int i = 0; i < l; i++)
      {
        p += ((x[i]<<i));
      }

      return p;
    }
  };
}

typedef HASHMAP::hash_map<Mer_t, MerVertex_t *> MerTable_t;

class deBrujinGraph_t
{
public:

  deBrujinGraph_t(const string & tag): tag_m(tag) {}

  string tag_m;
  MerTable_t mers_m;

  MerVertex_t * getVertex(const Mer_t & mer)
  {
    if (0)
    {
      string z;
      MerToAscii(mer, z);
      cerr << "getVertex " << z << endl;
    }

    MerTable_t::iterator m = mers_m.find(mer);
    if (m == mers_m.end())
    {
      m = mers_m.insert(make_pair(mer, new MerVertex_t(mer))).first;
    }

    return m->second;
  }

  void addMer(const Mer_t & mer)
  {
    if (0)
    {
      string z;
      MerToAscii(mer, z);
      cerr << "Processing " << z << endl;
    }

    MerVertex_t * p = getVertex(prefix(mer, Kmer_Len-1));
    MerVertex_t * s = getVertex(suffix(mer, Kmer_Len-1));

    p->out_m.push_back(s);
    s->in_m.push_back(p);
  }

  int nodeCount()
  {
    return mers_m.size();
  }

  void print()
  {
    cout << "digraph G {" << endl
         << " center=true" << endl
         << " label=\"deBrujin Graph of " << tag_m << " nc: " << nodeCount() << "\"" << endl;


    MerTable_t::iterator mi;
    for (mi = mers_m.begin(); mi != mers_m.end(); mi++)
    {
      assert (!mi->second->dead);

      string s(mi->second->str());
      int slen = s.length();
      if (slen > 8)
      {
        string q = s.substr(0,4);
        q += "...";
        q += s.substr(s.length()-4, 4);
        s = q;
      }

      cout << "  " << mi->second->node_m << " [label=\"" << s << "\\n" << slen << "\"]" << endl;

      for (int i = 0; i < mi->second->out_m.size(); i++)
      {
        cout << "  " << mi->second->node_m << " -> " << mi->second->out_m[i]->node_m << endl;
      }
    }

    cout << "}" << endl;
  }

  void compressPaths()
  {
    int deletednodes = 0;
    vector<MerVertex_t*> changed;

    MerTable_t::iterator mi = mers_m.begin();
    while (mi != mers_m.end())
    {
      if (mi->second->dead) 
      {
        MerTable_t::iterator n = mi;
        mi++;

        delete n->second;
        mers_m.erase(n);
      }
      else
      {
        MerVertex_t * v = mi->second;
        bool change = false;

        while(v->compressable())
        {
          if (v->out_m[0]->in_m[0] != v)
          {
            string vs(v->str());
            string bs(v->out_m[0]->str());
            string is(v->out_m[0]->in_m[0]->str());

            cerr << "out[0] != in[0]" << endl;
            cerr << "vs: " << vs << endl;
            cerr << "bs: " << bs << endl;
            cerr << "is: " << is << endl;
            exit(1);
          }

          v->collapse();
          deletednodes++;
          change = true;
        }

        if (change)
        {
          changed.push_back(v);

          MerTable_t::iterator n = mi;
          mi++;

          mers_m.erase(n);
        }
        else
        {
          mi++;
        }
      }
    }

    mi = mers_m.begin();
    while (mi != mers_m.end())
    {
      if (mi->second->dead) 
      {
        MerTable_t::iterator n = mi;
        mi++;

        delete n->second;
        mers_m.erase(n);
      }
      else
      {
        mi++;
      }
    }

    for (int i = 0; i < changed.size(); i++)
    {
      if (!changed[i]->dead)
      {
        mers_m.insert(make_pair(changed[i]->mer_m, changed[i]));
      }
    }
  }
};

void ComputeComplexity(const string & tag, const string & seq)
{
  Mer_t  fwd_mer;
  int  i, j, n;

  InitMer(fwd_mer);

  n = seq . length ();
  if  (n < Kmer_Len) { return; }

  deBrujinGraph_t graph(tag);
  EventTime_t timeall;
  EventTime_t timegraph;

  // Initialize the first k-1 characters
  for  (i = 0;  i < Kmer_Len - 1;  i ++)
  {
    //cerr << seq[i];
    Forward_Add_Ch (fwd_mer, seq [i]);
  }

  // Now handle all of the kmers in the genome
  for (;i < n; i++)
  {
    //cerr << seq[i] << endl;
    Forward_Add_Ch (fwd_mer, seq [i]);

    string s;
    MerToAscii(fwd_mer, s);
    //cerr << "m2a: " <<  s << endl;

    graph.addMer(fwd_mer);
  }

  cerr << graph.nodeCount() << " nodes.  " << timegraph.str(true, 8) << endl;


  EventTime_t timecompress;
  graph.compressPaths();
  cerr << graph.nodeCount() << " nodes compressed.  " << timecompress.str(true, 8) << endl;

  cerr << "Total time: " << timeall.str(true, 8) << endl;

  graph.print();

  return;
}




int main(int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version = "Version 1.0";
    string helptext = 
"Compute the complexity of a genome\n"
"\n"
" Usage: genome-complexity -f fasta\n"
"\n"
"   -f <fasta> fasta file to evaluate\n"
"   -k <len>   Length of mers to consider (default:30)\n"
"\n";

    string fastafile;

    tf = new AMOS_Foundation(version, helptext, "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("f=s", &fastafile);
    tf->getOptions()->addOptionResult("k=i", &Kmer_Len);
    tf->handleStandardOptions();

    if (fastafile.empty())
    {
      cerr << "You must specify a fasta file" << endl;
      exit(1);
    }

    cerr << "Processing sequences in " << fastafile << "..." << endl;

    FILE * fp = fopen(fastafile.c_str(), "r");

    if (!fp)
    {
      cerr << "Couldn't open " << fastafile << endl;
      exit(1);
    }

    string s, tag;

    while  (Fasta_Read (fp, s, tag))
    {
      ComputeComplexity(tag, s);
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }
  catch (const ExitProgramNormally & e)
  {
    retval = 0;
  }
  catch (const amosException & e)
  {
    cerr << e << endl;
    retval = 100;
  }

  try
  {
    if (tf) delete tf;
  }
  catch (const amosException & e)
  {
    cerr << "amosException while deleting tf: " << e << endl;
    retval = 105;
  }

  return retval;
}
