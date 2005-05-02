////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 12/05/2003
//!
//! \brief Include file for all of the AMOS universal types
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __universals_AMOS_HH
#define __universals_AMOS_HH 1

#include "ContigEdge_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include "Contig_AMOS.hh"
#include "Edge_AMOS.hh"
#include "Feature_AMOS.hh"
#include "Fragment_AMOS.hh"
#include "Group_AMOS.hh"
#include "Index_AMOS.hh"
#include "Kmer_AMOS.hh"
#include "Layout_AMOS.hh"
#include "Library_AMOS.hh"
#include "Link_AMOS.hh"
#include "Overlap_AMOS.hh"
#include "Read_AMOS.hh"
#include "ScaffoldEdge_AMOS.hh"
#include "ScaffoldLink_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Sequence_AMOS.hh"
#include "Universal_AMOS.hh"

#include "utility_AMOS.hh"
#include <vector>
#include <string>



namespace AMOS {

//================================================ UniversalSet_t ==============
//! \brief A non-redundant, complete collection of Universal objects
//!
//! Provides a non-redundant, complete collection of static Universal objects
//! accessible by their NCode and iteration. Useful when all possible AMOS
//! universal objects are required, but the exact makeup of objects is unknown.
//! Objects stored in this set are static, and only destruced when the class
//! is destructed. Objects are stored in order that will preserve the
//! def-before-ref rule - meaning two objects A,B where B has a link to A will
//! always appear in the order A,B when iterating through the set.
//!
//==============================================================================
class UniversalSet_t
{

private:

  std::vector<Universal_t *> objs_m;     //!< objects in correct order
  HASHMAP::hash_map<NCode_t, Universal_t *> hash_m;   //!< object hash


public:

  class iterator
  {
  private:
    std::vector<Universal_t *>::iterator i_m;

  public:
    iterator ()
    { }
    iterator (std::vector<Universal_t *>::iterator i)
    { i_m = i; }
    Universal_t & operator*() const
    { return **i_m; }
    Universal_t * operator->() const
    { return *i_m; }
    iterator & operator++()
    { ++ i_m; return *this; }
    iterator operator++(int)
    { iterator tmp = *this; this->operator++(); return tmp; }
    bool operator== (iterator x)
    { return i_m == x . i_m; }
    bool operator!= (iterator x)
    { return i_m != x . i_m; }
  };

  class const_iterator
  {
  private:
    std::vector<Universal_t *>::const_iterator i_m;

  public:
    const_iterator ()
    { }
    const_iterator (std::vector<Universal_t *>::const_iterator i)
    { i_m = i; }
    const Universal_t & operator*() const
    { return **i_m; }
    const Universal_t * operator->() const
    { return *i_m; }
    const_iterator & operator++()
    { ++ i_m; return *this; }
    const_iterator operator++(int)
    { const_iterator tmp = *this; this->operator++(); return tmp; }
    bool operator== (const_iterator x)
    { return i_m == x . i_m; }
    bool operator!= (const_iterator x)
    { return i_m != x . i_m; }
  };

  
  //--------------------------------------------------- UniversalSet_t ---------
  //! \brief Constructs a UniversalSet_t w/ default objects
  //!
  UniversalSet_t ( );


  //--------------------------------------------------- ~UniversalSet_t --------
  //! \brief Destroys a UniveralSet_t and its contained objects
  //!
  ~UniversalSet_t ( );


  //--------------------------------------------------- begin ------------------
  //! \brief Returns an iterator to the beginning of the set
  //!
  //! Iterator on all the objects in the set. Operates just like a std::vector
  //! iterator, however it does not support iterator addition/subtraction or
  //! reverse iteration. Objects will always be ordered
  //! definition-before-reference, meaning if B refers to A, A will always
  //! appear before B when iterating forward.
  //!
  //! \return An iterator to the beginning of the set
  //!
  iterator begin ( ) { return iterator (objs_m . begin( )); }
  const_iterator begin ( ) const { return const_iterator (objs_m . begin( )); }


  //--------------------------------------------------- clearAll ---------------
  //! \brief Issues a clear command to each object in the set
  //!
  //! \return void
  //!
  void clearAll ( );


