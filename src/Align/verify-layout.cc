#include "foundation_AMOS.hh"
#include "AMOS_Foundation.hh"

#include  <string>
#include  <vector>
using namespace std;
using namespace HASHMAP;
using namespace AMOS;

int  main (int argc, char * argv [])
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version = "Version 1.0";
    string helptext = 
"Verify a layout is correct\n"
"Usage: verify-layout [options] <bankname>\n"
"\n"
"Options\n"
" --verbose Show the coordinates and bases for each read in layout\n"
"           readid rc offset roffset len bases\n"
"\n";

    int verbose = 0;

    tf = new AMOS_Foundation(version, helptext, "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("verbose",   &verbose);

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 1)
    {
      cerr << "Usage: verify-layout [options] bankname" << endl;
      return EXIT_FAILURE;
    }

    string bankname = argvv.front(); argvv.pop_front();

    BankStream_t layout_bank(Layout_t::NCODE);
    Bank_t read_bank(Read_t::NCODE);

    layout_bank.open(bankname, B_READ|B_SPY);
    read_bank.open(bankname, B_READ|B_SPY);

    Layout_t lay;

    while (layout_bank >> lay)
    {
      cout << "Verifying layout i:" << lay.getIID()
           << " e:" << lay.getEID() 
           << " reads:" << lay.getTiling().size()
           << endl;

      vector<Tile_t> & tiling = lay.getTiling();

      sort(tiling.begin(), tiling.end(), TileOrderCmp());
      int rightmost = 0;

      for (int i = 0; i < tiling.size(); i++)
      {
        Read_t red;
        read_bank.fetch(tiling[i].source, red);

        Range_t clr(red.getClearRange());

        int len = clr.getLength();
        int right = tiling[i].offset + len - 1;
        int rc = tiling[i].range.isReverse() ?  1 : 0;

        if (verbose)
        {
          if (rc) { clr.swap(); }

          string bases = red.getSeqString(clr);
          string sp;
          sp.append(tiling[i].offset, ' ');

          cout << tiling[i].source << "\t" 
               << rc               << "\t"
               << tiling[i].offset << "\t"
               << right            << "\t"
               << len              << "\t"
               << sp << bases << endl;
        }

        if (tiling[i].offset > rightmost)
        {
          // Coverage hole
          cout << "*** Coverage hole: offset(" << tiling[i].offset
               << ") > rightmost(" << rightmost << ")" << endl;
        }

        if (right > rightmost) { rightmost = right; }
      }
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

