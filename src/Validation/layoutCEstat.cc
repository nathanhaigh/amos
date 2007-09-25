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

#define MIN_CE_OBS 3    // minimum number of clones required to report anomalous CE numbers
#define MAX_DEVIATION 5 // clones longer/shorter than this many standard deviations from mean are ignored in CE computation


enum MateStatus {MP_GOOD, MP_SHORT, MP_LONG, MP_NORMAL, MP_OUTIE, MP_SINGMATE, 
		 MP_LINKING, MP_SPANNING};

class AnnotatedFragment: public Fragment_t
{
public:
  MateStatus status;
  Pos_t size;    // computed fragment size
  ID_t contig1, contig2;
  ID_t scaff1, scaff2;
  AnnotatedFragment() : Fragment_t() {
    status = MP_GOOD;
    size = 0;
    contig1 = 0; contig2 = 0; scaff1 = 0; scaff2 = 0;
  }
};


map<string, string> globals; // global variables

void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "layoutCEstat - computes several variants of the CE statistic on layouts\n"
    "\n.USAGE.\n"
    "asmQC -b[ank] <bank_name>\n"
    "\n.OPTIONS.\n"
    "-h, -help     print out help message\n"
    "-b, -bank     bank where assembly is stored\n"
    "-ttest        use observed stdev\n"
    "-ztest        use library stdev\n"
    "-eid <eid>    run only on <eid>\n"
    "-iid <iid>    run only on <iid>\n"
    "-ceplot <file> file to output C/E statistic plot to\n"
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
    {"ttest",     0, 0, 't'},
    {"ztest",     0, 0, 'z'},
    {"ceplot",    1, 0, 'c'},
    {"eid",       1, 0, 'e'},
    {"iid",       1, 0, 'i'},
    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1){
    switch (c){
    case 'h':
      printHelpText();
      exit(0);
      break;
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case 't':
      globals["ttest"] = "true";
      break;
    case 'z':
      globals["ztest"] = "true";
      break;
    case 'c':
      globals["ceplot"] = string(optarg);
      break;
    case 'e':
      globals["eid"] = string(optarg);
      break;
    case 'i':
      globals["iid"] = string(optarg);
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

  m.size = abs(a.getBegin() - b.getBegin());
  return m.size;
}// mateLen


// computes mean and standard deviation for a set of observations
// observations outside of 5 standard deviations are excluded
pair<Pos_t, SD_t> getSz(list<Pos_t> & sizes)
{
  int numObs = sizes.size();
  if (numObs == 0) return pair<Pos_t, SD_t> (0, 0); 
  // this should probably be an assert...

  Pos_t mean = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    mean += *li;
  }
  mean = (Pos_t) rint (mean * 1.0 / numObs);

  SD_t stdev = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    stdev += (*li - mean) * (*li - mean);
  }
  stdev = (SD_t) rint(sqrt(stdev * 1.0 / (numObs - 1)));
  
  Pos_t origm = mean;
  SD_t origs = stdev;

  // recompute mean and stdev for just sizes within 5SD of mean

  mean = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    if (abs(*li - origm) < MAX_DEVIATION * origs)
      mean += *li;
    else 
      numObs--;
  }

  if (numObs == 0) return pair<Pos_t, SD_t> (0, 0);
  // this should also be an assert

  mean = (Pos_t) rint (mean * 1.0 / numObs);
 
  stdev = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    if (abs(*li - origm) < MAX_DEVIATION * origs)
      stdev += (*li - mean) * (*li - mean);
  }

  stdev = (SD_t) rint(sqrt(stdev * 1.0 / (numObs - 1)));

  return pair<Pos_t, SD_t>(mean, stdev);
}