  //--------------------------------------------------- end --------------------
  //! \brief Returns an iterator to the end of the set
  //!
  //! \return An iterator to the end of the set
  //!
  iterator end ( ) { return iterator (objs_m . end( )); }
  const_iterator end ( ) const { return const_iterator (objs_m . end( )); }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks if an object with a given NCode exists in the set
  //!
  //! \return true if it exists, false otherwise
  //!
  bool exists (NCode_t ncode)
  {
    return ( hash_m . find (ncode) != hash_m . end( ) );
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Returns the number of known universal objects
  //!
  //! \return The number of objects in the set
  //!
  Size_t getSize ( )
  {
    return objs_m . size( );
  }


  //--------------------------------------------------- operator[] -------------
  //! \brief Returns a reference to the object with the given NCode
  //!
  //! To call object-specific methods, it will be necessary to cast the return
  //! reference to the type of object you are expecting to be returned. e.g.
  //! `((Read_t &)set["RED"]).setType(Read_t::END)'. However, for virtual
  //! methods inherited from Universal_t, this casting is unnecessary.
  //!
  //! \pre The NCode exists in the set
  //! \throws ArgumentException_t
  //! \return Pointer to the object with the given NCode
  //!
  Universal_t & operator[] (NCode_t ncode)
  {
    HASHMAP::hash_map<NCode_t, Universal_t *>::iterator i;
    if ( (i = hash_m . find (ncode)) == hash_m . end( ) )
      AMOS_THROW_ARGUMENT
	("Unknown UniversalSet_t object NCode " + Decode(ncode));
    return *(i -> second);
  }


  //--------------------------------------------------- operator[] -------------
  const Universal_t & operator[] (NCode_t ncode) const
  {
    HASHMAP::hash_map<NCode_t, Universal_t *>::const_iterator i;
    if ( (i = hash_m . find (ncode)) == hash_m . end( ) )
      AMOS_THROW_ARGUMENT
	("Unknown UniversalSet_t object NCode " + Decode(ncode));
    return *(i -> second);
  }


  //--------------------------------------------------- operator[] -------------
  Universal_t & operator[] (const std::string & ncode)
  {
    return operator[] (Encode (ncode));
  }


  //--------------------------------------------------- operator[] -------------
  const Universal_t & operator[] (const std::string & ncode) const
  {
    return operator[] (Encode (ncode));
  }
};




//================================================ BankSet_t ===================
//! \brief A non-redundant, complete collection of AMOS banks
//!
//! Provides a non-redundant, complete collection of static AMOS banks
//! accessible by their NCode and iteration. Useful when all possible AMOS
//! banks are required, but the exact makeup of banks is unknown. Banks stored
//! in this set are static, and only destruced when the class is destructed.
//! Banks are stored in order that will preserve the def-before-ref rule -
//! meaning two banks A,B where B has a link to A will always appear in the
//! order A,B when iterating through the set.
//!
//==============================================================================
class BankSet_t
{

private:

  std::vector<Bank_t *> bnks_m;     //!< banks in correct order
  HASHMAP::hash_map<NCode_t, Bank_t *> hash_m;   //!< bank hash


public:

  class iterator
  {
  private:
    std::vector<Bank_t *>::iterator i_m;

  public:
    iterator ()
    { }
    iterator (std::vector<Bank_t *>::iterator i)
    { i_m = i; }
    Bank_t & operator*() const
    { return **i_m; }
    Bank_t * operator->() const
    { return *i_m; }
    iterator & operator++()
    { ++ i_m; return *this; }
    iterator operator++(int)
    { iterator tmp = *this; this->operator++(); return tmp; }
    bool operator== (iterator x)
    { return i_m == x . i_m; }
    bool operator!= (iterator x)
    { return i_m != x . i_m; }
  };

  class const_iterator
  {
  private:
    std::vector<Bank_t *>::const_iterator i_m;

  public:
    const_iterator ()
    { }
    const_iterator (std::vector<Bank_t *>::const_iterator i)
    { i_m = i; }
    const Bank_t & operator*() const
    { return **i_m; }
    const Bank_t * operator->() const
    { return *i_m; }
    const_iterator & operator++()
    { ++ i_m; return *this; }
    const_iterator operator++(int)
    { const_iterator tmp = *this; this->operator++(); return tmp; }
    bool operator== (const_iterator x)
    { return i_m == x . i_m; }
    bool operator!= (const_iterator x)
    { return i_m != x . i_m; }
  };

  
  //--------------------------------------------------- BankSet_t --------------
  //! \brief Constructs a BankSet_t w/ default banks
  //!
  BankSet_t ( );


