#include <universal_AMOS.hh>
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
    Universal_t unv;
    
    while ( msg . read (msgfile) )
      if ( msg . getMessageCode( ) == unv . getNCode( ) )
	{
	  unv . clear( );
	  unv . fromMessage (msg);

	  msg . clear( );
	  unv . toMessage (msg);

	  cout << "### NEW MESSAGE ###\n";
	  msg . write (cout);
	}
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  msgfile . close( );

  return 0;
}
