////////////////////////////////////////////////////////////////////////////////
//! \file
//!
//! Reports read overlaps inferred from CTG (contig) multi alignments.
//!
////////////////////////////////////////////////////////////////////////////////

// Note: Some olaps can be really horrible because of unnecssary
// gaps. Think about refining the alignments.

#include "foundation_AMOS.hh"
#include "amp.hh"
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>

using namespace AMOS;
using namespace std;

#define PROGNAME "olapsFromContig"

#define MIN(a,b) ( (a) < (b) ? (a) : (b) )

//=============================================================== Globals ======
string  OPT_BankName;                        // bank name parameter
int     OPT_MinOlapLen = 20;                 // minimum overlap len
float   OPT_MaxOlapErr = 0.05;               // maximum overlap err


struct TilingOrderCmp_t
{
  bool operator() (const Tile_t & a, const Tile_t & b)
  { return ( a.offset < b.offset ); }
};


struct SimpleOverlap_t
{
  int aHang, bHang;
  float err;
};


//--------------------------------------------------- GetOlapLens --------------
//! \brief Returns the length of each read involved in the overlap
//!
//! \return void
//!
void GetOlapLens
(int aLen, int bLen, int aHang, int bHang, int &aOlapLen, int &bOlapLen)
{
  aOlapLen = aLen;
  bOlapLen = bLen;

  if ( aHang > 0 ) aOlapLen -= aHang;
  else bOlapLen += aHang;

  if ( bHang > 0 ) bOlapLen -= bHang;
  else aOlapLen += bHang;
}


//------------------------------------------------------- Overlap --------------
//! \brief Finds best overlap between readA and readB
//!
//! \return void
//!
SimpleOverlap_t Overlap(const char *A, int aLen, const char *B, int bLen, float maxErr);


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


