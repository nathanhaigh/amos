// $Id$

// This program reads a set of contigs from a bank then reports 
// regions where mate-pair information indicates a problem

#include "foundation_AMOS.hh"

#include <getopt.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <math.h>
#include <functional>
#include <stdlib.h>

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

#define endl "\n"

enum MateStatus {MP_GOOD, MP_SHORT, MP_LONG, MP_NORMAL, MP_OUTIE, MP_SINGMATE, 
		 MP_LINKING};

class AnnotatedFragment: public Fragment_t
{
public:
  MateStatus status;
  Pos_t deviation;    // how far it deviates from mean
  float rank;         // rank in sorted library list
  Pos_t size;
  AnnotatedFragment() : Fragment_t() {
    status = MP_GOOD;
    deviation = 0;
    rank = 0;
  }
};

struct RankedRange {
  Pos_t left, right;
  float range;
};

struct cmpFrag :
  public binary_function<list<AnnotatedFragment>::iterator, list<AnnotatedFragment>::iterator, bool>
{
  bool operator () (list<AnnotatedFragment>::iterator a, list<AnnotatedFragment>::iterator b)
  {
    return a->size < b->size;
  }
};

struct pairCmp :
  public binary_function<pair<Pos_t, float>, pair<Pos_t, float>, bool>
{
  bool operator () (pair<Pos_t, float> a, pair<Pos_t, float> b)
  {
    return a.first < b.first;
  }
};

map<string, string> globals; // global variables

void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "asmQC - computes several mate-pair based statistics on an assembly\n"
    "\n.USAGE.\n"
    "asmQC -b[ank] <bank_name>\n"
    "\n.OPTIONS.\n"
    "-h, -help     print out help message\n"
    "-b, -bank     bank where assembly is stored\n"
    "-feat         write contig features into the bank\n"
    "-recompute    recompute library sizes\n"
    "-update       update bank with recomputed library sizes\n"
    "-minobs <n>   minimum number of good mate-pairs required to recompute\n"
    "              library sizes\n"
    "-numsd <n>    mate-pairs within <n> standard deviations of the mean\n"
    "              library size are considered good. <n> can be fractional\n"
    "-goodcvg <n>  report regions with good mate coverage less than <n>\n"
    "-shortcvg <n> report regions with short mate coverage greater than <n>\n"
    "-longcvg <n>  report regions with long mate coverage greater than <n>\n"
    "-samecvg <n>  report regions with coverage by mates with same orientation\n"
    "              greater than <n>\n"
    "-outiecvg <n> report regions with outie coverage greater than <n>\n"
    "-linking <n>  report regions with linking read coverage greater than <n>\n"
    "-singlemate <n> report regions with singleton mate coverage > than <n>\n"
    "-meachange <n> libraries whose mean changed by less than <n> will be considered unchanged\n"
    "-sdchange <n> libraries whose stdev. changed by less than <n> will be considered unchanged\n"
    "-debug        output status for each mate-pair to STDERR\n"
    "\n.DESCRIPTION.\n"
    "\n.KEYWORDS.\n"
    "AMOS bank, Validation, Mate pairs\n"
       << endl;
}

