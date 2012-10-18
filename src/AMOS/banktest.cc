#include "foundation_AMOS.hh"
#include "amp.hh"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iostream>
using namespace std;
using namespace AMOS;

const string BANK_STORE_DIR = "_bank_";
const string STREAM_STORE_DIR = "_stream_";

int main (int argc, char ** argv)
{
  srand (time(NULL));

  try {

    ID_t N, i , j, step;
    Bank_t readbank (Read_t::NCODE);
    BankStream_t readstream (Read_t::NCODE);
    Read_t read;
    ostringstream ss;

    if ( argc != 2 )
      {
	cerr << "USAGE: " << argv[0] << " #reads\n";
	return -1;
      }

    N = atol (argv[1]);
    if ( N < 50 )
      step = 1;
    else
      step = N / 50;

    read . setClearRange (Range_t(0,1080));
    read . setQualityClearRange (Range_t(1,1301));
    read . setVectorClearRange (Range_t(2,1032));
    read . setFragment (29373);
    read . setType (Read_t::END);
    read . compress( );
    read . setFlagA (false);
    read . setFlagB (true);
    read . setSequence (
			"acgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgacgtagctagctagttttcgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtaaaaaatcgatcgtaaaagtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacgtacg",
			"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678443573456345523453444442345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789077757856846845623523454377777712345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012");



    readbank . create (BANK_STORE_DIR);
    readstream . create (STREAM_STORE_DIR);


    cerr << "APPEND " << N << " reads\n" << Date( ) << endl << "begin";
    for ( i = 1; i <= N; i ++ )
      {
 	ss . str (NULL_STRING);
 	ss << 'a' << i;
 	read . setIID (i);
 	read . setEID (ss . str( ));
 	read . setComment (ss . str( ) + 'c');
 	readbank . append (read);

 	if ( i % step == 0 )
 	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl << endl;


    cerr << "SAPPEND " << N << " reads\n" << Date( ) << endl << "begin";
    for ( i = 1; i <= N; i ++ )
      {
 	ss . str (NULL_STRING);
 	ss << 'a' << i;
	read . setIID (i);
 	read . setEID (ss . str( ));
 	read . setComment (ss . str( ) + 'c');
	readstream << read;

	if ( i % step == 0 )
	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl << endl;


    cerr << "FETCH " << N
 	 << " random reads\n" << Date( ) << endl << "begin";
    for ( i = 1; i <= N; i ++ )
      {
 	j = 1 + rand( ) % N;
 	readbank . fetch (j, read);

 	if ( i % step == 0 )
 	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl << endl;


    cerr << "SFETCH " << N
	 << " consecutive reads\n" << Date( ) << endl << "begin";
    for ( i = 1; readstream >> read; i ++ )
      {
	if ( i % step == 0 )
	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl << endl;


    cerr << "REPLACE " << N
 	 << " random reads\n" << Date( ) << endl << "begin";
    for ( i = 1; i <= N; i ++ )
      {
 	j = 1 + rand( ) % N;

 	ss . str (NULL_STRING);
 	ss << 'r' << j;
 	read . setIID (j);
 	read . setEID (ss . str( ));
 	read . setComment (ss . str( ) + 'c');
 	readbank . replace (j, read);

 	if ( i % step == 0 )
 	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl << endl;


    cerr << "REMOVE " << N
 	 << " random reads\n" << Date( ) << endl << "begin";
    for ( i = 1; i <= N; i ++ )
      {
 	j = 1 + rand( ) % N;

	if ( readbank . getIDMap( ) . exists (j) )
	  readbank . remove (j);

 	if ( i % step == 0 )
 	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl << endl;


    j = rand( ) % N;
    readstream . seekg (j, BankStream_t::BEGIN);
    if ( readstream >> read )
      {
	cerr << "SFETCH OFFSET " << j
	     << " yeilds IID:" << read . getIID( )
	     << " EID:" << read . getEID( )
	     << " COMMENT:" << read . getComment( ) << endl << endl;
      }


    do {
      j = 1 + rand( ) % N;
    } while ( ! readbank . getIDMap( ) . exists (j) );
    readbank . fetch (j, read);
    cerr << "FETCH IID " << j
	 << " bid:" << readbank . getIDMap() . lookupBID (j)
	 << " yields IID:" << read . getIID( )
 	 << " EID:" << read . getEID( )
 	 << " COMMENT:" << read . getComment( ) << endl << endl;


    readbank . close( );
    readstream . close( );


    readstream . open (BANK_STORE_DIR);
    cerr << "SFETCH " << readstream . getSize( )
	 << " consecutive reads\n" << Date( ) << endl << "begin";
    for ( i = 1; readstream >> read; i ++ )
      {
// 	cout << readstream . getIDMap() . lookupBID (read . getIID( ))
// 	     << ": " << read . getIID( )
// 	     << ' ' << read . getEID( )
// 	     << ' ' << read . getComment( ) << endl;

	if ( i % step == 0 )
	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl;
    cerr << i - 1 << " fetched" << endl << endl;
    readstream . close( );


    readbank . open (BANK_STORE_DIR);
    cerr << "CLEAN bank\n" << Date( ) << endl << "begin...";
    readbank . clean( );
    cerr << "done.\n" << Date( ) << endl << endl;


    readbank . fetch (j, read);
    cerr << "FETCH IID " << j
	 << " bid:" << readbank . getIDMap() . lookupBID (j)
	 << " yields IID:" << read . getIID( )
 	 << " EID:" << read . getEID( )
 	 << " COMMENT:" << read . getComment( ) << endl << endl;
    readbank . close( );


    readstream . open (BANK_STORE_DIR);
    cerr << "SFETCH " << readstream . getSize( )
	 << " consecutive reads\n" << Date( ) << endl << "begin";
    for ( i = 1; readstream >> read; i ++ )
      {
// 	cout << readstream . getIDMap() . lookupBID (read . getIID( ))
// 	     << ": " << read . getIID( )
// 	     << ' ' << read . getEID( )
// 	     << ' ' << read . getComment( ) << endl;

	if ( i % step == 0 )
	  cerr << '.';
      }
    cerr << "done.\n" << Date( ) << endl;
    cerr << i - 1 << " fetched" << endl << endl;
    readstream . close( );


    //    readbank . destroy( );
    //    readstream . destroy( );
  }
  catch (const Exception_t & e) {

    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return -1;
  }

  return 0;
}