// get regions that are below or above a specified coverage
void getCvg(list<list<AnnotatedFragment>::iterator>::iterator begin,
	    list<list<AnnotatedFragment>::iterator>::iterator end,
	    hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > & posmap,
	    MateStatus status,
	    ID_t id, 
	    list<pair<Pos_t, Pos_t> > & interest,
	    Pos_t ctglen, int coverage, bool above)
{
  vector<Pos_t> starts, ends;
  list<pair<Pos_t, Pos_t> > ranges;
  interest.clear();
  
  for (list<list<AnnotatedFragment>::iterator>::iterator mi = begin; mi != end; mi++){
      if ((*mi)->status == status){
	Pos_t left, right;

	if (status == MP_GOOD){
	  left = posmap[((*mi)->getMatePair()).first].getEnd(); // use end of reads
	  right = posmap[((*mi)->getMatePair()).second].getEnd(); 
	} else if (status == MP_LONG || status == MP_SHORT) {
	  left = posmap[((*mi)->getMatePair()).first].getBegin(); // use beginning of reads
	  right = posmap[((*mi)->getMatePair()).second].getBegin(); 
	} else if (status == MP_NORMAL || status == MP_OUTIE) {
	  left = posmap[((*mi)->getMatePair()).first].getBegin(); 
	  right = posmap[((*mi)->getMatePair()).first].getEnd(); 
	  left = posmap[((*mi)->getMatePair()).second].getBegin(); 
	  right = posmap[((*mi)->getMatePair()).second].getEnd(); 
	} else if (status == MP_SINGMATE) {
	  if ((*mi)->contig1 != 0) {
	    left = posmap[((*mi)->getMatePair()).first].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).first].getEnd(); 
	  } else {
	    left = posmap[((*mi)->getMatePair()).second].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).second].getEnd(); 
	  }
	} else if (status == MP_SPANNING){ // check contig ids
	  if ((*mi)->contig1 == id) {
	    left = posmap[((*mi)->getMatePair()).first].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).first].getEnd(); 
	  } else {
	    left = posmap[((*mi)->getMatePair()).second].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).second].getEnd(); 
	  }
	} else if (status = MP_LINKING) { // check scaffold ids
	  if ((*mi)->scaff1 == id) {
	    left = posmap[((*mi)->getMatePair()).first].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).first].getEnd(); 
	  } else if ((*mi)->scaff2 == id ) {
	    left = posmap[((*mi)->getMatePair()).second].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).second].getEnd(); 
	  } else if ((*mi)->scaff1 == 0 && (*mi)->contig1 == id) {
	    left = posmap[((*mi)->getMatePair()).first].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).first].getEnd(); 
	  } else if ((*mi)->scaff2 == 0 && (*mi)->contig2 == id) {
	    left = posmap[((*mi)->getMatePair()).second].getBegin(); 
	    right = posmap[((*mi)->getMatePair()).second].getEnd(); 
	  } else {
	    cerr << "One of the IDs should match\n";
	    assert(0);
	  }
	}

	if (left < right)
	  ranges.push_back(pair<Pos_t, Pos_t>(left, right));
	else
	  ranges.push_back(pair<Pos_t, Pos_t>(right, left));
      }
  }

  if (ranges.size() == 0 && above) return;
  if (ranges.size() == 0 && coverage >= 0) {
    // all contig is below needed coverage
    interest.push_back(pair<Pos_t, Pos_t>(0, ctglen));
    return;
  }

  for (list<pair<Pos_t, Pos_t> >::iterator li = ranges.begin(); 
       li != ranges.end(); li++){
    //    cout << "Adding " << li->first << ", " << li->second << endl;
    starts.push_back(li->first);
    ends.push_back(li->second);
  }

  sort(starts.begin(), starts.end(), less<Pos_t>());
  sort(ends.begin(), ends.end(), less<Pos_t>());

  vector<Pos_t>::iterator si, ei;
  
  si = starts.begin(); ei = ends.begin();
  int cvg = 0;
  Pos_t s = 0, e = ctglen;
  while (si != starts.end() && ei != ends.end()){
    if (*si <= *ei){
      if (! above && cvg == coverage){
	e = *si;
	interest.push_back(pair<Pos_t, Pos_t>(s, e));
      }
      cvg++;
      if (above && cvg == coverage)
	s = *si;
      si++;
    } else {
      if (above && cvg == coverage) {
	e = *ei;
	interest.push_back(pair<Pos_t, Pos_t>(s, e));
      }
      cvg--;
      if (! above && cvg == coverage)
	s = *ei;
      ei++;
    }
  }
  
  assert(si == starts.end());
  while (ei != ends.end()){
    if (above && cvg == coverage){
      e = *ei;
      interest.push_back(pair<Pos_t, Pos_t>(s, e));
    }
    cvg--;
    if (! above && cvg == coverage)
      s = *ei;
    ei++;
  }
  if (! above && s != ctglen)
    interest.push_back(pair<Pos_t, Pos_t>(s, ctglen));
  
  return;
} // getCvg


