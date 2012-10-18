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

typedef double DoubleSD_t;

#define endl "\n"

#define MIN_CE_OBS 4    // minimum number of clones required to report anomalous CE numbers
#define MAX_DEVIATION 8 // clones longer/shorter than this many standard deviations from mean are ignored in CE computation


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
    "asmQC - computes several mate-pair based statistics of an assembly, including\n"
    "        compression-expansion (CE)\n"
    "\n"
    ".USAGE.\n"
    "asmQC -b[ank] <bank_name> [options]\n"
    "\n"
    ".OPTIONS.\n"
    "-h, -help         print out help message\n"
    "-b, -bank         bank where assembly is stored\n"
    "-scaff            take into account scaffolds when reporting results\n"
    "-feat             write contig features into the bank\n"
    "-recompute        recompute library sizes\n"
    "-update           update bank with recomputed library sizes\n"
    "-perinsert <file> output per-insert information in <file>\n"
    "-minobs <n>       minimum number of good mate-pairs required to recompute\n"
    "                  library sizes\n"
    "-numsd <n>        mate-pairs within <n> standard deviations of the mean library\n"
    "                  size are considered good. <n> can be fractional\n"
    "-goodcvg <n>      report regions with good mate coverage less than <n>\n"
    "-cestat           report regions with unusual CE stat (more than <numsd> \n"
    "                  standard errors from mean). cestat implies -recompute and \n"
    "                  invalidates -shortcvg and -longcvg\n"
    "-shortcvg <n>     report regions with short mate coverage greater than <n>\n"
    "-longcvg <n>      report regions with long mate coverage greater than <n>\n"
    "-samecvg <n>      report regions with coverage by mates with same orientation\n"
    "                  greater than <n>\n"
    "-outiecvg <n>     report regions with outie coverage greater than <n>\n"
    "-linking <n>      report regions with linking read coverage greater than <n>\n"
    "-singlemate <n>   report regions with singleton mate coverage > than <n>\n"
    "-meachange <n>    libraries whose mean changed by less than <n> will be \n"
    "                  considered unchanged\n"
    "-sdchange <n>     libraries whose stdev. changed by less than <n> will be \n"
    "                  considered unchanged\n"
    "-ceplot <file>    file to output CE statistic plot to\n"
    "-debug            output status for each mate-pair to STDERR\n"
    "\n"
    ".DESCRIPTION.\n"
    "\n"
    ".KEYWORDS.\n"
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
    {"scaff",     0, 0, 'B'},
    {"minobs",    1, 0, 'm'},
    {"singlemate", 1, 0, 'M'},
    {"goodcvg",   1, 0, 'g'},
    {"cestat",    0, 0, 'G'},
    {"shortcvg",  1, 0, 's'},
    {"numsd",     1, 0, 'S'},
    {"longcvg",   1, 0, 'l'},
    {"linking",   1, 0, 'L'},
    {"samecvg",   1, 0, 'n'},
    {"outiecvg",  1, 0, 'o'},
    {"recompute", 0, 0, 'r'},
    {"update",    0, 0, 'u'},
    {"feat",      0, 0, 'f'},
    {"debug",     0, 0, 'd'},
    {"perinsert", 1, 0, 'p'},
    {"ceplot"   , 1, 0, 'P'},
    {"meachange", 1, 0, 'c'},
    {"sdchange",  1, 0, 'C'},
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
    case 'B':
      globals["scaff"] = string("true");
      break;
    case 'm':
      globals["minobs"] = string(optarg);
      break;
    case 'M':
      globals["singlemate"] = string(optarg);
      break;
    case 's':
      globals["shortcvg"] = string(optarg);
      break;
    case 'S':
      globals["numsd"] = string(optarg);
      break;
    case 'g':
      globals["goodcvg"] = string(optarg);
      break;
    case 'G':
      globals["cestat"] = string("true");
      break;
    case 'l':
      globals["longcvg"] = string(optarg);
      break;
    case 'L':
      globals["linking"] = string(optarg);
      break;
    case 'n':
      globals["normalcvg"] = string(optarg);
      break;
    case 'o':
      globals["outiecvg"] = string(optarg);
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
    case 'p':
      globals["perinsert"] = string(optarg);
      break;
    case 'P':
      globals["ceplot"] = string(optarg);
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

  m.size = abs(a.getBegin() - b.getBegin());
  return m.size;
}// mateLen


