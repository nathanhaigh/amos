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
#include <climits>
#include <list>
using namespace std;

const char * _NAME_ = "layout-paths";



//=============================================================== Options ====//
string  OPT_AlignName;                // alignment name parameter

int     OPT_MaxTrim        = 20;      // maximum ignorable trim length
int     OPT_Fuzzy          = 3;       // fuzzy equals tolerance
int     OPT_Seed           = -1;      // random seed

float   OPT_MinIdentity    = 0.0;     // min identity to tile


//============================================================= Constants ====//
const long int LHAND = -LONG_MAX;
const long int RHAND =  LONG_MAX;

struct Signature_t
{
  vector<const DeltaNode_t *> nodes;  // supporting queries
  vector<long int> vect;              // alignment signature
  // [sR] [eR] [nodeR] [gapQ] [sR] [eR] [nodeR] [gapQ] ...
};

struct Edgeletfoo_t
//!< Compares query lo coord
{
  bool operator() (const DeltaEdgelet_t * a, const DeltaEdgelet_t * b) const
  { return ( a -> loQ < b -> loQ ); }
};


//========================================================== Fuction Decs ====//

//------------------------------------------------------ RecordSignatures ----//
//! \brief Reads the alignment graph and records the alignment signatures
//!
//! \post Populates the sigs vector, one per aligned sequence
//! \return void
//!
void RecordSignatures (const DeltaGraph_t & graph, list<Signature_t> & sigs);


//------------------------------------------------------------- ParseArgs ----//
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \param argc
//! \param argv
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

//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  DeltaGraph_t graph;
  list<Signature_t> sigs;

  //-- COMMAND
  ParseArgs (argc, argv);          // parse the command line arguments
  srand (OPT_Seed);

  //-- GET/CLEAN ALIGNMENTS
  graph . build (OPT_AlignName, false);
  graph . flagScore (0, OPT_MinIdentity);
  graph . flagQLIS( );
  graph . clean( );

  //-- GENERATE SIGNATURES
  RecordSignatures (graph, sigs);

  return EXIT_SUCCESS;
}


//------------------------------------------------------ RecordSignatures ----//
void RecordSignatures (const DeltaGraph_t & graph, list<Signature_t> & sigs)
{
  vector<DeltaEdgelet_t *> edgelets;

  map<string, DeltaNode_t>::const_iterator mi;
  vector<DeltaEdge_t *>::const_iterator ei;
  vector<DeltaEdgelet_t *>::iterator eli;

  for ( mi = graph.qrynodes.begin( ); mi != graph.qrynodes.end( ); ++ mi )
    {
      sigs . push_back (Signature_t());
      sigs . back( ) . nodes . push_back (&(mi -> second));

      //-- Collect the alignments for this query
      //   graph should be clean, so no need to worry about bad alignments
      edgelets . clear( );
      for ( ei  = (mi -> second) . edges . begin( );
            ei != (mi -> second) . edges . end( ); ++ ei )
        for ( eli  = (*ei) -> edgelets . begin( );
              eli != (*ei) -> edgelets . end( ); ++ eli )
          edgelets . push_back (*eli);

      assert ( !edgelets.empty( ) );

      //-- Sort by loQ
      sort (edgelets . begin( ), edgelets . end( ), Edgeletfoo_t( ));

      vector<DeltaEdgelet_t *>::iterator first = edgelets.begin();
      vector<DeltaEdgelet_t *>::iterator last = -- edgelets.end();

      for ( eli = edgelets.begin( ); eli != edgelets.end( ); ++ eli )
        {

        }
    }

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
