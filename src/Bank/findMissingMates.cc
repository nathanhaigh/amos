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
using namespace std;
using namespace AMOS;


//=============================================================== Globals ====//
string OPT_BankName;                 // bank name parameter
bool   OPT_BankSpy = false;          // read or read-only spy


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

#include <map>
typedef map <ID_t, ID_t> IDMap;



//========================================================= Function Defs ====//
int main (int argc, char ** argv)
{
  int exitcode = EXIT_SUCCESS;
  Read_t red;
  Fragment_t frg;
  Library_t lib;
  Matepair_t mtp;

  BankStream_t lib_bank (Library_t::NCODE);
  BankStream_t frg_bank (Fragment_t::NCODE);
  BankStream_t mtp_bank (Matepair_t::NCODE);
  BankStream_t ctg_bank (Contig_t::NCODE);
  BankStream_t scf_bank (Scaffold_t::NCODE);
  BankStream_t red_bank (Read_t::NCODE);

  long int cntw = 0;             // written object count

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    BankMode_t bm = OPT_BankSpy ? B_SPY : B_READ;
    red_bank . open (OPT_BankName, bm);
    mtp_bank . open (OPT_BankName, bm);
    frg_bank . open (OPT_BankName, bm);
    lib_bank . open (OPT_BankName, bm);
    ctg_bank . open (OPT_BankName, bm);
    scf_bank . open (OPT_BankName, bm);

    map <ID_t, Distribution_t> libmap;

    //-- Iterate through each library in the bank
    while ( lib_bank >> lib )
    {
      libmap[lib.getIID()] = lib.getDistribution();
    }

    cerr << "Indexed " << libmap.size() << " libraries" << endl;

    IDMap mates;
    IDMap readliblookup;
    IDMap frgliblookup;

    cerr << "Indexing frgs... ";
    while (frg_bank >> frg)
    {
      frgliblookup[frg.getIID()] = frg.getLibrary();
    }
    cerr << frgliblookup.size() << " fragments" << endl;

    cerr << "Indexing reds... ";
    while (red_bank >> red)
    {
      IDMap::const_iterator fi = frgliblookup.find(red.getFragment());

      if (fi != frgliblookup.end())
      {
        readliblookup[red.getIID()] = fi->second;
      }
    }
    cerr << readliblookup.size() << " reads in fragments" << endl;


    cerr << "Indexing mates... ";
    while ( mtp_bank >> mtp )
    {
      //-- Get the library record for this insert
      mates[mtp.getReads().first] = mtp.getReads().second;
      mates[mtp.getReads().second] = mtp.getReads().first;

      readliblookup[mtp.getReads().first] = frg.getLibrary();
      readliblookup[mtp.getReads().second] = frg.getLibrary();
    }
    cerr << mates.size() << " mates" << endl;


    IDMap contigscafflookup;

    Scaffold_t scaff;
    Contig_t contig;
    cerr << "Indexing scaffolds... ";
    int scaffcount = 0;
    while (scf_bank >> scaff)
    {
      scaffcount++;
      vector<Tile_t> & ctiling = scaff.getContigTiling();
      vector<Tile_t>::const_iterator ci;

      for (ci = ctiling.begin(); ci != ctiling.end(); ci++)
      {
        contigscafflookup[ci->source] = scaff.getIID();
      }
    }

    cerr << contigscafflookup.size() << " contigs in " 
         << scaffcount << " scaffolds" << endl;


    // load read tiling for scaffold of this contig
    string contigeid = "1047274138691";
    int rangeStart = 100;
    int rangeEnd = 1000;

    // ID_t contigiid = ctg_bank.lookupIID(contigeid.c_str());
    ID_t contigiid = 92;

    if (contigiid == AMOS::NULL_ID)
    {
      cerr << "contigiid == AMOS::NULL_ID" << endl;
      exit (1);
    }


    IDMap::iterator si = contigscafflookup.find(contigiid);

    vector <Tile_t> rtiling;

    if (si == contigscafflookup.end())
    {
      cerr << "Contig not in scaffold, getting tiling" << endl;
      ctg_bank.seekg(ctg_bank.getIDMap().lookupBID(contigiid));
      ctg_bank >> contig;
      rtiling = contig.getReadTiling();
    }
    else
    {
      scf_bank.seekg(scf_bank.getIDMap().lookupBID(si->second));
      scf_bank >> scaff;

      vector<Tile_t> & ctiling = scaff.getContigTiling();
      vector<Tile_t>::const_iterator ci;

      cerr << "Mapping reads to scaffold " << scaff.getEID() << "... ";

      for (ci = ctiling.begin(); ci != ctiling.end(); ci++)
      {
        ctg_bank.seekg(ctg_bank.getIDMap().lookupBID(ci->source));
        ctg_bank >> contig;

        int clen = contig.getSeqString().size();

        // map range to scaffold coordinates
        if (ci->source == contigiid)
        {
          if (ci->range.isReverse())
          {
            rangeStart = clen - rangeStart;
            rangeEnd   = clen - rangeEnd;
          }

          rangeStart += ci->offset;
          rangeEnd   += ci->offset;

          cerr << "new range: " << rangeStart << "," << rangeEnd << endl;
        }

        vector<Tile_t> & crtiling = contig.getReadTiling();
        vector<Tile_t>::const_iterator ri;
        for (ri = crtiling.begin(); ri != crtiling.end(); ri++)
        {
          Tile_t mappedTile;
          mappedTile.source = ri->source;
          mappedTile.gaps   = ri->gaps;
          mappedTile.range  = ri->range;

          int offset = ri->offset;
          if (ci->range.isReverse())
          {
            mappedTile.range.swap();
            offset = clen - (offset + ri->range.getLength() + ri->gaps.size());
          }

          mappedTile.offset = ci->offset + offset;

          rtiling.push_back(mappedTile);
        }
      }

      cerr << rtiling.size() << " reads mapped" << endl;
    }


    // test each read to see if it, or its mate falls within range

    cerr << "Testing reads" << endl;

    map <ID_t, bool> readstatus;

    vector<Tile_t>::const_iterator ri;
    for (ri = rtiling.begin(); ri != rtiling.end(); ri++)
    {
      if (hasOverlap(rangeStart, rangeEnd, 
                     ri->offset, ri->range.getLength() + ri->gaps.size(),
                     10000000))
      {
        cout << "*";
      }

      cout << ri->source << "\t" << red_bank.lookupEID(ri->source) << "\t" << ri->offset << "\t" << ri->offset+ri->gaps.size()+ri->range.getLength() << endl;

      IDMap::const_iterator mi;

      mi = mates.find(ri->source);

      if (mi != mates.end())
      {
        int projectedStart;
        int projectedEnd;

        IDMap::const_iterator rli = readliblookup.find(ri->source);

        if (rli != readliblookup.end())
        {
          map<ID_t, Distribution_t>::const_iterator li = libmap.find(ri->source);

          if (li != libmap.end())
          {
            int NUMSD = 3;
            int READLEN = 750;

            if (ri->range.isReverse())
            {
              projectedStart = ri->offset + ri->range.getLength() + ri->gaps.size() - (li->second.mean + NUMSD*li->second.sd);
              projectedEnd   = ri->offset + ri->range.getLength() + ri->gaps.size() - (li->second.mean - NUMSD*li->second.sd-READLEN);
            }
            else
            {
              projectedStart = ri->offset + li->second.mean - NUMSD*li->second.sd - READLEN;
              projectedEnd   = ri->offset + li->second.mean + NUMSD*li->second.sd;
            }


            if (hasOverlap(rangeStart, rangeEnd, 
                           projectedStart, projectedEnd,
                           10000000))
            {
              cout << "*" << endl;
            }

            cout << "mate: " << mi->second << "\t" << red_bank.lookupEID(mi->second) << "\t" << projectedStart << "\t" << projectedEnd;
          }
        }
      }
    }

    red_bank.close();
    mtp_bank.close();
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsv")) != EOF) )
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
    << endl;
  cerr
    << "Takes an AMOS bank directory and dumps a bambus .mates files to\n"
    << "stdout\n\n";
  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr
    << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
  return;
}
