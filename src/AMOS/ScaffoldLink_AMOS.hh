////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/19/2003
//!
//! \brief Header for ScaffoldLink_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __ScaffoldLink_AMOS_HH
#define __ScaffoldLink_AMOS_HH 1

#include "Link_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include <utility>




namespace AMOS {

//================================================ ScaffoldLink_t ==============
//! \brief Linking information between two scaffolds
//!
//! A single piece of linking information between two scaffolds. These
//! individual links can be bundled together to form reliable scaffold edges.
//!
//==============================================================================
class ScaffoldLink_t : public Link_t
{

private:

  void setNodeType (NCode_t nodtype);


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


  //--------------------------------------------------- ScaffoldLink_t ---------
  //! \brief Constructs an empty ScaffoldLink_t object
  //!
  //! Sets all members to zero or NULL
  //!
  ScaffoldLink_t ( )
  {

  }


  //--------------------------------------------------- ScaffoldLink_t ---------
  //! \brief Copy constructor
  //!
  ScaffoldLink_t (const ScaffoldLink_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~ScaffoldLink_t --------
  //! \brief Destroys a ScaffoldLink_t object
  //!
  ~ScaffoldLink_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Link_t::clear( );
  }


  //--------------------------------------------------- getScaffolds -----------
  //! \brief Get the pair of scaffold IIDs joined by this link
  //!
  //! \return The first and second scaffold IIDs joined by this link
  //!
  std::pair<ID_t, ID_t> getScaffolds ( ) const
  {
    return Link_t::getNodes( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return ScaffoldLink_t::NCODE;
  }


  //--------------------------------------------------- getNodeType ------------
  NCode_t getNodeType ( ) const
  {
    return Scaffold_t::NCODE;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setScaffolds -----------
  //! \brief Set the scaffold IID pair for this link
  //!
  //! Only returns the IIDs of the scaffolds. It is up to the user to keep track
  //! of where the actual scaffold objects are stored.
  //!
  //! \param scaffolds The new pair of scaffold IIDs
  //! \return void
  //!
  void setScaffolds (std::pair<ID_t, ID_t> scaffolds)
  {
    Link_t::setNodes (scaffolds);
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __ScaffoldLink_AMOS_HH
