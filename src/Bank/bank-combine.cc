////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Mike Schatz
//! \date 09/09/2005   
//!
//! \brief Combines multiple banks into a single bank, while mapping iids
//!        Only handles contigs and reads at this time
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace AMOS;


//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;

  if (argc < 4)
  {
    fprintf(stderr, "USAGE: bank-combine outbank in_1 in_2 ... in_n\n");
    exit(1);
  }

  vector<string> inbanks;
  string outbank = argv[1];
  for (int i = 2; i < argc; i++)
  {
    inbanks.push_back(argv[i]);
  }

  Bank_t ctg_out (Contig_t::NCODE);
  Bank_t red_out (Read_t::NCODE);

  BankStream_t ctg_bank (Contig_t::NCODE);
  BankStream_t red_bank (Read_t::NCODE);

  int redoffset = 0;
  int ctgoffset = 0;

  Read_t red;
  Contig_t ctg;

  try
  {
    ctg_out.create(outbank, B_READ|B_WRITE);
    red_out.create(outbank, B_READ|B_WRITE);

    vector<string>::iterator vi;
    for (vi = inbanks.begin(); vi != inbanks.end(); vi++)
    {
      ctg_bank.open(*vi, B_READ);
      red_bank.open(*vi, B_READ);

      while (red_bank >> red)
      {
        red.setIID(red.getIID()+redoffset);
        red_out.append(red);
      }

      while (ctg_bank >> ctg)
      {
        ctg.setIID(ctg.getIID()+ctgoffset);
        vector<Tile_t>::iterator ti;
        for (ti = ctg.getReadTiling().begin(); 
             ti != ctg.getReadTiling().end();
             ti++)
        {
          ti->source += redoffset;
        }

        ctg_out.append(ctg);
      }

      ctgoffset += ctg_bank.getMaxIID() + 1;
      redoffset += red_bank.getMaxIID() + 1;
    }
  }
  catch (const Exception_t & e) 
  {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  return exitcode;
}

