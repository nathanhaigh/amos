////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/08/2004
//!
//! \brief Layout the query seqs based on alignment and mate-pair info
//!
//! \todo link mates that span the origin when -c is on
//! \todo circ read olaps in readheap
//! \todo set rand seed on command line
//! \todo see "//TODO" in code
////////////////////////////////////////////////////////////////////////////////

#include "alloc.hh"
#include "amp.hh"
#include "delta.hh"
#include "universals_AMOS.hh"
#include <algorithm>
#include <cstdlib>
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



//============================================================= Constants ====//
const char FORWARD_CHAR  = '+';
const char REVERSE_CHAR  = '-';

const int   MAXGAPR     = 75000;        // maximum reference gap in a chain
const int   MAXGAPQ     =  1000;        // maximum query gap in a chain

const float MINCOV = 25.0;              // min coverage to tile
const float MINIDY = 70.0;              // min identity to tile

const float MAXCOVDIFF  =  2.0;         // max %coverage diff between 'best'
const float MAXIDYDIFF  =  2.0;         // max %identity diff between 'best'

const int   ERRLEN       =   10;        // max diff indel length before error
const int   TRMLEN       =   20;        // max end trimming before error

//TODO - DO IDY CHECKING
//const float ERRIDY       =  4.0;        // max %identity diff before error
//const float IDYOLAP      = 50.0;        // min olap coverage for idy check

const int   MININDELS    =    2;        // min coverage of indels needed
const int   MINOLAP      =   10;        // min olap between 2 1x regions

struct ReadMap_t;
struct Contig_t;

struct Conflict_t
{
  enum
    {
      INDEL,
      HIBREAK,
      LOBREAK,
    };
  enum
    {
      SUPPORTED,
      UNSUPPORTED,
      ARTIFICIAL,
    };

  short int status;                     // include this conflict in the layout
  int type;                             // type of the conflict (enum above)
  long int pos;                         // reference pos of the conflict
  long int gapR, gapQ;                  // gap in the ref and query
  vector<ReadMap_t *> support;          // the supporting reads
  vector<ReadMap_t *> discount;         // the disagreeing reads

  Conflict_t (int t, long int p, long int gr, long int gq, ReadMap_t * rmp)
  { type = t; pos = p; gapR = gr; gapQ = gq; support . push_back (rmp); }
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
    for ( cpi = conflicts . begin( ); cpi != conflicts . end( ); cpi ++ )
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
  char dirR;                            // direction of ref
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
  ID_t id;                              // ID of read
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
    for ( rapi = all . begin( ); rapi != all . end( ); rapi ++ )
      delete (*rapi);
    list<ReadAlignChain_t *>::iterator rcpi;
    for ( rcpi = best . begin( ); rcpi != best . end( ); rcpi ++ )
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
    for ( rmpi = reads . begin( ); rmpi != reads . end( ); rmpi ++ )
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
    for ( tpi = tiles . begin( ); tpi != tiles . end( ); tpi ++ )
      delete (*tpi);
  }
};

struct Assembly_t
{
  vector<Contig_t *> contigs;

  ~Assembly_t ( )
  {
    vector<Contig_t *>::iterator cpi;
    for ( cpi = contigs . begin( ); cpi != contigs . end( ); cpi ++ )
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
  {
    if ( A -> lo < B -> lo )
      return true;
    else
      return false;
  }
};

struct ReadHeapCmp_t
{
  bool operator( ) (const ReadMap_t * A, const ReadMap_t * B)
  {
    if ( A -> place -> end > B -> place -> end )
      return true;
    else
      return false;
  }
};

struct ReadIdCmp_t
{
  bool operator( ) (const ReadMap_t * A, const ReadMap_t * B)
  {
    if ( A -> id < B -> id )
      return true;
    else
      return false;
  }
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

