#include <Bank_AMOS.hh>
#include <IDMap_AMOS.hh>
#include <Universal_AMOS.hh>
#include <iostream>
using namespace std;
using namespace AMOS;

#define HSIZE 100

int main ( )
{
  try {

    Message_t msg;
    msg . read (cin);

    Bank_t bnk (Universal_t::NCode( ));
    bnk . create ("__testbank__");
    bnk . map( ) . readMessage (msg);
    bnk . close( );

    bnk . open ("__testbank__");
    msg . clear( );

    cerr << bnk . map( ) . lookup (0) << endl;
    cerr << bnk . map( ) . lookup (1) << endl;
    cerr << bnk . map( ) . lookup (10000) << endl;

    bnk . map( ) . writeMessage (msg);
    msg . write (cout);
    bnk . close( );
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  return 0;
}
