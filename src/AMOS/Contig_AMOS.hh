////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Contig_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Contig_AMOS_HH
#define __Contig_AMOS_HH 1

#include "Sequence_AMOS.hh"
#include <vector>
#include <cctype>




namespace AMOS {

//================================================ Contig_t ====================
//! \brief A tiling of assembled sequence reads
//!
//! A Contig_t consists of a consensus sequence with quality scores and a
//! tiling of underlying reads that produce the consensus. The consensus
//! sequence is stored as a 'gapped consensus'. That is the gaps are stored as
//! gap characters in the consensus sequence instead of as a position list.
//! The ungapped version of the consensus can be generated with the
//! getUngapped... methods. Gap characters can be any non-alpha, usu. '-'.
//! The compress and uncompress methods inherited from Sequence_t are made
//! private because they would corrupt the gap characters.
//!
//==============================================================================
class Contig_t : public Sequence_t
{
  
private:

  null_t poly_m;                   //!< polymorphism
  std::vector<Tile_t> reads_m;     //!< read tiling


  //--------------------------------------------------- compress ---------------
  //! \brief Reimplemented from Sequence_t as private to prohibit use
  //!
  void compress ( )
  {
    Sequence_t::compress( );
  }


  //--------------------------------------------------- uncompress -------------
  //! \brief Reimplemented from Sequence_t as private to prohibit use
  //!
  void uncompress ( )
  {
    Sequence_t::uncompress( );
  }


protected:

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

  //--------------------------------------------------- NCode ------------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Can be used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (Contig_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::CONTIG;
  }


  //--------------------------------------------------- Contig_t ---------------
  //! \brief Constructs an empty Contig_t object
  //!
  Contig_t ( )
  {
    poly_m = 0;
  }


  //--------------------------------------------------- Contig_t ---------------
  //! \brief Copy constructor
  //!
  Contig_t (const Contig_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Contig_t --------------
  //! \brief Destroys a Contig_t object
  //!
  ~Contig_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( )
  {
    Sequence_t::clear( );
    poly_m = 0;
    reads_m . clear( );
  }


  //--------------------------------------------------- getNCode ---------------
  //! \brief Get the AMOS NCode type identifier
  //!
  //! \return The AMOS NCode type identifier
  //!
  virtual NCode_t getNCode ( ) const
  {
    return Contig_t::NCode( );
  }


  //--------------------------------------------------- getPolymorphism --------
  //! \brief Get polymorphism information for this contig
  //!
  //! \note datatype not yet decided upon
  //!
  //! \return The polymorphism info
  //!
  null_t & getPolymorphism ( )
  {
    return poly_m;
  }


  //--------------------------------------------------- getReadTiling ----------
  //! \brief Get the tiling of underlying reads
  //!
  //! \return The vector of underlying reads
  //!
  const std::vector<Tile_t> & getReadTiling ( ) const
  {
    return reads_m;
  }


  //--------------------------------------------------- getUngappedQualString --
  //! \brief Get the ungapped quality score string
  //!
  //! Returns the quality string with all the gap scores removed.
  //!
  //! \return The full string of ungapped quality scores
  //!
  std::string getUngappedQualString ( ) const
  {
    return getUngappedQualString (Range_t (0, getLength( )));
  }


  //--------------------------------------------------- getUngappedQualString --
  //! \brief Get an ungapped quality score substring
  //!
  //! Returns a subrange of quality scores [left, right) with all the gap
  //! scores removed. The range bounds are relative to the gapped consensus
  //! sequence.
  //!
  //! \param range The range of quality scores to get
  //! \pre range begin <= end
  //! \pre range begin >= 0 && end <= length
  //! \throws ArgumentException
  //! \return A subrange of ungapped quality scores
  //!
  std::string getUngappedQualString (Range_t range) const;


  //--------------------------------------------------- getUngappedSeqString ---
  //! \brief Get the ungapped sequence base string
  //!
  //! Returns the sequence string with all the gaps (non-alphas) removed.
  //!
  //! \return The full string of ungapped sequence bases
  //!
  std::string getUngappedSeqString ( ) const
  {
    return getUngappedSeqString (Range_t (0, getLength( )));
  }


  //--------------------------------------------------- getUngappedSeqString ---
  //! \brief Get an ungapped sequence base substring
  //!
  //! Returns a subrange of ungapped sequence bases [left, right) with all the
  //! gaps (non-alphas) removed. The range bounds are relative to the gapped
  //! consensus sequence.
  //!
  //! \param range The range of sequence bases to get
  //! \pre range begin <= end
  //! \pre range begin >= 0 && end <= length
  //! \throws ArgumentException
  //! \return A subrange of ungapped sequence bases
  //!
  std::string getUngappedSeqString (Range_t range) const;


  //--------------------------------------------------- setPolymorphism --------
  //! \brief Set polymorphism information for this contig
  //!
  //! \note datatype not yet decided upon
  //!
  //! \param poly The new polymorphism info
  //! \return void
  //!
  void setPolymorphism (const null_t & poly)
  {
    poly_m = poly;
  }


  //--------------------------------------------------- setReadTiling ----------
  //! \brief Set the tiling of underlying reads
  //!
  //! \param reads The new vector of underlying reads
  //! \return void
  //!
  void setReadTiling (const std::vector<Tile_t> & reads)
  {
    reads_m = reads;
  }

};

} // namespace AMOS

#endif // #ifndef __Contig_AMOS_HH
