////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/12/2005
//!
//! \brief Header for Link_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Link_AMOS_HH
#define __Link_AMOS_HH 1

#include "Universal_AMOS.hh"
#include <utility>




namespace AMOS {

typedef char LinkType_t;
typedef char LinkAdjacency_t;

//====================================================== Link_t ================
//! \brief Linking information between two nodes, such as between contigs
//!
//! A single piece of linking information between two nodes. For example, a
//! probable extension of this class would be a ContigLink_t to represent an
//! inferred relationship between two contigs. These atomic links can be
//! bundled together to form an Edge_t.
//!
//==============================================================================
class Link_t : public Universal_t
{

private:

  std::pair<ID_t, ID_t> nods_m;           //!< the pair of nodes
  std::pair<ID_t, NCode_t> source_m;      //!< source of the link
  NCode_t nodtype_m;                      //!< the type node object
  SD_t sd_m;                              //!< standard deviation of the link
  Size_t size_m;                          //!< size of the link
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


  //--------------------------------------------------- readRecordFix ----------
  virtual void readRecordFix (std::istream & fix);


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;

  
public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object

  static const LinkType_t NULL_LINK = 0;
  static const LinkType_t OTHER     = 'X';
  static const LinkType_t MATEPAIR  = 'M';
  static const LinkType_t OVERLAP   = 'O';
  static const LinkType_t PHYSICAL  = 'P';
  static const LinkType_t ALIGNMENT = 'A';
  static const LinkType_t SYNTENY   = 'S';

  static const LinkAdjacency_t NULL_ADJACENCY = 0;
  static const LinkAdjacency_t NORMAL         = 'N';  //!< E,B
  static const LinkAdjacency_t ANTINORMAL     = 'A';  //!< B,E
  static const LinkAdjacency_t INNIE          = 'I';  //!< E,E
  static const LinkAdjacency_t OUTIE          = 'O';  //!< B,B


  //--------------------------------------------------------- Link_t -----------
  //! \brief Constructs an empty Link_t object
  //!
  //! Sets all members to zero or NULL
  //!
  Link_t ( )
  {
    nods_m . first = nods_m . second = source_m . first = NULL_ID;
    source_m . second = NULL_NCODE;
    nodtype_m = NULL_NCODE;
    sd_m = size_m = 0;
    type_m = NULL_LINK;
  }


  //--------------------------------------------------------- Link_t -----------
  //! \brief Copy constructor
  //!
  Link_t (const Link_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------------- ~Link_t ----------
  //! \brief Destroys a Link_t object
  //!
  ~Link_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( );


  //--------------------------------------------------- flip -------------------
  //! \brief Flip the orientation of the link
  //!
  //! Reverses the order of the IIDs and changes the adjacency as altered by
  //! the new orientation of the nodes. Does not alter adjacency if it is
  //! currently a NULL_ADJACENCY. After flip, NORMAL become ANTINORMAL,
  //! ANTINORMAL becomes NORMAL, and INNIE and OUTIE remain the same.
  //!
  //! \return void
  //!
  void flip ( );


  //--------------------------------------------------- getAdjacency -----------
  //! \brief Get the adjacency relation of the two nodes
  //!
  //! Get the adjacency information for the nodes, i.e. [N]ORMAL (EB),
  //! [A]NTINORMAL (BE), [I]NNIE (EE) or [O]UTIE (BB). Where, if we picture
  //! each node as an arrow, B is the tail and E is the head. Thus EB means the
  //! head of node1 is adjacent to the tail of node2. This may seem awkward in
  //! a theoretical sense, but comes in handy when the nodes are directed
  //! sequences.
  //!
  //! \return The pair of adjacent ends
  //!
  LinkAdjacency_t getAdjacency ( ) const;


  //----------------------------------------------------- getNodes -------------
  //! \brief Get the pair of node IIDs joined by this link
  //!
  //! \return The first and second node IIDs joined by this link
  //!
  std::pair<ID_t, ID_t> getNodes ( ) const
  {
    return nods_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Link_t::NCODE;
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
  //! \brief Get the weight/size of the link
  //!
  //! \return The weight/size of the link
  //!
  Size_t getSize ( ) const
  {
    return size_m;
  }


  //--------------------------------------------------- getSource --------------
  //! \brief Get the link source IID and type
  //!
  //! This method only returns the IID of the link source and the object type.
  //! The entire source object can be obtained by fetching the specified IID
  //! from the corresponding bank.
  //!
  //! \return The IID of the link source and the NCode type indentifier
  //!
  std::pair<ID_t, NCode_t> getSource ( ) const
  {
    return source_m;
  }


  //----------------------------------------------------- getNodeType ----------
  //! \brief Get the node type
  //!
  //! Describes the type of IID the nodes represent.
  //!
  //! \return The node type
  //!
  NCode_t getNodeType( ) const
  {
    return nodtype_m;
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
  //! \brief Set the adjacent ends of the two nodes
  //!
  //! Set the adjacency information for the nodes, i.e. [N]ORMAL (EB),
  //! [A]NTINORMAL (BE), [I]NNIE (EE) or [O]UTIE (BB). Where, if we picture
  //! each node as an arrow, B is the tail and E is the head. Thus EB means the
  //! head of node1 is adjacent to the tail of node2. This may seem awkward in
  //! a theoretical sense, but comes in handy when the nodes are directed
  //! sequences.
  //!
  //! \note Will store info in nibble portion of BankFlags
  //!
  //! \param adj The new adjacency of the nodes
  //! \pre adj must be one of [NAIO]
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setAdjacency (LinkAdjacency_t adj);


  //----------------------------------------------------- setNodes -------------
  //! \brief Set the node IID pair for this link
  //!
  //! Only returns the IIDs of the nodes. It is up to the user to keep track
  //! of where the actual node objects are stored.
  //!
  //! \param nods The new pair of node IIDs
  //! \return void
  //!
  void setNodes (std::pair<ID_t, ID_t> nods)
  {
    nods_m = nods;
  }


  //----------------------------------------------------- setNodeType ----------
  //! \brief Set the type of node
  //!
  //! Describes the type of IID the nodes represent.
  //!
  //! \param nodtype The new node type
  //! \return void
  //!
  void setNodeType (NCode_t nodtype)
  {
    nodtype_m = nodtype;
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
  //! \brief Set the weight/size of the link
  //!
  //! \param size The new link weight/size
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

#endif // #ifndef __Link_AMOS_HH
