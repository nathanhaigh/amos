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
#include "Layout_AMOS.hh"
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
//! getUngapped... methods. Gap characters should be '-' but '*' is also
//! accepted. The compress and uncompress methods inherited from Sequence_t
//! are made private because they would corrupt the gap characters.
//!
//==============================================================================
class Contig_t : public Sequence_t
{
  
private:
  bool gapsvalid_m;                   //<! indicates if gaps_m is up to date
  std::vector<Pos_t> gaps_m;          //!< consensus gaps
  std::vector<Tile_t> reads_m;        //!< read tiling
  ID_t scf_m;                         //!< the IID of the parent scaffold 

  //--------------------------------------------------- compress ---------------
  //! \brief Reimplemented from Sequence_t as private to prohibit use
  //!
  void compress ( );


  //--------------------------------------------------- uncompress -------------
  //! \brief Reimplemented from Sequence_t as private to prohibit use
  //!
  void uncompress ( );


  //--------------------------------------------------- indexGaps --------------
  //! \brief Records the positions of the consensus gaps for fast coordinate translation
  //!
  void indexGaps();

protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);


  //--------------------------------------------------- readRecordFix ----------
  virtual void readRecordFix (std::istream & fix);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object


  //--------------------------------------------------- Contig_t ---------------
  //! \brief Constructs an empty Contig_t object
  //!
  Contig_t ( )
   : gapsvalid_m(false), scf_m(NULL_ID)
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
    gaps_m . clear( );
    reads_m . clear( );
    gapsvalid_m = false;
    scf_m = NULL_ID;
  }

  void setScf(ID_t &id) {
     scf_m = id;
  }

  ID_t getScf() {
     return scf_m;
  }

  //--------------------------------------------------- gap2ungap --------------
  //! \brief Translates a 0-based gapped position (offset) to a 1-based ungapped position (sequence coordinate)
  //!
  //! This method requires O(num gaps). If the gapped position points to a gap
  //! the returned ungapped position will point to the base immediately
  //! preceeding the gap.
  //!
  //! \param The gapped position
  //! \pre gap < getLength( )
  //! \return The ungapped position
  //!
  Pos_t gap2ungap (Pos_t gap);


  //--------------------------------------------------- ungap2gap --------------
  //! \brief Translates an 1-based ungapped position to a 0-based gapped position
  //!
  //! This method requires O(number gaps) time.
  //!
  //! \param The ungapped position
  //! \pre ungap < getUngappedLength( )
  //! \return The gapped position
  //!
  Pos_t ungap2gap (Pos_t ungap);
  
  //--------------------------------------------------- getGCContent --------------
  //! \brief Get the gc content of the sequence
  //!
  //! \return The gc content for the specified range of the sequence
  //!
  virtual double getGCContent (const Range_t & rng) const
  {
    int gc = 0;
    int all = 0;

    int start = rng.getLo();
    int stop = rng.getHi();

    if (stop > length_m)
      AMOS_THROW_ARGUMENT("Can't compute GC Content beyond sequence length");

    // skip ambiguities, gaps

    for (int i = start; i < stop; i++)
    {
      switch(seq_m[i])
      {
        case 'A':
        case 'a': 
        case 'T':
        case 't': all++; break;

        case 'C':
        case 'c':
        case 'G':
        case 'g': all++; gc++; break;
      };
    }

    return (all) ? ((double)gc)/all : 0.0;
  }

  //--------------------------------------------------- getGCContent --------------
  //! \brief Get the gc content of the sequence
  //!
  //! \return The gc content of the full range of the sequence
  //!
  virtual double getGCContent ( ) const
  {
    return getGCContent(Range_t(0, length_m));
  }



  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Contig_t::NCODE;
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
  std::vector<Tile_t> & getReadTiling ( )
  {
    return reads_m;
  }


  //--------------------------------------------------- getSpan ----------------
  //! \brief Get the span of the read layout
  //!
  //! Returns the difference between min(offset) and max(offset+len). Since the
  //! layout is not necessarily sorted, this method requires linear time.
  //!
  //! \return The span of the layout
  //!
  Size_t getSpan ( ) const;


  //--------------------------------------------------- getUngappedLength ------
  //! \brief Get the ungapped consensus length
  //!
  //! Unlike getLength( ) which is constant time, this method requires linear
  //! time.
  //!
  //! \return The ungapped length of the consensus sequence
  //!
  Size_t getUngappedLength ( ) const;


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
  //! Returns a subrange of quality scores [begin, end) or (end, begin] with
  //! all the gap scores removed. The range bounds are relative to the gapped
  //! consensus sequence, and a reversed range will pull the reverse string of
  //! quality scores.
  //!
  //! \param range The range of quality scores to get
  //! \pre range lo >= 0 && hi <= length
  //! \throws ArgumentException
  //! \return A subrange of ungapped quality scores
  //!
  std::string getUngappedQualString (Range_t range) const;


  //--------------------------------------------------- getUngappedSeqString ---
  //! \brief Get the ungapped sequence base string
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
  //! Returns a subrange of ungapped sequence bases [begin, end) or (end, begin]
  //! with all the gaps (non-alphas) removed. The range bounds are relative to
  //! the gapped consensus sequence, and reversed range will pull the reverse
  //! complement string of sequence bases.
  //!
  //! \param range The range of sequence bases to get
  //! \pre range lo >= 0 && hi <= length
  //! \throws ArgumentException
  //! \return A subrange of ungapped sequence bases
  //!
  std::string getUngappedSeqString (Range_t range) const;



  //--------------------------------------------------- insertGapColumn -------
  //! \brief Insert a gap column at the specified position in the contig
  //!
  //! Inserts a gap into the consensus and tiling reads at the specified position.
  //! The downstream reads are shifted appropriately, and the consensus grows by
  //! 1bp.
  //!
  //! \param gindex The gapped consensus position to insert the gap column
  //! \pre 0 <= gindex <= |consensus|
  //! \throws ArgumentException
  //! \return void
  //!
  void insertGapColumn (Pos_t gindex);



  //--------------------------------------------------- getFoldCov -------------
  //! \brief Get the fold coverage of the contig sequence
  //!
  //! Example: A 1,000 bp long contig with 100x 100bp reads has a 10x coverage.
  //!
  //! \return Fold coverage value
  //!
  double getFoldCov ( ) const;



  //--------------------------------------------------- getCovStat -------------
  //! \brief Get the cov stat of the sequence
  //!
  //! \return The coverage stat for the sequence
  //!
  double getCovStat(const double globalArrivalRate) const;



  //--------------------------------------------------- getAvgRho --------------
  //! \brief Get the length of the sequence with more than 1X coverage
  //!
  //! \return The length for the sequence
  //!
  double getAvgRho( ) const;



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

  //--------------------------------------------------- reverseComplement ------
  //! \brief Reverse Complement the contig and layout of reads
  //!
  //! Reverse complements the consensus, reverses the consensus quality values
  //! and flips the orientations of the tiling reads. Note: The scaffold and contig
  //! edges are NOT updated.
  //! \return void
  //!
  void reverseComplement();


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


  //--------------------------------------------------- setReadTiling ----------
  //! \brief Set the tiling of underlying reads
  //!
  //! \param layout The new read layout
  //! \return void
  //!
  void setReadTiling (const Layout_t & layout)
  {
    reads_m = layout . getTiling( );
  }

  //--------------------------------------------------- setSequence -----------
  //! \brief Set the consensus sequence of the contig
  //!
  //! See Sequence_t::setSequence()
  //!
  void setSequence (const char * seq, const char * qual)
  {
    gapsvalid_m = false;
    Sequence_t::setSequence(seq, qual);
  }

  //--------------------------------------------------- setSequence -----------
  //! \brief Set the consensus sequence of the contig
  //!
  //! See Sequence_t::setSequence()
  //!
  void setSequence (const std::string & seq, const std::string & qual)
  {
    gapsvalid_m = false;
    Sequence_t::setSequence(seq, qual);
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
