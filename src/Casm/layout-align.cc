////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/08/2004
//!
//! \brief Layout the query seqs based on alignment and mate-pair info
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "delta.hh"
#include <algorithm>
#include <cstdlib>
#include <climits>
#include <ctime>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include <unistd.h>
using namespace std;




//=============================================================== Options ====//
string  OPT_BankName;                   // bank name parameter
string  OPT_AlignName;                  // alignment name parameter

bool    OPT_Matepair         = false;   // use matepair information
bool    OPT_PrintConflicts   = false;   // print conflicts
bool    OPT_PrintMaps        = false;   // print read maps
bool    OPT_PrintTigr        = false;   // print tigr contigs
bool    OPT_PrintUMD         = false;   // print UMD contigs
bool    OPT_Random           = false;   // randomly place ambiguous reads
bool    OPT_Singletons       = false;   // keep singletons

int     OPT_MinOverlap       = 10;      // minimum overlap between reads
int     OPT_MaxTrimLen       = 20;      // maximum ignorable trim length
int     OPT_MaxGap           = 10000;   // maximum gap in an alignment chain
int     OPT_Seed             = -1;      // random seed

float   OPT_Majority         = 70.0;    // majority needed to call a conflict
float   OPT_MinCoverage      = 25.0;    // min coverage to tile
float   OPT_MinIdentity      = 70.0;    // min identity to tile
float   OPT_MaxCoverageDiff  =  2.0;    // max %coverage diff between 'best'
float   OPT_MaxIdentityDiff  =  2.0;    // max %identity diff between 'best'


//============================================================= Constants ====//
const char FORWARD_CHAR  = '+';
const char REVERSE_CHAR  = '-';

const int FUZZY      =  10;     // fuzzy equals tolerance

struct ReadMap_t;
struct Contig_t;

struct Conflict_t
{
  // don't swap order, sorts depend on values
  enum
    {
      INDEL,          // both breaks of an indel present
      HIBREAK,        // read breaks early at hi end
      LOBREAK,        // read breaks early at lo end
    };
  enum
    {
      SUPPORTED,      // majority supported conflict
      UNSUPPORTED,    // majority unsupported conflict
      ARTIFACT,       // break is an artifact of a supported indel conflict
      AMBIGUOUS,      // no majority either way
    };

  short int status;                     // include this conflict in the layout
  int type;                             // type of the conflict (enum above)
  long int pos;                         // reference pos of the conflict
  long int gapR, gapQ;                  // gap in the ref and query
  set<ReadMap_t *> support;             // the supporting reads
  set<ReadMap_t *> discount;            // the disagreeing reads

  Conflict_t (int t, long int p, long int gr, long int gq, ReadMap_t * rmp)
  {
    status = UNSUPPORTED;
    type = t;
    pos = p;
    gapR = gr;
    gapQ = gq;
    support . insert (rmp);
  }
};

struct Reference_t
{
  const string * id;                    // points to reference map key
  long int len;                         // length of reference
  list<Conflict_t *> conflicts;         // conflicts on reference

  Reference_t ( ) { id = NULL; }
  ~Reference_t ( )
  {
    list<Conflict_t *>::iterator cpi;
    for ( cpi = conflicts . begin( ); cpi != conflicts . end( ); ++ cpi )
      delete (*cpi);
  }
};

struct Mate_t
{
  ReadMap_t * read;                     // points to read mate or NULL
  int minoff, maxoff;                   // min and max offsets
  AMOS::MateType_t type;
  //////////////// need to implement transposon, etc. ////////////////

  Mate_t ( ) { read = NULL; type = AMOS::Matepair_t::END; }
};

struct ReadAlign_t
{
  Reference_t * ref;                    // ponits to the reference map value
  char ori;                             // orientation of read relative to ref
  long int loR, hiR, lo, hi;            // lo hi in ref, lo hi in read
  float idy;                            // identity of alignment
  ReadAlign_t * from;                   // linking for chained alignments

  ReadAlign_t ( ) { ref = NULL; from = NULL; }
};

struct ReadAlignChain_t
{
  ReadAlign_t * head;         // points to head of alignment chain

  long int beg, end;          // begin and end map points in the reference
  long int tbeg, tend;        // lo and hi trim points
  long int len;               // read chain length (sum of align lens)
  float idy;                  // read weighted avg identity

  ReadAlignChain_t ( ) { head = NULL; }
  ReadAlignChain_t (const ReadMap_t * rmp, ReadAlign_t * rap);
};

struct ReadMap_t
{
  AMOS::ID_t id;                        // ID of read
  long int len;                         // len of read

  vector<ReadAlign_t *> all;            // all alignments for this read
  list<ReadAlignChain_t *> best;        // list of best alignment chains
  ReadAlignChain_t * place;             // read placement chain

  Mate_t mate;                          // the read's mate

  bool exclude;                         // exclude the read from the layout

  ReadMap_t ( ) { place = NULL; exclude = false; }
  ~ReadMap_t ( )
  {
    vector<ReadAlign_t *>::iterator rapi;
    for ( rapi = all . begin( ); rapi != all . end( ); ++ rapi )
      delete (*rapi);
    list<ReadAlignChain_t *>::iterator rcpi;
    for ( rcpi = best . begin( ); rcpi != best . end( ); ++ rcpi )
      delete (*rcpi);
  }
};

struct Mapping_t
{
  map<string, Reference_t> references;  // map of references
  vector<ReadMap_t *> reads;            // vector of reads

  ~Mapping_t ( )
  {
    vector<ReadMap_t *>::iterator rmpi;
    for ( rmpi = reads . begin( ); rmpi != reads . end( ); ++ rmpi )
      delete (*rmpi);
  }
};

struct Tile_t
{
  ReadMap_t * read;
  long int off;
  long int beg, end;

  Tile_t ( ) { read = NULL; }
  Tile_t (ReadMap_t * rmp, Contig_t * cp);
};

struct Contig_t
{
  vector<Tile_t *> tiles;
  long int len;
  long int adjust;

  Contig_t ( ) { len = adjust = 0; }
  ~Contig_t ( )
  {
    vector<Tile_t *>::iterator tpi;
    for ( tpi = tiles . begin( ); tpi != tiles . end( ); ++ tpi )
      delete (*tpi);
  }
};

struct Assembly_t
{
  vector<Contig_t *> contigs;

  ~Assembly_t ( )
  {
    vector<Contig_t *>::iterator cpi;
    for ( cpi = contigs . begin( ); cpi != contigs . end( ); ++ cpi )
      delete (*cpi);
  }
};


struct ConflictCmp_t
{
  bool operator( ) (const Conflict_t * A, const Conflict_t * B)
  {
    if ( A -> pos < B -> pos )
      return true;
    else if ( A -> pos > B -> pos )
      return false;

    else if ( A -> type < B -> type )
      return true;
    else if ( A -> type > B -> type )
      return false;

    else if ( A -> gapR < B -> gapR )
      return true;
    else if ( A -> gapR > B -> gapR )
      return false;

    else if ( A -> gapQ < B -> gapQ )
      return true;
    else
      return false;
  }
};

struct ReadAlignCmp_t
{
  bool operator( ) (const ReadAlign_t * A, const ReadAlign_t * B)
  { return ( A -> lo < B -> lo ); }
};

struct ReadHeapCmp_t
{
  bool operator( ) (const ReadMap_t * A, const ReadMap_t * B)
  { return ( A -> place -> end > B -> place -> end ); }
};

struct ReadIdCmp_t
{
  bool operator( ) (const ReadMap_t * A, const ReadMap_t * B)
  { return ( A -> id < B -> id ); }
};

struct ReadPlaceCmp_t
{
  bool operator( ) (const ReadMap_t * A, const ReadMap_t * B)
  {
    if ( A -> place == NULL  ||  B -> place == NULL )
      {
	if ( A -> place == NULL  &&  B -> place != NULL )
	  return true;
	else
	  return false;
      }

    if ( A -> place -> head -> ref < B -> place -> head -> ref )
      return true;
    else if ( A -> place -> head -> ref > B -> place -> head -> ref )
      return false;

    if ( A -> place -> tbeg < B -> place -> tbeg )
      return true;
    else
      return false;
  }
};

