////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for IDMap_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "IDMap_AMOS.hh"
#include <string>
#include <sstream>
#include <cstring>
using namespace AMOS;
using namespace std;





//================================================ HashTriple_t ================
//----------------------------------------------------- operator= --------------
IDMap_t::HashTriple_t & IDMap_t::HashTriple_t::operator=
(const HashTriple_t & s)
{
  if ( this != &s )
    {
      c = s.c;
      iid = s.iid;
      bid = s.bid;
      eid = SafeStrdup (s.eid);
    }
  return *this;
}




//================================================ HashNode_t ==================
//----------------------------------------------------- clear ------------------
void IDMap_t::HashNode_t::clear ( )
{
  if ( triple != NULL )
    if ( -- (triple -> c) == 0 )
      delete triple;
  delete next;
  
  triple = NULL;
  next = NULL;
}




//================================================ IDMap_t =====================
const NCode_t IDMap_t::NCODE = M_IDMAP;
const Size_t IDMap_t::DEFAULT_NUM_BUCKETS = 1000;


//----------------------------------------------------- clear ------------------
void IDMap_t::clear ( )
{
  if ( size_m > 0 )
    {
      vector<HashNode_t>::iterator hni;
      for ( hni = iid_bucs_m . begin( ); hni != iid_bucs_m . end( ); hni ++ )
	hni -> clearchain( );
      for ( hni = eid_bucs_m . begin( ); hni != eid_bucs_m . end( ); hni ++ )
	hni -> clearchain( );
      size_m = 0;
    }
  type_m = NULL_NCODE;
}


//----------------------------------------------------- minbuckets -------------
Size_t IDMap_t::minbuckets (Size_t min)
{
  //-- Bucket size constants
  const uint8_t NUM_BUCKET_SIZES = 27;
  const Size_t BUCKET_SIZES [NUM_BUCKET_SIZES] =
    {
      53l,         97l,         193l,       389l,       769l,
      1543l,       3079l,       6151l,      12289l,     24593l,
      49157l,      98317l,      196613l,    393241l,    786433l,
      1572869l,    3145739l,    6291469l,   12582917l,  25165843l,
      50331653l,   100663319l,  201326611l, 402653189l, 805306457l, 
      1610612741l, 2147354947l
    };

  uint8_t first = 0;
  uint8_t len = NUM_BUCKET_SIZES;
  uint8_t half, middle;
  
  //-- Binary search for a bucket size greater than hint
  while ( len > 0 )
    {
      half = len >> 1;
      middle = first + half;
      if ( BUCKET_SIZES [middle] < min )
	{
	  first = middle + 1;
	  len = len - half - 1;
	}
      else
	len = half;
    }
  if ( first == NUM_BUCKET_SIZES )
    first --;

  return BUCKET_SIZES [first];
}


//----------------------------------------------------- lookupnode -------------
bool IDMap_t::lookupnode (ID_t key, HashNode_t * & node) const
{
  if ( key == NULL_ID )
    {
      node = NULL;
      return false;
    }
  for ( node = hashfunc (key); node -> next != NULL; node = node -> next )
    if ( node -> triple -> iid == key )
      return true;
  if ( node -> triple == NULL  ||  node -> triple -> iid != key )
    return false;
  else
    return true;
}


//----------------------------------------------------- lookupnode -------------
bool IDMap_t::lookupnode (const char * key, HashNode_t * & node) const
{
  if ( key == NULL || key[0] == NULL_CHAR )
    {
      node = NULL;
      return false;
    }
  for ( node = hashfunc (key); node -> next != NULL; node = node -> next )
    if ( strcmp (node -> triple -> eid, key) == 0 )
      return true;
  if ( node -> triple == NULL  ||  strcmp (node -> triple -> eid, key) != 0)
    return false;
  else
    return true;
}


