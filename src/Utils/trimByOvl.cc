// $Id$

// trimByOvl.cc - uses the overlaps between reads to decide trimming
// coordinates

//#include <Overlap_AMOS.hh>
#include <iostream>
#include <fstream>
#include <datatypes_AMOS.hh>
#include <map>
#include <vector>
#include <stdio.h>
#include <algorithm>

using namespace std;
using namespace AMOS;

struct olap_t {
  long idA, idB;
  Range_t rangeA, rangeB;
  bool forw;
  int lenA, lenB;
  float perror;
}; 

struct cmpOlapByA : public binary_function<olap_t, olap_t, bool> {
  bool operator()(olap_t x, olap_t y) 
  { 
    if (x.rangeA.begin < y.rangeA.begin)
      return true;
    if (x.rangeA.begin > y.rangeA.begin)
      return false;
    if (x.rangeA.end < y.rangeA.end)
      return true;
    return false;
  }
};

int main(int argc, char **argv)
{
  
  if (argc != 2){
    cerr << "Usage: Bundler file.ovl" << endl;
    exit(1);
  }

  map<long, vector<olap_t> > olapmap;

  ifstream inFile(argv[1]);

  if (inFile == (ifstream *) NULL){
    cerr << "Cannot open input file: " << argv[1] << endl;
    exit(1);
  }

  long whichLine = 0;
  string inputLine;
  while (getline(inFile, inputLine)){
    whichLine++;
    
    if (whichLine % 10000 == 0)
      cerr << whichLine << "\r";

    long idA, idB;
    char dir;
    int startA, startB, endA, endB, lenA, lenB;
    float perror;
    olap_t overlap;

    if (sscanf(inputLine.c_str(), "%ld %ld %c %d %d %d %d %d %d %f", 
	       &idA, &idB, &dir, &startA, &endA, &lenA, 
	       &startB, &endB, &lenB, &perror) != 10){
      cerr << "Cannot parse line " << whichLine << " in the input:" << endl << inputLine << endl;
      continue;
    }

    if (endA - startA >= 150){
      
      overlap.idA = idA;
      overlap.idB = idB;
      overlap.lenA = lenA;
      overlap.lenB = lenB;
      overlap.rangeA = Range_t(startA, endA);
      overlap.rangeB = Range_t(startB, endB);
      overlap.perror = perror;
      overlap.forw = (dir == 'f');
      
      olapmap[idA].push_back(overlap);
      
      overlap.idA = idB;
      overlap.idB = idA;
      overlap.lenA = lenB;
      overlap.lenB = lenA;
      if (startB < endB){
	overlap.rangeA = Range_t(startB, endB);
	overlap.rangeB = Range_t(startA, endA);
      } else {
	overlap.rangeA = Range_t(endB, startB);
	overlap.rangeB = Range_t(endA, startA);
      }
      overlap.perror = perror;
      overlap.forw = (dir == 'f');
	  
      olapmap[idB].push_back(overlap);
    }
  }

  inFile.close();

  for (map<long, vector<olap_t> >::iterator i = olapmap.begin(); 
       i != olapmap.end(); i++){
    
    // we sort the ranges by leftmost coordinate along the read
    sort(i->second.begin(), i->second.end(), cmpOlapByA());
    // build a set of contiguous ranges
    // pick the longest one
    // within it, find the leftmost reverse hit - set as 5'
    // set rightmost end as 3'

    // output id, 5', 3', and quality: A - one range, 5' set by reverse
    // B - one range, no reverse hits
    // C - multiple ranges, 5' set by reverse
    // D - multiple ranges, no reverse hits in best range
    // E - no overlaps
    Pos_t start = 0, end = -1;
    vector<Range_t> ranges;
    vector<Pos_t> rev5p;
    int max_range_id = -1;
    Pos_t max_range_len = 0;
    bool found_rev = false;
    for (vector<olap_t>::iterator o = i->second.begin(); 
	 o != i->second.end(); o++){
      Pos_t rb, re;
      rb = o->rangeA.getBegin();
      re = o->rangeA.getEnd();
      if (! o->forw && !found_rev){
	rev5p.push_back(rb); // remember first 5p location due to rev read
	found_rev = true;
      }
      if (rb > end){ // starting new range
	if (end != -1){
	  ranges.push_back(Range_t(start, end));
	  if (end - start > max_range_len){
	    max_range_len = end - start;
	    max_range_id = ranges.size() - 1;
	  }
	}
	start = rb;
	if (! found_rev)
	  rev5p.push_back(-1);
	found_rev = false;
      }
      if (re > end)
	end = re;
    } // for each sorted overlap
    if (end != -1){
      ranges.push_back(Range_t(start, end));
      if (end - start > max_range_len){
	max_range_len = end - start;
	max_range_id = ranges.size() - 1;
      }
      if (! found_rev)
	rev5p.push_back(-1);
    }

    // ranges contains all the ranges found

    if (ranges.empty()) // no overlaps
      cout << i->first << "\t" << 0 << "\t" << 0 << "\t" << "E" << "\n";
    else {
      if (rev5p[max_range_id] == -1)
	cout << i->first << "\t" << ranges[max_range_id].getBegin() << "\t" 
	     << ranges[max_range_id].getEnd() << "\t" 
	     << ((ranges.size() == 1) ? "B" : "D") << "\n";
      else 
	cout << i->first << "\t" << rev5p[max_range_id] << "\t" 
	     << ranges[max_range_id].getEnd() 
	     << "\t" << ((ranges.size() == 1) ? "A" : "C") << "\n";
    }
  } // while each overlap for this read

  exit(0);
}
