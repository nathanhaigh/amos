#include "foundation_AMOS.hh"
#include <getopt.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <functional>

#include <Slice.h>
#include "ContigIterator_AMOS.hh"

using namespace std;
using namespace AMOS;


#define endl "\n"

bool PRINTALL = 0;
bool PRINTSNPS = 0;
bool PRINTSNPREPORT = 0;

bool USEIID = 0;
bool USEEID = 0;

string BANKNAME;

int SR_PRINTHEADER = 1;
int SR_PRINTBASE = 1;
int SR_PRINTREAD = 0;
int SR_PRINTLIBS = 0;
int SR_PRINTQUAL = 0;

int SR_MINAGREEINGCONFLICTS = 0;
int SR_MINAGREEINGQV = 0;
int SR_MINCONFLICTQV = 0;

map<ID_t, ID_t> frg2lib;


void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "analyzeSNPs - scans multiple alignment of reads looking for snps\n"
    "\n.USAGE.\n"
    "analyzeSNPs -b[ank] <bank_name>\n"
    "\n.OPTIONS.\n"
    "-h, -help     print out help message\n"
    "-b, -bank     bank where assembly is stored\n"
    "-a, -all      print all positions in TCOV format\n"
    "-s, -snps     print only positions with snps in TCOV format (Default)\n\n"

    "-S, -report   print a report on the snps\n"
    "-H            skip header on snp report\n"
    "-B            skip printing bases in snp report\n"
    "-r            print readname in snp report\n"
    "-l            print libid in snp report\n"
    "-q            print qvs in snp report\n"
    "-M, -minsnps <val> Set Minimum number of consistent disagreeing reads to report\n"
    "-C, -cumqv   <val> Set Minimum conflicting cummulative qv to report\n"
    "-Q, -minqv   <val> Set Minimum conflicting qv to report\n\n"

    "-E, -eid      use eids\n"
    "-I, -iid      use iids\n"
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
    {"all",       0, 0, 'a'},
    {"snps",      0, 0, 's'},
    {"report",    0, 0, 'S'},
    {"S",         0, 0, 'S'},
    {"H",         0, 0, 'H'},
    {"B",         0, 0, 'B'},
    {"r",         0, 0, 'r'},
    {"l",         0, 0, 'l'},
    {"q",         0, 0, 'q'},
    {"minsnps",   1, 0, 'M'},
    {"cumqv",     1, 0, 'C'},
    {"minqv",     1, 0, 'Q'},
    {"eid",       0, 0, 'e'},
    {"iid",       0, 0, 'i'},
    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1)
  {
    switch (c)
    {
      case 'h': printHelpText(); exit(0); break;

      case 'b': BANKNAME = optarg;   break;
      case 's': PRINTSNPS = 1;       break;
      case 'a': PRINTALL = 1;        break;
      case 'S': PRINTSNPREPORT = 1;  break;

      case 'H': SR_PRINTHEADER = 0;  break;
      case 'B': SR_PRINTBASE = 0;    break;
      case 'r': SR_PRINTREAD = 1;    break;
      case 'l': SR_PRINTLIBS = 1;    break;
      case 'q': SR_PRINTQUAL = 1;    break;

      case 'M': SR_MINAGREEINGCONFLICTS = atoi(optarg); break;
      case 'C': SR_MINAGREEINGQV        = atoi(optarg); break;
      case 'Q': SR_MINCONFLICTQV        = atoi(optarg); break;

      case 'e': USEEID = 1;          break;
      case 'i': USEIID = 1;          break;
      case '?': return false;
    };
  }

  if (!PRINTSNPS && !PRINTSNPREPORT && !PRINTALL) { PRINTSNPS = 1; }

  //cerr << "SR_MINAGREEINGCONFLICTS: " << SR_MINAGREEINGCONFLICTS << endl; 
  //cerr << "SR_MINAGREEINGQV:        " << SR_MINAGREEINGQV        << endl; 
  //cerr << "SR_MINCONFLICTQV:        " << SR_MINCONFLICTQV        << endl; 

  return true;
}

void printTCOV(ContigIterator_t ci)
{
  libSlice_Slice slice;
  libSlice_Consensus consensusResults;

  int gindex = ci.gindex();

  char cons = ci.cons();
  ostringstream quals;
  ostringstream reads;

  const TiledReadList_t & tiling = ci.getTilingReads();

  slice.dcov = tiling.size();
  slice.bc = new char [slice.dcov+1];

  if (slice.dcov)
  {
    int cur;
    slice.qv = new char [slice.dcov];
    slice.rc = new char [slice.dcov];
    slice.c  = cons;

    TiledReadList_t::const_iterator ri;
    for (ri = tiling.begin(), cur = 0; cur < slice.dcov; ri++, cur++)
    {
      char b = ri->base(gindex);
      char q = ri->qv(gindex);
      char r = ri->m_isRC;

      slice.bc[cur] = b;
      slice.qv[cur] = q;
      slice.rc[cur] = r;

      if (cur) { quals << ":"; reads << ":"; }

      quals << (int) q;

      if (USEEID) { reads << ri->m_eid;     } else
      if (USEIID) { reads << ri->m_iid;     } else
                  { reads << ri->m_readidx; }
    }

    slice.bc[cur] = '\0';

    libSlice_getConsensusParam(&slice, &consensusResults, NULL, 0, 0);

    delete slice.qv;
    delete slice.rc;
  }

  if (USEEID) {cout << ci.getContig().getEID() << " "; }
  else        {cout << ci.getContig().getIID() << " "; }

  cout << gindex+1    << " "
       << ci.uindex() << " "
       << ci.cons();

  if (slice.dcov)
  {
    cout << " " << consensusResults.qvConsensus;
    cout << " " << slice.bc << " " << quals.str() << " " << reads.str();
  }

  cout << endl;
  delete slice.bc;
}


