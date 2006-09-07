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

#include "Message_AMOS.hh"
#include <string>
#include <vector>




namespace AMOS {

//================================================ IDMap_t =====================
//! \brief An IID/EID double hash for mapping IIDs and EIDs to BIDs
//!
//! A double hash of IIDs and EIDs to each other and BIDs. Users may lookup an
//! EID or BID via an IID, or may lookup an IID or BID via an EID.
//!
//! A quick review of the identifier nomenclature: BID is a bank index, EID is
//! an external ID, and IID is an internal AMOS ID. BIDs are physical bank
//! indices; useful only for debugging purposes. IIDs are interger IDs used by
//! AMOS for object links and identification, e.g. retrieving an object from a
//! bank. EIDs are string indentifiers of unlimited length, e.g. sequence names.
//! The empty string is used as an equivalent for NULL EID.
//!
//==============================================================================
class IDMap_t : public IMessagable_t
{

public:

  //============================================== HashTriple_t ================
  //! \brief HashTriple for IDMap
  //!
  //! Contains the BID/EID/IID value triple. Also contains a reference counter
  //! which displays the number of hash keys that point to this value triple.
  //!
  //============================================================================
  struct HashTriple_t
  {
    uint8_t  c;               //!< reference counter
    ID_t   iid;               //!< internal AMOS ID
    ID_t   bid;               //!< bank index
    std::string eid;          //!< external ID

    //------------------------------------------------- HashTriple_t -----------
    //! \brief Constructs a HashTriple
    //!
    HashTriple_t (ID_t iid_p, const std::string & eid_p, ID_t bid_p)
      : c (0), iid (iid_p), eid (eid_p), bid (bid_p)
    { }

    //------------------------------------------------- ~HashTriple_t ----------
    //! \brief Destroys a HashTriple
    //!
    ~HashTriple_t ( )
    { }

    //------------------------------------------------- operator= --------------
    //! \brief Deep copy of the HashTriple
    //!
    HashTriple_t & operator= (const HashTriple_t & s);

  };


private:

  static const Size_t DEFAULT_NUM_BUCKETS;  //!< default min buckets

  //============================================== HashNode_t ==================
  //! \brief HashNode for IDMap
  //!
  //! Contains a HashNode pointer and a HashTriple pointer. If next is not NULL
  //! then a collision has occured and next is the pointer to the collision
  //! chain.
  //!
  //============================================================================
  struct HashNode_t
  {
    HashNode_t * next;        //!< collision chain
    HashTriple_t * triple;    //!< hash triple

    //------------------------------------------------- HashNode_t -------------
    //! \brief Constructs an empty HashNode
    //!
    HashNode_t ( )
      : next (NULL), triple (NULL)
    { }


    //------------------------------------------------- ~HashNode_t ------------
    //! \brief Destroys a HashNode
    //!
    ~HashNode_t ( )
    { }


    //------------------------------------------------- clear ------------------
    //! \brief Clears the node
    //!
    //! Deletes its triple if reference count falls to zero and deletes the
    //! next pointer. Resets both pointers to NULL.
    //!
    void clear ( );


    //------------------------------------------------- clearchain -------------
    //! \brief Clears the node and recursively deletes its chain
    //!
    //! Will also delete the triples the chain nodes refer to if a triple's
    //! reference counter falls to zero.
    //!
    void clearchain ( )
    {
      if ( next != NULL )
	next -> clearchain( );
      clear( );
    }
  };


  //============================================== iterator ====================
  //! \brief iterator for moving through the map
  //!
  //============================================================================
  class iterator
  {
  private:
    std::vector<HashNode_t> * iid_bucs;
    std::vector<HashNode_t> * eid_bucs;
    std::vector<HashNode_t>::iterator buc;
    HashNode_t * curr;
    bool iids;
    
