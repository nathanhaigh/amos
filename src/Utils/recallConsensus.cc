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


int bitcount(char c)
{
  int sum = 0;

  for (int i = 0; i < 8; i++)
  {
    char mask = 1 << i;
    sum += ((c & mask) ? 1 : 0);
  }

  return sum;
}

char deamb(char b)
{
  char flags;
  char upper = toupper(b);

  switch (upper)
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



bool normalizeSlice(libSlice_Slice * ambslice, libSlice_Slice * newslice)
{
  int ambcount = 0;
  for (int i = 0; i < ambslice->dcov; i++)
  {
    ambcount += bitcount(deamb(ambslice->bc[i]));
  }

  if (ambcount == ambslice->dcov)
  {
    return false;
  }

  cerr << " dcov: " << ambslice->dcov << " " << ambcount;

  newslice->dcov = ambcount;
  newslice->bc = new char[ambcount+1];
  newslice->rc = new char[ambcount];
  newslice->qv = new char[ambcount];
  newslice->c = ambslice->c;

  int cur = 0;
  for (int i = 0; i < ambslice->dcov; i++)
  {
    cerr << " " << ambslice->bc[i] << "[" << (int) ambslice->qv[i] << "]";
    char flags = deamb(ambslice->bc[i]); 
    int ambcount = bitcount(flags);

    if (flags & AMBIGUITY_FLAGBIT_A)
    {
      newslice->bc[cur] = 'A'; 
      newslice->qv[cur] = ambslice->qv[i]; 
      newslice->rc[cur] = ambslice->rc[i];
      cur++;
    }

    if (flags & AMBIGUITY_FLAGBIT_C)
    {
      newslice->bc[cur] = 'C'; 
      newslice->qv[cur] = ambslice->qv[i]; 
      newslice->rc[cur] = ambslice->rc[i];
      cur++;
    }

    if (flags & AMBIGUITY_FLAGBIT_G)
    {
      newslice->bc[cur] = 'G'; 
      newslice->qv[cur] = ambslice->qv[i]; 
      newslice->rc[cur] = ambslice->rc[i];
      cur++;
    }
    
    if (flags & AMBIGUITY_FLAGBIT_T)
    {
      newslice->bc[cur] = 'T'; 
      newslice->qv[cur] = ambslice->qv[i]; 
      newslice->rc[cur] = ambslice->rc[i];
      cur++;
    }
  }
  newslice->bc[cur] = '\0';

  return true;
}

pair<char,char> recallSlice(ContigIterator_t ci)
{
  libSlice_Slice slice;
  libSlice_Consensus consensusResults;

  int gindex = ci.gindex();

  char cons = ci.cons();
  char cqv  = ci.cqv();

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


    cons = consensusResults.consensus;
    cqv = consensusResults.qvConsensus;

    libSlice_Slice normal;
    if (normalizeSlice(&slice, &normal))
    {
      libSlice_getConsensusParam(&normal, &consensusResults, NULL, 0, 1);
      libSlice_updateAmbiguityConic(&normal, &consensusResults, 0, 0);

      if (USEEID) {cerr << ci.getContig().getEID() << " "; }
      else        {cerr << ci.getContig().getIID() << " "; }

      cerr << gindex+ONE_BASED_GINDEX << " "
           << ci.uindex()             << " "
           << ci.cons();
      
      cerr << " " << (int) cqv << " " << slice.bc << " " << quals.str() << " " << reads.str();
     
      cons = libSlice_convertAmbiguityFlags(consensusResults.ambiguityFlags);
      cqv = consensusResults.qvConsensus;

      cerr << " New " << cons << " " << (int) cqv << " " <<  normal.bc;

      for (int j = 0; j < normal.dcov; j++)
      {
        cerr << ":" << (int) normal.qv[j];
      }
      cerr << endl;

      delete normal.bc; normal.bc=NULL;
      delete normal.rc; normal.rc=NULL;
      delete normal.qv; normal.qv=NULL;
    }

    delete slice.qv; slice.qv=NULL;
    delete slice.rc; slice.rc=NULL;
  }

  delete slice.bc; slice.bc=NULL;

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
    contig_bank.open(BANKNAME, B_READ | B_WRITE);
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
    contigcount++;

    dots.update(contigcount);

    string cons;
    string cqual;

    while (ci.advanceNext())
    {
      bases++;
      pair<char, char> c = recallSlice(ci);
      cons.push_back(c.first);
      cqual.push_back(c.second+'0');
    }

    ctg.setSequence(cons.c_str(), cqual.c_str());

    contig_bank.replace(bi->iid, ctg);
 //break;
  }

  dots.end();

  cerr << endl;
  cerr << "Searched " << bases << " positions in " << contigcount << " contigs." << endl; 
}

