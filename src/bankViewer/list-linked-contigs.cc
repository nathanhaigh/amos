////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Michael Schatz
//! \date 1/25/06
//!
//! \brief Finds unplaced contigs that link to a single other contig
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <map>
#include <cmath>

#include "DataStore.hh"
#include "Insert.hh"
#include "InsertStats.hh"
#include <stdio.h>

using namespace std;
using namespace AMOS;

typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

int FRAGMENTSTRLEN = 3;
int m_verbose = 1;

DataStore * m_datastore;
int m_connectMates = 1;



//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter
bool   OPT_BankSpy = false;          // read or read-only spy


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
  Read_t red;
  Fragment_t frg;
  Library_t lib;


  Scaffold_t scaff;
  Contig_t contig;

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    m_datastore = new DataStore();
    m_datastore->openBank(OPT_BankName);

    map<ID_t, InsertStats *> librarystats;
    map<ID_t, InsertStats *>::iterator li;

    m_datastore->contig_bank.seekg(1);

    int contigcount = 0;

    Contig_t contig;
    m_datastore->contig_bank.seekg(1);
    while (m_datastore->contig_bank >> contig)
    {
      if (contig.getStatus() == 'U')
      {
        contigcount++;
        vector<Insert *> inserts;
        vector<Insert *>::iterator vi;
        m_datastore->calculateInserts(contig.getReadTiling(), inserts, 1, 0);

        map<ID_t, int> linkedcontigs;
        map<ID_t, int>::iterator li;

        for (vi = inserts.begin(); vi != inserts.end(); vi++)
        {
          if ((*vi)->m_state == Insert::LinkingMate)
          {
            li = linkedcontigs.find((*vi)->m_bcontig);

            if (li == linkedcontigs.end())
            {
              li = linkedcontigs.insert(make_pair((*vi)->m_bcontig, 0)).first;
            }

            li->second++;
          }

          delete *vi;
        }

        cout << contig.getIID();

        for (li = linkedcontigs.begin(); li != linkedcontigs.end(); li++)
        {
          cout << "\t" << li->first << "\t[" << li->second << "]";
        }

        if (linkedcontigs.size() == 1)
        {
          cout << "*";
        }

        cout << endl;
      }
    }

    cout << "contigs: " << contigcount   << endl;
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsvf:")) != EOF) )
    switch (ch)
      {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;

      case 's': OPT_BankSpy = true; break;

      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;

      case 'f': FRAGMENTSTRLEN = atoi(optarg); break;


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
    << "-f len        Number of characters of seqname to use as sublibrary (default=3)\n"
    << endl;
  cerr
    << "Finds all reads that should overlap a given contig range. Includes reads that\n"
    << "should be present by the virtue of their mate and the scaffold\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