  public:
    iterator ( )
    { curr = NULL; }
    iterator (std::vector<HashNode_t> * iid_bucs_p,
	      std::vector<HashNode_t> * eid_bucs_p);
    HashTriple_t & operator*() const
    { return *(curr -> triple); }
    operator HashTriple_t * () const
    { return (curr == NULL ? NULL : curr -> triple); }
    HashTriple_t * operator->() const
    { return (curr == NULL ? NULL : curr -> triple); }
    iterator & operator++();
    iterator operator++(int)
    {
      iterator tmp = *this;
      this->operator++();
      return tmp;
    }
  };


  //--------------------------------------------------- begin ------------------
  iterator begin ( )
  {
    return iterator (&iid_bucs_m, &eid_bucs_m);
  }


  //--------------------------------------------------- end --------------------
  iterator end ( )
  {
    return iterator ( );
  }


  //--------------------------------------------------- minbuckets -------------
  //! \brief Get the minimum number of buckets to satisfy need
  //!
  Size_t minbuckets (Size_t min);


  //--------------------------------------------------- hashfunc ---------------
  //! \brief Hash function for IIDs
  //!
  //! \param key The IID key
  //! \return The appropriate bucket
  //!
  HashNode_t * hashfunc (ID_t key) const
  {
    return &((HashNode_t &)(iid_bucs_m [key % getBuckets( )]));
  }


  //--------------------------------------------------- hashfunc ---------------
  //! \brief Hash function for EIDs
  //!
  //! \param key The EID key
  //! \return The appropriate bucket
  //!
  HashNode_t * hashfunc (const std::string & key) const
  {
    int c;
    unsigned long h = 5381;
    std::string::const_iterator i;
    std::string::const_iterator end = key . end( );

    for ( i = key . begin( ); i != end; i ++ )
      h = ((h << 5) + h) ^ (*i);

    return &((HashNode_t &)(eid_bucs_m [h % getBuckets( )]));
  }


  //--------------------------------------------------- lookupnode -------------
  //! \brief Lookup a hashnode in the hash table
  //!
  //! If key is NULL_ID, false will be returned and node will be set to NULL.
  //!
  //! \param key The key to look for in the hash table
  //! \param node Pointer to adjust
  //! \post Will adjust node to point to the end or right pos of the chain
  //! \return true if found, else false
  //!
  bool lookupnode (ID_t key, HashNode_t * & node) const;


  //--------------------------------------------------- lookupnode -------------
  //! \brief Lookup a hashnode in the hash table
  //!
  //! If key is NULL, false will be returned and node will be set to NULL.
  //!
  //! \param key The key to look for in the hash table
  //! \param node Pointer to adjust
  //! \post Will adjust node to point to the end or right pos of the chain
  //! \return true if found, else false
  //!
  bool lookupnode (const std::string & key, HashNode_t * & node) const;


  //--------------------------------------------------- removenode -------------
  //! \brief Removes a hash node from the collision chain
  //!
  //! Will delete the node's triple if the reference count falls to zero.
  //!
  //! \param curr The node to remove
  //! \param prev The previous node, or NULL if curr is root node
  //! \return void
  //!
  void removenode (HashNode_t * curr, HashNode_t * prev);


  std::vector<HashNode_t> iid_bucs_m;   //!< the iid hash buckets
  std::vector<HashNode_t> eid_bucs_m;   //!< the eid hash buckets
  Size_t size_m;                        //!< number of value triples
  NCode_t type_m;                       //!< type of the IDs


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object


  //============================================== const_iterator ==============
  //! \brief const_iterator for moving through the map
  //!
  //============================================================================
  class const_iterator
  {
  private:
    const std::vector<HashNode_t> * iid_bucs;
    const std::vector<HashNode_t> * eid_bucs;
    std::vector<HashNode_t>::const_iterator buc;
    const HashNode_t * curr;
    bool iids;
    
  public:
    const_iterator ( )
    { curr = NULL; }
    const_iterator (const std::vector<HashNode_t> * iid_bucs_p,
		    const std::vector<HashNode_t> * eid_bucs_p);
    const HashTriple_t & operator*() const
    { return *(curr -> triple); }
    operator const HashTriple_t * () const
    { return (curr == NULL ? NULL : curr -> triple); }
    const HashTriple_t * operator->() const
    { return (curr == NULL ? NULL : curr -> triple); }
    const_iterator & operator++();
    const_iterator operator++(int)
    {
      const_iterator tmp = *this;
      this->operator++();
      return tmp;
    }
  };