//========================================================= Function Defs ======
int main (int argc, char ** argv)
{
  long nOlaps = 0;
  int exitcode = EXIT_SUCCESS;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {
  
    Bank_t readBank(Read_t::NCODE);
    readBank.open(OPT_BankName, B_SPY);
    Read_t readA, readB;

    BankStream_t contigBank(Contig_t::NCODE);
    contigBank.open(OPT_BankName, B_SPY);
    Contig_t contig;

    while ( contigBank >> contig )
      {
        vector<Tile_t> & tiling = contig.getReadTiling();
        sort(tiling.begin(), tiling.end(), TilingOrderCmp_t());

        vector<Tile_t>::iterator ati;
        vector<Tile_t>::iterator bti;
        for ( ati = tiling.begin(); ati != tiling.end(); ++ati )
          {
            for ( bti = ati+1; bti != tiling.end(); ++bti )
              {
                if ( bti->offset > ati->offset + ati->getGappedLength() - 1 )
                  break;

                //-- Potential overlap between read A and B
                int aOlapHang = bti->offset - ati->offset;
                int bOlapHang =
                  (bti->offset + bti->getGappedLength()) -
                  (ati->offset + ati->getGappedLength());
                Pos_t lOlapPos = bti->offset;
                Pos_t rOlapPos =
                  MIN(ati->offset + ati->getGappedLength() - 1,
                      bti->offset + bti->getGappedLength() - 1);
                int aOlapLen = rOlapPos - lOlapPos + 1;
                int bOlapLen = rOlapPos - lOlapPos + 1;
                vector<Pos_t> aGaps;
                vector<Pos_t> bGaps;

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

                    //-- gapped contig gap pos calculation
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
                            if ( aOlapHang > 0 ) aOlapHang--;
                            else aOlapHang++;
                          }
                        else if ( gapPos > rOlapPos )
                          {
                            // shrink b hang
                            if ( bOlapHang > 0 ) bOlapHang--;
                            else bOlapHang++;
                          }
                        else
                          {
                            // overlap gaps
                            if ( aGapPos < bGapPos )
                              {
                                aOlapLen--; // a gap
                                aGaps.push_back(*(agi-1));
                              }
                            else
                              {
                                bOlapLen--; // b gap
                                bGaps.push_back(*(bgi-1));
                              }
                          }
                      }
                  }

                // fix hangs if a is reversed
                if ( ati->range.isReverse() )
                  {
                    int oaOlapHang = aOlapHang;
                    int obOlapHang = bOlapHang;
                    aOlapHang = -obOlapHang;
                    bOlapHang = -oaOlapHang;
                  }

                // skip if olap is too short
                if ( aOlapLen < OPT_MinOlapLen && bOlapLen < OPT_MinOlapLen )
                  continue;

                char olapType =
                  ati->range.isReverse() == bti->range.isReverse() ? 'N' : 'I';

                //-- Get reads and check error rate of olap
                readBank.fetch(ati->source, readA);
                readBank.fetch(bti->source, readB);

                int aLen = readA.getClearRange().getLength();
                int bLen = readB.getClearRange().getLength();
                int alignLen = aLen + aGaps.size();
                if ( aOlapHang < 0 ) alignLen -= aOlapHang;
                if ( bOlapHang > 0 ) alignLen += bOlapHang;

                char alignA[alignLen+1];
                char alignB[alignLen+1];
                for ( int i = 0; i < alignLen; i++ )
                  {
                    alignA[i] = '.';
                    alignB[i] = '.';
                  }
                alignA[alignLen] = '\0';
                alignB[alignLen] = '\0';

                int aOff = 0;
                int bOff = 0;
                if ( aOlapHang < 0 )
                  aOff -= aOlapHang;
                else
                  bOff += aOlapHang;

                string A = readA.getSeqString(readA.getClearRange());
                string B = readB.getSeqString(readB.getClearRange());

                // lazy way to handle the reverse gap math
                if ( ati->range.isReverse() ) ReverseComplement(A);
                if ( bti->range.isReverse() ) ReverseComplement(B);

                for ( int i = 0; i < aGaps.size(); ++i )
                  A.insert(aGaps[i]+i, 1, '-');
                for ( int i = 0; i < bGaps.size(); ++i )
                  B.insert(bGaps[i]+i, 1, '-');

                if ( ati->range.isReverse() ) ReverseComplement(A);
                if ( bti->range.isReverse() ) ReverseComplement(B);

                if ( olapType == 'I' ) ReverseComplement(B);

                for ( int i = 0; i < A.size(); ++i )
                  alignA[aOff+i] = A[i];
                for ( int i = 0; i < B.size(); ++i )
                  alignB[bOff+i] = B[i];

                /*
                cout << alignA << endl;
                cout << alignB << endl;
                for ( int i = 1; i <= alignLen; ++i )
                  {
                    if ( i % 10 == 0 ) putchar('|');
                    else putchar(' ');
                  }
                putchar('\n');
                */

                int olapErrs = 0;
                int olapColumns = 0;
                for ( int i = 0; i < alignLen; ++i )
                  if ( alignA[i] != '.' && alignB[i] != '.' )
                    {
                      olapColumns++; 
                      if ( alignA[i] != alignB[i] )
                        olapErrs++;
                    }

                float olapErr = float(olapErrs) / float(olapColumns);

                // if olap too noisy, try to realign from scratch
                if ( olapErr > OPT_MaxOlapErr )
                  {
                    fprintf(stderr, "Redoing overlap:\n");
                    fprintf(stderr, "%d\t%d\t%c\t%d\t%d\t%d\t%d\t%.2f\t%.2f\n",
                            ati->source, bti->source, olapType,
                            aOlapHang, bOlapHang,
                            aOlapLen, bOlapLen,
                            olapErr*100.0,  olapErr*100.0);

                    string A = readA.getSeqString(readA.getClearRange());
                    string B = readB.getSeqString(readB.getClearRange());
                    if ( olapType == 'I' ) ReverseComplement(B);

                    // needleman-wunsch with no end penalties
                    SimpleOverlap_t newolap =
                      Overlap(A.c_str(), aLen, B.c_str(), bLen, OPT_MaxOlapErr);

                    aOlapHang = newolap.aHang;
                    bOlapHang = newolap.bHang;
                    olapErr = newolap.err;
                    GetOlapLens(aLen, bLen,
                                aOlapHang, bOlapHang,
                                aOlapLen, bOlapLen);

                    fprintf(stderr, "%d\t%d\t%c\t%d\t%d\t%d\t%d\t%.2f\t%.2f\n",
                            ati->source, bti->source, olapType,
                            aOlapHang, bOlapHang,
                            aOlapLen, bOlapLen,
                            olapErr*100.0,  olapErr*100.0);

                    if ( aOlapLen >= OPT_MinOlapLen &&
                         bOlapLen >= OPT_MinOlapLen &&
                         olapErr <= OPT_MaxOlapErr )
                      { fprintf(stderr, "Accepted\n"); }
                    else
                      { fprintf(stderr, "Rejected\n"); }
                  }


                if ( aOlapLen >= OPT_MinOlapLen &&
                     bOlapLen >= OPT_MinOlapLen &&
                     olapErr <= OPT_MaxOlapErr )
                  {
                    printf("%d\t%d\t%c\t%d\t%d\t%.2f\t%.2f\n",
                           ati->source, bti->source, olapType,
                           aOlapHang, bOlapHang,
                           olapErr*100.0,  olapErr*100.0);
                    
                    if ( ++nOlaps % 10000 == 0 )
                      fprintf(stderr, "Processed %ld overlaps\n", nOlaps);
                  }
              }
          }
      }

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


