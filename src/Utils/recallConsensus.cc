#if HAVE_CONFIG_H
#include "config.h"
#endif
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
int VERBOSE = 0;
int AMBIGUITY = 0;

void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "recallConsensus - Recalls the consensus of contigs\n"
    "\n.USAGE.\n"
    "recallConsensus -b[ank] <bank_name> [OPTIONS]\n"
    "\n.OPTIONS.\n"
    "-h, -help       Print out help message\n"
    "-b, -bank       Bank where assembly is stored\n"
    "-v, -verbose    Be verbose\n"
    "-a, -ambiguity  Use Ambiguity Codes\n\n"

    "\n.DESCRIPTION.\n"
    "\n.KEYWORDS.\n"
    "AMOS bank\n"
       << endl;
}

bool GetOptions(int argc, char ** argv)
{  
  int option_index = 0;
  static struct option long_options[] = {
    {"help",        0, 0, 'h'},
    {"bank",        1, 0, 'b'},
    {"verbose",     0, 0, 'v'},
    {"ambiguity",   0, 0, 'a'},

    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1)
  {
    switch (c)
    {
      case 'h': printHelpText(); exit(0); break;
      case 'b': BANKNAME = optarg;   break;
      case 'v': VERBOSE = 1; break;
      case 'a': AMBIGUITY = 1; break;

      case '?': 
         cerr << "Error processing options: " << argv[optind-1] << endl; 
         return false;
    };
  }

  return true;
}

char getAmbiguityFlags(char b)
{
  char flags;
  b = toupper(b);

  switch (b)
  {
    case 'A': flags = AMBIGUITY_FLAGBIT_A; break;
    case 'C': flags = AMBIGUITY_FLAGBIT_C; break;
    case 'G': flags = AMBIGUITY_FLAGBIT_G; break;
    case 'T': flags = AMBIGUITY_FLAGBIT_T; break;

    case 'W': flags = AMBIGUITY_FLAGBIT_A | AMBIGUITY_FLAGBIT_T; break;
    case 'M': flags = AMBIGUITY_FLAGBIT_A | AMBIGUITY_FLAGBIT_C; break;
    case 'R': flags = AMBIGUITY_FLAGBIT_A | AMBIGUITY_FLAGBIT_G; break;
    case 'S': flags = AMBIGUITY_FLAGBIT_C | AMBIGUITY_FLAGBIT_G; break;
    case 'Y': flags = AMBIGUITY_FLAGBIT_C | AMBIGUITY_FLAGBIT_T; break;
    case 'K': flags = AMBIGUITY_FLAGBIT_G | AMBIGUITY_FLAGBIT_T; break;

    case 'V': flags = AMBIGUITY_FLAGBIT_A | AMBIGUITY_FLAGBIT_C | AMBIGUITY_FLAGBIT_G; break;
    case 'H': flags = AMBIGUITY_FLAGBIT_A | AMBIGUITY_FLAGBIT_C | AMBIGUITY_FLAGBIT_T; break;
    case 'D': flags = AMBIGUITY_FLAGBIT_A | AMBIGUITY_FLAGBIT_G | AMBIGUITY_FLAGBIT_T; break;
    case 'B': flags = AMBIGUITY_FLAGBIT_C | AMBIGUITY_FLAGBIT_G | AMBIGUITY_FLAGBIT_T; break;

    case '-': flags = AMBIGUITY_FLAGBIT_GAP; break;

    default:
      flags = AMBIGUITY_FLAGBIT_A | AMBIGUITY_FLAGBIT_C | AMBIGUITY_FLAGBIT_G | AMBIGUITY_FLAGBIT_T;
  };

  return flags;
}

