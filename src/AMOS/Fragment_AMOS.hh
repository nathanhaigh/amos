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
//! fragment.
//!
//==============================================================================
class Fragment_t : public Universal_t
{

private:

  ID_t library_m;                  //!< IID of the parent library
  Size_t size_m;                   //!< size of the fragment, 0 if not known
  ID_t source_m;                   //!< IID of the source fragment
  FragmentType_t type_m;           //!< type of fragment


protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix,
			   std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix,
			    std::ostream & var) const;


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object

  static const FragmentType_t NULL_FRAGMENT;
  static const FragmentType_t OTHER;
  static const FragmentType_t BAC;
  static const FragmentType_t INSERT;
  static const FragmentType_t TRANSPOSON;
  static const FragmentType_t WALK;


  //--------------------------------------------------- Fragment_t -------------
  //! \brief Contstructs an empty Fragment_t object
  //!
  //! Sets all members to NULL
  //!
  Fragment_t ( )
  {
    size_m = 0;
    library_m = source_m = NULL_ID;
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
  virtual void clear ( )
  {
    Universal_t::clear( );
    size_m = 0;
    library_m = source_m = NULL_ID;
    type_m = NULL_FRAGMENT;
  }


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


  //--------------------------------------------------- getSize ----------------
  //! \brief Get the size of the fragment, if known
  //!
  //! Size will be 0 if unknown
  //!
  //! \return The size of the fragment
  //!
  Size_t getSize ( ) const
  {
    return size_m;
  }


  //--------------------------------------------------- getSource --------------
  //! \brief Get the source fragment of this fragment
  //!
  //! e.g. The insert fragment used for a PCR walk fragment
  //!
  //! \return The IID of the source fragment
  //!
  ID_t getSource ( ) const
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


  //--------------------------------------------------- getSize ----------------
  //! \brief Set the size of the fragment, if known
  //!
  //! \param size The size of the fragment
  //! \return void
  //!
  void setSize (Size_t size)
  {
    size_m = size;
  }


  //--------------------------------------------------- getSource --------------
  //! \brief Set the source fragment of this fragment
  //!
  //! e.g. The insert fragment used for a PCR walk fragment
  //!
  //! \param source The IID of the source fragment
  //! \return void
  //!
  void setSource (ID_t source)
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
  inline void setType (FragmentType_t type);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Fragment_AMOS_HH
