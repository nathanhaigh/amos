////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/12/2005
//!
//! \brief Header for Edge_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Edge_AMOS_HH
#define __Edge_AMOS_HH 1

#include "Link_AMOS.hh"
#include <vector>




namespace AMOS {

//====================================================== IEdge_t ===============
//! \brief Interface for Edge_t
//!
//==============================================================================
class IEdge_t
{
  
private:

  std::vector<ID_t> links_m;             //!< bundle of Links


protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);


  //--------------------------------------------------- readRecordFix ----------
  virtual void readRecordFix (std::istream & fix);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;


public:

  //--------------------------------------------------------- IEdge_t ----------
  //! \brief Constructs an empty IEdge_t object
  //!
  IEdge_t ( )
  {

  }


  //--------------------------------------------------------- IEdge_t ----------
  //! \brief Copy constructor
  //!
  IEdge_t (const IEdge_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------------- ~IEdge_t ---------
  //! \brief Destroys a IEdge_t object
  //!
  virtual ~IEdge_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  void clear ( )
  {
    links_m . clear( );
  }


  //--------------------------------------------------------- getLinks ---------
  //! \brief Get the bundle of link IIDs that make up this contig edge
  //!
  //! \return A vector of link IIDs
  //!
  const std::vector<ID_t> & getLinks ( ) const
  {
    return links_m;
  }


  //--------------------------------------------------------- getLinks ---------
  //! \brief Get the bundle of link IIDs that make up this contig edge
  //!
  //! \return A vector of link IIDs
  //!
  std::vector<ID_t> & getLinks ( )
  {
    return links_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const = 0;


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);

  
  //--------------------------------------------------------- setLinks ---------
  //! \brief Set the bundle of link IIDs that make up this contig edge
  //!
  //! \param links The new vector of link IIDs
  //! \return void
  //!
  void setLinks (const std::vector<ID_t> & links)
  {
    links_m = links;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};




//====================================================== Edge_t ================
//! \brief Reliable linking information between two nodes
//!
//! Derived from a Link_t, this class represents a reliable link between
//! two nodes that is usually composed of multiple links.
//!
//==============================================================================
  class Edge_t : public Link_t, public IEdge_t
{
  
private:


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


  //--------------------------------------------------------- Edge_t -----------
  //! \brief Constructs an empty Edge_t object
  //!
  Edge_t ( )
  {

  }


  //--------------------------------------------------------- Edge_t -----------
  //! \brief Copy constructor
  //!
  Edge_t (const Edge_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------------- ~Edge_t ----------
  //! \brief Destroys a Edge_t object
  //!
  ~Edge_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Link_t::clear( );
    IEdge_t::clear( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Edge_t::NCODE;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Edge_AMOS_HH
