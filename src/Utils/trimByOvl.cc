// $Id$

// trimByOvl.cc - uses the overlaps between reads to decide trimming
// coordinates

//#include <Overlap_AMOS.hh>
#include "datatypes_AMOS.hh"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <stdlib.h>

using namespace std;
using namespace AMOS;

#define MAXOVL 10  // max overlap between ranges

struct olap_t {
  //  long idA, idB;
  Range_t rangeA, rangeB;
  bool forw;
  //  int lenA, lenB;
  float perror;
}; 

struct position_t {
  Pos_t x;
  bool start; // true - start of range, false - end of range
  position_t (Pos_t xx, bool ss) : x(xx), start(ss) {};
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

struct cmpPos : public binary_function<position_t, position_t, bool> {
  bool operator () (position_t x, position_t y)
  {
    if (x.x < y.x)
      return true;
    if (x.x == y.x && x.start && !y.start)
      return true;  // beginnings come before ends
    return false;
  }
};

int main(int argc, char **argv)
{
  
  if (argc < 2 || argc > 3){
    cerr << "Usage: trimByOvl file.ovl [n]" << endl;
    exit(1);
  }


  // main assumption here is that the incoming ids are a contiguous set of 
  // integers starting at 0 (or 1)
  vector<vector<olap_t> > olapmap;
  vector<long int> how_many(0);

  if (argc == 3){
    olapmap.reserve(strtol(argv[2], NULL, 10) + 1);
    olapmap.resize(strtol(argv[2], NULL, 10) + 1);
    how_many.reserve(strtol(argv[2], NULL, 10) + 1);
    how_many.resize(strtol(argv[2], NULL, 10) + 1, 0);
  }

  ifstream inFile(argv[1]);

  if (inFile == (ifstream *) NULL){
    cerr << "Cannot open input file: " << argv[1] << endl;
    exit(1);
  }

  long whichLine = 0;
  string inputLine;

  cerr << "counting overlaps" << endl;
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
    
    how_many[idA]++;
    how_many[idB]++;
  }


  cerr << "Reserving memory" << endl;
  for (int i = 0; i < how_many.size(); i++){
    olapmap[i].reserve(how_many[i]);
    //    olapmap[i].resize(how_many[i]);
  }
  

  cerr << "Rereading the file " << endl;
  

  inFile.clear();
  inFile.seekg(0, ios::beg);
  whichLine = 0;

  while (getline(inFile, inputLine)){
    whichLine++;

    //    cerr << "whichLine" << endl;
    //    cerr << "Line is " << inputLine << endl;
    
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

    if (startA == endA || startB == endB){
      cerr << "Line " << whichLine << " in the input seems incorrect: " << endl << inputLine <<endl;
      continue;
    }
    overlap.perror = perror;
    overlap.forw = (dir == 'f');
    overlap.rangeA = Range_t(startA, endA);
    overlap.rangeB = Range_t(startB, endB);
    
    if (idA >= olapmap.capacity())
      olapmap.resize(idA + 1);

    //    cerr << "Adding to " << idA << " " << overlap.rangeA.getBegin() << ", " << overlap.rangeA.getEnd() <<endl;

    olapmap[idA].push_back(overlap);
    
    if (startB < endB){
      overlap.rangeA = Range_t(startB, endB);
      overlap.rangeB = Range_t(startA, endA);
    } else {
      overlap.rangeA = Range_t(endB, startB);
      overlap.rangeB = Range_t(endA, startA);
    }
    overlap.perror = perror;
    overlap.forw = (dir == 'f');
    
    if (idB >= olapmap.capacity())
      olapmap.resize(idB + 1);
    
    //    cerr << "Adding to " << idB << " " << overlap.rangeA.getBegin() << ", " << overlap.rangeA.getEnd() <<endl;
    olapmap[idB].push_back(overlap);
  }
  
  if (whichLine == 0){
    cerr << "didn't read any lines" << endl;
    exit(1);
  }

  inFile.close();

  cerr << "Trimming the reads" << endl;