struct ReadReferenceCmp_t
{
  bool operator( ) (const ReadMap_t * A, const ReadMap_t * B)
  {
    if ( A -> place == NULL  ||  B -> place == NULL )
      {
	if ( A -> place == NULL  &&  B -> place != NULL )
	  return true;
	else
	  return false;
      }

    if ( A -> place -> head -> ref < B -> place -> head -> ref )
      return true;
    else
      return false;
  }
};

struct TileCmp_t
{
  bool operator( ) (const Tile_t * A, const Tile_t * B)
  { return ( A -> off < B -> off ); }
};

struct ReadExcludePredicate_t
{
  bool operator( ) (const ReadMap_t * A)
  { return ( A -> exclude ); }
};

struct ConflictExcludePredicate_t
{
  bool operator( ) (const Conflict_t * A)
  { return ( A -> status != Conflict_t::SUPPORTED  &&
	     A -> status != Conflict_t::AMBIGUOUS); }
};



//========================================================== Fuction Decs ====//
//-------------------------------------------------------------- Assemble ----//
//! \brief Construct the assembly
//!
//! Construct the read layout by throwing out disagreeing reads and adjusting
//! layout offsets for reference differences (i.e. large indels).
//!
//! \param mapping The read mapping
//! \param assembly The assembly contigs
//! \pre RefineConflicts has been called
//! \return void
//!
void Assemble (Mapping_t & mapping, Assembly_t & assembly);


//----------------------------------------------------------- ChainAligns ----//
//! \brief Run LAS and store the best alignment chains for each read
//!
//! \param mapping The read mapping
//! \return void
//!
void ChainAligns (Mapping_t & mapping);


//---------------------------------------------------------- CleanContigs ----//
//! \breif Clean contigs
//!
//! Sort by offset. Get rid of negative offsets. Get rid of zero coverage.
//! Sets length.
//!
//! \param assembly The assembly contigs
//! \return void
//!
void CleanContigs (Assembly_t & assembly);


//--------------------------------------------------------- FindConflicts ----//
//! \brief Find and store conflicts between the reads and the refs
//!
//! \param mapping The read mapping
//! \return void
//!
void FindConflicts (Mapping_t & mapping);


//------------------------------------------------------------ ParseAlign ----//
//! \brief Parse, sort and store the alignment input
//!
//! \param mapping The uninitialized read mapping
//! \post All reads sorted by read ID
//! \post All alignments are sorted by lo read coordinate
//! \post All mate records are NULL
//! \return void
//!
void ParseAlign (Mapping_t & mapping);


//------------------------------------------------------------ ParseMates ----//
//! \brief Parse, sort and store the matepair input
//!
//! \param mates The read-initialized mapping
//! \pre mapping is sorted by read ID
//! \post All mate records in reads are populated
//! \return void
//!
void ParseMates (Mapping_t & mapping);


//------------------------------------------------------- PlaceHappyMates ----//
//! \brief Place all unplaced read matepairs while keeping them 'happy'
//!
//! If a matepair can be happy at more than one spot, it will not be placed,
//! unless the OPT_Random option is turned on, then it will be placed at
//! random into one of it's happy spots.
//!
//! \param mapping The read mapping
//! \return void
//!
void PlaceHappyMates (Mapping_t & mapping);


//----------------------------------------------------------- PlaceRandom ----//
//! \brief Randomly place all unplaced reads
//!
//! \param mapping The read mapping
//! \return void
//!
void PlaceRandom (Mapping_t & mapping);


//------------------------------------------------------ PlaceUnambiguous ----//
//! \brief Place all unplaced reads with unambiguous mappings
//!
//! \param mapping The read mapping with everything populated except placements
//! \return void
//!
void PlaceUnambiguous (Mapping_t & mapping);


//-------------------------------------------------------- PrintConflicts ----//
//! \brief Prints all of the reference conflicts
//!
//! \param mapping The read mapping with conflicts generated
//! \return void
//!
void PrintConflicts (const Mapping_t & mapping);


//---------------------------------------------------------- PrintMapping ----//
//! \brief Prints all the read mappings to stdout
//!
//! \param mapping The read mapping
//! \pre read list is sorted by place
//! \return void
//!
void PrintMapping (Mapping_t & mapping);


//------------------------------------------------------------- PrintTigr ----//
//! \brief Prints the assembly in TIGR CTG format to cout
//!
//! \param assembly The assembly
//! \return void
//!
void PrintTigr (const Assembly_t & assembly);


//------------------------------------------------------------- PrintUMD -----//
//! \brief Prints the assembly in UMD CTG format to cout
//!
//! \param assembly The assembly
//! \return void
//!
void PrintUMD (const Assembly_t & assembly);


//------------------------------------------------------- RefineConflicts ----//
//! \brief Count discounting reads, combine and select conflicts
//!
//! Leaves support and discount vectors sorted by ReadMap *
//!
//! \param mapping The read mapping
//! \pre FindConflicts has been called
//! \return void
//!
void RefineConflicts (Mapping_t & mapping);


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
//------------------------------------------------------------------ lmin ----//
inline long int lmin (long int a, long int b)
{ return (a < b ? a : b); }


//------------------------------------------------------------------ lmax ----//
inline long int lmax (long int a, long int b)
{ return (a < b ? b : a); }


//---------------------------------------------------------- CallConflict ----//
inline void CallConflict (Conflict_t * c)
{
  long int yay = c -> support  . size( );
  long int nay = c -> discount . size( );
  float tot = yay + nay;
  if ( tot == 0 ) tot = -1;

  if ( (float)yay / tot * 100.0 >= OPT_Majority )
    c -> status = Conflict_t::SUPPORTED;
  else if ( (float)nay / tot * 100.0 >= OPT_Majority )
    c -> status = Conflict_t::UNSUPPORTED;
  else
    c -> status = Conflict_t::AMBIGUOUS;
}


//----------------------------------------------------------- GapDistance ----//
pair<long int, long int> GapDistance (ReadAlign_t * loa, ReadAlign_t * hia)
{
  long int gapR = hia -> loR - loa -> hiR - 1;
  return make_pair ( - gapR < hia -> ref -> len / 2 ? gapR :
		     (hia -> loR - 1) + (hia -> ref -> len - loa -> hiR),
		     hia -> lo  - loa -> hi  - 1 );
}


//--------------------------------------------------------- IsBetterChain ----//
inline bool IsBetterChain (const ReadAlignChain_t * cand,
			   const ReadAlignChain_t * best,
			   const ReadMap_t * read)
{  
  if ( (float)labs(best -> len - cand -> len) /
       (float)(read -> len) * 100.0 > OPT_MaxCoverageDiff )
    return cand -> len > best -> len;
  else
    {
      if ( fabs(best -> idy - cand -> idy) > OPT_MaxIdentityDiff )
	return cand -> idy > best -> idy;
      else
	{
	  if ( cand -> idy <= best -> idy  &&  cand -> len <= best -> len )
	    return false;
	  if ( cand -> idy >= best -> idy  &&  cand -> len >= best -> len )
	    return true;
	  if ( cand -> head -> from == NULL  &&  best -> head -> from != NULL )
	    return true;
	  if ( cand -> head -> from != NULL  &&  best -> head -> from == NULL )
	    return false;
	}
    }

  return cand -> len > best -> len;
}


//---------------------------------------------------------- IsEqualChain ----//
inline bool IsEqualChain (const ReadAlignChain_t * cand,
			  const ReadAlignChain_t * best,
			  const ReadMap_t * read)
{
  return ((float)(best -> len - cand -> len) /
	  (float)(read -> len) * 100.0 <= OPT_MaxCoverageDiff  &&
	  best -> idy - cand -> idy <= OPT_MaxIdentityDiff);
}


//------------------------------------------------------- IsEqualConflict ----//
inline bool IsEqualConflict (const Conflict_t * A, const Conflict_t * B)
{
  return ( A -> type == B -> type  &&
	   labs (A -> pos - B -> pos) <= FUZZY  &&
	   labs (A -> gapR - B -> gapR) <= FUZZY  &&
	   labs (A -> gapQ - B -> gapQ) <= FUZZY );
}


//---------------------------------------------------------- IsValidChain ----//
inline bool IsValidChain(const ReadAlignChain_t * cand,
			 const ReadMap_t * read)
{
  return ((float)cand->len / (float)read->len * 100.0 >= OPT_MinCoverage  &&
	  cand -> idy >= OPT_MinIdentity);
}


//-------------------------------------------------------------- PushHeap ----//
inline void PushHeap (vector<ReadMap_t *> & heap, ReadMap_t * rmp)
{
  heap . push_back (rmp);
  push_heap (heap . begin( ), heap . end( ), ReadHeapCmp_t( ));
}