//--------------------------------------------------------------- Overlap ------
SimpleOverlap_t Overlap(const char *A, int aLen, const char *B, int bLen, float maxErr)
{
  // %maxErr flat line
  double MatchScore = 1;
  double MismatchScore = (double(maxErr) - MatchScore) / double(maxErr);
  double GapScore = MismatchScore;

  Pos_t i,j;
  char O[aLen][bLen];
  double S[aLen][bLen];
  double score, bestScore;
  char bestOp;

  // Fill first column
  for ( i = 0, j = 0; i < aLen; ++i )
    {
      // no gap penalty in first column
      S[i][j] = ( A[i] == B[j] ? MatchScore : MismatchScore );
      O[i][j] = '\\';
    }

  // Fill first row
  for ( i = 0, j = 0; j < bLen; ++j )
    {
      // no gap penalty in first row
      S[i][j] = ( A[i] == B[j] ? MatchScore : MismatchScore );
      O[i][j] = '\\';
    }

  // Fill interior cells
  for ( i = 1; i < aLen-1; ++i )
    for ( j = 1; j < bLen-1; ++j )
      {
        bestScore = S[i-1][j-1] + ( A[i] == B[j] ? MatchScore : MismatchScore );
        bestOp = '\\';

        score = S[i][j-1] + GapScore;
        if ( score > bestScore )
          {
            bestScore = score;
            bestOp = '-';
          }

        score = S[i-1][j] + GapScore;
        if ( score > bestScore )
          {
            bestScore = score;
            bestOp = '|';
          }

        S[i][j] = bestScore;
        O[i][j] = bestOp;
      }

  // Fill last column
  j = bLen-1;
  for ( i = 1; i < aLen-1; ++i )
    {
      bestScore = S[i-1][j-1] + ( A[i] == B[j] ? MatchScore : MismatchScore );
      bestOp = '\\';

      score = S[i][j-1] + GapScore;
      if ( score > bestScore )
        {
          bestScore = score;
          bestOp = '-';
        }

      score = S[i-1][j]; // no gap penalty in last column
      if ( score > bestScore )
        {
          bestScore = score;
          bestOp = '|';
        }

      S[i][j] = bestScore;
      O[i][j] = bestOp;
    }

  // Fill last row
  i = aLen-1;
  for ( j = 1; j < bLen; ++j )
    {
      bestScore = S[i-1][j-1] + ( A[i] == B[j] ? MatchScore : MismatchScore );
      bestOp = '\\';

      score = S[i][j-1]; // no gap penalty in last row
      if ( score > bestScore )
        {
          bestScore = score;
          bestOp = '-';
        }

      score = S[i-1][j] + GapScore;
      if ( score > bestScore )
        {
          bestScore = score;
          bestOp = '|';
        }

      S[i][j] = bestScore;
      O[i][j] = bestOp;
    }

  /*
  char showA[2*(aLen+bLen)];
  char showB[2*(aLen+bLen)];
  int showCnt = 0;
  */

  // Backtrack
  i = aLen-1;
  j = bLen-1;
  int olapErrs = 0;
  int olapColumns = 0;
  SimpleOverlap_t olap;
  while ( i >= 0 && j >= 0 )
    {
      switch ( O[i][j] )
        {
        case '\\':
          if ( i == aLen - 1 )
            {
              // showCnt = 0;
              olapErrs = 0;
              olapColumns = 0;
              olap.bHang = (bLen-1-j);
            }
          if ( j == bLen - 1 )
            {
              // showCnt = 0;
              olapErrs = 0;
              olapColumns = 0;
              olap.bHang = -(aLen-1-i);
            }
          /*
          showA[showCnt] = A[i];
          showB[showCnt] = B[j];
          showCnt++;
          */
          if ( A[i] != B[j] )
            olapErrs++;
          olapColumns++;
          i--; j--;
          break;

        case '-':
          /*
          showA[showCnt] = '.';
          showB[showCnt] = B[j];
          showCnt++;
          */
          olapErrs++;
          olapColumns++;
          j--;
          break;

        case '|':
          /*
          showA[showCnt] = A[i];
          showB[showCnt] = '.';
          showCnt++;
          */
          olapErrs++;
          olapColumns++;
          i--;
          break;

        default:
          fprintf(stderr,"Error: i = %d, j = %d, O = %c\n", i, j, O[i][j]);
          exit(EXIT_FAILURE);
        }
    }

  if ( i < 0 ) olap.aHang = -(j+1);
  if ( j < 0 ) olap.aHang =  (i+1);
  olap.err = float(olapErrs) / float(olapColumns);
  /*
  for ( i = showCnt - 1; i >= 0; --i )
    putchar(showA[i]);
  putchar('\n');
  for ( i = showCnt - 1; i >= 0; --i )
    putchar(showB[i]);
  putchar('\n');
  printf("%f\n", S[aLen-1][bLen-1]);
  */
  return olap;
}


//------------------------------------------------------------- ParseArgs ------
void ParseArgs (int argc, char **argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt(argc, argv, "he:o:")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp(PROGNAME);
        exit(EXIT_SUCCESS);
      break;

      case 'e':
        OPT_MaxOlapErr = atof(optarg);
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

  if ( OPT_MaxOlapErr < 0 || OPT_MaxOlapErr > 1 )
    {
      cerr << "Error: Maximum overlap error must be 0 < err < 1\n";
      errflg++;
    }

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
void PrintHelp (const char *s)
{
  PrintUsage(s);

  cerr
    << "-e float      Set maximum overlap error, default "
    << OPT_MaxOlapErr << endl
    << "-o int        Set minimum overlap length, default "
    << OPT_MinOlapLen << endl
    << endl;
  cerr
    << "  Converts bank CTG messages to overlaps.\n"
    << endl;

  return;
}


//------------------------------------------------------------ PrintUsage ------
void PrintUsage (const char *s)
{
  cerr << "\nUSAGE: " << s << " [OPTION]... BANK\n";
}
