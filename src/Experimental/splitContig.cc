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
bool   OPT_SNPAnalysis = false;
bool   OPT_Verbose = false;
typedef map <ID_t, ID_t> IDMap;

string contigeid;
ID_t   contigiid = AMOS::NULL_ID;



bool hasOverlap(Pos_t rangeStart, // 0-based exact offset of range
                Pos_t rangeEnd,   // 0-based exact end of range
                Pos_t seqOffset,  // 0-bases exact offset of seq
                Pos_t seqLen,     // count of bases of seq (seqend+1)
                Pos_t contigLen)  // count of bases in contig (contigend+1)
{
  int retval = 1;

  if (seqOffset >= (Pos_t) 0)
  {
    if ((seqOffset > rangeEnd)                  // sequence right flanks
        || (seqOffset + seqLen-1 < rangeStart)) // sequence left flanks
    {
      retval = 0;
    }
  }
  else
  {
    // Negative Offset, test left and right separately
    retval = hasOverlap(rangeStart, rangeEnd, 
                        0, seqLen+seqOffset, contigLen) ||
             hasOverlap(rangeStart, rangeEnd, 
                        contigLen+seqOffset, -seqOffset, contigLen);
  }

  return retval;
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

  map <ID_t, Distribution_t> libmap;
  map <ID_t, Pos_t> readlens;

  IDMap mates;
  IDMap readliblookup;
  IDMap frgliblookup;
  IDMap redctglookup;
  IDMap ctgscflookup;

  Scaffold_t scaff;
  Contig_t contig;

  BankStream_t lib_bank (Library_t::NCODE);
  BankStream_t frg_bank (Fragment_t::NCODE);
  BankStream_t ctg_bank (Contig_t::NCODE);
  BankStream_t scf_bank (Scaffold_t::NCODE);
  BankStream_t red_bank (Read_t::NCODE);
  Bank_t       read_bank(Read_t::NCODE);

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    BankMode_t bm = OPT_BankSpy ? B_SPY : B_READ;
    red_bank . open (OPT_BankName, bm);
    read_bank . open (OPT_BankName, bm);
    frg_bank . open (OPT_BankName, bm);
    lib_bank . open (OPT_BankName, bm);
    ctg_bank . open (OPT_BankName, bm);
    scf_bank . open (OPT_BankName, bm);


    while ( lib_bank >> lib )
    {
      libmap[lib.getIID()] = lib.getDistribution();
    }
    cerr << "Indexed " << libmap.size() << " libraries" << endl;


    cerr << "Indexing frgs... ";
    while (frg_bank >> frg)
    {
      frgliblookup[frg.getIID()] = frg.getLibrary();
      mates[frg.getMatePair().first] = frg.getMatePair().second;
      mates[frg.getMatePair().second] = frg.getMatePair().first;
    }
    cerr << frgliblookup.size() << " fragments";
    cerr << mates.size() << " mates" << endl;

    cerr << "Indexing reds... ";
    while (red_bank >> red)
    {
      IDMap::const_iterator fi = frgliblookup.find(red.getFragment());

      if (fi != frgliblookup.end())
      {
        readliblookup[red.getIID()] = fi->second;
      }

      readlens[red.getIID()] = red.getClearRange().getLength();
    }
    cerr << readliblookup.size() << " reads in fragments" << endl;


    vector<Tile_t>::const_iterator ri;

    cerr << "Index contigs...";
    int contigcount = 0;
    while (ctg_bank >> contig)
    {
      for (ri = contig.getReadTiling().begin();
           ri != contig.getReadTiling().end();
           ri++)
      {
        redctglookup[ri->source] = contig.getIID();
      }
    }
    cerr << redctglookup.size() << " reads in contigs" << endl;

    cerr << "Index scaffolds... ";
    while (scf_bank >> scaff)
    {
      for (ri = scaff.getContigTiling().begin();
           ri != scaff.getContigTiling().end();
           ri++)
      {
        ctgscflookup[ri->source] = scaff.getIID();
      }
    }
    cerr << ctgscflookup.size() << " contigs in scaffolds" << endl;


    if (contigiid == AMOS::NULL_ID)
    {
      contigiid = ctg_bank.lookupIID(contigeid.c_str());
    }

    if (contigiid == AMOS::NULL_ID)
    {
      cerr << "contigiid == AMOS::NULL_ID" << endl;
      exit (1);
    }

    vector <Tile_t> rtiling;

    ctg_bank.seekg(ctg_bank.getIDMap().lookupBID(contigiid));
    ctg_bank >> contig;
    rtiling = contig.getReadTiling();

    cerr << "Finding mates for contig" << endl;

    int count = 0;


    map<ID_t, int> fctgfreq;
    map<ID_t, int> rctgfreq;
    map<ID_t, int>::iterator fi;
    IDMap redmatectg;

    IDMap::const_iterator ci;
    IDMap::const_iterator mi;

    if (OPT_Verbose)
    {
      cout << ">Read\tCtg\tDir" << endl;
    } 

    for (ri = rtiling.begin(); ri != rtiling.end(); ri++)
    {
      string eid = red_bank.getIDMap().lookupEID(ri->source);
      char dir = ri->range.isReverse() ? 'R' : 'F';

      mi = mates.find(ri->source);
      if (mi != mates.end())
      {
        ci = redctglookup.find(mi->second);
        
        if (ci != redctglookup.end())
        {
          if (OPT_Verbose)
          {
            cout << eid << "\t" << ci->second << "\t" << dir << endl;
          }

          redmatectg[ri->source] = ci->second;

          if (dir == 'F')
          {
            fi = fctgfreq.find(ci->second);
            if (fi == fctgfreq.end()) { fi = fctgfreq.insert(make_pair(ci->second, 0)).first; }
          }
          else
          {
            fi = rctgfreq.find(ci->second);
            if (fi == rctgfreq.end()) { fi = rctgfreq.insert(make_pair(ci->second, 0)).first; }
          }

          fi->second++;

        }
      }
      else
      {
        if (OPT_Verbose)
        {
          cout << eid << "\t***\t" << dir << endl;
        }
        redmatectg[ri->source] = AMOS::NULL_ID;
      }
    }

    if (OPT_Verbose)
    {
      cout << endl << endl;
    }

    cout << ">Ctg\tScaff\tFreq\tDir" << endl;
     
    for (fi = rctgfreq.begin(); fi != rctgfreq.end(); fi++)
    {
      cout << fi->first << "\t" 
           << ctgscflookup[fi->first] << "\t"
           << fi->second << "\tR" << endl;
    }

    for (fi = fctgfreq.begin(); fi != fctgfreq.end(); fi++)
    {
      cout << fi->first << "\t" 
           << ctgscflookup[fi->first] << "\t"
           << fi->second << "\tF" << endl;
    }


    cout << endl << endl;

    if (OPT_SNPAnalysis)
    {
      cerr << "Analyzing SNPs" << endl;

      cout << ">Pos Base([CTG,FRQ])" << endl;

      ContigIterator_t ctgit(contig, &read_bank);
      while (ctgit.advanceNext())
      {
        if (ctgit.hasSNP())
        {
          Column_t column(ctgit.getColumn());
          vector<BaseStats_t *> freq(column.getBaseInfo());

          cout << ctgit.gindex();

          for (int i = 0; i < freq.size(); i++)
          {
            map<ID_t, int> ctgfreq;
            map<ID_t, int>::iterator cfi;

            char base = toupper(freq[i]->m_base);

            // Figure out which matecontigs these reads represent
            vector<TiledReadList_t::const_iterator>::const_iterator ri;
            for (ri = freq[i]->m_reads.begin(); ri != freq[i]->m_reads.end(); ri++)
            {
              ID_t matectg = redmatectg[(*ri)->m_iid];
              cfi = ctgfreq.find(matectg);
              if (cfi == ctgfreq.end()){cfi = ctgfreq.insert(make_pair(matectg, 0)).first; }
              cfi->second++;
            }

            // Dump frequency of matecontigs
            cout << "\t" << base << "(";
            bool first = true;
            for (cfi = ctgfreq.begin(); cfi != ctgfreq.end(); cfi++)
            {
              if (!first) { cout << " "; }
              first = false;

              cout << "[" << cfi->first << "," << cfi->second << "]";
            }

            cout << ")";
          }

          cout << endl;
        }
      }
    }

    red_bank.close();
    frg_bank.close();
    lib_bank.close();
    ctg_bank.close();
    scf_bank.close();
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsvEIS")) != EOF) )
  {
    switch (ch)
    {
      case 'h': PrintHelp (argv[0]); exit (EXIT_SUCCESS); break;
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break;

      case 's': OPT_BankSpy = true;      break;
      case 'S': OPT_SNPAnalysis = true;  break;
      case 'V': OPT_Verbose = true;  break;
      case 'E': contigeid = argv[optind++]; break;
      case 'I': contigiid = atoi(argv[optind++]); break;

      default: errflg ++;
    }
  }

  if (errflg > 0 || optind != argc - 1)
  {
    PrintUsage (argv[0]);
    cerr << "Try '" << argv[0] << " -h' for more information.\n";
    exit (EXIT_FAILURE);
  }


  if (contigeid.empty() && contigiid == AMOS::NULL_ID)
  {
    cerr << "contig id is required" << endl;
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
    << "-E contigeid  Contig eid of interest\n"
    << "-I contigeid  Contig iid of interest\n"
    << "-S            Perform SNP analysis in addition to mates\n"
    << "-V            Verbose mode\n"
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
