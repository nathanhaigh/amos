////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for IDMap_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "IDMap_AMOS.hh"
#include <cstdio>
#include <string>
#include <sstream>
#include <cstring>
using namespace AMOS;
using namespace std;

const int MAX_EID_LENGTH = 2048; //! Maximum length for an EID



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
      eid = s.eid;
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
bool IDMap_t::lookupnode (const string & key, HashNode_t * & node) const
{
  if ( key . empty( ) )
    {
      node = NULL;
      return false;
    }
  for ( node = hashfunc (key); node -> next != NULL; node = node -> next )
    if ( node -> triple -> eid == key )
      return true;
  if ( node -> triple == NULL  ||  node -> triple -> eid != key )
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
      for ( const_iterator itr = s . begin( ); itr != s . end( ); ++ itr )
	insert (itr -> iid, itr -> eid, itr -> bid);
    }
}


//----------------------------------------------------- insert -----------------
const IDMap_t::HashTriple_t * IDMap_t::insert (ID_t iid,
                                               const string & eid,
					       ID_t bid)
{
  if ( iid == NULL_ID  &&  eid . empty( ) ) return NULL;

  HashNode_t * curri, * curre;

  //cerr << "insert bid: " << bid << "\tiid:" << iid << "\teid: \"" << eid << "\"" << endl;

  if ( lookupnode (iid, curri) )
    {
      ostringstream ss;
      ss << "Cannot insert int key '" << iid << "' multiple times";
      AMOS_THROW_ARGUMENT (ss . str( ));
    }
  if ( lookupnode (eid, curre) )
    {
      ostringstream ss;
      ss << "Cannot insert string key '" << eid << "' multiple times";
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
    Size_t size = -1;
    istringstream ss;

    if ( msg . exists (F_OBJECT) )
      {
        string str = msg . getField (F_OBJECT);

	if ( str . length( ) != NCODE_SIZE )
	  AMOS_THROW_ARGUMENT ("Invalid object type format");
	type_m = Encode (str);
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
	    ss >> bid >> iid;
            ss . ignore( );
	    getline (ss, eid);
	    if ( ! ss . fail( ) )
	      insert (iid, eid, bid);
	  }

	if ( ! ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid map format");
	ss . clear( );

	if ( size >= 0  &&  size != size_m )
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
  string eid;

  HashNode_t * prev = NULL;
  HashNode_t * curr = hashfunc (key);
  if ( curr -> triple != NULL )
    for ( ; curr != NULL; curr = curr -> next )
      {
	if ( curr -> triple -> iid == key )
	  {
            size_m --;
	    if ( curr -> triple -> c > 1 )
	      eid = curr -> triple -> eid;
	    removenode (curr, prev);
	    break;
	  }
	prev = curr;
      }
  
  if ( eid . empty( ) )
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
}


//----------------------------------------------------- remove -----------------
void IDMap_t::remove (const string & key)
{
  if ( key . empty( ) ) return;
  ID_t iid = NULL_ID;

  HashNode_t * prev = NULL;
  HashNode_t * curr = hashfunc (key);
  if ( curr -> triple != NULL )
    for ( ; curr != NULL; curr = curr -> next )
      {
        if ( curr -> triple -> eid == key )
	  {
            size_m --;
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
}


//----------------------------------------------------- resize -----------------
void IDMap_t::resize (Size_t min)
{
  if ( (min = minbuckets (min)) == getBuckets( ) )
    return;

  vector<HashTriple_t *> triples (size_m);
  long int pos = 0;

  if ( size_m > 0 )
    {
      //-- Collect all the triples
      for ( iterator itr = begin( ); itr != end ( ); ++ itr )
	{
	  itr -> c ++;
          triples [pos ++] = itr;
	}

      Size_t  size = size_m;
      NCode_t type = type_m;
      clear( );
      size_m = size;
      type_m = type;
    }

  //-- Resize the bucket vectors
  if ( size_m != pos )
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
      msg . setField (F_OBJECT, Decode (type_m));
    
    ss << size_m;
    msg . setField (F_SIZE, ss . str( ));
    ss . str (NULL_STRING);
    
    if (size_m != 0 )
      {
	//-- Output all the triples
	string str;
        for ( const_iterator itr = begin( ); itr != end( ); ++ itr )
	  {
	    ss << itr -> bid << '\t' << itr -> iid << '\t';
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


//----------------------------------------------------- read -------------------
void IDMap_t::read(const std::string & path)
{
  Size_t size;
  ID_t bid, iid;
  char buffer [MAX_EID_LENGTH+1];

  clear( );

  FILE * fp = fopen(path.c_str(), "r");

  if (!fp)
    { AMOS_THROW_IO("Could not open bank partition, " + path); } 

  if (fscanf(fp, "%s %d", buffer, &size) != 2)
    { AMOS_THROW_IO("Invalid Header, Map file is corrupted for " + path); }

  type_m = Encode (buffer);
  resize (size);

  while (fscanf(fp, "%d\t%d", &bid, &iid) == 2)
  {
    int c = getc(fp);
    if (c != '\t') 
      { AMOS_THROW_IO("Map file is corrupted for " + path); }

    // Go to <= MAX_EID_LENGTH so that we know when at least 1 char is chopped
    int i;
    bool done = false;
    for (i = 0; i <= MAX_EID_LENGTH; i++)
    {
      c = getc(fp);
      buffer[i] = c;

      if (c == '\n' || c == EOF) 
      { 
        buffer[i] = '\0';
        done = true; break; 
      }
    }

    if (!done)
    {
      buffer[MAX_EID_LENGTH] = '\0';
      int chop = 0; // This immediately goes to 1
      while (!(c == '\n' || c == EOF))
      {
        chop++;
        c = getc(fp);
      }

      cerr << "WARNING: Truncated EID for IID " << iid << " to " << MAX_EID_LENGTH << " characters (chopped last " << chop <<")" << endl;
    }

    insert (iid, buffer, bid);
  }

  if (size_m != size)
  {
    AMOS_THROW_IO ("Bank Corrupted: Number of named elements doesn't match map header in " + path);
  }

  fclose(fp);
}


//----------------------------------------------------- write ------------------
void IDMap_t::write (ostream & out) const
{
  out << Decode(type_m) << ' ' << size_m << NL_CHAR;

  for ( const_iterator itr = begin( ); itr != end( ); ++ itr )
    out << itr -> bid << '\t' << itr -> iid << '\t' << itr -> eid << NL_CHAR;
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
