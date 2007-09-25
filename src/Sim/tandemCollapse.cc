// $Revision$

#if HAVE_CONFIG_H
#include "config.h"
#endif

// C includes
#include <getopt.h>
#include <ctype.h>

// STL includes
#include <vector>
#include <map>
#include <fstream>
#include <queue>

// AMOS includes
#include "Layout_AMOS.hh"
#include "Contig_AMOS.hh"
#include "Bank_AMOS.hh"

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

#define endl "\n"    // die endl, die!

map <string, string> globals;  // global variables

void printHelpText()
{
  cout << 
    ".NAME." << endl <<
    "tandemCollapse" << endl << 
    endl <<
    "Program for simulating the effect of tandem repeat collapses on a read layout" << endl << 
    endl <<
    ".USAGE." << endl << 
    "tandemCollapse -b bank -c config" << endl << 
    endl <<
    ".OPTIONS." << endl <<
    "-b bank   - location of bank directory" << endl <<
    "-c config - config file that specifies locations of tandems" << endl <<
    "-m misassemblies.list - output location of mis-assemblies in final layout" << endl <<
    endl <<
    ".DESCRIPTION." << endl <<
    "This program takes a configuration file specifying an input layout" << endl <<
    "together with a description of a set of tandem repeats and generates " << endl <<
    "a new layout representing a mis-assembly of the original layout through" << endl <<
    "the collapse of the specified repeats." << endl <<
    endl <<
    "The format of the configuration file is:" << endl <<
    "CTG iid 7" << endl <<
    "ctg7 collapse " << endl <<
    "R 200 1200 4" << endl <<
    "R 8000 2000 2" << endl <<
    "..." << endl <<
    endl << 
    "The first line indicates the source of the layout: the contig (CTG)" << endl <<
    "or layout (LAY) identified by the corresponding iid or eid." << endl <<
    "The second line is the eid of the layout that will be generated." << endl << 
    "The remaining lines (prefixed by R) list information about the repeats" << endl <<
    "Specifically: start_coord  length num_copies" << endl <<
    endl <<
    "Note: in case repeats overlap, the behaviour of the program is unspecified" << endl <<
    endl <<
    ".KEYWORDS" << endl <<
    "Simulation" << endl
    ;

} // printHelpText

bool GetOptions(int argc, char ** argv)
{
  int option_index = 0;
  
  static struct option long_options[] = {
    {"b",    1, 0, 'b'},
    {"c",    1, 0, 'c'},
    {"m",    1, 0, 'm'},
    {"h",    0, 0, 'h'},
    {"help", 0, 0, 'h'},
    {0,0,0,0}
  };

  bool helpRequested = false;
  int c;

  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -
1){
    switch (c){
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case 'c':
      globals["conffile"] = string(optarg);
      break;
    case 'm':
      globals["misfile"] = string(optarg);
      break;
    case 'h':
      helpRequested = true;
      break;
    case '?':
      return false;
    }
  } // while c = getopt

  if (helpRequested) {
    printHelpText();
    exit(0);
  } // if help

  if (optind < argc) // leftover command line params.
    globals["input"] = string(argv[optind]);

} // GetOptions


// useful for sorting tile arrays
struct cmpTile : public binary_function<Tile_t, Tile_t, bool> {
  bool operator () (Tile_t x, Tile_t y)
  {
    if (x . offset < y . offset)
      return true;
    if (x . offset == y . offset && x . range . getLength () < y . range . getLength ())
        return true;
    return false;
  }
};

// useful for sorting pair arrays
template <class A, class B>
struct ltByFirst : public binary_function<pair<A, B>, pair<A, B>, bool> {
  bool operator () (pair<A, B> x, pair<A, B> y)
  {
    if (x . first < y . first)
      return true;
    return false;
  }
};

// useful for sorting pair arrays
template <class A, class B>
struct gtByFirst : public binary_function<pair<A, B>, pair<A, B>, bool> {
  bool operator () (pair<A, B> x, pair<A, B> y)
  {
    if (x . first > y . first)
      return true;
    return false;
  }
};

//-------------------------------------------

