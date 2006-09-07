////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/21/2005
//!
//! \brief Header for Index_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Index_AMOS_HH
#define __Index_AMOS_HH 1

#include "universals_AMOS.hh"
#include <utility>




namespace AMOS {

//================================================ Index_t =====================
//! \brief An association index between two types of AMOS objects
//!
//! Provides an efficient means for storing/retrieving inter-object
//! relationships, e.g. read to mated read. Since most objects only store a
//! minimal set of references, it can be akward and costly to identify related
//! objects not directly linked with an IID reference. The Index_t provides a
//! way to amortize this cost by storing the association information directly
//! for future, repeated use. When in memory, it is essentially a watered-down
//! version of an STL multimap.
//!
//==============================================================================
class Index_t : public Universal_t
{

private:

  std::pair<NCode_t, NCode_t> type_m;            //!< the index type
  HASHMAP::hash_multimap<ID_t, ID_t> index_m;    //!< ID_t -> ID_t index


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


  typedef HASHMAP::hash_multimap<ID_t, ID_t>::iterator iterator;
  //!< Un-mutable index iterator, only itr -> second can be modified

  typedef HASHMAP::hash_multimap<ID_t, ID_t>::const_iterator const_iterator;
  //!< constant index iterator


  //--------------------------------------------------- Index_t ----------------
  //! \brief Constructs an empty Index_t object
  //!
  //! Sets all members to zero or NULL
  //!
  Index_t ( )
  {
    type_m . first = type_m . second = NULL_NCODE;
  }


  //--------------------------------------------------- Index_t ----------------
  //! \brief Constructs an Index_t of a certain type
  //!
  Index_t (NCode_t a, NCode_t b)
  {
    type_m . first = a;
    type_m . second = b;
  }