// computes mean and standard deviation of array of tiles
void getMeanSD(vector<pair<Pos_t, Pos_t> >::iterator beg, vector<pair<Pos_t, Pos_t> >::iterator end, 
	       Size_t &mea, SD_t &sd)
{
  int n = 0;
  Size_t sum = 0;
  SD_t var = 0;

  for (vector<pair<Pos_t, Pos_t> >::iterator vi = beg; vi != end; vi++){
    n++;
    //    cout << "Adding " << vi->second - vi->first << " " << vi->first << ", " << vi->second << endl;
    sum += vi->second - vi->first;
  }
  if (n == 0) { return;}
  mea = sum / n;
  //  cout << "N " << n << endl;
  //  cout << "Mean " << mea << endl;
  for (vector<pair<Pos_t, Pos_t> >::iterator vi = beg; vi != end; vi++)
    var += (vi->second - vi->first - mea) * (vi->second - vi->first - mea);
  
  //  cout << "Var " << var << endl;
  if (n > 1) { 
    sd = var / (n - 1);
    sd = (SD_t) sqrt((double)sd);
  } else {
    sd = 0;
  }
  //  cout << "SD " << sd << endl;
}

// compares two pairs by first item (less than)
struct LessByFirst : public binary_function<pair<Pos_t, Pos_t>, pair<Pos_t, Pos_t>, bool> {
  bool operator () (pair<Pos_t, Pos_t> x, pair<Pos_t, Pos_t> y)
  {
    return (x.first < y.first);
  }
};

// compares two pairs by second item (greater than)
struct GtBySecond : public binary_function<pair<Pos_t, Pos_t>, pair<Pos_t, Pos_t>, bool> {
  bool operator () (pair<Pos_t, Pos_t> x, pair<Pos_t, Pos_t> y)
  {
    return (x.second > y.second);
  }
};

