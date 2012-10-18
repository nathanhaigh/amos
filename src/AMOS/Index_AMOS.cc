////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/21/2005
//!
//! \brief Source for Index_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Index_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Index_t =====================
const NCode_t Index_t::NCODE = M_INDEX;


//----------------------------------------------------- buildContigFeature -----
void Index_t::buildContigFeature (const std::string bankname)
{
  clear( );
  setType (Contig_t::NCODE, Feature_t::NCODE);

  Feature_t fea;
  pair<NCode_t, ID_t> src;
  BankStream_t fea_bank (Feature_t::NCODE);

  fea_bank . open (bankname, B_READ);

  while ( fea_bank >> fea )
    {
      src = fea . getSource( );
      if ( src . first != NULL_ID  &&  src . second == Contig_t::NCODE )
        insert (src . first, fea . getIID( ));
    }

  fea_bank . close( );
}


//----------------------------------------------------- buildContigScaffold ----
void Index_t::buildContigScaffold (const std::string bankname)
{
  clear( );
  setType (Contig_t::NCODE, Scaffold_t::NCODE);

  Scaffold_t scf;
  vector<Tile_t>::const_iterator cur;
  vector<Tile_t>::const_iterator end;
  BankStream_t scf_bank (Scaffold_t::NCODE);

  scf_bank . open (bankname, B_READ);

  while ( scf_bank >> scf )
    {
      end = scf . getContigTiling( ) . end( );
      if ( scf . getIID( ) != NULL_ID )
        for ( cur = scf . getContigTiling( ) . begin( ); cur != end; ++ cur )
          if ( cur -> source != NULL_ID )
            insert (cur -> source, scf . getIID( ));
    }

  scf_bank . close( );
}


//----------------------------------------------------- buildReadContig --------
void Index_t::buildReadContig (const std::string bankname)
{
  clear( );
  setType (Read_t::NCODE, Contig_t::NCODE);

  Contig_t ctg;
  vector<Tile_t>::const_iterator cur;
  vector<Tile_t>::const_iterator end;
  BankStream_t ctg_bank (Contig_t::NCODE);

  ctg_bank . open (bankname, B_READ);

  while ( ctg_bank >> ctg )
    {
      end = ctg . getReadTiling( ) . end( );
      if ( ctg . getIID( ) != NULL_ID )
        for ( cur = ctg . getReadTiling( ) . begin( ); cur != end; ++ cur )
          if ( cur -> source != NULL_ID )
            insert (cur -> source, ctg . getIID( ));
    }

  ctg_bank . close( );
}


//----------------------------------------------------- buildReadLibrary -------
void Index_t::buildReadLibrary (const std::string bankname)
{
  clear( );
  setType (Read_t::NCODE, Library_t::NCODE);

  Read_t red;
  Fragment_t frg;
  Bank_t frg_bank (Fragment_t::NCODE);
  BankStream_t red_bank (Read_t::NCODE);

  frg_bank . open (bankname, B_READ);
  red_bank . open (bankname, B_READ);

  while ( red_bank >> red )
    {
      frg_bank . fetch (red . getFragment( ), frg);
      if ( red . getIID( ) != NULL_ID  &&  frg . getLibrary( ) != NULL_ID )
        insert (red . getIID( ), frg . getLibrary( ));
    }

  frg_bank . close( );
  red_bank . close( );
}


//----------------------------------------------------- buildReadMate ----------
void Index_t::buildReadMate (const std::string bankname)
{
  clear( );
  setType (Read_t::NCODE, Read_t::NCODE);

  Fragment_t frg;
  pair<ID_t, ID_t> mtp;
  BankStream_t frg_bank (Fragment_t::NCODE);

  frg_bank . open (bankname, B_READ);

  while ( frg_bank >> frg )
    {
      mtp = frg . getMatePair( );
      if ( mtp . first != NULL_ID  &&  mtp . second != NULL_ID )
        {
          insert (mtp . first, mtp . second);
          insert (mtp . second, mtp . first);
        }
    }

  frg_bank . close( );
}


