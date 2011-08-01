////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Michael Schatz
//! \date 2011.07.29
//!
//! \brief Compute the ce stat along an assembly
//!
////////////////////////////////////////////////////////////////////////////////

#include "foundation_AMOS.hh"
#include <cmath>
#include <iostream>
#include <cassert>
#include <sstream>
#include <unistd.h>
#include <map>
#include <iomanip>
#include "Feature_AMOS.hh"

#include "DataStore.hh"
#include "Insert.hh"
#include "CoverageStats.hh"

using namespace std;
using namespace AMOS;

typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

//=============================================================== Globals ====//
string    OPT_BankName;                 // bank name parameter
bool      OPT_BankSpy = false;          // read or read-only spy
double    OPT_Features = 0.0;
bool      OPT_IIDs = false;
bool      OPT_BAM = false;
string    CMD_SAMTOOLS = "samtools";


DataStore * m_datastore;
int m_connectMates = 1;
typedef std::vector<Insert *> InsertList_t;
InsertList_t m_inserts;


int m_coveragePlot = 0;
int m_cestats = 1;

void printCEStats(const std::string &id)
{
  typedef map<ID_t, CoverageStats> LibStats;
  LibStats::iterator li;

  LibStats libStats = m_datastore->computeCEStats(m_inserts);

  if ( OPT_Features ) 
    {
      double b = 0;
      double e = 0;
      double sign;

      for (li = libStats.begin(); li != libStats.end(); li++)
        {
          // This are stored as oldvalue newvalue so skip every other one
          for (int i = 1; i < li->second.m_curpos; i+=2)
            {
              if ( b )
                if ( li->second.m_cestat[i] * sign < 0 ||
                     fabs(li->second.m_cestat[i]) < OPT_Features )
                  {
                    cout << id << " " << Feature_t::MATEPAIR << " " << b << " "
                         << li->second.m_coverage[i].x() << " ";
                    if  ( sign < 0 )
                      cout << "CE_COMPRESS ";
                    else
                      cout << "CE_STRETCH ";
                    cout << "LIB=" << li->first << endl;
                    b = e = 0;
                  }

              if ( fabs(li->second.m_cestat[i]) >= OPT_Features )
                {
                  if ( b )
                    e = li->second.m_coverage[i].x();
                  else
                    {
                      b = e = li->second.m_coverage[i].x();
                      sign = li->second.m_cestat[i];
                    }
                }
            }

          if ( b && e )
            {
              cout << id << " " << Feature_t::MATEPAIR << " " << b << " " << e << " ";
              if  ( sign < 0 )
                cout << "CE_COMPRESS ";
              else
                cout << "CE_STRETCH ";
              cout << "LIB=" << li->first << endl;
              b = e = 0;
            }
        }
    }
  else
    {
      for (li = libStats.begin(); li != libStats.end(); li++)
        {
          cout << ">" << id  << " lib:" << li->first << endl;
          // This are stored as oldvalue newvalue so skip every other one
          for (int i = 1; i < li->second.m_curpos; i+=2)
            {
              cout << setprecision(10) << li->second.m_coverage[i].x() << " "
                   << setprecision(6)  << li->second.m_cestat[i] << endl;
            }
        }
    }

  vector<Insert *>::iterator i;
              
  for (i =  m_inserts.begin();
               i != m_inserts.end();
               i++)
          {
            delete (*i);
          }
              
          m_inserts.clear();
}


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

    if (OPT_BAM)
    {
      cerr << "processing bam: " << OPT_BankName << endl;


      string cmd = CMD_SAMTOOLS;
      cmd += " view -h ";
      cmd += OPT_BankName;

      cerr << "cmd: " << cmd << endl;

      FILE * bam = popen(cmd.c_str(), "r");

      if (!bam)
      {
        cerr << "ERROR: Couldn't exec: " << cmd << endl;
        exit(1);
      }

      char buffer[10*1024];

      while (fgets(buffer, sizeof(buffer), bam))
      {
        fprintf(stdout, "%s", buffer);
      }

      pclose(bam);
    }
    else
    {
      m_datastore = new DataStore();
      m_datastore->openBank(OPT_BankName);

      if ( m_datastore->scaffold_bank.isOpen() )
      {
        cerr << "Processing scaffolds... ";
        int scaffcount = 0;
        m_datastore->scaffold_bank.seekg(1);
        while (m_datastore->scaffold_bank >> scaff)
        {
          scaffcount++;
          vector <Tile_t> rtiling;
              
          m_datastore->mapReadsToScaffold(scaff, rtiling, 1);
          m_datastore->calculateInserts(rtiling, m_inserts, m_connectMates, 1);
              
          if ( OPT_IIDs )
          {
            stringstream oss;
            oss << scaff.getIID();
            printCEStats(oss.str());
          }
          else
            printCEStats(scaff.getEID());
              
        }
      }
      else if ( m_datastore->contig_bank.isOpen() )
      {
        cerr << "Processing contigs... ";
        int contigcount = 0;
        m_datastore->contig_bank.seekg(1);
        while (m_datastore->contig_bank >> contig)
        {
          contigcount++;

          m_datastore->calculateInserts(contig.getReadTiling(), m_inserts, m_connectMates, 1);            

          if ( OPT_IIDs )
          {
            stringstream oss;
            oss << contig.getIID();
            printCEStats(oss.str());
          }
          else
            printCEStats(contig.getEID());
        }
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

  while ( !errflg && ((ch = getopt (argc, argv, "hif:svB")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'B':
        OPT_BAM = true;
        break;

      case 'i':
        OPT_IIDs = true;
        break;

      case 'f':
        OPT_Features = atof(optarg);
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
    << "-h        Display help information\n"
    << "-B        The input is a BAM file, not an AMOS bank\n"
    << "-i        Dump scaffold/contig IIDs instead of EIDs\n"
    << "-f float  Only output CE features outside float deviations\n"
    << "-s        Disregard bank locks and write permissions (spy mode)\n"
    << "-v        Display the compatible bank version\n"
    << endl;
  cerr
    << "Print the compression-expansion (CE) statistic value at the beginning and end \n"
    << "of each insert across each scaffold separated by library. If no scaffold data \n"
    << "available, use contigs.\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
