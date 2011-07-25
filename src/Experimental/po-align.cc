// Michael Schatz
// 
// Convert layouts to multiple-alignments (contigs)
// using partial order graphs

#include "foundation_AMOS.hh"
#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <deque>
#include <map>

using namespace std;
using namespace AMOS;

#ifdef EDIT
int MATCH = 0;
int INDEL = 1;
int MISMATCH = 1;
int BASEINDEL = INDEL;

#define SCORECMP(a, b) (a < b)

#else
int MATCH = 4;
int INDEL = -1;
int MISMATCH = -2;

#define SCORECMP(a, b) (a > b)

#ifdef LOCAL
int BASEINDEL = 0;
#else
int BASEINDEL = INDEL;
#endif

#endif

int basecmp(char s, char t)
{
  if (s == t) { return MATCH; }
  return MISMATCH;
}





class PONode
{
public:
  static int s_num;
  PONode(char base): m_base(base), m_state(0), m_num(s_num++), m_offset(0), m_nextvisit(-1) {}

  void addSeq(int id)       { m_seqs.push_back(id); }
  void addRight(PONode * n) { m_right.push_back(n); }
  void addLeft(PONode * n)  { m_left.push_back(n);  }

  void trimLeft(PONode *n)
  {
    vector<PONode *>::iterator vi;
    for (vi = m_left.begin(); vi != m_left.end(); vi++)
    {
      if (*vi == n)
      {
        m_left.erase(vi);
        return;
      }
    }
  }

  ostream & label(ostream & os)
  {
    os << "\"" << m_base << " (" << m_num << ")" << "[" << m_offset << "]" << "\\n";

    vector<int>::iterator i;
    for (i = m_seqs.begin(); i != m_seqs.end(); i++)
    {
      os << *i << "\\n";
    }

    os << "\"";
    return os;
  }

  bool hasSeq(int seqid)
  {
    vector<int>::const_iterator si;
    for (si = m_seqs.begin(); si != m_seqs.end(); si++)
    {
      if (seqid == *si)
      {
        return true;
      }
    }

    return false;
  }

  char m_base;

  int m_state;
  int m_num;
  int m_offset;
  int m_nextvisit;

  vector <int> m_seqs;
  vector <PONode *> m_left;
  vector <PONode *> m_right;

  vector <char>     m_aligndir;
  vector <int>      m_alignscore;
  vector <PONode *> m_alignbuddy;
};

int PONode::s_num(1);


class POGraph
{
public:
  POGraph() : m_aligncount(0), m_reads(0)
  {
    m_startnode = new PONode('*');
    m_nodes.push_back(m_startnode);
  }

  int m_aligncount;
  int m_reads;
  PONode * m_startnode;

  map<int, PONode *> m_readstarts;
  map<int, ID_t> m_readcount2iid;
  map<int, int> m_readtrim;

  vector<PONode *> m_nodes;

  void assignOffset()
  {
    deque<PONode *> fringe;
    vector<PONode *>::iterator pi;

    for (pi = m_nodes.begin(); pi != m_nodes.end(); pi++)
    {
      (*pi)->m_offset = -1;
    }

    m_startnode->m_offset = -1;
    fringe.push_back(m_startnode);

    // Assign a distance to every node as the max distance from start
    while (!fringe.empty())
    {
      PONode * n = fringe.front();
      fringe.pop_front();

      int d = n->m_offset+1;
      //n->label(cerr) << endl;

      for (pi = n->m_right.begin(); pi != n->m_right.end(); pi++)
      {
        if (d > (*pi)->m_offset)
        {
          (*pi)->m_offset = d;
          fringe.push_back(*pi);
        }
      }
    }
  }