//----------------------------------------------------- buildScaffoldFeature ---
void Index_t::buildScaffoldFeature (const std::string bankname)
{
  clear( );
  setType (Scaffold_t::NCODE, Feature_t::NCODE);

  Feature_t fea;
  pair<NCode_t, ID_t> src;
  BankStream_t fea_bank (Feature_t::NCODE);

  fea_bank . open (bankname, B_READ);

  while ( fea_bank >> fea )
    {
      src = fea . getSource( );
      if ( src . first != NULL_ID  &&  src . second == Scaffold_t::NCODE )
        insert (src . first, fea . getIID( ));
    }

  fea_bank . close( );
}


//----------------------------------------------------- readMessage ------------
void Index_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {
    Size_t size = -1;
    istringstream ss;

    if ( msg . exists (F_OBJECT) )
      {
        string str = msg . getField (F_OBJECT);

        if ( str . length( ) != NCODE_SIZE + NCODE_SIZE + 1 )
          AMOS_THROW_ARGUMENT ("Invalid object type format");

        type_m . first = Encode (str.substr (0, NCODE_SIZE)); 
        type_m . second = Encode (str.substr (NCODE_SIZE + 1));
      }

    if ( msg . exists (F_SIZE) )
      {
        ss . str (msg . getField (F_SIZE));
        ss >> size;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid size format");
        index_m . resize (size);

        ss . clear( );
      }

    if ( msg . exists (F_MAP) )
      {
        pair<ID_t, ID_t> iids;
        ss . str (msg . getField (F_MAP));

        while ( ss )
          {
            ss >> iids . first >> iids . second;
            if ( ! ss . fail( ) )
              insert (iids);
          }

        if ( ! ss . eof( ) )
          AMOS_THROW_ARGUMENT ("Invalid map format");

        if ( size >= 0  &&  size != getSize( ) )
          AMOS_THROW_ARGUMENT ("map and size fields do not agree");

        ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Index_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  Size_t size;
  readLE (fix, &size);
  readLE (fix, &(type_m . first));
  readLE (fix, &(type_m . second));

  pair<ID_t, ID_t> iids;
  index_m . clear( );
  index_m . resize (size);
  for ( Pos_t i = 0; i < size; ++ i )
    {
      readLE (var, &(iids . first));
      readLE (var, &(iids . second));
      insert (iids);
    }
}



//----------------------------------------------------- readRecordFix ----------
void Index_t::readRecordFix (istream & fix)
{
  Universal_t::readRecordFix (fix);

  Size_t size;
  readLE (fix, &size);
  readLE (fix, &(type_m . first));
  readLE (fix, &(type_m . second));

  index_m . clear( );
}


//----------------------------------------------------- writeMessage -----------
void Index_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    Size_t size = getSize( );
    ostringstream ss;

    msg . setMessageCode (Index_t::NCODE);

    if ( type_m . first != NULL_NCODE  ||  type_m . second != NULL_NCODE )
      {
        ss << Decode (type_m . first) << ',' << Decode (type_m . second);
        msg . setField (F_OBJECT, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( size != 0 )
      {
        ss << size;
        msg . setField (F_SIZE, ss . str( ));
        ss . str (NULL_STRING);

        string str;
        for ( const_iterator itr = begin( ); itr != end( ); ++ itr )
          {
            ss << itr -> first << '\t' << itr -> second;
            str . append (ss . str( ));
            str . push_back (NL_CHAR);
            ss . str (NULL_STRING);
          }

        msg . setField (F_MAP, str);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Index_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  Size_t size = getSize( );
  writeLE (fix, &size);
  writeLE (fix, &(type_m . first));
  writeLE (fix, &(type_m . second));

  for ( const_iterator itr = begin( ); itr != end( ); ++ itr )
    {
      writeLE (var, &(itr -> first));
      writeLE (var, &(itr -> second));
    }
}
