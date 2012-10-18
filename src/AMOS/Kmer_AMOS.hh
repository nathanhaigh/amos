////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Kmer_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Kmer_AMOS_HH
#define __Kmer_AMOS_HH 1

#include "Universal_AMOS.hh"
#include <vector>
#include <string>




namespace AMOS {

//================================================ Kmer_t ======================
//! \brief String of K bases present in the assembly
//!
//! A string of K bases that is present in one or more occurrences throught a
//! set of sequencing reads.
//!
//==============================================================================
class Kmer_t : public Universal_t
{
  
private:

  uint8_t * seq_m;              //!< the compressed string of A, C, G and T's
  uint32_t count_m;             //!< the number of occurrences of this Kmer
  uint8_t length_m;             //!< the length of the Kmer (value of K)
  std::vector<ID_t> reads_m;    //!< list of Read_t types that contain this Kmer


protected:

  static const uint8_t ADENINE_BITS  = 0x0;   //!< 'A' bit
  static const uint8_t CYTOSINE_BITS = 0x40;  //!< 'C' bit
  static const uint8_t GUANINE_BITS  = 0x80;  //!< 'G' bit
  static const uint8_t THYMINE_BITS  = 0xC0;  //!< 'T' bit
  static const uint8_t SEQ_BITS      = 0xC0;  //!< sequence bit mask


  //--------------------------------------------------- compress ---------------
  //! \brief Compresses a sequence char into two bits
  //!
  //! \note Must work with uncompress(uint8_t) method
  //!
  //! \param seqchar The sequence base character
  //! \pre seqchar is A,C,G,T (case insensitive)
  //! \throws ArgumentException_t
  //! \return The compressed 2 bits (in the highest two bit positions)
  //!
  static uint8_t compress (char seqchar)
  {
    switch ( toupper(seqchar) )
      {
      case 'A': return ADENINE_BITS;
      case 'C': return CYTOSINE_BITS;
      case 'G': return GUANINE_BITS;
      case 'T': return THYMINE_BITS;
      default:
	AMOS_THROW_ARGUMENT ((std::string)"Invalid Kmer character " + seqchar);
      }
  }


