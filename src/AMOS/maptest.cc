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

    IDMap_t idmap1(10000);
    IDMap_t idmap2(1000);
    Message_t msg;
    msg . read (cin);
    idmap1 . readMessage (msg);

    idmap2 = idmap1;
    idmap2 . invert( );

    msg . clear( );
    idmap2 . writeMessage (msg);
    msg . write (cout);
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  return 0;
}
