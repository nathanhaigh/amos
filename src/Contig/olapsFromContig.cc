////////////////////////////////////////////////////////////////////////////////
//! \file
//!
//! Reports read overlaps inferred from CTG (contig) multi alignments.
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"

using namespace AMOS;
using namespace std;

#define PROGNAME "olapsFromContig"

#define MIN(a,b) ( (a) < (b) ? (a) : (b) )

//=============================================================== Globals ======
string  OPT_BankName;                        // bank name parameter
int     OPT_MinOlapLen = 20;                 // minimum overlap len

//----------------------------------------------------- ParseArgs --------------
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
void ParseArgs (int argc, char ** argv);


//----------------------------------------------------- PrintHelp --------------
//! \brief Prints help information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintHelp (const char * s);


//----------------------------------------------------- PrintUsage -------------
//! \brief Prints usage information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintUsage (const char * s);


struct TilingOrderCmp
{
  bool operator() (const Tile_t & a, const Tile_t & b)
  { return ( a.offset < b.offset ); }
};


//========================================================= Function Defs ======
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {
  
    BankStream_t contigBank(Contig_t::NCODE);
    contigBank.open(OPT_BankName, B_SPY);
    Contig_t contig;

    int nContigs = 0;
    long long loopa = 0;
    long long loopb = 0;
    long long loopc = 0;
    while ( contigBank >> contig )
      {
        //cerr << "\rContig " << ++nContigs;

        vector<Tile_t> & tiling = contig.getReadTiling();
        sort(tiling.begin(), tiling.end(), TilingOrderCmp());

        vector<Tile_t>::iterator ati;
        vector<Tile_t>::iterator bti;
        for ( ati = tiling.begin(); ati != tiling.end(); ++ati )
          {
            for ( bti = ati+1; bti != tiling.end(); ++bti )
              {
                if ( bti->offset > ati->offset + ati->getGappedLength() - 1 )
                  break;

                //-- Potential overlap between read A and B
                Size_t aHang = bti->offset - ati->offset;
                Size_t bHang =
                  (bti->offset + bti->getGappedLength()) -
                  (ati->offset + ati->getGappedLength());
                Pos_t lOlapPos = bti->offset;
                Pos_t rOlapPos =
                  MIN(ati->offset + ati->getGappedLength() - 1,
                      bti->offset + bti->getGappedLength() - 1);
                Size_t aOlapLen = rOlapPos - lOlapPos + 1;
                Size_t bOlapLen = rOlapPos - lOlapPos + 1;

                sort(ati->gaps.begin(), ati->gaps.end());
                sort(bti->gaps.begin(), bti->gaps.end());

                //-- Adjust hangs for alignment gaps
                vector<Pos_t>::const_iterator agi = ati->gaps.begin();
                vector<Pos_t>::const_iterator bgi = bti->gaps.begin();
                while ( agi != ati->gaps.end() || bgi != bti->gaps.end() )
                  {
                    Pos_t gapPos;
                    Pos_t aGapPos = MAX_POS;
                    Pos_t bGapPos = MAX_POS;

                    //-- gap pos calculation
                    // gapped offset + ungapped gap pos + #previous gaps + 1
                    if ( agi != ati->gaps.end() )
                      {
                        aGapPos =
                          ati->offset + *agi + (agi - ati->gaps.begin()) + 1;
                      }
                    if ( bgi != bti->gaps.end() )
                      {
                        bGapPos =
                          bti->offset + *bgi + (bgi - bti->gaps.begin()) + 1;
                      }
                    if ( aGapPos == bGapPos )
                      {
                        // remove double gap in overlap
                        ++agi;
                        ++bgi;
                        aOlapLen--;
                        bOlapLen--;
                      }
                    else
                      {
                        // check left-most gap
                        if ( aGapPos < bGapPos )
                          { gapPos = aGapPos; ++agi; }
                        else
                          { gapPos = bGapPos; ++bgi; }

                        if ( gapPos < lOlapPos )
                          {
                            // shrink a hang
                            if ( aHang > 0 ) aHang--;
                            else aHang++;
                          }
                        else if ( gapPos > rOlapPos )
                          {
                            // shrink b hang
                            if ( bHang > 0 ) bHang--;
                            else bHang++;
                          }
                        else
                          {
                            // overlap gaps
                            if ( aGapPos < bGapPos )
                              aOlapLen--; // a gap
                            else
                              bOlapLen--; // b gap
                          }
                      }
                  }

                // fix hangs if a is reversed
                if ( ati->range.isReverse() )
                  {
                    Size_t oaHang = aHang;
                    Size_t obHang = bHang;
                    aHang = -obHang;
                    bHang = -oaHang;
                  }

                if ( aOlapLen >= OPT_MinOlapLen && bOlapLen >= OPT_MinOlapLen )
                  {
                    char olapType =
                      ati->range.isReverse() == bti->range.isReverse()
                      ? 'N' : 'I';
                    cout
                      << ati->source << '\t'
                      << bti->source << '\t'
                      << olapType << '\t'
                      << aHang << '\t'
                      << bHang << '\t'
                      << aOlapLen << '\t'
                      << bOlapLen << '\n';
                  }
              }
          }
      }
    cerr << " ... done.\n";

    contigBank.close();
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH
  
  return exitcode;
}



//------------------------------------------------------------- ParseArgs ------
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt(argc, argv, "ho:")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp(PROGNAME);
        exit(EXIT_SUCCESS);
      break;

      case 'o':
        OPT_MinOlapLen = atoi(optarg);
        break;

      default:
        errflg ++;
      }

  if ( optind+1 == argc )
    OPT_BankName = argv[optind++];
  else
    errflg ++;

  if ( OPT_MinOlapLen <= 0 )
    {
      cerr << "Error: Minimum overlap length must be > 0\n";
      errflg++;
    }

  if ( errflg > 0 )
    {
      PrintUsage(PROGNAME);
      cerr << "Try '" << PROGNAME << " -h' for more information.\n";
      exit(EXIT_FAILURE);
    }
}


//------------------------------------------------------------- PrintHelp ------
void PrintHelp (const char * s)
{
  PrintUsage(s);

  cerr
    << "-o int        Set minimum overlap length, default "
    << OPT_MinOlapLen << endl
    << endl;
  cerr
    << "  Converts bank CTG messages to overlaps.\n"
    << endl;

  return;
}


//------------------------------------------------------------ PrintUsage ------
void PrintUsage (const char * s)
{
  cerr << "\nUSAGE: " << s << " [OPTION]... BANK\n";
}
