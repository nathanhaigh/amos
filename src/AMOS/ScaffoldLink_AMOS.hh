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

#include "Universal_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include <utility>




namespace AMOS {

//================================================ ScaffoldLink_t ==============
//! \brief Linking information between two scaffolds
//!
//! A single piece of linking information between two scaffolds. These
//! individual links can be bundled together to form reliable scaffold edges.
//!
//==============================================================================
class ScaffoldLink_t : public Universal_t
{

private:

  std::pair<ID_t, ID_t> scaffolds_m;      //!< the pair of scaffold IIDs
  SD_t sd_m;                              //!< standard deviation of the link
  Size_t size_m;                          //!< size of the link
  std::pair<ID_t, NCode_t> source_m;      //!< source of the link
  LinkType_t type_m;                      //!< type of link


protected:

  static const uint8_t ADJACENCY_BIT   = 0x4;  //!< adjacency exists flag
  static const uint8_t ADJACENCY_BITS  = 0x3;  //!< adjacency info mask
  static const uint8_t NORMAL_BITS     = 0x1;
  static const uint8_t ANTINORMAL_BITS = 0x2;
  static const uint8_t INNIE_BITS      = 0x3;
  static const uint8_t OUTIE_BITS      = 0x0;


  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;

  
public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object

  static const LinkType_t NULL_LINK;
  static const LinkType_t OTHER;
  static const LinkType_t MATEPAIR;
  static const LinkType_t OVERLAP;
  static const LinkType_t PHYSICAL;
  static const LinkType_t ALIGNMENT;
  static const LinkType_t SYNTENY;

  static const LinkAdjacency_t NULL_ADJACENCY;
  static const LinkAdjacency_t NORMAL;         //!< E,B
  static const LinkAdjacency_t ANTINORMAL;     //!< B,E
  static const LinkAdjacency_t INNIE;          //!< E,E
  static const LinkAdjacency_t OUTIE;          //!< B,B


  //--------------------------------------------------- ScaffoldLink_t ---------
  //! \brief Constructs an empty ScaffoldLink_t object
  //!
  //! Sets all members to zero or NULL
  //!
  ScaffoldLink_t ( )
  {
    scaffolds_m . first = scaffolds_m . second = source_m . first = NULL_ID;
    source_m . second = NULL_NCODE;
    sd_m = size_m = 0;
    type_m = NULL_LINK;
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
  virtual void clear ( );


  //--------------------------------------------------- flip -------------------
  //! \brief Flip the orientation of the link
  //!
  //! Reverses the order of the scaffold IIDs and changes the adjacency as
  //! altered by the new orientation of the scaffolds. Does not alter adjacency
  //! if it is currently a NULL_ADJACENCY. After flip, NORMAL become ANTINORMAL,
  //! ANTINORMAL becomes NORMAL, and INNIE and OUTIE remain the same.
  //!
  //! \return void
  //!
  void flip ( );


  //--------------------------------------------------- getAdjacency -----------
  //! \brief Get the adjacent ends of the two scaffolds
  //!
  //! Get the adjacency information for the scaffolds, i.e. [N]ORMAL (EB),
  //! [A]NTINORMAL (BE), [I]NNIE (EE) or [O]UTIE (BB). Where B is the
  //! beginning of the scaffold and E is the end of a scaffold and EB means the
  //! end of scaffold1 is adjacent to the beginning of scaffold2.
  //!
  //! \return The pair of adjacent ends
  //!
  LinkAdjacency_t getAdjacency ( ) const;


  //--------------------------------------------------- getScaffolds -----------
  //! \brief Get the pair of scaffold IIDs joined by this link
  //!
  //! \return The first and second scaffold IIDs joined by this link
  //!
  std::pair<ID_t, ID_t> getScaffolds ( ) const
  {
    return scaffolds_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return ScaffoldLink_t::NCODE;
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
  //! The link size is measured as the gap size between the neighboring
  //! scaffolds. For instance, scaffold A ends at 10 (position 9) and scaffold
  //! B starts at 12 (position 12); the gap size would be 12 - 10 = 2.
  //!
  //! \return The size of the link
  //!
  Size_t getSize ( ) const
  {
    return size_m;
  }


  //--------------------------------------------------- getSource --------------
  //! \brief Get the link source IID and type of Bank it is stored in
  //!
  //! This method only returns the IID of the link source and the type of Bank
  //! it is stored in. The entire source object can be retrieved by fetching
  //! the specified IID from the cooresponding NCode type identifier.
  //!
  //! \return The IID of the link source and the NCode type indentifier.
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
  //! \brief Set the adjacent ends of the two scaffolds
  //!
  //! Set the adjacency information for the scaffolds, i.e. [N]ORMAL (EB),
  //! [A]NTINORMAL (BE), [I]NNIE (EE) or [O]UTIE (BB). Where B is the
  //! beginning of the scaffold and E is the end of a scaffold and EB would mean
  //! the end of scaffold1 is adjacent to the beginning of scaffold2.
  //!
  //! \note Will store info in nibble portion of BankFlags
  //!
  //! \param adj The new adjacency of the scaffolds
  //! \pre adj must be one of [NAIO]
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setAdjacency (LinkAdjacency_t adj);


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
    scaffolds_m = scaffolds;
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
  //! \brief Set the link source IID
  //!
  //! \param source The new IID and NCode type identifier of the link source
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
  void setType (LinkType_t type);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __ScaffoldLink_AMOS_HH