    if ( A -> place -> beg < B -> place -> beg )
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
  {
    if ( A -> off < B -> off )
      return true;
    else
      return false;
  }
};

struct ReadExcludePredicate_t
{
  bool operator( ) (const ReadMap_t * A)
  {
    if ( A -> exclude )
      return true;
    return false;
  }
};

struct ConflictExcludePredicate_t
{
  bool operator( ) (const Conflict_t * A)
  {
    if ( A -> status != Conflict_t::SUPPORTED )
      return true;
    return false;
  }
};



//=============================================================== Options ====//
string  OPT_BankName;                 // bank name parameter
string  OPT_AlignName;                // alignment name parameter
bool    OPT_Matepair = false;         // use matepair information
bool    OPT_Circular = false;         // circular reference
bool    OPT_PrintConflicts = false;   // print conflicts
bool    OPT_PrintMaps = false;        // print read maps
bool    OPT_PrintTigr = false;        // print tigr contigs
bool    OPT_PrintUMD = false;         // print UMD contigs
bool    OPT_Random = false;           // randomly place ambiguous reads
int     OPT_Seed = -1;                // random seed



//========================================================== Fuction Decs ====//
long int lmin (long int a, long int b)
{ return (a < b ? a : b); }

long int lmax (long int a, long int b)
{ return (a < b ? b : a); }

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
//! Get rid of negative offests, sort by offest
//!
//! \param assembly The assembly contigs
//! \return void
//!
void CleanContigs (Assembly_t & assembly);


//--------------------------------------------------------- FindConflicts ----//
//! \brief Find and store conflicts between the reads and the refs
//!
//! \param mapping The read mapping
//! \pre read list is sorted by place
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


//------------------------------------------------------------- ParseArgs ----//
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
void ParseArgs (int argc, char ** argv);


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


//------------------------------------------------------------- PrintHelp ----//
//! \brief Prints help information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintHelp (const char * s);


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


//------------------------------------------------------------ PrintUsage ----//
//! \brief Prints usage information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintUsage (const char * s);


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



//========================================================== Inline Funcs ====//
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
       (float)(read -> len) * 100.0 > MAXCOVDIFF )
    return cand -> len > best -> len;
  else
    {
      if ( fabs(best -> idy - cand -> idy) > MAXIDYDIFF )
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
  return (
	  (float)(best -> len - cand -> len) /
	  (float)(read -> len) * 100.0 <= MAXCOVDIFF
	  &&
	  best -> idy - cand -> idy <= MAXIDYDIFF
	  );
}


//------------------------------------------------------- IsEqualConflict ----//
inline bool IsEqualConflict (const Conflict_t * A, const Conflict_t * B)
{
  if ( A -> type == B -> type
       &&
       labs (A -> pos - B -> pos) <= ERRLEN
       &&
       labs (A -> gapR - B -> gapR) <= ERRLEN
       &&
       labs (A -> gapQ - B -> gapQ) <= ERRLEN )
    return true;
  else
    return false;
}


