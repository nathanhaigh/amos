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

class MerVertex_t;
typedef HASHMAP::hash_multimap<Mer_t, MerVertex_t *, hash<unsigned long> > MerTable_t;
typedef list<MerVertex_t *> NodeTable_t;



// Default length, can override at runtime
int Kmer_Len = 30;
int Seed_Len = 30;

// Convert from bits to ascii via an array lookup
const char * bintoascii = "ACGT";

int OPT_DisplaySeq = 0;
int OPT_DisplayStarts = 0;
int OPT_SeqToDisplay = 8;
int OPT_DisplayStats = 0;
int OPT_JustCompress = 0;

bool SortLens(const int & a, const int & b)
{
  return b < a;
}

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
  : startpos_m(startpos), endpos_m(endpos), node_m(NODECOUNT++), dead(false) {
    starts_m.push_back(startpos);}
  int startpos_m;
  int endpos_m;
  int   node_m;
  bool  dead;

  vector<MerVertex_t *> successor_m;
  vector<MerVertex_t *> predecessor_m;
  vector<int> starts_m;

  void addStartPos(int pos)
  {
    starts_m.push_back(pos);
  }

  int len() const
  {
    return endpos_m-startpos_m+1;
  }

  // Can this node be compressed with its immediate neighbor
  bool isCompressable()
  {
    if (successor_m.empty() || (successor_m[0] == this))
    {
      return false;
    }

    // Make sure all of the out-links point to the same node v
    int ol = successor_m.size();
    for (int i = 1; i < ol; i++)
    {
      if (successor_m[i] != successor_m[0])
      {
        return false;
      }
    }

    // Make sure all of v's in-links are from me
    int il = successor_m[0]->predecessor_m.size();

    if (ol != il)
    {
      return false;
    }

    for (int i = 0; i < il; i++)
    {
      if (successor_m[0]->predecessor_m[i] != this)
      {
        return false;
      }
    }

    return true;
  }

  // Actually collapse this node with its immediate neighbor
  void collapse()
  {
    MerVertex_t * buddy = successor_m[0];
    //cerr << "Collapse " << node_m << " and " << buddy->node_m << endl;
    successor_m = buddy->successor_m;
    buddy->dead = true;
    endpos_m += buddy->len() - (Kmer_Len-2);

    // Update the links so that the buddy's neighbors now point at me
    for (int i = 0; i < successor_m.size(); i++)
    {
      MerVertex_t * bo = successor_m[i];

      for (int j = 0; j < bo->predecessor_m.size(); j++)
      {
        if (bo->predecessor_m[j] == buddy)
        {
          bo->predecessor_m[j] = this;
          break;
        }
      }
    }

  }

  bool isTreeLeaf()
  {
    int il = predecessor_m.size();
    int ol = successor_m.size();

    if ((il != ol) || (il == 0)) { return false; }

    // see if all of my in links are from the same node
    for (int i = 1; i < il; i++)
    {
      if (predecessor_m[i] != predecessor_m[0]) { return false; }
    }

    // see if all of my out links are to the same node
    for (int i = 0; i < ol; i++)
    {
      if (successor_m[i] != predecessor_m[0]) { return false; }
    }

    // now see if parent has just 1 extra in- and out- link
    MerVertex_t * parent = successor_m[0];

    if (parent->predecessor_m.size()  != ol+1) { return false; }
    if (parent->successor_m.size()    != il+1) { return false; }

    return true;
  }

  bool isSelfLeaf()
  {
    int ol = successor_m.size();
    int il = predecessor_m.size();

    if (ol != il) { return false; }

    int self = 0;
    for (int i = 0; i < il; i++)
    {
      if (successor_m[i] == this) { self++; }
    }

    if ((self > 0) && (self == il-1))
    {
      return true;
    }

    return false;
  }

  void foldSelf()
  {
    int numcycles = successor_m.size()-1;

    int mylen = len();
    endpos_m += numcycles * (mylen - (Kmer_Len-2));

    MerVertex_t * other = NULL;
    int ol = successor_m.size();
    int il = predecessor_m.size();

    // keep just the non-self successor
    for (int i = 0; i < ol; i++)
    {
      if (successor_m[i] != this) 
      { other = successor_m[i]; break; }
    }

    assert(other != NULL);

    successor_m.clear();
    successor_m.push_back(other);

    // keep just the non-self predecessor
    for (int i = 0; i < il; i++)
    {
      if (predecessor_m[i] != this)
      { other = predecessor_m[i]; break; }
    }

    assert(other != NULL);

    predecessor_m.clear();
    predecessor_m.push_back(other);
  }

  // backward: multiple predecessors (predecessor_m), one successor(successor_m)
  bool isBackwardDecisionNode()
  {
    int il = predecessor_m.size();
    int ol = successor_m.size();
    if ((ol != il) || (il < 2)) { return false; }

    // check that all of the outlinks go to the same node (successor_m[0])
    for (int i = 1; i < ol; i++)
    {
      if (successor_m[i] != successor_m[0]) { return false; }
    }

    // now check that there are > 1 predecessors
    for (int i = 1; i < il; i++)
    {
      if (predecessor_m[i] != predecessor_m[0])
      {
        return true;
      }
    }

    // nope, all of the in links are from same node
    return false;
  }

  // forward: one predecessor (predecessor_m), multiple successors (successor_m)
  bool isForwardDecisionNode()
  {
    int il = predecessor_m.size();
    int ol = successor_m.size();
    if ((ol != il) || (il < 2)) { return false; }

    // check that all of the inlinks are from the same node (predecessor_m[0])
    for (int i = 1; i < il; i++)
    {
      if (predecessor_m[i] != predecessor_m[0]) { return false; }
    }

    // now check that there are > 1 successors
    for (int i = 1; i < ol; i++)
    {
      if (successor_m[i] != successor_m[0])
      {
        return true;
      }
    }

    return false;
  }

  // There are multiple predecessors (predecessor_m), split this node for each
  NodeTable_t splitBackwards()
  {
    NodeTable_t retval;

    // count how many different predecessors I have
    int il = predecessor_m.size();
    int numpre = 1; //predecessor_m[0];
    int pre[il];
    pre[0] = 0;

    for (int i = 1; i < il; i++)
    {
      bool seen = false;
      for (int j = 0; j < i; j++)
      {
        if (predecessor_m[i] == predecessor_m[j])
        {
          seen = true;
          pre[i] = pre[j];
          break;
        }
      }

      if (!seen) 
      { 
        pre[i] = numpre; 
        numpre++; 
      }
    }

    if (numpre > 1)
    {
      // mark me dead, I'm replaced by the other nodes
      dead = true;

      // the sole successor node
      MerVertex_t * successor = successor_m[0];
      int successorlinksfixed = 0;

      for (int p = 0; p < numpre; p++)
      {
        MerVertex_t * nn = NULL;

        for (int i = 0; i < il; i++)
        {
          if (pre[i] == p)
          {
            if (nn == NULL)
            {
              nn = new MerVertex_t(startpos_m, endpos_m);
              retval.push_back(nn);

              // update the links inside predecessor_m[i]
              MerVertex_t * pred = predecessor_m[i];
              int bol = pred->successor_m.size();
              for (int b = 0; b < bol; b++)
              {
                if (pred->successor_m[b] == this)
                {
                  pred->successor_m[b] = nn;
                }
              }
            }

            nn->predecessor_m.push_back(predecessor_m[i]);
            nn->successor_m.push_back(successor);
          }
        }

        // update successors's out-links
        int nl = nn->successor_m.size();
        int succpred = successor->predecessor_m.size();
        for (int k = 0; k < nl; k++)
        {
          for (int j = 0; j < succpred; j++)
          {
            if (successor->predecessor_m[j] == this)
            {
              successor->predecessor_m[j] = nn;
              break;
            }
          }
        }
      }
    }

    return retval;
  }

  // There are multiple successors, split this node for each
  NodeTable_t splitForwards()
  {
    NodeTable_t retval;

    // count how many different successors I have
    int ol = successor_m.size();
    int numsuc = 1; //successor_m[0];
    int suc[ol];
    suc[0] = 0;

    for (int i = 1; i < ol; i++)
    {
      bool seen = false;
      for (int j = 0; j < i; j++)
      {
        if (successor_m[i] == successor_m[j])
        {
          seen = true;
          suc[i] = suc[j];
          break;
        }
      }

      if (!seen) 
      { 
        suc[i] = numsuc; 
        numsuc++; 
      }
    }

    if (numsuc > 1)
    {
      // mark me dead, I'm replaced by the other nodes
      dead = true;

      // the sole predecessor node
      MerVertex_t * pred = predecessor_m[0];
      int predsucc = pred->successor_m.size();

      for (int s = 0; s < numsuc; s++)
      {
        MerVertex_t * nn = NULL;

        for (int i = 0; i < ol; i++)
        {
          if (suc[i] == s)
          {
            if (nn == NULL)
            {
              nn = new MerVertex_t(startpos_m, endpos_m);
              retval.push_back(nn);

              // update the links inside successor_m[i]
              MerVertex_t * succ = successor_m[i];
              int bil = succ->predecessor_m.size();
              for (int b = 0; b < bil; b++)
              {
                if (succ->predecessor_m[b] == this)
                {
                  succ->predecessor_m[b] = nn;
                }
              }
            }

            nn->predecessor_m.push_back(pred);
            nn->successor_m.push_back(successor_m[i]);
          }
        }

        // update pred's out-links
        int nl = nn->predecessor_m.size();
        for (int k = 0; k < nl; k++)
        {
          for (int j = 0; j < predsucc; j++)
          {
            if (pred->successor_m[j] == this)
            {
              pred->successor_m[j] = nn;
              break;
            }
          }
        }
      }
    }

    return retval;
  }


  void foldIntoParent()
  {
    MerVertex_t * parent = predecessor_m[0];

    int numcycles = predecessor_m.size();

    int mylen = len();
    int plen  = parent->len();

    parent->endpos_m += numcycles * (mylen - (Kmer_Len-2) + plen - (Kmer_Len -2));

    dead = true;

    // Clear all of the parents out- and in- links to me (all but 1 each)

    int pol = parent->successor_m.size();
    int pil = parent->predecessor_m.size();

    MerVertex_t * pout = NULL;

    for (int i = 0; i < pol; i++)
    {
      if (parent->successor_m[i] != this) 
      { pout = parent->successor_m[i]; break; }
    }

    assert(pout != NULL);

    parent->successor_m.clear();
    parent->successor_m.push_back(pout);

    MerVertex_t * pin = NULL;

    for (int i = 0; i < pil; i++)
    {
      if (parent->predecessor_m[i] != this) 
      { pin = parent->predecessor_m[i]; break; }
    }

    assert(pin != NULL);

    parent->predecessor_m.clear();
    parent->predecessor_m.push_back(pin);
  }

  // Return the sequence stored in the node
  string str(const string & seq)
  {
    string r = seq.substr(startpos_m, endpos_m-startpos_m+1);
    return r;
  }
  
  bool isNonDecisionNode()
  {
    int il = predecessor_m.size();
    int ol = successor_m.size();
    
    if (il != ol) { return false; }

    // check all in links are same
    for (int i = 1; i < il; i++)
    {
      if (predecessor_m[i] != predecessor_m[0]) { return false; }
    }

    for (int i = 1; i < ol; i++)
    {
      if (successor_m[i] != successor_m[0]) { return false; }
    }

    return true;
  }
};

