////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Scaffold_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Scaffold_AMOS_HH
#define __Scaffold_AMOS_HH 1

#include "Universal_AMOS.hh"
#include <vector>




namespace AMOS {

//================================================ Scaffold_t ==================
//! \brief A ordered, oriented and positioned list of contigs
//!
//! A list of contigs that are ordered, oriented and positioned in
//! relation to one another.
//!
//==============================================================================
class Scaffold_t : public Universal_t
{

private:

  null_t poly_m;                   //!< polymorphism
  std::vector<Tile_t> contigs_m;   //!< contig tiling
  std::vector<ID_t> edges_m;       //!< list of contig edges


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
  virtual Size_t readRecord (std::istream & fix,
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
  virtual Size_t writeRecord (std::ostream & fix,
			      std::ostream & var) const;


public:

  //--------------------------------------------------- Scaffold_t -----------
  //! \brief Constructs an empty Scaffold_t object
  //!
  Scaffold_t ( )
  {

  }


  //--------------------------------------------------- Scaffold_t -------------
  //! \brief Copy constructor
  //!
  Scaffold_t (const Scaffold_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Scaffold_t ----------
  //! \brief Destroys a Scaffold_t object
  //!
  ~Scaffold_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( )
  {
    Universal_t::clear( );
    contigs_m . clear( );
    edges_m . clear( );
  }


  //--------------------------------------------------- getNCode ---------------
  //! \brief Get the unique bank type identifier
  //!
  //! \return The unique bank type identifier
  //!
  virtual NCode_t getNCode ( ) const
  {
    return Bankable_k::SCAFFOLD;
  }


  //--------------------------------------------------- getContigTiling --------
  //! \brief Get the tiling of underlying contigs
  //!
  //! \return The vector of underlying contigs
  //!
  const std::vector<Tile_t> & getContigTiling ( ) const
  {
    return contigs_m;
  }


  //--------------------------------------------------- getContigEdges ---------
  //! \brief Get the contig edge IDs
  //!
  //! \return The vector of contig edge IDs
  //!
  const std::vector<ID_t> & getContigEdges ( ) const
  {
    return edges_m;
  }


  //--------------------------------------------------- getPolymorphism --------
  //! \brief Get polymorphism information for this scaffold
  //!
  //! \note datatype not yet decided upon
  //!
  //! \return The polymorphism info
  //!
  null_t & getPolymorphism ( )
  {
    return poly_m;
  }


  //--------------------------------------------------- setContigTiling --------
  //! \brief Set the tiling of underlying contigs
  //!
  //! \param contigs The new vector of underlying contigs
  //! \return void
  //!
  void setContigTiling (const std::vector<Tile_t> & contigs)
  {
    contigs_m = contigs;
  }


  //--------------------------------------------------- setContigEdges ---------
  //! \brief Set the contig edge IDs
  //!
  //! \param edges The new vector of contig edge IDs
  //! \return void
  //!
  void setContigEdges (const std::vector<ID_t> & edges)
  {
    edges_m = edges;
  }


  //--------------------------------------------------- setPolymorphism --------
  //! \brief Set polymorphism information for this scaffold
  //!
  //! \note datatype not yet decided upon
  //!
  //! \param poly The new polymorphism info
  //! \return void
  //!
  void setPolymorphism (const null_t & poly)
  {
    poly_m = poly;
  }

};

} // namespace AMOS

#endif // #ifndef __Scaffold_AMOS_HH