//--------------------------------------------------------------- PopHeap ----//
inline void PopHeap (vector<ReadMap_t *> & heap)
{
  pop_heap (heap . begin( ), heap . end( ), ReadHeapCmp_t( ));
  heap . pop_back( );
}




//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  Mapping_t mapping;
  Assembly_t assembly;

  //-- COMMAND
  ParseArgs (argc, argv);          // parse the command line arguments
  srand (OPT_Seed);

  //-- INPUT
  cerr << "INPUT" << endl;
  cerr << " reads" << endl;
  ParseAlign (mapping);            // parse the alignment data
  cerr << " lis" << endl;
  ChainAligns (mapping);           // build the LAS chains for each read
  if ( OPT_Matepair )
    {
      cerr << " mates" << endl;
      ParseMates (mapping);          // parse the matepair data
    }

  //-- READ PLACEMENT
  cerr << "PLACEMENT" << endl;
  PlaceUnambiguous (mapping);      // place unambiguous reads
  if ( OPT_Matepair )
    PlaceHappyMates (mapping);     // place 'happy' matepairs
  if ( OPT_Random )
    PlaceRandom (mapping);         // randomly place all unplaced reads

  sort (mapping.reads.begin( ), mapping.reads.end( ), ReadPlaceCmp_t( ));

  //-- CONFLICT DETECTION
  cerr << "CONFLICT" << endl;
  FindConflicts (mapping);         // find conflicts btwn reads and refs
  RefineConflicts (mapping);       // assess, combine and select conflicts

  //-- ASSEMBLY
  if ( OPT_PrintTigr  ||  OPT_PrintUMD )
    {
      cerr << "ASSEMBLY" << endl;
      Assemble (mapping, assembly);// layout the reads to form the assembly
      CleanContigs (assembly);     // clean up the contigs
    }

  //-- OUTPUT
  cerr << "OUTPUT" << endl;
  if ( OPT_PrintConflicts )
    PrintConflicts (mapping);      // print conflicts

  if ( OPT_PrintMaps )
    PrintMapping (mapping);        // print read mapping

  if ( OPT_PrintTigr )
    PrintTigr (assembly);          // print TIGR contigs
  
  if ( OPT_PrintUMD )
    PrintUMD (assembly);           // print UMD contigs

  return EXIT_SUCCESS;
}




//-------------------------------------------------------------- Assemble ----//
void Assemble (Mapping_t & mapping, Assembly_t & assembly)
{
  ReadMap_t temprm;
  temprm . place = new ReadAlignChain_t( );
  temprm . place -> head = new ReadAlign_t( );

  Reference_t * rp;
  Contig_t * cp;
  Tile_t * tp;

  map<string, Reference_t>::iterator rmi;
  list<ReadMap_t *>::iterator rmpi, rmpic;
  list<Conflict_t *>::iterator cpi;

  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;

  long int confbeg, confend;


  //-- For each reference
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); ++ rmi )
    {
      rp = &(rmi -> second);

      //-- Find the read mapping range
      temprm . place -> head -> ref = rp;
      rmpip = equal_range (mapping . reads . begin( ), mapping . reads . end( ),
			   &temprm, ReadReferenceCmp_t( ));

      //-- For all reads mapping to the reference
      list<ReadMap_t *> reads (rmpip.first, rmpip.second);
      reads . remove_if (ReadExcludePredicate_t( ));

      //-- For all non-excluded conflicts to this reference
      list<Conflict_t *> confs (rp->conflicts.begin( ), rp->conflicts.end( ));
      confs . remove_if (ConflictExcludePredicate_t( ));


      //-- Keep going until all reads have been assembled
      cp = NULL;
      rmpi = reads . end( );
      while ( ! reads . empty( ) )
	{
	  //-- If end of list reached, push a new contig and start over
	  if ( rmpi == reads . end( ) )
	    {
	      if ( cp == NULL  ||  !cp -> tiles . empty( ) )
		{
		  cp = new Contig_t( );
		  assembly . contigs . push_back (cp);
		}
	      rmpi = reads . begin( );
	    }


	  //-- Jump to the next conflict
	  for ( cpi  = confs . begin( ); cpi != confs . end( ); ++ cpi )
	    if ( (*cpi) -> pos + FUZZY > (*rmpi) -> place -> tbeg )
	      {
		confbeg = (*cpi) -> pos;
		confend = (*cpi) -> gapR == 0
		  ? confbeg : confbeg + (*cpi) -> gapR + 1;
		break;
	      }


	  //-- Assemble reads not involved in the conflict
	  while ( rmpi != reads . end( )  &&
		  (cpi == confs . end( )  ||
		   (*rmpi) -> place -> beg < lmax (confbeg, confend)) )
	    {
	      //-- Assemble pre-conflict reads
	      if ( cpi == confs . end( )  ||
		   (*rmpi) -> place -> end <= confbeg + OPT_MaxTrimLen )
		{
		  tp = new Tile_t (*rmpi, cp);
		  
		  //-- ASSEMBLE the read
		  cp -> tiles . push_back (tp);
		  rmpi = reads . erase (rmpi);
		}
	      //-- Skip involved reads
	      else
		++ rmpi;
	    }


	  //-- If no conflict to process, continue
	  if ( cpi == confs . end( ) )
	    continue;


	  //-- If SUPPORTED, process the conflict
	  if ( (*cpi) -> status == Conflict_t::SUPPORTED )
	    {
	      //-- If LOBREAK, push a new contig
	      if ( (*cpi) -> type == Conflict_t::LOBREAK  &&
		   !cp -> tiles . empty( ) )
		{
		  cp = new Contig_t( );
		  assembly . contigs . push_back (cp);
		}

	      //-- Assemble reads overlapping the conflict
	      for ( rmpic = reads . begin( ); rmpic != rmpi; ) 
		{
		  //-- If a supporting read
		  if ( (*cpi)->support.find(*rmpic) != (*cpi)->support.end( ) )
		    {
		      tp = new Tile_t (*rmpic, cp);

		      //-- If LOBREAK on an INDEL, need to adjust the offset
		      if ( (*cpi) -> type == Conflict_t::INDEL  &&
			   (*rmpic) -> place -> tbeg >= confend - FUZZY  &&
			   (*rmpic) -> place -> tbeg -
			   (*rmpic) -> place -> beg > OPT_MaxTrimLen )
			tp -> off += (*cpi) -> gapQ - (*cpi) -> gapR;

		      //-- ASSEMBLE the read
		      cp -> tiles . push_back (tp);
		      rmpic = reads . erase (rmpic);
		    }
		  else
		    ++ rmpic;
		}

	      //-- Conflict post-processing
	      switch ( (*cpi) -> type )
		{
		case Conflict_t::LOBREAK:       // do nothing
		  break;
		case Conflict_t::HIBREAK:       // new contig
		  rmpi = reads . end( );
		  break;
		case Conflict_t::INDEL:         // adjust for reference gap
		  cp -> adjust += (*cpi) -> gapQ - (*cpi) -> gapR;
		  break;
		}
	    }
	  //-- If AMBIGUOUS, push a new contig if necessary
	  else if ( !cp -> tiles . empty( ) )
	    {
	      cp = new Contig_t( );
	      assembly . contigs . push_back (cp);
	    }


	  //-- Back up to end of resovled conflict
	  while ( rmpi != reads . begin( ) )
	    {
	      -- rmpi;
	      if ( (*rmpi) -> place -> beg < confend - OPT_MaxTrimLen )
		{
		  ++ rmpi;
		  break;
		}
	    }


	  //-- Resovled the conflict, remove it
	  cpi = confs . erase (cpi);
	}
    }

  delete temprm . place -> head;
  delete temprm . place;
}