int MerVertex_t::NODECOUNT=0;


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
        m->second->addStartPos(startpos);
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

      p->successor_m.push_back(s);
      s->predecessor_m.push_back(p);

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


  void printStats()
  {
    int numNodes = nodes_m.size();
    int numEdges = 0;

    long totalspan = 0;

    vector<int> lengths;

    long max = 0;

    for (NodeTable_t::iterator ni = nodes_m.begin();
         ni != nodes_m.end();
         ni++)
    {
      numEdges += (*ni)->successor_m.size();

      long l = (*ni)->len();

      totalspan += l;
      lengths.push_back(l);

      if (l > max) { max = l; }
    }

    sort(lengths.begin(), lengths.end(), SortLens);

    long target = totalspan/2;
    long sum = 0;
    int n50cnt = 0;

    for (int i = 0; i < lengths.size(); i++)
    {
      sum += lengths[i];

      if (sum >= target)
      {
        n50cnt = i;
        break;
      }
    }

    cerr << "n="          << numNodes 
         << " m="         << numEdges 
         << " totalspan=" << totalspan
         << " max="       << max
         << " mean="      << ((double) totalspan)/ ((double) numNodes)
         << " n50="       << lengths[n50cnt]
         << " n50cnt="    << n50cnt
         << endl;
  }

  // Print graph in DOT format
  void print()
  {
    cout << "digraph G {" << endl;

    // Renumber nodes and print
    int count = 1;

    for (NodeTable_t::iterator ni = nodes_m.begin();
         ni != nodes_m.end();
         ni++)
    {
      assert(!(*ni)->dead);
      (*ni)->node_m = count;
      count++;

      if (OPT_DisplayStarts)
      {
        cout << "  " << (*ni)->node_m 
             << " [label=\"";

        sort((*ni)->starts_m.begin(), (*ni)->starts_m.end());

        for (int j = 0; j < (*ni)->starts_m.size(); j++)
        {
          if (j > 0) { cout << ","; }
          cout << (*ni)->starts_m[j]+1; // use 1-based coordinates
        }

        cout << ":" << (*ni)->len() << "\"]" << endl;
      }
      else if (OPT_DisplaySeq)
      {
        string s((*ni)->str(seq_m));
        int slen = s.length();
        if (slen > OPT_SeqToDisplay)
        {
          string q = s.substr(0,4);
          q += "...";
          q += s.substr(s.length()-4, 4);
          s = q;
        }

        cout << "  " << (*ni)->node_m << " [label=\"" << s << "\\n" << slen << "\"]" << endl;
      }
      else
      {
        cout << "  " << (*ni)->node_m << " [label=\"" << (*ni)->len() << "\"]" << endl;
      }
    }

    // Print Edges
    for (NodeTable_t::iterator ni = nodes_m.begin(); ni != nodes_m.end(); ni++)
    {
      for (int j = 0; j < (*ni)->successor_m.size(); j++)
      {
        cout << "  " << (*ni)->node_m << " -> " << (*ni)->successor_m[j]->node_m << endl;
      }
    }

    cout << "}" << endl;
  }

  // convert the hash table to a list of mers
  void convertToNodes()
  {
    for (MerTable_t::iterator mi = mers_m.begin();
         mi != mers_m.end();
         mi++)
    {
      if (!mi->second->dead)
      {
        nodes_m.push_back(mi->second);
      }
      else
      {
        delete mi->second;
      }
    }

    mers_m.clear();
  }

  // Compress the graph
  void compressPaths()
  {
    vector<MerVertex_t*> changed;

    // Foreach mer in graph
    NodeTable_t::iterator mi = nodes_m.begin();
    while (mi != nodes_m.end())
    {
      if ((*mi)->dead) 
      {
        // Prune nodes that were collapsed away
        NodeTable_t::iterator t = mi;
        mi++;

        delete *t;
        nodes_m.erase(t);
      }
      else
      {
        // Try to merge this node with its neighbors
        MerVertex_t * v = *mi;

        while(v->isCompressable())
        {
          v->collapse();
        }

        mi++;
      }
    }

    cleanNodeArray();
  }

  void cleanNodeArray()
  {
    NodeTable_t::iterator ni = nodes_m.begin();
    while (ni != nodes_m.end())
    {
      if ((*ni)->dead)
      {
        NodeTable_t::iterator t = ni;
        ni++;

        delete *t;
        nodes_m.erase(t);
      }
      else
      {
        ni++;
      }
    }
  }

  void compressTrees()
  {
    int nodecount = nodes_m.size();
    int totalfolds = 0;
    int selffolds = 0;

    bool madefold = true;

    while (madefold)
    {
      cerr << ".";
      madefold = false;
      for (NodeTable_t::iterator ni = nodes_m.begin(); 
           ni != nodes_m.end(); 
           ni++)
      {
        if ((*ni)->dead) { continue; }

        if ((*ni)->isSelfLeaf())
        {
          (*ni)->foldSelf();
          madefold = true;
          totalfolds++;
          selffolds++;
        }

        if ((*ni)->isTreeLeaf())
        {
          (*ni)->foldIntoParent();
          madefold=true;
          totalfolds++;
        }
      }

      compressPaths();
    }

    cerr << "Made " << totalfolds << " total folds, " << selffolds << " selffolds" << endl;
  }

  void splitBackwardDecisionNodes()
  {
    int numsplit = 0;
    bool split = true;
    while (split)
    {
      split = false;
      for (NodeTable_t::iterator ni = nodes_m.begin();
           ni != nodes_m.end();
           ni++)
      {
        if ((*ni)->dead) { continue; }
        if ((*ni)->isBackwardDecisionNode())
        {
          split = true;
          numsplit++;
          NodeTable_t extranodes = (*ni)->splitBackwards();
          for (NodeTable_t::iterator mi = extranodes.begin();
               mi != extranodes.end();
               mi++)
          {
            nodes_m.push_back(*mi);
          }
        }
      }
    }

    cleanNodeArray();

    cerr << "splitBackwardDecisionNodes: " << numsplit << endl;
  }

  void splitForwardDecisionNodes()
  {
    int numsplit = 0;
    bool split = true;
    while (split)
    {
      split = false;
      for (NodeTable_t::iterator ni = nodes_m.begin();
           ni != nodes_m.end();
           ni++)
      {
        if ((*ni)->dead) { continue; }
        if ((*ni)->isForwardDecisionNode())
        {
          split = true;
          numsplit++;
          NodeTable_t extranodes = (*ni)->splitForwards();
          for (NodeTable_t::iterator mi = extranodes.begin();
               mi != extranodes.end();
               mi++)
          {
            nodes_m.push_back(*mi);
          }
        }
      }
    }

    cleanNodeArray();
    cerr << "splitForwardDecisionNodes: " << numsplit << endl;
  }

  void convertNonDecisionToEdges()
  { 
    int numnondecisionnodes = 0;

    for (NodeTable_t::iterator ni = nodes_m.begin();
         ni != nodes_m.end();
         ni++)
    {
      if ((*ni)->isNonDecisionNode())
      {
        numnondecisionnodes++;
      }
    }

    int final = nodes_m.size() - numnondecisionnodes;
    cerr << "--Final: n=" << final << endl;
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
  graph.convertToNodes();
  graph.printStats();

  EventTime_t timecompress;
  graph.compressPaths();
  cerr << graph.nodeCount() << " nodes compressed.  " << timecompress.str(true, 8) << endl;
  graph.printStats();

  // Just do the compression
  if (OPT_JustCompress)
  {
    if (!OPT_DisplayStats)
    {
      graph.print();
    }

    return;
  }


  EventTime_t timetree;
  graph.compressTrees();
  cerr << graph.nodeCount() << " nodes treecompressed. " << timetree.str(true, 8) << endl;
  graph.printStats();

  EventTime_t timebackwards;
  graph.splitBackwardDecisionNodes();
  cerr << graph.nodeCount() << " nodes splitBackwardDecisionNodes.  " << timebackwards.str(true, 8) << endl;

  EventTime_t timeforward;
  graph.splitForwardDecisionNodes();
  cerr << graph.nodeCount() << " nodes splitForwardDecisionNodes.  " << timeforward.str(true, 8) << endl;

  EventTime_t timecompress3;
  graph.compressPaths();
  cerr << graph.nodeCount() << " nodes compressed3.  " << timecompress3.str(true, 8) << endl;


  cerr << "--PostSplit:";
  graph.printStats();

  EventTime_t timeconverttoedges;
  graph.convertNonDecisionToEdges();

  cerr << "Total time: " << timeall.str(true, 8) << endl;
  
  if (!OPT_DisplayStats)
  {
    graph.print();
  }

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
    string version = "Version 1.1";
    string helptext = 
"Compute the complexity of a genome\n"
"\n"
" Usage: genome-complexity -f fasta\n"
"\n"
"   -f <fasta> fasta file to evaluate\n"
"   -k <len>   Length of mers to consider (default:30)\n"
"   -C         Interpret genome as circular\n"
"   -p         Display the start positions and length of each sequence\n"
"   -d         Display the sequences for edge node\n"
"   -D <len>   Only show first and last 4 bp for sequences longer than <len>\n"
"   -S         Just Compute graph statistics\n";
"\n";

    string fastafile;

    tf = new AMOS_Foundation(version, helptext, "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("f=s", &fastafile);
    tf->getOptions()->addOptionResult("k=i", &Kmer_Len);
    tf->getOptions()->addOptionResult("C",   &isCircular);
    tf->getOptions()->addOptionResult("d",   &OPT_DisplaySeq);
    tf->getOptions()->addOptionResult("p",   &OPT_DisplayStarts);
    tf->getOptions()->addOptionResult("D=i", &OPT_SeqToDisplay);
    tf->getOptions()->addOptionResult("c",   &OPT_JustCompress);
    tf->getOptions()->addOptionResult("S",   &OPT_DisplayStats);
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
      cout << tag << endl;

      if (isCircular)
      {
        // Add the first kmer_len-1 characters to end to create circularity
        s += s.substr(0, Kmer_Len-1);
      }

      // Compute the complexity for each sequence in the fasta file
      ComputeComplexity(tag, s);

      cout << endl << endl;
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