//---------------------------------------------------------- IsValidChain ----//
inline bool IsValidChain(const ReadAlignChain_t * cand,
			 const ReadMap_t * read)
{
  return (
	  (float)cand -> len / (float)read -> len * 100.0 >= MINCOV
	  &&
	  cand -> idy >= MINIDY
	  );
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

  //-- INPUT
  cerr << "layout.INPUT\n";
  ParseArgs (argc, argv);          // parse the command line arguments
  ParseAlign (mapping);            // parse the alignment data
  ChainAligns (mapping);           // build the LAS chains for each read

  srand (OPT_Seed);

  //-- READ PLACEMENT
  cerr << "layout.PLACEMENT\n";
  PlaceUnambiguous (mapping);      // place unambiguous reads
  if ( OPT_Matepair )
    {
      ParseMates (mapping);        // parse the matepair data
      PlaceHappyMates (mapping);   // place 'happy' matepairs
    }
  if ( OPT_Random )
    {
      PlaceRandom (mapping);       // randomly place all unplaced reads
    }

  sort (mapping.reads.begin( ), mapping.reads.end( ), ReadPlaceCmp_t( ));

  //-- CONFLICT DETECTION
  cerr << "layout.CONFLICT\n";
  FindConflicts (mapping);         // find conflicts btwn reads and refs
  RefineConflicts (mapping);       // assess, combine and select conflicts

  //-- ASSEMBLY
  if ( OPT_PrintTigr  ||  OPT_PrintUMD )
    {
      cerr << "layout.ASSEMBLY\n";
      Assemble (mapping, assembly);// layout the reads to form the assembly
      CleanContigs (assembly);     // clean up the contigs
    }

  //-- OUTPUT
  cerr << "layout.OUTPUT\n";
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
  list<Conflict_t *>::iterator cpi;
  list<ReadMap_t *>::iterator rmpi, rmpic;

  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;

  long int currend, confbeg, confend;


  //-- For each reference
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); rmi ++ )
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


      cp = new Contig_t( );
      assembly . contigs . push_back (cp);
      rmpi = reads . begin( );

      //-- Keep going until all reads have been assembled
      while ( ! reads . empty( ) )
	{
	  //-- If end of list reached, push a new contig and start over
	  if ( rmpi == reads . end( )  &&  !cp->tiles.empty( ) )
	    {
	      cp = new Contig_t( );
	      assembly . contigs . push_back (cp);
	      rmpi = reads . begin( );
	    }

	  //-- Jump to the next conflict
	  if ( rmpi != reads . end( ) )
	    {
	      for ( cpi  = confs . begin( ); cpi != confs . end( ); cpi ++ )
		if ( (*cpi) -> pos > (*rmpi) -> place -> beg )
		  break;
	    }
	  rmpic = reads . end( );

	  //-- Mark the beg and end of the conflict
	  if ( cpi != confs . end( ) )
	    {
	      confbeg = (*cpi) -> pos;
	      confend = confbeg + (*cpi) -> gapR;

	      //-- If LOBREAK, push a new contig
	      if ( (*cpi) -> type == Conflict_t::LOBREAK  &&
		   !cp->tiles.empty( ) )
		{
		  cp = new Contig_t( );
		  assembly . contigs . push_back (cp);
		}
	    }


	  //-- Assemble reads up to but not including the conflict
	  while ( rmpi != reads . end( ) )
	    {
	      //-- Break if past the conflict
	      if ( cpi != confs . end( ) )
		if ( (*rmpi) -> place -> beg > confend )
		  break;

	      //-- If clear of the conflict
	      if ( cpi == confs.end( )  ||
		   ((*cpi) -> type != Conflict_t::LOBREAK  &&
		    (*rmpi) -> place -> end <= confbeg) )
		{
		  tp = new Tile_t (*rmpi, cp);

		  //-- 0 coverage forces a new contig
		  if ( ! cp->tiles.empty( )  &&  tp->off > cp->len - MINOLAP )
		    {
		      //-- Push a new contig and redo the tile
		      cp = new Contig_t( );
		      assembly . contigs . push_back (cp);
		      (*tp) = Tile_t (*rmpi, cp);
		    }
		  
		  //-- ASSEMBLE the read
		  cp -> tiles . push_back (tp);
		  currend = tp -> off + labs (tp -> end - tp -> beg) + 1;
		  if ( currend > cp -> len )
		    cp -> len = currend;

		  //-- Read has been assembled, remove from the list
		  rmpi = reads . erase (rmpi);
		}
	      //-- If involved in the conflict, save for later
	      else
		{
		  if ( rmpic == reads . end( ) )
		    rmpic = rmpi;
		  rmpi ++;
		}
	    }

	  //TODO - TEMPORARY
	  if ( rmpic != reads . end( ) )
	    while ( rmpic != reads . begin( )  &&
		    (*rmpic) -> place -> end > confbeg )
	      rmpic --;

	  //-- Assemble reads overlapping the conflict
	  while ( rmpic != reads . end( ) )
	    {
	      //-- Break if past the conflict
	      if ( rmpic == rmpi )
		{
		  switch ( (*cpi) -> type )
		    {
		    case Conflict_t::LOBREAK:
		      //-- If a LOBREAK, do nothing
		      break;

		    case Conflict_t::HIBREAK:
		      //-- If a HIHANG, start a new contig
		      rmpi = reads . end( );
		      break;

		    case Conflict_t::INDEL:
		      //-- If an INDEL, adjust for the reference gap
		      cp -> adjust += (*cpi) -> gapQ - (*cpi) -> gapR;
		      break;
		    }

		  //-- Resolved conflict, remove from list
		  cpi = confs . erase (cpi);
		  break;
		}

	      //-- If a supporting read
	      if ( binary_search ((*cpi) -> support . begin( ),
				  (*cpi) -> support . end( ), *rmpic) )
		{
		  tp = new Tile_t (*rmpic, cp);

		  //TODO - THINK ABOUT THIS HARDER
		  //-- If a LOBREAK on an INDEL, need to adjust the offset
		  if ( (*cpi) -> type == Conflict_t::INDEL  &&
		       (*rmpic) -> place -> tbeg >= lmin (confbeg, confend) )
		    {
		      tp -> off += (*cpi) -> gapQ - (*cpi) -> gapR;
		    }
		  else
		    {
		      //-- 0 coverage forces a new contig
		      if ( !cp -> tiles . empty( )  &&
			   tp -> off > cp -> len - MINOLAP )
			{
			  //-- Push a new contig and redo the tile
			  cp = new Contig_t( );
			  assembly . contigs . push_back (cp);
			  (*tp) = Tile_t (*rmpic, cp);
			}
		    }

		  //-- ASSEMBLE the read
		  cp -> tiles . push_back (tp);
		  currend = tp -> off + labs (tp -> end - tp -> beg) + 1;
		  if ( currend > cp -> len )
		    cp -> len = currend;

		  //-- Read has been assembled, remove from the list
		  rmpic = reads . erase (rmpic);
		}
	      //-- If overlapping, but not supporting, save for later
	      else
		{
		  rmpic ++;
		}
	    }
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
	rmpi != mapping . reads . end( ); rmpi ++ )
    {
      //-- Initialize the dynamic programming matrix
      n = (*rmpi) -> all . size( );
      las = (ScoreLAS *) SafeRealloc (las, sizeof (ScoreLAS) * n);
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
	    if ( OPT_Circular  &&
		 olap1 > las [j] . a -> ref -> len / 2 )
	      {
		olap1 = (las [j] . a -> hiR - las [j] . a -> ref -> len) +
		  (1 - las [i] . a -> loR);
	      }
	    if ( olap1 < -(MAXGAPR) )
	      continue;
	    olap = olap1 > 0 ? olap1 : 0;

	    olap2 = las [j] . a -> hi - las [i] . a -> lo + 1;
	    if ( olap2 < -(MAXGAPQ) )
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
      //   i.e. within MAXCOVDIFF of the longest and within MAXIDYDIFF
      //   of the longest (with the higest idy)
      rcpi = (*rmpi) -> best . begin( );
      while ( rcpi != (*rmpi) -> best . end( ) )
	{
	  if ( IsEqualChain (*rcpi, bestchain, *rmpi) )
	    rcpi ++;
	  else
	    {
	      delete (*rcpi);
	      rcpi = (*rmpi) -> best . erase (rcpi);
	    }
	}
    }
}






