#include "foundation_AMOS.hh"
#include <iostream>
#include <cassert>
#include <unistd.h>
using namespace std;
using namespace AMOS;


//=============================================================== Globals ====//
string OPT_BankName; 
bool   OPT_BankSpy    = false;

bool   OPT_Singletons = false;
bool   OPT_Dups       = false;
bool   OPT_Placed     = false;

bool   OPT_JustEID    = false;
bool   OPT_JustIID    = false;


void ParseArgs (int argc, char ** argv);
void PrintHelp (const char * s);
void PrintUsage (const char * s);


int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;

  HASHMAP::hash_map<ID_t, vector<ID_t> > readdups;
  HASHMAP::hash_map<ID_t, vector<ID_t> >::iterator rdi;

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

      if ( rtiling.size() > 1 ) // only count non-singleton contigs
        for (ti = rtiling.begin(); ti != rtiling.end(); ti++)
          {
            rdi = readdups.find(ti->source);
            if (rdi == readdups.end())
              {
                rdi = readdups.insert
                  (make_pair(ti->source, vector<ID_t>())).first;
              }
            
            rdi->second.push_back(ciid);
          }
    }

    AMOS::IDMap_t::const_iterator ri;
    for (ri = red_bank.getIDMap().begin();
         ri;
         ri++)
    {
      int numcontigs = 0;
      char code = 'S';
      rdi = readdups.find(ri->iid);

      if (rdi == readdups.end())
      {
        if (OPT_Singletons)
        {
          if      (OPT_JustEID) { cout << ri->eid << endl; }
          else if (OPT_JustIID) { cout << ri->iid << endl; }
          else
          {
            cout << ri->iid << "\t" 
                 << ri->eid << "\t"
                 << code << "\t"
                 << numcontigs
                 << endl;
          }
        }
      }
      else
      {
        numcontigs = rdi->second.size();

        if (numcontigs == 1) { code =  'P'; }
        else                 { code =  'D'; }

        if ((OPT_Placed && numcontigs == 1) ||
            (OPT_Dups   && numcontigs >  1))
        {
          if      (OPT_JustEID) { cout << ri->eid << endl; }
          else if (OPT_JustIID) { cout << ri->iid << endl; }
          else
          {
            cout << ri->iid << "\t" 
                 << ri->eid << "\t"
                 << code << "\t"
                 << numcontigs;

            vector<ID_t>::iterator vi;
            for (vi = rdi->second.begin(); vi != rdi->second.end(); vi++)
            {
              cout << "\t" << *vi;
            }

            cout << endl;
          }
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




void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hsvSDPEI")) != EOF) )
    switch (ch)
      {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;
      case 's': OPT_BankSpy = true; break;

      case 'S': OPT_Singletons = true; break;
      case 'D': OPT_Dups       = true; break;
      case 'P': OPT_Placed     = true; break;

      case 'E': OPT_JustEID    = true; break;
      case 'I': OPT_JustIID    = true; break;

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

  if (!OPT_Singletons && !OPT_Dups && !OPT_Placed)
  {
    OPT_Singletons = OPT_Dups = OPT_Placed = true;
  }
}




void PrintHelp (const char * s)
{
  PrintUsage (s);
   
   cerr << "\n.DESCRIPTION.\n"
        << "  Prints the status and containing contig(s) of each read as:\n"
        << "  iid  eid  code  numcontigs  contig iid list\n"
        << "\n"
        << "  Codes are:\n"
        << "  S: Singleton read\n"
        << "  P: Placed in a single contig\n"
        << "  D: Placed in multiple contigs\n"
        << "\n.OPTIONS.\n"
        << "  -h  Display help information\n"
        << "  -s  Disregard bank locks and write permissions (spy mode)\n"
        << "  -v  Display the compatible bank version\n"
        << "\n"
        << "  -S  Just list singleton reads\n"
        << "  -D  Just list duplicate reads\n"
        << "  -P  Just list placed reads\n"
        << "\n"
        << "  -E  Just list read EIDs\n"
        << "  -I  Just list read IIDs\n"
        << "\n.KEYWORDS.\n"
        << "  amos bank, reads\n"
        << endl;

  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr << "\n.USAGE.\n" << "  " <<  s << "  [options]  -b <bank path>\n";
  return;
}