  //--------------------------------------------------- ~BankSet_t -------------
  //! \brief Destroys a UniveralSet_t and closes its contained banks
  //!
  ~BankSet_t ( );


  //--------------------------------------------------- begin ------------------
  //! \brief Returns an iterator to the beginning of the set
  //!
  //! Iterator on all the banks in the set. Operates just like a std::vector
  //! iterator, however it does not support iterator addition/subtraction or
  //! reverse iteration. Banks will always be ordered
  //! definition-before-reference, meaning if B refers to A, A will always
  //! appear before B when iterating forward.
  //!
  //! \return An iterator to the beginning of the set
  //!
  iterator begin ( ) { return iterator (bnks_m . begin( )); }
  const_iterator begin ( ) const { return const_iterator (bnks_m . begin( )); }


  //--------------------------------------------------- clearAll ---------------
  //! \brief Issues a clear command to each bank in the set
  //!
  //! \return void
  //!
  void clearAll ( );


  //--------------------------------------------------- closeAll ---------------
  //! \brief Issues a close command to each bank in the set
  //!
  //! \return void
  //!
  void closeAll ( );


  //--------------------------------------------------- end --------------------
  //! \brief Returns an iterator to the end of the set
  //!
  //! \return An iterator to the end of the set
  //!
  iterator end ( ) { return iterator (bnks_m . end ( )); }
  const_iterator end ( ) const { return const_iterator (bnks_m . end( )); }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks if a bank with a given NCode exists in the set
  //!
  //! \return true if it exists, false otherwise
  //!
  bool exists (NCode_t ncode)
  {
    return ( hash_m . find (ncode) != hash_m . end( ) );
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Returns the number of known AMOS banks
  //!
  //! \return The number of banks in the set
  //!
  Size_t getSize ( )
  {
    return bnks_m . size( );
  }


  //--------------------------------------------------- operator[] -------------
  //! \brief Returns a reference to the bank with the given NCode
  //!
  //! \pre The NCode exists in the set
  //! \throws ArgumentException_t
  //! \return Pointer to the bank with the given NCode
  //!
  Bank_t & operator[] (NCode_t ncode)
  {
    HASHMAP::hash_map<NCode_t, Bank_t *>::iterator i;
    if ( (i = hash_m . find (ncode)) == hash_m . end( ) )
      AMOS_THROW_ARGUMENT
	("Unknown BankSet_t object NCode " + Decode(ncode));
    return *(i -> second);
  }


  //--------------------------------------------------- operator[] -------------
  const Bank_t & operator[] (NCode_t ncode) const
  {
    HASHMAP::hash_map<NCode_t, Bank_t *>::const_iterator i;
    if ( (i = hash_m . find (ncode)) == hash_m . end( ) )
      AMOS_THROW_ARGUMENT
	("Unknown BankSet_t object NCode " + Decode(ncode));
    return *(i -> second);
  }


  //--------------------------------------------------- operator[] -------------
  Bank_t & operator[] (const std::string & ncode)
  {
    return operator[] (Encode (ncode));
  }


  //--------------------------------------------------- operator[] -------------
  const Bank_t & operator[] (const std::string & ncode) const
  {
    return operator[] (Encode (ncode));
  }
};




//================================================ BankStreamSet_t =============
//! \brief A non-redundant, complete collection of AMOS banks
//!
//! Provides a non-redundant, complete collection of static AMOS banks
//! accessible by their NCode and iteration. Useful when all possible AMOS
//! banks are required, but the exact makeup of banks is unknown. Banks stored
//! in this set are static, and only destruced when the class is destructed.
//! Banks are stored in order that will preserve the def-before-ref rule -
//! meaning two banks A,B where B has a link to A will always appear in the
//! order A,B when iterating through the set.
//!
//==============================================================================
class BankStreamSet_t
{

private:

  std::vector<BankStream_t *> bnks_m;     //!< banks in correct order
  HASHMAP::hash_map<NCode_t, BankStream_t *> hash_m;   //!< bank hash


public:

  class iterator
  {
  private:
    std::vector<BankStream_t *>::iterator i_m;

  public:
    iterator ()
    { }
    iterator (std::vector<BankStream_t *>::iterator i)
    { i_m = i; }
    BankStream_t & operator*() const
    { return **i_m; }
    BankStream_t * operator->() const
    { return *i_m; }
    iterator & operator++()
    { ++ i_m; return *this; }
    iterator operator++(int)
    { iterator tmp = *this; this->operator++(); return tmp; }
    bool operator== (iterator x)
    { return i_m == x . i_m; }
    bool operator!= (iterator x)
    { return i_m != x . i_m; }
  };

