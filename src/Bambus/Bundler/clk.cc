// $Id$ 

// This program uses DST, MTP, and CTG records from a bank in order to generate
// a set of contig links (CLK).

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>
#include <map>
#include <set>
#include <list>
#include <functional>
#include "foundation_AMOS.hh"

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

map<string, string> globals; // global variables

void printHelpText()
{
  cerr << 
    "\n"
    "USAGE:\n"
    "\n"
    "clk -b[ank] <bank_name>\n"
       << endl;
}


bool GetOptions(int argc, char ** argv)
{  
  int option_index = 0;
  static struct option long_options[] = {
    {"help",  0, 0, 'h'},
    {"h",     0, 0, 'h'},
    {"b",     1, 0, 'b'},
    {"bank",  1, 0, 'b'},
    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -
	 1){
    switch (c){
    case 'h':
      printHelpText();
      break;
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions



// --------------------------------------------------------------------
// --------------------------------------------------------------------
int main(int argc, char **argv)
{
  ID_t LinkId = 0; // link IIDs

  if (!GetOptions(argc, argv)){
    cerr << "Command line parsing failed" << endl;
    printHelpText();
    exit(1);
  }

  if (globals.find("bank") == globals.end()){ // no bank was specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  Bank_t library_bank (Library_t::NCODE);
  if (! library_bank.exists(globals["bank"])){
    cerr << "No library account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    library_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open library account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }
  
  Bank_t contig_bank (Contig_t::NCODE);
  BankStream_t contig_stream (Contig_t::NCODE);
  if (! contig_bank.exists(globals["bank"])){
    cerr << "No contig account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    contig_bank.open(globals["bank"], B_READ);
    contig_stream.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open contig account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }

  BankStream_t mate_bank (Matepair_t::NCODE);
  if (! mate_bank.exists(globals["bank"])){
    cerr << "No mate account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    mate_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open mate account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }

  Bank_t read_bank (Read_t::NCODE);
  if (! read_bank.exists(globals["bank"])){
    cerr << "No read account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    read_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open read account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }


  Bank_t frag_bank (Fragment_t::NCODE);
  if (! frag_bank.exists(globals["bank"])){
    cerr << "No fragment account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    frag_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open fragment account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }


  BankStream_t link_bank (ContigLink_t::NCODE);
  try {
    if (! link_bank.exists(globals["bank"]))
      link_bank.create(globals["bank"]);
    else 
      link_bank.open(globals["bank"]);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open link account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }
  
  // stream through contigs and build map of read id to contig id
  // stream through mates and retain those that link two different contigs
  // for one read in each of the mates retrieve the set of fragments
  // for all chosen fragments find the libraries
  // for all chosen libraries find the mean and standard deviation
  // for all mate pairs between same pair of contigs, retrieve the exact mapping of the reads in the contigs and generate contig links
  // repeat until exhausting set of mate pairs
  
  Contig_t ctg;
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2ctg;     // map from read to contig
  hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > rd2posn; // position in contig
  hash_map<ID_t, Size_t, hash<ID_t>, equal_to<ID_t> > ctglen;   // length of contig

  while (contig_stream >> ctg)
    for (vector<Tile_t>::iterator ti = ctg.getReadTiling().begin(); 
	 ti != ctg.getReadTiling().end(); ti++){
      rd2ctg[ti->source] = ctg.getIID();
      rd2posn[ti->source] = 
	Range_t(ti->offset, ti->offset + ti->range.end - ti->range.begin);
      ctglen[ctg.getIID()] = ctg.getLength();
    }
      //      cerr << "Read " << ti->source << " lives in contig " << ctg.getIID() << endl;;
  
  Matepair_t mtp;
  list<Matepair_t> mtl;
  Read_t rd1;
  Fragment_t frg;
  set<ID_t> libIDs;
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2lib;
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2frg;
  while (mate_bank >> mtp)
    if (rd2ctg[mtp.getReads().first] != rd2ctg[mtp.getReads().second]){
      // keep this matepair as it links two different contigs
      mtl.push_back(mtp);
      read_bank.fetch(mtp.getReads().first, rd1); // get the read
      frag_bank.fetch(rd1.getFragment(), frg); // get the fragment
      rd2frg[mtp.getReads().first] = rd1.getFragment();
      libIDs.insert(frg.getLibrary());
      rd2lib[mtp.getReads().first] = frg.getLibrary();
      //      cerr << "linking " << rd2ctg[mtp.getReads().first] << " and " << rd2ctg[mtp.getReads().second] << endl;
    }
  
  // now we get the library information for each library
  hash_map<ID_t, pair<Pos_t, SD_t>, hash<ID_t>, equal_to<ID_t> > lib2size;
  Library_t lib;
  for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
    library_bank.fetch(*li, lib);
    lib2size[*li] = pair<Pos_t, SD_t> (lib.getDistribution().mean, lib.getDistribution().sd);
  }


  // now we are ready to make up contig links
  ContigLink_t ctl;
  for (list<Matepair_t>::iterator mi = mtl.begin(); mi != mtl.end(); mi++){
    ID_t 
      rdA = mi->getReads().first, 
      rdB = mi->getReads().second;

    ID_t libId = rd2lib[rdA];

    ID_t 
      ctgA = rd2ctg[rdA],
      ctgB = rd2ctg[rdB];

    Size_t 
      lenA = ctglen[ctgA],
      lenB = ctglen[ctgB];

    Range_t 
      rangeA = rd2posn[rdA],
      rangeB = rd2posn[rdB];

    bool
      forwA = false,
      forwB = false;  // orientation of reads in contigs

    Size_t 
      adjA,
      adjB;           // distance from 5' read end to contig end closest to middle of clone

    if (rangeA.getBegin() > rangeA.getEnd()){
      forwA = false;
      adjA = rangeA.getBegin();
    } else {
      forwA = true;
      adjA = lenA - rangeA.getBegin();
    }

    if (rangeB.getBegin() > rangeB.getEnd()){
      forwB = false;
      adjB = rangeB.getBegin();
    } else {
      forwB = true;
      adjB = lenB - rangeB.getBegin();
    }

    ctl.setType(ContigLink_t::MATEPAIR);
    ctl.setIID(++LinkId);
    ctl.setContigs(pair<ID_t, ID_t>(ctgA, ctgB));
    ctl.setSize(lib2size[libId].first - adjA - adjB);
    ctl.setSD(lib2size[libId].second);
    ctl.setSource(pair<ID_t, NCode_t>(rd2frg[rdA], Fragment_t::NCODE));


    if (forwA && ! forwB)
      ctl.setAdjacency(ContigLink_t::NORMAL);
    if (forwA && forwB)
      ctl.setAdjacency(ContigLink_t::INNIE);
    if (! forwA && ! forwB)
      ctl.setAdjacency(ContigLink_t::OUTIE);
    if (! forwA && forwB)
      ctl.setAdjacency(ContigLink_t::ANTINORMAL);
    
    try {
      link_bank << ctl;
      cerr << "Adding link " << LinkId << endl;
    } catch (Exception_t & e)
      {
	cerr << "Failed to append link " << LinkId << " to bank " << globals["bank"] 
	     << ": " << endl << e << endl;
	exit(1);
      }
  }

  link_bank.close();
  frag_bank.close();
  library_bank.close();
  contig_bank.close();
  mate_bank.close();

  exit(0);
} // main
