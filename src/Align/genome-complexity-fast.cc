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
typedef long long unsigned Mer_t;
static Mer_t Forward_Mask;



// Default length, can override at runtime
int Kmer_Len = 30;
int Seed_Len = 30;

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
  mer = 0;
}

//  Add  ch  to  mer  on the right, sliding one character
//  off the left end of  mer .
static void  Forward_Add_Ch(Mer_t & mer, char ch)
{
  mer &= Forward_Mask;
  mer <<= 2;
  mer |= Char_To_Binary (ch);
}


// Class for storing a mer and its neighbors
class MerVertex_t
{
  static int NODECOUNT; // give all the nodes a unique id
public:
  MerVertex_t(int startpos, int endpos) 
  : startpos_m(startpos), endpos_m(endpos), node_m(NODECOUNT++), dead(false) {}
  int startpos_m;
  int endpos_m;
  int   node_m;
  bool  dead;

  vector<MerVertex_t *> out_m;
  vector<MerVertex_t *> in_m;

  int len() const
  {
    return endpos_m-startpos_m+1;
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
    endpos_m += buddy->len() - (Kmer_Len-2);

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
  string str(const string & seq)
  {
    string r = seq.substr(startpos_m, endpos_m-startpos_m+1);
    return r;
  }
};

int MerVertex_t::NODECOUNT=0;

typedef HASHMAP::hash_multimap<Mer_t, MerVertex_t *, hash<unsigned long> > MerTable_t;
typedef vector<MerVertex_t *> NodeTable_t;

// Class for storing the entire graph: Nodes are stored in a giant MerTable_T
class deBrujinGraph_t
{
public:

  deBrujinGraph_t(const string & tag,
                  const string & seq)
    : tag_m(tag), seq_m(seq) {}

  const string & tag_m;
  const string & seq_m;

  MerTable_t mers_m;
  NodeTable_t nodes_m;

  bool substrEqual(int s1, int e1, int s2, int e2)
  {
    if (e1-s1 != e2-s2) { return false; }

    int l = e1-s1+1;
    for (int i = Seed_Len; i < l; i++)
    {
      if (seq_m[s1+i] != seq_m[s2+i])
      {
        return false;
      }
    }

    return true;
  }

  // Get (or create) a vertex
  MerVertex_t * getVertex(const Mer_t & mer, int startpos, int endpos)
  {
    MerTable_t::iterator m = mers_m.find(mer);

    while ((m != mers_m.end()) &&
           (m->first == mer))
    {
      if (substrEqual(startpos, endpos, 
                      m->second->startpos_m, m->second->endpos_m))
      {
        return m->second;
      }

      m++;
    }

    m = mers_m.insert(make_pair(mer, new MerVertex_t(startpos, endpos)));

    return m->second;
  }

  void construct()
  {
    Mer_t seed;
    InitMer(seed);

    // Initialize the first s characters
    for  (int i = 0;  i < Seed_Len;  i ++)
    {
      Forward_Add_Ch(seed, seq_m[i]);
    }

    // prefix is the first (k-1) bp
    MerVertex_t * p = getVertex(seed, 0, Kmer_Len-2);
    int n = seq_m.length();

    for (int i = 1; i+Kmer_Len-2 < n; i++)
    {
      Forward_Add_Ch(seed, seq_m[i+Seed_Len-1]);

      // suffix is the next (k-1) bp
      MerVertex_t * s = getVertex(seed, i, i+Kmer_Len-2);

      p->out_m.push_back(s);
      s->in_m.push_back(p);

      p = s;
    }
  }

  // Return number of nodes in the graph
  int nodeCount()
  {
    if (nodes_m.size())
    {
      return nodes_m.size();
    }

    return mers_m.size();
  }

  // Print graph in DOT format
  void print()
  {
    cout << "digraph G {" << endl;

    // Renumber nodes and print
    int count = 1;

    for (int i = 0; i < nodes_m.size(); i++)
    {
      assert(!nodes_m[i]->dead);
      nodes_m[i]->node_m = count;
      count++;

      if (OPT_DisplaySeq)
      {
        string s(nodes_m[i]->str(seq_m));
        int slen = s.length();
        if (slen > OPT_SeqToDisplay)
        {
          string q = s.substr(0,4);
          q += "...";
          q += s.substr(s.length()-4, 4);
          s = q;
        }

        cout << "  " << nodes_m[i]->node_m << " [label=\"" << s << "\\n" << slen << "\"]" << endl;
      }
      else
      {
        cout << "  " << nodes_m[i]->node_m << " [label=\"" << nodes_m[i]->len() << "\"]" << endl;
      }
    }

    // Print Edges
    for (int i = 0; i < nodes_m.size(); i++)
    {
      for (int j = 0; j < nodes_m[i]->out_m.size(); j++)
      {
        cout << "  " << nodes_m[i]->node_m << " -> " << nodes_m[i]->out_m[j]->node_m << endl;
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
    for (mi = mers_m.begin(); mi != mers_m.end(); mi++)
    {
      if (mi->second->dead) 
      {
        delete mi->second;
      }
      else
      {
        nodes_m.push_back(mi->second);
      }
    }

    mers_m.clear();

    // Add back the nodes that changed (and weren't subsequently marked dead)
    for (int i = 0; i < changed.size(); i++)
    {
      if (!changed[i]->dead)
      {
        nodes_m.push_back(changed[i]);
      }
    }
  }
};

// For now, just create the deBrujin Graph, and print it out
void ComputeComplexity(const string & tag, const string & seq)
{
  int  i, j, n;

  n = seq . length ();
  if  (n < Kmer_Len) { return; }

  deBrujinGraph_t graph(tag, seq);
  EventTime_t timeall;
  EventTime_t timegraph;

  graph.construct();
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

    if (Seed_Len > Kmer_Len-1)
    {
      Seed_Len = Kmer_Len-1;
    }

    Forward_Mask = ((long long unsigned) 1 << (2 * Seed_Len - 2)) - 1;


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
