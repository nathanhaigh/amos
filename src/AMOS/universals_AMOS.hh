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
#include "Fragment_AMOS.hh"
#include "Kmer_AMOS.hh"
#include "Layout_AMOS.hh"
#include "Library_AMOS.hh"
#include "Matepair_AMOS.hh"
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
    operator Universal_t * () const
    { return *i_m; }
    Universal_t * operator->() const
    { return *i_m; }
    iterator & operator++()
    { ++ i_m; }
    iterator operator++(int)
    {
      iterator tmp = *this;
      this->operator++();
      return tmp;
    }
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
    operator const Universal_t * () const
    { return *i_m; }
    const Universal_t * operator->() const
    { return *i_m; }
    const_iterator & operator++()
    { ++ i_m; }
    const_iterator operator++(int)
    {
      const_iterator tmp = *this;
      this->operator++();
      return tmp;
    }
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
  void clearAll ( )
  {
    for ( iterator i = begin( ); i != end( ); ++ i )
      i -> clear( );
  }


  //--------------------------------------------------- end --------------------
  //! \brief Returns an iterator to the end of the set
  //!
  //! \return An iterator to the end of the set
  //!
  iterator end ( ) { return iterator (objs_m . end ( )); }
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
      AMOS_THROW_ARGUMENT ("Unknown Universal_t object NCode " + Decode(ncode));
    return *(i -> second);
  }

  const Universal_t & operator[] (NCode_t ncode) const
  {
    HASHMAP::hash_map<NCode_t, Universal_t *>::const_iterator i;
    if ( (i = hash_m . find (ncode)) == hash_m . end( ) )
      AMOS_THROW_ARGUMENT ("Unknown Universal_t object NCode " + Decode(ncode));
    return *(i -> second);
  }

  Universal_t & operator[] (const std::string & ncode)
  {
    return operator[] (Encode (ncode));
  }

  const Universal_t & operator[] (const std::string & ncode) const
  {
    return operator[] (Encode (ncode));
  }

};

} // namespace AMOS

#endif // #ifndef __universals_AMOS_HH
