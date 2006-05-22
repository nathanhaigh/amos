// $Revision$

#if HAVE_CONFIG_H
#include "config.h"
#endif

// C includes
#include <getopt.h>

// STL includes
#include <map>
#include <string>
#include <vector>

// AMOS includes
#include "Contig_AMOS.hh"
#include "BankStream_AMOS.hh"
#include "Bank_AMOS.hh"
#include "Layout_AMOS.hh"
#include "Fragment_AMOS.hh"
#include "Library_AMOS.hh"
#include "Index_AMOS.hh"

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

#define endl "\n"   // die endl, die!

map<string, string> globals;   // global variables


void printHelpText()
{
  cout << 
    "cvgStat -b bank [-lay|-ctg] [-red|-frg]> file.cvg\n"
    "\n"
    "-b bank - name of bank to be used\n"
    "-lay    - computes coverage of layout messages\n"
    "-ctg    - computes coverage of contig messages (default)\n"
    "-scaff  - computes coverage of scaffolds\n"
    "-red    - computes read coverage (default)\n"
    "-frg    - computes fragment/clone coverage\n"
    "-rdfrg  - computes difference between fragment and read coverages\n"
    "-iid    - headers are iids\n"
    "-eid    - headers are eids\n"
       << endl;
} // printHelpText

bool GetOptions(int argc, char ** argv)
{
  int option_index = 0;
  static struct option long_options[] = {
    {"b",     1, 0, 'b'},
    {"lay",   0, 0, 'l'},
    {"ctg",   0, 0, 'c'},
    {"scaff", 0, 0, 's'},
    {"red",   0, 0, 'r'},
    {"frg",   0, 0, 'f'},
    {"rdfrg", 0, 0, 'd'},
    {"iid",   0, 0, 'i'},
    {"eid",   0, 0, 'e'},
    {"help",  0, 0, 'h'},
    {"h",     0, 0, 'h'}, 
    {0, 0, 0, 0}
  };
  
  bool helpRequested = false;
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1){
    switch (c){
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case 'l':
      globals["layouts"] = "true";
      break;
    case 'c':
      globals["contigs"] = "true";
      break;
    case 'r':
      globals["reads"] = "true";
      break;
    case 'f':
      globals["fragments"] = "true";
      break;
    case 'd':
      globals["frgnoread"] = "true";
      globals["fragments"] = "true";
      break;
    case 'i':
      globals["byiid"] = "true";
      break;
    case 'e':
      globals["byeid"] = "true";
      break;
    case 's':
      globals["scaff"] = "true";
      break;
    case 'h':
      //      printHelpText();
      helpRequested = true;
      break;
    case '?':
      return false;
    }
  } // while each parameter

  if (helpRequested){
    printHelpText();
    exit(0);
  } // if help

  if (optind < argc){
    globals["input"] = string(argv[optind]);
  } 

  return true;
} // GetOptions



