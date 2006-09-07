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

#include "Universal_AMOS.hh"




namespace AMOS {

typedef char FragmentType_t;

//================================================ Fragment_t ==================
//! \brief Single piece of DNA
//!
//! A single piece of DNA from which a group of reads were sequenced. This
//! allows for the hierarchical clustering of reads that come from the same
//! fragment. Mate-pair information is also stored in this type.
//!
//==============================================================================
class Fragment_t : public Universal_t
{

private:

  ID_t library_m;                     //!< IID of the parent library
  Size_t size_m;                      //!< size of the fragment, 0 if not known
  std::pair<ID_t, NCode_t> source_m;  //!< source of the fragment
  std::pair<ID_t, ID_t> reads_m;      //!< the pair of mated read IIDs
  FragmentType_t type_m;              //!< type of fragment


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

  static const FragmentType_t NULL_FRAGMENT = 0;
  static const FragmentType_t OTHER         = 'X';
  static const FragmentType_t BAC           = 'B';
  static const FragmentType_t INSERT        = 'I';
  static const FragmentType_t TRANSPOSON    = 'T';
  static const FragmentType_t WALK          = 'W';


  //--------------------------------------------------- Fragment_t -------------
  //! \brief Contstructs an empty Fragment_t object
  //!
  //! Sets all members to NULL
  //!
  Fragment_t ( )
  {
    size_m = 0;
    library_m = source_m . first = reads_m . first = reads_m . second = NULL_ID;
    source_m . second = NULL_NCODE;
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
  virtual void clear ( );


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Fragment_t::NCODE;
  }


  //--------------------------------------------------- getLibrary -------------
  //! \brief Get the parent library IID
  //!
  //! \return The IID of the parent library
  //!
  ID_t getLibrary ( ) const
  {
    return library_m;
  }


  //--------------------------------------------------- getMatePair ------------
  //! \brief Get the paired sequencing reads for this fragment
  //!
  //! Orientation of the pair can be deduced from the fragment type. An INSERT
  //! fragment will have innie mates and a TRANSPOSON fragment will have outie
  //! mates.
  //!
  //! \return The mated reads
  //!
  std::pair<ID_t, ID_t> getMatePair ( ) const
  {
    return reads_m;
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Get the size of the fragment, if known
  //!
  //! Size will be 0 if unknown. If exact size is unknown, the parent library
  //! of this fragment should contain an estimated size distribution.
  //!
  //! \return The size of the fragment
  //!
  Size_t getSize ( ) const
  {
    return size_m;
  }


  //--------------------------------------------------- getSource --------------
  //! \brief Get the fragment source IID and type
  //!
  //! Returns the source this fragment was generated from, e.g. the insert
  //! fragment used for a PCR walk.
  //!
  //! \return The IID of the fragment source and the NCode type identifier
  //!
  std::pair<ID_t, NCode_t> getSource ( ) const
  {
    return source_m;
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


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setLibrary -------------
  //! \brief Set the parent library IID
  //!
  //! \param library The new IID of parent library
  //! \return void
  //!
  void setLibrary (ID_t library)
  {
    library_m = library;
  }


  //--------------------------------------------------- setMatePair ------------
  //! \brief Set the paired sequencing reads for this fragment
  //!
  //! Orientation of the pair can be stored in the fragment type. An INSERT
  //! fragment will have innie mates and a TRANSPOSON fragment will have outie
  //! mates.
  //!
  //! \param reads The new pair of read IIDs
  //! \return void
  //!
  void setReads (std::pair<ID_t, ID_t> reads)
  {
    reads_m = reads;
  }


  //--------------------------------------------------- setSize ----------------
  //! \brief Set the size of the fragment, if known
  //!
  //! \param size The size of the fragment
  //! \return void
  //!
  void setSize (Size_t size)
  {
    size_m = size;
  }


  //--------------------------------------------------- setSource --------------
  //! \brief Set the link source IID and type
  //!
  //! Sets the source this fragment was generated from, e.g. the insert
  //! fragment used for a PCR walk.
  //!
  //! \param source The IID and NCode type of the fragment source
  //! \return void
  //!
  void setSource (std::pair<ID_t, NCode_t> source)
  {
    source_m = source;
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the fragment type
  //!
  //! \param type The new fragment type
  //! \pre type is one of [OITB] or NULL
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setType (FragmentType_t type);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Fragment_AMOS_HH
