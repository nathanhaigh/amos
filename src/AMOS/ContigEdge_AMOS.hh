////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for ContigEdge_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __ContigEdge_AMOS_HH
#define __ContigEdge_AMOS_HH 1

#include "Edge_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include <vector>




namespace AMOS {

//================================================ ContigEdge_t ================
//! \brief Reliable linking information between two contigs
//!
//! Derived from a ContigLink_t, this class represents a reliable link between
//! two contigs that is usually composed of multiple contig links. A list
//! of contig edges is generally the output contig scaffolding.
//!
//==============================================================================
  class ContigEdge_t : public ContigLink_t, public IEdge_t
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


  //--------------------------------------------------- ContigEdge_t -----------
  //! \brief Constructs an empty ContigEdge_t object
  //!
  ContigEdge_t ( )
  {
    // ContigLink_t sets NodeType
  }


  //--------------------------------------------------- ContigEdge_t -----------
  //! \brief Copy constructor
  //!
  ContigEdge_t (const ContigEdge_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~ContigEdge_t ----------
  //! \brief Destroys a ContigEdge_t object
  //!
  ~ContigEdge_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    // ContigLink_t preserves NodeType
    ContigLink_t::clear( );
    IEdge_t::clear( );
  }


  //--------------------------------------------------- getContigLinks ---------
  //! \brief Get the bundle of contig link IIDs that make up this contig edge
  //!
  //! \return A vector of contig link IIDs
  //!
  const std::vector<ID_t> & getContigLinks ( ) const
  {
    return IEdge_t::getLinks( );
  }


  //--------------------------------------------------- getContigLinks ---------
  //! \brief Get the bundle of contig link IIDs that make up this contig edge
  //!
  //! \return A vector of contig link IIDs
  //!
  std::vector<ID_t> & getContigLinks ( )
  {
    return IEdge_t::getLinks( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return ContigEdge_t::NCODE;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);

  
  //--------------------------------------------------- setContigLinks ---------
  //! \brief Set the bundle of contig link IIDs that make up this contig edge
  //!
  //! \param links The new vector of contig link IIDs
  //! \return void
  //!
  void setContigLinks (const std::vector<ID_t> & links)
  {
    IEdge_t::setLinks (links);
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __ContigEdge_AMOS_HH
