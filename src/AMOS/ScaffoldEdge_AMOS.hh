////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/19/2003
//!
//! \brief Header for ScaffoldEdge_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __ScaffoldEdge_AMOS_HH
#define __ScaffoldEdge_AMOS_HH 1

#include "Edge_AMOS.hh"
#include "ScaffoldLink_AMOS.hh"
#include <vector>




namespace AMOS {

//================================================ ScaffoldEdge_t ==============
//! \brief Reliable linking information between two scaffolds
//!
//! Derived from a ScaffoldLink_t, this class represents a reliable link between
//! two scaffolds that is usually composed of multiple scaffold links.
//!
//==============================================================================
  class ScaffoldEdge_t : public ScaffoldLink_t, public IEdge_t
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


  //--------------------------------------------------- ScaffoldEdge_t ---------
  //! \brief Constructs an empty ScaffoldEdge_t object
  //!
  ScaffoldEdge_t ( )
  {
    // ScaffoldLink_t sets NodeType
  }


  //--------------------------------------------------- ScaffoldEdge_t ---------
  //! \brief Copy constructor
  //!
  ScaffoldEdge_t (const ScaffoldEdge_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~ScaffoldEdge_t --------
  //! \brief Destroys a ScaffoldEdge_t object
  //!
  ~ScaffoldEdge_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    // ScaffoldLink_t preserves NodeType
    ScaffoldLink_t::clear( );
    IEdge_t::clear( );
  }


  //--------------------------------------------------- getScaffoldLinks -------
  //! \brief Get the bundle of scaffold link IIDs that make up this edge
  //!
  //! \return A vector of scaffold link IIDs
  //!
  const std::vector<ID_t> & getScaffoldLinks ( ) const
  {
    return IEdge_t::getLinks( );
  }


  //--------------------------------------------------- getScaffoldLinks -------
  //! \brief Get the bundle of scaffold link IIDs that make up this edge
  //!
  //! \return A vector of scaffold link IIDs
  //!
  std::vector<ID_t> & getScaffoldLinks ( )
  {
    return IEdge_t::getLinks( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return ScaffoldEdge_t::NCODE;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);

  
  //--------------------------------------------------- setScaffoldLinks -------
  //! \brief Set the bundle of scaffold link IIDs that make up this edge
  //!
  //! \param links The new vector of scaffold link IIDs
  //! \return void
  //!
  void setScaffoldLinks (const std::vector<ID_t> & links)
  {
    IEdge_t::setLinks (links);
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __ScaffoldEdge_AMOS_HH
