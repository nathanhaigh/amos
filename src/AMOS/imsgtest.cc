#include <messages_AMOS.hh>
#include <datatypes_AMOS.hh>
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
    Bankable_t bnk;
    
    while ( msg . read (msgfile) )
      if ( msg . getMessageType( ) == Message_k::M_BANKABLE )
	{
	  bnk . clear( );
	  bnk . fromMessage (msg);

	  msg . clear( );
	  bnk . toMessage (msg);
	  msg . write (cout);
	}
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  msgfile . close( );

  return 0;
}
