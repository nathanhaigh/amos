#include <Bank_AMOS.hh>
#include <IDMap_AMOS.hh>
#include <iostream>
using namespace std;
using namespace AMOS;

#define HSIZE 100

int main ( )
{
  //  char c;

  try {

    Message_t msg;
    IDMap_t idmap;

    msg . read (cin);
    idmap . readMessage (msg);

    //    for ( ID_t i = 1; i <= HSIZE; i ++ )
    //      idmap . insert (i, i + 10);

    msg . clear( );
    idmap . writeMessage (msg);
    msg . write (cout);
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  return 0;
}
