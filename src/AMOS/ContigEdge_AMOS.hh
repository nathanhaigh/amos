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
class ContigEdge_t : public ContigLink_t
{
  
private:

  std::vector<ID_t> links_m;             //!< bundle of ContigLinks


protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix,
			   std::istream & var);


  //--------------------------------------------------- sizeVar ----------------
  virtual Size_t sizeVar ( ) const;


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix,
			    std::ostream & var) const;


public:

  //--------------------------------------------------- NCode ------------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Can be used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (ContigEdge_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::CONTIGEDGE;
  }


  //--------------------------------------------------- ContigEdge_t -----------
  //! \brief Constructs an empty ContigEdge_t object
  //!
  ContigEdge_t ( )
  {

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
    ContigLink_t::clear( );
    links_m . clear( );
  }


  //--------------------------------------------------- getContigLinks ---------
  //! \brief Get the bundle of contig link IDs that make up this contig edge
  //!
  //! \return A vector of contig link IDs
  //!
  const std::vector<ID_t> & getContigLinks ( ) const
  {
    return links_m;
  }


  //--------------------------------------------------- getContigLinks ---------
  //! \brief Get the bundle of contig link IDs that make up this contig edge
  //!
  //! \return A vector of contig link IDs
  //!
  std::vector<ID_t> & getContigLinks ( )
  {
    return links_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return ContigEdge_t::NCode( );
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);

  
  //--------------------------------------------------- setContigLinks ---------
  //! \brief Set the bundle of contig link IDs that make up this contig edge
  //!
  //! \param links The new vector of contig link IDs
  //! \return void
  //!
  void setContigLinks (const std::vector<ID_t> & links)
  {
    links_m = links;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __ContigEdge_AMOS_HH