// computes mean and standard deviation for a set of observations
// observations outside of 5 standard deviations are excluded
pair<Pos_t, DoubleSD_t> getSz(list<Pos_t> & sizes)
{
  int numObs = sizes.size();
  if (numObs == 0) return pair<Pos_t, DoubleSD_t> (0, 0); 
  // this should probably be an assert...

  Pos_t mean = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    mean += *li;
  }
  mean = (Pos_t) rint (mean * 1.0 / numObs);

  DoubleSD_t stdev = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    stdev += (*li - mean) * (*li - mean);
  }
  stdev = (DoubleSD_t) rint(sqrt(stdev * 1.0 / (numObs - 1)));
  
  Pos_t origm = mean;
  DoubleSD_t origs = stdev;

  // recompute mean and stdev for just sizes within 5SD of mean

  mean = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    if (abs(*li - origm) < MAX_DEVIATION * origs)
      mean += *li;
    else 
      numObs--;
  }

  if (numObs == 0) return pair<Pos_t, DoubleSD_t> (0, 0);
  // this should also be an assert

  mean = (Pos_t) rint (mean * 1.0 / numObs);
 
  stdev = 0;
  for (list<Pos_t>::iterator li = sizes.begin();  li != sizes.end(); li++){
    if (abs(*li - origm) < MAX_DEVIATION * origs)
      stdev += (*li - mean) * (*li - mean);
  }

  stdev = (DoubleSD_t) rint(sqrt(stdev * 1.0 / (numObs - 1)));

  return pair<Pos_t, DoubleSD_t>(mean, stdev);
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

  if (coverage < 0)
     return;
  
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
// if (coverage < 0) return;
  if (ranges.size() == 0) {
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
	       Size_t &mea, DoubleSD_t &sd)
{
  int n = 0;
  Size_t sum = 0;
  DoubleSD_t var = 0;

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
    sd = (DoubleSD_t) sqrt((double)sd);
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
	       ID_t libid, pair<Size_t, DoubleSD_t> & libifo, float num_sd, 
	       list<pair<Pos_t, Pos_t> > & interest, 
	       list<bool> & stretch,
	       ofstream & ceplotFile)
{
  vector<pair<Pos_t, Pos_t> > starts, ends;
  Pos_t pos, st;
  Size_t mea;         // sum of sizes of inserts
  DoubleSD_t sderr;         // standard error for inserts
  Size_t mean;        // global mean size of library
  DoubleSD_t stdev;         // global standard deviation
  bool over = false;  // is error on the longer or shorter side
  bool inbad = false; // are we in a bad regions

  interest.clear();
  stretch.clear();

  // get parameters for global distribution
  mean = libifo.first;
  stdev = libifo.second;
  cout << "in CESTAT libid=" << libid << " mea=" << mean << " stdev=" << stdev << endl;
  
  for (list<list<AnnotatedFragment>::iterator>::iterator mi = begin; mi != end; mi++){
      Pos_t left, right;
      
      if ((*mi)->getLibrary() != libid) // only do the selected library
	continue;


      if ((*mi)->status == MP_GOOD || (*mi)->status == MP_LONG || (*mi)->status == MP_SHORT){
	cout << "Got insert " << (((*mi)->getMatePair()).first) << endl; 

	left = posmap[((*mi)->getMatePair()).first].getBegin(); // use end of reads
	right = posmap[((*mi)->getMatePair()).second].getBegin(); 
	cout << left << ", " << right << endl;
	if (abs (mean - (*mi)->size) > MAX_DEVIATION * stdev){ // skip inserts that are too long or too short
	  cout << "too far" << mean << " - " << (*mi)->size << " vs " << MAX_DEVIATION * stdev << endl;;
	  continue;
	}
	if (left < right)
	  starts.push_back(pair<Pos_t, Pos_t>(left, right));
	else 
	  starts.push_back(pair<Pos_t, Pos_t>(right, left));
      } 
  }// for each mate

  if (starts.size() < 3){
    return;
  }

  cout << "got " << starts.size() << " elements\n";

  int inends = 0;
  sort(starts.begin(), starts.end(), LessByFirst());
  for (vector<pair<Pos_t, Pos_t> >::iterator si = starts.begin(); si != starts.end(); si++){
    if (ends.size() == 0 || si->first <= ends[0].second) { // reached beginning of range
      cout << "got begin " << ++inends << endl;
      cout << si->first << ", " << si->second << endl;
      ends.push_back(*si);
      push_heap(ends.begin(), ends.end(), GtBySecond());
      pos = si->first;
    } else { // reached end of range
      cout << "got end " << --inends << endl;
      pos = ends[0].second;
      pop_heap(ends.begin(), ends.end(), GtBySecond());
      ends.pop_back();
    }
    getMeanSD(ends.begin(), ends.end(), mea, sderr);

    if (globals.find("ceplot") != globals.end()){
      ceplotFile << pos << "\t" << ((stdev != 0) ? (mea - mean) * sqrt((double)ends.size()) / stdev : 0 ) << "\t" << ends.size() << endl;
    }

    if (ends.size() > MIN_CE_OBS && abs(mea - mean) > num_sd * stdev / sqrt((double)ends.size())){ // too far off
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

  cout << "Done with starts" << endl;
  while (ends.size() > 0){
    pos = ends[0].second;
    pop_heap(ends.begin(), ends.end(), GtBySecond());
    ends.pop_back();
    if (ends.size()== 0){
      cout << "Done with ends\n";
      break;
    }

    getMeanSD(ends.begin(), ends.end(), mea, sderr);

    if (globals.find("ceplot") != globals.end()){
      ceplotFile << pos << "\t" << ((stdev != 0) ? (mea - mean) * sqrt((double)ends.size()) / stdev : 0 ) << "\t" << ends.size() << endl;
    }

    if (ends.size() > MIN_CE_OBS && abs(mea - mean) > num_sd * stdev / sqrt((double)ends.size())){ // too far off
      cerr << "CE " << libid << " pos= " << pos << " skew= " << (mea - mean) * sqrt((double)ends.size()) / stdev << " nobs= " << ends.size() << endl;
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

  bool recompute = false;     // recompute library sizes
  bool update = false;        // update library sizes in bank
  bool feats = false;         // add features to bank
  bool debug = false;         // write debugging info to STDERR
  bool perinsert = false;     // do per-insert output
  bool byscaff = false;       // results reported by scaffold
  bool cestat = false;        // do C/E statistics

  cout << "Invocation: ";
  for (int i = 0; i < argc; i++){
    cout << argv[i] << " ";
  }
  cout << endl << endl;

  if (globals.find("debug") != globals.end()){
    debug = true;
    cout << "Debugging mode: additional information written to STDERR" << endl;
  }

  if (globals.find("meanchange") != globals.end())
      MEA_CHANGE = strtol(globals["meanchange"].c_str(), NULL, 10);

  if (globals.find("sdchange") != globals.end())
      SD_CHANGE = strtol(globals["sdchange"].c_str(), NULL, 10);


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


  if (globals.find("numsd") != globals.end())
    NUM_SD = (float)strtod(globals["numsd"].c_str(), NULL);
  cout << "Mates within " << NUM_SD 
       << " standard deviations from mean considered good" << endl;

  if (globals.find("goodcvg") != globals.end())
    MIN_GOOD_CVG = strtol(globals["goodcvg"].c_str(), NULL, 10);
  cout << "Reporting regions with less than " << MIN_GOOD_CVG
       << " good mate coverage" << endl;

  if (globals.find("cestat") != globals.end()){
    cout << "Reporting regions with anomalous C/E stat: over " << NUM_SD 
	 << " standard errors from mean" << endl;
    //    globals["recompute"] = string("true");
    globals.erase("shortcvg");
    globals.erase("longcvg");
    cestat = true;
  }

  if (globals.find("recompute") != globals.end()){
    recompute = true;
    cout << "Will recompute library sizes" << endl;
    cout << "Libraries whose mean changes by less than " << MEA_CHANGE
	 << "\nand whose std. dev. changes by less than " << SD_CHANGE 
	 << " are considered to not have changed" << endl;
  } else
    cout << "Will not recompute library sizes" << endl;

  if (recompute){
    if (globals.find("minobs") != globals.end())
      MIN_OBS = strtol(globals["minobs"].c_str(), NULL, 10);
    cout << "Minimum number of observations required to change library size: " 
	 << MIN_OBS << endl;
  }

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
  if (globals.find("perinsert") != globals.end()){
    perinsert = true;
    cout << "Per-insert information reported in file: " 
	 << globals["perinsert"] << endl;
  }
  cout << endl;

  if (globals.find("scaff") != globals.end()){
    byscaff = true;
    cout << "Reporting results by scaffold.  In this mode linking clones connect different scaffolds." << endl;
  }
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

  BankStream_t contig_stream (Contig_t::NCODE);
  if (! contig_stream.exists(globals["bank"])){
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

  BankStream_t feat_stream (Feature_t::NCODE);
  if (feats)
    {
      try {
       if (! feat_stream.exists(globals["bank"])){
	 feat_stream.create(globals["bank"], B_READ|B_WRITE);
       } else {
	 feat_stream.open(globals["bank"], B_READ|B_WRITE);
       }
      } catch (Exception_t & e)
        {
          cerr << "Failed to open feat account in bank " << globals["bank"] 
               << ": " << endl << e << endl;
          exit(1);
        }
    }

  BankStream_t scaff_stream (Scaffold_t::NCODE);
  if (byscaff){
    try {
      if (! scaff_stream.exists(globals["bank"])){
        cerr << "No scaffold account found in bank " << globals["bank"] << endl
             << "ignoring -scaff option" << endl;
        byscaff = false;
      } else {
	scaff_stream.open(globals["bank"], B_READ);
      }
    } catch (Exception_t & e)
      {
	cerr << "Failed to open scaffold account in bank " << globals["bank"]
	     << ": " << endl << e << endl;
	exit(1);
      }
  }
  

  ofstream insertFile;
  if (perinsert){
    insertFile.open(globals["perinsert"].c_str(), ofstream::out);
    if (! insertFile.is_open()){
      cerr << "Failed to open insert file " << globals["perinsert"] << endl;
      exit(1);
    }
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

  //**********************************************
  // LOAD SCAFFOLD INFO
  // ctg2scaff <- scaffold IID for each contig
  // ctg2posn <- position of contig in scaffold
  // scaffspan <- span of scaffold
  Scaffold_t scaff;
  
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > ctg2scaff;   // map from contig to scaffold
  hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > ctg2posn; // position in scaffold
  hash_map<ID_t, Size_t, hash<ID_t>, equal_to<ID_t> > scaffspan; // span of scaffold
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > scaffname;   // name of scaffold

  set<ID_t> scfIDs;

  if (byscaff){
    while (scaff_stream >> scaff){
      scaffspan[scaff.getIID()] = scaff.getSpan();
      scfIDs.insert(scaff.getIID());
      scaffname[scaff.getIID()] = scaff.getEID();
      //     cerr << "Span of " << scaff.getIID() << " is " << scaff.getSpan() << endl;
      for (vector<Tile_t>::iterator ti = scaff.getContigTiling().begin();
	   ti != scaff.getContigTiling().end(); ti++){
	ctg2scaff[ti->source] = scaff.getIID();
	
	Pos_t f, l; // coords of beginning/end of contig
	if (ti->range.getEnd() < ti->range.getBegin()) { // reverse
	  f = ti->offset + ti->range.getBegin() - ti->range.getEnd();
	  l = ti->offset;
	} else {
	  f = ti->offset;
	  l = ti->offset + ti->range.getEnd() - ti->range.getBegin();
	}
	ctg2posn[ti->source] = Range_t(f, l);
      } // for each tile
    } // while scaff_stream
  } // if (byscaff)
  

  //********************************************
  // LOAD CONTIG INFO
  // rd2ctg <- contig IID for each read
  // rd2scfid <- scaffold IID for each read
  // rd2posn <- coordinates of reads in contigs
  // rd2scaff <- coordinates of reads in scaffolds
  // ctglen <- length of contigs
  // ctgname <- EIDs of contigs
  Contig_t ctg;
  
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2ctg;      // map from read to contig
  hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > rd2scfid;    // map from read to scaffold
  hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > rd2posn;  // position in contig
  hash_map<ID_t, Range_t, hash<ID_t>, equal_to<ID_t> > rd2scaff; // position in scaffold
  hash_map<ID_t, Size_t, hash<ID_t>, equal_to<ID_t> > ctglen;    // length of contig
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
      
      //      cerr << "Coords in contig " << ctg.getIID() << " for read " << ti->source << " = " << f << ", " << l << endl;
      
      if (byscaff) { // coordinates are based on scaffold
	rd2scfid[ti->source] = ctg2scaff[ctg.getIID()];
	if (         rd2posn[ti->source].isReverse() &&   ctg2posn[ctg.getIID()].isReverse()){ // both reverse
	  rd2scaff[ti->source] = Range_t(ctg2posn[ctg.getIID()].getBegin() - f, ctg2posn[ctg.getIID()].getBegin() - l);
	} else if (! rd2posn[ti->source].isReverse() &&   ctg2posn[ctg.getIID()].isReverse()){ // contig reverse
	  rd2scaff[ti->source] = Range_t(ctg2posn[ctg.getIID()].getBegin() - f, ctg2posn[ctg.getIID()].getBegin() - l);
	} else if (  rd2posn[ti->source].isReverse() && ! ctg2posn[ctg.getIID()].isReverse()){ // read reverse
	  rd2scaff[ti->source] = Range_t(ctg2posn[ctg.getIID()].getBegin() + f, ctg2posn[ctg.getIID()].getBegin() + l);
	} else if (! rd2posn[ti->source].isReverse() && ! ctg2posn[ctg.getIID()].isReverse()){ // both forward
	  rd2scaff[ti->source] = Range_t(ctg2posn[ctg.getIID()].getBegin() + f, ctg2posn[ctg.getIID()].getBegin() + l);
	}
	//	cerr << "Coords in scaffold " << ctg2scaff[ctg.getIID()] << " for read " << ti->source << " = " << rd2posn[ti->source].getBegin() << ", " 
	//	     << rd2posn[ti->source].getEnd() << endl;
	//	cerr << "Contig coords for contig " << ctg.getIID() << " = " << ctg2posn[ctg.getIID()].getBegin() << ", " << ctg2posn[ctg.getIID()].getEnd() << endl;
      } 
      
      ctglen[ctg.getIID()] = ctg.getLength();
      ctgname[ctg.getIID()] = ctg.getEID();
    } // for each tile
  // for each contig
  
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
  hash_map<ID_t, list<list<AnnotatedFragment>::iterator>, hash<ID_t>, equal_to<ID_t> > scf2frag;
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > rd2name;

  while (frag_bank >> frag){
    
    frag.status = MP_GOOD;
    mtp = frag.getMatePair();
    
    if (mtp.first != 0) 
      read_bank.fetch(mtp.first, rd1); // get the read
    if (mtp.second != 0)
      read_bank.fetch(mtp.second, rd2);
    
    if (mtp.first == 0 || mtp.second == 0) {
      if (perinsert)
	insertFile << "UNMATED: "
		   << ((mtp.first != 0) ? mtp.first : mtp.second)
		   << "(" << ((mtp.first != 0) ? rd1.getEID() : rd2.getEID())
		   << ")" << endl;
      continue;
    }
    
    if (frag.getType() != Fragment_t::INSERT) {// we only handle end reads
      if (perinsert)
	insertFile << "NOT END MATES: "
		   << mtp.first 
		   << "(" << rd1.getEID() << ") " 
		   << mtp.second 
		   << "(" << rd2.getEID() << ") "
		   << "\n";
      continue;
    }
    
    
    rd2name[mtp.first] = rd1.getEID();
    rd2name[mtp.second] = rd2.getEID();
    libIDs.insert(frag.getLibrary());


    list<AnnotatedFragment>::iterator ti; 
    if (rd2ctg[mtp.first] == rd2ctg[mtp.second]){
      // mate-pair between reads within the same contig
      frag.contig1 = rd2ctg[mtp.first];
      frag.contig2 = rd2ctg[mtp.first];
      if (rd2scfid[mtp.first] != 0){
	frag.scaff1 = rd2scfid[mtp.first];
	frag.scaff2 = rd2scfid[mtp.first];
      } else {
	frag.scaff1 = frag.scaff2 = 0;
      }
      ti = mtl.insert(mtl.end(), frag);
      
      lib2frag[frag.getLibrary()].push_back(ti);
      ctg2frag[rd2ctg[mtp.first]].push_back(ti);
      if (rd2scfid[mtp.first] != 0) 
	scf2frag[rd2scfid[mtp.first]].push_back(ti);
    } else {
      if (rd2ctg[mtp.first] == 0 ||
	  rd2ctg[mtp.second] == 0) { // mate should be in some contig
	frag.status = MP_SINGMATE;
	frag.contig1 = rd2ctg[mtp.first];
	frag.contig2 = rd2ctg[mtp.second];
	frag.scaff1 = rd2scfid[mtp.first];
	frag.scaff2 = rd2scfid[mtp.second];
	ti = mtl.insert(mtl.end(), frag);
	lib2frag[frag.getLibrary()].push_back(ti);
	ctg2frag[rd2ctg[mtp.first]].push_back(ti);
	if (rd2scfid[mtp.first] != 0) scf2frag[rd2scfid[mtp.first]].push_back(ti);
	if (rd2scfid[mtp.second] != 0) scf2frag[rd2scfid[mtp.second]].push_back(ti);
	if (perinsert)
	  insertFile << "SINGLETON_MATE: " 
		     << mtp.first 
		     << "(" << rd2name[mtp.first] << ") " 
		     << mtp.second 
		     << "(" << rd2name[mtp.second] << ") "
		     << " in separate contigs: "
		     << rd2ctg[mtp.first] << ", "
		     << rd2ctg[mtp.second] 
		     << "\n";
      } else {
	if (byscaff && ((rd2scfid[mtp.first] != rd2scfid[mtp.second]) || rd2scfid[mtp.first] == 0)) 
	  frag.status = MP_LINKING;  // links two different scaffolds.  0 - indicates contig is not in a scaffold
	else 
	  frag.status = MP_SPANNING; // spans gap between contigs
	frag.contig1 = rd2ctg[mtp.first];
	frag.contig2 = rd2ctg[mtp.second];
	frag.scaff1 = rd2scfid[mtp.first];
	frag.scaff2 = rd2scfid[mtp.second];
	ti = mtl.insert(mtl.end(), frag);
	lib2frag[frag.getLibrary()].push_back(ti);
	ctg2frag[rd2ctg[mtp.first]].push_back(ti);
	ctg2frag[rd2ctg[mtp.second]].push_back(ti);
	if (rd2scfid[mtp.first] != 0) 
	  scf2frag[rd2scfid[mtp.first]].push_back(ti);
	if (rd2scfid[mtp.first] != 0 && rd2scfid[mtp.second] != 0 
	    && rd2scfid[mtp.first] != rd2scfid[mtp.second]) 
	  scf2frag[rd2scfid[mtp.second]].push_back(ti);
	
	if (perinsert) {
	  if (frag.status == MP_SPANNING)
	    insertFile << "SPANNING: " 
		       << mtp.first 
		       << "(" << rd2name[mtp.first] << ") " 
		       << mtp.second 
		       << "(" << rd2name[mtp.second] << ") "
		       << " in separate contigs: "
		       << rd2ctg[mtp.first] << ", "
		       << rd2ctg[mtp.second] 
		       << "\n";
	  else // LINKING
	    insertFile << "LINKING: " 
		       << mtp.first 
		       << "(" << rd2name[mtp.first] << ") " 
		       << mtp.second 
		       << "(" << rd2name[mtp.second] << ") "
		       << " in separate scaffolds: "
		       << rd2scfid[mtp.first] << ", "
		       << rd2scfid[mtp.second]
		       << "\n";
	}
      }
    }
  } // for each fragment
  
  //**********************************************************
  // GET LIBRARY INFO
  //
  // lib2name <- EID of library
  // lib2size <- mean, stdev pair
  hash_map<ID_t, pair<Pos_t, DoubleSD_t>, hash<ID_t>, equal_to<ID_t> > lib2size;
  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > lib2name;
  Library_t lib;
  for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
    library_bank.fetch(*li, lib);
    lib2name[*li] = lib.getEID();
    lib2size[*li] = pair<Pos_t, DoubleSD_t> (lib.getDistribution().mean, 
				       lib.getDistribution().sd);
  }
  
  // now we're ready to figure out new library sizes
  hash_map<ID_t, pair<Pos_t, DoubleSD_t>, hash<ID_t>, equal_to<ID_t> > NewLib2size;
  
  for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
    // for each library
    list<Pos_t> sizes;
    pair<Pos_t, DoubleSD_t> libsize = lib2size[*li];
    
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

      // rerun mateLen if necessary for scaffolds
      // this step should assign MP_OUTIE and MP_NORMAL tags to mates
      // spanning contig boundaries. These are not used to recompute
      // library sizes
      if (byscaff && (*mi)->status == MP_SPANNING){
	mateLen(**mi, rd2scaff[(*mi)->getMatePair().first],
		rd2scaff[(*mi)->getMatePair().second],
		scaffspan[rd2scfid[(*mi)->getMatePair().first]], 0);
      } // if byscaff
    } 

    // compute new sizes
    pair<Pos_t, DoubleSD_t> newSize;
    if (! recompute || sizes.size() < MIN_OBS ){
      newSize = libsize;
    } else {
      newSize = getSz(sizes);
      if ((abs (((int) newSize.first) - ((int) libsize.first)) > MEA_CHANGE) ||
	  (abs (((int) newSize.second) - ((int) libsize.second)) > SD_CHANGE) ){ // if new size significantly different
	cout <<  "Library " << lib2name[*li] << " recomputed as mean = " 
	     << newSize.first << " SD = " << newSize.second << endl;
	if (update){
	  Library_t newlib;
	  Distribution_t dist;
	  dist.mean = newSize.first;
	  dist.sd = newSize.second;
	  newlib.setIID(*li);
	  newlib.setEID(lib2name[*li]);
	  newlib.setDistribution(dist);
	  try {
	    library_bank.replace(*li, newlib);
	  }  
	  catch (Exception_t & e)
	    {
	      cerr << "Failed to replace library " << *li << "(" 
		   << lib2name[*li] << ") in bank " << globals["bank"] 
		   << ": " << endl << e << endl;
	      exit(1);
	    }
	} // if updating library in bank
      } else {
	cout << "Size of library " << lib2name[*li] 
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
      if ((*mi)->status != MP_GOOD && (*mi)->status != MP_SPANNING){
	if (perinsert){
	  if ((*mi)->status == MP_OUTIE)
	    insertFile << "OUTIE: ";
	  if ((*mi)->status == MP_NORMAL)
	    insertFile << "SAME: ";
	  
	  insertFile << ((*mi)->getMatePair()).first 
		     << "(" << rd2name[((*mi)->getMatePair()).first] << ") " 
		     << ((*mi)->getMatePair()).second 
		     << "(" << rd2name[((*mi)->getMatePair()).second] 
		     << ") LIBRARY: "
		     << *li << "(" << lib2name[*li] << ")"
		     << "\n";
	}
	continue; // only interested in mates not already tagged
      }

      if ((*mi)->status == MP_SPANNING && ! byscaff)
	continue;  // only in scaffolds do we measure spanning clones

      Pos_t sz;
      if ((*mi)->status == MP_SPANNING) { 
	(*mi)->status = MP_GOOD;  // in scaffolds spanning is good
	sz = mateLen(**mi, rd2scaff[(*mi)->getMatePair().first],
		     rd2scaff[(*mi)->getMatePair().second],
		     scaffspan[rd2scfid[(*mi)->getMatePair().first]], 0);
      } else
	sz = mateLen(**mi, rd2posn[((*mi)->getMatePair()).first], 
		     rd2posn[((*mi)->getMatePair()).second], 
		     ctglen[rd2ctg[((*mi)->getMatePair()).first]], 0);

      if (sz == 0 && ! byscaff){
	cerr << "Zero size? " << ((*mi)->getMatePair()).first << " (" << rd2posn[((*mi)->getMatePair()).first].getBegin() << ", "
	     << rd2posn[((*mi)->getMatePair()).first].getEnd() << ") - "
	     << ((*mi)->getMatePair()).second << " (" << rd2posn[((*mi)->getMatePair()).second].getBegin() << ", "
	     << rd2posn[((*mi)->getMatePair()).second].getEnd() << ") " << ctglen[rd2ctg[((*mi)->getMatePair()).first]] << "\n";
      } else if (sz == 0 && byscaff){
	cerr << "Zero size? " << ((*mi)->getMatePair()).first << " (" << rd2scaff[((*mi)->getMatePair()).first].getBegin() << ", "
	     << rd2scaff[((*mi)->getMatePair()).first].getEnd() << ") - "
	     << ((*mi)->getMatePair()).second << " (" << rd2scaff[((*mi)->getMatePair()).second].getBegin() << ", "
	     << rd2scaff[((*mi)->getMatePair()).second].getEnd() << ") " << scaffspan[rd2scfid[((*mi)->getMatePair()).first]] << "\n";
	cerr << "Scaffold " << rd2scfid[((*mi)->getMatePair()).first] << " contig " << rd2ctg[((*mi)->getMatePair()).first] << endl;
	cerr << "Scaffold " << rd2scfid[((*mi)->getMatePair()).second] << " contig " << rd2ctg[((*mi)->getMatePair()).second] << endl;
	cerr << (*mi)->status << endl;
      }
      assert (sz != 0); // sz == 0 implies mis-orientation.  shouldn't have made it this far

      if (abs(sz - newSize.first) > newSize.second * NUM_SD) {
	// farther than NUM_SD standard deviations from the mean
	if (sz < newSize.first){
	  (*mi)->status = MP_SHORT; // too short
	  if (perinsert)
	    insertFile << "SHORT " 
		       << ((*mi)->getMatePair()).first 
		       << "(" << rd2name[((*mi)->getMatePair()).first] << ") " 
		       << ((*mi)->getMatePair()).second 
		       << "(" << rd2name[((*mi)->getMatePair()).second] << ") "
		       << "SIZE: " << sz << " EXPECTED: " << newSize.first 
		       << "(" << newSize.second << ") LIBRARY: " 
		       << *li << "(" << lib2name[*li] << ")"
		       << "\n";
		
	} else { 
	  (*mi)->status = MP_LONG; // too long
	  if (perinsert)
	    insertFile << "LONG " 
		       << ((*mi)->getMatePair()).first 
		       << "(" << rd2name[((*mi)->getMatePair()).first] << ") " 
		       << ((*mi)->getMatePair()).second 
		       << "(" << rd2name[((*mi)->getMatePair()).second] << ") "
		       << "SIZE: " << sz << " EXPECTED: " << newSize.first 
		       << "(" << newSize.second << ") LIBRARY: "
		       << *li << "(" << lib2name[*li] << ")"
		       << "\n";
	}
      } else // long or short
	if (perinsert)
	  insertFile << "GOOD " 
		     << ((*mi)->getMatePair()).first 
		     << "(" << rd2name[((*mi)->getMatePair()).first] << ") " 
		     << ((*mi)->getMatePair()).second 
		     << "(" << rd2name[((*mi)->getMatePair()).second] << ") "
		     << "SIZE: " << sz << " EXPECTED: " << newSize.first 
		     << "(" << newSize.second << ") LIBRARY: "
		     << *li << "(" << lib2name[*li] << ")"
		     << "\n";
    } // for each mate pair
  }// for each library
  
  if (recompute) 
    cout << endl;
    

  //*****************************************************************************
  // Now all the tough work is done.  It remains to report on all the problems
  // we've encountered.
  //
  // First we'll handle scaffolds, then the contigs that don't belong to scaffolds
  //*****************************************************************************

  for (set<ID_t>::iterator ctg = ctgIDs.begin(); ctg != ctgIDs.end(); ctg++) {
    
    list<pair<Pos_t, Pos_t> > ranges;  // ranges we are interested in
    list<pair<Pos_t, Pos_t> > interest;

    if (byscaff && ctg2scaff[*ctg] != 0)
      continue;  // contig in scaffold.  Skip since it's been handled in the scaffold section

    cout << ">Contig_" << *ctg << " " << ctgname[*ctg] << " " 
	 << ctglen[*ctg] << " bases" << endl;

    cerr << ">Contig_" << *ctg << " " << ctgname[*ctg] << " " 
	 << ctglen[*ctg] << " bases" << endl;

    getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_GOOD, *ctg, interest, ctglen[*ctg], MIN_GOOD_CVG, false); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MIN_GOOD_CVG << "X" 
	   << ((MIN_GOOD_CVG != 0) ? " or less": "") << " clone coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("LOW_GOOD_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*ctg, Contig_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }


    if (cestat) {
      // calculate C/E statistics
      for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
	pair<Pos_t, DoubleSD_t> libsize = NewLib2size[*li];
	list<bool> stretch;
	
	if (globals.find("ceplot") != globals.end())
	  ceplotFile << ">c" << *ctg << " " << ctgname[*ctg] << " l" << *li << " " << lib2name[*li] << endl;

	getCEstat(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, *li, libsize, NUM_SD, interest, stretch, ceplotFile);
	
	// report interesting ranges
	if (interest.size() > 0){
	  cout << "In library " << *li << endl;
	    
	  list<bool>::iterator si = stretch.begin();
	  for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	       ii != interest.end(); ii++, si++){
	    cout << ii->first << "\t" << ii->second;
	    if (*si) 
	      cout << " STRETCH in lib " << *li << "(" << lib2name[*li] << ")" << endl;
	    else 
	      cout << " COMPRESS in lib " << *li << "(" << lib2name[*li] << ")" << endl;
	    if (feats){
	      Feature_t f;
	      ostringstream comment;
	      
	      if (*si)
	        comment << "CE_STRETCH LIB=" << *li << "(" << lib2name[*li] << ")";
	      else
	        comment << "CE_COMPRESS LIB=" << *li << "(" << lib2name[*li] << ")";

	      f.setComment(comment.str());
	      f.setRange(Range_t(ii->first, ii->second));
	      f.setSource(make_pair(*ctg, Contig_t::NCODE));
	      f.setType(Feature_t::MATEPAIR);
	      feat_stream << f;
	    }
	  }
	  cout << endl;
	}
      } // for each lib
    } // if cestat

    if (! cestat){
      // find coverage by short mates

      getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_SHORT, *ctg, interest, ctglen[*ctg], MAX_SHORT_CVG, true); 
      
      // report interesting ranges
      
      if (interest.size() > 0){
	cout << "Regions of " << MAX_SHORT_CVG << "X or more short mate coverage"
	     <<endl;
	
	for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	     ii != interest.end(); ii++){
	  cout << ii->first << "\t" << ii->second << endl;
	  if (feats){
	    Feature_t f;
	    f.setComment("HIGH_SHORT_CVG");
	    f.setRange(Range_t(ii->first, ii->second));
	    f.setSource(make_pair(*ctg, Contig_t::NCODE));
	    f.setType(Feature_t::MATEPAIR);
	    feat_stream << f;
	  }
	}
	cout << endl;
      }
    }


    if (! cestat) {
      // find coverage by long mates

      getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_LONG, *ctg, interest, ctglen[*ctg], MAX_LONG_CVG, true); 

      // report interesting ranges
      
      if (interest.size() > 0){
	cout << "Regions of " << MAX_LONG_CVG << "X or more long mate coverage" 
	     <<endl;
	
	for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	     ii != interest.end(); ii++){
	  cout << ii->first << "\t" << ii->second << endl;
	  if (feats){
	    Feature_t f;
	    f.setComment("HIGH_LONG_CVG");
	    f.setRange(Range_t(ii->first, ii->second));
	    f.setSource(make_pair(*ctg, Contig_t::NCODE));
	    f.setType(Feature_t::MATEPAIR);
	    feat_stream << f;
	  }
	}
	cout << endl;
      }
    }

    // find coverage by normal reads

    getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_NORMAL, *ctg, interest, ctglen[*ctg], MAX_NORMAL_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_NORMAL_CVG 
	   << "X or more same-orientation coverage" <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_NORMAL_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*ctg, Contig_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_OUTIE, *ctg, interest, ctglen[*ctg], MAX_OUTIE_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_OUTIE_CVG << "X or more outie coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_OUTIE_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*ctg, Contig_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    // find coverage by linking reads
    getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_LINKING, *ctg, interest, ctglen[*ctg], MAX_LINKING_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_LINKING_CVG << "X or more linking coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_LINKING_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*ctg, Contig_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    // find coverage by spanning reads
    getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_SPANNING, *ctg, interest, ctglen[*ctg], MAX_LINKING_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_LINKING_CVG << "X or more linking coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_SPANNING_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*ctg, Contig_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    // find coverage by single mate reads

    getCvg(ctg2frag[*ctg].begin(), ctg2frag[*ctg].end(), rd2posn, MP_SINGMATE, *ctg, interest, ctglen[*ctg], MAX_SINGLEMATE_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_SINGLEMATE_CVG << "X or more singleton mate coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_SINGLEMATE_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*ctg, Contig_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }
  } // for each contig


  for (set<ID_t>::iterator scf = scfIDs.begin(); scf != scfIDs.end(); scf++) {

    list<pair<Pos_t, Pos_t> > ranges;  // ranges we are interested in
    list<pair<Pos_t, Pos_t> > interest;

    cout << ">Scaffold_" << *scf << " " << scaffspan[*scf] << " bases" << endl;
    cerr << ">Scaffold_" << *scf << " " << scaffspan[*scf] << " bases" << endl;

    getCvg(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, MP_GOOD, *scf, interest, scaffspan[*scf], MIN_GOOD_CVG, false); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MIN_GOOD_CVG << "X" 
	   << ((MIN_GOOD_CVG != 0) ? " or less": "") << " clone coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("LOW_GOOD_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    if (cestat) {
      // calculate C/E statistics
      for (set<ID_t>::iterator li = libIDs.begin(); li != libIDs.end(); li++){
	pair<Pos_t, DoubleSD_t> libsize = NewLib2size[*li];
	list<bool> stretch;

	if (globals.find("ceplot") != globals.end())
	  ceplotFile << ">s" << *scf << " " << scaffname[*scf] << " l" << *li << " " << lib2name[*li] << endl;

	getCEstat(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, *li, libsize, NUM_SD, interest, stretch, ceplotFile);
	
	// report interesting ranges
	if (interest.size() > 0){
	  cout << "In library " << *li << endl;
	    
	  list<bool>::iterator si = stretch.begin();
	  for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	       ii != interest.end(); ii++, si++){
	    cout << ii->first << "\t" << ii->second;
	    if (*si) 
	      cout << " STRETCH in lib " << *li << "(" << lib2name[*li] << ")" << endl;
	    else 
	      cout << " COMPRESS in lib " << *li << "(" << lib2name[*li] << ")" << endl;
	    if (feats){
	      Feature_t f;
	      ostringstream comment;
	      
	      if (*si)
	        comment << "CE_STRETCH LIB=" << *li << "(" << lib2name[*li] << ")";
	      else
	        comment << "CE_COMPRESS LIB=" << *li << "(" << lib2name[*li] << ")";

	      f.setComment(comment.str());
	      f.setRange(Range_t(ii->first, ii->second));
	      f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	      f.setType(Feature_t::MATEPAIR);
	      feat_stream << f;
	    }
	  }
	  cout << endl;
	}
      } // for each lib
    } // if cestat


    if (! cestat){
      // find coverage by short mates
      
      getCvg(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, MP_SHORT, *scf, interest, scaffspan[*scf], MAX_SHORT_CVG, true); 
      
      // report interesting ranges
      
      if (interest.size() > 0){
	cout << "Regions of " << MAX_SHORT_CVG << "X or more short mate coverage"
	     <<endl;
	
	for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	     ii != interest.end(); ii++){
	  cout << ii->first << "\t" << ii->second << endl;
	  if (feats){
	    Feature_t f;
	    f.setComment("HIGH_SHORT_CVG");
	    f.setRange(Range_t(ii->first, ii->second));
	    f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	    f.setType(Feature_t::MATEPAIR);
	    feat_stream << f;
	  }
	}
	cout << endl;
      }
    }

    
    if (! cestat){
      // find coverage by long mates
      
      getCvg(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, MP_LONG, *scf, interest, scaffspan[*scf], MAX_LONG_CVG, true); 
      
      // report interesting ranges
      
      if (interest.size() > 0){
	cout << "Regions of " << MAX_LONG_CVG << "X or more long mate coverage" 
	     <<endl;
	
	for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	     ii != interest.end(); ii++){
	  cout << ii->first << "\t" << ii->second << endl;
	  if (feats){
	    Feature_t f;
	    f.setComment("HIGH_LONG_CVG");
	    f.setRange(Range_t(ii->first, ii->second));
	    f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	    f.setType(Feature_t::MATEPAIR);
	    feat_stream << f;
	  }
	}
	cout << endl;
      }
    }
    
    // find coverage by normal reads

    getCvg(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, MP_NORMAL, *scf, interest, scaffspan[*scf], MAX_NORMAL_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_NORMAL_CVG 
	   << "X or more same-orientation coverage" <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_NORMAL_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    getCvg(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, MP_OUTIE, *scf, interest, scaffspan[*scf], MAX_OUTIE_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_OUTIE_CVG << "X or more outie coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_OUTIE_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    // find coverage by linking reads
    getCvg(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, MP_LINKING, *scf, interest, scaffspan[*scf], MAX_LINKING_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_LINKING_CVG << "X or more linking coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_LINKING_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }

    // find coverage by single mate reads
    getCvg(scf2frag[*scf].begin(), scf2frag[*scf].end(), rd2scaff, MP_SINGMATE, *scf, interest, scaffspan[*scf], MAX_SINGLEMATE_CVG, true); 

    // report interesting ranges

    if (interest.size() > 0){
      cout << "Regions of " << MAX_SINGLEMATE_CVG << "X or more singleton mate coverage" 
	   <<endl;
      
      for (list<pair<Pos_t, Pos_t> >::iterator ii = interest.begin(); 
	   ii != interest.end(); ii++){
	cout << ii->first << "\t" << ii->second << endl;
	if (feats){
	  Feature_t f;
	  f.setComment("HIGH_SINGLEMATE_CVG");
	  f.setRange(Range_t(ii->first, ii->second));
          f.setSource(make_pair(*scf, Scaffold_t::NCODE));
	  f.setType(Feature_t::MATEPAIR);
          feat_stream << f;
	}
      }
      cout << endl;
    }
  } // for each scaffold



  frag_bank.close();
  read_bank.close();
  library_bank.close();
  feat_stream.close();
  if (perinsert)
    insertFile.close();
  if (globals.find("ceplot") != globals.end())
    ceplotFile.close();

  return(0);
} // main
    