//----------------------------------------------------- removenode -------------
void IDMap_t::removenode (HashNode_t * curr, HashNode_t * prev)
{
  if ( prev == NULL )
    //-- If a root node
    {
      if ( curr -> next == NULL )
	curr -> clear( );                  // free curr's triple
      else
	{
	  prev = curr -> next;             // this will be the new root
	  curr -> next = NULL;             // as to not delete next on clear
	  curr -> clear( );                // free curr's triple
	  *curr = *prev;                   // overwrite root node
	  delete prev;                     // free duplicate node
	}
    }
  else
    //-- If not a root node
    {
      prev -> next = curr -> next;         // bypass curr
      curr -> next = NULL;                 // as to not delete next on clear
      curr -> clear( );                    // free curr's triple
      delete curr;                         // free removed node
    }
}


//----------------------------------------------------- concat -----------------
void IDMap_t::concat (const IDMap_t & s)
{
  if ( this != &s )
    {
      resize (getSize( ) + s . getSize( ));

      //-- Copy all the triples
      for ( const_iterator itr (&(s.iid_bucs_m), &(s.eid_bucs_m)); itr; ++ itr )
	insert (itr -> iid, itr -> eid, itr -> bid);
    }
}


//----------------------------------------------------- insert -----------------
const IDMap_t::HashTriple_t * IDMap_t::insert (ID_t iid,
					       const char * eid,
					       ID_t bid)
{
  if ( iid == NULL_ID  &&  (eid == NULL || eid[0] == NULL_CHAR) ) return NULL;

  HashNode_t * curri, * curre;

  if ( lookupnode (iid, curri) )
    {
      ostringstream ss;
      ss << "Cannot insert int key '" << iid << "' multiple times";
      AMOS_THROW_ARGUMENT (ss . str( ));
    }
  if ( lookupnode (eid, curre) )
    {
      ostringstream ss;
      ss << "Cannot insert char key '" << eid << "' multiple times";
      AMOS_THROW_ARGUMENT (ss . str( ));
    }

  HashTriple_t * currt = new HashTriple_t (iid, eid, bid);

  if ( curri != NULL )
    {
      if ( curri -> triple != NULL )
	curri = curri -> next = new HashNode_t( );
      curri -> triple = currt;
      currt -> c ++;
    }
  if ( curre != NULL )
    {
      if ( curre -> triple != NULL )
	curre = curre -> next = new HashNode_t( );
      curre -> triple = currt;
      currt -> c ++;
    }

  if ( ++ size_m >= getBuckets( ) )
    resize (size_m + 1);

  return currt;
}


