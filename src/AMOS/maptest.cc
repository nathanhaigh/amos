#include <IDMap_AMOS.hh>
#include <Message_AMOS.hh>
#include <iostream>
#include <cstdlib>
using namespace std;
using namespace AMOS;

int main ( )
{
  try {

    IDMap_t idmap1, idmap2;
    Message_t msg;

    msg . read (cin);
    idmap1 . readMessage (msg);

    idmap2 = idmap1;

    idmap2 . writeMessage (msg);
    msg . write (cout);
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  return 0;
}
