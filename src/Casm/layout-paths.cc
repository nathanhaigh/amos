///////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/08/2005
//!
//! \brief Layout the qry seqs and report alternate paths through the ref
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include "delta.hh"
#include "delta.cc"
#include "amp.hh"
using namespace std;

const char * _NAME_ = "layout-paths";



//=============================================================== Options ====//
string  OPT_AlignName;                // alignment name parameter

int     OPT_MaxTrim        = 20;      // maximum ignorable trim length
int     OPT_Fuzzy          =  3;      // fuzzy equals tolerance
int     OPT_Seed           = -1;      // random seed

float   OPT_MinIdentity    = 70.0;    // min identity to tile


//============================================================= Constants ====//



//========================================================== Fuction Decs ====//

//------------------------------------------------------------- ParseArgs ----//
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
void ParseArgs (int argc, char ** argv);


//------------------------------------------------------------- PrintHelp ----//
//! \brief Prints help information to cout
//!
//! \param s The program name
//! \return void
//!
void PrintHelp (const char * s);


//------------------------------------------------------------ PrintUsage ----//
//! \brief Prints usage information to cout
//!
//! \param s The program name
//! \return void
//!
void PrintUsage (const char * s);


//========================================================== Inline Funcs ====//

//------------------------------------------------------------ PrintDelta ----//
void PrintDelta (const DeltaGraph_t & graph)
{
  bool header;
  unsigned long int s1, e1, s2, e2;
  map<string, DeltaNode_t>::const_iterator mi;
  vector<DeltaEdge_t *>::const_iterator ei;
  vector<DeltaEdgelet_t *>::const_iterator eli;
  //-- Print the file header
  cout
    << graph . refpath << ' ' << graph . qrypath << endl
    << (graph.datatype == PROMER_DATA ? PROMER_STRING : NUCMER_STRING) << endl;
  for ( mi = graph.qrynodes.begin( ); mi != graph.qrynodes.end( ); ++ mi )
    {
      for ( ei  = (mi -> second) . edges . begin( );
            ei != (mi -> second) . edges . end( ); ++ ei )
        {
          header = false;
          for ( eli  = (*ei) -> edgelets . begin( );
                eli != (*ei) -> edgelets . end( ); ++ eli )
            {
              if ( ! (*eli) -> isGOOD )
                continue;
 
              //-- Print the sequence header
              if ( ! header )
                {
                  cout
                    << '>'
                    << *((*ei) -> refnode -> id) << ' '
                    << *((*ei) -> qrynode -> id) << ' '
                    << (*ei) -> refnode -> len << ' '
                    << (*ei) -> qrynode -> len << endl;
                  header = true;
                }
 
 
              //-- Print the alignment
              s1 = (*eli) -> loR;
              e1 = (*eli) -> hiR;
              s2 = (*eli) -> loQ;
              e2 = (*eli) -> hiQ;
              if ( (*eli) -> dirR == REVERSE_DIR )
                Swap (s1, e1);
              if ( (*eli) -> dirQ == REVERSE_DIR )
                Swap (s2, e2);
              cout
                << s1 << ' ' << e1 << ' ' << s2 << ' ' << e2 << ' '
                << (*eli) -> idyc << ' '
                << (*eli) -> simc << ' '
                << (*eli) -> stpc << endl
                << (*eli) -> delta;
            }
        }
    }
}
 


//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  DeltaGraph_t graph;

  //-- COMMAND
  ParseArgs (argc, argv);          // parse the command line arguments
  srand (OPT_Seed);

  graph . build (OPT_AlignName);
  graph . clean( );

  cerr << graph . getNodeCount( ) << '\t'
       << graph . getEdgeCount( ) << '\t'
       << graph . getEdgeletCount( ) << endl;

  graph . flagScore (0, OPT_MinIdentity);
  graph . clean( );

  cerr << graph . getNodeCount( ) << '\t'
       << graph . getEdgeCount( ) << '\t'
       << graph . getEdgeletCount( ) << endl;

  graph . flagQLIS ( );
  graph . clean( );

  cerr << graph . getNodeCount( ) << '\t'
       << graph . getEdgeCount( ) << '\t'
       << graph . getEdgeletCount( ) << endl;

  PrintDelta (graph);

  return EXIT_SUCCESS;
}


//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg  &&
  ((ch = getopt (argc, argv, "f:hi:s:t:")) != EOF) )
    switch (ch)
      {
      case 'f':
        OPT_Fuzzy = atoi (optarg);
        break;

      case 'h':
        PrintHelp (_NAME_);
        exit (EXIT_SUCCESS);
        break;

      case 'i':
	OPT_MinIdentity = atof (optarg);
	break;

      case 's':
	OPT_Seed = atoi (optarg);
	break;

      case 't':
	OPT_MaxTrim = atoi (optarg);
	break;

      default:
        errflg ++;
      }

  if ( errflg > 0 || optind != argc - 1 )
    {
      PrintUsage (_NAME_);
      cout << "Try '" << _NAME_ << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  if ( OPT_Seed < 0 )
    OPT_Seed = time (NULL);

  OPT_AlignName = argv [optind ++];
}


//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  cout
    << ".NAME.\n\n"
    << s << "\n\n"
    << "Highlights structural disagreements between reference and query\n\n";

  PrintUsage (s);

  cout
    << ".OPTIONS.\n\n"
    << "-f uint       Set the fuzzy equals tolerance, default "
    << OPT_Fuzzy << endl
    << "-h            Display help information\n"
    << "-i float      Set the minimum alignment identity, default "
    << OPT_MinIdentity << endl
    << "-s uint       Set random generator seed to unsigned int, default\n"
    << "              seed is generated by the system clock\n"
    << "-t uint       Set maximum ignorable trim length, default "
    << OPT_MaxTrim << endl
    << endl;

  cout
    << ".DESCRIPTION.\n\n"
    << s << " highlights structural disagreements between reference and query\n"
    << "sequences by determining the 'path' each query takes through the\n"
    << "reference. The paths are generated by charting the reference position\n"
    << "and direction of the best alignments for each query. For redundant\n"
    << "query sequence data (e.g. shotgun sequencing reads) similar paths are\n"
    << "combined to form supporting evidence of polymorphism between the\n"
    << "reference and query. For each break in the path, the number of reads\n"
    << "spanning the break are also collected to provide counter evidence.\n"
    << endl
    << "This is an effective way of detecting misassemblies in a set of\n"
    << "contigs. By mapping the original set of sequencing reads to the\n"
    << "contig set, this program can identify the reads that disagree with\n"
    << "the assembly and report the alternate structure they support.\n\n";

  cout
    << ".KEYWORDS.\n\n"
    << "validation, comparison, assembly\n\n";

  return;
}


//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cout
    << ".USAGE.\n\n"
    << s << " <deltafile>\n\n";

  return;
}