  //--------------------------------------------------- uncompress -------------
  //! \brief Uncompresses two bits into a sequence char
  //!
  //! \note Must work with compress(char)
  //!
  //! \param byte The compressed sequence bits (in highest two bit positions)
  //! \throws Exception_t
  //! \return The sequence char
  //!
  static char uncompress (uint8_t byte)
  {
    switch ( byte & SEQ_BITS )
      {
      case ADENINE_BITS:  return 'A';
      case CYTOSINE_BITS: return 'C';
      case GUANINE_BITS:  return 'G';
      case THYMINE_BITS:  return 'T';
      default:
        AMOS_THROW_ARGUMENT ("Unknown logic error");
      }
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

  static const uint8_t MAX_LENGTH;
  //!< Maximum Kmer length


  //--------------------------------------------------- Kmer_t -----------------
  //! \brief Constructs an empty Kmer_t object
  //!
  //! Sets all members to 0 or NULL
  //!
  Kmer_t ( )
  {
    seq_m = NULL;
    count_m = length_m = 0;
  }


  //--------------------------------------------------- Kmer_t -----------------
  //! \brief Copy constructor
  //!
  Kmer_t (const Kmer_t & source)
  {
    seq_m = NULL;
    *this = source;
  }


  //--------------------------------------------------- ~Kmer_t ----------------
  //! \brief Destroys a Kmer_t object
  //!
  //! Frees the memory used for the compressed sequence
  //!
  ~Kmer_t ( )
  {
    free (seq_m);
  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( );


  //--------------------------------------------------- getBase ----------------
  //! \brief Get a single sequence base from the kmer
  //!
  //! Retrieves and uncompresses the sequence base for the requested index.
  //!
  //! \param index The index of the requested base
  //! \pre index >= 0 && index < length
  //! \throws ArgumentException_t
  //! \return The requested (uppercase) base character
  //!
  //  -- developers note --
  //  If we imagine consecutive bytes stored left-to-right, then we can index
  //  the seqchars left-to-right as follows:
  //   [0] [1] [2] [3]   [4] [5] [6] [7]  ... seqchars
  //  [7 6 5 4 3 2 1 0] [7 6 5 4 3 2 1 0] ... bits
  //  [byte 0         ] [byte 1         ] ... bytes
  //  Thus, to retrieve seqchar 5 we need to index byte 1. Since uncompress
  //  expects the sequence bits in the high-order end of the byte, we left
  //  shift 2-bits. The uncompress method will take care of the necessary
  //  masking.
  //
  char getBase (Pos_t index) const
  {
    if ( index < 0 || index >= length_m )
      AMOS_THROW_ARGUMENT ("Requested kmer index is out of range");
    return uncompress ((seq_m [index / 4]) << (index % 4 * 2));
  }


  //--------------------------------------------------- getCount ---------------
  //! \brief Get the number of this Kmer's occurrences
  //!
  //! \return The number of occurrrences
  //!
  uint32_t getCount ( ) const
  {
    return count_m;
  }


  //--------------------------------------------------- getLength --------------
  //! \brief Gets the length of the Kmer
  //!
  //! \return The Kmer length
  //!
  uint8_t getLength ( ) const
  {
    return length_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Kmer_t::NCODE;
  }


  //--------------------------------------------------- getReads ---------------
  //! \brief Get the containing read IIDs
  //!
  //! \return The vector of read IIDs that contain this Kmer
  //!
  const std::vector<ID_t> & getReads ( ) const
  {
    return reads_m;
  }


  //--------------------------------------------------- getReads ---------------
  //! \brief Get the containing read IIDs
  //!
  //! \return The vector of read IIDs that contain this Kmer
  //!
  std::vector<ID_t> & getReads ( )
  {
    return reads_m;
  }


  //--------------------------------------------------- getSeqString -----------
  //! \brief Get the Kmer sequence string
  //!
  //! Uncompresses the Kmer sequence and returns a string representation.
  //!
  //! \return The Kmer sequence string
  //!
  std::string getSeqString ( ) const;


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setBase ----------------
  //! \brief Set a single sequence base for the kmer
  //!
  //! Only the characters A,C,G,T are allowed, all other characters will throw
  //! an exception.
  //!
  //! \param seqchar The sequence base character
  //! \param index The index to assign this character
  //! \pre index >= 0 && index < length
  //! \pre seqchar is A,C,G,T (case insensitive)
  //! \throws ArgumentException_t
  //! \return void
  //!
  //  -- developers note --
  //  If we imagine consecutive bytes stored left-to-right, then we can index
  //  the seqchars left-to-right as follows:
  //   [0] [1] [2] [3]   [4] [5] [6] [7]  ... seqchars
  //  [7 6 5 4 3 2 1 0] [7 6 5 4 3 2 1 0] ... bits
  //  [byte 0         ] [byte 1         ] ... bytes
  //  Thus, to set seqchar 2, we need overwrite the bits at the 2^3 and 2^2
  //  positions of byte 0. Since compress returns the sequence bits in the
  //  high-order end of the byte, we right-shift the return value 4-bits and
  //  "OR" it with the stored byte (making sure to clear those two bits first).
  //
  void setBase (char seqchar, Pos_t index)
  {
    if ( index < 0 || index >= length_m )
      AMOS_THROW_ARGUMENT ("Requested kmer index is out of range");

    int offset = index % 4 * 2;              // the bitmask offset
    uint8_t * seqp = seq_m + index / 4;      // the required byte

    //-- Delete the previous two bits, then set the two new bits
    *seqp &= ~(SEQ_BITS >> offset);
    *seqp |= compress (seqchar) >> offset;
  }


  //--------------------------------------------------- setCount ---------------
  //! \brief Set the number of this Kmer's occurrences
  //!
  //! \param count The new number of occurrences
  //! \return void
  //!
  void setCount (uint32_t count)
  {
    count_m = count;
  }


  //--------------------------------------------------- setReads ---------------
  //! \brief Set the list of containing read IIDs
  //!
  //! \param reads The new vector of read IIDs that contain this Kmer
  //! \return void
  //!
  void setReads (const std::vector<ID_t> & reads)
  {
    reads_m = reads;
  }


  //--------------------------------------------------- setSeqString -----------
  //! \brief Set the Kmer sequence string
  //!
  //! Compresses the string and updates the Kmer sequence. Does not alter
  //! Kmer count or read list, these must be reset manually. Only the
  //! characters A,C,G,T are allowed, all other characters will throw an
  //! exception (except for newlines, which will be discarded).
  //!
  //! \param seq The new Kmer sequence string
  //! \pre seq can be longer than MAX_LENGTH (255) bases
  //! \pre seqchar is A,C,G,T (case insensitive)
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setSeqString (const std::string & seq);


  //--------------------------------------------------- operator++ -------------
  //! \brief Increments kmer count by 1 (postfix)
  //!
  //! Increment the kmer count by 1 using postfix notation, i.e. mykmer ++;
  //!
  //! \return This kmer, after incrementation
  //!
  Kmer_t & operator++ (int)
  {
    count_m ++;
    return *this;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;


  //--------------------------------------------------- operator= --------------
  //! \brief Assignment (copy) operator
  //!
  //! Efficiently copies the compressed data from the another Kmer_t.
  //!
  //! \param source The Kmer_t object to copy
  //! \return The resulting Kmer_t object
  //!
  Kmer_t & operator= (const Kmer_t & source);
};

} // namespace AMOS

#endif // #ifndef __Kmer_AMOS_HH