//---------------------------------------------------------- CleanContigs ----//
void CleanContigs (Assembly_t & assembly)
{
  vector<Contig_t *>::iterator cpi;
  vector<Tile_t *>::iterator tpi;
  long int adj;

  for ( cpi  = assembly . contigs . begin( );
	cpi != assembly . contigs . end( ); cpi ++ )
    {
      //-- Sort the tiled reads
      sort ((*cpi) -> tiles . begin( ), (*cpi) -> tiles . end( ), TileCmp_t( ));

      //-- Make the tiles begin at 0
      if ( !(*cpi) -> tiles . empty( ) )
	{
	  adj = (*cpi) -> tiles . front( ) -> off;
	  for ( tpi  = (*cpi) -> tiles . begin( );
		tpi != (*cpi) -> tiles . end( ); tpi ++ )
	    (*tpi) -> off -= adj;
	}

      //TODO - remove empty contigs
    }
}




//--------------------------------------------------------- FindConflicts ----//
void FindConflicts (Mapping_t & mapping)
{
  ReadAlign_t * curraln;
  ReadMap_t * currmap;
  pair<long int, long int> gap;
  vector<ReadMap_t *>::iterator rmpi;


  //-- For each read
  for ( rmpi  = mapping . reads . begin( );
	rmpi != mapping . reads . end( ); rmpi ++ )
    {
      if ( (*rmpi) -> place == NULL )
	continue;

      currmap = *rmpi;
      curraln = currmap -> place -> head;

      //-- If there is a HIBREAK
      if ( currmap -> place -> end - currmap -> place -> tend > TRMLEN )
	curraln -> ref -> conflicts . push_back
	  (new Conflict_t (Conflict_t::HIBREAK, curraln -> hiR, 0, 0, currmap));
      
      for ( ; curraln -> from != NULL; curraln = curraln -> from )
	{
	  gap = GapDistance (curraln -> from, curraln);
	  //-- If there is an INDEL, push it and it's break points
	  if ( labs (gap . first) > ERRLEN  ||  labs (gap . second) > ERRLEN )
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
      if ( currmap -> place -> tbeg - currmap -> place -> beg > TRMLEN )
	curraln -> ref -> conflicts . push_back
	  (new Conflict_t (Conflict_t::LOBREAK, curraln -> loR, 0, 0, currmap));
    }

  //-- For each reference, sort and remove redundant conflicts
  map<string, Reference_t>::iterator rmi;
  list<Conflict_t *>::iterator first, last, next, nxtf;
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); rmi ++ )
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
	      while ( !(*next) -> support . empty( ) )
		{
		  (*first) -> support . push_back ((*next)->support.back( ));
		  (*next) -> support . pop_back( );
		}
	      delete (*next);
	      rmi -> second . conflicts . erase (next);
	      next = first;
	    }
	  else
	    {
	      if ( labs ((*first) -> pos - (*next) -> pos) <= ERRLEN )
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
  ID_t id;
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
	    dai != dr . getRecord( ) . aligns . end( ); dai ++ )
	{
	  currmp -> all . push_back (new ReadAlign_t ( ));
	  currap = currmp -> all . back( );

	  currap -> ref = refp;
	  currap -> idy = dai -> idy;

	  //-- Force ascending coordinates
	  if ( dai -> sR < dai -> eR )
	    {
	      currap -> dirR = FORWARD_CHAR;
	      currap -> loR = dai -> sR;
	      currap -> hiR = dai -> eR;
	    }
	  else
	    {
	      currap -> dirR = REVERSE_CHAR;
	      currap -> loR = dai -> eR;
	      currap -> hiR = dai -> sR;
	    }

	  if ( (currap -> dirR == FORWARD_CHAR  &&  dai -> sQ < dai -> eQ)  ||
	       (currap -> dirR == REVERSE_CHAR  &&  dai -> sQ > dai -> eQ) )
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




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "b:cChMrs:TU")) != EOF) )
    switch (ch)
      {
      case 'b':
	OPT_BankName = optarg;
	OPT_Matepair = true;
	break;

      case 'c':
	OPT_Circular = true;
	break;

      case 'C':
	OPT_PrintConflicts = true;
	break;

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'M':
	OPT_PrintMaps = true;
	break;

      case 'r':
	OPT_Random = true;
	break;

      case 's':
	OPT_Seed = atoi (optarg);
	break;

      case 'T':
	OPT_PrintTigr = true;
	break;

      case 'U':
	OPT_PrintUMD = true;
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




//------------------------------------------------------------ ParseMates ----//
void ParseMates (Mapping_t & mapping)
{
  AMOS::Read_t red;
  AMOS::Fragment_t frg;
  AMOS::Library_t lib;
  AMOS::Matepair_t mtp;

  AMOS::Bank_t red_bank (AMOS::Bank_k::READ);
  AMOS::Bank_t frg_bank (AMOS::Bank_k::FRAGMENT);
  AMOS::Bank_t lib_bank (AMOS::Bank_k::LIBRARY);
  AMOS::Bank_t mtp_bank (AMOS::Bank_k::MATEPAIR);

  ID_t id;
  ReadMap_t * rmp, temprm;
  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;

  try {
    mtp_bank . open (OPT_BankName);
    red_bank . open (OPT_BankName);
    frg_bank . open (OPT_BankName);
    lib_bank . open (OPT_BankName);

    //-- Populate the mate records
    for ( id = 1; id <= mtp_bank . getLastIID( ); id ++ )
      {
	mtp . setIID (id);
	mtp_bank . fetch (mtp);
	if ( mtp . isRemoved( ) )
	  continue;

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
	red . setIID (rmp -> id);
	red_bank . fetch (red);
	frg . setIID (red . getFragment( ));
	frg_bank . fetch (frg);
	lib . setIID (frg . getLibrary( ));
	lib_bank . fetch (lib);

	//-- Set the mate offsets
	rmp -> mate . minoff = rmp -> mate . maxoff =
	  lib . getDistribution( ) . mean;
	rmp -> mate . minoff -= lib . getDistribution( ) . sd * 3;
	rmp -> mate . maxoff += lib . getDistribution( ) . sd * 3;
	rmp -> mate . read -> mate . minoff = rmp -> mate . minoff;
	rmp -> mate . read -> mate . maxoff = rmp -> mate . maxoff;
      }

    mtp_bank . close( );
    red_bank . close( );
    frg_bank . close( );
    lib_bank . close( );
  }
  catch (AMOS::Exception_t & e) {

    //-- On error, print debugging information
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    exit (EXIT_FAILURE);
  }
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
	rmpi != mapping . reads . end( ); rmpi ++ )
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
	    mrcpi != mrmp -> best . end( ); mrcpi ++ )
	{
	  //-- If mate not placed, search through all its best
	  if ( mrmp -> place == NULL )
	    mrcp = *mrcpi;
	  else
	    {
	      mrcp = mrmp -> place;
	      mrcpi = mrmp -> best . end( );
	      mrcpi --;                        // last loop iteration
	    }

	  for ( rcpi  = rmp -> best . begin( );
		rcpi != rmp -> best . end( ); rcpi ++ )	    
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
	rmpi != mapping . reads . end( ); rmpi ++ )
    {
      rmp = *rmpi;

      //-- If already placed, skip
      if ( rmp -> place != NULL )
	continue;

      //-- Get all of the valid chains
      places . clear( );
      for ( rcpi  = rmp -> best . begin( );
	    rcpi != rmp -> best . end( ); rcpi ++ )
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
	rmpi != mapping . reads . end( ); rmpi ++ )
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
	rmi != mapping . references . end( ); rmi ++ )
    {
      for ( cpi  = rmi -> second . conflicts . begin( );
	    cpi != rmi -> second . conflicts . end( ); cpi ++ )
	{
	  if ( (*cpi) -> status == Conflict_t::SUPPORTED )
	    cout << "+  ";
	  else if ( (*cpi) -> status == Conflict_t::UNSUPPORTED )
	    cout << " - ";
	  else
	    cout << " . ";
	  cout << (*cpi) -> pos
	       << "\t"   << (*cpi) -> gapR
	       << "\t"   << (*cpi) -> gapQ
	       << "\t"   << (*cpi) -> type
	       << "\tN." << (*cpi) -> discount . size( )
	       << "\tY." << (*cpi) -> support . size( ) << "\t" << endl;

	  /*
	  vector<ReadMap_t *>::const_iterator rmpi;
	  cout << "S: ";
	  for (rmpi  = (*cpi) -> support . begin( );
	       rmpi != (*cpi) -> support . end( ); rmpi ++ )
	    cout << ((*rmpi) -> exclude ? '-' : '+') << (*rmpi) -> id << ' ';
	  cout << "\nD: ";
	  for (rmpi  = (*cpi) -> discount . begin( );
	       rmpi != (*cpi) -> discount . end( ); rmpi ++ )
	    cout << ((*rmpi) -> exclude ? '-' : '+') << (*rmpi) -> id << ' ';
	  cout << endl << endl;
	  */
	}
    }
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-b path       The path of the AMOS bank to use for mate-pair info\n"
    << "-c            Circular reference sequence\n"
    << "-C            Print conflicts\n"
    << "-h            Display help information\n"
    << "-M            Print read maps\n"
    << "-r            Randomly place repetitive reads into one of their copy\n"
    << "              locations if it cannot be placed via mate-pair info\n"
    << "-s uint       Set random generator seed to unsigned int. Default\n"
    << "              seed is generated by the system clock\n"
    << "-T            Print TIGR contig\n"
    << "-U            Print UMD contig\n\n";

  cerr
    << "  Position query sequences on a reference based on the alignment\n"
    << "information contained in the deltafile. Can utilize mate-pair\n"
    << "information to place repetitive sequences, or random placement\n"
    << "to simulate even coverage.\n\n";

  return;
}




