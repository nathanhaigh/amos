#include "foundation_AMOS.hh"
#include <getopt.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include "amp.hh"

#include <Slice.h>
#include "ContigIterator_AMOS.hh"

using namespace std;
using namespace AMOS;


#define endl "\n"

bool TCOV = 0;
bool SNPREPORT = 0;

string BANKNAME;

int SR_PRINT_SNPS = 0;
int SR_PRINTBASE = 1;
int SR_PRINTREAD = 0;
int SR_PRINTLIBS = 0;
int SR_PRINTQUAL = 0;

int SR_MINAGREEINGCONFLICTS = 1;
int SR_MINAGREEINGQV = 0;
int SR_MINCONFLICTQV = 0;

int ONE_BASED_GINDEX = 0;
bool USEIID = 0;
bool USEEID = 0;

map<ID_t, ID_t> frg2lib;

class HaplotypeDelta
{
public:
  HaplotypeDelta(int offset, char base) : m_offset(offset), m_base(base) {}

  int  m_offset;
  char m_base;
};


class HaplotypeGroup
{
public:

  HaplotypeGroup() : m_loffset(1024*1024*1024), m_roffset(-1) {}

  void addDelta(int offset, char base)
  {
    m_delta.push_back(HaplotypeDelta(offset, base));
  }

  bool containsRead(ID_t readiid)
  {
    return m_readiids.find(readiid) != m_readiids.end();
  }

  void addRead(const TiledRead_t & red)
  {
    if (red.m_loffset < m_loffset) { m_loffset = red.m_loffset; }
    if (red.m_roffset > m_roffset) { m_roffset = red.m_roffset; }

    m_readiids.insert(red.m_iid);
  }

  int m_loffset;
  int m_roffset;

  vector<HaplotypeDelta> m_delta;

  std::set<ID_t> m_readiids;
};

int getHaplotypeGroup(vector<HaplotypeGroup> & groups, ID_t readiid)
{
  for (int i = 0; i < groups.size(); i++)
  {
    if (groups[i].containsRead(readiid))
    {
      return i;
    }
  }

  return -1;
}


void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "analyzeSNPs - scans multiple alignment of reads looking for snps used for phasing the haplotypes\n"
    "\n.USAGE.\n"
    "analyzeSNPs -b[ank] <bank_name> [OPTIONS]\n"
    "\n.OPTIONS.\n"
    "-h, -help     print out help message\n"
    "-b, -bank     bank where assembly is stored\n"

    "SNP Report Options\n"
    "-r            Print readnames\n"
    "-l            Print libid\n"
    "-q            Print qvs\n"
    "-M, -minsnps  <val> Set Minimum number of consistent disagreeing reads to report (default:" << SR_MINAGREEINGCONFLICTS << ")\n"
    "-C, -cumqv    <val> Set Minimum conflicting cummulative qv to report (default: " << SR_MINAGREEINGQV << ")\n"
    "-Q, -minqv    <val> Set Minimum conflicting qv to report (default: " << SR_MINCONFLICTQV << ")\n\n"

    "General Options\n"
    "-e, -eid      Display eids\n"
    "-i, -iid      Display iids\n"
    "-1            Display 1-based gapped coordinates\n\n"
    "\n.DESCRIPTION.\n"
    "\n.KEYWORDS.\n"
    "AMOS bank\n"
       << endl;
}