bool GetOptions(int argc, char ** argv)
{  
  int option_index = 0;
  static struct option long_options[] = {
    {"help",      0, 0, 'h'},
    {"h",         0, 0, 'h'},
    {"b",         1, 0, 'b'},
    {"bank",      1, 0, 'b'},
    {"minobs",    1, 0, 'm'},
    {"numsd",     1, 0, 'S'},
    {"goodcvg",   1, 0, 'g'},
    {"shortcvg",  1, 0, 's'},
    {"longcvg",   1, 0, 'l'},
    {"samecvg",   1, 0, 'n'},
    {"outiecvg",  1, 0, 'o'},
    {"linking",   1, 0, 'L'},
    {"singlemate", 1, 0, 'M'},
    {"recompute", 0, 0, 'r'},
    {"update",    0, 0, 'u'},
    {"feat",      0, 0, 'f'},
    {"debug",     0, 0, 'd'},
    {"meachange", 1, 0, 'c'},
    {"sdchange",  1, 0, 'C'},
    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -
         1){
    switch (c){
    case 'h':
      printHelpText();
      exit(0);
      break;
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case 'm':
      globals["minobs"] = string(optarg);
      break;
    case 'S':
      globals["numsd"] = string(optarg);
      break;
    case 'g':
      globals["goodcvg"] = string(optarg);
      break;
    case 's':
      globals["shortcvg"] = string(optarg);
      break;
    case 'l':
      globals["longcvg"] = string(optarg);
      break;
    case 'n':
      globals["normalcvg"] = string(optarg);
      break;
    case 'o':
      globals["outiecvg"] = string(optarg);
      break;
    case 'L':
      globals["linking"] = string(optarg);
      break;
    case 'M':
      globals["singlemate"] = string(optarg);
      break;
    case 'r':
      globals["recompute"] = string("true");
      break;
    case 'u':
      globals["update"] = string("true");
      break;
    case 'f':
      globals["feat"] = string("true");
      break;
    case 'd':
      globals["debug"] = string("true");
      break;
    case 'c':
      globals["meachange"] = string(optarg);
      break;
    case 'C':
      globals["sdchange"] = string(optarg);
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions


// returns the distance between the reads implied by the ranges within
// the contig of length Len.  Returns 0 if the reads are improperly
// oriented or if they are too close to the end of the contig (closer than End)
int mateLen (AnnotatedFragment & m, Range_t & a, Range_t & b, int Len, int End)
{
  int oriA;
  int oriB;

  if (a.getBegin() < a.getEnd()) 
    oriA = 1;
  else 
    oriA = -1;

  if (b.getBegin() < b.getEnd())
    oriB = 1;
  else
    oriB = -1;

  if (oriA == oriB){ // A --> B -->
    m.status = MP_NORMAL;
    return 0;
  }

  if (oriA == 1 && a.getBegin() > b.getBegin()){ // B <--  --> A
    m.status = MP_OUTIE;
    return 0;
  }

  if (oriA == -1 && a.getBegin() < b.getBegin()){ // A <-- --> B
    m.status = MP_OUTIE;
    return 0;
  }

  if (oriA == 1 && (b.getEnd() < a.getBegin() || a.getEnd() > b.getBegin())) {
    // B <-------
    // A   ------->
    m.status = MP_OUTIE;
    return 0;
  }

  if (oriA == -1 && (a.getEnd() < b.getBegin() || b.getEnd() > a.getBegin())) {
    // A <-------
    // B   ------->
    m.status = MP_OUTIE;
    return 0;
  }

  // now everything is kosher A --> <-- B or B --> <-- A
  if ((oriA == 1 && (Len - a.getBegin() < End))||
      (oriA == -1 && (a.getBegin() < End)) ||
      (oriB == 1 && (Len - b.getBegin() < End)) ||
      (oriB == -1 && (b.getBegin() < End))){
    return 0; // mate-pair too close to end to count
  }

  return abs(a.getBegin() - b.getBegin());
}// mateLen

// zscore for the Mann-Whitney test
float zscore (int nlib, float librank, int cov, float covrank){
  int na = nlib - cov;
  int nb = cov;

  float aranksum = librank - covrank;
  float branksum = covrank;

  float meana = na * (nlib + 1) / 2;
  float meanb = nb * (nlib + 1) / 2;

  float stdev = sqrt((float)na * nb * (nlib + 1) / 12);

  float zscore = (branksum - meanb > 0) ? (branksum - meanb + 0.5) : (branksum -meanb - 0.5);

  zscore /= stdev;

  return zscore;
} //zscore

//----------------------------------------------
int main(int argc, char **argv)
{
  if (! GetOptions(argc, argv)){
    cerr << "Command line parsing failed" << endl;
    printHelpText();
    exit(1);
  }

  // set up some global variables
  int MIN_OBS = 100;   // min. num. of observations required to change lib size
  float NUM_SD = 3.00; // num. of standard deviations within which mate is good
  int MIN_GOOD_CVG = 0; // good mate coverages below this will be reported
  int MAX_SHORT_CVG = 2; // short mate coverages over this will be reported
  int MAX_LONG_CVG = 2; // long mate coverages over this will be reported
  int MAX_NORMAL_CVG = 3; // normal orientation read coverages > will be reptd.
  int MAX_OUTIE_CVG = 3; // outie orientation read coverages > will be reptd.
  int MAX_LINKING_CVG = 3; // linking mate coverage > will be reported
  int MAX_SINGLEMATE_CVG = 3; // single mate coverage > will be reported
  int MEA_CHANGE = 50;// libraries that change by less are considered unchanged
  int SD_CHANGE = 10;// libraries that change by less are considered unchanged

  bool recompute = false; // recompute library sizes
  bool update = false;    // update library sizes in bank
  bool feats = false;     // add features to bank
  bool debug = false;     // write debugging info to STDERR

  if (globals.find("debug") != globals.end()){
    debug = true;
    cout << "Debugging mode: additional information written to STDERR" << endl;
  }

  if (globals.find("meanchange") != globals.end())
      MEA_CHANGE = strtol(globals["meanchange"].c_str(), NULL, 10);

  if (globals.find("sdchange") != globals.end())
      SD_CHANGE = strtol(globals["sdchange"].c_str(), NULL, 10);

  if (globals.find("recompute") != globals.end()){
    recompute = true;
    cout << "Will recompute library sizes" << endl;
    cout << "Libraries whose mean changes by less than " << MEA_CHANGE
	 << "\nand whose std. dev. changes by less than " << SD_CHANGE 
	 << " are considered to not have changed" << endl;
  } else
    cout << "Will not recompute library sizes" << endl;

  if (globals.find("update") != globals.end()){
    update = true;
    cout << "Will update libraries in bank" << endl;
  } else
    cout << "Will not update libraries in bank" << endl;

  if (globals.find("feat") != globals.end()){
    feats = true;
    cout << "Will write features in bank" << endl;
  } else 
    cout << "Will not write features in bank" << endl;

  if (recompute){
    if (globals.find("minobs") != globals.end())
      MIN_OBS = strtol(globals["minobs"].c_str(), NULL, 10);
    cout << "Minimum number of observations required to change library size: " 
	 << MIN_OBS << endl;
  }

  if (globals.find("numsd") != globals.end())
    NUM_SD = (float)strtod(globals["numsd"].c_str(), NULL);
  cout << "Mates within " << NUM_SD 
       << " standard deviations from mean considered good" << endl;

  if (globals.find("goodcvg") != globals.end())
    MIN_GOOD_CVG = strtol(globals["goodcvg"].c_str(), NULL, 10);
  cout << "Reporting regions with less than " << MIN_GOOD_CVG
       << " good mate coverage" << endl;

  if (globals.find("shortcvg") != globals.end())
    MAX_SHORT_CVG = strtol(globals["shortcvg"].c_str(), NULL, 10);
  cout << "Reporting regions with more than " << MAX_SHORT_CVG
       << " short mate coverage" << endl;

  if (globals.find("longcvg") != globals.end())
    MAX_LONG_CVG = strtol(globals["longcvg"].c_str(), NULL, 10);
  cout << "Reporting regions with more than " << MAX_LONG_CVG
       << " long mate coverage" << endl;

  if (globals.find("normalcvg") != globals.end())
    MAX_NORMAL_CVG = strtol(globals["normalcvg"].c_str(), NULL, 10);
  cout << "Reporting regions with more than " << MAX_NORMAL_CVG 
       << " coverage by mates oriented the same way" << endl;

  if (globals.find("outiecvg") != globals.end())
    MAX_OUTIE_CVG = strtol(globals["outiecvg"].c_str(), NULL, 10);
  cout << "Reporting regions with more than " << MAX_OUTIE_CVG
       << " coverage by outie mates" << endl;

  if (globals.find("outiecvg") != globals.end())
    MAX_OUTIE_CVG = strtol(globals["outiecvg"].c_str(), NULL, 10);
  cout << "Reporting regions with more than " << MAX_OUTIE_CVG
       << " coverage by outie mates" << endl;

  if (globals.find("linking") != globals.end())
    MAX_LINKING_CVG = strtol(globals["linking"].c_str(), NULL, 10);
  cout << "Reporting regions with more than " << MAX_LINKING_CVG
       << " coverage by linking mates" << endl;

  if (globals.find("singlemate") != globals.end())
    MAX_SINGLEMATE_CVG = strtol(globals["singlemate"].c_str(), NULL, 10);
  cout << "Reporting regions with more than " << MAX_SINGLEMATE_CVG
       << " coverage by singleton mates" << endl;

  cout << endl;

  // open necessary files
  if (globals.find("bank") == globals.end()){ // no bank was specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  // prepare banks we'll need
  
  Bank_t library_bank (Library_t::NCODE);
  if (! library_bank.exists(globals["bank"])){
    cerr << "No library account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    if (update)
      library_bank.open(globals["bank"], B_READ|B_WRITE);
    else
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
    contig_stream.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open contig account in bank " << globals["bank"] 
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

  BankStream_t frag_bank (Fragment_t::NCODE);
  if (! frag_bank.exists(globals["bank"])){
    cerr << "No frag account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    frag_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open frag account in bank " << globals["bank"] 
           << ": " << endl << e << endl;
      exit(1);
    }


  // Stream through the contigs and retrieve map of read positions within ctgs.
  Contig_t ctg;
  
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2ctg;     // map from read to contig
  hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > rd2posn; // position in contig
  hash_map<ID_t, Size_t, hash<ID_t>, equal_to<ID_t> > ctglen;   // length of contig
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > ctgname;   // name of contig

  set<ID_t> ctgIDs;
  while (contig_stream >> ctg)
    for (vector<Tile_t>::iterator ti = ctg.getReadTiling().begin(); 
         ti != ctg.getReadTiling().end(); ti++){
      rd2ctg[ti->source] = ctg.getIID();
      ctgIDs.insert(ctg.getIID());
      Pos_t f, l; // coords of beginning/end of read
      if (ti->range.getEnd() < ti->range.getBegin()) { // reverse
	f = ti->offset + ti->range.getBegin() - ti->range.getEnd() + 
	  ti->gaps.size();
	l = ti->offset;
      } else {
	f = ti->offset;
	l = ti->offset + ti->range.getEnd() - ti->range.getBegin() + 
	  ti->gaps.size();
      }
      rd2posn[ti->source] = 
        Range_t(f, l);

      ctglen[ctg.getIID()] = ctg.getLength();
      ctgname[ctg.getIID()] = ctg.getEID();
    }

  contig_stream.close();

  AnnotatedFragment frag;
  pair<ID_t, ID_t> mtp;
  list<AnnotatedFragment> mtl;
  Read_t rd1, rd2;
  set<ID_t> libIDs;
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2lib;
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2frg;
  hash_map<ID_t, list<list<AnnotatedFragment>::iterator>, hash<ID_t>, equal_to<ID_t> > lib2frag;
  hash_map<ID_t, list<list<AnnotatedFragment>::iterator>, hash<ID_t>, equal_to<ID_t> > ctg2frag;
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > rd2name;

  while (frag_bank >> frag){
    frag.status = MP_GOOD;
    mtp = frag.getMatePair();

    if (mtp.first != 0) 
      read_bank.fetch(mtp.first, rd1); // get the read
    if (mtp.second != 0)
      read_bank.fetch(mtp.second, rd2);

    if (mtp.first == 0 || mtp.second == 0) {
//       if (perinsert)
// 	insertFile << "UNMATED: "
// 		   << ((mtp.first != 0) ? mtp.first : mtp.second)
// 		   << "(" << ((mtp.first != 0) ? rd1.getEID() : rd2.getEID())
// 		   << ") " << endl;
      continue;
    }

    if (frag.getType() != Fragment_t::INSERT) {// we only handle end reads
      //      cerr << "Type is " << frag.getType() << endl;
      //      if (perinsert)
// 	insertFile << "NOT END MATES: "
// 		   << mtp.first 
// 		   << "(" << rd1.getEID() << ") " 
// 		   << mtp.second 
// 		   << "(" << rd2.getEID() << ") "
// 		   << "\n";
      continue;
    }


    rd2name[mtp.first] = rd1.getEID();
    rd2name[mtp.second] = rd2.getEID();
    //    rd2frg[mtp.first] = frag.getIID();
    //    rd2frg[mtp.second] = frag.getIID();
    rd2lib[mtp.first] = frag.getLibrary();
    rd2lib[mtp.second] = frag.getLibrary();
    libIDs.insert(frag.getLibrary());

    if (frag.getType() != Fragment_t::INSERT) {// we only handle end reads
      //      cerr << "Type is " << frag.getType() << endl;
      continue;
    }

    list<AnnotatedFragment>::iterator ti; 
    if (rd2ctg[mtp.first] == rd2ctg[mtp.second]){
      // mate-pair between reads within the same contig

      //      ctgIDs.insert(rd2ctg[mtp.first]);
      ti = mtl.insert(mtl.end(), frag);

      lib2frag[frag.getLibrary()].push_back(ti);
      ctg2frag[rd2ctg[mtp.first]].push_back(ti);
    } else {
      if (rd2ctg[mtp.first] == 0 ||
	  rd2ctg[mtp.second] == 0) { // mate should be in contig
	frag.status = MP_SINGMATE;
	ti = mtl.insert(mtl.end(), frag);
	lib2frag[frag.getLibrary()].push_back(ti);
	ctg2frag[rd2ctg[mtp.first]].push_back(ti);
      } else {
	frag.status = MP_LINKING;
	ti = mtl.insert(mtl.end(), frag);
	lib2frag[frag.getLibrary()].push_back(ti);
	ctg2frag[rd2ctg[mtp.first]].push_back(ti);
      }
    }
  }
  
  // now we get the library information for each library
  hash_map<ID_t, pair<Pos_t, SD_t>, hash<ID_t>, equal_to<ID_t> > lib2size;
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > lib2name;
  hash_map<ID_t, pair<int, float>, hash<ID_t>, equal_to<ID_t> > lib2rank;
  Library_t lib;
  for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
    library_bank.fetch(*li, lib);
    lib2name[*li] = lib.getEID();
    lib2size[*li] = pair<Pos_t, SD_t> (lib.getDistribution().mean, 
				       lib.getDistribution().sd);
    lib2rank[*li] = pair<int, float>(0, 0.0);
  }
  
  // now we're ready to figure out new library sizes
  hash_map<ID_t, pair<Pos_t, SD_t>, hash<ID_t>, equal_to<ID_t> > NewLib2size;
  
  for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
    // for each library
    vector<list<AnnotatedFragment>::iterator> sizes;
    pair<Pos_t, SD_t> libsize = lib2size[*li];
    
    for (list<list<AnnotatedFragment>::iterator>::iterator 
	   mi = lib2frag[*li].begin(); mi != lib2frag[*li].end(); mi++){
      if ((*mi)->status != MP_GOOD)
	continue;
      // for each mate pair
      Pos_t sz = mateLen(**mi, rd2posn[(*mi)->getMatePair().first], 
			 rd2posn[(*mi)->getMatePair().second], 
			 ctglen[rd2ctg[(*mi)->getMatePair().first]], 
			 libsize.first + 3 * libsize.second);

      (*mi)->size = sz;

      sizes.push_back(*mi);
    } // if recomputing library sizes

    // sort mates within library and assign ranks
    sort(sizes.begin(), sizes.end(), cmpFrag());
    int lastidx = 0;
    Pos_t lastsize = sizes[0]->size;
    float rank;
    for (int i = 1; i < sizes.size(); i++){
      if (sizes[i]->size != lastsize){
	rank = 1.0 * (i + lastidx - 1) / 2.0;
	for (int j = lastidx; j < i; j++)
	  sizes[j]->rank = rank;
	lastidx = i;
	lastsize = sizes[i]->size;
      }
    }
    rank = 1.0 * (sizes.size() + lastidx - 1) / 2.0;
    for (int j = lastidx; j < sizes.size(); j++)
      sizes[j]->rank = rank;

    float ranksum = 1.0 * sizes.size() * (sizes.size() + 1) / 2.0;
    lib2rank[*li] = pair<int, float> (sizes.size(), ranksum);
  } // for each library

  for (set<ID_t>::iterator ctg = ctgIDs.begin(); ctg != ctgIDs.end(); ctg++) {

    list<pair<Pos_t, Pos_t> > ranges;  // ranges we are interested in
    list<pair<Pos_t, Pos_t> > interest;

    cout << ">Contig_" << *ctg << " " << ctgname[*ctg] << " " 
	 << ctglen[*ctg] << " bases" << endl;

    Contig_t newcontig;

    hash_map<ID_t, list<list<AnnotatedFragment>::iterator>, hash<ID_t>, equal_to<ID_t> > byLib;


    // Group good mate pairs by library
    for (list<list<AnnotatedFragment>::iterator>::iterator 
	   mi = ctg2frag[*ctg].begin(); mi != ctg2frag[*ctg].end(); mi++){

      if ((*mi)->status == MP_GOOD)
	byLib[(*mi)->getLibrary()].push_back(*mi);
    }


    // Process them by library
    for (hash_map<ID_t, list<list<AnnotatedFragment>::iterator>, hash<ID_t>, equal_to<ID_t> >::iterator bl = byLib.begin(); bl != byLib.end(); bl++){
      
      vector<pair<Pos_t, float> > lefts, rights;
      
      cout << "#Lib " << bl->first << endl;

      for (list<list<AnnotatedFragment>::iterator>::iterator
	     mi = bl->second.begin(); mi != bl->second.end(); mi++){
	
	if ((*mi)->status == MP_GOOD){
	  Pos_t left, right;
	  float rank = (*mi)->rank;
	  left = rd2posn[((*mi)->getMatePair()).first].getBegin(); 
	  right = rd2posn[((*mi)->getMatePair()).second].getBegin(); 

	  if (left < right) {
	    lefts.push_back(pair<Pos_t, float>(left, rank));
	    rights.push_back(pair<Pos_t, float>(right, rank));
	  } else {
	    lefts.push_back(pair<Pos_t, float>(right, rank));
	    rights.push_back(pair<Pos_t, float>(left, rank));
	  }
	} // if good mate
      }	// for each mate
      sort(lefts.begin(), lefts.end(), pairCmp());
      sort(rights.begin(), rights.end(), pairCmp());
      
      vector<pair<Pos_t, float> >::iterator si, ei;
      
      si = lefts.begin(); ei = rights.begin();
      int cvg = 0;
      Pos_t s = 0, e = ctglen[*ctg];
      float ranksum = 0;
      float lastbadZ = 0.0;
      bool inbad = false;
      
      while (si != lefts.end() && ei != rights.end()){
	if (si->first <= ei->first){
	  // beginning of interval
	  cvg++;
	  ranksum += si->second;
	  if (cvg > 5){ // below 5 it doesn't make sense
	    float z = zscore(lib2rank[bl->first].first, 
			     lib2rank[bl->first].second, cvg, ranksum);
	    if (fabs(z) >= 2){
	      if (! inbad){
		s = si->first;
		inbad = true;
		lastbadZ = z;
	      } else {
		if (fabs(z) > fabs(lastbadZ)){
		  lastbadZ = z;
		}
	      }
	    } else if (fabs(z) < 2 && inbad){
	      cout << s << " BAD Z=" << lastbadZ << " " << si->first << endl;
	      inbad = false;
	    }
	  } else if (inbad){
	    cout << s << " BAD Z=" << lastbadZ << " " << si->first << endl;
	    inbad = false;
	  }
	  si++;
	} else {
	  // end of interval
	  cvg--;
	  ranksum -= ei->second;
	  if (cvg > 5) {
	    float z = zscore(lib2rank[bl->first].first, 
			     lib2rank[bl->first].second, cvg, ranksum);
	    if (fabs(z) >= 2){ 
	      if (! inbad){
		s = ei->first;
		lastbadZ = z;
		inbad = true;
	      } else {
		if (fabs(z) > fabs(lastbadZ)){
		  lastbadZ = z;
		}
	      }
	    } else if (fabs(z) < 2 && inbad){
	      cout << s << " BAD Z=" << lastbadZ << " " << ei->first << endl;
	      inbad = false;
	    }
	  } else if (inbad) {
	    cout << s << " BAD Z=" << lastbadZ << " " << ei->first << endl;
	    inbad = false;
	  }
	  ei++;
	}
      }
	
      assert(si == lefts.end());
      
      while (ei != rights.end()){
	cvg--;
	ranksum -= ei->second;
	if (cvg > 5) {
	  float z = zscore(lib2rank[bl->first].first, 
			   lib2rank[bl->first].second, cvg, ranksum);
	  if (fabs(z) >= 2){
	    if (! inbad){
	      s = ei->first;
	      inbad = true;
	      lastbadZ = z;
	    } else {
	      if (fabs(z) > fabs(lastbadZ)){
		lastbadZ = z;
	      }
	    }
	  } else if (fabs(z) < 2 && inbad){
	    cout << s << " BAD Z=" << lastbadZ << " " << ei->first << endl;
	    inbad = false;
	  }
	} else if (inbad) {
	  cout << s << " BAD Z=" << lastbadZ << " " << ei->first << endl;
	  inbad = false;
	}
	
	ei++;
      }
      //      } // for each mate in library in contig
    } // for each library in contig
  } // for each contig

  frag_bank.close();
  read_bank.close();
  library_bank.close();

  return(0);
} // main
    
