////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/11/2004
//!
//! \brief Source for universals_AMOS.hh
//!
////////////////////////////////////////////////////////////////////////////////

#include "universals_AMOS.hh"
#include <cassert>
using namespace AMOS;
using namespace std;
using namespace HASHMAP;




//================================================ UniversalSet_t ==============

//----------------------------------------------------- UniversalSet_t ---------
UniversalSet_t::UniversalSet_t ( )
{
  //-- Order must maintain definition-before-reference rule!
  objs_m . push_back (new Universal_t( ));
  objs_m . push_back (new Sequence_t( ));
  objs_m . push_back (new Library_t( ));
  objs_m . push_back (new Fragment_t( ));
  objs_m . push_back (new Read_t( ));
  objs_m . push_back (new Matepair_t( ));
  objs_m . push_back (new Overlap_t( ));
  objs_m . push_back (new Kmer_t( ));
  objs_m . push_back (new Layout_t( ));
  objs_m . push_back (new Contig_t( ));
  objs_m . push_back (new ContigLink_t( ));
  objs_m . push_back (new ContigEdge_t( ));
  objs_m . push_back (new Scaffold_t( ));
  objs_m . push_back (new ScaffoldLink_t( ));
  objs_m . push_back (new ScaffoldEdge_t( ));

  //-- Set up the hash
  for ( iterator i = begin( ); i != end( ); ++ i )
    hash_m [i -> getNCode( )] = i;

  //-- Make sure everything went OK, e.g. all the NCodes were unique
  assert ( objs_m . size( ) == hash_m . size( ) );
}


//----------------------------------------------------- ~UniversalSet_t --------
UniversalSet_t::~UniversalSet_t ( )
{
  for ( iterator i = begin( ); i != end( ); ++ i )
    delete i;
}