// computes regions with anomalous C/E stat
void getCEstat(list<list<AnnotatedFragment>::iterator>::iterator begin,
	       list<list<AnnotatedFragment>::iterator>::iterator end,
	       hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > & posmap,
	       ID_t libid, pair<Size_t, SD_t> & libifo, float num_sd, 
	       list<pair<Pos_t, Pos_t> > & interest, 
	       list<bool> & stretch,
	       ofstream & ceplotFile)
{
  vector<pair<Pos_t, Pos_t> > starts, ends;
  Pos_t pos, st;
  Size_t mea;         // sum of sizes of inserts
  SD_t sderr;         // standard error for inserts
  Size_t mean;        // global mean size of library
  SD_t stdev;         // global standard deviation
  bool over = false;  // is error on the longer or shorter side
  bool inbad = false; // are we in a bad regions

  interest.clear();
  stretch.clear();

  //  cout << "in CESTAT libid=" << libid << endl;

  // get parameters for global distribution
  mean = libifo.first;
  stdev = libifo.second;
  
  for (list<list<AnnotatedFragment>::iterator>::iterator mi = begin; mi != end; mi++){
      Pos_t left, right;
      
      if ((*mi)->getLibrary() != libid) // only do the selected library
	continue;
  
      if ((*mi)->status == MP_GOOD || (*mi)->status == MP_LONG || (*mi)->status == MP_SHORT){
	if (abs (mean - (*mi)->size) > MAX_DEVIATION * stdev) // skip inserts that are too long or too short
	  continue;

	left = posmap[((*mi)->getMatePair()).first].getBegin(); // use end of reads
	right = posmap[((*mi)->getMatePair()).second].getBegin(); 
	if (left < right)
	  starts.push_back(pair<Pos_t, Pos_t>(left, right));
	else 
	  starts.push_back(pair<Pos_t, Pos_t>(right, left));
      } 
  }// for each mate

  if (starts.size() < MIN_CE_OBS){
    return;
  }

  //  cout << "got " << starts.size() << " elements\n";

  //  int inends = 0;
  sort(starts.begin(), starts.end(), LessByFirst());
  for (vector<pair<Pos_t, Pos_t> >::iterator si = starts.begin(); si != starts.end(); si++){
    if (ends.size() == 0 || si->first <= ends[0].second) { // reached beginning of range
      //     cout << "got begin " << ++inends << endl;
      //    cout << si->first << ", " << si->second << endl;
      ends.push_back(*si);
      push_heap(ends.begin(), ends.end(), GtBySecond());
      pos = si->first;
    } else { // reached end of range
      //   cout << "got end " << --inends << endl;
      pos = ends[0].second;
      pop_heap(ends.begin(), ends.end(), GtBySecond());
      ends.pop_back();
    }

    getMeanSD(ends.begin(), ends.end(), mea, sderr);
    if (globals.find("ztest") != globals.end()) // use library stdev as sderr
      sderr = stdev;

    if (globals.find("ceplot") != globals.end()){
      ceplotFile << pos << "\t" << mea << " " << mean << " " << sderr << " " << ((sderr != 0) ? (mea - mean) * sqrt((double)ends.size()) / sderr : 0 ) << "\t" << ends.size() << endl;
    }


    if (ends.size() > MIN_CE_OBS && abs(mea - mean) > num_sd * sderr / sqrt((double)ends.size())){ // too far off
      //      cerr << "CE " << libid << " pos= " << pos << " skew= " << (mea - mean) * sqrt((double)ends.size()) / sderr << " nobs= " << ends.size() << endl;
      if (mea > mean) {
	if (inbad && over == false){ // switched type of bad region
	  interest.push_back(pair<Pos_t, Pos_t> (st, pos));
	  stretch.push_back(over);
	  st = pos;
	}
	over = true;
      } else {
	if (inbad && over == true){ // switched type of bad region
	  interest.push_back(pair<Pos_t, Pos_t> (st, pos));
	  stretch.push_back(over);
	  st = pos;
	}
	over = false;
      }
      if (! inbad){      // if not already in a bad region, we are now
	inbad = true;
	st = pos;        // start of bad region is current position
      }
    } else if (inbad) { // bad region ended
      inbad = false;
      interest.push_back(pair<Pos_t, Pos_t> (st, pos));
      stretch.push_back(over);
    }
  } // for each start

  cerr << "Done with starts" << endl;
  while (ends.size() > 0){
    pos = ends[0].second;
    pop_heap(ends.begin(), ends.end(), GtBySecond());
    ends.pop_back();
    if (ends.size()== 0){
      cerr << "Done with ends\n";
      break;
    }

    getMeanSD(ends.begin(), ends.end(), mea, sderr);
    if (globals.find("ztest") != globals.end()) // use library stdev as sderr
      sderr = stdev;

    if (globals.find("ceplot") != globals.end()){
      ceplotFile << pos << "\t" << mea << " " << mean << " " << sderr << " " << ((sderr != 0) ? (mea - mean) * sqrt((double)ends.size()) / sderr : 0 ) << "\t" << ends.size() << endl;
    }

    if (ends.size() > MIN_CE_OBS && abs(mea - mean) > num_sd * sderr / sqrt((double)ends.size())){ // too far off
      //      cerr << "CE " << libid << " pos= " << pos << " skew= " << (mea - mean) * sqrt((double)ends.size()) / sderr << " nobs= " << ends.size() << endl;
      if (mea > mean) {
	if (inbad && over == false){ // switched type of bad region
	  interest.push_back(pair<Pos_t, Pos_t> (st, pos));
	  stretch.push_back(over);
	  st = pos;
	}
	over = true;
      } else {
	if (inbad && over == true){ // switched type of bad region
	  interest.push_back(pair<Pos_t, Pos_t> (st, pos));
	  stretch.push_back(over);
	  st = pos;
	}
	over = false;
      }
      if (! inbad){      // if not already in a bad region, we are now
	inbad = true;
	st = pos;        // start of bad region is current position
      }
    } else if (inbad) { // bad region ended
      inbad = false;
      interest.push_back(pair<Pos_t, Pos_t> (st, pos));
      stretch.push_back(over);
    }
  } // for each end
} // getCEstat