  //--------------------------------------------------- IDMap_t ----------------
  //! \brief Contstructs an empty IDMap_t object
  //!
  IDMap_t ( )
    : size_m (0), type_m (NULL_NCODE)
  {
    resize (DEFAULT_NUM_BUCKETS);
  }


  //--------------------------------------------------- IDMap_t ----------------
  //! \brief Contstructs an empty IDMap_t object
  //!
  //! \param buckets Minimum number of hash table buckets to start with
  //!
  IDMap_t (Size_t buckets)
    : size_m (0), type_m (NULL_NCODE)
  {
    resize (buckets);
  }


  //--------------------------------------------------- IDMap_t ----------------
  //! \brief Copy constructor
  //!
  IDMap_t (const IDMap_t & source)
    : size_m (0), type_m (NULL_NCODE)
  {
    *this = source;
  }


  //--------------------------------------------------- ~IDMap_t ---------------
  //! \brief Destroys a IDMap_t object
  //!
  ~IDMap_t ( )
  {
    clear( );
  }


  //--------------------------------------------------- begin ------------------
  //! \brief Returns an IDMap_t::const_iterator to the beginning of the map
  //!
  //! Constant iterator on all the HashTriple_t values in the map. Will go
  //! NULL when at end of list. Test for validity just as you would test a
  //! pointer: for ( myitr = mymap.begin( ); mitr; ++ mitr )
  //!
  const_iterator begin ( ) const
  {
    return const_iterator (&iid_bucs_m, &eid_bucs_m);
  }