  void dumpMSA()
  {
    cerr << "DumpMSA" << endl;

    assignOffset();

    // Print the dotted msa for each read
    for (int readcount = 0; readcount < m_reads; readcount++)
    {
      cerr << ">" << readcount << "\t";
      int offset = getStartOffset(readcount);
      cerr << offset << "\t";
      for (int j = 0; j < offset; j++) { cerr << " "; }
      cerr << '\'';

      string aligned = getAlignedString(readcount);
      cerr << aligned << endl;
    }
  }

  int getStartOffset(int readcount)
  {
    PONode * n = m_readstarts[readcount];

    int trim = m_readtrim[readcount];
    while (trim)
    {
      n = n->m_right[0];
      trim--;
    }

    return n->m_offset - trim;
  }

  string getAlignedString(int readcount)
  {
    string aligned;
    PONode * cur = m_readstarts[readcount];

    int trim = m_readtrim[readcount];

    while (trim)
    {
      aligned.push_back(cur->m_base);
      cur = cur->m_right[0];
      trim--;
    }

    int np = cur->m_offset;
    while (cur)
    {
      while (np < cur->m_offset-1)
      {
        aligned.push_back('-');
        np++;
      }

      np=cur->m_offset;
      aligned.push_back(cur->m_base);

      // find the edge with the read
      int bestdist = -1;
      PONode * best = NULL;

      vector<PONode *>::iterator pi;

      for (pi = cur->m_right.begin(); pi != cur->m_right.end(); pi++)
      {
        if ((*pi)->hasSeq(readcount))
        {
          if (!best || (*pi)->m_offset < bestdist)
          {
            best = *pi;
            bestdist = (*pi)->m_offset;
          }
        }
      }

      cur = best;
    }

    return aligned;
  }

  vector<Pos_t> getGaps(int readcount)
  {
    vector<Pos_t> retval;
    string aligned = getAlignedString(readcount);

    for (int i = 0; i < aligned.length(); i++)
    {
      if (aligned[i] == '-')
      {
        retval.push_back(i - retval.size());
      }
    }

    return retval;
  }

  void dumpDot()
  {
    cerr << "DumpDot()" << endl;
    cout << "digraph G" << endl;
    cout << "{" << endl; 
    cout << "  rankdir=LR;" << endl;

    deque<PONode *> fringe;
    deque<PONode *>::iterator fi;

    vector<PONode *>::iterator pi;

    for (pi = m_nodes.begin(); pi != m_nodes.end(); pi++)
    {
      (*pi)->m_state = 0;
    }

    for (pi = m_startnode->m_right.begin(); pi != m_startnode->m_right.end(); pi++)
    {
      fringe.push_back(*pi);
      (*pi)->m_state = -1;
    }

    while (!fringe.empty())
    {
      PONode * n = fringe.front();
      fringe.pop_front();

      cout << "  "; n->label(cout) << endl;
      for (pi = n->m_right.begin(); pi != n->m_right.end(); pi++)
      {
        cout << "  "; n->label(cout) << " -> "; (*pi)->label(cout) << endl;

        if ((*pi)->m_state != -1)
        {
          (*pi)->m_state = -1;
          fringe.push_back(*pi);
        }
      }
    }

    cout << "}" << endl;
  }

  void seed(const string & str)
  {
    cerr << "Seed:\t" << str << endl;
    m_aligncount = 1;
    int len = str.length();

    PONode * last = m_startnode;

    for (int i = 0; i < len; i++)
    {
      PONode * n = new PONode(str[i]);
      m_nodes.push_back(n);
      n->addSeq(m_reads);
      n->addLeft(last);
      last->addRight(n);
      last = n;

      if (i == 0)
      {
        m_readstarts[m_reads] = n;
      }
    }

    m_reads++;
  }


  void printAlignRow(PONode * node)
  {
    fprintf(stderr, "%c[%02d]", node->m_base, node->m_num);

    for (int i = 0; i < node->m_aligndir.size(); i++)
    {
      fprintf(stderr, " %3d%c[%02d]", 
              node->m_alignscore[i], node->m_aligndir[i], node->m_num);
    }
    fprintf(stderr, "\n");
  }

