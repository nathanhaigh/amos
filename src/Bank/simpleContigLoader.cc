#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
using namespace std;
using namespace AMOS;


//=============================================================== Globals ====//
string OPT_BankName; 
string OPT_ContigFile;




//========================================================== Fuction Decs ====//
//----------------------------------------------------- ParseArgs --------------
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
void ParseArgs (int argc, char ** argv);


//----------------------------------------------------- PrintHelp --------------
//! \brief Prints help information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintHelp (const char * s);


//----------------------------------------------------- PrintUsage -------------
//! \brief Prints usage information to cerr
//!
//! \param s The program name, i.e. argv[0]
//! \return void
//!
void PrintUsage (const char * s);



void loadContig(Contig_t & contig, Bank_t & bank)
{
  bank.append(contig);
  cout << "Loaded contig " << contig.getEID() << ": "
       << contig.getLength() << "bp " 
       << contig.getReadTiling().size() << "reads" << endl;
}



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;

  Bank_t read_bank (Read_t::NCODE);
  Bank_t contig_bank(Contig_t::NCODE);

  long int cntw = 0;             // written object count

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    ifstream cfile;
    cfile.open(OPT_ContigFile.c_str());

    if (!cfile)
    {
      AMOS_THROW_IO("Can't open " + OPT_ContigFile);
    }

    read_bank.open (OPT_BankName, B_READ|B_SPY);

    if (contig_bank.exists(OPT_BankName))
    {
      contig_bank.open(OPT_BankName, B_READ|B_WRITE);
      contig_bank.clear();
    }
    else
    {
      contig_bank.create(OPT_BankName, B_READ|B_WRITE);
    }

    Contig_t ctg;
    bool first = true;
    string id;
    cfile >> id;

    while (cfile)
    {
      if (id[0] == '>')
      {
        if (!first)
        {
          loadContig(ctg, contig_bank);
          ctg.clear();
        }

        first = false;
        ctg.setEID(id.substr(1,id.length()));
        ctg.setIID(contig_bank.getMaxIID()+1);

        string cons;
        cfile >> cons;

        string cqual;
        cqual.append(cons.length(), 'X');

        ctg.setSequence(cons,cqual);

        cerr << "Saw contig header: " << id << endl;
      }
      else if (id[0] == '#')
      {
        ID_t readiid;
        int offset;
        char dir;

        readiid = atoi(id.c_str()+1);
        cfile >> offset;
        cfile >> dir;

        Read_t read;
        read_bank.fetch(readiid, read);

        Range_t clr = read.getClearRange();

             if (dir == 'F') { } 
        else if (dir == 'R') { clr.swap(); }
        else 
        {
          AMOS_THROW_IO((string)"Invalid Direction Specified " + dir + " for read " + id);
        }

        Tile_t tle;
        tle.source = readiid;
        tle.offset = offset;
        tle.range = clr;

        ctg.getReadTiling().push_back(tle);
        cerr << "Added read: " << id << " " << offset << " " << dir << endl;
      }
      else
      {
        AMOS_THROW_IO("Invalid field: " + id);
      }

      cfile >> id;
    }

    loadContig(ctg, contig_bank);
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hv")) != EOF) )
    switch (ch)
      {

      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'v':
	PrintBankVersion (argv[0]);
	exit (EXIT_SUCCESS);
	break;

      default:
        errflg ++;
      }

  if ( errflg > 0 || optind != argc - 2 )
    {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
    }

  OPT_BankName = argv [optind ++];
  OPT_ContigFile = argv[optind++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << "-v            Display the compatible bank version\n"
    << endl;
  cerr
    << "Loads contigs from file into a bank\n"
    << "Format of contig file is:\n\n"
    << ">contig1 consensus\n"
    << "#readiid1 offset dir\n"
    << "#readiid2 offset dir\n"
    << "#readiid3 offset dir\n"
    << ">contig2 consensus\n"
    << "#readiid4 offset dir\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  <bank path> <contig file>\n\n";
  return;
}
