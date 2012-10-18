////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/11/2005
//!
//! \brief Header for Group_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Group_AMOS_HH
#define __Group_AMOS_HH 1

#include "Universal_AMOS.hh"
#include <vector>




namespace AMOS {

//================================================ Group_t =====================
//! \brief A homogenous grouping of AMOS objects
//!
//! A group of AMOS objects which are all of the same type.
//!
//==============================================================================
class Group_t : public Universal_t
{

private:

  std::vector<ID_t> mems_m;     //!< IIDs of the grouped objects
  NCode_t memtype_m;            //!< object type stored in the group


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


  //-------------------------------------------------------- Group_t -----------
  //! \brief Constructs an empty Group_t object
  //!
  //! Sets all members to zero or NULL
  //!
  Group_t ( )
  {
    memtype_m = NULL_NCODE;
  }


  //-------------------------------------------------------- Group_t -----------
  //! \brief Copy constructor
  //!
  Group_t (const Group_t & source)
  {
    *this = source;
  }


  //-------------------------------------------------------- ~Group_t ----------
  //! \brief Destroys a Group_t object
  //!
  ~Group_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Universal_t::clear( );
    mems_m . clear( );
    memtype_m = NULL_NCODE;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Group_t::NCODE;
  }


  //--------------------------------------------------- getMembers -------------
  //! \brief Get the vector of group members
  //!
  //! \return The vector of grouped object IIDs
  //!
  const std::vector<ID_t> & getMembers ( ) const
  {
    return mems_m;
  }


  //--------------------------------------------------- getMembers -------------
  std::vector<ID_t> & getMembers ( )
  {
    return mems_m;
  }


  //--------------------------------------------------- getMemberType ----------
  //! \brief Get the type of grouped object
  //!
  //! Describes the type of IIDs stored in the group vector.
  //!
  //! \return The type of grouped objects
  //!
  NCode_t getMemberType( ) const
  {
    return memtype_m;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setMembers -------------
  //! \brief Set the vector of group members
  //!
  //! \param mems The new vector of group members
  //! \return void
  //!
  void setMembers (const std::vector<ID_t> & mems)
  {
    mems_m = mems;
  }


  //--------------------------------------------------- setMemberType ----------
  //! \brief Set the type of grouped objects
  //!
  //! Describes the type of IIDs stored in the group vector.
  //!
  //! \param memtype The new object type
  //! \return void
  //!
  void setMemberType (NCode_t memtype)
  {
    memtype_m = memtype;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Group_AMOS_HH
