////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Fragment_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Fragment_AMOS_HH
#define __Fragment_AMOS_HH 1

#include "Bankable_AMOS.hh"
#include <vector>




namespace AMOS {

//================================================ Fragment_t ==================
//! \brief Single piece of DNA
//!
//! A single piece of DNA from which a group of reads were sequenced. This
//! allows for the hierarchical clustering of reads that come from the same
//! fragment.
//!
//==============================================================================
class Fragment_t : public Bankable_t
{

public:

  enum FragmentType_t
  //! fragment description
    {
      NULL_FRAGMENT = 0,
      OTHER,
      INSERT,
      TRANSPOSON,
      BAC,
      MAX_FRAGMENTS,
    };


private:

  std::pair<ID_t, ID_t> ends_m;    //!< The forward and reverse fragment ends
  ID_t library_m;                  //!< ID of the parent library
  std::vector<ID_t> reads_m;       //!< the list of associated read IDs
  FragmentType_t type_m;           //!< type of fragment


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

  static const BankType_t BANKTYPE = Bankable_t::FRAGMENT;
  //!< Bank type, MUST BE UNIQUE for all derived Bankable classes!


  //--------------------------------------------------- Fragment_t -------------
  //! \brief Contstructs an empty Fragment_t object
  //!
  //! Sets all members to NULL
  //!
  Fragment_t ( )
  {
    ends_m . first = ends_m . second = library_m = NULL_ID;
    type_m = NULL_FRAGMENT;
  }


  //--------------------------------------------------- Fragment_t -------------
  //! \brief Copy constructor
  //!
  Fragment_t (const Fragment_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Fragment_t ------------
  //! \brief Destroys a Fragment_t object
  //!
  ~Fragment_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( )
  {
    Bankable_t::clear( );
    ends_m . first = ends_m . second = library_m = NULL_ID;
    reads_m . clear( );
    type_m = NULL_FRAGMENT;
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


  //--------------------------------------------------- getEnds ----------------
  //! \brief Returns the IDs of the forward and reverse ends of the Fragment
  //!
  //! Returns the IDs of the reads that represent the forward and reverse
  //! ends of the fragment. Useful for retrieving mate-pair information.
  //!
  //! \return The forward and reverse ends respectively
  //!
  std::pair<ID_t, ID_t> getEnds ( ) const
  {
    return ends_m;
  }


  //--------------------------------------------------- getLibrary -------------
  //! \brief Get the parent library ID
  //!
  //! \return The ID of the parent library
  //!
  ID_t getLibrary ( ) const
  {
    return library_m;
  }


  //--------------------------------------------------- getReads ---------------
  //! \brief Get the child read IDs
  //!
  //! \return The vector of child read IDs
  //!
  const std::vector<ID_t> & getReads ( ) const
  {
    return reads_m;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the fragment type
  //!
  //! \return The type of fragment
  //!
  FragmentType_t getType ( ) const
  {
    return type_m;
  }


  //--------------------------------------------------- setEnds ----------------
  //! \brief Sets the IDs of the forward and reverse ends of the Fragment
  //!
  //! Set the IDs of the reads that represent the forward and reverse
  //! ends of the Fragment. Useful in storing mate-pair information.
  //!
  //! \param ends The foward and reverse ends respectively
  //! \return void
  //!
  void setEnds (std::pair<ID_t, ID_t> ends)
  {
    ends_m = ends;
  }


  //--------------------------------------------------- setLibrary -------------
  //! \brief Set the parent library ID
  //!
  //! \param library The new ID of parent library
  //! \return void
  //!
  void setLibrary (ID_t library)
  {
    library_m = library;
  }


  //--------------------------------------------------- setReads ---------------
  //! \brief Set the child read IDs
  //!
  //! \param reads The new vector of child read IDs
  //! \return void
  //!
  void setReads (const std::vector<ID_t> & reads)
  {
    reads_m = reads;
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the fragment type
  //!
  //! \param type The new fragment type
  //! \return void
  //!
  void setType (FragmentType_t type)
  {
    type_m = type;
  }

};

} // namespace AMOS

#endif // #ifndef __Fragment_AMOS_HH
