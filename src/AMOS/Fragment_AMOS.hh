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

public:

  static const FragmentType_t NULL_FRAGMENT  = 0;
  static const FragmentType_t OTHER          = 'X';
  static const FragmentType_t BAC            = 'B';
  static const FragmentType_t INSERT         = 'I';
  static const FragmentType_t TRANSPOSON     = 'T';
  static const FragmentType_t WALK           = 'W';


private:

  ID_t library_m;                  //!< ID of the parent library
  Size_t size_m;                   //!< size of the fragment, 0 if not known
  ID_t source_m;                   //!< ID of the source fragment
  FragmentType_t type_m;           //!< type of fragment


protected:

  //--------------------------------------------------- readRecord -------------
  virtual Size_t readRecord (std::istream & fix,
			     std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  virtual Size_t writeRecord (std::ostream & fix,
			      std::ostream & var) const;


public:

  //--------------------------------------------------- NCode ------------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Can be used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (Fragment_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::FRAGMENT;
  }


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
    return Fragment_t::NCode( );
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
  //! \return The ID of the source fragment
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
  //! \brief Set the parent library ID
  //!
  //! \param library The new ID of parent library
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
  //! \param The ID of the source fragment
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
  void setType (FragmentType_t type)
  {
    switch (type)
      {
      case NULL_FRAGMENT:
      case OTHER:
      case BAC:
      case INSERT:
      case TRANSPOSON:
      case WALK:
	type_m = type;
	break;
      default:
        AMOS_THROW_ARGUMENT ((string)"Invalid fragment type char: " + type);
      }
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Fragment_AMOS_HH
