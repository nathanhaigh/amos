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
  virtual void readRecord (std::istream & fix,
			   std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix,
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
  virtual void clear ( )
  {
    Sequence_t::clear( );
    reads_m . clear( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Contig_t::NCode( );
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


  //--------------------------------------------------- getReadTiling ----------
  //! \brief Get the tiling of underlying reads
  //!
  //! \return The vector of underlying reads
  //!
  std::vector<Tile_t> & getReadTiling ( )
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


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- readUMD ----------------
  //! \brief Read a UMD contig message from an input stream
  //!
  //! Reads a University of Maryland style contig message and populates the
  //! appropriate fields with the values read from the stream. Will throw an
  //! exception if a message is found, but is not properly formatted. All
  //! fields not included in the message will be reinitialized. Contig EID will
  //! be set, and each tile will be stored with Read IID, read offset, and read
  //! range. Read range will be flipped to represent reverse complement.
  //!
  //! \param in The input stream to read from
  //! \pre The incoming message is properly formatted
  //! \throws IOException_t
  //! \return true if a message was read, false if no message read (EOF)
  //!
  bool readUMD (std::istream & in);


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


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;


  //--------------------------------------------------- writeUMD ---------------
  //! \brief Write a UMD contig message to an output stream
  //!
  //! Writes a University of Maryland style contig message to the output stream.
  //! Will throw an exception if there was an error trying to write to the
  //! output stream.
  //!
  //! \param out The output stream to write to
  //! \pre The ostream is open and writable
  //! \throws IOException_t
  //! \return void
  //!
  void writeUMD (std::ostream & out) const;

};

} // namespace AMOS

#endif // #ifndef __Contig_AMOS_HH
