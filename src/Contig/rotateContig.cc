#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

// Rotate a contig to a new offset

using namespace AMOS;
using namespace std;

string bank_name;

string contigeid;
ID_t   contigiid = 0;

Pos_t  gindex = 0;
Pos_t  uindex = 0;



void PrintUsage(const char * name)
{
  cerr << "Usage: " << name << " [-g gindex | -u uindex] [-E eid | -I iid] bankname" << endl;
}

void PrintHelp(const char * name)
{
  PrintUsage(name);
}

void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hvE:I:g:u:")) != EOF) )
  {
    switch (ch)
    {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;

      case 'E': contigeid = optarg;  break;
      case 'I': contigiid = atoi(optarg); break;
      case 'g': gindex    = atoi(optarg); break;
      case 'u': uindex    = atoi(optarg); break;

      default: errflg ++; 
    };
  }


  if (errflg || optind != argc - 1)
  {
    PrintUsage (argv[0]);
    cerr << "Try '" << argv[0] << " -h' for more information.\n";
    exit (EXIT_FAILURE);
  }

  bank_name = argv[optind++];

  if ((contigeid.empty() && contigiid == NULL_ID) || 
     (!contigeid.empty() && contigiid != NULL_ID))
  {
    cerr << "Must specify one contig id" << endl;
    exit (EXIT_FAILURE);
  }

  if ((gindex == 0 && uindex == 0) || (gindex && uindex))
  {
    cerr << "Must specify one rotation coordinate" << endl;
    exit (EXIT_FAILURE);
  }

  cerr << "Rotating contig eid: " << contigeid << " iid: " << contigiid
       << " to gindex: " << gindex << " uindex: " << uindex
       << " in " << bank_name << endl;

}



int main (int argc, char ** argv)
{
  Bank_t contig_bank(Contig_t::NCODE);

  ParseArgs(argc, argv);

  Contig_t contig;

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    contig_bank.open(bank_name, B_READ|B_WRITE);

    if (!contigeid.empty()) { contig_bank.fetch(contigeid, contig); }
    else                    { contig_bank.fetch(contigiid, contig); }

    string cons = contig.getSeqString();
    string cqual = contig.getQualString();

    if (uindex) { gindex = contig.gap2ungap(uindex); }

    if (gindex < 0 || gindex >= cons.length())
    {
      cerr << "Specified offset " << gindex << " is outside of valid range" << endl;
      contig_bank.close();
      AMOS_THROW("Offset out of range");
    }

    // adjust consensus
    string left  = cons.substr(0, gindex);
    string lqual = cqual.substr(0, gindex);

    cons  = cons.substr(gindex, cons.length() - gindex);
    cqual = cqual.substr(gindex, cqual.length() - gindex);

    cons += left;
    cqual += lqual;

    // shift reads
    vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::iterator ti;
    for (ti = tiling.begin(); ti != tiling.end(); ti++)
    {
      ti->offset -= gindex;
      while (ti->offset < 0)
      {
        ti->offset += cons.length();
      }

      while (ti->offset + ti->getGappedLength() - 1 >= cons.length())
      {
        ti->offset -= cons.length();
      }
    }

    contig.setSequence(cons, cqual);

    contig_bank.replace(contig.getIID(), contig);
    contig_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}
