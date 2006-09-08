////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Sequence_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Sequence_AMOS_HH
#define __Sequence_AMOS_HH 1

#include "Universal_AMOS.hh"
#include <string>




namespace AMOS {

//================================================ Sequence_t ==================
//! \brief Sequence bases with quality scores
//!
//! Stores both sequence and quality score data in a space efficient manner
//! (when compressed). Can represent any type of sequence data, but must always
//! be used with both sequence AND quality data. If just one of these two data
//! types is required use a simple character array instead. Can be used in
//! uncompressed mode where a base and its quality occupy 2 bytes, or a
//! compressed mode where a base and its quality are packed into a single byte.
//! In uncompressed mode, any characters are valid for bases and quality
//! scores, however in compressed mode, acceptable sequence bases are A,C,G,T
//! and N (case insensitive) and acceptable quality scores are between
//! MIN_QUALITY and MAX_QUALITY.
//!
//==============================================================================
class Sequence_t : public Universal_t
{

protected:

  uint8_t * seq_m;      //!< compressed seq and qual data or uncompressed seq
  uint8_t * qual_m;     //!< uncompressed qual data
  Size_t length_m;      //!< length of the sequence and quality data


  static const uint8_t COMPRESS_BIT  = 0x1;   //!< compressed sequence flag
  static const uint8_t ADENINE_BITS  = 0x0;   //!< 'A' bit
  static const uint8_t CYTOSINE_BITS = 0x40;  //!< 'C' bit
  static const uint8_t GUANINE_BITS  = 0x80;  //!< 'G' bit
  static const uint8_t THYMINE_BITS  = 0xC0;  //!< 'T' bit
  static const uint8_t SEQ_BITS      = 0xC0;  //!< sequence bit mask
  static const uint8_t QUAL_BITS     = 0x3F;  //!< quality bit mask


  //--------------------------------------------------- compress ---------------
  //! \brief Compresses a sequence char and quality char into a single byte
  //!
  //! \note Must work with uncompress(uint8_t) method
  //!
  //! \param seqchar The sequence base character (case insensitive)
  //! \param qualchar The quality score character
  //! \return The compressed byte
  //!
  static inline uint8_t compress (char seqchar, char qualchar)
  {
    //-- Force quality score into its bits
    qualchar = Char2Qual (qualchar);

    if ( qualchar & SEQ_BITS )
      return 0;

    switch ( toupper (seqchar) )
      {
      case 'A': return (uint8_t)qualchar | ADENINE_BITS;
      case 'C': return (uint8_t)qualchar | CYTOSINE_BITS;
      case 'G': return (uint8_t)qualchar | GUANINE_BITS;
      case 'T': return (uint8_t)qualchar | THYMINE_BITS;
      case 'N': return 0;
      default:
	return 0;
      }
  }


  //--------------------------------------------------- uncompress -------------
  //! \brief Uncompresses a byte into a sequence and quality char
  //!
  //! \note Must work with compress(char,char)
  //!
  //! \param byte The compressed sequence and quality byte
  //! \return The sequence and quality char respectively
  //!
  static inline std::pair<char, char> uncompress (uint8_t byte)
  {
    std::pair<char, char> retval;

    switch ( byte & SEQ_BITS )
      {
      case ADENINE_BITS:  retval . first = 'A'; break;
      case CYTOSINE_BITS: retval . first = 'C'; break;
      case GUANINE_BITS:  retval . first = 'G'; break;
      case THYMINE_BITS:  retval . first = 'T'; break;
      }

    byte &= QUAL_BITS;
    if ( byte == 0 )
      retval . first = 'N';

    retval . second = Qual2Char (byte);

    return retval;
  }


  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);

  //--------------------------------------------------- readRecordFix ----------
  virtual void readRecordFix (std::istream & fix);

  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object


