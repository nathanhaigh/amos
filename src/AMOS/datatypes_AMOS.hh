////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/16/2003
//!
//! \brief Include file for all of the AMOS datatypes
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __datatypes_AMOS_HH
#define __datatypes_AMOS_HH 1

//-- System includes
#include <inttypes.h>   //!< ints of the form [u]int(size)_t, i.e. int32_t

#include <vector>
#include <cstdlib>


//-- Integer types
typedef uint8_t   null_t;      //!< don't know which type yet (devel)
typedef uint32_t  ID_t;        //!< internal ID type (for internal IDs)
typedef uint32_t  SD_t;        //!< standard deviation type
typedef int32_t   Size_t;      //!< size type (for links,etc)
typedef int32_t   Pos_t;       //!< position type (in a sequence,etc)
                               //-- Pos_t must be >= Size_t


//-- Miscellaneous types and definitions
namespace AMOS {

const ID_t    NULL_ID     =  0;         //!< NULL ID definition
const char    MIN_QUALITY = '0';        //!< min quality score definition
const char    MAX_QUALITY = '0' + 63;   //!< max quality score definition




//================================================ Distribution_t ==============
//! \brief A set of statistics that define a distribution
//!
//! Additional parameters may be added to this class if more complex
//! distributions need to be represented. The contained parameters should
//! suffice to represent normal and skewed distributions.
//!
//! \todo make more general, however poisson and binomials can be approximated
//! with a normal distribution... so this might stay the same
//==============================================================================
struct Distribution_t
{

  uint32_t mean;        //!< the mean
  SD_t sd;              //!< the standard deviation
  float skew;           //!< the skewness


  //--------------------------------------------------- Distribution_t ---------
  //! \brief Constructs an empty Distribution_t object
  //!
  //! Sets all members to 0
  //!
  Distribution_t ( )
  {
    mean = 0; sd = 0; skew = 0;
  }


  //--------------------------------------------------- ~Distribution_t --------
  //! \brief Destroys a Distribution_t object
  //!
  ~Distribution_t ( )
  {

  }
};




//================================================ Range_t =====================
//! \brief A gapped coordinate range
//!
//! A range of values defined by the start and end gap numbers that bracket the
//! requested values with gap counting starting at 0 (the gap before the first
//! value). The minimum value points to the leftmost value in the list, while
//! the maximum value points to one after the rightmost value in the list.
//! For example, the range [2,5) for the list 0,1,2,3,4,5,6 would define the
//! sublist 2,3,4. Reversed ranges are also allowed, for example (5,2] would
//! define the subset 4,3,2.
//!
//==============================================================================
struct Range_t
{

  Pos_t begin;              //!< The begin of the range
  Pos_t end;                //!< The end of the range


  //--------------------------------------------------- Range_t ----------------
  //! \brief Constructs an empty Range_t object
  //!
  //! Sets begin and end to zero, thus creating a zero length range
  //!
  Range_t ( )
  {
    begin = end = 0;
  }