//----------------------------------------------------------- ChainAligns ----//
void ChainAligns (Mapping_t & mapping)
{
  struct ScoreLAS
  {
    ReadAlign_t * a;
    long int score;
    long int from;
  };

  ScoreLAS * las = NULL;

  bool olapflag;
  long int i, j, n, best;
  long int olap1, olap2, olap, len;

  vector<ReadMap_t *>::iterator rmpi;
  list<ReadAlignChain_t *>::iterator rcpi;
  ReadAlignChain_t * bestchain, * currchain;

  //-- For each read in the mapping
  for ( rmpi  = mapping . reads . begin( );
	rmpi != mapping . reads . end( ); ++ rmpi )
    {
      //-- Initialize the dynamic programming matrix
      n = (*rmpi) -> all . size( );
      las = (ScoreLAS *) AMOS::SafeRealloc (las, sizeof (ScoreLAS) * n);
      for ( i = 0; i < n; i ++ )
	{
	  las [i] . a = (*rmpi) -> all[i];
	  las [i] . score = las [i] . a -> hi - las [i] . a -> lo + 1;
	  las [i] . from = -1;
	}

      //-- Isn't it dynamic?
      for ( i = 0; i < n; i ++ )
	for ( j = 0; j < i; j ++ )
	  {
	    if ( las [i] . a -> ref != las [j] . a -> ref  ||
		 las [i] . a -> ori != las [j] . a -> ori )
	      continue;

	    olap1 = las [j] . a -> hiR - las [i] . a -> loR + 1;
	    if ( OPT_MaxGap >= 0  &&  olap1 < -(OPT_MaxGap) )
	      continue;
	    olap = olap1 > 0 ? olap1 : 0;

	    olap2 = las [j] . a -> hi - las [i] . a -> lo + 1;
	    if ( OPT_MaxGap >= 0  &&  olap2 < -(OPT_MaxGap) )
	      continue;
	    olap = olap > olap2 ? olap : olap2;

	    len = las [i] . a -> hi - las [i] . a -> lo + 1;
	    if ( las [j] . score + len - olap > las [i] . score )
	      {
		las [i] . from = j;
		las [i] . score = las [j] . score + len - olap;
	      }
	  }

      //-- Store all the non-redundant chains and store sorted by score
      bestchain = NULL;
      while (true)
	{
	  best = 0;
	  for ( i = 1; i < n; i ++ )
	    if ( las [i] . score > las [best] . score )
	      best = i;
	  if ( las [best] . score <= 0 )
	    break;

	  olapflag = false;
	  for ( i = best; las [i] . from >= 0; i = las [i] . from )
	    {
	      if ( las [i] . score == 0 )
		olapflag = true;
	      las [i] . score = 0;
	      las [i] . a -> from = las [las [i] . from] . a;
	    }
	  if ( las [i] . score == 0 )
	    olapflag = true;
	  las [i] . score = 0;
	  las [i] . a -> from = NULL;

	  if ( !olapflag )
	    {
	      currchain = new ReadAlignChain_t (*rmpi, las [best] . a);
	      (*rmpi) -> best . push_back (currchain);
	      if ( bestchain == NULL )
		bestchain = currchain;
	      else if ( IsBetterChain (currchain, bestchain, *rmpi) )
		bestchain = currchain;
	    }
	}

      //-- Keep only the 'best' chains, thus best . size > 0 == ambiguity
      //   i.e. within OPT_MaxCoverageDiff of the longest and within
      //   OPT_MaxIdentityDiff of the longest (with the higest idy)
      rcpi = (*rmpi) -> best . begin( );
      while ( rcpi != (*rmpi) -> best . end( ) )
	{
	  if ( IsEqualChain (*rcpi, bestchain, *rmpi) )
	    ++ rcpi;
	  else
	    {
	      delete (*rcpi);
	      rcpi = (*rmpi) -> best . erase (rcpi);
	    }
	}
    }

  free (las);
}




//---------------------------------------------------------- CleanContigs ----//
void CleanContigs (Assembly_t & assembly)
{
  vector<Contig_t *>::iterator cpi;
  vector<Tile_t *>::iterator tpi;
  Contig_t * cp;
  long int adj, end;

  vector<Contig_t *> contigs;

  for ( cpi  = assembly . contigs . begin( );
	cpi != assembly . contigs . end( ); ++ cpi )
    {
      //-- Sort the tiled reads
      sort ((*cpi) -> tiles . begin( ), (*cpi) -> tiles . end( ), TileCmp_t( ));

      //-- Write new clean contigs
      cp = NULL;
      for ( tpi = (*cpi)->tiles.begin( ); tpi != (*cpi)->tiles.end( ); ++ tpi )
	{
	  //-- If a new contig or a zero coverage gap
	  if ( cp == NULL || (*tpi)->off - adj > cp->len - OPT_MinOverlap )
	    {
	      cp = new Contig_t( );
	      cp -> adjust = (*cpi) -> adjust;
	      contigs . push_back (cp);

	      adj = (*tpi) -> off;
	    }

	  (*tpi) -> off -= adj;
	  cp -> tiles . push_back (*tpi);
	  end = (*tpi) -> off + labs ((*tpi) -> end - (*tpi) -> beg) + 1;
	  if ( end > cp -> len )
	    cp -> len = end;
	}
    }

  //-- Use the new clean contigs
  assembly . contigs . clear( );
  for ( cpi = contigs . begin( ); cpi != contigs . end( ); ++ cpi )
    {
      if ( (*cpi) -> tiles . size( ) <= 1 )
	if ( (*cpi) -> tiles . size( ) == 0  ||  ! OPT_Singletons )
	  continue;
      assembly . contigs . push_back (*cpi);
    }
}




//--------------------------------------------------------- FindConflicts ----//
void FindConflicts (Mapping_t & mapping)
{
  ReadAlign_t * curraln;
  ReadMap_t * currmap;
  pair<long int, long int> gap;
  vector<ReadMap_t *>::iterator rmpi;
  set<ReadMap_t *> yay;


  //-- For each read
  for ( rmpi  = mapping . reads . begin( );
	rmpi != mapping . reads . end( ); ++ rmpi )
    {
      if ( (*rmpi) -> place == NULL )
	continue;

      currmap = *rmpi;
      curraln = currmap -> place -> head;

      //-- If there is a HIBREAK
      if ( currmap -> place -> end - currmap -> place -> tend > OPT_MaxTrimLen )
	curraln -> ref -> conflicts . push_back
	  (new Conflict_t (Conflict_t::HIBREAK, curraln -> hiR, 0, 0, currmap));
      
      for ( ; curraln -> from != NULL; curraln = curraln -> from )
	{
	  gap = GapDistance (curraln -> from, curraln);
	  //-- If there is an INDEL, push it and it's break points
	  if ( labs (gap . first) > FUZZY  ||  labs (gap . second) > FUZZY )
	    {
	      curraln -> ref -> conflicts . push_back
		(new Conflict_t (Conflict_t::INDEL,
				 curraln -> from -> hiR,
				 gap . first, gap . second, currmap));
	      curraln -> ref -> conflicts . push_back
		(new Conflict_t (Conflict_t::HIBREAK,
				 curraln -> from -> hiR,
				 0, 0, currmap));
	      curraln -> ref -> conflicts . push_back
		(new Conflict_t (Conflict_t::LOBREAK,
				 curraln -> from -> hiR + gap . first + 1,
				 0, 0, currmap));
	    }
	}

      //-- If there is a LOBREAK
      if ( currmap -> place -> tbeg - currmap -> place -> beg > OPT_MaxTrimLen )
	curraln -> ref -> conflicts . push_back
	  (new Conflict_t (Conflict_t::LOBREAK, curraln -> loR, 0, 0, currmap));
    }

  //-- For each reference, sort and remove redundant conflicts
  map<string, Reference_t>::iterator rmi;
  list<Conflict_t *>::iterator first, last, next, nxtf;
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); ++ rmi )
    {
      if ( rmi -> second . conflicts . empty( ) )
	continue;

      rmi -> second . conflicts . sort (ConflictCmp_t( ));

      //-- Do my own 'unique' method instead of STL, needed for fuzzy equals
      first = next = rmi -> second . conflicts . begin( );
      last  = nxtf = rmi -> second . conflicts . end( );
      while ( ++ next != last )
	{
	  if ( IsEqualConflict (*first, *next) )
	    {
	      //-- Merge the two support sets
	      set_union
		((*first) -> support . begin( ), (*first) -> support . end( ),
		 (*next)  -> support . begin( ), (*next)  -> support . end( ),
		 insert_iterator<set<ReadMap_t *> > (yay, yay . begin( )));
	      (*first) -> support . swap (yay);
	      yay . clear( );

	      delete (*next);
	      rmi -> second . conflicts . erase (next);
	      next = first;
	    }
	  else
	    {
	      if ( labs ((*first) -> pos - (*next) -> pos) <= FUZZY )
		{
		  if ( nxtf == last )
		    nxtf = next;
		}
	      else
		{
		  if ( nxtf != last )
		    {
		      next = nxtf;
		      nxtf = last;
		    }

		  first = next;
		}
	    }
	}
    }
}