//----------------------------------------------------- readMessage ------------
void IDMap_t::readMessage (const Message_t & msg)
{
  clear( );

  try {
    Size_t size = 0;
    istringstream ss;

    if ( msg . exists (F_TYPE) )
      {
	if ( msg . getField (F_TYPE) . length( ) != NCODE_SIZE )
	  AMOS_THROW_ARGUMENT ("Invalid type format");
	type_m = Encode (msg . getField (F_TYPE));
      }

    if ( msg . exists (F_SIZE) )
      {
	ss . str (msg . getField (F_SIZE));
	ss >> size;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid size format");
	ss . clear( );

	resize (size);
      }

    if ( msg . exists (F_MAP) )
      {
	string eid;
	ID_t bid, iid;
	ss . str (msg . getField (F_MAP));

	while ( ss )
	  {
	    ss >> bid;
	    ss . ignore( );
	    ss >> iid;
	    ss . ignore( );
	    getline (ss, eid);
	    if ( ! ss . fail( ) )
	      insert (iid, eid . c_str( ), bid);
	  }

	if ( ! ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid map format");
	ss . clear( );

	if ( size != 0  &&  size != size_m )
	  AMOS_THROW_ARGUMENT ("map and size fields do not agree");
      }
  }
  catch (ArgumentException_t) {

    clear( );
    throw;
  }
}


//----------------------------------------------------- remove -----------------
void IDMap_t::remove (ID_t key)
{
  if ( key == NULL_ID ) return;
  const char * eid = NULL;

  HashNode_t * prev = NULL;
  HashNode_t * curr = hashfunc (key);
  if ( curr -> triple != NULL )
    for ( ; curr != NULL; curr = curr -> next )
      {
	if ( curr -> triple -> iid == key )
	  {
	    if ( curr -> triple -> c > 1 )
	      eid = curr -> triple -> eid;
	    removenode (curr, prev);
	    break;
	  }
	prev = curr;
      }
  
  if ( eid == NULL )
    return;

  prev = NULL;
  curr = hashfunc (eid);
  if ( curr -> triple != NULL )
    for ( ; curr != NULL; curr = curr -> next )
      {
	if ( curr -> triple -> eid == eid )
	  {
	    removenode (curr, prev);
	    break;
	  }
	prev = curr;
      }

  size_m --;
}


//----------------------------------------------------- remove -----------------
void IDMap_t::remove (const char * key)
{
  if ( key == NULL || key[0] == NULL_CHAR ) return;
  ID_t iid = NULL_ID;

  HashNode_t * prev = NULL;
  HashNode_t * curr = hashfunc (key);
  if ( curr -> triple != NULL )
    for ( ; curr != NULL; curr = curr -> next )
      {
	if ( strcmp (curr -> triple -> eid, key) == 0 )
	  {
	    if ( curr -> triple -> c > 1 )
	      iid = curr -> triple -> iid;
	    removenode (curr, prev);
	    break;
	  }
	prev = curr;
      }
  
  if ( iid == NULL_ID )
    return;

  prev = NULL;
  curr = hashfunc (iid);
  if ( curr -> triple != NULL )
    for ( ; curr != NULL; curr = curr -> next )
      {
	if ( curr -> triple -> iid == iid )
	  {
	    removenode (curr, prev);
	    break;
	  }
	prev = curr;
      }

  size_m --;
}


//----------------------------------------------------- resize -----------------
void IDMap_t::resize (Size_t min)
{
  if ( (min = minbuckets (min)) == getBuckets( ) )
    return;

  vector<HashTriple_t *> triples;
  triples . reserve (size_m);

  if ( size_m > 0 )
    {
      //-- Collect all the triples
      for ( iterator itr (&iid_bucs_m, &eid_bucs_m); itr; ++ itr )
	{
	  itr -> c ++;
	  triples . push_back (itr);
	}

      Size_t  size = size_m;
      NCode_t type = type_m;
      clear( );
      size_m = size;
      type_m = type;
    }

  //-- Resize the bucket vectors
  if ( size_m != (Size_t)triples . size( ) )
    AMOS_THROW ("Unknown fatal error during hash resize");
  iid_bucs_m . resize (min);
  eid_bucs_m . resize (min);

  //-- Put the triples back in the map via modified insert
  HashNode_t * curri, * curre;
  vector<HashTriple_t *>::iterator tpi;
  for ( tpi = triples . begin( ); tpi != triples . end( ); tpi ++ )
    {
      lookupnode ((*tpi) -> iid, curri);
      lookupnode ((*tpi) -> eid, curre);

      if ( curri != NULL )
	{
	  if ( curri -> triple != NULL )
	    curri = curri -> next = new HashNode_t( );
	  curri -> triple = (*tpi);
	  curri -> triple -> c ++;
	}
      if ( curre != NULL )
	{
	  if ( curre -> triple != NULL )
	    curre = curre -> next = new HashNode_t( );
	  curre -> triple = (*tpi);
	  curre -> triple -> c ++;
	}

      (*tpi) -> c --;
    }
}


//----------------------------------------------------- writeMessage -----------
void IDMap_t::writeMessage (Message_t & msg) const
{
  msg . clear( );

  try {
    ostringstream ss;

    msg . setMessageCode (IDMap_t::NCODE);
    
    if ( type_m != NULL_NCODE )
      msg . setField (F_TYPE, Decode (type_m));
    
    ss << size_m;
    msg . setField (F_SIZE, ss . str( ));
    ss . str (NULL_STRING);
    
    if (size_m != 0 )
      {
	//-- Output all the triples
	string str;
	for ( const_iterator itr (&iid_bucs_m, &eid_bucs_m); itr; ++ itr )
	  {
	    ss << itr -> bid << ',' << itr -> iid << ',';
	    str . append (ss . str( ));
	    ss . str (NULL_STRING);
	    str . append (itr -> eid);
	    str . push_back (NL_CHAR);
	  }
	
	msg . setField (F_MAP, str);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- operator= --------------
IDMap_t & IDMap_t::operator= (const IDMap_t & s)
{
  if ( this != &s )
    {
      clear( );
      type_m = s . type_m;
      concat (s);
    }

  return *this;
}


//----------------------------------------------------- readRecord -------------
void IDMap_t::readRecord (istream & in)
{
  Size_t size;
  string eid;
  ID_t bid, iid;

  clear( );
  readLE (in, &type_m);
  readLE (in, &size);
  resize (size);
  for ( Size_t i = 0; i < size; i ++ )
    {
      readLE (in, &bid);
      readLE (in, &iid);
      getline (in, eid, NULL_CHAR);
      insert (iid, eid . c_str( ), bid);
    }
}


//----------------------------------------------------- writeRecord ------------
void IDMap_t::writeRecord (ostream & out) const
{
  writeLE (out, &type_m);
  writeLE (out, &size_m);
  for ( const_iterator itr (&iid_bucs_m, &eid_bucs_m); itr; ++ itr )
    {
      writeLE (out, &(itr -> bid));
      writeLE (out, &(itr -> iid));
      out . write (itr -> eid, strlen (itr -> eid) + 1);
    }
}





//================================================ iterator ====================
//------------------------------------------------ iterator --------------------
IDMap_t::iterator::iterator (vector<HashNode_t> * iid_bucs_p,
			     vector<HashNode_t> * eid_bucs_p)
  : iid_bucs (iid_bucs_p), eid_bucs (eid_bucs_p)
{
  if ( iid_bucs -> empty( ) )
    curr = NULL;
  else
    {
      iids = true;
      buc = iid_bucs -> begin( );
      curr = &(*buc);
      if ( curr -> triple == NULL )
	this->operator++();
    }
}


//------------------------------------------------ operator++ ------------------
IDMap_t::iterator & IDMap_t::iterator::operator++ ( )
{
  while ( curr != NULL )
    {
      curr = curr -> next;
      while ( curr == NULL )
	{
	  ++ buc;

	  if ( iids  &&  buc >= iid_bucs -> end( ) )
	    {
	      iids = false;
	      buc = eid_bucs -> begin( );
	    }
		
	  if ( !iids  &&  buc >= eid_bucs -> end( ) )
	    return *this;

	  if ( buc -> triple != NULL )
	    curr = &(*buc);
	}

      if ( iids  ||  curr -> triple -> c == 1 )
	break;
    }
  return *this;
}


//------------------------------------------------ const_iterator --------------
IDMap_t::const_iterator::const_iterator (const vector<HashNode_t> * iid_bucs_p,
					 const vector<HashNode_t> * eid_bucs_p)
  : iid_bucs (iid_bucs_p), eid_bucs (eid_bucs_p)
{
  if ( iid_bucs -> empty( ) )
    curr = NULL;
  else
    {
      iids = true;
      buc = iid_bucs -> begin( );
      curr = &(*buc);
      if ( curr -> triple == NULL )
	this->operator++();
    }
}


//------------------------------------------------ operator++ ------------------
IDMap_t::const_iterator & IDMap_t::const_iterator::operator++ ( )
{
  while ( curr != NULL )
    {
      curr = curr -> next;
      while ( curr == NULL )
	{
	  ++ buc;

	  if ( iids  &&  buc >= iid_bucs -> end( ) )
	    {
	      iids = false;
	      buc = eid_bucs -> begin( );
	    }
		
	  if ( !iids  &&  buc >= eid_bucs -> end( ) )
	    return *this;

	  if ( buc -> triple != NULL )
	    curr = &(*buc);
	}

      if ( iids  ||  curr -> triple -> c == 1 )
	break;
    }
  return *this;
}
