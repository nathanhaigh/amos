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

#include "Bankable_AMOS.hh"
#include "exceptions_AMOS.hh"
#include <utility>
#include <cctype>




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
class ContigLink_t : public Bankable_t
{

public:

  enum LinkType_t
  //! link description
    {
      NULL_LINK = 0,
      OTHER,
      MATEPAIR,
      OVERLAP,
      PHYSICAL,
      ALIGNMENT,
      SYNTENY,
      MAX_LINKS,
    };


private:

  std::pair<ID_t, ID_t> contigs_m;        //!< the pair of contig IDs
  SD_t sd_m;                              //!< standard deviation of the link
  Size_t size_m;                          //!< size of the link
  std::pair<ID_t, BankType_t> source_m;   //!< source of the link
  LinkType_t type_m;                      //!< type of link


protected:

  static const uint8_t FIRST_BIT  = 0x1;  //!< adjacency info for 1st contig
  static const uint8_t SECOND_BIT = 0x2;  //!< adjacency info for 2nd contig


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

  static const BankType_t BANKTYPE = Bankable_t::CONTIGLINK;
  //!< Bank type, MUST BE UNIQUE for all derived Bankable classes!


  //--------------------------------------------------- ContigLink_t -----------
  //! \brief Constructs an empty ContigLink_t object
  //!
  //! Sets all members to zero or NULL
  //!
  ContigLink_t ( )
    : Bankable_t ( )
  {
    contigs_m.first = contigs_m.second = source_m.first = NULL_ID;
    source_m.second = Bankable_t::BANKTYPE;
    sd_m = size_m = 0;
    type_m = this->NULL_LINK;
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


  //--------------------------------------------------- getAdjacency -----------
  //! \brief Get the adjacent ends of the two contigs
  //!
  //! Get the ends of the first and second contigs that are adjacent, i.e.
  //! (E,B), (E,E), (B,B), or (E,B), where (E,B) would mean the end of the
  //! first contig is adjacent to the beginning of the second contig.
  //!
  //! \return The pair of adjacent ends
  //!
  std::pair<char, char> getAdjacency ( ) const;


  //--------------------------------------------------- getBankType ------------
  //! \brief Get the unique bank type identifier
  //!
  //! \return The unique bank type identifier
  //!
  BankType_t getBankType ( ) const
  {
    return BANKTYPE;
  }


  //--------------------------------------------------- getContigs -------------
  //! \brief Get the pair of contig IDs joined by this link
  //!
  //! \return The first and second contig IDs joined by this link
  //!
  std::pair<ID_t, ID_t> getContigs ( ) const
  {
    return contigs_m;
  }


  //--------------------------------------------------- getSD ------------------
  //! \brief Get the standard deviation of the link size
  //!
  //! \return The standard deviation of the link
  //!
  SD_t getSD ( ) const
  {
    return sd_m;
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Get the size of the link
  //!
  //! The link size is measured as the gap size between the neighboring contigs.
  //! For instance, contig A ends at 10 (position 9) and contig B starts at 12
  //! (position 12); the gap size would be 12 - 10 = 2.
  //!
  //! \return The size of the link
  //!
  Size_t getSize ( ) const
  {
    return size_m;
  }


  //--------------------------------------------------- getSource --------------
  //! \brief Get the link source ID and type of Bank it is stored in
  //!
  //! This method only returns the ID of the link source and the type of Bank
  //! it is stored in. The entire source object can be retrieved by fetching
  //! the specified ID from the cooresponding BankType Bank.
  //!
  //! \return The ID of the link source and the BankType
  //!
  std::pair<ID_t, BankType_t> getSource ( ) const
  {
    return source_m;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the link type
  //!
  //! Describes the type of link this object represents.
  //!
  //! \return The type of link
  //!
  LinkType_t getType ( ) const
  {
    return type_m;
  }


  //--------------------------------------------------- setAdjacency -----------
  //! \brief Set the adjacent ends of the two contigs
  //!
  //! Set the ends of the first and second contigs that are linked, i.e.
  //! (E,B), (E,E), (B,B), or (E,B), where (E,B) would mean the end of the
  //! first contig is adjacent to the beginning of the second contig.
  //! 'B' denotes begin, while 'E' denotes end.
  //!
  //! \note Will store info in extra portion of BankableFlags
  //!
  //! \param adj The new adjacent ends (B or E) of the two contigs
  //! \pre Each char in the pair must be either 'B' or 'E'
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setAdjacency (std::pair<char, char> adj);


  //--------------------------------------------------- setContigs -------------
  //! \brief Set the contig ID pair for this link
  //!
  //! Only returns the IDs of the contigs. It is up to the user to keep track
  //! of where the actual contig objects are stored.
  //!
  //! \param contigs The new pair of contig IDs
  //! \return void
  //!
  void setContigs (std::pair<ID_t, ID_t> contigs)
  {
    contigs_m = contigs;
  }

  //--------------------------------------------------- setSD ------------------
  //! \brief Set the standard deviation of the link size
  //!
  //! \param sd The new standard deviation for the link size
  //! \return void
  //!
  void setSD (SD_t sd)
  {
    sd_m = sd;
  }


  //--------------------------------------------------- setSize ----------------
  //! \brief Set the size of the link
  //!
  //! \param size The new link size
  //! \return void
  //!
  void setSize (Size_t size)
  {
    size_m = size;
  }


  //--------------------------------------------------- setSource --------------
  //! \brief Set the link source ID
  //!
  //! \param source The new ID and BankType of the link source
  //! \return void
  //!
  void setSource (std::pair<ID_t, BankType_t> source)
  {
    source_m = source;
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the linking type
  //!
  //! \param type The new link type
  //! \return void
  //!
  void setType (LinkType_t type)
  {
    type_m = type;
  }

};

} // namespace AMOS

#endif // #ifndef __ContigLink_AMOS_HH
