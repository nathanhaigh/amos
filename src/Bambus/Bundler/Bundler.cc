// $Id$

#include "Bundler.hh"
#include <stdio.h>
#include <list>

using namespace AMOS;
using namespace std;

ID_t getID(char*);

int main(int argc, char *argv[])
{
  LinkMap_t linkMap;
  IdMap_t contigIdMap;
  ContigLen_t contigLenMap;

  if (argc != 2){
    cerr << "Usage: Bundler file.inp" << endl;
    exit(1);
  }

  ifstream inFile(argv[1]);

  if (inFile == (ifstream *) NULL){
    cerr << "Cannot open input file: " << argv[1] << endl;
    exit(1);
  }

  string inputLine;
  int whichLine = 0;
  char ts[256];
  Size_t ti;
  
  char tlnm[256], tasmA[256], tasmB[256], 
    toriA[256], toriB[256], tlib[256], tolap[256];
  Size_t minlen, maxlen;  // contig adjacency gap range;
  

  while (getline(inFile, inputLine)){
    whichLine++;

    // skip comments
    if (inputLine[0] == '#'){
      continue;
    }
    
    if (sscanf(inputLine.c_str(), "contig %s %d", ts, &ti) == 2){
      // contig record
      ID_t cid;
      cid = getID(ts);
      contigIdMap[ts] = cid;
      contigLenMap[cid] = ti;
      //      cout << "contig " << cid << " (" << ts << ") has length " << ti << endl;
    } else if (sscanf(inputLine.c_str(), 
		      "link %s %s %s %s %s %s %d %d %s", 
		      tlnm, tasmA, toriA, tasmB, toriB, tlib, 
		      &minlen, &maxlen, tolap) == 9){ 
      ID_t ctgA, ctgB;
      // link record
      if (contigIdMap.find(tasmA) == contigIdMap.end()){
	cerr << "Contig " << tasmA << " not defined at line " 
	     << whichLine << endl;
	exit(1);
      }
      if (contigIdMap.find(tasmB) == contigIdMap.end()){
	cerr << "Contig " << tasmB << " not defined at line " 
	     << whichLine << endl;
	exit(1);
      }

      ctgA = contigIdMap.find(tasmA)->second;
      ctgB = contigIdMap.find(tasmB)->second;

      ContigLink_t cl;

      if (       (string) toriA == "BE" && (string) toriB == "BE"){
	// type is normal
	cl.setAdjacency(ContigLink_t::NORMAL);
      } else if ((string) toriA == "BE" && (string) toriB == "EB"){
	// type is innie 
	cl.setAdjacency(ContigLink_t::INNIE);
      } else if ((string) toriA == "EB" && (string) toriB == "EB"){
	// type is antinormal
	cl.setAdjacency(ContigLink_t::ANTINORMAL);
      } else if ((string) toriA == "EB" && (string) toriB == "BE"){
	// type is outie
	cl.setAdjacency(ContigLink_t::OUTIE);
      }

      if (linkMap.find(pair<ID_t, ID_t>(ctgB, ctgA)) != linkMap.end()){
	// must reverse the link
	cl.setContigs(pair<ID_t, ID_t>(ctgB, ctgA));
	if (cl.getAdjacency() == ContigLink_t::NORMAL){
	  cl.setAdjacency(ContigLink_t::ANTINORMAL);
	} else if (cl.getAdjacency() == ContigLink_t::ANTINORMAL){
	  cl.setAdjacency(ContigLink_t::NORMAL);
	}
      } else {
	cl.setContigs(pair<ID_t, ID_t>(ctgA, ctgB));
      }

      cl.setSize((maxlen + minlen) / 2);
      cl.setSD((maxlen - minlen) / 6);
      cl.setType(ContigLink_t::MATEPAIR);
      cl.setComment((string)tlnm);

      linkMap.insert(pair<pair<ID_t, ID_t>, ContigLink_t>
		     (cl.getContigs(), cl));
    } else if (inputLine.find_first_not_of(" 	") == -1){ // empty lines OK
      // the string in the find is "SPC TAB"
      // do nothing anyway...
    } else { 
      cerr << "Malformed line " << whichLine << " in input: " 
	   << inputLine << endl;
      // should I crash here?  
    }
  }
  cerr << "Read " << whichLine << " lines from input file" << endl;
  
  // now all the links should be in the linkMap, nicely grouped by the contigs
  // they connect.

  LinkMap_t::iterator li = linkMap.begin();
  while (li != linkMap.end()){
    pair<ID_t, ID_t> lastPair = (*li).first;
    cerr << "Doing contig pair (" << lastPair.first << ", " 
	 << lastPair.second << ")" << endl; 
    list <ContigLink_t> linkList;
    map <LinkAdjacency_t, int> adjacencies;
    map <LinkAdjacency_t, list<LinkMap_t::iterator> > adjList;
    map <LinkAdjacency_t, list<LinkMap_t::iterator> > bestList;
    while ((*li).first == lastPair){
      // handle one pair of contigs
      linkList.push_back(li->second);
      adjacencies[li->second.getAdjacency()]++;
      adjList[li->second.getAdjacency()].push_back(li);
      li++;
    }
    cerr << "NORMALS: " << adjacencies[ContigLink_t::NORMAL]
	 << " ANTINORMALS: " << adjacencies[ContigLink_t::ANTINORMAL]
	 << " INNIES: " << adjacencies[ContigLink_t::INNIE]
	 << " OUTIES: " << adjacencies[ContigLink_t::OUTIE]
	 << "\n";
    // now, for each innie, outie and such we find a consistent subset of links
    // using as min and max mean -/+ 3*stdev

    // we'll use Dan Sommer's algorithm for keeping track of the maximum clique
    // Specifically, for each maximal clique we track the number of links in 
    // the clique, the coordinate (beginning or end) of the clique, and we 
    // track the links belonging to the maximum clique as they get removed from
    // it.
    LinkAdjacency_t bestAdj;
    int bestAdjCount = 0;
    for (map<LinkAdjacency_t, int>::iterator at = adjacencies.begin();
	 at != adjacencies.end(); at++){
      if (at->second > 1){ // there are links we have to deal with
	cerr << "Doing " << at->second 
	     << " links of type " << at->first << endl;
	map<Size_t, LinkMap_t::iterator> begins, ends;
	list<LinkMap_t::iterator> best;
	list<LinkMap_t::iterator>::iterator bestEnd = best.end();
	Size_t bestCoord;
	int ncurrent = 0, nbest = 0;
	for (list<LinkMap_t::iterator>::iterator 
	       lnks = adjList[at->first].begin();
	     lnks != adjList[at->first].end(); lnks++){
	  begins.insert(pair<Size_t, LinkMap_t::iterator> 
			(((**lnks).second.getSize() - 3 * (**lnks).second.getSD()), *lnks));
	  ends.insert(pair<Size_t, LinkMap_t::iterator>
		      (((**lnks).second.getSize() + 3 * (**lnks).second.getSD()), *lnks));
	} // for each link
	// because of the way maps work, the elements come sorted by the key
	
	map<Size_t, LinkMap_t::iterator>::iterator bgi = begins.begin();
	map<Size_t, LinkMap_t::iterator>::iterator eni = ends.begin();
	
	while (bgi != begins.end() || eni != ends.end()){
	  if (bgi != begins.end() && bgi->first <= eni->first){
	    Size_t bgiLend = bgi->second->second.getSize() 
	      - 3 * bgi->second->second.getSD(),
	      bgiRend = bgi->second->second.getSize() 
	      + 3 * bgi->second->second.getSD();
	    // we add to the clique
	    ncurrent++;
	    if (ncurrent > nbest){
	      nbest = ncurrent;
	      bestEnd = best.begin(); // reset best list
	      bestCoord = bgiLend; // where the maximum occured
	    }
	    bgi++;
	  } else if (eni != ends.end() && 
		     (bgi == begins.end() || 
		      bgi->first > eni->first)){
	    Size_t eniLend = eni->second->second.getSize() 
	      - 3 * eni->second->second.getSD(),
	      eniRend = eni->second->second.getSize() 
	      + 3 * eni->second->second.getSD();

	    if (eniLend <= bestCoord && eniRend >= bestCoord){ 
	      // current link was in best clique
	      best.push_front(eni->second);
	    }

	    // we remove from the clique
	    ncurrent--;
	    eni++;
	    //	  map<Size_t, LinkMap_t::iterator>::iterator rm = ends.begin();
	    //	  ends.erase(rm);
	  } 
	} // while bgi != begins.end()
	// best is a map containing all the links in the current clique
	at->second = nbest;
	// keep just the good
	//	adjList[at->first].clear();
	Size_t minRange, maxRange;
	list<LinkMap_t::iterator>::iterator bg = best.begin();
	Size_t sz = (**bg).second.getSize();
	Size_t sd = (**bg).second.getSD();
	minRange = sz - 3 * sd;
	maxRange = sz + 3 * sd;
	for (list<LinkMap_t::iterator>::iterator bi = best.begin();
	     bi != bestEnd; bi++){
	  bestList[at->first].push_back(*bi);
	  sz = (**bi).second.getSize();
	  sd = (**bi).second.getSD();
	  if (sz - 3 * sd > minRange)
	    minRange = sz - 3 * sd;
	  if (sz + 3 * sd < maxRange)
	    maxRange = sz + 3 * sd;
	}
	// now minRange-maxRange is the range of the clique.
	// we can mark all links that end before minRange or start after
	// maxRange as incorrect
	for (list<LinkMap_t::iterator>::iterator 
	       lnks = adjList[at->first].begin();
	     lnks != adjList[at->first].end(); lnks++){
	  sz = (**lnks).second.getSize();
	  sd = (**lnks).second.getSD();
	  if (sz + 3 * sd < minRange)
	    cerr << "Link " << (**lnks).second.getComment() << " (" 
		 << (**lnks).second.getSize() << ", " 
		 << (**lnks).second.getSD() << ")"
		 << " is too short ( < " << minRange << ")" << endl;
	  if (sz - 3 * sd > maxRange)
	    cerr << "Link " << (**lnks).second.getComment() << " (" 
		 << (**lnks).second.getSize() << ", " 
		 << (**lnks).second.getSD() << ")"
		 << " is too long ( > " << maxRange << ")" << endl;
	} // for each link
	
      } else { // all links are good
	for (list<LinkMap_t::iterator>::iterator 
	       bi = adjList[at->first].begin();
	     bi != adjList[at->first].end(); bi++){
	  bestList[at->first].push_back(*bi);
	}
      }// if enough links exist
      if (at->second > bestAdjCount){
	bestAdjCount = at->second;
	bestAdj = at->first;
      }
    } // for each adjacency
    
    cerr << "new NORMALS: " << adjacencies[ContigLink_t::NORMAL]
	 << " ANTINORMALS: " << adjacencies[ContigLink_t::ANTINORMAL]
	 << " INNIES: " << adjacencies[ContigLink_t::INNIE]
	 << " OUTIES: " << adjacencies[ContigLink_t::OUTIE]
	 << "\n";

    // here we want to combine the gaussians described by the "good" links
    // use the idea of Huson et al. RECOMB 2001

    float newMean, newSD, p = 0, q = 0;
    
    for (list<LinkMap_t::iterator>::iterator ni = bestList[bestAdj].begin();
	 ni != bestList[bestAdj].end(); ni++){
      float tmp = (**ni).second.getSD();
      tmp *= tmp;
      
      p += (**ni).second.getSize() / tmp;
      q += 1 / tmp; 
      cerr << "p is " << p << " and q is " << q << endl;
    }
    

    newMean = p / q;
    newSD = 1 / sqrt(q);

    cout << lastPair.first << "\t" << lastPair.second << "\t" 
	 << bestAdj << "\t" << newMean << "\t" << newSD << "\t" 
	 << bestAdjCount << endl;

  } // for each contig pair

} // main


ID_t getID(char * str)
{
  ID_t ret;

  if (sscanf(str, "%d", &ret) == 1)
    return ret;
  if (sscanf(str, "contig_%d", &ret) == 1) 
    return ret;
  if (sscanf(str, "scaff_%d", &ret) == 1)
    return ret;
  

  // here I may want to make up IDs
  
  return ret;
}
