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

    IDMap_t idmap;
    Message_t msg;
    msg . read (cin);
    idmap . readMessage (msg);

    idmap . invert( );

    msg . clear( );
    idmap . writeMessage (msg);
    msg . write (cout);
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  return 0;
}
