////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for Matepair_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Matepair_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Matepair_t ==================
const NCode_t Matepair_t::NCODE = M_MATEPAIR;
const MateType_t Matepair_t::NULL_MATE  = 0;
const MateType_t Matepair_t::OTHER      = 'X';
const MateType_t Matepair_t::END        = 'E';
const MateType_t Matepair_t::TRANSPOSON = 'T';


//----------------------------------------------------- readMessage ------------
void Matepair_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    istringstream ss;

    if ( msg . exists (F_READ1) )
      {
	ss . str (msg . getField (F_READ1));
	ss >> reads_m . first;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid contig1 link format");
	ss . clear( );
      }

    if ( msg . exists (F_READ2) )
      {
	ss . str (msg . getField (F_READ2));
	ss >> reads_m . second;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid contig2 link format");
	ss . clear( );
      }

    if ( msg . exists (F_TYPE) )
      {
	ss . str (msg . getField (F_TYPE));
	setType (ss . get( ));
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Matepair_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  readLE (fix, &(reads_m . first));
  readLE (fix, &(reads_m . second));
  type_m = fix . get( );
}


//----------------------------------------------------- setType ----------------
void Matepair_t::setType (MateType_t type)
{
  switch (type)
    {
    case NULL_MATE:
    case OTHER:
    case END:
    case TRANSPOSON:
      type_m = type;
      break;
    default:
      AMOS_THROW_ARGUMENT ((string)"Invalid mate type " + type);
    }
  type_m = type;
}


//----------------------------------------------------- writeMessage -----------
void Matepair_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (Matepair_t::NCODE);

    if ( reads_m . first != NULL_ID )
      {
	ss << reads_m . first;
	msg . setField (F_READ1, ss . str( ));
	ss . str (NULL_STRING);
      }

    if ( reads_m . second != NULL_ID )
      {
	ss << reads_m . second;
	msg . setField (F_READ2, ss . str( ));
	ss . str (NULL_STRING);
      }

    if ( type_m != NULL_MATE )
      {
	ss << type_m;
	msg . setField (F_TYPE, ss . str( ));
	ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Matepair_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  writeLE (fix, &(reads_m . first));
  writeLE (fix, &(reads_m . second));
  fix . put (type_m);
}
