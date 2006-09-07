////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for ContigLink_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __ContigLink_AMOS_HH
#define __ContigLink_AMOS_HH 1

#include "Link_AMOS.hh"
#include "Contig_AMOS.hh"
#include <utility>




namespace AMOS {

//================================================ ContigLink_t ================
//! \brief Linking information between two contigs
//!
//! A single piece of linking information between two contigs. This evidence is
//! used in the scaffolding process to determine the order and orientation of
//! the assembly contigs. These individual links can be bundled together to
//! form reliable contig edges, as done in scaffolding.
//!
//==============================================================================
class ContigLink_t : public Link_t
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


  //--------------------------------------------------- ContigLink_t -----------
  //! \brief Constructs an empty ContigLink_t object
  //!
  //! Sets all members to zero or NULL
  //!
  ContigLink_t ( )
  {

  }


  //--------------------------------------------------- ContigLink_t -----------
  //! \brief Copy constructor
  //!
  ContigLink_t (const ContigLink_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~ContigLink_t ----------
  //! \brief Destroys a ContigLink_t object
  //!
  ~ContigLink_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Link_t::clear( );
  }


  //--------------------------------------------------- getContigs -------------
  //! \brief Get the pair of contig IIDs joined by this link
  //!
  //! \return The first and second contig IIDs joined by this link
  //!
  std::pair<ID_t, ID_t> getContigs ( ) const
  {
    return Link_t::getNodes( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return ContigLink_t::NCODE;
  }


  //----------------------------------------------------- getNodeType ----------
  NCode_t getNodeType( ) const
  {
    return Contig_t::NCODE;
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setContigs -------------
  //! \brief Set the contig IID pair for this link
  //!
  //! Only returns the IIDs of the contigs. It is up to the user to keep track
  //! of where the actual contig objects are stored.
  //!
  //! \param contigs The new pair of contig IIDs
  //! \return void
  //!
  void setContigs (std::pair<ID_t, ID_t> contigs)
  {
    Link_t::setNodes (contigs);
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __ContigLink_AMOS_HH