//------------------------------------------------------------ ParseAlign ----//
void ParseAlign (Mapping_t & mapping)
{
  DeltaReader_t dr;
  Reference_t * refp;
  AMOS::ID_t id;
  stringstream ss;
  ReadMap_t * currmp;
  ReadAlign_t * currap;
  vector<DeltaAlignment_t>::const_iterator dai;
  pair<map<string, Reference_t>::iterator, bool> insret;

  dr . open (OPT_AlignName);

  currmp = NULL;

  //-- Process the delta input
  while ( dr . readNextHeadersOnly( ) )
    {
      insret = mapping . references . insert
	(map<string, Reference_t>::value_type
	 (dr . getRecord( ) . idR, Reference_t( )));
      refp = &((insret . first) -> second);

      //-- If a new reference ID
      if ( insret . second )
	{
	  refp -> id  = &((insret . first) -> first);
	  refp -> len = dr . getRecord( ) . lenR;
	}
      else
	assert (refp -> len == (long int)dr . getRecord( ) . lenR);

      ss . str (dr . getRecord( ) . idQ);
      ss >> id;
      assert (!ss . fail( ));
      ss . clear( );

      //-- If a new read, push it
      if ( currmp == NULL || id != currmp -> id )
	{
	  mapping . reads . push_back (new ReadMap_t( ));
	  currmp = mapping . reads . back( );
	  currmp -> id = id;
	  currmp -> len = dr . getRecord( ) . lenQ;
	  currmp -> place = NULL;
	  currmp -> mate . read = NULL;
	}

      //-- For all the alignments in this record
      for ( dai  = dr . getRecord( ) . aligns . begin( );
	    dai != dr . getRecord( ) . aligns . end( ); ++ dai )
	{
	  currmp -> all . push_back (new ReadAlign_t ( ));
	  currap = currmp -> all . back( );

	  currap -> ref = refp;
	  currap -> idy = dai -> idy;

	  //-- Force ascending coordinates
	  if ( dai -> sR < dai -> eR )
	    {
	      currap -> loR = dai -> sR;
	      currap -> hiR = dai -> eR;
	    }
	  else
	    {
	      currap -> loR = dai -> eR;
	      currap -> hiR = dai -> sR;
	    }

	  if ( (dai -> sR < dai -> eR  &&  dai -> sQ < dai -> eQ)  ||
	       (dai -> sR > dai -> eR  &&  dai -> sQ > dai -> eQ) )
	    {
	      currap -> ori = FORWARD_CHAR;
	      if ( dai -> sQ < dai -> eQ )
		{
		  currap -> lo = dai -> sQ;
		  currap -> hi = dai -> eQ;
		}
	      else
		{
		  currap -> lo = dai -> eQ;
		  currap -> hi = dai -> sQ;
		}
	    }
	  else
	    {
	      currap -> ori = REVERSE_CHAR;
	      if ( dai -> sQ < dai -> eQ )
		{
		  currap -> lo = RevComp1 (dai -> eQ, currmp -> len);
		  currap -> hi = RevComp1 (dai -> sQ, currmp -> len);
		}
	      else
		{
		  currap -> lo = RevComp1 (dai -> sQ, currmp -> len);
		  currap -> hi = RevComp1 (dai -> eQ, currmp -> len);
		}
	    }
	}

      //-- Sort the alignments by lo read coordinate
      sort (currmp -> all.begin( ), currmp -> all.end( ), ReadAlignCmp_t( ));
    }

  dr . close( );

  sort (mapping . reads . begin( ), mapping . reads . end( ), ReadIdCmp_t( ));
}




//------------------------------------------------------------ ParseMates ----//
void ParseMates (Mapping_t & mapping)
{
  AMOS::Read_t red;
  AMOS::Fragment_t frg;
  AMOS::Library_t lib;
  AMOS::Matepair_t mtp;

  AMOS::Bank_t red_bank (AMOS::Read_t::NCODE);
  AMOS::Bank_t frg_bank (AMOS::Fragment_t::NCODE);
  AMOS::Bank_t lib_bank (AMOS::Library_t::NCODE);
  AMOS::BankStream_t mtp_bank (AMOS::Matepair_t::NCODE);

  ReadMap_t * rmp, temprm;
  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;

  try {
    mtp_bank . open (OPT_BankName, B_READ);
    red_bank . open (OPT_BankName, B_READ);
    frg_bank . open (OPT_BankName, B_READ);
    lib_bank . open (OPT_BankName, B_READ);

    //-- Populate the mate records
    while ( mtp_bank >> mtp )
      {
	//-- Find the read
	temprm . id = mtp . getReads( ) . first;
	rmpip = equal_range (mapping . reads . begin( ),
			     mapping . reads . end( ),
			     &temprm, ReadIdCmp_t( ));

	//-- If 0 or >1 reads, or already mated, skip
	if ( rmpip . second - rmpip . first != 1  ||
	     (*(rmpip . first)) -> mate . read != NULL )
	  continue;
	rmp = *(rmpip . first);

	//-- Find the mate
	temprm . id = mtp . getReads( ) . second;
	rmpip = equal_range (mapping . reads . begin( ),
			     mapping . reads . end( ),
			     &temprm, ReadIdCmp_t( ));

	//-- If 0 or >1 mates
	if ( rmpip . second - rmpip . first != 1 )
	  continue;
	assert ( (*(rmpip . first)) -> mate . read == NULL );

	//- Link the mated reads
	rmp -> mate . read = *(rmpip . first);
	rmp -> mate . read -> mate . read = rmp;

	//-- Get the library record for this insert
	red_bank . fetch (rmp -> id, red);
	frg_bank . fetch (red . getFragment( ), frg);
	lib_bank . fetch (frg . getLibrary( ), lib);

	//-- Set the mate offsets
	rmp -> mate . minoff = rmp -> mate . maxoff =
	  lib . getDistribution( ) . mean;
	rmp -> mate . minoff -= lib . getDistribution( ) . sd * 3;
	rmp -> mate . maxoff += lib . getDistribution( ) . sd * 3;
	rmp -> mate . read -> mate . minoff = rmp -> mate . minoff;
	rmp -> mate . read -> mate . maxoff = rmp -> mate . maxoff;
      }
  }
  catch (AMOS::Exception_t & e) {

    //-- Couldn't open banks, or find reads
    cerr << "WARNING: Could not parse mate-pair information from bank\n" << e;
  }

  mtp_bank . close( );
  red_bank . close( );
  frg_bank . close( );
  lib_bank . close( );
}




//------------------------------------------------------- PlaceHappyMates ----//
void PlaceHappyMates (Mapping_t & mapping)
{
  ReadMap_t * rmp, * mrmp;
  ReadAlignChain_t * rcp, * mrcp;
  int i;
  vector< pair<ReadAlignChain_t *, ReadAlignChain_t *> > places;
  vector<ReadMap_t *>::iterator rmpi;
  list<ReadAlignChain_t *>::iterator rcpi, mrcpi;

  //-- For each read in the mapping
  for ( rmpi  = mapping . reads . begin( );
	rmpi != mapping . reads . end( ); ++ rmpi )
    {
      rmp = *rmpi;

      //-- Get the matepair
      mrmp = rmp -> mate . read;

      //-- If the read has already been placed, or no mate, skip
      if ( rmp -> place != NULL  ||  mrmp == NULL )
	continue;

      //-- Find all the happy placements
      places . clear( );
      for ( mrcpi  = mrmp -> best . begin( );
	    mrcpi != mrmp -> best . end( ); ++ mrcpi )
	{
	  //-- If mate not placed, search through all its best
	  if ( mrmp -> place == NULL )
	    mrcp = *mrcpi;
	  else
	    {
	      mrcp = mrmp -> place;
	      mrcpi = mrmp -> best . end( );
	      -- mrcpi;                        // last loop iteration
	    }

	  for ( rcpi  = rmp -> best . begin( );
		rcpi != rmp -> best . end( ); ++ rcpi )	    
	    {
	      rcp = *rcpi;

	      //-- Skip if constraints are not satisfied
	      if ( rcp -> head -> ref != mrcp -> head -> ref )
		continue;
	      if ( mrcp -> head -> ori == FORWARD_CHAR )
		{
		  if (rcp -> head -> ori != REVERSE_CHAR
		      ||
		      rcp  -> end < mrcp -> beg + mrmp -> mate . minoff
		      ||
		      rcp  -> end > mrcp -> beg + mrmp -> mate . maxoff )
		    continue;
		}
	      else
		{
		  if (rcp -> head -> ori != FORWARD_CHAR
		      ||
		      rcp  -> beg < mrcp -> end - mrmp -> mate . maxoff
		      ||
		      rcp  -> beg > mrcp -> end - mrmp -> mate . minoff )
		    continue;
		}

	      //-- Happy link! Add the matepair if valid
	      if ( IsValidChain (rcp, rmp)  &&  IsValidChain (mrcp, mrmp) )
		places . push_back (make_pair (rcp, mrcp));
	    }
	}

      //-- If 1 happy link was found, place the matepair
      //   or many happy links and OPT_Random, randomly place the matepair
      if ( !places . empty( )
	   &&
	   (OPT_Random  ||  places . size( ) == 1) )
	{
	  i = (int)((double)places . size( ) * rand( ) / (RAND_MAX + 1.0));
	  rmp  -> place = places [i] . first;
	  mrmp -> place = places [i] . second;
	}
    }
}




