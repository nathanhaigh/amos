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

#include "Bankable_AMOS.hh"
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
class Kmer_t : public Bankable_t
{
  
private:

  friend inline Kmer_t & operator++ (Kmer_t &, int);

  uint8_t * seq_m;              //!< the compressed string of A, C, G and T's
  uint32_t count_m;             //!< the number of occurrences of this Kmer
  uint8_t len_m;                //!< the length of the Kmer (value of K)
  std::vector<ID_t> reads_m;    //!< list of Read_t types that contain this Kmer


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
  Size_t readRecord (std::istream & fix,
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
  Size_t writeRecord (std::ostream & fix,
		      std::ostream & var) const;


public:

  static const BankType_t BANKTYPE = Bankable_t::KMER;
  //!< Bank type, MUST BE UNIQUE for all derived Bankable classes!


  //--------------------------------------------------- Kmer_t -----------------
  //! \brief Constructs an empty Kmer_t object
  //!
  //! Sets all members to 0 or NULL
  //!
  Kmer_t ( )
    : Bankable_t ( )
  {
    seq_m = NULL;
    count_m = len_m = 0;
  }


  //--------------------------------------------------- Kmer_t -----------------
  //! \brief Copy constructor
  //!
  Kmer_t (const Kmer_t & source)
  {
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


  //--------------------------------------------------- getBankType ------------
  //! \brief Get the unique bank type identifier
  //!
  //! \return The unique bank type identifier
  //!
  BankType_t getBankType ( ) const
  {
    return BANKTYPE;
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
    return len_m;
  }


  //--------------------------------------------------- getReads ---------------
  //! \brief Get the containing read IDs
  //!
  //! \return The vector of read IDs that contain this Kmer
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


  //--------------------------------------------------- setLength --------------
  //! \brief Set the length of the Kmer
  //!
  //! \param len The new Kmer length
  //! \return void
  //!
  void setLength (uint8_t len)
  {
    len_m = len;
  }


  //--------------------------------------------------- setReads ---------------
  //! \brief Set the list of containing read IDs
  //!
  //! \param reads The new vector of read IDs that contain this Kmer
  //! \return void
  //!
  void setReads (const std::vector<ID_t> & reads)
  {
    reads_m = reads;
  }


  //--------------------------------------------------- setSeqString -----------
  //! \brief Set the Kmer sequence string
  //!
  //! Compresses the string and updates the Kmer sequence.
  //!
  //! \param str The new Kmer sequence string
  //! \return void
  //!
  void setSeqString (const std::string & str);


  //--------------------------------------------------- operator= --------------
  //! \brief Assignment (copy) operator
  //!
  //! \param source The Kmer_t object to copy
  //! \return The resulting Kmer_t object
  //!
  Kmer_t & operator= (const Kmer_t & source);
};




//================================================ Helper Functions ============
//--------------------------------------------------- operator++ -------------
//! \brief Increments a Kmer count by 1 (postfix)
//!
//! Increment a Kmer count by 1 using postfix notation, i.e. mykmer ++;
//!
//! \param k The Kmer to increment
//! \return The incremented Kmer
//!
inline Kmer_t & operator++ (Kmer_t & k, int)
{
  k.count_m ++;
  return k;
}

} // namespace AMOS

#endif // #ifndef __Kmer_AMOS_HH