bool GetOptions(int argc, char ** argv)
{  
  int option_index = 0;
  static struct option long_options[] = {
    {"help",      0, 0, 'h'},
    {"bank",      1, 0, 'b'},

    {"B",         0, 0, 'B'},
    {"r",         0, 0, 'r'},
    {"l",         0, 0, 'l'},
    {"q",         0, 0, 'q'},
    {"minsnps",   1, 0, 'M'},
    {"M",         1, 0, 'M'},
    {"cumqv",     1, 0, 'C'},
    {"C",         1, 0, 'C'},
    {"minqv",     1, 0, 'Q'},
    {"Q",         1, 0, 'Q'},

    {"eid",       0, 0, 'e'},
    {"iid",       0, 0, 'i'},
    {"1",         0, 0, '1'},

    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1)
  {
    switch (c)
    {
      case 'h': printHelpText(); exit(0); break;
      case 'b': BANKNAME = optarg;   break;

      case 'B': SR_PRINTBASE = 0;    break;
      case 'r': SR_PRINTREAD = 1;    break;
      case 'l': SR_PRINTLIBS = 1;    break;
      case 'q': SR_PRINTQUAL = 1;    break;

      case 'M': SR_MINAGREEINGCONFLICTS = atoi(optarg); break;
      case 'C': SR_MINAGREEINGQV        = atoi(optarg); break;
      case 'Q': SR_MINCONFLICTQV        = atoi(optarg); break;

      case 'e': USEEID = 1;           break;
      case 'i': USEIID = 1;           break;
      case '1': ONE_BASED_GINDEX = 1; break;

      case '?': 
         cerr << "Error processing options: " << argv[optind-1] << endl; 
         return false;
    };
  }

  //cerr << "SR_MINAGREEINGCONFLICTS: " << SR_MINAGREEINGCONFLICTS << endl; 
  //cerr << "SR_MINAGREEINGQV:        " << SR_MINAGREEINGQV        << endl; 
  //cerr << "SR_MINCONFLICTQV:        " << SR_MINCONFLICTQV        << endl; 

  return true;
}