  //--------------------------------------------------- Index_t ----------------
  //! \brief Copy constructor
  //!
  Index_t (const Index_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Index_t ---------------
  //! \brief Destroys a Index_t object
  //!
  ~Index_t ( )
  {

  }


  //--------------------------------------------------- begin ------------------
  //! \brief Returns a iterator to the beginning of the index
  //!
  //! Iterator is of the type hash_multimap<ID_t, ID_t>::iterator. Access
  //! TO and FROM IIDs with 'itr -> first' and 'itr -> second' respectively.
  //!
  //! \return An iterator to the beginning of the index
  //!
  iterator begin ( )
  {
    return index_m . begin( );
  }


  //--------------------------------------------------- begin ------------------
  const_iterator begin ( ) const
  {
    return index_m . begin( );
  }


  //--------------------------------------------------- buildContigFeature -----
  //! \brief Builds the index with Contig_t Feature_t's
  //!
  //! Builds the index with contig features, i.e. the index will contain a
  //! set of all associated features for each contig IID. Will throw an
  //! exception if there was any trouble opening and reading from the banks.
  //!
  //! \param bankname Name of the bank directory to index
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return void
  //!
  void buildContigFeature (const std::string bankname);


  //--------------------------------------------------- buildContigScaffold ----
  //! \brief Builds the index with Contig_t to Scaffold_t mappings
  //!
  //! Builds the index with contig to scaffold links, i.e. the index will
  //! reference the containing scaffold(s) for each contig IID. Will throw an
  //! exception if there was any trouble opening and reading from the banks.
  //!
  //! \param bankname Name of the bank directory to index
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return void
  //!
  void buildContigScaffold (const std::string bankname);


  //--------------------------------------------------- buildReadContig --------
  //! \brief Builds the index with Read_t to Contig_t mappings
  //!
  //! Builds the index with read to contig links, i.e. the index will
  //! reference the containing contig(s) for each read IID. Will throw an
  //! exception if there was any trouble opening and reading from the banks.
  //!
  //! \param bankname Name of the bank directory to index
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return void
  //!
  void buildReadContig (const std::string bankname);


  //--------------------------------------------------- buildReadLibrary -------
  //! \brief Builds the index with Read_t to Library_t mappings
  //!
  //! Builds the index with read to library links, i.e. the index will
  //! reference the parent library for each read IID. Will throw an exception
  //! if there was any trouble opening and reading from the banks.
  //!
  //! \param bankname Name of the bank directory to index
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return void
  //!
  void buildReadLibrary (const std::string bankname);


  //--------------------------------------------------- buildReadMate ----------
  //! \brief Builds the index with mated Read_t's
  //!
  //! Builds the index with read mates, i.e. the index will reference the
  //! the mate of each read IID. Will throw an exception if there was any
  //! trouble opening and reading from the banks.
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return void
  //!
  void buildReadMate (const std::string bankname);


  //--------------------------------------------------- buildScaffoldFeature ---
  //! \brief Builds the index with Scaffold_t Feature_t's
  //!
  //! Builds the index with scaffold features, i.e. the index will contain a
  //! set of all associated features for each scaffold IID. Will throw an
  //! exception if there was any trouble opening and reading from the banks.
  //!
  //! \param bankname Name of the bank directory to index
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return void
  //!
  void buildScaffoldFeature (const std::string bankname);


  //--------------------------------------------------- end --------------------
  //! \brief Returns a iterator to the end of the index
  //!
  //! Iterator is of the type hash_multimap<ID_t, ID_t>::iterator. Access
  //! TO and FROM IIDs with 'itr -> first' and 'itr -> second' respectively.
  //!
  //! \return A iterator to the end of the index
  //!
  iterator end ( )
  {
    return index_m . end( );
  }


  //--------------------------------------------------- end --------------------
  const_iterator end ( ) const
  {
    return index_m . end( );
  }


  //--------------------------------------------------- empty ------------------
  //! \brief Returns true if the index is empty
  //!
  //! \return True if the index is empty, else false
  //!
  bool empty ( ) const
  {
    return index_m . empty( );
  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Universal_t::clear( );

    type_m . first = type_m . second = NULL_NCODE;
    index_m . clear( );
  }


  //--------------------------------------------------- count ------------------
  //! \brief Returns the number of IDs associated with a particular key
  //!
  //! \return The number of IDs associated with a particular key
  //!
  Size_t count (ID_t key) const
  {
    return index_m . count (key);
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Index_t::NCODE;
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Returns the number of ID pairs in the index
  //!
  //! \return The number of ID pairs in the index
  //!
  Size_t getSize ( ) const
  {
    return index_m . size( );
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the index type as a pair of NCodes
  //!
  //! Get the index type as a pair of NCodes for the FROM and TO type
  //! respectively.
  //!
  //! \return The index type
  //!
  std::pair<NCode_t, NCode_t> getType( ) const
  {
    return type_m;
  }


  //--------------------------------------------------- insert -----------------
  //! \brief Inserts a new IID pair into the index
  //!
  //! Inserts a new IID pair into the index. Index keys are allowed to have
  //! multiple, not necessarily unique, values.
  //!
  //! \param key The index key
  //! \param value The new index value
  //! \return void
  //!
  void insert (ID_t key, ID_t value)
  {
    insert (std::make_pair(key, value));
  }


  //--------------------------------------------------- insert -----------------
  void insert (std::pair<ID_t, ID_t> iids)
  {
    index_m . insert (iids);
  }


  //--------------------------------------------------- isMulti ----------------
  //! \brief Returns true if count is > 1 for a particular key
  //!
  //! \return True if count is > 1 for a particular key
  //!
  bool isMulti (ID_t key) const
  {
    return (count (key) > 1);
  }


  //--------------------------------------------------- lookup -----------------
  //! \brief Look up a single IID by its index key
  //!
  //! Look up a single IID by its index key, even though multiple IDs make be
  //! associated with the same key. If multiple IDs exist for the same key, a
  //! single value will be picked (in no guaranteed fasion) and returned
  //!
  //! \param key The key to lookup
  //! \return An IID value associated with the given key or NULL_ID if none
  //!
  ID_t lookup (ID_t key) const
  {
    const_iterator i = index_m . find (key);
    return ( i == end( ) ? NULL_ID : i -> second );
  }


  //--------------------------------------------------- lookupAll --------------
  //! \brief Look up a list of IIDs by their index key
  //!
  //! Look up all IIDs associated with the same index key. A begin and end
  //! iterator will be returned, representing the bounds of the value list.
  //! Like standard STL begin/end iterators, the begin iterator will point to
  //! the first object and the end iterator will point one past the end object.
  //!
  //! \param key The key to lookup
  //! \return A pair of iterators bounding the values with the given key
  //!
  std::pair<iterator, iterator> lookupAll (ID_t key)
  {
    return index_m . equal_range (key);
  }


  //--------------------------------------------------- lookupAll --------------
  std::pair<const_iterator, const_iterator> lookupAll (ID_t key) const
  {
    return index_m . equal_range (key);
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- remove -----------------
  //! \brief Remove all IIDs associated with a particular key
  //!
  //! Will remove the index key and all values associated with that key. Returns
  //! the number of ID pairs removed.
  //!
  //! \param key The key to remove
  //! \return The number of ID pairs removed
  //!
  Size_t remove (ID_t key)
  {
    return index_m . erase (key);
  }


  //--------------------------------------------------- remove -----------------
  //! \brief Remove a single ID pair
  //!
  //! \param itr An iterator pointing to the pair to remove
  //! \return void
  //!
  void remove (iterator itr)
  {
    index_m . erase (itr);
  }


  //--------------------------------------------------- setType ----------------
  //! \brief Set the index type as a pair of NCodes
  //!
  //! Set the index type as a pair of NCodes for the FROM and TO type
  //! respectively.
  //!
  //! \param type The new index type
  //! \return void
  //!
  void setType (std::pair<NCode_t, NCode_t> type)
  {
    type_m = type;
  }


  //--------------------------------------------------- setType ----------------
  void setType (NCode_t a, NCode_t b)
  {
    setType (std::make_pair (a, b));
  }


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Index_AMOS_HH