  for (int i = 0; i < olapmap.size(); i++){
    if (i % 10000 == 0)
      cerr << i << "\r";
    
    vector<position_t> allpos;
    vector<position_t> shortpos;

    for (vector<olap_t>::iterator j = olapmap[i].begin(); j != olapmap[i].end(); j++){
      //      cerr << "Adding " << j->rangeA.getBegin() << ", " << 
      //	j->rangeA.getEnd() << endl;
      allpos.push_back(position_t(j->rangeA.getBegin(), true));
      allpos.push_back(position_t(j->rangeA.getEnd(), false));
      shortpos.push_back(position_t((j->rangeA.getBegin() + MAXOVL), true));
      shortpos.push_back(position_t(j->rangeA.getEnd(), false));
    }

    sort(allpos.begin(), allpos.end(), cmpPos());
    sort(shortpos.begin(), shortpos.end(), cmpPos());

    if (allpos.size() != shortpos.size())
      cerr << "Allpos and shortpos for " << i << " have different sizes " <<
	allpos.size() << " != " << shortpos.size() << endl;


    cout << "cov " << i << " ";
    //    cerr << "cov " << i << " ";

    int cov = 0;
    for (int j = 0; j < allpos.size(); j++){
      if (j > 0) 
	cout << "(" << allpos[j - 1].x << "," << allpos[j].x << ")=" 
	     << cov << " ";
      if (allpos[j].start) { // beginning of a range
	//	cerr << j << " " << allpos[j].x << " start ";
	//	if (cov == 0)
	  //	  cout << allpos[j].x << " ";
	cov++;
      } else { // end of range
	//	cerr << j << " " << allpos[j].x << " end ";
	cov--;
	//	if (cov == 0)
	  //	  cout << allpos[j].x << " B ";
      }
    }
    
    //   cerr << endl;
    cout << endl;
    
    cout << "chi " << i << " ";
    //    cerr << "chi " << i << " ";
    cov = 0;
    for (int j = 0; j < shortpos.size(); j++){
      if (j > 0) 
	cout << "(" << shortpos[j - 1].x << "," << shortpos[j].x << ")=" 
	     << cov << " ";
      if (shortpos[j].start) { // beginning of a range
	//	cerr << j << " " << allpos[j].x << " start ";
	//	if (cov == 0)
	// cout << shortpos[j].x << " ";
	cov++;
      } else { // end of range
	//	cerr << j << " " << allpos[j].x << " end ";
	cov--;
	//	if (cov == 0)
	// cout << shortpos[j].x << " B ";
      }
    }
    //    cerr << endl;
    cout << endl;
    

//     // we sort the ranges by leftmost coordinate along the read
//     sort(olapmap[i].begin(), olapmap[i].end(), cmpOlapByA());
//     // build a set of contiguous ranges
//     // pick the longest one
//     // within it, find the leftmost reverse hit - set as 5'
//     // set rightmost end as 3'

//     // output id, 5', 3', and quality: A - one range, 5' set by reverse
//     // B - one range, no reverse hits
//     // C - multiple ranges, 5' set by reverse
//     // D - multiple ranges, no reverse hits in best range
//     // E - no overlaps

//     Pos_t start = 0, end = -1;
//     vector<Range_t> ranges;
//     vector<Pos_t> rev5p;
//     int max_range_id = -1;
//     Pos_t max_range_len = 0;
//     bool found_rev = false;
//     for (vector<olap_t>::iterator o = olapmap[i].begin(); 
// 	 o != olapmap[i].end(); o++){
//       Pos_t rb, re;
//       rb = o->rangeA.getBegin();
//       re = o->rangeA.getEnd();
//       if (! o->forw && !found_rev){
// 	rev5p.push_back(rb); // remember first 5p location due to rev read
// 	found_rev = true;
//       }
//       if (rb > end){ // starting new range
// 	if (end != -1){
// 	  ranges.push_back(Range_t(start, end));
// 	  if (end - start > max_range_len){
// 	    max_range_len = end - start;
// 	    max_range_id = ranges.size() - 1;
// 	  }
// 	}
// 	start = rb;
// 	if (! found_rev)
// 	  rev5p.push_back(-1);
// 	found_rev = false;
//       }
//       if (re > end)
// 	end = re;
//     } // for each sorted overlap

//     if (end != -1){
//       ranges.push_back(Range_t(start, end));
//       if (end - start > max_range_len){
// 	max_range_len = end - start;
// 	max_range_id = ranges.size() - 1;
//       }
//       if (! found_rev)
// 	rev5p.push_back(-1);
//     }

//     // ranges contains all the ranges found

//     if (ranges.empty()) // no overlaps
//       cout << i << "\t" << 0 << "\t" << 0 << "\t" << "E" << "\n";
//     else {
//       if (rev5p[max_range_id] == -1)
// 	cout << i << "\t" << ranges[max_range_id].getBegin() << "\t" 
// 	     << ranges[max_range_id].getEnd() << "\t" 
// 	     << ((ranges.size() == 1) ? "B" : "D") << "\n";
//       else 
// 	cout << i << "\t" << rev5p[max_range_id] << "\t" 
// 	     << ranges[max_range_id].getEnd() 
// 	     << "\t" << ((ranges.size() == 1) ? "A" : "C") << "\n";
//     }
  } // while each overlap for this read

  exit(0);
}
