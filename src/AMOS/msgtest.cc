#include <messages_AMOS.hh>
#include <fstream>
using namespace std;
using namespace AMOS;

int main (int argc, char ** argv)
{
  fstream msgfile;

  if ( argc < 2 )
    {
      cerr << "need a filename\n";
      exit (1);
    }

  msgfile . open (argv[1], ios::in);
  if ( !msgfile . good( ) )
    {
      cerr << "bad file\n";
      exit (1);
    }

  try {

    Message_t msg;
    
    while ( msg . read (msgfile) )
      {
	msg . write (cout);
      }

    msg . clear( );
    msg . write (cout);

  }
  catch (Exception_t & e) {
    cerr << e;
  }

  msgfile . close( );

  return 0;
}
