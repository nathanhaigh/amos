#include "foundation_AMOS.hh"
#include <iostream>
using namespace std;
using namespace AMOS;

int main ( )
{
  Contig_t ctg;

  try {

    while ( ctg . readUMD (cin) )
      ctg . writeUMD (cout);

  }
  catch (Exception_t & e) {
    cerr << e;
  }

  return 0;
}