//********************************************
//**** MAIN **********************************
//********************************************
int main(int argc, char **argv)
{
  if (! GetOptions(argc, argv)){
    cerr << "Command line parsing failed" << endl;
    printHelpText();
    exit(1);
  }
  
  // set up some global variables
  int MIN_OBS = 100;          // min. num. of observations required to change lib size
  float NUM_SD = 3.00;        // num. of standard deviations within which mate is good
  int MIN_GOOD_CVG = 0;       // good mate coverages below this will be reported
  int MAX_SHORT_CVG = 2;      // short mate coverages over this will be reported
  int MAX_LONG_CVG = 2;       // long mate coverages over this will be reported
  int MAX_NORMAL_CVG = 3;     // normal orientation read coverages > will be reptd.
  int MAX_OUTIE_CVG = 3;      // outie orientation read coverages > will be reptd.
  int MAX_LINKING_CVG = 3;    // linking mate coverage > will be reported
  int MAX_SINGLEMATE_CVG = 3; // single mate coverage > will be reported
  int MEA_CHANGE = 50;        // libraries that change by less are considered unchanged
  int SD_CHANGE = 10;         // libraries that change by less are considered unchanged

  bool recompute = true;      // recompute library sizes
  bool debug = false;         // write debugging info to STDERR

  cerr << "Invocation: ";
  for (int i = 0; i < argc; i++){
    cerr << argv[i] << " ";
  }
  cerr << endl << endl;
  
  if (globals.find("debug") != globals.end()){
    debug = true;
    cerr << "Debugging mode: additional information written to STDERR" << endl;
  }

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
    library_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open library account in bank " << globals["bank"] 
           << ": " << endl << e << endl;
      exit(1);
    }

  BankStream_t contig_stream (Layout_t::NCODE);
  Bank_t contig_bank (Layout_t::NCODE);
  
  if (globals.find("eid") == globals.end() &&
      globals.find("iid") == globals.end()){
    
    if (! contig_stream.exists(globals["bank"])){
      cerr << "No layout account found in bank " << globals["bank"] << endl;
      exit(1);
    }
    try {
      contig_stream.open(globals["bank"], B_READ);
    } catch (Exception_t & e)
      {
	cerr << "Failed to open layout account in bank " << globals["bank"] 
	     << ": " << endl << e << endl;
	exit(1);
      }
  } else {
    if (! contig_bank.exists(globals["bank"])){
      cerr << "No layout account found in bank " << globals["bank"] << endl;
      exit(1);
    }
    try {
      contig_bank.open(globals["bank"], B_READ);
    } catch (Exception_t & e)
      {
	cerr << "Failed to open layout account in bank " << globals["bank"] 
	     << ": " << endl << e << endl;
	exit(1);
      }
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
  
  // C/E statistic plot to be written to this file
  ofstream ceplotFile;
  if (globals.find("ceplot") != globals.end()){
    ceplotFile.open(globals["ceplot"].c_str(), ofstream::out);
    if (! ceplotFile.is_open()){
      cerr << "Failed to open ceplot file " << globals["ceplot"] << endl;
      exit(1);
    }
  }

  //********************************************
  // LOAD CONTIG INFO
  // rd2ctg <- contig IID for each read
  // rd2scfid <- scaffold IID for each read
  // rd2posn <- coordinates of reads in contigs
  // rd2scaff <- coordinates of reads in scaffolds
  // ctglen <- length of contigs
  // ctgname <- EIDs of contigs
  Layout_t ctg;
  
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2ctg;      // map from read to contig
  hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > rd2posn;  // position in contig
  hash_map<ID_t, Size_t, hash<ID_t>, equal_to<ID_t> > ctglen;    // length of contig
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > ctgname;   // name of contig

  set<ID_t> ctgIDs;

  if (globals.find("eid") == globals.end() && globals.find("iid") == globals.end()){
    while (contig_stream >> ctg)
      for (vector<Tile_t>::iterator ti = ctg.getTiling().begin(); 
	   ti != ctg.getTiling().end(); ti++){
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
      
      //      cerr << "Coords in contig " << ctg.getIID() << " for read " << ti->source << " = " << f << ", " << l << endl;
      
	ctglen[ctg.getIID()] = ctg.getSpan();
	ctgname[ctg.getIID()] = ctg.getEID();
      } // for each tile
    // for each contig
  } else {
    if (globals.find("eid") != globals.end()){
      if (! contig_bank.existsEID(globals["eid"])){
	cerr << "Cannot find eid: " << globals["eid"] << "  in the bank\n";
	exit(1);
      }
      contig_bank.fetch(globals["eid"], ctg);
    } else {// iid 
      ID_t iid = strtol(globals["iid"].c_str(), NULL, 10);
      if (! contig_bank.existsIID(iid)){
	cerr << "Cannot find iid: " << iid << " in the bank\n";
	exit(1);
      }
    }
    for (vector<Tile_t>::iterator ti = ctg.getTiling().begin(); 
	 ti != ctg.getTiling().end(); ti++){
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
      
      //      cerr << "Coords in contig " << ctg.getIID() << " for read " << ti->source << " = " << f << ", " << l << endl;
      
      ctglen[ctg.getIID()] = ctg.getSpan();
      ctgname[ctg.getIID()] = ctg.getEID();
    } // for each tile
  } // if contig selected by iid or eid
  
  contig_stream.close();
  

  //*********************************************
  // LOAD FRAGMENT AND READ INFORMATION
  // rd2name <- EID of read
  // mtl <- list of all mate pairs
  // lib2frag <- list of fragments in library
  // ctg2frag <- list of fragments in contig
  // scf2frag <- list of fragments in scaffold
  AnnotatedFragment frag;
  pair<ID_t, ID_t> mtp;
  list<AnnotatedFragment> mtl;
  Read_t rd1, rd2;
  set<ID_t> libIDs;
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
    
    if (mtp.first == 0 || mtp.second == 0)
      continue;
    
    if (frag.getType() != Fragment_t::INSERT) // we only handle end reads
      continue;
    
    
    rd2name[mtp.first] = rd1.getEID();
    rd2name[mtp.second] = rd2.getEID();
    libIDs.insert(frag.getLibrary());
    

    list<AnnotatedFragment>::iterator ti; 
    if (rd2ctg[mtp.first] == rd2ctg[mtp.second]){
      // mate-pair between reads within the same contig
      frag.contig1 = rd2ctg[mtp.first];
      frag.contig2 = rd2ctg[mtp.first];
      ti = mtl.insert(mtl.end(), frag);
      
      lib2frag[frag.getLibrary()].push_back(ti);
      ctg2frag[rd2ctg[mtp.first]].push_back(ti);
    } else {
      if (rd2ctg[mtp.first] == 0 ||
	  rd2ctg[mtp.second] == 0) { // mate should be in some contig
	frag.status = MP_SINGMATE;
	frag.contig1 = rd2ctg[mtp.first];
	frag.contig2 = rd2ctg[mtp.second];
	ti = mtl.insert(mtl.end(), frag);
	lib2frag[frag.getLibrary()].push_back(ti);
	ctg2frag[rd2ctg[mtp.first]].push_back(ti);
      } else {
	frag.status = MP_SPANNING; // spans gap between contigs
	frag.contig1 = rd2ctg[mtp.first];
	frag.contig2 = rd2ctg[mtp.second];
	ti = mtl.insert(mtl.end(), frag);
	lib2frag[frag.getLibrary()].push_back(ti);
	ctg2frag[rd2ctg[mtp.first]].push_back(ti);
	ctg2frag[rd2ctg[mtp.second]].push_back(ti);
      }
    }
  } // for each fragment
  
  //**********************************************************
  // GET LIBRARY INFO
  //
  // lib2name <- EID of library
  // lib2size <- mean, stdev pair
  hash_map<ID_t, pair<Pos_t, SD_t>, hash<ID_t>, equal_to<ID_t> > lib2size;
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > lib2name;
  Library_t lib;
  for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
    library_bank.fetch(*li, lib);
    lib2name[*li] = lib.getEID();
    lib2size[*li] = pair<Pos_t, SD_t> (lib.getDistribution().mean, 
				       lib.getDistribution().sd);
  }
  
  // now we're ready to figure out new library sizes
  hash_map<ID_t, pair<Pos_t, SD_t>, hash<ID_t>, equal_to<ID_t> > NewLib2size;
  
  for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
    // for each library
    list<Pos_t> sizes;
    pair<Pos_t, SD_t> libsize = lib2size[*li];
    
    for (list<list<AnnotatedFragment>::iterator>::iterator 
	   mi = lib2frag[*li].begin(); mi != lib2frag[*li].end(); mi++){
      // for each mate pair
      if ((*mi)->status == MP_GOOD) {
	Pos_t sz = mateLen(**mi, rd2posn[(*mi)->getMatePair().first], 
			   rd2posn[(*mi)->getMatePair().second], 
			   ctglen[rd2ctg[(*mi)->getMatePair().first]], 
			   libsize.first + 3 * libsize.second);

	if (recompute && sz != 0) {
	  sizes.push_back(sz);
	}
      }
    } 
    
    // compute new sizes
    pair<Pos_t, SD_t> newSize;
    if (! recompute || sizes.size() < MIN_OBS ){
      newSize = libsize;
    } else {
      newSize = getSz(sizes);
      if ((abs (((int) newSize.first) - ((int) libsize.first)) > MEA_CHANGE) ||
	  (abs (((int) newSize.second) - ((int) libsize.second)) > SD_CHANGE) ){ // if new size significantly different
	cerr <<  "Library " << lib2name[*li] << " recomputed as mean = " 
	     << newSize.first << " SD = " << newSize.second << endl;
      } else {
	cerr << "Size of library " << lib2name[*li] 
	     << " has not changed enough: MEA= " << newSize.first 
	     << " SD= " << newSize.second << " versus MEA= "
	     << libsize.first << " SD= " << libsize.second << endl;
      }// if size actually changed
    } 
    NewLib2size[*li] = newSize;
    
    // recheck mate lengths and set status
    for (list<list<AnnotatedFragment>::iterator>::iterator 
	   mi = lib2frag[*li].begin(); mi != lib2frag[*li].end(); mi++){
      // for each mate pair
      // turn off end check 
      if ((*mi)->status != MP_GOOD && (*mi)->status != MP_SPANNING)
	continue; // only interested in mates not already tagged

      if ((*mi)->status == MP_SPANNING)
	continue;  // only in scaffolds do we measure spanning clones

      Pos_t sz;
      sz = mateLen(**mi, rd2posn[((*mi)->getMatePair()).first], 
		   rd2posn[((*mi)->getMatePair()).second], 
		   ctglen[rd2ctg[((*mi)->getMatePair()).first]], 0);

      if (sz == 0){
	cerr << "Zero size? " << ((*mi)->getMatePair()).first << " (" << rd2posn[((*mi)->getMatePair()).first].getBegin() << ", "
	     << rd2posn[((*mi)->getMatePair()).first].getEnd() << ") - "
	     << ((*mi)->getMatePair()).second << " (" << rd2posn[((*mi)->getMatePair()).second].getBegin() << ", "
	     << rd2posn[((*mi)->getMatePair()).second].getEnd() << ") " << ctglen[rd2ctg[((*mi)->getMatePair()).first]] << "\n";
      } 
      assert (sz != 0); // sz == 0 implies mis-orientation.  shouldn't have made it this far

      if (abs(sz - newSize.first) > newSize.second * NUM_SD) {
	// farther than NUM_SD standard deviations from the mean
	if (sz < newSize.first)
	  (*mi)->status = MP_SHORT; // too short
	else  
	  (*mi)->status = MP_LONG; // too long
	
      } 
    } // for each mate pair
  }// for each library
  
  if (recompute) 
    cerr << endl;
    

  //*****************************************************************************
  // Now all the tough work is done.  It remains to report on all the problems
  // we've encountered.
  //*****************************************************************************

  for (set<ID_t>::iterator ctg = ctgIDs.begin(); ctg != ctgIDs.end(); ctg++) {
    
    list<pair<Pos_t, Pos_t> > ranges;  // ranges we are interested in
    list<pair<Pos_t, Pos_t> > interest;

    if (globals.find("eid") == globals.end() && globals.find("iid") == globals.end())    
      cout << ">Contig_" << *ctg << " " << ctgname[*ctg] << " " 
	   << ctglen[*ctg] << " bases " << ctg2frag[*ctg].size() << " fragments" << endl;
    
    cerr << ">Contig_" << *ctg << " " << ctgname[*ctg] << " " 
	 << ctglen[*ctg] << " bases" << endl;
    
    // calculate C/E statistics
    for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
      pair<Pos_t, SD_t> libsize = NewLib2size[*li];
      list<bool> stretch;
      
      if (globals.find("ceplot") != globals.end())
	ceplotFile << ">c" << *ctg << " " << ctgname[*ctg] << " l" << *li << " " << lib2name[*li] << endl;
      
      getCEstat(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, *li, libsize, NUM_SD, interest, stretch, ceplotFile);
      
      // report interesting ranges
      if (interest.size() > 0){
	if (globals.find("eid") == globals.end() && globals.find("iid") == globals.end())
	  cout << "In library " << *li << endl;
	
	list<bool>::iterator si = stretch.begin();
	for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	     ii != interest.end(); ii++, si++){
	  cout << ii->first << " " << ii->second << endl;
	  //	  if (*si) 
	  //	    cout << " STRETCH in lib " << *li << "(" << lib2name[*li] << ")" << endl;
	  //	  else 
	  //	    cout << " COMPRESS in lib " << *li << "(" << lib2name[*li] << ")" << endl;
	}
	//	cout << endl;
      }
    } // for each lib
  } // for each contig
  frag_bank.close();
  read_bank.close();
  library_bank.close();
  if (globals.find("ceplot") != globals.end())
    ceplotFile.close();

  return(0);
} // main
    
