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
  //! \brief Read all the class members from a biserial record
  //!
  //! Reads the fixed and variable length streams from a biserial record and
  //! initializes all the class members to the values stored within. Used in
  //! translating a biserial Bankable object, and needed to retrieve objects
  //! from a Bank. Returned size of the record will only be valid if the read
  //! was successful, i.e. fix.good( ) and var.good( ).
  //!
  //! \note This method must be able to interpret the biserial record
  //! produced by its related function writeRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \pre The get pointer of fix is at the beginning of the record
  //! \pre The get pointer of var is at the beginning of the record
  //! \return size of read record (size of fix + size of var)
  //!
  Size_t readRecord (std::istream & fix,
		     std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write all the class members to a biserial record
  //!
  //! Writes the fixed an variable length streams to a biserial record. Used in
  //! generating a biserial Bankable object, and needed to commit objects to a
  //! Bank. Will only write to the ready streams, but the size of the record
  //! will always be returned.
  //!
  //! \note This method must be able to produce a biserial record that can
  //! be read by its related funtion readRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \return size of written record (size of fix + size of var)
  //!
  Size_t writeRecord (std::ostream & fix,
		      std::ostream & var) const;


public:

  static const BankType_t BANKTYPE = Bankable_t::CONTIGEDGE;
  //!< Bank type, MUST BE UNIQUE for all derived Bankable classes!


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
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( )
  {
    ContigLink_t::clear( );
    links_m . clear( );
  }


  //--------------------------------------------------- getBankType ------------
  //! \brief Get the unique bank type identifier
  //!
  //! \return The unique bank type identifier
  //!
  BankType_t getBankType ( ) const
  {
    return BANKTYPE;
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

};

} // namespace AMOS

#endif // #ifndef __ContigEdge_AMOS_HH
