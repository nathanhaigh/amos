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
    AMOS_THROW_ARGUMENT ("Insert key equals NULL_ID");
  if ( val == NULL_ID )
    AMOS_THROW_ARGUMENT ("Insert val equals NULL_ID");

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
	    AMOS_THROW_ARGUMENT ("key already exists");
	  curr = curr -> next;
	}
      if ( curr -> key == key )
	AMOS_THROW_ARGUMENT ("key already exists");	
  
      curr -> next = new HashNode_t( );
      curr = curr -> next;
      curr -> key = key;
      curr -> val = val;
    }

  size_m ++;
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
	AMOS_THROW_ARGUMENT ("key does not exist");
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
      IBankable_t::operator= (source);

      for ( Size_t i = 0; i < BUCKETS; i ++ )
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
  HashNode_t * prev = NULL;
  HashNode_t * curr = hashfunc (key);

  while ( curr -> key != key )
    {
      if ( curr -> next == NULL )
	AMOS_THROW_ARGUMENT ("key does not exist");
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
	  AMOS_THROW_ARGUMENT ("Invalid sze format");

	if ( size != size_m )
	  AMOS_THROW_ARGUMENT ("sze value does not match number of entries");
      }
  }
  catch (ArgumentException_t) {

    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
Size_t IDMap_t::readRecord (istream & fix,
			    istream & var)
{
  ID_t key, val, size;

  clear( );

  fix . read ((char *)&size, sizeof (ID_t));

  for ( ID_t i = 0; i < size; i ++ )
    {
      var . read ((char *)&key, sizeof (ID_t));
      var . read ((char *)&val, sizeof (ID_t));
      insert (key, val);
    }

  return sizeof (ID_t) + size_m * sizeof (ID_t) * 2;
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

	for ( Size_t i = 0; i < BUCKETS; i ++ )
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


//----------------------------------------------------- writeRecord ------------
Size_t IDMap_t::writeRecord (ostream & fix,
			     ostream & var) const
{
  const HashNode_t * curr;

  fix . write ((char *)&size_m, sizeof (ID_t));

  for ( Size_t i = 0; i < BUCKETS; i ++ )
    if ( table_m [i] . key != NULL_ID )
      {
	curr = table_m + i;
	while ( curr != NULL )
	  {
	    var . write ((char *)&(curr -> key), sizeof (ID_t));
	    var . write ((char *)&(curr -> val), sizeof (ID_t));
	    curr = curr -> next;
	  }
      }

  return sizeof (ID_t) + size_m * sizeof (ID_t) * 2;
}
