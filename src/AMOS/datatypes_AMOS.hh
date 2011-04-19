////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/16/2003
//!
//! \brief Include file for some simple AMOS type definitions and structs
//!
//! These are objects that cannot be stored by themselves in a Bank, however
//! are capable of writing messages and records. Mostly used as ID-less
//! submessages.
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __datatypes_AMOS_HH
#define __datatypes_AMOS_HH 1

#include "Message_AMOS.hh"
#include <vector>
#include <string>





//-- Miscellaneous types and definitions
namespace AMOS {

//================================================ Distribution_t ==============
//! \brief A set of statistics that define a distribution
//!
//! Additional parameters may be added to this class if more complex
//! distributions need to be represented. The contained parameters should
//! suffice to represent normal distributions.
//!
//! \todo make more general for more complex distributions
//==============================================================================
struct Distribution_t : public IMessagable_t
{

  Pos_t mean;           //!< the mean
  SD_t sd;              //!< the standard deviation


  static const NCode_t NCODE;
  //!< The NCode type identifier for this object


  //--------------------------------------------------- Distribution_t ---------
  //! \brief Constructs an empty Distribution_t object
  //!
  //! Sets all members to 0
  //!
  Distribution_t ( )
  {
    clear( );
  }


  //--------------------------------------------------- ~Distribution_t --------
  //! \brief Destroys a Distribution_t object
  //!
  ~Distribution_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( )
  {
    mean = 0; sd = 0;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Distribution_t::NCODE;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- readRecord -------------
  //! \brief Read a binary record
  //!
  void readRecord (std::istream & in);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write a binary record
  //!
  void writeRecord (std::ostream & out) const;

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
    clear( );
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


  //--------------------------------------------------- getLo ------------------
  //! \brief Get the lo range position, (begin < end ? begin : end)
  //!
  Pos_t getLo ( ) const
  {
    return (begin < end ? begin : end);
  }


  //--------------------------------------------------- getHi ------------------
  //! \brief Get the hi range position, (begin > end ? begin : end)
  //!
  Pos_t getHi ( ) const
  {
    return (begin > end ? begin : end);
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


  //--------------------------------------------------- isReverse --------------
  //! \brief Returns true if begin > end, otherwise false
  //!
  //! \return true if begin > end, otherwise false
  //!
  bool isReverse ( ) const
  {
    return begin > end;
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


  //--------------------------------------------------- swap -------------------
  //! \brief Exchange the beginning and end of the range
  //!
  void swap ( )
  {
   Pos_t save = begin;
   begin = end;
   end = save;
  }

};




//================================================ Tile_t ======================
//! \brief A singe sequence's location and orientation in a larger sequence
//!
//! A tiled sequence, like a read in a contig, or a contig in a scaffold. For
//! explanation purposes, let us assume we are dealing with an underlying read
//! in a contig. The id is the IID of the read. The gaps are the alignment gaps
//! in the read, stored as a list of gap positions. How these positions are
//! stored relative to the parent contig is left to the user. Commonly, each
//! gap integer is simply the gap location in the read. Thus, two adjacent gaps
//! would share the same value. The offset is the offset of the beginning on
//! the read in the contig, thus an offset of 0 would signify a read beginning
//! at the very start of the contig. Finally, the range is the portion of the
//! read that has been aligned to the contig, with the range order also
//! specifying the orientation of the read relative to the contig, i.e. if the
//! beginning of the range is greater than the end of the range the read is
//! reversed relative to the forward strand of the contig.
//!
//==============================================================================
struct Tile_t : public IMessagable_t
{
  ID_t source;                //!< source of the tile 
  NCode_t source_type;        //!< the type of tile source (read, contig, or motif
  std::vector<Pos_t> gaps;    //!< the absolute gap positions
  Pos_t offset;               //!< the offset of the tile
  Range_t range;              //!< the usable range of the tile


  static const NCode_t NCODE;
  //!< The NCode type identifier for this object


  //--------------------------------------------------- Tile_t -----------------
  //! \brief Constructs an empty Tile_t object
  //!
  //! Sets id to NULL_ID, offset to 0
  //!
  Tile_t ( )
  {
    source = NULL_ID;
    source_type = NULL_NCODE;
    offset = 0;
  }


  //--------------------------------------------------- ~Tile_t ----------------
  //! \brief Destroys a Tile_t object
  //!
  ~Tile_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( );


  //--------------------------------------------------- getGappedLength --------
  //! \brief Gets the gapped length of the tile
  //!
  //! (range . getLength( ) + gaps . size( ))
  //!
  //! \return The gapped length of the tile
  //!
  Size_t getGappedLength ( ) const
  {
    return (range . getLength( ) + gaps . size( ));
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Tile_t::NCODE;
  }


  //--------------------------------------------------- getRightOffset --------
  //! \brief Gets the exact right offset of the tile
  //!
  //! offset + (range . getLength( ) + gaps . size( )) - 1
  //!
  //! \return The right offset of the tile
  //!

  Pos_t getRightOffset() const
  {
    return offset + range.getLength() + gaps.size() - 1;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- readRecord -------------
  //! \brief Read a binary record
  //!
  void readRecord (std::istream & in);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write a binary record
  //!
  void writeRecord (std::ostream & out) const;

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
bool operator== (const Range_t & a, const Range_t & b);
bool operator!= (const Range_t & a, const Range_t & b);


//----------------------------------------------------- operator== -------------
//! \brief An equality operator for Distributions
//!
//! Returns true if distributions have identical values
//!
//! \param a Distribution A
//! \param b Distribution B
//! \return true if distributions have identical values
//!
bool operator== (const Distribution_t & a, const Distribution_t & b);
bool operator!= (const Distribution_t & a, const Distribution_t & b);


//----------------------------------------------------- operator== -------------
//! \brief An equality operator for Tiles
//!
//! Returns true if distributions have identical values
//!
//! \param a Tile A
//! \param b Tile B
//! \return true if distributions have identical values
//!
bool operator== (const Tile_t & a, const Tile_t & b);
bool operator!= (const Tile_t & a, const Tile_t & b);


//----------------------------------------------------- WrapStirng -------------
//! \brief Inserts a newline every 'per' characters when writing to stream
//!
void WrapString (std::ostream & out, const std::string & s, int per);


//--TEMPORARY
typedef Range_t Ordered_Range_t;

} // namespace AMOS

#endif // #ifndef __datatypes_AMOS_HH
