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
#include <utility>




namespace AMOS {

//================================================ Overlap_t ===================
//! \brief An overlap relation between to sequencing reads
//!
//! A similarity based overlap between two Read_t that suggests their relative
//! location (to one another) in the original sequence.
//!
//==============================================================================
class Overlap_t : public Universal_t
{

private:

  Size_t aHang_m;          //!< length of non-overlapping portion of A
  Size_t bHang_m;          //!< length of non-overlapping portion of B
  std::pair<ID_t, ID_t> reads_m;   //!< the pair of read IDs


protected:

  static const char NORMAL     = 'N';     //!< E,B
  static const char ANTINORMAL = 'A';     //!< B,E
  static const char INNIE      = 'I';     //!< E,E
  static const char OUTIE      = 'O';     //!< B,B

  static const uint8_t FIRST_BIT  = 0x1;  //!< adjacency info for 1st read
  static const uint8_t SECOND_BIT = 0x2;  //!< adjacency info for 2nd read


  //--------------------------------------------------- readRecord -------------
  //! \brief Read all the class members from a biserial record
  //!
  //! Reads the fixed and variable length streams from a biserial record and
  //! initializes all the class members to the values stored within. Used in
  //! translating a biserial Bankable object, and needed to retrieve objects
  //! from a Bank. Returned size of the record will only be valid if the read
  //! was successful, i.e. fix.good( ) and var.good( ).
  //!
  //! \note This method must be able to interpret the biserial record
  //! produced by its related function writeRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \pre The get pointer of fix is at the beginning of the record
  //! \pre The get pointer of var is at the beginning of the record
  //! \return size of read record (size of fix + size of var)
  //!
  virtual Size_t readRecord (std::istream & fix,
			     std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write all the class members to a biserial record
  //!
  //! Writes the fixed an variable length streams to a biserial record. Used in
  //! generating a biserial Bankable object, and needed to commit objects to a
  //! Bank. Will only write to the ready streams, but the size of the record
  //! will always be returned.
  //!
  //! \note This method must be able to produce a biserial record that can
  //! be read by its related funtion readRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \return size of written record (size of fix + size of var)
  //!
  virtual Size_t writeRecord (std::ostream & fix,
			      std::ostream & var) const;


public:

  //--------------------------------------------------- BankCode ---------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (Overlap_t::BankCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t BankCode ( )
  {
    return Bank_k::OVERLAP;
  }


  //--------------------------------------------------- Overlap_t --------------
  //! \brief Constructs an empty Overlap_t object
  //!
  //! Sets all members to 0
  //!
  Overlap_t ( )
  {
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
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( )
  {
    Universal_t::clear( );
    aHang_m = bHang_m = 0;
    reads_m . first = reads_m . second = NULL_ID;
  }


  //--------------------------------------------------- getAdjacency -----------
  //! \brief Get the overlapping ends of the two reads
  //!
  //! Get the ends of the first and second reads that are overlapping, i.e.
  //! (E,B), (E,E), (B,B), or (E,B), where (E,B) would mean the end of the
  //! first contig is overlapping the beginning of the second read.
  //!
  //! \return The pair of overlapping ends
  //!
  std::pair<char, char> getAdjacency ( ) const;


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
  //! \brief Get the AMOS NCode type identifier
  //!
  //! \return The AMOS NCode type identifier
  //!
  virtual NCode_t getNCode ( ) const
  {
    return Overlap_t::BankCode( );
  }


  //--------------------------------------------------- getReads ---------------
  //! \brief Get the pair of contig IDs joined by this overlap
  //!
  //! \return The first and second read IDs joined by this overlap
  //!
  std::pair<ID_t, ID_t> getReads ( ) const
  {
    return reads_m;
  }


  //--------------------------------------------------- setAdjacency -----------
  //! \brief Set the overlapping ends of the two reads
  //!
  //! Set the ends of the first and second reads that overlap, i.e.
  //! (E,B), (E,E), (B,B), or (E,B), where (E,B) would mean the end of the
  //! first read overlaps the beginning of the second read. 'B' denotes begin,
  //! while 'E' denotes end.
  //!
  //! \note Will store info in extra portion of BankableFlags
  //!
  //! \param adj The new adjacent ends (B or E) of the two contigs
  //! \pre Each char in the pair must be either 'B' or 'E'
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setAdjacency (std::pair<char, char> adj);


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


  //--------------------------------------------------- setReads ---------------
  //! \brief Set the read ID pair for this overlap
  //!
  //! \param reads The new pair of read IDs
  //!
  void setReads (std::pair<ID_t, ID_t> reads)
  {
    reads_m = reads;
  }

};

} // namespace AMOS

#endif // #ifndef __Overlap_AMOS_HH