//----------------------------------------------------------- PlaceRandom ----//
void PlaceRandom (Mapping_t & mapping)
{
  ReadMap_t * rmp;
  vector<ReadAlignChain_t *> places;
  vector<ReadMap_t *>::iterator rmpi;
  list<ReadAlignChain_t *>::iterator rcpi;

  //-- For each read in the mapping
  for ( rmpi  = mapping . reads . begin( );
	rmpi != mapping . reads . end( ); ++ rmpi )
    {
      rmp = *rmpi;

      //-- If already placed, skip
      if ( rmp -> place != NULL )
	continue;

      //-- Get all of the valid chains
      places . clear( );
      for ( rcpi  = rmp -> best . begin( );
	    rcpi != rmp -> best . end( ); ++ rcpi )
	if ( IsValidChain (*rcpi, rmp) )
	  places . push_back (*rcpi);

      //-- Randomly pick a valid chain
      if ( !places . empty( ) )
	rmp -> place = places
	  [(int)((double)places . size( ) * rand( ) / (RAND_MAX + 1.0))];
    }
}




//------------------------------------------------------ PlaceUnambiguous ----//
void PlaceUnambiguous (Mapping_t & mapping)
{
  ReadMap_t * rmp;
  vector<ReadMap_t *>::iterator rmpi;

  //-- For each read in the mapping
  for ( rmpi  = mapping . reads . begin( );
	rmpi != mapping . reads . end( ); ++ rmpi )
    {
      rmp = *rmpi;

      //-- If not placed, unambiguous, and valid, place the read
      if ( rmp -> place == NULL
	   &&
	   rmp -> best . front( ) == rmp -> best . back( )
	   &&
	   IsValidChain (rmp -> best . front( ), rmp) )
	rmp -> place = rmp -> best . front( );
    }
}




//-------------------------------------------------------- PrintConflicts ----//
void PrintConflicts (const Mapping_t & mapping)
{
  list<Conflict_t *>::const_iterator cpi;
  map<string, Reference_t>::const_iterator rmi;

  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); ++ rmi )
    {
      cout << '>' << rmi -> first << endl;
      for ( cpi  = rmi -> second . conflicts . begin( );
	    cpi != rmi -> second . conflicts . end( ); ++ cpi )
	{
	  switch ( (*cpi) -> status )
	    {
	    case Conflict_t::SUPPORTED:
	      cout << "+  ";
	      break;
	    case Conflict_t::UNSUPPORTED:
	      cout << "-  ";
	      break;
	    case Conflict_t::ARTIFACT:
	      cout << ".  ";
	      break;
	    case Conflict_t::AMBIGUOUS:
	      cout << "?  ";
	      break;
	    }

	  cout << (*cpi) -> pos
	       << "\t"   << (*cpi) -> gapR
	       << "\t"   << (*cpi) -> gapQ
	       << "\t"   << (*cpi) -> type
	       << "\tN." << (*cpi) -> discount . size( )
	       << "\tY." << (*cpi) -> support  . size( ) << "\t" << endl;

	  //--
	  set<ReadMap_t *>::const_iterator rmpi;
	  cout << " S: ";
	  for (rmpi  = (*cpi) -> support . begin( );
	       rmpi != (*cpi) -> support . end( ); ++ rmpi )
	    cout << ((*rmpi) -> exclude ? '-' : '+') << (*rmpi) -> id << ' ';
	  cout << "\n D: ";
	  for (rmpi  = (*cpi) -> discount . begin( );
	       rmpi != (*cpi) -> discount . end( ); ++ rmpi )
	    cout << ((*rmpi) -> exclude ? '-' : '+') << (*rmpi) -> id << ' ';
	  cout << endl << endl;
	  //--
	}
    }
}




//---------------------------------------------------------- PrintMapping ----//
void PrintMapping (Mapping_t & mapping)
{
  ReadMap_t temprm;
  temprm . place = new ReadAlignChain_t( );
  temprm . place -> head = new ReadAlign_t( );

  map<string, Reference_t>::iterator rmi;
  vector<ReadMap_t *>::iterator rmpi;

  ReadAlign_t * curraln;
  list<ReadAlignChain_t *>::iterator racpi;

  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;


  //-- For each reference
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); ++ rmi )
    {
      //-- Find the reference range
      temprm . place -> head -> ref = &(rmi -> second);
      rmpip = equal_range (mapping . reads . begin( ), mapping . reads . end( ),
			   &temprm, ReadReferenceCmp_t( ));

      cout << '>' << rmi -> first << '\t'
	   << (rmi -> second) . len
	   << endl;

      //-- For each read mapping to this reference
      for ( rmpi = rmpip . first; rmpi != rmpip . second; ++ rmpi )
	{
	  cout << (*rmpi) -> id << '\t'
	       << (*rmpi) -> place -> beg << '\t'
	       << ((*rmpi) -> place -> head -> ori == FORWARD_CHAR ? '+' : '-');
	  if ( (*rmpi) -> mate . read == NULL )
	    cout << "\tNULL";
	  else
	    cout << '\t' << (*rmpi) -> mate . read -> id;
	  cout << endl;

	  /*
	  for ( racpi  = (*rmpi) -> best . begin( );
		racpi != (*rmpi) -> best . end( ); racpi ++ )
	    {
	      cout << "#\n";
	      for ( curraln  = (*racpi) -> head;
		    curraln != NULL; curraln = curraln -> from )
		cout << ' ' << curraln -> loR << '\t' << curraln -> hiR << endl;
	    }
	  */
	}
    }

  delete temprm . place -> head;
  delete temprm . place;
}




//------------------------------------------------------------- PrintTigr ----//
void PrintTigr (const Assembly_t & assembly)
{
  vector<Contig_t *>::const_iterator cpi;
  vector<Tile_t *>::const_iterator tpi;
  long int ctgs, len;

  ctgs = 0;
  for ( cpi = assembly . contigs . begin( );
	cpi != assembly . contigs . end( ); ++ cpi )
    {
      cout
	<< "##" << ++ctgs << ' '
	<< (*cpi) -> tiles . size( ) << ' '
	<< (*cpi) -> len << " 00000000 checksum."
	<< endl;

      for ( tpi = (*cpi) -> tiles . begin( );
	    tpi != (*cpi) -> tiles . end( ); ++ tpi )
	{
	  len = labs ((*tpi) -> end - (*tpi) -> beg) + 1;

	  cout
	    << '#' << (*tpi) -> read -> id
	    << '(' << (*tpi) -> off << ')'
	    << " [" << ((*tpi) -> beg < (*tpi) -> end ? "" : "RC") << "] "
	    << len << " bases, 00000000 checksum. "
	    << '{' << (*tpi) -> beg << ' ' << (*tpi) -> end << "} "
	    << '<' << (*tpi) -> off + 1 << ' ' << (*tpi) -> off + len << '>'
	    << endl;
	}
    }
}




