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
    ContigLink_t ctl;
    ContigEdge_t cte;
    Fragment_t frg;

    Universal_t * unvp = NULL;
    
    while ( msg . read (msgfile) )
      {
	cout << "# Found new message: "
	     << Decode (msg . getMessageCode( )) << endl;
	if ( msg . getMessageCode( ) == Message_k::M_UNIVERSAL )
	  unvp = &unv;
	else if ( msg . getMessageCode( ) == Message_k::M_CONTIGLINK )
	  unvp = &ctl;
	else if ( msg . getMessageCode( ) == Message_k::M_CONTIGEDGE )
	  unvp = &cte;
	else if ( msg . getMessageCode( ) == Message_k::M_FRAGMENT )
	  unvp = &frg;
	else
	  {
	    cout << "# don't know how to parse message\n";
	    continue;
	  }

	unvp -> clear( );
	unvp -> readMessage (msg);

	msg . clear( );
	unvp -> writeMessage (msg);

	msg . write (cout);
      }
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  msgfile . close( );

  return 0;
}