pair<char,char> recallSlice(ContigIterator_t ci)
{
  int gindex = ci.gindex();

  char cons = ci.cons();
  int cqv  = ci.cqv();

  const TiledReadList_t & tiling = ci.getTilingReads();

  if (!tiling.empty())
  {
    libSlice_Slice slice;
    libSlice_Consensus result;
    slice.dcov = tiling.size();

    int cur = 0;

    slice.bc = new char [slice.dcov*5+1];
    slice.qv = new char [slice.dcov*5];
    slice.rc = new char [slice.dcov*5];
    slice.c  = cons;

    TiledReadList_t::const_iterator ri;
    for (ri = tiling.begin(); ri != tiling.end(); ri++)
    {
      char b = ri->base(gindex);
      char q = ri->qv(gindex);
      char r = ri->m_isRC;

      char flags = getAmbiguityFlags(b);

      if (flags & AMBIGUITY_FLAGBIT_A)   { slice.bc[cur] = 'A'; slice.qv[cur] = q; slice.rc[cur] = r; cur++; }
      if (flags & AMBIGUITY_FLAGBIT_C)   { slice.bc[cur] = 'C'; slice.qv[cur] = q; slice.rc[cur] = r; cur++; }
      if (flags & AMBIGUITY_FLAGBIT_G)   { slice.bc[cur] = 'G'; slice.qv[cur] = q; slice.rc[cur] = r; cur++; }
      if (flags & AMBIGUITY_FLAGBIT_T)   { slice.bc[cur] = 'T'; slice.qv[cur] = q; slice.rc[cur] = r; cur++; }
      if (flags & AMBIGUITY_FLAGBIT_GAP) { slice.bc[cur] = '-'; slice.qv[cur] = q; slice.rc[cur] = r; cur++; }
    }

    slice.bc[cur] = '\0';
    slice.dcov = cur;

    if (AMBIGUITY)
    {
      libSlice_getConsensusParam(&slice, &result, NULL, 0, 1);
      libSlice_updateAmbiguityConic(&slice, &result, 0, 0);
      cons = libSlice_convertAmbiguityFlags(result.ambiguityFlags);
      cqv = result.qvConsensus;
    }
    else
    {
      libSlice_getConsensusParam(&slice, &result, NULL, 0, 0);
      cons = result.consensus;
      cqv = result.qvConsensus;
    }

    delete slice.qv; slice.qv=NULL;
    delete slice.rc; slice.rc=NULL;
    delete slice.bc; slice.bc=NULL;
  }

  cqv /= tiling.size();
  cqv += MIN_QUALITY;

  if (cqv > MAX_QUALITY) { cqv = MAX_QUALITY; }

  return make_pair(cons, cqv);
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

  Bank_t contig_bank (Contig_t::NCODE);
  Bank_t read_bank (Read_t::NCODE);

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

  try 
  {
    contig_bank.open(BANKNAME, B_READ | B_WRITE);
  } 
  catch (Exception_t & e)
  {
    cerr << "Failed to open contig account in bank " << BANKNAME
         << ": " << endl << e << endl;
    read_bank.close();
    exit(1);
  }

  try
  {
    int bases = 0;
    int contigcount = 0;

    int ccount = contig_bank.getSize();

    cerr << "Recalling " << ccount << " contigs";

    ProgressDots_t dots(ccount, 50);

    Contig_t ctg;
    AMOS::IDMap_t::const_iterator bi;
    for (bi = contig_bank.getIDMap().begin();
         bi;
         bi++)
    {
      contig_bank.fetch(bi->iid, ctg);
      ContigIterator_t ci(ctg, &read_bank);

      string cons;
      string cqual;

      while (ci.advanceNext())
      {
        pair<char, char> c = recallSlice(ci);
        cons.push_back(c.first);
        cqual.push_back(c.second);
        bases++;
      }

      ctg.setSequence(cons.c_str(), cqual.c_str());
      contig_bank.replace(bi->iid, ctg);

      contigcount++;
      dots.update(contigcount);
    }

    dots.end();
    cerr << endl;
    cerr << "Recalled " << bases << " positions in " << contigcount << " contigs." << endl; 
  }
  catch (Exception_t & e)
  {
    cerr << "Error recalling consensus: " << endl << e << endl;
    exit(1);
  }


}

