////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/08/2004
//!
//! \brief Dumps a bambus .mates file from an AMOS bank
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
using namespace std;
using namespace AMOS;


//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter
bool   OPT_BankSpy = false;          // read or read-only spy
typedef map <ID_t, ID_t> IDMap;


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



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;

  map<ID_t, vector<ID_t> > readdups;
  map<ID_t, vector<ID_t> >::iterator rdi;

  Contig_t contig;

  BankStream_t ctg_bank (Contig_t::NCODE);
  BankStream_t red_bank (Read_t::NCODE);

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try 
  {

    BankMode_t bm = OPT_BankSpy ? B_SPY : B_READ;
    ctg_bank.open (OPT_BankName, bm);
    red_bank.open(OPT_BankName, bm);

    while (ctg_bank >> contig)
    {
      ID_t ciid = contig.getIID();
      vector<Tile_t> & rtiling = contig.getReadTiling();
      vector<Tile_t>::const_iterator ti;

      for (ti = rtiling.begin(); ti != rtiling.end(); ti++)
      {
        rdi = readdups.find(ti->source);
        if (rdi == readdups.end())
        {
          rdi = readdups.insert(make_pair(ti->source, vector<ID_t>())).first;
        }

        rdi->second.push_back(ciid);
      }
    }

    AMOS::IDMap_t::const_iterator ci;
    for (ci = red_bank.getIDMap().begin();
         ci;
         ci++)
    {
      cout << ci->iid << "\t" << ci->eid << "\t";

      rdi = readdups.find(ci->iid);

      if (rdi == readdups.end())
      {
        cout << "S" << endl;
      }
      else
      {
        if (rdi->second.size() == 1) { cout << "P"; }
        else                         { cout << "D"; }

        vector<ID_t>::iterator vi;
        for (vi = rdi->second.begin(); vi != rdi->second.end(); vi++)
        {
          cout << "\t" << *vi;
        }

        cout << endl;
      }
    }
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsvExy")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 's':
	OPT_BankSpy = true;
	break;

      case 'v':
	PrintBankVersion (argv[0]);
	exit (EXIT_SUCCESS);
	break;

      default:
        errflg ++;
      }

  if (errflg > 0 || optind != argc - 1)
  {
    PrintUsage (argv[0]);
    cerr << "Try '" << argv[0] << " -h' for more information.\n";
    exit (EXIT_FAILURE);
  }

  OPT_BankName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);
  cerr
    << "-h            Display help information\n"
    << "-s            Disregard bank locks and write permissions (spy mode)\n"
    << "-v            Display the compatible bank version\n"
    << endl;
  cerr
    << "Prints the status and containing contig(s) of each read in the read\n"
    << "bank as follows:\n"
    << "S: Singleton read\n"
    << "P: Placed in a single contig\n"
    << "D: Placed in multiple contigs\n"
    << endl;

  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
