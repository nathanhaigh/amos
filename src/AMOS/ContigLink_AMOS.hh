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

#include "Universal_AMOS.hh"
#include <utility>
#include <cctype>




namespace AMOS {

typedef char LinkType_t;
typedef char LinkAdjacency_t;

//================================================ ContigLink_t ================
//! \brief Linking information between two contigs
//!
//! A single piece of linking information between two contigs. This evidence is
//! used in the scaffolding process to determine the order and orientation of
//! the assembly contigs. These individual links can be bundled together to
//! form reliable contig edges, as done in scaffolding.
//!
//==============================================================================
class ContigLink_t : public Universal_t
{

public:

  static const LinkType_t NULL_LINK  = 0;
  static const LinkType_t OTHER      = 'X';
  static const LinkType_t MATEPAIR   = 'M';
  static const LinkType_t OVERLAP    = 'O';
  static const LinkType_t PHYSICAL   = 'P';
  static const LinkType_t ALIGNMENT  = 'A';
  static const LinkType_t SYNTENY    = 'S';

  static const LinkAdjacency_t NULL_ADJACENCY = 0;
  static const LinkAdjacency_t NORMAL     = 'N';     //!< E,B
  static const LinkAdjacency_t ANTINORMAL = 'A';     //!< B,E
  static const LinkAdjacency_t INNIE      = 'I';     //!< E,E
  static const LinkAdjacency_t OUTIE      = 'O';     //!< B,B


private:

  std::pair<ID_t, ID_t> contigs_m;        //!< the pair of contig IDs
  SD_t sd_m;                              //!< standard deviation of the link
  Size_t size_m;                          //!< size of the link
  std::pair<ID_t, NCode_t> source_m;      //!< source of the link
  LinkType_t type_m;                      //!< type of link


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
  //! (ContigLink_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::CONTIGLINK;
  }


  //--------------------------------------------------- ContigLink_t -----------
  //! \brief Constructs an empty ContigLink_t object
  //!
  //! Sets all members to zero or NULL
  //!
  ContigLink_t ( )
  {
    contigs_m . first = contigs_m . second = source_m . first = NULL_ID;
    source_m . second = NULL_NCODE;
    sd_m = size_m = 0;
    type_m = NULL_LINK;
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
    Universal_t::clear( );
    contigs_m . first = contigs_m . second = source_m . first = NULL_ID;
    source_m . second = NULL_NCODE;
    sd_m = size_m = 0;
    type_m = NULL_LINK;
  }


  //--------------------------------------------------- flip -------------------
  //! \brief Flip the orientation of the link
  //!
  //! Reverses the order of the contig IDs and changes the adjacency as
  //! altered by the new orientation of the contigs. Does not alter adjacency
  //! if it is currently a NULL_ADJACENCY. After flip, NORMAL become ANTINORMAL,
  //! ANTINORMAL becomes NORMAL, and INNIE and OUTIE remain the same.
  //!
  //! \return void
  //!
  void flip ( )
  {
    LinkAdjacency_t la = getAdjacency( );
    if ( la == NORMAL ) setAdjacency (ANTINORMAL);
    else if ( la == ANTINORMAL ) setAdjacency (NORMAL);

    contigs_m = std::make_pair (contigs_m . second, contigs_m . first);
  }


  //--------------------------------------------------- getAdjacency -----------
  //! \brief Get the adjacent ends of the two contigs
  //!
  //! Get the adjacency information for the contigs, i.e. [N]ORMAL (EB),
  //! [A]NTINORMAL (BE), [I]NNIE (EE) or [O]UTIE (BB). Where B is the
  //! beginning of the contig and E is the end of a contig and EB means the
  //! end of contig1 is adjacent to the beginning of contig2.
  //!
  //! \return The pair of adjacent ends
  //!
  LinkAdjacency_t getAdjacency ( ) const;


  //--------------------------------------------------- getContigs -------------
  //! \brief Get the pair of contig IDs joined by this link
  //!
  //! \return The first and second contig IDs joined by this link
  //!
  std::pair<ID_t, ID_t> getContigs ( ) const
  {
    return contigs_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return ContigLink_t::NCode( );
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
  //! the specified ID from the cooresponding NCode type identifier.
  //!
  //! \return The ID of the link source and the NCode type indentifier.
  //!
  std::pair<ID_t, NCode_t> getSource ( ) const
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


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setAdjacency -----------
  //! \brief Set the adjacent ends of the two contigs
  //!
  //! Set the adjacency information for the contigs, i.e. [N]ORMAL (EB),
  //! [A]NTINORMAL (BE), [I]NNIE (EE) or [O]UTIE (BB). Where B is the
  //! beginning of the contig and E is the end of a contig and EB would mean
  //! the end of contig1 is adjacent to the beginning of contig2.
  //!
  //! \note Will store info in extra portion of BankableFlags
  //!
  //! \param adj The new adjacency of the contigs
  //! \pre adj must be one of [NAIO]
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setAdjacency (LinkAdjacency_t adj);


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
  //! \param source The new ID and NCode type identifier of the link source
  //! \return void
  //!
  void setSource (std::pair<ID_t, NCode_t> source)
  {
    source_m = source;
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the linking type
  //!
  //! \param type The new link type
  //! \pre type is one of [XMOPAS] or NULL
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setType (LinkType_t type)
  {
    switch (type)
      {
      case NULL_LINK:
      case OTHER:
      case MATEPAIR:
      case OVERLAP:
      case PHYSICAL:
      case ALIGNMENT:
      case SYNTENY:
	type_m = type;
	break;
      default:
	AMOS_THROW_ARGUMENT ((std::string)"Invalid contig link type " + type);
      }
    type_m = type;
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __ContigLink_AMOS_HH
