////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/30/2004
//!
//! Build overlaps from mummer alignment data
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "delta.hh"
#include <list>
#include <map>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <unistd.h>
using namespace std;
using namespace AMOS;



//=============================================================== Options ====//
string  OPT_BankName;                   // bank name parameter
string  OPT_AlignName;                  // alignment name parameter

int     OPT_MaxTrimLen       = 20;      // maximum ignorable trim length

float   OPT_MinIdentity      = 90.0;    // minimum overlap identity


//============================================================= Constants ====//
const char FORWARD_CHAR  = '+';
const char REVERSE_CHAR  = '-';


struct Align_t
{
  ID_t idB;
  char oriB;
  long int lo, hi, loB, hiB;
  float idy;
};

struct ReadAlign_t
{
  ID_t id;
  long int len;

  vector<Align_t *> all;

  ~ReadAlign_t ( )
  {
    vector<Align_t *>::iterator api;
    for ( api = all . begin( ); api != all . end( ); api ++ )
      delete (*api);
  }
};

struct Mapping_t
{
  map<ID_t, ReadAlign_t *> reads;

  ~Mapping_t ( )
  {
    map<ID_t, ReadAlign_t *>::iterator rapi;
    for ( rapi = reads . begin( ); rapi != reads . end( ); rapi ++ )
      delete (rapi -> second);
  }
};

struct Overlaps_t
{
  list<Overlap_t *> ovls;

  ~Overlaps_t ( )
  {
    list<Overlap_t *>::iterator opi;
    for ( opi = ovls . begin( ); opi != ovls . end( ); opi ++ )
      delete (*opi);
  }
};

struct OverlapCmp_t
{
  bool operator( ) (const Overlap_t * A, const Overlap_t * B)
  {
    if ( A -> getReads( ) . first < B -> getReads( ) . first )
      return true;
    else
      return false;
  }
};


//========================================================== Fuction Decs ====//
long int lmin (long int a, long int b)
{ return (a < b ? a : b); }

long int lmax (long int a, long int b)
{ return (a < b ? b : a); }


//---------------------------------------------------------- LoadOverlaps ----//
//! \brief Uploads the overlaps into the AMOS bank
//!
//! \return void
//!
void LoadOverlaps (const Overlaps_t & overlaps);


//--------------------------------------------------------------- Overlap ----//
//! \brief Get the overlaps
//!
//! \post overlaps are sorted by read A ID
//! \return void
//!
void Overlap (Mapping_t & mapping, Overlaps_t & overlaps);


//------------------------------------------------------------ ParseAlign ----//
//! \brief Parse, sort and store the alignment input
//!
//! \return void
//!
void ParseAlign (Mapping_t & mapping);


//------------------------------------------------------------- PrintOVL -----//
//! \brief Prints the overlaps to stdout
//!
//! \param overlaps The overlaps
//! \return void
//!
void PrintOVL (const Overlaps_t & overlaps);


//------------------------------------------------------------- ParseArgs ----//
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
void ParseArgs (int argc, char ** argv);


//------------------------------------------------------------- PrintHelp ----//
//! \brief Prints help information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintHelp (const char * s);


//------------------------------------------------------------ PrintUsage ----//
//! \brief Prints usage information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintUsage (const char * s);


//========================================================== Inline Funcs ====//
//------------------------------------------------------------- IsOverlap ----//
bool IsOverlap (const ReadAlign_t * A,
		const ReadAlign_t * B,
		const Align_t * cand)
{
  return (
	  (cand -> idy >= OPT_MinIdentity)
	  &&
	  (cand -> lo  <= OPT_MaxTrimLen  ||
	   cand -> loB <= OPT_MaxTrimLen)
	  &&
	  (cand -> hi  + OPT_MaxTrimLen > A -> len  ||
	   cand -> hiB + OPT_MaxTrimLen > B -> len)
	  );
}