  class const_iterator
  {
  private:
    std::vector<BankStream_t *>::const_iterator i_m;

  public:
    const_iterator ()
    { }
    const_iterator (std::vector<BankStream_t *>::const_iterator i)
    { i_m = i; }
    const BankStream_t & operator*() const
    { return **i_m; }
    const BankStream_t * operator->() const
    { return *i_m; }
    const_iterator & operator++()
    { ++ i_m; return *this; }
    const_iterator operator++(int)
    { const_iterator tmp = *this; this->operator++(); return tmp; }
    bool operator== (const_iterator x)
    { return i_m == x . i_m; }
    bool operator!= (const_iterator x)
    { return i_m != x . i_m; }
  };

  
  //--------------------------------------------------- BankStreamSet_t --------
  //! \brief Constructs a BankStreamSet_t w/ default banks
  //!
  BankStreamSet_t ( );


  //--------------------------------------------------- ~BankStreamSet_t -------
  //! \brief Destroys a UniveralSet_t and closes its contained banks
  //!
  ~BankStreamSet_t ( );


  //--------------------------------------------------- begin ------------------
  //! \brief Returns an iterator to the beginning of the set
  //!
  //! Iterator on all the banks in the set. Operates just like a std::vector
  //! iterator, however it does not support iterator addition/subtraction or
  //! reverse iteration. Banks will always be ordered
  //! definition-before-reference, meaning if B refers to A, A will always
  //! appear before B when iterating forward.
  //!
  //! \return An iterator to the beginning of the set
  //!
  iterator begin ( ) { return iterator (bnks_m . begin( )); }
  const_iterator begin ( ) const { return const_iterator (bnks_m . begin( )); }


  //--------------------------------------------------- clearAll ---------------
  //! \brief Issues a clear command to each bank in the set
  //!
  //! \return void
  //!
  void clearAll ( );


  //--------------------------------------------------- closeAll ---------------
  //! \brief Issues a close command to each bank in the set
  //!
  //! \return void
  //!
  void closeAll ( );


  //--------------------------------------------------- end --------------------
  //! \brief Returns an iterator to the end of the set
  //!
  //! \return An iterator to the end of the set
  //!
  iterator end ( ) { return iterator (bnks_m . end ( )); }
  const_iterator end ( ) const { return const_iterator (bnks_m . end( )); }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks if a bank with a given NCode exists in the set
  //!
  //! \return true if it exists, false otherwise
  //!
  bool exists (NCode_t ncode)
  {
    return ( hash_m . find (ncode) != hash_m . end( ) );
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Returns the number of known AMOS banks
  //!
  //! \return The number of banks in the set
  //!
  Size_t getSize ( )
  {
    return bnks_m . size( );
  }


  //--------------------------------------------------- operator[] -------------
  //! \brief Returns a reference to the bank with the given NCode
  //!
  //! \pre The NCode exists in the set
  //! \throws ArgumentException_t
  //! \return Pointer to the bank with the given NCode
  //!
  BankStream_t & operator[] (NCode_t ncode)
  {
    HASHMAP::hash_map<NCode_t, BankStream_t *>::iterator i;
    if ( (i = hash_m . find (ncode)) == hash_m . end( ) )
      AMOS_THROW_ARGUMENT
	("Unknown BankStreamSet_t object NCode " + Decode(ncode));
    return *(i -> second);
  }


  //--------------------------------------------------- operator[] -------------
  const BankStream_t & operator[] (NCode_t ncode) const
  {
    HASHMAP::hash_map<NCode_t, BankStream_t *>::const_iterator i;
    if ( (i = hash_m . find (ncode)) == hash_m . end( ) )
      AMOS_THROW_ARGUMENT
	("Unknown BankStreamSet_t object NCode " + Decode(ncode));
    return *(i -> second);
  }


  //--------------------------------------------------- operator[] -------------
  BankStream_t & operator[] (const std::string & ncode)
  {
    return operator[] (Encode (ncode));
  }


  //--------------------------------------------------- operator[] -------------
  const BankStream_t & operator[] (const std::string & ncode) const
  {
    return operator[] (Encode (ncode));
  }
};

} // namespace AMOS

#endif // #ifndef __universals_AMOS_HH