int main (int argc, char ** argv)
{

  if (!GetOptions(argc, argv)){
    cerr << "Command line parsing failed, use -h option for usage info" 
         << endl;
    //printHelpText();
    exit(1);
  }

  if (globals.find("conffile") == globals.end()){
    cerr << "Must provide a config file with option -c (see -h for more info)" << endl;
    exit(1);
  }

  ifstream conf_stream(globals["conffile"].c_str());
  if (! conf_stream.is_open()){
    cerr << "Could not open config file " << globals["conffile"] << endl;
    exit (1);
  }

  ofstream mis;
  if (globals.find("misfile") != globals.end()){
    mis.open(globals["misfile"].c_str());
    if (! mis.is_open()){
      cerr << "Could not open misassembly file " << globals["misfile"] << endl;
      exit(1);
    }
  }

  bool byIID = true;
  bool doCTG = true;
  ID_t iid = 0;
  string eid = "";
  string out_eid;
  string tmp;

  conf_stream >> tmp;
  if (tmp == "CTG") {
    doCTG = true;
    cout << "Will process contig ";
  } else if (tmp == "LAY") {
    doCTG = false;
    cout << "Will process layout ";
  } else {
    cerr << "Don't understand " << tmp << " on first line of configuration file" << endl;
    exit(1);
  }

  conf_stream >> tmp;
  
  if (tmp == "iid") {
    conf_stream >> iid;
    byIID = true;
    cout << "with iid " << iid << endl;
  } else if (tmp == "eid") {
    while (isspace(conf_stream.peek())) conf_stream.get();  // skip whitespace
    getline(conf_stream, eid);
    byIID = false;
    cout << "with eid " << eid << endl;
  } else {
    cerr << "Don't understand " << tmp << " on first line of configuration file" << endl;
    exit(1);
  }

  while (isspace(conf_stream.peek())) conf_stream.get();  // skip to next line
  getline(conf_stream, out_eid);

  cout << "Output layout eid is <" << out_eid << ">" << endl;

  vector<pair<Pos_t, Pos_t> > brkpoints;  // places where repeats were collapsed: coord, rep_len

  while (! conf_stream.eof()){
    Pos_t start, len;
    int ncopy;

    conf_stream >> tmp >> start >> len >> ncopy;

    if (conf_stream.eof())
      break;

    cerr << "Repeat " << start << " " << len << " " << ncopy << " implies breakpoints:" << endl;
    if (tmp != "R"){
      cerr << "Don't understand " << tmp << " in configuration file.  Repeat line expected" << endl;
      exit(1);
    }

    start += len;
    for (int i = 1; i < ncopy; i++, start += len) {
      brkpoints.push_back(pair<Pos_t, Pos_t> (start, len));
      cerr << start << "\t" << len << endl;
    }
  }
  conf_stream.close();

  // done reading configuration.  now it's time for the real thing.
  //
  // start by loading the vector of tiles from the selected contig/layout
  // go along the tile vector until we hit a breakpoint.
  // remove all tiles spanning the breakpoint
  // shift down (reduce offset) of subsequent tiles by cumulative length of repeats seen up to 
  // this point

  Bank_t contig_bank (Contig_t::NCODE);
  Bank_t layout_bank (Layout_t::NCODE);

  vector <Tile_t> tv; // tiles we'll use for our processing
  
  if (!doCTG) { // just layouts
    if (! layout_bank.exists(globals["bank"])) {
      cerr << "Layout bank doesn't exist in " << globals["bank"] << endl;
      exit(1);
    }
    try {
      layout_bank.open(globals["bank"], B_READ|B_WRITE); 
    } catch (Exception_t & e) {
      cerr << "Failed to open layout account in bank " << globals["bank"]
	   << ": " << endl << e << endl;
      exit(1);
    }

    Layout_t lay;
    if (byIID) {
      try {
	layout_bank.fetch(iid, lay);
      } catch (Exception_t & e) {
	cerr << "Could not fetch layout with iid" << iid
	     << ": " << endl << e << endl;
	exit(1);
      }
    } else { // byeid
      try {
	layout_bank.fetch(eid, lay);
      } catch (Exception_t & e) {
	cerr << "Could not fetch layout with eid" << eid
	     << ": " << endl << e << endl;
	exit(1);
      }
    }

    tv = lay.getTiling();
  } else {
    if (! contig_bank.exists(globals["bank"])) {
      cerr << "Contig bank doesn't exist in " << globals["bank"] << endl;
      exit(1);
    } 
    try {
      contig_bank.open(globals["bank"], B_READ|B_WRITE); 
    } catch (Exception_t & e) {
      cerr << "Failed to open contig account in bank " << globals["bank"]
	   << ": " << endl << e << endl;
      exit(1);
    }
    if (! layout_bank.exists(globals["bank"])){
      // must create it
      try {
	layout_bank.create(globals["bank"], B_WRITE);
      } catch (Exception_t & e) {
	cerr << "Failed to create layout account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }
    } else {
      try {
	layout_bank.open(globals["bank"], B_WRITE);
      } catch (Exception_t & e) {
	cerr << "Failed to open layout account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }
    } // if layout bank exists

    Contig_t ctg;
    if (byIID) {
      try {
	contig_bank.fetch(iid, ctg);
      } catch (Exception_t & e) {
	cerr << "Could not fetch contig with iid" << iid
	     << ": " << endl << e << endl;
	exit(1);
      }
    } else { // byeid
      try {
	contig_bank.fetch(eid, ctg);
      } catch (Exception_t & e) {
	cerr << "Could not fetch contig with eid" << eid
	     << ": " << endl << e << endl;
	exit(1);
      }
    }
    
    tv = ctg.getReadTiling();

    contig_bank.close(); // don't need this anymore
  } // if doCtg

  // sort tile vector by tile offset & breakpoints by coord
  sort(tv.begin(), tv.end(), cmpTile());
  sort(brkpoints.begin(), brkpoints.end(), ltByFirst<Pos_t, Pos_t> ());

  priority_queue <pair<Pos_t, int>, vector<pair<Pos_t, int> >, gtByFirst<Pos_t, int> > pq; // "active" tiles
  int nextBrk = 0;  // next breakpoint to be processed
  Pos_t adjust = 0; // shift tiles by this much
  vector<Tile_t> new_tv; // output tiles

  new_tv.reserve(tv.size()); // need at most this many tiles

  int ti = 0;
  int laststart = 0;
  while (ti < tv.size()) {
    if (nextBrk < brkpoints.size() && brkpoints[nextBrk].first < tv[ti].offset &&
	! pq.empty() && brkpoints[nextBrk].first < pq.top().first) {
      // pq had better not be empty at this point
      // clean up pq and move on
      cerr << "found breakpoint @" << brkpoints[nextBrk].first << endl;;
      while (! pq.empty())
	pq.pop();

      if (globals.find("misfile") != globals.end()){ // need to output breaks
	if (laststart == 0 || brkpoints[nextBrk].first - brkpoints[nextBrk].second - adjust > laststart){
	  laststart = brkpoints[nextBrk].first - brkpoints[nextBrk].second - adjust;
	  mis << laststart << " " << brkpoints[nextBrk].first - adjust << endl;
	}
	// otherwise this was a > 2 copy repeat
      }

      adjust += brkpoints[nextBrk].second;
      nextBrk++;
      continue;
    }
    if (pq.empty() || tv[ti].offset <= pq.top().first) { // add to pq
      cerr << "Adding to pq @" << tv[ti].offset << " " << tv[ti].source << endl;
      pq.push(pair<Pos_t, int> (tv[ti].offset + tv[ti].getGappedLength(), ti));
      ti++;
      continue;
    } 
    if (! pq.empty() && pq.top().first < tv[ti].offset) { // remove from pq
      Tile_t nt = tv[pq.top().second];

      cerr << "Removing from pq @" << pq.top().first << " " << nt.source << endl;

      nt.offset -= adjust;  // shift by current adjustment
      new_tv.push_back(nt); // add to output
      pq.pop();             // remove from queue
      continue;
    }
  } // while ti < tv.size();

  // now we just have to empty the queue
  while (! pq.empty()){
    if (nextBrk < brkpoints.size() && brkpoints[nextBrk].first < pq.top().first) 
      break; // nothing more to do here,this step would emtpy the queue anyway
    
    Tile_t nt = tv[pq.top().second];
      
    nt.offset -= adjust;  // shift by current adjustment
    new_tv.push_back(nt); // add to output
    pq.pop();
  } // while ! pq.empty()

  // now we need to generate a new layout
  Layout_t new_lay;
  ID_t newid = layout_bank.getMaxIID() + 1;

  new_lay.setTiling(new_tv);
  new_lay.setEID(out_eid);
  new_lay.setIID(newid);

  try {
    layout_bank.append(new_lay);
  } catch (Exception_t & e) {
    cerr << "Could not append new layout to bank iid=" << newid
	 << " eid=" << out_eid << " : " << endl << e << endl;
    exit(1);
  }
  
  layout_bank.close();  
  if (globals.find("misfile") != globals.end())
    mis.close();

  cerr << "Done\n";

  exit(0);
} // main