//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  Mapping_t mapping;
  Overlaps_t overlaps;

  //-- COMMAND
  ParseArgs (argc, argv);          // parse the command line arguments

  //-- INPUT
  ParseAlign (mapping);            // parse the alignment data

  //-- OVERLAP
  Overlap (mapping, overlaps);

  //-- OUTPUT
  //  PrintOVL (overlaps);

  //-- UPLOAD
  LoadOverlaps (overlaps);

  return EXIT_SUCCESS;
}




//---------------------------------------------------------- LoadOverlaps ----//
void LoadOverlaps (const Overlaps_t & overlaps)
{
  BankStream_t ovl_bank (Overlap_t::NCODE);
  list<Overlap_t *>::const_iterator opi;

  try {
    if ( ovl_bank . exists (OPT_BankName) )
      ovl_bank . open (OPT_BankName);
    else
      ovl_bank . create (OPT_BankName);

    //-- Upload da overlaps
    for ( opi  = overlaps . ovls . begin( );
	  opi != overlaps . ovls . end( ); opi ++ )
      ovl_bank << **opi;

    ovl_bank . close( );
  }
  catch (Exception_t & e) {

    //-- On error, print debugging information
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    exit (EXIT_FAILURE);
  }
}




//--------------------------------------------------------------- Overlap ----//
void Overlap (Mapping_t & mapping, Overlaps_t & overlaps)
{
  list<Overlap_t *>::iterator opi;
  vector<Align_t *>::iterator api;
  map<ID_t, ReadAlign_t *>::iterator rmpiA, rmpiB;
  ReadAlign_t * rapA, * rapB;
  Align_t * ap;
  Overlap_t * op;
  pair<ID_t, ID_t> idp;

  //-- For each read, get it's overlaps
  for ( rmpiA  = mapping . reads . begin( );
	rmpiA != mapping . reads . end( ); rmpiA ++ )
    {
      rapA = rmpiA -> second;

      //-- For each alignment
      for ( api  = rapA -> all . begin( );
	    api != rapA -> all . end( ); api ++ )
	{
	  ap = *api;

	  rmpiB = mapping . reads . find (ap -> idB);
	  assert ( rmpiB != mapping . reads . end( ));
	  rapB = rmpiB -> second;

	  //-- If an overlap
	  if ( IsOverlap (rapA, rapB, ap) )
	    {
	      overlaps . ovls . push_back (new Overlap_t( ));
	      op = overlaps . ovls . back( );

	      op -> setReads (make_pair (rapA -> id, rapB -> id));

	      if ( ap -> oriB == FORWARD_CHAR )
		op -> setAdjacency (Overlap_t::NORMAL);
	      else
		op -> setAdjacency (Overlap_t::INNIE);

	      op -> setAhang (ap->lo  - ap->loB);
	      op -> setBhang ((rapB->len - ap->hiB) - (rapA->len - ap->hi));
	    }
	}
    }

  //-- Sort by read A
  overlaps . ovls . sort (OverlapCmp_t( ));

  //-- Remove redundant overlaps, all overlaps where idB <= idA
  opi = overlaps . ovls . begin( );
  while ( opi != overlaps . ovls . end( ) )
    {
      idp = (*opi) -> getReads( );
      if ( idp . second <= idp . first )
	{
	  delete (*opi);
	  opi = overlaps . ovls . erase (opi);
	}
      else
	opi ++;
    }
}