  void align(const string & str, int id, int offset)
  {
    m_readcount2iid[m_reads] = id;

    if (m_reads == 0)
    {
      seed(str);
      return;
    }

    int len = str.length();
    assignOffset();

    int verbose = 0;
    //if (m_reads == 148) { verbose = 1; }

    cerr << endl << endl;
    cerr << "align:" << m_reads
         << " iid: " << id 
         << " off: " << offset
         << " "      << str.substr(0,20) << endl;

    if (verbose) { dumpMSA(); cerr << "        *     "; for (int i = 0; i < len; i++) { cerr << "   " << str[i] << "     "; } cerr << endl;}


    deque<PONode *> fringe;
    deque<PONode *>::iterator fi;
    vector<PONode *>::iterator pi, ppi;

    // Find nodes to start aligning at
    offset -= 10;
    if (offset < 0) { offset = 0; }

    for (pi = m_nodes.begin(); pi != m_nodes.end(); pi++)
    {
      if ((*pi)->m_offset == offset-1)
      {
        (*pi)->m_aligndir.resize(len+1);
        (*pi)->m_alignscore.resize(len+1);
        (*pi)->m_alignbuddy.resize(len+1);

        for (int i = 0; i <= len; i++)
        {
          (*pi)->m_aligndir[i] = '<';
          (*pi)->m_alignscore[i] = i*BASEINDEL;
          (*pi)->m_alignbuddy[i] = *pi;
        }

        (*pi)->m_aligndir[0] = '*';
        (*pi)->m_state = m_aligncount;

        if (verbose) { printAlignRow(*pi); }

        for (ppi = (*pi)->m_right.begin(); ppi != (*pi)->m_right.end(); ppi++)
        {
          fringe.push_back(*ppi);
        }
      }
    }

    if (fringe.empty())
    {
      cerr << "No nodes at appropriate distance!!!" << endl;
      exit(1);
    }

    // Mark nodes that will be visited in this alignment
    int visit = 0;
    deque<PONode *> tovisit;
    for (fi = fringe.begin(); fi != fringe.end(); fi++) { tovisit.push_back(*fi); }
    while (!tovisit.empty())
    {
      PONode * n = tovisit.front();
      tovisit.pop_front();
      if (n->m_nextvisit != m_aligncount)
      {
        n->m_nextvisit = m_aligncount;
        visit++;
        for (ppi = n->m_right.begin(); ppi != n->m_right.end(); ppi++)
        {
          tovisit.push_back(*ppi);
        }
      }
    }
    //cerr << "visit: " << visit << endl;



    vector<PONode *> endnodes;

    int      bestscore = 0;
    char     bestdir   = '^';
    PONode * bestnode  = NULL;

    int      bestcolscore = -100000;
    char     bestcoldir = ' ';
    PONode * bestcolnode = NULL;

    int ENDSPACEFREE = 1;

    // Align bases
    while (!fringe.empty())
    {
      PONode * n = fringe.front();
      fringe.pop_front();

      if (n->m_state == m_aligncount)
      {
        // already handled this node
        continue;
      }

      bool allgood = true;
      for (pi = n->m_left.begin(); pi != n->m_left.end(); pi++)
      {
        if (((*pi)->m_state != m_aligncount) && ((*pi)->m_nextvisit == m_aligncount))
        {
          allgood = false;
          break;
        }
      }

      if (allgood)
      {
        n->m_alignscore.resize(len+1);
        n->m_alignbuddy.resize(len+1);
        n->m_aligndir.resize(len+1);

        bestscore = 0;
        bestdir   = '^';
        bestnode  = NULL;

        // find the row[0] value
        for (pi = n->m_left.begin(); pi != n->m_left.end(); pi++)
        {
          if ((*pi)->m_state == m_aligncount)
          {
            int piscore = (*pi)->m_alignscore[0] + INDEL;

            if (bestnode == NULL || SCORECMP(piscore, bestscore))
            {
              bestscore = piscore;
              bestnode = *pi;
            }
          }
        }

        n->m_alignscore[0] = bestscore;
        n->m_alignbuddy[0] = bestnode;
        n->m_aligndir[0]   = bestdir;

        if (ENDSPACEFREE)
        {
          n->m_alignscore[0] = 0;
          n->m_alignbuddy[0] = NULL;
          n->m_aligndir[0] = '*';
        }

        // For each position in the row
        for (int i = 1; i <= len; i++)
        {
          // assume best is left indel row[i-1] + INDEL
          bestnode = n;
          bestscore = n->m_alignscore[i-1] + INDEL;
          bestdir = '<';

          // Now try all previous nodes as both indel and mismatch
          for (pi = n->m_left.begin(); pi != n->m_left.end(); pi++)
          {
            if ((*pi)->m_state == m_aligncount)
            {
              // match
              int m = (*pi)->m_alignscore[i-1] + basecmp(n->m_base, str[i-1]);
              if (SCORECMP(m, bestscore))
              {
                bestnode = *pi; bestscore = m; bestdir = '\\';
              }

              // indel
              m = (*pi)->m_alignscore[i] + INDEL;
              if (SCORECMP(m, bestscore))
              {
                bestnode = *pi; bestscore = m; bestdir = '^';
              }
            }
          }

          n->m_alignscore[i] = bestscore;
          n->m_alignbuddy[i] = bestnode;
          n->m_aligndir[i]   = bestdir;
        }

        if (verbose) { printAlignRow(n); }

        // Keep track of the node with the best score in the rightmost
        // column for end-space free where the graph is longer than
        // the sequence.
        if (n->m_alignscore[len] > bestcolscore)
        {
          bestcolscore = n->m_alignscore[len];
          bestcoldir = n->m_aligndir[len];
          bestcolnode = n;
        }

        // mark this node as aligned, and add its right children
        n->m_state = m_aligncount;

        if (n->m_right.empty())
        {
          endnodes.push_back(n);
        }
        else
        {
          for (pi = n->m_right.begin(); pi != n->m_right.end(); pi++)
          {
            fringe.push_back(*pi);
          }
        }
      }
    }

    // Now backtrack to find the optimal alignment

    //cerr << "Merge alignment" << endl;


    // Find the best scoring alignment

    PONode * cur = NULL;
    PONode * right = NULL;
    bestscore = -10000;

    int pos = len;
    // Explicitly check all of the end nodes for best match
    for (pi = endnodes.begin(); pi != endnodes.end(); pi++)
    {
      if (pi == endnodes.begin() || (SCORECMP((*pi)->m_alignscore[pos], bestscore)))
      {
        cur = *pi; bestscore = (*pi)->m_alignscore[pos]; 
      }
    }

    if (bestcolscore > bestscore)
    {
      bestscore = bestcolscore;
      bestdir = bestcoldir;
      cur = bestcolnode;
    }

    if (cur == NULL)
    {
      cerr << "WTF? No cur" << endl;
      dumpMSA();
      cerr << "Endnodes.size: " << endnodes.size() << endl;
      cerr << "bestscore: " << bestscore
           << " bestcolscore: " << bestcolscore << endl;
      if (bestcolnode)
      {
        cerr << "bestcolnode: " << bestcolnode->m_num
             << endl;
      }
      else
      {
        cerr << "bestcolnode: NULL" << endl;
      }

      return;
    }

    //while (cur != m_startnode && pos > 0)
    while (pos > 0)
    {
      //cerr << '.';
      char strbase = str[pos-1];

      bestdir = cur->m_aligndir[pos];
      PONode * left = cur->m_alignbuddy[pos];

      if (verbose) { cerr << "align b:" << cur->m_base << " n:" << str[pos-1] << " " << bestdir << endl; }

      if (bestdir == '\\')
      {
        if (cur->m_base != strbase)
        {
          cur = new PONode(strbase);
          m_nodes.push_back(cur);
        }

        cur->addSeq(m_reads);

        if (right)
        {
          cur->addRight(right);
          right->addLeft(cur);
        }

        right = cur;
        pos--;
      }
      else if (bestdir == '<')
      {
        PONode * n = new PONode(strbase);
        m_nodes.push_back(n);
        n->addSeq(m_reads);

        if (right)
        {
          n->addRight(right);
          right->addLeft(n);
        }

        right = n;

        pos--;
      }
      else if (bestdir == '^')
      {
        // gap in read aligns to graph,
        // nothing to do now, edge created later
      }
      else
      {
        cerr << "bestdir???: " << bestdir << endl;
        exit(1);
      }

      cur = left;
    }

    // Might need to trim the prefix of reads
    cur = right;
    int trim = 0;
    while (cur->m_right.size() == 1)
    {
      trim++;
      cur->m_right[0]->trimLeft(cur);
      cur = cur->m_right[0];
    }

    if (trim)
    {
      cerr << endl;
      cerr << "###Trim: " << trim << endl;
    }

    m_readtrim[m_reads] = trim;

    m_readstarts[m_reads] = right;
    if (verbose) { cerr << "first: "; right->label(cerr) << endl; }

    // this string was fully incorporated into the POA
    m_aligncount++;
    m_reads++;
  }
};