void printSNPReportHeader()
{
  cout << "AsmblID\tGPos\tUPos\tConsensus\tdcov\tconflicts";

  if (SR_PRINTBASE) { cout << "\t(Base)";  }
  if (SR_PRINTREAD) { cout << "\t{Reads}"; }
  if (SR_PRINTLIBS) { cout << "\t<Libs>";  }
  if (SR_PRINTQUAL) { cout << "\t[Quals]"; }

  cout << endl;
}


void printSNPReport(ContigIterator_t ci)
{
  Column_t column(ci.getColumn());

  int dcov = column.m_depth;
  int gindex = ci.gindex();

  bool foundGoodSNP = false;

  vector<BaseStats_t *> freq(column.sortBaseInfo(BaseStatsCmp()));

  // skip the consensus, only look at conflicting bases
  for (int i = 1; !foundGoodSNP && i < freq.size(); i++)
  {
    foundGoodSNP = (freq[i]->m_reads.size() >= SR_MINAGREEINGCONFLICTS) &&
                   (freq[i]->m_cumqv        >= SR_MINAGREEINGQV) &&
                   (freq[i]->m_maxqv        >= SR_MINCONFLICTQV);
  }

  if (foundGoodSNP)
  {
    if (USEEID) { cout << ci.getContig().getEID() << "\t"; }
    else        { cout << ci.getContig().getIID() << "\t"; }

    cout << gindex+1    << "\t"
         << ci.uindex() << "\t"
         << ci.cons()   << "\t"
         << dcov        << "\t"
         << dcov - freq[0]->m_reads.size();

    for (int i = 0; i < freq.size(); i++)
    {
      char base = toupper(freq[i]->m_base);

      ostringstream reads;
      ostringstream quals;
      ostringstream libs;

      bool first = true;
      vector<TiledReadList_t::const_iterator>::const_iterator ri;
      for (ri = freq[i]->m_reads.begin(); ri != freq[i]->m_reads.end(); ri++)
      {
        if (!first) { reads << ":"; quals << ":"; libs << ":"; }
        first = false;

        if (SR_PRINTLIBS) { libs  << frg2lib[(*ri)->m_fragid]; }
        if (SR_PRINTQUAL) { quals << (int) (*ri)->qv(gindex);        }
        if (SR_PRINTREAD) 
        {
          if (USEEID) { reads << (*ri)->m_eid;     } else
          if (USEIID) { reads << (*ri)->m_iid;     } else
                      { reads << (*ri)->m_readidx; }
        }
      }

      if (SR_PRINTBASE) { cout << "\t" << base << "(" << freq[i]->m_reads.size() << ")"; }
      if (SR_PRINTREAD) { cout << "\t{" << reads.str() << "}"; }
      if (SR_PRINTLIBS) { cout << "\t<" << libs.str() << ">";  }
      if (SR_PRINTQUAL) { cout << "\t[" << quals.str() << "]"; }
    }

    cout << endl;
  }
}





//----------------------------------------------
int main(int argc, char **argv)
{
  if (! GetOptions(argc, argv))
  {
    cerr << "Command line parsing failed" << endl;
    printHelpText();
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
    contig_stream.open(BANKNAME, B_READ);
  } 
  catch (Exception_t & e)
  {
    cerr << "Failed to open contig account in bank " << BANKNAME
         << ": " << endl << e << endl;
    exit(1);
  }

  
  try 
  {
    read_bank.open(BANKNAME, B_READ);
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

  if (PRINTSNPREPORT && SR_PRINTHEADER) { printSNPReportHeader(); }

  int snpcount = 0;
  Contig_t ctg;
  while (contig_stream >> ctg) 
  {
    ContigIterator_t ci(ctg, &read_bank);

    while (ci.advanceNext())
    {
      bool hasSNP = ci.hasSNP();
      if (hasSNP) { snpcount++; }

      if ((hasSNP && PRINTSNPS) || PRINTALL)
      {
        printTCOV(ci);
      }

      if (hasSNP && PRINTSNPREPORT)
      {
        printSNPReport(ci);
      }
    }
  }

  cerr << snpcount << " SNPs found" << endl;
}