//------------------------------------------------------------ ParseAlign ----//
void ParseAlign (Mapping_t & mapping)
{
  bool insret;
  DeltaReader_t dr;
  ID_t id, idB;
  stringstream ss;
  ReadAlign_t * rap;
  Align_t * ap;
  vector<DeltaAlignment_t>::const_iterator dai;

  dr . open (OPT_AlignName);

  rap = NULL;

  //-- Process the delta input
  while ( dr . readNextHeadersOnly( ) )
    {
      ss . str (dr . getRecord( ) . idR);
      ss >> idB;
      assert (!ss . fail( ));
      ss . clear( );

      ss . str (dr . getRecord( ) . idQ);
      ss >> id;
      assert (!ss . fail( ));
      ss . clear( );

      //-- If a new read, push it
      if ( rap == NULL  ||  id != rap -> id )
	{
	  rap = new ReadAlign_t( );
	  insret = (mapping . reads . insert (make_pair (id, rap))) . second;
	  assert (insret);

	  rap -> id = id;
	  rap -> len = dr . getRecord( ) . lenQ;
	}

      //-- For all the alignments in this record
      for ( dai  = dr . getRecord( ) . aligns . begin( );
	    dai != dr . getRecord( ) . aligns . end( ); dai ++ )
	{
	  rap -> all . push_back (new Align_t( ));
	  ap = rap -> all . back( );

	  ap -> idB = idB;
	  ap -> idy = dai -> idy;

	  //-- Force ascending coordinates
	  if ( dai -> sQ < dai -> eQ )
	    {
	      ap -> lo = dai -> sQ;
	      ap -> hi = dai -> eQ;
	    }
	  else
	    {
	      ap -> lo = dai -> eQ;
	      ap -> hi = dai -> sQ;
	    }

	  if ( (dai -> sQ < dai -> eQ  &&  dai -> sR < dai -> eR)  ||
	       (dai -> sQ > dai -> eQ  &&  dai -> sR > dai -> eR) )
	    {
	      ap -> oriB = FORWARD_CHAR;
	      if ( dai -> sR < dai -> eR )
		{
		  ap -> loB = dai -> sR;
		  ap -> hiB = dai -> eR;
		}
	      else
		{
		  ap -> loB = dai -> eR;
		  ap -> hiB = dai -> sR;
		}
	    }
	  else
	    {
	      ap -> oriB = REVERSE_CHAR;
	      if ( dai -> sR < dai -> eR )
		{
		  ap -> loB = RevComp1 (dai -> eR, dr . getRecord( ) . lenR);
		  ap -> hiB = RevComp1 (dai -> sR, dr . getRecord( ) . lenR);
		}
	      else
		{
		  ap -> loB = RevComp1 (dai -> sR, dr . getRecord( ) . lenR);
		  ap -> hiB = RevComp1 (dai -> eR, dr . getRecord( ) . lenR);
		}
	    }
	}
    }

  dr . close( );
}




//------------------------------------------------------------- PrintOVL -----//
void PrintOVL (const Overlaps_t & overlaps)
{
  list<Overlap_t *>::const_iterator opi;
  Message_t msg;

  for ( opi  = overlaps . ovls . begin( );
	opi != overlaps . ovls . end( ); opi ++ )
    {
      (*opi) -> writeMessage (msg);
      msg . write (cout);
    }
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg  &&
	  ((ch = getopt (argc, argv, "b:hi:t:")) != EOF) )
    switch (ch)
      {
      case 'b':
	OPT_BankName = optarg;
	break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'i':
	OPT_MinIdentity = atof (optarg);
	break;

      case 't':
	OPT_MaxTrimLen = atoi (optarg);
	break;

     default:
        errflg ++;
      }

  if ( OPT_BankName . empty( ) )
    {
      cerr << "ERROR: The -b option is mandatory\n";
      errflg ++;
    }

  if ( errflg > 0 || optind != argc - 1 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  OPT_AlignName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       Set path of the AMOS bank to populate\n"
    << "-h            Display help information\n"
    << "-i float      Set the minimum alignment identity, default "
    << OPT_MinIdentity << endl
    << "-t uint       Set maximum ignorable trim length, default "
    << OPT_MaxTrimLen << endl
    << endl;

  cerr
    << "  Populate overlap store based on the all vs. all alignment\n"
    << "of sequencing reads.\n\n";

  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  -b <bank path>  <deltafile>\n\n";
  return;
}