//-------------------------------------------------------------- PrintUMD ----//
void PrintUMD (const Assembly_t & assembly)
{
  vector<Contig_t *>::const_iterator cpi;
  vector<Tile_t *>::const_iterator tpi;
  long int ctgs;

  ctgs = 0;
  for ( cpi = assembly . contigs . begin( );
	cpi != assembly . contigs . end( ); ++ cpi )
    {
      cout
	<< "C " << ++ctgs << '\t'
	<< (*cpi) -> tiles . size( ) << '\t'
	<< *((*cpi)->tiles.front( )->read->place->head->ref->id) << '\t'
	<< (*cpi) -> tiles . front( ) -> read -> place -> beg << "-"
	<< (*cpi) -> tiles . front( ) -> read -> place -> beg
	+ (*cpi) -> len - 1 << endl;

      for ( tpi  = (*cpi) -> tiles . begin( );
	    tpi != (*cpi) -> tiles . end( ); ++ tpi )
	{
	  cout
	    << (*tpi) -> read -> id << ' '
	    << (*tpi) -> off + (*tpi) -> beg - 1 << ' '
	    << (*tpi) -> off + (*tpi) -> end - 1 << '\t'

	    << (*tpi) -> read -> place -> tbeg -
	    (*tpi) -> read -> place -> beg << '\t'
	    << (*tpi) -> read -> place -> end -
	    (*tpi) -> read -> place -> tend << '\t'
	    << (*tpi) -> read -> place -> idy << endl;
	}

      cout << endl;
    }
}




//------------------------------------------------------- RefineConflicts ----//
void RefineConflicts (Mapping_t & mapping)
{
  ReadMap_t temprm;
  temprm . place = new ReadAlignChain_t( );
  temprm . place -> head = new ReadAlign_t( );

  long int cpos, beg, end, tot;
  Reference_t * rp;
  ReadAlign_t * rap;

  list<Conflict_t *>::iterator cpi, locpi, hicpi, bcpi, ecpi;
  map<string, Reference_t>::iterator rmi;
  vector<ReadMap_t *>::iterator rmpi, rmpie, ri, rj;
  set<ReadMap_t *>::iterator si;
  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;

  vector<ReadMap_t *> heap;
  vector<Conflict_t *> breaks;
  vector<Conflict_t *>::iterator bi;
  set<ReadMap_t *> readsetA;
  set<ReadMap_t *> readsetB;


  //-- For each reference and its conflicts
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); ++ rmi )
    {
      rp = &(rmi -> second);
      bcpi = rp -> conflicts . begin( );
      ecpi = rp -> conflicts . end( );

      //-- Find the read mapping range
      temprm . place -> head -> ref = rp;
      rmpip = equal_range (mapping . reads . begin( ), mapping . reads . end( ),
			   &temprm, ReadReferenceCmp_t( ));
      rmpi = rmpip . first;
      rmpie = rmpip . second;


      //-- For each *BREAK*, collect nay counts
      for ( cpi = bcpi; cpi != ecpi; ++ cpi )
	{
	  if ( (*cpi) -> type == Conflict_t::INDEL )
	    continue;

	  cpos = (*cpi) -> pos;

	  //-- Update the heap to current conflict pos
	  while ( !heap . empty( )  &&  heap . front( ) -> place -> end < cpos )
	    PopHeap (heap);

	  //-- Load up the heap with reads that overlap the conflict
	  for ( ; rmpi != rmpie  &&  (*rmpi) -> place -> beg <= cpos; ++ rmpi )
	    if ( (*rmpi) -> place -> end >= cpos )
	      PushHeap (heap, *rmpi);

	  //-- Collect the nay counts, i.e. reads that do not agree on break
	  for ( ri = heap . begin( ); ri != heap . end( ); ++ ri )
	    for ( rap = (*ri)->place->head; rap != NULL; rap = rap->from )
	      if ( rap -> loR < cpos - FUZZY  &&  rap -> hiR > cpos + FUZZY )
		{
		  (*cpi) -> discount . insert (*ri);
		  break;
		}

	  //-- Can't discount break if supporting it
	  set_difference
	    ((*cpi) -> discount . begin( ), (*cpi) -> discount . end( ),
	     (*cpi) -> support  . begin( ), (*cpi) -> support  . end( ),
	     insert_iterator<set<ReadMap_t *> >
	     (readsetA, readsetA . begin( )));
	  (*cpi) -> discount . swap (readsetA);
	  readsetA . clear( );

	  //-- Call the break SUPPORTED, UNSUPPORTED, AMBIGUOUS
	  CallConflict (*cpi);

	  //-- We don't want to accept breaks, so call them ambiguous
	  if ( (*cpi) -> status == Conflict_t::SUPPORTED )
	    (*cpi) -> status = Conflict_t::AMBIGUOUS;
	}
      heap . clear( );


      //-- For each *INDEL*, sum yay/nay counts from its break counts
      for ( cpi = bcpi; cpi != ecpi; ++ cpi )
	{
	  if ( (*cpi)->type != Conflict_t::INDEL )
	    continue;

	  //-- The reference breaks for the gap
	  beg = (*cpi) -> pos;
	  end = (*cpi) -> pos + (*cpi) -> gapR + 1;

	  //-- Find conflicts corresponding to the reference HIBREAK
	  for ( hicpi = bcpi; hicpi != ecpi; ++ hicpi )
	    if ( hicpi != cpi  &&
		 labs ((*hicpi)->pos - beg) <= FUZZY )
	      {
		breaks . push_back (*hicpi);

		if ( (*hicpi) -> type == Conflict_t::HIBREAK )
		  {

		    //-- Add to INDEL support if hang isn't too large
		    for ( si  = (*hicpi) -> support . begin( );
			  si != (*hicpi) -> support . end( ); ++ si )
		      if ( (*si) -> place -> end - beg <
			   (*cpi) -> gapQ + OPT_MaxTrimLen )
			(*cpi) -> support . insert (*si);

		    //-- Add to hibreak discount
		    for ( si  = (*hicpi) -> discount . begin( );
			  si != (*hicpi) -> discount . end( ); ++ si )
		      readsetA . insert (*si);
		  }
	      }

	  //-- Find conflicts corresponding to the reference LOBREAK
	  for ( locpi = bcpi; locpi != ecpi; ++ locpi )
	    if ( locpi != cpi  &&
		 labs ((*locpi)->pos + (*locpi)->gapR + 1 - end) <= FUZZY )
	      {
		breaks . push_back (*locpi);

		if ( (*locpi) -> type == Conflict_t::LOBREAK )
		  {
		    //-- Add to INDEL support if hang isn't too large
		    for ( si  = (*locpi) -> support . begin( );
			  si != (*locpi) -> support . end( ); ++ si )
		      if ( end - (*si) -> place -> beg <
			   (*cpi) -> gapQ + OPT_MaxTrimLen )
			(*cpi) -> support . insert (*si);

		    //-- Add to lobreak discount
		    for ( si  = (*locpi) -> discount . begin( );
			  si != (*locpi) -> discount . end( ); ++ si )
		      readsetB . insert (*si);
		  }
	      }

	  //-- One break discounted good enough for regular gap
	  if ( (*cpi) -> gapR > 0 )
	    set_union
	      (readsetA . begin( ), readsetA . end( ),
	       readsetB . begin( ), readsetB . end( ),
	       insert_iterator<set<ReadMap_t *> >
	       ((*cpi) -> discount, (*cpi) -> discount . begin( )));
	  //-- Tandems with reference overlap need both breaks discounted
	  else
	    set_intersection
	      (readsetA . begin( ), readsetA . end( ),
	       readsetB . begin( ), readsetB . end( ),
	       insert_iterator<set<ReadMap_t *> >
	       ((*cpi) -> discount, (*cpi) -> discount . begin( )));
	  readsetA . clear( );
	  readsetB . clear( );

	  //-- Can't support indel if discounting it
	  set_difference
	    ((*cpi) -> support  . begin( ), (*cpi) -> support  . end( ),
	     (*cpi) -> discount . begin( ), (*cpi) -> discount . end( ),
	     insert_iterator<set<ReadMap_t *> >
	     (readsetA, readsetA . begin( )));
	  (*cpi) -> support . swap (readsetA);
	  readsetA . clear( );

	  //-- Call the indel SUPPORTED, UNSUPPORTED, AMBIGUOUS
	  CallConflict (*cpi);

	  //-- Flag indel breaks as artifacts
	  if ( (*cpi) -> status != Conflict_t::UNSUPPORTED )
	    for ( bi = breaks . begin( ); bi != breaks . end( ); ++ bi )
	      {
		if ( (*bi) -> type != Conflict_t::INDEL )
		  (*bi) -> status = Conflict_t::ARTIFACT;
		else if ( (*bi)  -> status != Conflict_t::UNSUPPORTED  &&
			  (*cpi) -> status != Conflict_t::ARTIFACT )
		  {
		    long int byay = (*bi) -> support . size( );
		    long int cyay = (*cpi) -> support . size( );
		    float tyay = byay + cyay;
		    if ( tyay == 0 ) tyay = -1;

		    if ((float)byay / tyay * 100.0 >= OPT_Majority)
		      (*cpi) -> status = Conflict_t::ARTIFACT;
		    else if ((float)cyay / tyay * 100.0 >= OPT_Majority)
		      (*bi) -> status = Conflict_t::ARTIFACT;
		    else
		      {
			long int bnay = (*bi) -> discount . size( );
			long int cnay = (*cpi) -> discount . size( );
			float tnay = bnay + cnay;
			if ( tnay == 0 ) tnay = -1;
			
			if ((float)bnay / tnay * 100.0 >= OPT_Majority)
			  (*bi) -> status = Conflict_t::ARTIFACT;
			else if ((float)cnay / tnay * 100.0 >= OPT_Majority)
			  (*cpi) -> status = Conflict_t::ARTIFACT;
			else
			  {
			    (*bi)  -> status = Conflict_t::AMBIGUOUS;
			    (*cpi) -> status = Conflict_t::AMBIGUOUS;
			  }
		      }
		  }
	      }

	  breaks . clear( );
	}


      //-- INCLUDE/EXCLUDE reads based on consistency
      for ( cpi = bcpi; cpi != ecpi; ++ cpi )
	if ( (*cpi) -> status == Conflict_t::ARTIFACT )
	  {
	    //-- Exclude supporting reads (will be revived later if good)
	    for ( si  = (*cpi) -> support . begin( );
		  si != (*cpi) -> support . end( ); ++ si )
	      (*si) -> exclude = true;
	  }	  

      for ( cpi = bcpi; cpi != ecpi; ++ cpi )
	if ( (*cpi) -> status == Conflict_t::SUPPORTED )
	  {
	    //-- Include supporting reads (revives supporting artifacts)
	    for ( si  = (*cpi) -> discount . begin( );
		  si != (*cpi) -> discount . end( ); ++ si )
	      (*si) -> exclude = true;
	    //-- Exclude discounting reads
	    for ( si  = (*cpi) -> support . begin( );
		  si != (*cpi) -> support . end( ); ++ si )
	      (*si) -> exclude = false;
	  }

      for ( cpi = bcpi; cpi != ecpi; ++ cpi )
	if ( (*cpi) -> status == Conflict_t::UNSUPPORTED )
	  {
	    //-- Exclude supporting reads
	    for ( si  = (*cpi) -> support . begin( );
		  si != (*cpi) -> support . end( ); ++ si )
	      (*si) -> exclude = true;
	  }
	else if ( (*cpi) -> status == Conflict_t::AMBIGUOUS )
	  {
	    //-- Exclude everything ambiguous
	    for ( si  = (*cpi) -> discount . begin( );
		  si != (*cpi) -> discount . end( ); ++ si )
	      (*si) -> exclude = true;
	    for ( si  = (*cpi) -> support . begin( );
		  si != (*cpi) -> support . end( ); ++ si )
	      (*si) -> exclude = true;
	  }
    }

  delete temprm . place -> head;
  delete temprm . place;
}




