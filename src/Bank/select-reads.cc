////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Mike Schatz
//! \date 03/14/2005
//!
//! \brief Dumps RED, FRG, MTP, LIB messages associated with a list of reads
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
string OPT_EIDList;
bool   OPT_AutoIncludeMates = false;



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

typedef map<AMOS::ID_t, char> idmap;



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;
  Read_t red1, red2;
  Fragment_t frg;
  Library_t lib;
  Matepair_t mtp;

  Bank_t red_bank (Read_t::NCODE);
  Bank_t frg_bank (Fragment_t::NCODE);
  BankStream_t lib_bank (Library_t::NCODE);
  BankStream_t mtp_bank (Matepair_t::NCODE);

  long int cntw = 0;             // written object count

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    BankMode_t bm = OPT_BankSpy ? B_SPY : B_READ;
    mtp_bank . open (OPT_BankName, bm);
    red_bank . open (OPT_BankName, bm);
    frg_bank . open (OPT_BankName, bm);
    lib_bank . open (OPT_BankName, bm);

    idmap iidsToPrint;

    if (!OPT_EIDList.empty())
    {
      ifstream eidfile;
      eidfile.open(OPT_EIDList.c_str());

      if (!eidfile)
      {
        throw Exception_t("Couldn't open EID File",
                          __LINE__, __FILE__);
      }

      string eid;
      while (eidfile >> eid)
      {
        ID_t iid = red_bank.lookupIID(eid.c_str());
        if (iid != AMOS::NULL_ID)
        {
          iidsToPrint[iid] = 0;
        }
        else
        {
          cerr << "ERROR: EID:" << eid << " not found in bank, skipping" << endl;
        }
      }
      cerr << "Loaded " << iidsToPrint.size() << " reads to select from " << OPT_EIDList << endl;
    }

    Message_t msg;

    //-- Iterate through each library in the bank
    while ( lib_bank >> lib )
      {
        lib.writeMessage(msg);
        msg.write(cout);
      }

    int extra;   

    //-- Iterate through each object in the mate bank
    while ( mtp_bank >> mtp )
      {
        idmap::iterator r1 = iidsToPrint.find(mtp.getReads().first);
        idmap::iterator r2 = iidsToPrint.find(mtp.getReads().second);

        if (OPT_EIDList.empty() || 
            r1 != iidsToPrint.end() || 
            r2 != iidsToPrint.end())
        {

          if (r1 == iidsToPrint.end())
          {
            mtp.flip();
            r1 = r2;
            r2 = iidsToPrint.end();
          }

          red_bank . fetch (mtp . getReads( ) . first, red1);
          frg_bank . fetch (red1 . getFragment( ), frg);

          frg.writeMessage(msg);
          msg.write(cout);

          if (OPT_EIDList.empty() || 
              r1 != iidsToPrint.end() || 
              OPT_AutoIncludeMates)
          {
            iidsToPrint[red1.getIID()] = 1;
            red1.writeMessage(msg);
            msg.write(cout);
            cntw++;

            if (r1 == iidsToPrint.end() && !OPT_EIDList.empty())
            {
              extra++;
            }
          }

          if (OPT_EIDList.empty() || 
              r2 != iidsToPrint.end() || 
              OPT_AutoIncludeMates)
          {
            red_bank . fetch (mtp . getReads( ) . second, red2);
            iidsToPrint[red2.getIID()] = 1;
            red2.writeMessage(msg);
            msg.write(cout);
            cntw++;

            if (r2 == iidsToPrint.end() && !OPT_EIDList.empty())
            {
              extra++;
            }
          }

          mtp.writeMessage(msg);
          msg.write(cout);
        }
      }

    if (OPT_AutoIncludeMates)
    {
      cerr << "Included " << extra << " extra mates" << endl;
    }

    if (!OPT_EIDList.empty())
    {
      // Check that all reads (including unmated) have been printed
      int unmated = 0;
      idmap::iterator ii;
      for (ii =  iidsToPrint.begin();
           ii != iidsToPrint.end();
           ii++)
      {
        if (ii->second == 0)
        {
          red_bank . fetch (ii->first, red1);
          frg_bank . fetch (red1 . getFragment( ), frg);

          frg.writeMessage(msg);
          msg.write(cout);

          red1.writeMessage(msg);
          msg.write(cout);

          unmated++;
          cntw++;
        }
      }

      cerr << "Included " << unmated << " unmated reads" << endl;
    }

    mtp_bank . close( );
    red_bank . close( );
    frg_bank . close( );
    lib_bank . close( );
  }
  catch (const Exception_t & e) {
    cerr << "FATAL: " << e . what( ) << endl
         << "  there has been a fatal error, abort" << endl;
    exitcode = EXIT_FAILURE;
  }
  //-- END: MAIN EXCEPTION CATCH


  cerr << "Total Reads written: " << cntw << endl;

  return exitcode;
}




//------------------------------------------------------------- ParseArgs ----//
void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;


  while ( !errflg && ((ch = getopt (argc, argv, "hsvEM")) != EOF) )
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

      case 'E':
        OPT_EIDList = argv[optind++];
        break;

      case 'M':
        OPT_AutoIncludeMates = true;
        break;

      default:
        errflg ++;
      }

  if ( errflg > 0 || optind != argc - 1 )
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
    << "-E EIDLIST    Specify file containing list of eid's to extract\n"
    << "-M            Dump mates of eids as well\n"
    << endl;
  cerr
    << "Takes an AMOS bank directory and dumps RED, FRG, MTP, & LIB messages\n"
    << "associated with list of reads to stdout" << endl;
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << " -E EIDLIST  [options]  <bank path>\n\n";
  return;
}
