#include "foundation_AMOS.hh"
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

  msgfile . open (argv[1], fstream::in);
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
    Kmer_t kmr;
    Library_t lib;
    Matepair_t mtp;
    Overlap_t ovl;
    Scaffold_t scf;
    Sequence_t seq;
    Contig_t ctg;
    Read_t red;

    Universal_t * unvp = NULL;
    
    while ( msg . read (msgfile) )
      {
	cerr << "# Found new message: "
	     << Decode (msg . getMessageCode( )) << endl;
	if ( msg . getMessageCode( ) == M_UNIVERSAL )
	  unvp = &unv;
	else if ( msg . getMessageCode( ) == M_CONTIGLINK )
	  unvp = &ctl;
	else if ( msg . getMessageCode( ) == M_CONTIGEDGE )
	  unvp = &cte;
	else if ( msg . getMessageCode( ) == M_FRAGMENT )
	  unvp = &frg;
	else if ( msg . getMessageCode( ) == M_KMER )
	  unvp = &kmr;
	else if ( msg . getMessageCode( ) == M_LIBRARY )
	  unvp = &lib;
	else if ( msg . getMessageCode( ) == M_MATEPAIR )
	  unvp = &mtp;
	else if ( msg . getMessageCode( ) == M_OVERLAP )
	  unvp = &ovl;
	else if ( msg . getMessageCode( ) == M_SCAFFOLD )
	  unvp = &scf;
	else if ( msg . getMessageCode( ) == M_SEQUENCE )
	  unvp = &seq;
	else if ( msg . getMessageCode( ) == M_CONTIG )
	  unvp = &ctg;
	else if ( msg . getMessageCode( ) == M_READ )
	  unvp = &red;
	else
	  {
	    cerr << "# don't know how to parse message\n";
	    continue;
	  }

	unvp -> clear( );
	unvp -> readMessage (msg);

	msg . clear( );
	unvp -> writeMessage (msg);

	msg . write (cout);

	cerr << "# successfully parsed message\n";
      }
  }
  catch (Exception_t & e) {
    cerr << e;
  }

  msgfile . close( );

  return 0;
}