//------------------------------------------------------ ReadAlignChain_t ----//
ReadAlignChain_t::ReadAlignChain_t (const ReadMap_t * rmp, ReadAlign_t * rap)
{
  long int n;
  long int olap1, olap2, nrlen;
  float idyt;

  head = rap;

  tend = rap -> hiR;
  end  = tend + (rmp -> len - rap -> hi);

  n = 0;
  idyt = 0;
  len = rap -> hi  - rap -> lo  + 1;
  idy = rap -> idy * len;
  while ( rap -> from != NULL )
    {
      assert ( rap -> from -> lo <= rap -> lo  );

      olap1 = rap -> from -> hiR - rap -> loR + 1;
      if ( olap1 < 0 )
	olap1 = 0;
      olap2 = rap -> from -> hi  - rap -> lo  + 1;

      nrlen = rap -> from -> hi - rap -> from -> lo + 1 -
	(olap1 > olap2 ? olap1 : olap2);

      len  += nrlen;
      idy  += rap -> from -> idy * nrlen;

      rap = rap -> from;
    }

  tbeg = rap -> loR;
  beg  = tbeg - (rap -> lo - 1);

  idy /= (float)len;
}




//---------------------------------------------------------------- Tile_t ----//
Tile_t::Tile_t (ReadMap_t * rmp, Contig_t * cp)
{
  if ( cp -> tiles . empty( ) )
    cp -> adjust = - (rmp -> place -> beg);
  
  read = rmp;
  off  = rmp -> place -> beg + cp -> adjust;
  beg  = 1;
  end  = rmp -> len;
  
  if ( rmp -> place -> head -> ori == REVERSE_CHAR )
    {
      beg = RevComp1 (beg, rmp -> len);
      end = RevComp1 (end, rmp -> len);
    }
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg  &&
	  ((ch = getopt (argc, argv, "b:Cg:hi:I:m:Mo:rs:St:TUv:V:")) != EOF) )
    switch (ch)
      {
      case 'b':
	OPT_BankName = optarg;
	OPT_Matepair = true;
	break;

      case 'C':
	OPT_PrintConflicts = true;
	break;

      case 'g':
	OPT_MaxGap = atoi (optarg);
	break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'i':
	OPT_MinIdentity = atof (optarg);
	break;

      case 'I':
	OPT_MaxIdentityDiff = atof (optarg);
	break;

      case 'm':
	OPT_Majority = atof (optarg);
	break;

      case 'M':
	OPT_PrintMaps = true;
	break;

      case 'o':
	OPT_MinOverlap = atoi (optarg);
	break;

      case 'r':
	OPT_Random = true;
	break;

      case 's':
	OPT_Seed = atoi (optarg);
	break;

      case 'S':
	OPT_Singletons = true;
	break;

      case 't':
	OPT_MaxTrimLen = atoi (optarg);
	break;

      case 'T':
	OPT_PrintTigr = true;
	break;

      case 'U':
	OPT_PrintUMD = true;
	break;

      case 'v':
	OPT_MinCoverage = atof (optarg);
	break;

      case 'V':
	OPT_MaxCoverageDiff = atof (optarg);
	break;

      default:
        errflg ++;
      }

  if ( errflg > 0 || optind != argc - 1 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  if ( OPT_Seed < 0 )
    OPT_Seed = time (NULL);

  OPT_AlignName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       Set path of the AMOS bank to use for mate-pair info\n"
    << "-C            Print conflict positions and support counts\n"
    << "-g uint       Set maximum alignment gap length, default "
    << OPT_MaxGap << endl
    << "-h            Display help information\n"
    << "-i float      Set the minimum alignment identity, default "
    << OPT_MinIdentity << endl
    << "-I float      Set the identity tolerance between repeats, default "
    << OPT_MaxIdentityDiff << endl
    << "-m float      Set the majority needed to discern a conflict, default "
    << OPT_Majority << endl
    << "-M            Print read mappings\n"
    << "-o uint       Set minimum overlap length for assembly, default "
    << OPT_MinOverlap << endl
    << "-r            Randomly place repetitive reads into one of their copy\n"
    << "              locations if they cannot be placed via mate-pair info\n"
    << "-s uint       Set random generator seed to unsigned int, default\n"
    << "              seed is generated by the system clock\n"
    << "-S            Include singleton contigs in the output\n"
    << "-t uint       Set maximum ignorable trim length, default "
    << OPT_MaxTrimLen << endl
    << "-T            Print TIGR contig\n"
    << "-U            Print UMD contig\n"
    << "-v float      Set the minimum alignment coverage, default "
    << OPT_MinCoverage << endl
    << "-V float      Set the coverage tolerance between repeats, default "
    << OPT_MaxCoverageDiff << endl
    << endl;

  cerr
    << "  Position query sequences on a reference based on the alignment\n"
    << "information contained in the deltafile. Can utilize mate-pair\n"
    << "information to place repetitive sequences, or random placement\n"
    << "to simulate even coverage.\n\n";

  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  [-b amosbank]  <deltafile>\n\n";
  return;
}
