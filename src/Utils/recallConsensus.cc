#include "foundation_AMOS.hh"
#include <getopt.h>
#include <map>
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

string BANKNAME;
int USEIID = 0;
int USEEID = 1;
int ONE_BASED_GINDEX = 0;

void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "recallConsensus - Recalls the consensus of contigs\n"
    "\n.USAGE.\n"
    "recallConsensus -b[ank] <bank_name> [OPTIONS]\n"
    "\n.OPTIONS.\n"
    "-h, -help     print out help message\n"
    "-b, -bank     bank where assembly is stored\n\n"

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

    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1)
  {
    switch (c)
    {
      case 'h': printHelpText(); exit(0); break;
      case 'b': BANKNAME = optarg;   break;

      case '?': 
         cerr << "Error processing options: " << argv[optind-1] << endl; 
         return false;
    };
  }

  return true;
}

pair<char,char> recallSlice(ContigIterator_t ci)
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

  cout << gindex+ONE_BASED_GINDEX << " "
       << ci.uindex()             << " "
       << ci.cons();

  if (slice.dcov)
  {
    cout << " " << consensusResults.qvConsensus;
    cout << " " << slice.bc << " " << quals.str() << " " << reads.str();
  }

  cout << endl;
  delete slice.bc;

  return make_pair('A', 'X');
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

  int bases = 0;
  int snpcount = 0;
  int displaycount = 0;
  int contigcount = 0;

  int ccount = contig_stream.getSize();

  cerr << "Recalling " << ccount << " contigs";

  ProgressDots_t dots(ccount, 50);

  Contig_t ctg;
  while (contig_stream >> ctg) 
  {
    ContigIterator_t ci(ctg, &read_bank);
    contigcount++;

    dots.update(contigcount);

    string cons;
    string cqual;

    while (ci.advanceNext())
    {
      pair<char, char> c = recallSlice(ci);
      cons += c.first;
      cqual += c.second;
    }
  }

  dots.end();

  cerr << endl
       << displaycount << " positions reported of " << snpcount << " total SNPs found." << endl;
  cerr << "Searched " << bases << " positions in " << contigcount << " contigs." << endl; 
}

