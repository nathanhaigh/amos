////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for IDMap_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __IDMap_AMOS_HH
#define __IDMap_AMOS_HH 1

#include "Bank_AMOS.hh"




namespace AMOS {

//================================================ IDMap_t =====================
//! \brief A one-to-one mapping of IDs
//!
//! Usually to store the mapping of eid's to iid's when storing Message data
//! in an AMOS Bank.
//!
//==============================================================================
class IDMap_t : public IBankable_t
{

private:

  static const Size_t BUCKETS = 100000;  //!< size of hash table


  //============================================== HashNode_t ==================
  //! \brief HashNode for IDMap
  //!
  //! Contains a key/value ID pair and a HashNode pointer. If next is not NULL
  //! then a collision has occured and next is the pointer to the collision
  //! chain.
  //!
  //============================================================================
  struct HashNode_t
  {
    ID_t key;                  //!< hash key
    ID_t val;                  //!< value of the key mapping
    HashNode_t * next;         //!< collision chain

    //------------------------------------------------- HashNode_t -------------
    //! \brief Constructs an empty HashNode
    //!
    //! Initializes all fields to NULL
    //!
    HashNode_t ( )
    {
      key = val = NULL_ID;
      next = NULL;
    }


    //------------------------------------------------- ~HashNode_t ------------
    //! \brief Destroys a HashNode and frees collision chain
    //!
    //! Destroys a HashNode and frees collision chain
    //!
    ~HashNode_t ( )
    {
      delete next;
    }


    //------------------------------------------------- clear ------------------
    //! \brief Clears the object and frees collision chain
    //!
    //! \return void
    //!
    void clear ( )
    {
      key = val = NULL_ID;
      delete next;
      next = NULL;
    }
  };


  HashNode_t table_m [BUCKETS];      //!< the hash table

  ID_t size_m;                       //!< the number of entries


  //--------------------------------------------------- hashfunc ---------------
  //! \brief Simple division method hash function
  //!
  //! \param key The hash key
  //! \return Address of the appropriate hashnode
  //!
  HashNode_t * hashfunc (ID_t key)
  {
    return table_m + (key % BUCKETS);
  }


protected:

  //--------------------------------------------------- readRecord -------------
  virtual Size_t readRecord (std::istream & fix,
			     std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  virtual Size_t writeRecord (std::ostream & fix,
			      std::ostream & var) const;


public:

  //--------------------------------------------------- NCode ------------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Can be used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (IDMap_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::IDMAP;
  }


  //--------------------------------------------------- IDMap_t ----------------
  //! \brief Contstructs an empty IDMap_t object
  //!
  //! Sets all members to NULL
  //!
  IDMap_t ( )
  {
    size_m = 0;
  }


  //--------------------------------------------------- IDMap_t ----------------
  //! \brief Copy constructor
  //!
  IDMap_t (const IDMap_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~IDMap_t ---------------
  //! \brief Destroys a IDMap_t object
  //!
  ~IDMap_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    IBankable_t::clear( );

    for ( Size_t i = 0; i < BUCKETS; i ++ )
      table_m [i] . clear( );
    size_m = 0;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return IDMap_t::NCode( );
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Get the number of key/value pairs in the mapping
  //!
  //! \return The number of key/value pairs in the mapping
  //!
  ID_t getSize ( )
  {
    return size_m;
  }


  //--------------------------------------------------- insert -----------------
  //! \brief Insert a new key/value pair in the map
  //!
  //! \param key The key ID
  //! \param val The mapped value of the key ID
  //! \pre key does not already exist in the map and != NULL
  //! \pre val does not already exist in the map (not checked) and != NULL
  //! \throws ArgumentException_t
  //! \return void
  //!
  void insert (ID_t key, ID_t val);


  //--------------------------------------------------- lookup -----------------
  //! \brief Lookup the value of a certain key
  //!
  //! Returns NULL_ID if key == NULL_ID
  //!
  //! \param The key of the value to lookup
  //! \pre key exists in the map
  //! \throws ArgumentException_t
  //! \return The ID of the lookup value
  //!
  ID_t lookup (ID_t key);


  //--------------------------------------------------- operator= --------------
  //! \brief Assignment (copy) operator
  //!
  //! Efficiently copies the compressed data from the other map
  //!
  //! \param source The map to copy
  //! \return The resulting map object
  //!
  IDMap_t & operator= (const IDMap_t & source);


  //--------------------------------------------------- remove -----------------
  //! \brief Remove an existing key/value pair from the map
  //!
  //! Removes an existsing key/value pair from the map. Has no effect if the
  //! key does not exist.
  //!
  //! \param key The key of the pair to remove
  //! \return void
  //!
  void remove (ID_t key);

};

} // namespace AMOS

#endif // #ifndef __IDMap_AMOS_HH
