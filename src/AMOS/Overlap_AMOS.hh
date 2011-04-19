////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Overlap_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Overlap_AMOS_HH
#define __Overlap_AMOS_HH 1

#include "Universal_AMOS.hh"




namespace AMOS {

typedef Adjacency_t OverlapAdjacency_t;

//================================================ Overlap_t ===================
//! \brief An overlap relation between to sequencing reads
//!
//! A similarity based overlap between two Read_t that suggests their relative
//! location (to one another) in the original sequence. If storing a large
//! number of overlaps in an AMOS bank, it is wise to omit the IID and EID
//! fields from the objects as these values will only be written to the bank
//! if they are non-NULL. Also, an extra bit flag (FlagC) is provided in
//! addition to FlagA and FlagB which are inherited.
//!
//==============================================================================
class Overlap_t : public Oriented_t
{

private:

  Size_t aHang_m;          //!< length of non-overlapping portion of A
  Size_t bHang_m;          //!< length of non-overlapping portion of B
  std::pair<ID_t, ID_t> reads_m;   //!< the pair of read IIDs
  uint32_t score_m;        //!< overlap score


protected:

  static const uint8_t FLAGC_BIT       = 0x8;  //!< C flag


  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);


  //--------------------------------------------------- readRecordFix ----------
  virtual void readRecordFix (std::istream & fix);

  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object

  static const OverlapAdjacency_t NULL_ADJACENCY = 0;
  static const OverlapAdjacency_t NORMAL         = 'N';  //!< E,B
  static const OverlapAdjacency_t ANTINORMAL     = 'A';  //!< B,E
  static const OverlapAdjacency_t INNIE          = 'I';  //!< E,E
  static const OverlapAdjacency_t OUTIE          = 'O';  //!< B,B


  //--------------------------------------------------- Overlap_t --------------
  //! \brief Constructs an empty Overlap_t object
  //!
  //! Sets all members to 0
  //!
  Overlap_t ( )
  {
    score_m = 0;
    aHang_m = bHang_m = 0;
    reads_m . first = reads_m . second = NULL_ID;
  }


  //--------------------------------------------------- Overlap_t --------------
  //! \brief Copy constructor
  //!
  Overlap_t (const Overlap_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Overlap_t -------------
  //! \brief Destroys a Overlap_t object
  //!
  ~Overlap_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Oriented_t::clear( );
    score_m = 0;
    aHang_m = bHang_m = 0;
    reads_m . first = reads_m . second = NULL_ID;
  }


  //--------------------------------------------------- flip -------------------
  //! \brief Flip the orientation of the overlap
  //!
  //! Reverses the order of the read IIDs and changes the overlap adjacency as
  //! altered by the new orientation of the reads. Does not alter adjacency
  //! if it is currently a NULL_ADJACENCY. Also, swaps the a/bHang values.
  //! After flip, NORMAL becomes ANTINORMAL, ANTINORMAL becomes NORMAL, and
  //! INNIE and OUTIE remain the same.
  //!
  //! \return void
  //!
  void flip ( );


  //--------------------------------------------------- getAhang ---------------
  //! \brief Get the A-hang overlap value
  //!
  //! Get the length of the non-overlapping portion of the first read.
  //!
  //! \return The A-hang overlap value
  //!
  Size_t getAhang ( ) const
  {
    return aHang_m;
  }


  //--------------------------------------------------- getBhang ---------------
  //! \brief Get the B-hang overlap value
  //!
  //! Get the length of the non-overlapping portion of the second read.
  //!
  //! \return The B-hang overlap value
  //!
  Size_t getBhang ( ) const
  {
    return bHang_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Overlap_t::NCODE;
  }


  //--------------------------------------------------- getReads ---------------
  //! \brief Get the pair of contig IIDs joined by this overlap
  //!
  //! \return The first and second read IIDs joined by this overlap
  //!
  std::pair<ID_t, ID_t> getReads ( ) const
  {
    return reads_m;
  }


  //--------------------------------------------------- getScore ---------------
  //! \brief Get the overlap score
  //!
  //! \return A 32-bit unsigned integer representing the overlap score
  //!
  uint32_t getScore ( ) const
  {
    return score_m;
  }


  //--------------------------------------------------- isFlagC ----------------
  //! \brief Check the value of flag C
  //!
  //! \return The value of flag C
  //!
  bool isFlagC ( ) const
  {
    return flags_m . nibble & FLAGC_BIT;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setAhang ---------------
  //! \brief Set the A-hang overlap value
  //!
  //! Set the length of the non-overlapping portion of the first read.
  //!
  //! \param aHang The new A-hang overlap value
  //! \return The new A-hang overlap value
  //!
  void setAhang (Size_t aHang)
  {
    aHang_m = aHang;
  }


  //--------------------------------------------------- setBhang ---------------
  //! \brief Set the B-hang overlap value
  //!
  //! Set the length of the non-overlapping portion of the second read.
  //!
  //! \param bHang The new B-hang overlap value
  //! \return The new B-hang overlap value
  //!
  void setBhang (Size_t bHang)
  {
    bHang_m = bHang;
  }


  //--------------------------------------------------- setFlagC ---------------
  //! \brief Set the value of flag C
  //!
  //! \param flag The new flag C value
  //! \return void
  //!
  void setFlagC (bool flag)
  {
    if ( flag )
      flags_m . nibble |= FLAGC_BIT;
    else
      flags_m . nibble &= ~FLAGC_BIT;
  }


  //--------------------------------------------------- setReads ---------------
  //! \brief Set the read IID pair for this overlap
  //!
  //! \param reads The new pair of read IIDs
  //!
  void setReads (std::pair<ID_t, ID_t> reads)
  {
    reads_m = reads;
  }


  //--------------------------------------------------- setScore ---------------
  //! \brief Set the overlap score
  //!
  //! \param score A 32-bit unsigned integer representing the overlap score
  //! \return void
  //!
  void setScore (uint32_t score)
  {
    score_m = score;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Overlap_AMOS_HH