//----------------------------------------------
int main(int argc, char **argv)
{
  if (! GetOptions(argc, argv))
  {
    exit(1);
  }

  // open necessary files
  if (BANKNAME.empty())
  {
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  BankStream_t contig_stream (Contig_t::NCODE);
  Bank_t read_bank (Read_t::NCODE);

  try 
  {
    contig_stream.open(BANKNAME, B_READ|B_SPY);
  } 
  catch (Exception_t & e)
  {
    cerr << "Failed to open contig account in bank " << BANKNAME
         << ": " << endl << e << endl;
    exit(1);
  }

  
  try 
  {
    read_bank.open(BANKNAME, B_READ|B_SPY);
  } 
  catch (Exception_t & e)
  {
    cerr << "Failed to open read account in bank " << BANKNAME
         << ": " << endl << e << endl;
    exit(1);
  }

  if (SR_PRINTLIBS)
  {
    BankStream_t fragment_stream (Fragment_t::NCODE);
    
    try
    {
      fragment_stream.open(BANKNAME, B_READ);
      Fragment_t frg;

      while (fragment_stream >> frg)
      {
        frg2lib.insert(make_pair(frg.getIID(), frg.getLibrary()));
      }
    }
    catch (Exception_t & e)
    {
      cerr << "Error loading fragment-library relations: " << e << endl;
      cerr << "Not printing libraries" << endl;
      SR_PRINTLIBS = 0;
    }
  }

  int bases = 0;
  int snpcount = 0;
  int displaycount = 0;
  int contigcount = 0;

  int ccount = contig_stream.getSize();

  cerr << "Searching " << ccount << " contigs" << endl;

  Contig_t ctg;
  while (contig_stream >> ctg) 
  {
    ContigIterator_t ci(ctg, &read_bank);
    contigcount++;
    ID_t iid = ctg.getIID();

    vector<HaplotypeGroup> groups;

    try
    {
      while (ci.advanceNext())
      {
        bases++;

        bool hasSNP = ci.hasSNP();
        if (hasSNP) { snpcount++; }

        bool foundGoodSNP = false;
        
        Column_t column(ci.getColumn());
        vector<BaseStats_t *> freq(column.getBaseInfo());

        // start at 1 to skip the consensus, only look at conflicting bases
        for (int i = 1; !foundGoodSNP && i < freq.size(); i++)
        {
          foundGoodSNP = (freq[i]->m_reads.size() >= SR_MINAGREEINGCONFLICTS) &&
                         (freq[i]->m_cumqv        >= SR_MINAGREEINGQV) &&
                         (freq[i]->m_maxqv        >= SR_MINCONFLICTQV);
        }

        if (foundGoodSNP)
        {
          displaycount++;

          int dcov   = ci.depth();
          int gindex = ci.gindex();

          if (SR_PRINT_SNPS)
          {
            if (USEEID) { cout << ci.getContig().getEID() << "\t"; }
            else        { cout << ci.getContig().getIID() << "\t"; }

            cout << gindex+ONE_BASED_GINDEX << "\t"
                 << ci.uindex()             << "\t"
                 << ci.cons()               << "\t"
                 << dcov                    << "\t"
                 << dcov - freq[0]->m_reads.size();
          }

          for (int i = 0; i < freq.size(); i++)
          {
            if (SR_PRINT_SNPS)
            {
              cout << "\t" << (char) toupper(freq[i]->m_base) << "(" << freq[i]->m_reads.size() << ")";
            }

            bool first = true;

            map<int, int> groupcnt;

            if (SR_PRINT_SNPS && SR_PRINTREAD) { cout << "\t{"; }

            vector<TiledReadList_t::const_iterator>::const_iterator ri;
            for (ri = freq[i]->m_reads.begin(); ri != freq[i]->m_reads.end(); ri++)
            {
              int groupid = getHaplotypeGroup(groups, (*ri)->m_iid);
              groupcnt[groupid]++;

              if (SR_PRINT_SNPS && SR_PRINTREAD)
              {
                if (!first) { cout << ":"; }
                first = false;

                if (USEEID) { cout << (*ri)->m_eid;     } else
                if (USEIID) { cout << (*ri)->m_iid;     } else
                            { cout << (*ri)->m_readidx; }

                cout << "[" << groupid << "]";
              }
            }

            if (SR_PRINT_SNPS && SR_PRINTREAD) { cout << "}"; }

            map<int,int>::iterator mi;
            int maxid  = -1;
            int maxcnt = -1;

            if (SR_PRINT_SNPS) { cout << "["; }

            first = true;

            for (mi = groupcnt.begin(); mi != groupcnt.end(); mi++)
            {
              if (SR_PRINT_SNPS)
              {
                if (!first) { cout << ","; } first = false;
                cout << mi->first << ":" << mi->second;
              }

              if (((mi->first != -1) && (mi->second > maxcnt)) ||
                  ((mi->first == -1) && (maxid == -1)))
              {
                maxcnt = mi->second;
                maxid  = mi->first;
              }
            }

            int MIN_GROUP_OVERLAP = 5;

            if (maxid == -1 || maxcnt < MIN_GROUP_OVERLAP)
            {
              // create a new group
              maxid = groups.size();
              HaplotypeGroup newgroup;
              groups.push_back(newgroup);
            }

            if (SR_PRINT_SNPS)
            {
              cout << "][[" << maxid << ":" << maxcnt << "]]";
            }

            groups[maxid].addDelta(gindex, toupper(freq[i]->m_base));

            // add all the current reads to the group
            for (ri = freq[i]->m_reads.begin(); ri != freq[i]->m_reads.end(); ri++)
            {
              groups[maxid].addRead(*(*ri));
            }
          }

          if (SR_PRINT_SNPS) { cout << endl; }
        }
      }

      for (int g = 0; g < groups.size(); g++)
      {
        cout << ">" << g << " [" << groups[g].m_loffset << ":" << groups[g].m_roffset << "] ";

        cout << "(";
        bool first = true;
        for (int i = 0; i < groups[g].m_delta.size(); i++)
        {
          if (!first) { cout << " "; } first = false;
          cout << groups[g].m_delta[i].m_offset << ":" << groups[g].m_delta[i].m_base;
        }
        cout << ") ";

        first = true;
        set<ID_t>::iterator si;
        for (si = groups[g].m_readiids.begin(); si != groups[g].m_readiids.end(); si++)
        {
          if (!first) { cout << ":"; } first = false;
          cout << *si;
        }
        cout << endl;

        string seq = ctg.getUngappedSeqString(Range_t(groups[g].m_loffset, groups[g].m_roffset));

        for (int i = 0; i < groups[g].m_delta.size(); i++)
        {
          seq[groups[g].m_delta[i].m_offset - groups[g].m_loffset] = tolower(groups[g].m_delta[i].m_base);
        }

        cout << seq << endl;
      }
    }
    catch (Exception_t & e)
    {
      cerr << "ERROR in contig iid" << iid << "\n" << e;
      exit(1);
    }
    catch(...)
    {
      cerr << "Unknown error in contig iid" << iid << "\n";
      exit(1);
    }
  }

  cerr << "Searched " << bases << " positions in " << contigcount << " contigs." << endl; 
  cerr << displaycount << " positions reported of " << snpcount << " total SNPs found." << endl;
}