//--------------------------------------------------------
int main(int argc, char ** argv)
{
  bool doLay = false; 
  bool doCtg = true;
  bool doRed = true;
  bool doFrg = false;
  bool doScaff = false;

  if (!GetOptions(argc, argv)){
    cerr << "Command line parsing failed, use -h option for usage info" 
	 << endl;
    //printHelpText();
    exit(1);
  }

  if (globals.find("layouts") != globals.end()) {
    doLay = true;
    doCtg = false;
    doScaff = false;
  }
  if (globals.find("contigs") != globals.end()) {
    doCtg = true;
    doLay = false;
    doScaff = false;
  }
  if (globals.find("fragments") != globals.end()) {
    doFrg = true;
    doRed = false;
  }
  if (globals.find("reads") != globals.end()) {
    doRed = true;
    doFrg = false;
  }
  if (globals.find("scaff") != globals.end()) {
    doCtg = false;
    doLay = false;
    doScaff = true;
  }

  BankStream_t contig_stream (Contig_t::NCODE);
  Bank_t contig_bank (Contig_t::NCODE);
  BankStream_t scaffold_stream (Scaffold_t::NCODE);
  BankStream_t layout_stream (Layout_t::NCODE);
  Bank_t fragment_bank (Fragment_t::NCODE);
  BankStream_t library_stream (Library_t::NCODE);

  if (globals.find("bank") == globals.end()){
    cerr << "A bank must be specified with option -b" << endl;
    exit(1);
  }

  if (doCtg) {
    try {
      if (! contig_stream.exists(globals["bank"])){
	cerr << "Cannot find a contig account in bank: " << globals["bank"] << endl;
	exit(1);
      }
      contig_stream.open(globals["bank"], B_READ);
    } catch (Exception_t & e)
      {
	cerr << "Failed to open contig account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }
  } // if doCtg

  if (doLay) {
    try {
      if (! layout_stream.exists(globals["bank"])){
	cerr << "Cannot find a layout account in bank: " << globals["bank"] << endl;
	exit(1);
      }
      layout_stream.open(globals["bank"], B_READ);
    } catch (Exception_t & e)
      {
	cerr << "Failed to open layout account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }
  } // if doLay


  if (doScaff) {
    try {
      if (! scaffold_stream.exists(globals["bank"])) {
	cerr << "Cannot find a scaffold account in bank: " << globals["bank"] << endl;
	exit(1);
      }
      scaffold_stream.open(globals["bank"], B_READ);
    } catch  (Exception_t & e)
      {
	cerr << "Failed to open scaffold account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }
    try {
      if (! contig_bank.exists(globals["bank"])) {
	cerr << "Cannot find a contig account in bank: " << globals["bank"] << endl;
	exit(1);
      }
      contig_bank.open(globals["bank"], B_READ);
    } catch  (Exception_t & e)
      {
	cerr << "Failed to open contig account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }
  } // if doScaff

  map <ID_t, Distribution_t> lib2dist; // map from libid to distribution
  Index_t rd2mate;                     // mate pair index
  Index_t rd2lib;                      // read to library index

  if (doFrg) {
    try { 
      if (! fragment_bank.exists(globals["bank"])){
	cerr << "Cannot find a fragment account in bank: " << globals["bank"] << endl;
	exit(1);
      }
      fragment_bank.open(globals["bank"], B_READ);
    } catch (Exception_t & e)
      {
	cerr << "Failed to open fragment account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }

    try { 
      if (! library_stream.exists(globals["bank"])){
	cerr << "Cannot find a library account in bank: " << globals["bank"] << endl;
	exit(1);
      }
      library_stream.open(globals["bank"], B_READ);
    } catch (Exception_t & e)
      {
	cerr << "Failed to open library account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
     } 

    // populate a libid 2 distribution map
    Library_t lib;
    while (library_stream >> lib)
      lib2dist[lib.getIID()] = lib.getDistribution();

    library_stream.close();

    // build mate pair index
    try {
      rd2mate.buildReadMate(globals["bank"]);
      rd2lib.buildReadLibrary(globals["bank"]);
    } catch (const Exception_t & e) {
      cerr << "ERROR: " << e.what() << endl
	   << "    could not collect mate pair info\n";
      exit(1);
    }
  } // if doFrg

  // go through the bank one contig at a time
  while (true) {
    vector<Tile_t> tv;  // vector of tiles
    Pos_t len = 0;      // length of layout or contig

    if (doLay) {
      Layout_t lay;

      layout_stream >> lay;

      if (layout_stream.eof()) // emptied bank
	break;

      if (lay.getEID() != "" && globals.find("byiid") == globals.end()){
	cout << ">" << lay.getEID() << " LAY iid:" << lay.getIID() << endl;
      } else if (lay.getIID() != 0 && globals.find("byeid") == globals.end()){
	cout << ">" << lay.getIID() << " LAY eid:" << lay.getEID() << endl;
      } else 
	continue;              // no id, no output

      tv = lay.getTiling();
    }

    if (doCtg) {
      Contig_t ctg;

      contig_stream >> ctg;

      if (contig_stream.eof()) // emptied bank
	break;

      if (ctg.getEID() != "" && globals.find("byiid") == globals.end()){
	cout << ">" << ctg.getEID() << " CTG iid:" << ctg.getIID() << endl;
      } else if (ctg.getIID() != 0 && globals.find("byeid") == globals.end()){
	cout << ">" << ctg.getIID() << " CTG eid:" << ctg.getEID() << endl;
      } else 
	continue;              // no id, no output

      tv = ctg.getReadTiling();
      len = ctg.getLength();
    }

    if (doScaff) {
      Scaffold_t scf;

      scaffold_stream >> scf;

      if (scaffold_stream.eof()) // emptied bank
	break;

      len = scf.getSpan();
      
      if (scf.getEID() != "" && globals.find("byiid") == globals.end()){
	cout << ">" << scf.getEID() << " SCF iid:" << scf.getIID() << endl;
      } else if (scf.getIID() != 0 && globals.find("byeid") == globals.end()){
	cout << ">" << scf.getIID() << " SCF eid:" << scf.getEID() << endl;
      } else 
	continue;              // no id, no output

      for (vector<Tile_t>::iterator ti = scf.getContigTiling().begin();
	   ti != scf.getContigTiling().end(); ti++) { // for each contig
	Tile_t tle;
	Contig_t ctg;
	
	try {
	  contig_bank.fetch(ti->source, ctg);
	} catch (Exception_t & e)
	  {
	    cerr << "Can't find contig " << ti->source << " in bank " << globals["bank"]
		 << ": " << endl << e << endl;
	    exit(1);
	  }

	for (vector<Tile_t>::iterator cti = ctg.getReadTiling().begin();
	     cti != ctg.getReadTiling().end(); cti++) {
	  tle = *cti;
	  if (ti->range.isReverse()) {
	    tle.range.swap();
	    tle.offset = ti->offset + ctg.getLength() 
	      - tle.offset - tle.getGappedLength();
	  } else { // forward tile
	    tle.offset += ti->offset;
	  }
	  tv.push_back(tle);
	} // for each tile in contig
      } // for each contig
    } // if doScaff

    // now we just have a tiling of reads.
    // need to convert it to pairs of coords on the contig/layout
    vector<Pos_t> starts;
    vector<Pos_t> ends;
    starts.reserve(tv.size()); // we'll need at most as many spaces as tiles
    ends.reserve(tv.size());

    if (doFrg) { // this will take some work to convert from mates to a tile
      hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > rd2tile;
      for (vector<Tile_t>::iterator ti = tv.begin(); ti != tv.end(); ti++) {
	Range_t rng = ti->range;

	// offset is always leftmost point
	// adjust range for gaps
	if (ti->range.begin < ti->range.end) {
	  rng.begin = ti->offset;
	  rng.end = ti->getGappedLength() + ti->offset;
	} else {
	  rng.end = ti->offset;
	  rng.begin = ti->getGappedLength() + ti->offset;
	}

	rd2tile[ti->source] = rng;
      }

      // second pass through tiles, this time for real
      for (vector<Tile_t>::iterator ti = tv.begin(); ti != tv.end(); ti++) {
	//	cerr << "REad " << ti->source << " has mate " << rd2mate.lookup(ti->source) << endl;
	//	cerr << "At coords " << rd2tile[ti->source].begin << ", " << rd2tile[ti->source].end << endl;
	if (rd2mate.lookup(ti->source) != NULL_ID && 
	    rd2mate.lookup(ti->source) < ti->source) { // only do this for mate with lowest iid
	  if (rd2lib.lookup(ti->source) == NULL_ID) {
	    cerr << "No library for read with IID: " << ti->source << endl;
	    exit(1);
	  }

	  Distribution_t dist = lib2dist[rd2lib.lookup(ti->source)];
	  ID_t mate = rd2mate.lookup(ti->source);
	  Range_t rngA = rd2tile[ti->source];

	  if (rd2tile.find(mate) == rd2tile.end()) // skip fragment
	    continue;

	  Range_t rngB = rd2tile[mate];

	  //	  cerr << "Got " << rngA.begin << ", " << rngA.end << ":  " << rngB.begin << ", " << rngB.end << endl;

	  int oriA = (rngA.begin < rngA.end) ? 1 : -1;
	  int oriB = (rngB.begin < rngB.end) ? 1 : -1;

	  if (oriA == oriB) { // not a valid mate 
	    //	    cerr << "ORI\n";
	    continue;
	  }

	  if (oriA == 1 && (rngB.begin < rngA.begin || rngB.end < rngA.begin)) {
	    //	    cerr << "OUTIE B before A\n";
	    continue;   // if A is forward, B must occur after it
	  }

	  if (oriB == 1 && (rngA.begin < rngB.begin || rngA.end < rngB.end)) {
	    //	    cerr << "OUTIE A before B\n";
	    continue;  // if B is forward, A must occur after it
	  }
	 
	  // otherwise this is a happy-ish mate
	  // calculate location of fragment in contig
	  int min = (oriA == 1) ? rngA.begin : rngB.begin;
	  int max = (oriA == 1) ? rngB.begin : rngA.begin;

	  int unreadmin = (oriA == 1) ? rngA.end : rngB.end;
	  int unreadmax = (oriA == 1) ? rngB.end : rngA.end;

	  if (unreadmin > unreadmax){
	    int tmp = unreadmin;
	    unreadmin = unreadmax;
	    unreadmax = tmp;
	  }

	  //	  cerr << "Min = " << min << " Max = " << max << endl;
	  assert (min < max);

	  if (abs(max - min - dist.mean) > 3 * dist.sd) {
	    //	    cerr << "too short/long\n";
	    continue;  // mate too short or too long
	  }

	  // add to list of intervals
	  if (globals.find("frgnoread") != globals.end()){
	    starts.push_back(unreadmin);
	    ends.push_back(unreadmax);
	  } else {
	    starts.push_back(min);
	    ends.push_back(max);
	  }
	} // if good matepair
      } // for each tile
    } else { // just the reads....easy
      for (vector<Tile_t>::iterator ti = tv.begin(); ti != tv.end(); ti++) {
	starts.push_back(ti->offset);
	ends.push_back(ti->offset + ti->getGappedLength());
	if (len < ti->offset + ti->getGappedLength())
	  len = ti->offset + ti->getGappedLength(); // adjust length if too short (e.g. layout)
      }
    }// if doFrg

    // here we actually compute the coverage and we're done!
    sort(starts.begin(), starts.end(), less<Pos_t>());
    sort(ends.begin(), ends.end(), less<Pos_t>());
    
    vector<Pos_t>::iterator si, ei; // iterators in start and end arrays
    si = starts.begin(); ei = ends.begin();
    int cvg = 0;    // cumulative coverage stat
    Pos_t last = 0; // last change in coverage

    while (si != starts.end() && ei != ends.end()){
      if (*si <= *ei) { // hit a start
	//if (*si != 0 || si != starts.begin())
	if (*si != last)
	  cout << last << "\t" << *si << "\t" << cvg << endl;
	cvg++;
	last = *si;
	si++;
      } else { // hit an end
	if (*ei != last)
	  cout << last << "\t" << *ei << "\t" << cvg << endl;
	cvg--;
	last = *ei;
	ei++;
      }
    }

    assert (si == starts.end());  // if tile is correct we have finished the starts
    
    while (ei != ends.end()) {
      if (*ei != last)
	cout << last << "\t" << *ei << "\t" << cvg << endl;
      cvg--;
      last = *ei;
      ei++;
    }
    
    if (last < len) // 0x coverage at end of contig
      cout << last << "\t" << len << "\t" << 0 << endl;
    
  } // while true

  // clean up  
  if (doCtg)
    contig_stream.close();
  if (doLay)
    layout_stream.close();
  if (doFrg) {
    fragment_bank.close();
    library_stream.close();
  }
  exit(0);
} // main