//---------------------------------------------------------- PrintMapping ----//
void PrintMapping (Mapping_t & mapping)
{
  ReadMap_t temprm;
  temprm . place = new ReadAlignChain_t( );
  temprm . place -> head = new ReadAlign_t( );

  map<string, Reference_t>::iterator rmi;
  vector<ReadMap_t *>::iterator rmpi;

  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;


  //-- For each reference
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); rmi ++ )
    {
      //-- Find the reference range
      temprm . place -> head -> ref = &(rmi -> second);
      rmpip = equal_range (mapping . reads . begin( ), mapping . reads . end( ),
			   &temprm, ReadReferenceCmp_t( ));

      cout << '>' << rmi -> first << '\t'
	   << (rmi -> second) . len
	   << endl;

      //-- For each read mapping to this reference
      for ( rmpi = rmpip . first; rmpi != rmpip . second; rmpi ++ )
	{
	  cout << (*rmpi) -> id << '\t'
	       << (*rmpi) -> place -> beg << '\t'
	       << ((*rmpi) -> place -> head -> ori == FORWARD_CHAR ? '+' : '-');
	  if ( (*rmpi) -> mate . read == NULL )
	    cout << "\tNULL";
	  else
	    cout << '\t' << (*rmpi) -> mate . read -> id;
	  cout << endl;
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
	cpi != assembly . contigs . end( ); cpi ++ )
    {
      if ( (*cpi) -> tiles . size( ) <= 1 )
	{
	  if ( (*cpi) -> tiles . empty( ) )
	    cerr << "WARNING: empty contig\n";
	  continue;
	}

      cout
	<< "##" << ++ctgs << ' '
	<< (*cpi) -> tiles . size( ) << ' '
	<< (*cpi) -> len << " 00000000 checksum."
	<< endl;

      for ( tpi = (*cpi) -> tiles . begin( );
	    tpi != (*cpi) -> tiles . end( ); tpi ++ )
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
	cpi != assembly . contigs . end( ); cpi ++ )
    {
      if ( (*cpi) -> tiles . size( ) <= 1 )
	{
	  if ( (*cpi) -> tiles . empty( ) )
	    cerr << "WARNING: empty contig\n";
	  continue;
	}

      cout
	<< "C " << ++ctgs << '\t'
	<< (*cpi) -> tiles . front( ) -> read -> place -> beg << endl;

      for ( tpi  = (*cpi) -> tiles . begin( );
	    tpi != (*cpi) -> tiles . end( ); tpi ++ )
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




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  [-b amosbank]  <deltafile>\n\n";
  return;
}




//------------------------------------------------------- RefineConflicts ----//
void RefineConflicts (Mapping_t & mapping)
{
  ReadMap_t temprm;
  temprm . place = new ReadAlignChain_t( );
  temprm . place -> head = new ReadAlign_t( );

  long int cpos, beg, end;
  Reference_t * rp;
  ReadAlign_t * rap;

  list<Conflict_t *>::iterator cpi, locpi, hicpi, bcpi, ecpi;
  map<string, Reference_t>::iterator rmi;
  vector<ReadMap_t *>::iterator rmpi, rmpie, ri;
  pair<
    vector<ReadMap_t *>::iterator,
    vector<ReadMap_t *>::iterator
    > rmpip;

  vector<ReadMap_t *> heap;
  vector<Conflict_t *> breaks;
  vector<Conflict_t *>::iterator bi;


  //-- For each reference and its conflicts
  for ( rmi  = mapping . references . begin( );
	rmi != mapping . references . end( ); rmi ++ )
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
      for ( cpi = bcpi; cpi != ecpi; cpi ++ )
	{
	  if ( (*cpi) -> type == Conflict_t::INDEL )
	    continue;

	  cpos = (*cpi) -> pos;

	  //-- Update the heap to current conflict pos
	  while ( !heap . empty( )  &&  heap . front( ) -> place -> end < cpos )
	    PopHeap (heap);

	  //-- Load up the heap with reads that overlap the conflict
	  for ( ; rmpi != rmpie  &&  (*rmpi) -> place -> beg <= cpos; rmpi ++ )
	    if ( (*rmpi) -> place -> end >= cpos )
	      PushHeap (heap, *rmpi);

	  //-- Collect the nay counts, i.e. reads that do not agree on break
	  for ( ri = heap . begin( ); ri != heap . end( ); ri ++ )
	    for ( rap = (*ri) -> place -> head; rap != NULL; rap = rap->from )
	      if ( rap -> loR < cpos - ERRLEN  &&  rap -> hiR > cpos + ERRLEN )
		{
		  (*cpi) -> discount . push_back (*ri);
		  break;
		}

	  //-- Sort and unique-ify the support vectors
	  sort ((*cpi) -> support . begin( ), (*cpi) -> support . end( ));
	  ri = unique ((*cpi)->support . begin( ), (*cpi)-> support . end( ));
	  if ( ri != (*cpi) -> support . end( ) )
	    (*cpi) -> support . erase (ri, (*cpi) -> support . end( ));
	  sort ((*cpi) -> discount . begin( ), (*cpi) -> discount . end( ));
	  ri = unique ((*cpi)->discount . begin( ), (*cpi)-> discount . end( ));
	  if ( ri != (*cpi) -> discount . end( ) )
	    (*cpi) -> discount . erase (ri, (*cpi) -> discount .end( ));

	  //-- Decide whether or not BREAK is real
	  if ( (*cpi) -> support . size( ) > (*cpi) -> discount . size( ) )
	    (*cpi) -> status = Conflict_t::SUPPORTED;
	  else
	    (*cpi) -> status = Conflict_t::UNSUPPORTED;
	}
      heap . clear( );


      //-- For each *INDEL*, sum yay/nay counts from its break counts
      for ( cpi = bcpi; cpi != ecpi; cpi ++ )
	{
	  if ( (*cpi) -> type != Conflict_t::INDEL )
	    continue;

	  //-- The reference breaks for the gap
	  beg = (*cpi) -> pos;
	  end = (*cpi) -> pos + (*cpi) -> gapR + 1;

	  hicpi = locpi = cpi;
	  while ( hicpi != bcpi  &&  beg - (*hicpi) -> pos <= ERRLEN )
	    hicpi --;
	  while ( locpi != bcpi  &&  end - (*locpi) -> pos <= ERRLEN )
	    locpi --;

	  //-- Find hang conflicts corresponding to the reference HIBREAK
	  for ( ; hicpi != ecpi  &&  (*hicpi) -> pos - beg <= ERRLEN; hicpi ++ )
	    if ( hicpi != cpi  &&  labs (beg - (*hicpi) -> pos) <= ERRLEN )
	      {
		breaks . push_back (*hicpi);

		if ( (*hicpi) -> type == Conflict_t::HIBREAK )
		  {
		    //-- Add to INDEL support if hang isn't too large
		    if ( (*cpi)->support.size( ) >= (unsigned int)MININDELS )
		      for ( ri  = (*hicpi) -> support . begin( );
			    ri != (*hicpi) -> support . end( ); ri ++ )
			if ( (*ri)->place->end - beg < (*cpi)->gapQ + TRMLEN )
			  (*cpi) -> support . push_back (*ri);

		    //-- Add to INDEL discount
		    for ( ri  = (*hicpi) -> discount . begin( );
			  ri != (*hicpi) -> discount . end( ); ri ++ )
		      (*cpi) -> discount . push_back (*ri);
		  }
	      }

	  //-- Find hang conflicts corresponding to the reference LOBREAK
	  for ( ; locpi != ecpi  &&  (*locpi) -> pos - end <= ERRLEN; locpi ++ )
	    if ( locpi != cpi  &&  labs (end - (*locpi) -> pos) <= ERRLEN )
	      {
		breaks . push_back (*locpi);

		if ( (*locpi) -> type == Conflict_t::LOBREAK )
		  {
		    //-- Add to INDEL support if hang isn't too large
		    if ( (*cpi)->support.size( ) >= (unsigned int)MININDELS )
		      for ( ri  = (*locpi) -> support . begin( );
			    ri != (*locpi) -> support . end( ); ri ++ )
			if ( end - (*ri)->place->beg < (*cpi)->gapQ + TRMLEN )
			  (*cpi) -> support . push_back (*ri);
		    
		    //-- Add to INDEL discount
		    for ( ri  = (*locpi) -> discount . begin( );
			  ri != (*locpi) -> discount . end( ); ri ++ )
		      (*cpi) -> discount . push_back (*ri);
		  }
	      }

	  //-- Sort and unique-ify the support vectors
	  sort ((*cpi) -> support . begin( ), (*cpi) -> support . end( ));
	  ri = unique ((*cpi)->support . begin( ), (*cpi)-> support . end( ));
	  if ( ri != (*cpi) -> support . end( ) )
	    (*cpi) -> support . erase (ri, (*cpi) -> support . end( ));
	  sort ((*cpi) -> discount . begin( ), (*cpi) -> discount . end( ));
	  ri = unique ((*cpi)->discount . begin( ), (*cpi)-> discount . end( ));
	  if ( ri != (*cpi) -> discount . end( ) )
	    (*cpi) -> discount . erase (ri, (*cpi) -> discount .end( ));

	  //-- Decide whether or not INDEL is real
	  if ( (*cpi) -> support . size( ) > (*cpi) -> discount . size( ) )
	    (*cpi) -> status = Conflict_t::SUPPORTED;
	  else
	    (*cpi) -> status = Conflict_t::UNSUPPORTED;

	  //-- If INDEL is real, flag its BREAKS as artificial
	  if ( (*cpi) -> status == Conflict_t::SUPPORTED )
	    for ( bi = breaks . begin( ); bi != breaks . end( ); bi ++ )
	      {
		//TODO - CHECK FOR BEST INDEL IF BREAK==INDEL
		if ( (*bi) -> type == Conflict_t::INDEL )
		  (*bi) -> status = Conflict_t::UNSUPPORTED;
		else
		  (*bi) -> status = Conflict_t::ARTIFICIAL;
	      }

	  breaks . clear( );
	}


      //-- EXCLUDE all "inconsistent" reads
      for ( cpi = bcpi; cpi != ecpi; cpi ++ )
	if ( (*cpi) -> status == Conflict_t::ARTIFICIAL )
	  for ( ri  = (*cpi) -> support . begin( );
		ri != (*cpi) -> support . end( ); ri ++ )
	    (*ri) -> exclude = true;

      for ( cpi = bcpi; cpi != ecpi; cpi ++ )
	if ( (*cpi) -> status == Conflict_t::SUPPORTED )
	  {
	    for ( ri  = (*cpi) -> discount . begin( );
		  ri != (*cpi) -> discount . end( ); ri ++ )
	      (*ri) -> exclude = true;
	    for ( ri  = (*cpi) -> support . begin( );
		  ri != (*cpi) -> support . end( ); ri ++ )
	      (*ri) -> exclude = false;
	  }

      for ( cpi = bcpi; cpi != ecpi; cpi ++ )
	if ( (*cpi) -> status == Conflict_t::UNSUPPORTED )
	  for ( ri  = (*cpi) -> support . begin( );
		ri != (*cpi) -> support . end( ); ri ++ )
	    (*ri) -> exclude = true;
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
      assert ( rap -> from -> lo  < rap -> lo  );

      olap1 = rap -> from -> hiR - rap -> loR + 1;
      if ( olap1 < 0  ||
	   (OPT_Circular  &&  olap1 > rap -> ref -> len / 2) )
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