  //--------------------------------------------------- Sequence_t -------------
  //! \brief Constructs an empty Sequence_t object
  //!
  //! Sets all members to 0 or NULL
  //!
  Sequence_t ( )
  {
    seq_m = qual_m = NULL;
    length_m = 0;
  }


  //--------------------------------------------------- Sequence_t -------------
  //! \brief Copy constructor
  //!
  Sequence_t (const Sequence_t & source)
  {
    seq_m = qual_m = NULL;
    *this = source;
  }


  //--------------------------------------------------- ~Sequence_t ------------
  //! \brief Destroys a Sequence_t object
  //!
  //! Frees the memory used for storing the sequence and quality data.
  //!
  virtual ~Sequence_t ( )
  {
    free (seq_m);
    free (qual_m);
  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  //! All data will be cleared, but object compression status will remain
  //! unchanged. Use the compress/uncompress members to change this info.
  //!
  virtual void clear ( );


  //--------------------------------------------------- compress ---------------
  //! \brief Compress the internal representation of this sequence
  //!
  //! After compression, this object will continue to compress incoming data
  //! until the uncompress method is called. Compression packs both a base
  //! and a quality score into a single byte, effectively halving the memory
  //! requirements for each object.
  //!
  //! The sequence should only contain A,C,G,T and Ns and quality scores in the
  //! range [MIN_QUALITY,MAX_QUALITY], if either of these conditions are not
  //! met the information will be lost when the data is compressed (see
  //! postconditions below).
  //!
  //! \post All invalid quality scores will be cast to MIN_QUALITY
  //! \post All N's will be assigned a MIN_QUALITY quality score
  //! \post All MIN_QUALITY scores will be assigned a N seqchar
  //! \return void
  //!
  void compress ( );


  //--------------------------------------------------- getBase ----------------
  //! \brief Get a single base and its quality score
  //!
  //! Retrieves and uncompresses the sequence base for the requested index.
  //!
  //! \param index The index of the requested base
  //! \pre index >= 0 && index < length
  //! \throws ArgumentException_t
  //! \return The requested (uppercase) base character and its quality score
  //!
  std::pair<char, char> getBase (Pos_t index) const
  {
    if (seq_m == NULL)
      AMOS_THROW_ARGUMENT("No sequence data");
    
    if ( index < 0 || index >= length_m )
      AMOS_THROW_ARGUMENT ("Requested sequence index is out of range");

    if ( isCompressed( ) )
      return uncompress (seq_m [index]);
    else
      return std::make_pair ((char)(seq_m [index]), (char)(qual_m [index]));
  }



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

    // skip ambiguities, gaps

    for (int i = start; i < stop; i++)
    {
      switch(getBase(i).first)
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



  //--------------------------------------------------- getLength --------------
  //! \brief Get the length of the sequence
  //!
  //! \return The length of the sequence
  //!
  Size_t getLength ( ) const
  {
    return length_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Sequence_t::NCODE;
  }


  //--------------------------------------------------- getQualString ----------
  //! \brief Get the quality score string
  //!
  //! \return The full string of quality scores
  //!
  std::string getQualString ( ) const
  {
    return getQualString (Range_t (0, length_m));
  }


  //--------------------------------------------------- getQualString ----------
  //! \brief Get a quality score substring
  //!
  //! Returns a subrange of quality scores [begin, end) or (end, begin]. The
  //! reversed range will pull the reverse string of quality scores.
  //!
  //! \param range The range of quality scores to get
  //! \pre range lo >= 0 && hi <= length
  //! \throws ArgumentException_t
  //! \return A subrange quality scores
  //!
  std::string getQualString (Range_t range) const;


  //--------------------------------------------------- getSeqString -----------
  //! \brief Get the sequence base string
  //!
  //! \return The full string of sequence bases
  //!
  std::string getSeqString ( ) const
  {
    return getSeqString (Range_t (0, length_m));
  }


  //--------------------------------------------------- getSeqString -----------
  //! \brief Get a sequence base substring
  //!
  //! Returns a subrange of sequence bases [begin, end) or (end, being]. The
  //! reversed range will pull the reverse complement string of sequences bases.
  //!
  //! \param range The range of sequence bases to get
  //! \pre range lo >= 0 && hi <= length
  //! \throws ArgumentException_t
  //! \return A subrange of sequence bases
  //!
  std::string getSeqString (Range_t range) const;


  //--------------------------------------------------- isCompressed -----------
  //! \brief Checks if the sequence data is compressed
  //!
  //! Returns true if the Sequence is currently operating in compressed mode,
  //! or false if under normal operation.
  //!
  //! \return True if compressed, false if not
  //!
  bool isCompressed ( ) const
  {
    return flags_m . nibble & COMPRESS_BIT;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setBase ----------------
  //! \brief Set a sequence base and its quality score
  //!
  //! Any characters may be used for seq and qualchar unless dealing with a
  //! compressed sequence. If compressed, the sequence should only contain
  //! A,C,G,T and Ns and quality scores in the range [MIN_QUALITY,MAX_QUALITY],
  //! if either of these conditions are not met the information will be lost
  //! when the data is compressed (see postconditions below).
  //!
  //! \param seqchar The sequence base character
  //! \param qualchar The quality score character
  //! \param index The index to assign these values
  //! \pre index >= 0 && index < length
  //! \post If compressed:
  //! - All invalid quality scores will be cast to MIN_QUALITY
  //! - All N's will be assigned a MIN_QUALITY quality score
  //! - All MIN_QUALITY scores will be assigned a N seqchar
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setBase (char seqchar, char qualchar, Pos_t index)
  {  
    if (seq_m == NULL)
      AMOS_THROW_ARGUMENT("No sequence data");

    if ( index < 0 || index >= length_m )
      AMOS_THROW_ARGUMENT ("Requested sequence index is out of range");

    if ( isCompressed( ) )
      seq_m [index] = compress (seqchar, qualchar);
    else
      {
	seq_m  [index] = seqchar;
	qual_m [index] = qualchar;
      }
  }


  //--------------------------------------------------- setSequence ------------
  //! \brief Set the entire sequence
  //!
  //! Combines and compresses the sequence and quality data contained in the
  //! two C strings. If current Sequence object is compressed, please refer
  //! to the postconditions for the setBase(char,char,Pos_t) operation. All
  //! newline characters will be discarded, but the newlines must be in the
  //! same location in both the sequence and quality files.
  //!
  //! \param seq The sequence base string
  //! \param qual The quality score string
  //! \pre strlen(seq) == strlen(qual)
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setSequence (const char * seq, const char * qual);


  //--------------------------------------------------- setSequence ------------
  //! \brief Set the entire sequence
  //!
  //! Combines and compresses the sequence and quality data contained in the
  //! two STL strings. If current Sequence object is compressed, please refer
  //! to the postconditions for the setBase(char,char,Pos_t) operation. All
  //! newline characters will be discarded. but the newlines must be in the
  //! same location in both the sequence and quality files.
  //!
  //! \param seq The sequence base string
  //! \param qual The quality score string
  //! \pre seq . size( ) == qual . size( )
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setSequence (const std::string & seq, const std::string & qual);


  //--------------------------------------------------- uncompress -------------
  //! \brief Uncompress the internal representation of this sequence
  //!
  //! After uncompression, this object will not compress incoming data until
  //! the compress method is called once again. The uncompressed version uses
  //! two bytes to store a base and quality score, thus doubling the memory
  //! requirements over a compressed version.
  //!
  //! \return void
  //!
  void uncompress ( );


  //--------------------------------------------------- operator= --------------
  //! \brief Assignment (copy) operator
  //!
  //! Efficiently copies the compressed data from the other Sequence_t.
  //!
  //! \param source The sequence to copy
  //! \return The resulting Sequence_t object
  //!
  Sequence_t & operator= (const Sequence_t & source);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Sequence_AMOS_HH