  //--------------------------------------------------- Range_t ----------------
  //! \brief Copy constructor
  //!
  //! \param source The range to be copied
  //!
  Range_t (const Range_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- Range_t ----------------
  //! \brief Constructs a Range_t object
  //!
  //! Sets begin and end to the parameter values
  //!
  //! \param b Construct value of begin
  //! \param e Construct value of end
  //!
  Range_t (Pos_t b, Pos_t e)
  {
    begin = b;
    end = e;
  }


  //--------------------------------------------------- ~Range_t ---------------
  //! \brief Destroys a Range_t object
  //!
  ~Range_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears the range to an empty range [0,0)
  //!
  void clear ( )
  {
    begin = end = 0;
  }


  //--------------------------------------------------- getBegin ---------------
  //! \brief Get the beginning of the range
  //!
  Pos_t getBegin ( ) const
  {
    return begin;
  }


  //--------------------------------------------------- getEnd -----------------
  //! \brief Get the end of the range
  //!
  Pos_t getEnd ( ) const
  {
    return end;
  }


  //--------------------------------------------------- getLength --------------
  //! \brief Get the length of the range
  //!
  //! \return abs (end - begin)
  //!
  Size_t getLength ( ) const
  {
    return labs (end - begin);
  }


  //--------------------------------------------------- setBegin ---------------
  //! \brief Set the beginning of the range
  //!
  void setBegin (Pos_t b)
  {
    begin = b;
  }


  //--------------------------------------------------- setEnd -----------------
  //! \brief Set the end of the range
  //!
  void setEnd (Pos_t e)
  {
    end = e;
  }


  //--------------------------------------------------- setRange ---------------
  //! \brief Set the whole range
  //!
  void setRange (Pos_t b, Pos_t e)
  {
    begin = b;
    end = e;
  }

};




//================================================ Tile_t ======================
//! \brief A singe sequence's location and orientation in a larger sequence
//!
//! A tiled sequence, like a read in a contig, or a contig in a scaffold. For
//! explanation purposes, let us assume we are dealing with an underlying read
//! in a contig. The id is the ID of the read. The gaps are the alignment gaps
//! in the read, stored in a delta encoded fasion where positive numbers are
//! the distance to the next insertion in the contig and negative numbers are
//! the distance to the next deletion (gap) in the contig. If the user only
//! wishes to store the location of the read gaps, then only positive numbers
//! should be used, with each integer indicating the absence of a base, i.e.
//! a gap in the read. The offset is the offset of the beginning on the read in
//! the contig, thus an offset of 0 would signify a read beginning at the very
//! start of the contig. Finally, the range is the portion of the read that has
//! been aligned to the contig, with the range order also specifying the
//! orientation of the read relative to the contig, i.e. if the beginning of
//! the range is greater than the end of the range the read is reversed
//! relative to the forward strand of the contig.
//!
//! \note It was suggested to only store gaps in the tile and not in the larger
//! sequence, however by allowing the user to store both tile and sequence gaps
//! (both positive and negative ints) the Tile struct can represent both
//! read->consensus mappings (that only stores read gaps, since the consensus
//! is stored w/ gaps) and contig->finished mappings generated by a pairwise
//! alignment algorithm (where both gaps in the contig and finished sequence
//! need to be stored).
//==============================================================================
struct Tile_t
{
  ID_t id;                    //!< the ID of the tiled sequence
  std::vector<int32_t> gaps;  //!< the list of gap positions
  Pos_t offset;               //!< the offset of the tile
  Range_t range;              //!< the usable range of the tile


  //--------------------------------------------------- Tile_t -----------------
  //! \brief Constructs an empty Tile_t object
  //!
  //! Sets id to NULL_ID, offset to 0
  //!
  Tile_t ( )
  {
    id = NULL_ID;
    offset = 0;
  }


  //--------------------------------------------------- ~Tile_t ----------------
  //! \brief Destroys a Tile_t object
  //!
  ~Tile_t ( )
  {

  }

};




//================================================ Helper Functions ============
//----------------------------------------------------- operator& --------------
//! \brief An intersection operator for Ranges
//!
//! Returns the (foward) intersection of two overlapping ranges.
//!
//! \param a Range A
//! \param b Range B
//! \return The intersection of Range A and B or [0,0) if no overlap
//!
Range_t operator& (Range_t a, Range_t b);


//----------------------------------------------------- operator| --------------
//! \brief A union operator for Ranges
//!
//! Returns the union of two overlapping ranges
//!
//! \param a Range A
//! \param b Range B
//! \return The union of Range A and B or [0,0) if no overlap
//!
Range_t operator| (Range_t a, Range_t b);


//----------------------------------------------------- operator== -------------
//! \brief An equality operator for Ranges
//!
//! Returns true if the two ranges have equal begin and end values.
//!
//! \param a Range A
//! \param b Range B
//! \return true if a.begin == b.begin && a.end == b.end, else false
//!
bool operator== (Range_t a, Range_t b);


//--TEMPORARY
typedef Range_t Ordered_Range_t;

} // namespace AMOS


//-- Primary (bankable) types, include
#ifndef __NO_BANKABLE_TYPES
#include "Bankable_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include "Contig_AMOS.hh"
#include "Fragment_AMOS.hh"
#include "Kmer_AMOS.hh"
#include "Library_AMOS.hh"
#include "Overlap_AMOS.hh"
#include "Read_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Sequence_AMOS.hh"
#endif // #ifndef __NO_BANKABLE_TYPES

#endif // #ifndef __datatypes_AMOS_HH