int main (int argc, char ** argv)
{
  if (argc == 1)
  {
    POGraph g;
    g.align("AAAAAAAAAA", 0, 0);
    g.align("TAAAAAAA", 1, 0);
    g.dumpDot();
    g.dumpMSA();
    return 0;
  }

  if (argc != 2)
  {
    cerr << "Usage: po-align bankname" << endl;
    return 1;
  }

  string bankname = argv[1];
  int exitcode = 0;

  BankStream_t lay_bank(Layout_t::NCODE);
  Bank_t read_bank(Read_t::NCODE);
  Bank_t ctg_bank(Contig_t::NCODE);

  try
  {
    lay_bank.open(bankname, B_READ);
    read_bank.open(bankname, B_READ);

    if (!ctg_bank.exists(bankname)){ctg_bank.create(bankname);}
    ctg_bank.open(bankname, B_READ | B_WRITE);

    Layout_t lay;
    int count = 0;

    while (lay_bank >> lay)
    {
      cerr << "Processing layout " << count << " i: " << lay.getIID() << " e:" << lay.getEID() << endl;
      count++;

      vector<Tile_t> & tiling = lay.getTiling();
      sort(tiling.begin(), tiling.end(), TileOrderCmp());

      POGraph graph;

      vector<Tile_t>::iterator ti;
      for (ti = tiling.begin(); ti != tiling.end(); ti++)
      {
        Read_t red;
        read_bank.fetch(ti->source, red);

        string seq = red.getSeqString(ti->range);
        graph.align(seq, ti->source, ti->offset);
      }

      graph.dumpMSA();

      int c = 0;
      for (ti = tiling.begin(); ti != tiling.end(); ti++)
      {
        ti->offset = graph.getStartOffset(c);
        ti->gaps = graph.getGaps(c);
        c++;
      }

      Contig_t ctg;
      ctg.setReadTiling(tiling);
      
      int span = ctg.getSpan();

      string cons;
      string qual;
      for (int j = 0; j < span; j++)
      {
        cons.push_back('X');
        qual.push_back('X');
      }

      string eid(lay.getEID());
      eid += "_poalign";

      ctg.setSequence(cons,qual);
      ctg.setEID(eid);
      ctg.setIID(ctg_bank.getMaxIID()+1);

      ctg_bank.append(ctg);
      break;
    }
  }
  catch (const Exception_t & e) 
  {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }

  return exitcode;
}
