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

// Store the (variable length) mer as a compressed bit vector
typedef vector<bool> Mer_t;

// Default length, can override at runtime
int Kmer_Len = 30;

// Convert from bits to ascii via an array lookup
const char * bintoascii = "ACGT";

int OPT_DisplaySeq = 0;
int OPT_SeqToDisplay = 8;

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

// Initialize an empty mer
void InitMer(Mer_t & mer)
{
  mer.clear();
  mer.resize(Kmer_Len*2);
}

// Convert a Mer to a string
void MerToAscii(Mer_t mer, string & s)
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

// Add a DNA character to a (fixed-length) mer: shift the bits to the left and append on the end
void  Forward_Add_Ch(Mer_t & mer, char ch)
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

// Extend mer with the 'extra' bases in extend
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

  Mer_t suff(suffix(extend, extend.size()/2 - (Kmer_Len-2)));
  mer.insert(mer.end(), suff.begin(), suff.end());

  if (DEBUG)
  {
    string z;
    MerToAscii(mer, z);

    cerr << ": " << z << endl;
  }
}


// Class for storing a mer and its neighbors
class MerVertex_t
{
  static int NODECOUNT; // give all the nodes a unique id
public:
  MerVertex_t(Mer_t s) : mer_m(s), node_m(NODECOUNT++), dead(false) {}
  Mer_t mer_m;
  int   node_m;
  bool  dead;

  vector<MerVertex_t *> out_m;
  vector<MerVertex_t *> in_m;

  int len() const
  {
    return mer_m.size()/2;
  }

  // Can this node be compressed with its immediate neighbor
  bool isCompressable()
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

  // Actually collapse this node with its immediate neighbor
  void collapse()
  {
    MerVertex_t * buddy = out_m[0];
    //cerr << "Collapse " << node_m << " and " << buddy->node_m << endl;
    out_m = buddy->out_m;
    buddy->dead = true;
    extendMer(mer_m, buddy->mer_m);

    // Update the links so that the buddy's neighbors now point at me
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

  // Return the sequence stored in the node
  string str()
  {
    string r;
    MerToAscii(mer_m, r);

    return r;
  }
};

int MerVertex_t::NODECOUNT=0;

// Ugliness for using hash_map
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

// Class for storing the entire graph: Nodes are stored in a giant MerTable_T
class deBrujinGraph_t
{
public:

  deBrujinGraph_t(const string & tag): tag_m(tag) {}

  string tag_m;
  MerTable_t mers_m;

  // Get (or create) a vertex
  MerVertex_t * getVertex(const Mer_t & mer)
  {
    MerTable_t::iterator m = mers_m.find(mer);
    if (m == mers_m.end())
    {
      m = mers_m.insert(make_pair(mer, new MerVertex_t(mer))).first;
    }

    return m->second;
  }

  // Add a k-mer to the graph by adding the k-1 prefix and suffix strings
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

  // Return number of nodes in the graph
  int nodeCount()
  {
    return mers_m.size();
  }

  // Print graph in DOT format
  void print()
  {
    cout << "digraph G {" << endl;

    // Renumber nodes and print
    int count = 1;
    MerTable_t::iterator mi;
    for (mi = mers_m.begin(); mi != mers_m.end(); mi++)
    {
      assert (!mi->second->dead);
      mi->second->node_m = count;
      count++;

      if (OPT_DisplaySeq)
      {
        string s(mi->second->str());
        int slen = s.length();

        if (slen > OPT_SeqToDisplay)
        {
          string q = s.substr(0,4);
          q += "...";
          q += s.substr(s.length()-4, 4);
          s = q;
        }

        cout << "  " << mi->second->node_m << " [label=\"" << s << "\\n" << slen << "\"]" << endl;
      }
      else
      {
        cout << "  " << mi->second->node_m << " [label=\"" << mi->second->len() << "\"]" << endl;
      }
    }

    // Print Edges
    for (mi = mers_m.begin(); mi != mers_m.end(); mi++)
    {
      for (int i = 0; i < mi->second->out_m.size(); i++)
      {
        cout << "  " << mi->second->node_m << " -> " << mi->second->out_m[i]->node_m << endl;
      }
    }

    cout << "}" << endl;
  }

  // Compress the graph
  void compressPaths()
  {
    int deletednodes = 0;
    vector<MerVertex_t*> changed;

    // Foreach mer in graph
    MerTable_t::iterator mi = mers_m.begin();
    while (mi != mers_m.end())
    {
      if (mi->second->dead) 
      {
        // Prune nodes that were collapsed away
        MerTable_t::iterator n = mi;
        mi++;

        delete n->second;
        mers_m.erase(n);
      }
      else
      {
        // Try to merge this node with its neighbors
        MerVertex_t * v = mi->second;
        bool change = false;

        while(v->isCompressable())
        {
          v->collapse();
          deletednodes++;
          change = true;
        }

        // If the current node was changed, store it separately so we don't try 
        // to update it a second time
        if (change)
        {
          changed.push_back(v);

          MerTable_t::iterator n = mi;
          mi++;

          mers_m.erase(n);
        }
        else
        {
          // Nothing to do
          mi++;
        }
      }
    }

    // Prune out all of the dead nodes
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

    // Add back the nodes that changed (and weren't subsequently marked dead)
    for (int i = 0; i < changed.size(); i++)
    {
      if (!changed[i]->dead)
      {
        mers_m.insert(make_pair(changed[i]->mer_m, changed[i]));
      }
    }
  }
};

// For now, just create the deBrujin Graph, and print it out
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
    Forward_Add_Ch (fwd_mer, seq [i]);
  }

  // Now handle all of the kmers in the genome
  for (;i < n; i++)
  {
    Forward_Add_Ch (fwd_mer, seq [i]);
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
  int isCircular = false;

  try
  {
    // Get standard options to work (--help, --version)
    string version = "Version 1.0";
    string helptext = 
"Compute the complexity of a genome\n"
"\n"
" Usage: genome-complexity -f fasta\n"
"\n"
"   -f <fasta> fasta file to evaluate\n"
"   -k <len>   Length of mers to consider (default:30)\n"
"   -C         Interpret genome as circular\n"
"   -d         Display the sequences for edge node\n"
"   -D <len>   Only show first and last 4 bp for sequences longer than <len>\n"
"\n";

    string fastafile;

    tf = new AMOS_Foundation(version, helptext, "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("f=s", &fastafile);
    tf->getOptions()->addOptionResult("k=i", &Kmer_Len);
    tf->getOptions()->addOptionResult("C",   &isCircular);
    tf->getOptions()->addOptionResult("d",   &OPT_DisplaySeq);
    tf->getOptions()->addOptionResult("D=i", &OPT_SeqToDisplay);
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

    // Use Art's fasta reader
    while  (Fasta_Read (fp, s, tag))
    {
      if (isCircular)
      {
        // Add the first kmer_len-1 characters to end to create circularity
        s += s.substr(0, Kmer_Len-1);

      }

      // Compute the complexity for each sequence in the fasta file
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

  // Standard stuff to try to exit gracefully
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
