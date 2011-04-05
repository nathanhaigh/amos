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
bool   OPT_UseScaffolds = false;
bool   OPT_MissingOnly = false;
bool   OPT_USEMATES = true;
typedef map <ID_t, ID_t> IDMap;

ID_t   contigiid = AMOS::NULL_ID;
string contigeid;
int    rangeStart = -1;
int    rangeEnd = -1;
bool   verbosemode = false;



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
  IDMap contigscafflookup;

  Scaffold_t scaff;
  Contig_t contig;

  BankStream_t lib_bank (Library_t::NCODE);
  BankStream_t frg_bank (Fragment_t::NCODE);
  BankStream_t ctg_bank (Contig_t::NCODE);
  BankStream_t scf_bank (Scaffold_t::NCODE);
  BankStream_t red_bank (Read_t::NCODE);

  //-- Parse the command line arguments
  ParseArgs (argc, argv);

  //-- BEGIN: MAIN EXCEPTION CATCH
  try {

    BankMode_t bm = OPT_BankSpy ? B_SPY : B_READ;
    red_bank . open (OPT_BankName, bm);
    frg_bank . open (OPT_BankName, bm);
    lib_bank . open (OPT_BankName, bm);
    ctg_bank . open (OPT_BankName, bm);

    if (OPT_UseScaffolds)
    {
      cerr << "Opening scaffold bank" << endl;
      scf_bank . open (OPT_BankName, bm);
    }


    while ( lib_bank >> lib )
    {
      libmap[lib.getIID()] = lib.getDistribution();
    }
    cerr << "Indexed " << libmap.size() << " libraries" << endl;


    cerr << "Indexing frgs... ";
    while (frg_bank >> frg)
    {
      frgliblookup[frg.getIID()] = frg.getLibrary();

      std::pair<ID_t, ID_t> mtp = frg.getMatePair();

      mates[mtp.first] = mtp.second;
      mates[mtp.second] = mtp.first;
    }
    cerr << frgliblookup.size() << " fragments" << endl;
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


    if (OPT_UseScaffolds)
    {
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
    }



    // load read tiling for scaffold of this contig
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
    map<ID_t, Tile_t *> tilelookup;
    map<ID_t, Tile_t *>::iterator tli;
    vector<Tile_t>::iterator ri;

    IDMap::iterator si = contigscafflookup.find(contigiid);
    if (si == contigscafflookup.end())
    {
      if (OPT_UseScaffolds)
      {
        cerr << "Contig not in scaffold, using original read tiling" << endl;
      }

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

        int clen = contig.getLength();

        // map original range to scaffold coordinates
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

    tilelookup.clear();
    for (ri = rtiling.begin(); ri != rtiling.end(); ri++)
    {
      tilelookup.insert(make_pair(ri->source, &(*ri)));
    }

    if (rangeEnd < rangeStart)
    {
      int t = rangeEnd;
      rangeEnd = rangeStart;
      rangeStart = t;
    }



    cerr << "Finding reads that overlap [" << rangeStart << "," << rangeEnd << "]" << endl;
    int count = 0;
    for (ri = rtiling.begin(); ri != rtiling.end(); ri++)
    {
      bool hasovl = false;
      if (hasOverlap(rangeStart, rangeEnd, 
                     ri->offset, ri->getGappedLength(),
                     10000000))
      {
        hasovl = true;
      }

      if (verbosemode)
      {
        if (hasovl) { cout << "*"; }
        cout << "read: " << ri->source 
             << "\t" << red_bank.lookupEID(ri->source) 
             << "\t" << ri->offset 
             << "\t" << ri->getRightOffset()
             << "\t" << (ri->range.isReverse() ? "R" : "F") 
             << endl;
      }
      else if (hasovl)
      {
        if (!OPT_MissingOnly) 
        { 
          cout << "t\t" << red_bank.lookupEID(ri->source) 
               << "\t" << ri->offset << "\t" << ri->getRightOffset() 
               << "\t" << (ri->range.isReverse() ?  "R" : "F") << endl; }
      }
      
      if (OPT_USEMATES)
      {
        IDMap::const_iterator mi = mates.find(ri->source);
        if (mi != mates.end())
        {
          IDMap::const_iterator rli = readliblookup.find(ri->source);
          if (rli != readliblookup.end())
          {
            map<ID_t, Distribution_t>::const_iterator li = libmap.find(rli->second);
            if (li != libmap.end())
            {
              int projectedStart;
              int projectedEnd;

              int NUMSD = 3;
              int readlen = readlens[mi->second];

              if (ri->range.isReverse())
              {
                int roffset = ri->offset + ri->range.getLength() + ri->gaps.size() - 1;
                projectedStart = roffset - li->second.mean - NUMSD*li->second.sd;
                projectedEnd   = roffset - li->second.mean + NUMSD*li->second.sd + readlen;
              }
              else
              {
                projectedStart = ri->offset + li->second.mean - NUMSD*li->second.sd - readlen;
                projectedEnd   = ri->offset + li->second.mean + NUMSD*li->second.sd;
              }

              hasovl = false;

              if (hasOverlap(rangeStart, rangeEnd, 
                             projectedStart, projectedEnd - projectedStart + 1,
                             10000000))
              {
                hasovl = true;
              }

              if (verbosemode)
              {
                if (hasovl) { cout << "*"; }

                cout << "mate: " << mi->second 
                     << "\t" << red_bank.lookupEID(mi->second) 
                     << "\t" << projectedStart 
                     << "\t" << projectedEnd 
                     << "\t" << (ri->range.isReverse() ? "F" : "R") 
                     << endl;
              }
              else if (hasovl)
              {
                if (OPT_MissingOnly)
                {
                  tli = tilelookup.find(mi->second);
                }

                if (!OPT_MissingOnly || tli == tilelookup.end())
                {
                  cout << "p\t" << red_bank.lookupEID(mi->second) << "\t" << projectedStart << "\t" << projectedEnd 
                       << "\t" << (ri->range.isReverse() ? "F" : "R") << endl;
                }
              }
            }
            else if (verbosemode)
            {
              cout << "no dist" << endl;
            }
          }
          else if (verbosemode)
          {
            cout << "no lib" << endl;
          }
        }
        else if (verbosemode)
        {
          cout << "no mate" << endl;
        }
      }
    }

    red_bank.close();
    frg_bank.close();
    lib_bank.close();
    ctg_bank.close();
    
    if (OPT_UseScaffolds)
    {
      scf_bank.close();
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

  while ( !errflg && ((ch = getopt (argc, argv, "hsvVE:I:x:y:SMm")) != EOF) )
    switch (ch)
      {
      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 's': OPT_BankSpy = true; break; 
      case 'v': PrintBankVersion (argv[0]); exit (EXIT_SUCCESS); break; 
      case 'E': contigeid  = optarg; break;
      case 'I': contigiid  = atoi(optarg); break;
      case 'x': rangeStart = atoi(optarg); break;
      case 'y': rangeEnd   = atoi(optarg); break;
      case 'V': verbosemode = true; break;
      case 'S': OPT_UseScaffolds = true; break;
      case 'M': OPT_MissingOnly = true; break;
      case 'm' :OPT_USEMATES  = false; break;

      default: errflg ++;
      }

  if (errflg > 0 || optind != argc - 1)
  {
    PrintUsage (argv[0]);
    cerr << "Try '" << argv[0] << " -h' for more information.\n";
    exit (EXIT_FAILURE);
  }


  if (rangeStart == -1 || rangeEnd == -1 || 
      (contigeid.empty() && contigiid == AMOS::NULL_ID))
  {
    cerr << "Range coordinates and contig id are required" << endl;
    exit (EXIT_FAILURE);
  }

  OPT_BankName = argv [optind ++];
}




//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage (s);

   cerr << "\n.DESCRIPTION.\n"
        << "  Finds all reads that should overlap a given contig range. Includes reads that\n"
        << "  should be present by the virtue of their mate and the scaffold\n"
        << "\n.OPTIONS.\n"
        << "  -h            Display help information\n"
        << "  -s            Disregard bank locks and write permissions (spy mode)\n"
        << "  -v            Display the compatible bank version\n"
        << "  -S            Looks for mates by virtue of the scaffold\n"
        << "  -M            Only display missing mates (not reads already present in range)\n"
        << "  -m            Don't use mate information, just read tiling\n"
        << "  -E contigeid  Contig eid of interest\n"
        << "  -I contigiid  Contig iid of interest\n"
        << "  -x start      Start of range\n"
        << "  -y end        End of range\n"
        << "\n.KEYWORDS.\n"
        << "  amos bank, mates"
        << endl;

  return;
}




//------------------------------------------------------------ PrintUsage ----//
void PrintUsage (const char * s)
{
  cerr << "\n.USAGE.\n" << "  " <<  s << "  [options]  -b <bank path>\n";
  return;
}