  //--------------------------------------------------- end --------------------
  //! \brief Returns an IDMap_t::const_iterator to the end of the map
  //!
  //! Constant iterator on all the HashTriple_t values in the map. Usage similar
  //! to STL iterators, i.e.
  //! for ( itr = map.begin( ); itr != map.end( ); ++ itr )
  //!
  const_iterator end ( ) const
  {
    return const_iterator ( );
  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data
  //!
  //! Clears data, but does not resize the hash table.
  //!
  //! \return void
  //!
  void clear ( );


  //--------------------------------------------------- concat -----------------
  //! \brief Concatenate another IDMap onto this one
  //!
  //! Has no effect if source is the same IDMap as this one.
  //!
  //! \pre The two IDMaps are entirely disjoint
  //! \return void
  //!
  void concat (const IDMap_t & source);


  //--------------------------------------------------- empty ------------------
  //! \brief Returns true if the map is empty
  //!
  //! \return true if empty, else false
  //!
  bool empty ( ) const
  {
    return (size_m == 0);
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Check if a certain EID exists in the map
  //!
  //! Returns true if the EID exists, otherwise false. Will always return
  //! false if key is NULL.
  //!
  //! \param key The EID key to check
  //! \return true if key exists, otherwise false
  //!
  bool exists (const std::string & key) const
  {
    HashNode_t * curr;
    return lookupnode (key, curr);
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Check if a certain IID exists in the map
  //!
  //! Returns true if the IID exists, otherwise false. Will always return
  //! false if key is NULL_ID.
  //!
  //! \param key The IID key to check
  //! \return true if key exists, otherwise false
  //!
  bool exists (ID_t key) const
  {
    HashNode_t * curr;
    return lookupnode (key, curr);
  }


  //--------------------------------------------------- getBuckets -------------
  //! \brief Returns the current number of buckets being used
  //!
  //! \return The current number of buckets  being used
  //!
  Size_t getBuckets ( ) const
  {
    return iid_bucs_m . size( );
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return IDMap_t::NCODE;
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Get the number of value triples in the map
  //!
  //! \return The number of value triples in the map
  //!
  Size_t getSize ( ) const
  {
    return size_m;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief The type of IDs stored in the map
  //!
  //! \return The NCode of the IDs or NULL_NCODE if unset
  //!
  NCode_t getType ( ) const
  {
    return type_m;
  }


  //--------------------------------------------------- insert -----------------
  //! \brief Insert a new EID/IID keyed value into the map
  //!
  //! Inserts a new BID into the map via both an EID and IID key. Has no effect
  //! if *both* EID and IID are NULL, but *one* of them and BID are permitted
  //! to be NULL. If EID is NULL but IID is not, then a triple will be inserted
  //! into the map, keyed only by the IID. If IID is NULL but EID is not, then
  //! a triple will be inserted, keyed only by EID. If neither is NULL, then
  //! the triple will be keyed by both. If both are NULL, there is no effect.
  //!
  //! \param iid The IID key
  //! \param eid The EID key
  //! \param bid The bank index value
  //! \pre The IID does not already exist in the map
  //! \pre The EID does not already exist in the map
  //! \throws ArgumentException_t
  //! \return Pointer to the inserted triple
  //!
  const HashTriple_t * insert (ID_t iid,
                               const std::string & eid,
                               ID_t bid = NULL_ID);


  //--------------------------------------------------- insertEID --------------
  //! \brief Insert a new EID keyed value into the map
  //!
  //! Inserts a new BID into the map via an EID key. Has no effect if EID is
  //! NULL, but BID is permitted to be NULL_ID.
  //!
  //! \param eid The EID key
  //! \param bid The bank index value
  //! \pre EID contains no newlines
  //! \pre The EID does not already exist in the map
  //! \throws ArgumentException_t
  //! \return void
  //!
  const HashTriple_t * insert (const std::string & eid, ID_t bid = NULL_ID)
  {
    return insert (NULL_ID, eid, bid);
  }


  //--------------------------------------------------- insertIID --------------
  //! \brief Insert a new IID keyed value into the map
  //!
  //! Inserts a new BID into the map via an IID key. Has no effect if IID is
  //! NULL_ID, but BID is permitted to be NULL_ID.
  //!
  //! \param iid The IID key
  //! \param bid The bank index value
  //! \pre The IID does not already exist in the map
  //! \throws ArgumentException_t
  //! \return void
  //!
  const HashTriple_t * insert (ID_t iid, ID_t bid = NULL_ID)
  {
    return insert (iid, NULL_STRING, bid);
  }


  //--------------------------------------------------- lookupBID --------------
  //! \brief Lookup the BID associated with an EID key
  //!
  //! \note BID value is permitted to be NULL_ID, so returning NULL_ID does not
  //! guarantee that the EID key does not exist. NULL_ID will always be
  //! returned for a NULL key.
  //!
  //! \param key The EID key of the BID to lookup
  //! \return The BID of the lookup key or NULL_ID if key does not exist
  //!
  ID_t lookupBID (const std::string & key) const
  {
    HashNode_t * curr;
    if ( !lookupnode (key, curr) )
      return NULL_ID;
    return curr -> triple -> bid;
  }


  //--------------------------------------------------- lookupBID --------------
  //! \brief Lookup the BID associated with an IID key
  //!
  //! \note BID value is permitted to be NULL_ID, so returning NULL_ID does not
  //! guarantee that the IID key does not exist. NULL_ID will always be
  //! returned for a NULL_ID key.
  //!
  //! \param key The IID key of the BID to lookup
  //! \return The BID of the lookup key or NULL_ID if key does not exist
  //!
  ID_t lookupBID (ID_t key) const
  {
    HashNode_t * curr;
    if ( !lookupnode (key, curr) )
      return NULL_ID;
    return curr -> triple -> bid;
  }


  //--------------------------------------------------- lookupEID --------------
  //! \brief Lookup the EID associated with an IID key
  //!
  //! \note EID value is permitted to be NULL, so returning the empty string
  //! does not guarantee that the IID key does not exist. The empty string will
  //! always be returned for a NULL_ID key.
  //!
  //! \param key The IID key of the EID to lookup
  //! \return The EID of the lookup key or empty ('\\0') if key does not exist
  //!
  const std::string & lookupEID (ID_t key) const
  {
    HashNode_t * curr;
    if ( !lookupnode (key, curr) )
      return NULL_STRING;
    return curr -> triple -> eid;
  }


  //--------------------------------------------------- lookupIID --------------
  //! \brief Lookup the IID associated with an EID key
  //!
  //! \note IID value is permitted to be NULL_ID, so returning NULL_ID does not
  //! guarantee that the EID key does not exist. NULL_ID will always be
  //! returned for a NULL key.
  //!
  //! \param key The EID key of the IID to lookup
  //! \return The IID of the lookup key or NULL_ID if key does not exist
  //!
  ID_t lookupIID (const std::string & key) const
  {
    HashNode_t * curr;
    if ( !lookupnode (key, curr) )
      return NULL_ID;
    return curr -> triple -> iid;
  }


  //--------------------------------------------------- operator= --------------
  //! \brief Assignment (copy) operator
  //!
  //! Simply clears the current map and calls insert on everything in source.
  //! Does not guarantee that getBuckets( ) will equal source.getBuckets( )
  //! after the assignment. Throws an exception if copy failed due to corrupt
  //! source map.
  //!
  //! \param source The map to copy
  //! \throws ArgumentException_t
  //! \return The resulting map object
  //!
  IDMap_t & operator= (const IDMap_t & source);


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- remove -----------------
  //! \brief Remove an existing value triple from the map by IID key
  //!
  //! Removes an existsing value triple from the map. Has no effect if the key
  //! does not exist or is NULL_ID. If IID is associated with an EID, the EID
  //! key will also be removed.
  //!
  //! \param key The IID key of the triple to remove
  //! \return void
  //!
  void remove (ID_t key);


  //--------------------------------------------------- remove -----------------
  //! \brief Remove an existing value triple from the map by EID key
  //!
  //! Removes an existsing value triple from the map. Has no effect if the key
  //! does not exist or is NULL. If EID is associated with an IID, the IID key
  //! will also be removed.
  //!
  //! \param key The EID key of the triple to remove
  //! \return void
  //!
  void remove (const std::string & key);


  //--------------------------------------------------- resize -----------------
  //! \brief Resize the hash table
  //!
  //! This will cause the hash to reorganize itself and is not recommended
  //! as a frequent operation. If request is larger than max number of buckets
  //! the max number of buckets will be used. Otherwise, the resulting
  //! number of buckets is only guaranteed to be greater than request.
  //!
  //! Number of buckets will automatically increase whenever an insert operation
  //! causes the number of elements to be >= than number of buckets.
  //!
  //! \param min Minimum number of buckets to use
  //! \return void
  //!
  void resize (Size_t min);


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;


  //--------------------------------------------------- read -------------------
  //! \brief Read a tab-delimited IDMap record
  //!
  //! IDMap NCode and size listed on the first line separated by a single space,
  //! followed by one tab-delimited triple per line in the order: bid, iid, eid.
  //! One of the stream's error bits will be set on failure.
  //! The EID strings cannot contain any whitespace
  //!
  //! \param in The stream to read the IDMap from
  //! \return void
  //!
  void read(const std::string & path);


  //--------------------------------------------------- setType ----------------
  //! \brief Set the type of the mapped IDs
  //!
  //! \param type The NCode type of the mapped IDs
  //! \return void
  //!
  void setType (NCode_t type)
  {
    type_m = type;
  }


  //--------------------------------------------------- write ------------------
  //! \brief Write a tab-delimited IDMap record
  //!
  //! IDMap NCode and size listed on the first line separated by a single space,
  //! followed by one tab-delimited triple per line in the order: bid, iid, eid.
  //! One of the stream's error bits will be set on failure.
  //!
  //! \param out The stream to write the IDMap to
  //! \return void
  //!
  void write (std::ostream & out) const;

};

} // namespace AMOS

#endif // #ifndef __IDMap_AMOS_HH
