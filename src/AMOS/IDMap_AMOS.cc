////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for IDMap_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "IDMap_AMOS.hh"
using namespace AMOS;
using namespace Message_k;
using namespace std;




//================================================ IDMap_t =====================
//----------------------------------------------------- insert -----------------
void IDMap_t::insert (ID_t key, ID_t val)
{
  if ( key == NULL_ID )
    AMOS_THROW_ARGUMENT ("Cannot insert NULL_ID map key");
  if ( val == NULL_ID )
    AMOS_THROW_ARGUMENT ("Cannot insert NULL_ID map value");

  HashNode_t * curr = hashfunc (key);

  if ( curr -> key == NULL_ID )
    {
      curr -> key = key;
      curr -> val = val;
    }
  else
    {
      while ( curr -> next != NULL )
	{
	  if ( curr -> key == key )
	    AMOS_THROW_ARGUMENT ("Cannot insert multiple map keys");
	  curr = curr -> next;
	}
      if ( curr -> key == key )
	AMOS_THROW_ARGUMENT ("Cannot insert multiple map keys");
  
      curr -> next = new HashNode_t( );
      curr = curr -> next;
      curr -> key = key;
      curr -> val = val;
    }

  size_m ++;
}


//----------------------------------------------------- invert -----------------
void IDMap_t::invert ( )
{
  HashNode_t * oldtable = table_m;
  table_m = new HashNode_t [buckets_m];
  HashNode_t * oldcurr, * invcurr;

  try {

  for ( Size_t i = 0; i < buckets_m; i ++ )
    {
      oldcurr = oldtable + i;
      if ( oldcurr -> key == NULL_ID )
	continue;
	
      do
	{
	  invcurr = hashfunc (oldcurr -> val);
	    
	  if ( invcurr -> key == NULL_ID )
	    {
	      invcurr -> key = oldcurr -> val;
	      invcurr -> val = oldcurr -> key;
	    }
	  else
	    {
	      while ( invcurr -> next != NULL )
		{
		  if ( invcurr -> key == oldcurr -> val )
		    AMOS_THROW_ARGUMENT
		      ("Cannot invert map with duplicate values");
		  invcurr = invcurr -> next;
		}
	      if ( invcurr -> key == oldcurr -> val )
		AMOS_THROW_ARGUMENT
		  ("Cannot invert map with duplicate values");	
		
	      invcurr -> next = new HashNode_t( );
	      invcurr = invcurr -> next;
	      invcurr -> key = oldcurr -> val;
	      invcurr -> val = oldcurr -> key;
	    }
	    
	  oldcurr = oldcurr -> next;
	} while (oldcurr != NULL);
    }
  }
  catch (Exception_t) {

    delete[] table_m;
    table_m = oldtable;
    throw;
  }

  delete[] oldtable;
}


//----------------------------------------------------- lookup -----------------
ID_t IDMap_t::lookup (ID_t key)
{
  if ( key == NULL_ID )
    return NULL_ID;

  HashNode_t * curr = hashfunc (key);

  while ( curr -> key != key )
    {
      if ( curr -> next == NULL )
	AMOS_THROW_ARGUMENT ("Could not find map key");
      curr = curr -> next;
    }

  return curr -> val;
}


//----------------------------------------------------- operator= --------------
IDMap_t & IDMap_t::operator= (const IDMap_t & source)
{
  if ( this != &source )
    {
      HashNode_t * tcurr;
      const HashNode_t * scurr;

      clear( );

      for ( Size_t i = 0; i < buckets_m; i ++ )
	{
	  tcurr = table_m + i;
	  scurr = source . table_m + i;

	  *tcurr = *scurr;
	  while ( tcurr -> next != NULL )
	    {
	      tcurr -> next = new HashNode_t( );
	      tcurr = tcurr -> next;
	      scurr = scurr -> next;
	      *tcurr = *scurr;
	    }
	}

      size_m = source . size_m;
    }

  return *this;
}


//----------------------------------------------------- remove -----------------
void IDMap_t::remove (ID_t key)
{
  if ( key == NULL_ID )
    return;

  HashNode_t * prev = NULL;
  HashNode_t * curr = hashfunc (key);

  while ( curr -> key != key )
    {
      if ( curr -> next == NULL )
	AMOS_THROW_ARGUMENT ("Could not find map key for removal");
      prev = curr;
      curr = curr -> next;
    }

  if ( prev == NULL )
    {
      if ( curr -> next == NULL )
	curr -> clear( );
      else
	*curr = *(curr -> next);
    }
  else
    {
      prev -> next = curr -> next;
      curr -> next = NULL;
      delete curr;
    }

  size_m --;
}


//----------------------------------------------------- read -------------------
void IDMap_t::read (istream & in)
{
  ID_t key, val, size;

  clear( );

  in . read ((char *)&size, sizeof (ID_t));

  for ( ID_t i = 0; i < size; i ++ )
    {
      in . read ((char *)&key, sizeof (ID_t));
      in . read ((char *)&val, sizeof (ID_t));
      insert (key, val);
    }
}


//----------------------------------------------------- readMessage ------------
void IDMap_t::readMessage (const Message_t & msg)
{
  clear( );

  try {
    ID_t key, val, size;
    stringstream ss;

    if ( msg . exists (F_MAP) )
      {
	ss . str (msg . getField (F_MAP));

	while ( ss )
	  {
	    ss >> key;
	    ss . ignore( );
	    ss >> val;
	    if ( ! ss . fail( ) )
	      insert (key, val);
	  }

	if ( ! ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid map format");
	ss . clear( );
      }

    if ( msg . exists (F_SIZE) )
      {
	ss . str (msg . getField (F_SIZE));
	ss >> size;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid size format");

	if ( size != size_m )
	  AMOS_THROW_ARGUMENT ("map and size fields do not agree");
      }
  }
  catch (ArgumentException_t) {

    clear( );
    throw;
  }
}


//----------------------------------------------------- write ------------------
void IDMap_t::write (ostream & out) const
{
  const HashNode_t * curr;

  out . write ((char *)&size_m, sizeof (ID_t));

  for ( Size_t i = 0; i < buckets_m; i ++ )
    if ( table_m [i] . key != NULL_ID )
      {
	curr = table_m + i;
	while ( curr != NULL )
	  {
	    out . write ((char *)&(curr -> key), sizeof (ID_t));
	    out . write ((char *)&(curr -> val), sizeof (ID_t));
	    curr = curr -> next;
	  }
      }
}


//----------------------------------------------------- writeMessage -----------
void IDMap_t::writeMessage (Message_t & msg) const
{
  msg . clear( );

  try {
    stringstream ss;

    msg . setMessageCode (NCode( ));

    const HashNode_t * curr;

    if ( size_m != 0 )
      {
	ss << size_m;
	msg . setField (F_SIZE, ss . str( ));
	ss . str("");

	for ( Size_t i = 0; i < buckets_m; i ++ )
	  if ( table_m [i] . key != NULL_ID )
	    {
	      curr = table_m + i;
	      while ( curr != NULL )
		{
		  ss << curr -> key << ',' << curr -> val << endl;
		  curr = curr -> next;
		}
	    }
	msg . setField (F_MAP, ss . str( ));
	ss . str("");
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}
