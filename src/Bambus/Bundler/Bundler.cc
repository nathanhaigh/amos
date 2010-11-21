// $Id$

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <map>
#include <math.h>
#include <stdio.h>
#include <list>
#include <iostream>
#include <getopt.h>

#include "ContigLink_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

using namespace AMOS;
using namespace std;

typedef multimap<pair<ID_t, ID_t> , ContigLink_t > LinkMap_t;

map<string, string> globals; // global variables
ID_t EdgeId = 0;             // where numbering starts for edges

void printHelpText()
{
  cerr << 
    "\n"
    "Bundle links into edges among connected contigs\n"
    "\n"
    "USAGE:\n"
    "\n"
    "Bundler -b[ank] <bank_name> -t[ype] <comma separated list of link types to process>\n"
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
    {"t",     1, 0, 't'},
    {"type",  1, 0, 't'},
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
    case 't':
      globals["type"] = string(optarg);
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions


int main(int argc, char *argv[])
{
  LinkMap_t linkMap;

  if (!GetOptions(argc, argv)){
    cerr << "Command line parsing failed" << endl;
    printHelpText();
    exit(1);
  }

  if (globals.find("bank") == globals.end()){ // no bank was specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }
  
  if (globals.find("type") == globals.end()) {
     globals["type"] = "ALL";
  }


  BankStream_t link_bank (ContigLink_t::NCODE);
  try {
    if (! link_bank.exists(globals["bank"])) {
      cerr << "Cannot find a contig link account in bank: " << globals["bank"] << endl;
      exit(1);
    }
    link_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open link account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }

  BankStream_t edge_bank (ContigEdge_t::NCODE);
  try {
    if (! edge_bank.exists(globals["bank"])) 
      edge_bank.create(globals["bank"]);
    else 
      edge_bank.open(globals["bank"]);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open edge account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }  

  Size_t ti;
  Size_t minlen, maxlen;  // contig adjacency gap range;


  ContigLink_t ctl;

  while (link_bank >> ctl){
    ID_t ctgA, ctgB;
    
    ctgA = ctl.getContigs().first;
    ctgB = ctl.getContigs().second;

    if (linkMap.find(pair<ID_t, ID_t>(ctgB, ctgA)) != linkMap.end()){
      // must reverse the link
      ctl.flip();
    } 
    
    // make sure we accept this type
    if (globals["type"].find(ctl.getType()) != string::npos || globals["type"].find("ALL") != string::npos) {
       linkMap.insert(pair<pair<ID_t, ID_t>, ContigLink_t>
   		   (ctl.getContigs(), ctl));
    }
  }

  // now all the links should be in the linkMap, nicely grouped by the contigs
  // they connect.

  map <LinkAdjacency_t, int> adjacencies;
  LinkMap_t::iterator li = linkMap.begin();
  while (li != linkMap.end()){
    pair<ID_t, ID_t> lastPair = (*li).first;
    cerr << "Doing contig pair (" << lastPair.first << ", " 
	 << lastPair.second << ")" << endl; 
    map <LinkAdjacency_t, list<LinkMap_t::iterator> > adjList;
    map <LinkAdjacency_t, list<LinkMap_t::iterator> > bestList;
    adjacencies.clear();  // make sure we don't carry unnecessary info from before
    while (li->first == lastPair){
      LinkAdjacency_t a = li->second.getAdjacency();

      // handle one pair of contigs
      adjacencies[a]++;
      adjList[a].push_back(li);
    
      assert(adjacencies[a] == adjList[a].size());
      li++;
    }

    cerr << "NORMALS: " << adjacencies[(LinkAdjacency_t)ContigLink_t::NORMAL]
	 << " ANTINORMALS: " << adjacencies[(LinkAdjacency_t)ContigLink_t::ANTINORMAL]
	 << " INNIES: " << adjacencies[(LinkAdjacency_t)ContigLink_t::INNIE]
	 << " OUTIES: " << adjacencies[(LinkAdjacency_t)ContigLink_t::OUTIE]
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
      	multimap<Size_t, LinkMap_t::iterator> begins, ends;
      	list<LinkMap_t::iterator> best;
      	list<LinkMap_t::iterator>::iterator bestEnd = best.end();
      	Size_t bestCoord;
      	int ncurrent = 0, nbest = 0;
      
      	cerr << "Doing " << at->second 
      	     << " links of type " << at->first << endl;
      
      	cerr << " Found " << adjList[at->first].size() << " links " << endl;
      
      	assert (at->second == adjList[at->first].size());
      
      	for (list<LinkMap_t::iterator>::iterator 
      	       lnks = adjList[at->first].begin();
      	     lnks != adjList[at->first].end(); lnks++){
      	  begins.insert(pair<Size_t, LinkMap_t::iterator> 
      			(((**lnks).second.getSize() - 3 * (**lnks).second.getSD()), *lnks));
      	  ends.insert(pair<Size_t, LinkMap_t::iterator>
      		      (((**lnks).second.getSize() + 3 * (**lnks).second.getSD()), *lnks));
   	  } // for each link

      	// because of the way maps work, the elements come sorted by the key
      	multimap<Size_t, LinkMap_t::iterator>::iterator bgi = begins.begin();
      	multimap<Size_t, LinkMap_t::iterator>::iterator eni = ends.begin();
	
	     cerr << " Begins has " << begins.size() << " elements" << endl;
   	  cerr << " Ends has " << ends.size() << " elements" << endl;
	
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
	              cerr << "best coord is " << bestCoord << endl;
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
	           } else {
	              cerr << "link " << eniLend << ", " << eniRend << " did not surround " << bestCoord << endl;
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
	     cerr << "nbest is " << nbest << endl;
	     cerr << "best has " << best.size() << " records " << endl;
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
	        cerr << "Clique range = " << minRange << ", " << maxRange << endl;
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
	           cerr << "Link " << (**lnks).second.getIID() << " (" 
		             << (**lnks).second.getSize() << ", " 
		             << (**lnks).second.getSD() << ")"
		             << " is too short ( < " << minRange << ")" << endl;
	        if (sz - 3 * sd > maxRange)
	           cerr << "Link " << (**lnks).second.getIID() << " (" 
		             << (**lnks).second.getSize() << ", " 
		             << (**lnks).second.getSD() << ")"
		             << " is too long ( > " << maxRange << ")" << endl;
        } // for each link
	
      } else { // all links are good
      	for (list<LinkMap_t::iterator>::iterator 
      	       bi = adjList[at->first].begin();
      	     bi != adjList[at->first].end(); bi++){
      	  cerr << "Link " << (**bi).second.getIID() << " (" 
      	       << (**bi).second.getSize() << ", " 
      	       << (**bi).second.getSD() << ")"
      	       << " is just right" << endl;
      	  bestList[at->first].push_back(*bi);
      	}
      } // if enough links exist

      if (at->second > bestAdjCount){
      	bestAdjCount = at->second;
      	bestAdj = at->first;
      }
    } // for each adjacency
    
    cerr << "new NORMALS: " << adjacencies[(LinkAdjacency_t)ContigLink_t::NORMAL]
	 << " ANTINORMALS: " << adjacencies[(LinkAdjacency_t)ContigLink_t::ANTINORMAL]
	 << " INNIES: " << adjacencies[(LinkAdjacency_t)ContigLink_t::INNIE]
	 << " OUTIES: " << adjacencies[(LinkAdjacency_t)ContigLink_t::OUTIE]
	 << "\n";


    for (map<LinkAdjacency_t, int>::iterator at = adjacencies.begin();
	 at != adjacencies.end(); at++){
      if (at->second >= 1){ // there are links we have to deal with
      	ContigEdge_t cte;
      	bestAdj = at->first;
      	
      	// import adjacency info from first link
      	list<LinkMap_t::iterator>::iterator li = bestList[bestAdj].begin();
      	cte.setContigs((**li).second.getContigs());
      	cte.setAdjacency((**li).second.getAdjacency());
      
      	vector<ID_t> linkIds;
      	linkIds.reserve(bestList[bestAdj].size());  // try to keep memory low - best to prealloc
      	
          // here we want to combine the gaussians described by the "good" links
          // use the idea of Huson et al. RECOMB 2001
      	float newMean, newSD, p = 0, q = 0;
      	for (list<LinkMap_t::iterator>::iterator ni = bestList[bestAdj].begin();
      	     ni != bestList[bestAdj].end(); ni++){
      	  float tmp = (**ni).second.getSD();
      	  if (tmp == 0) tmp = 1; // avoid overflow errors
      	  tmp *= tmp;
      	  
      	  p += (**ni).second.getSize() / tmp;
      	  q += 1 / tmp; 
      	  linkIds.push_back((**ni).second.getIID());
   	}
	
	newMean = p / q;
	newSD = 1 / sqrt(q);
	
	// set size as aggregate of all links
	cte.setSize((int)newMean);
	cte.setSD((int)newSD);
	
	// set contig link IDs
	cte.setContigLinks(linkIds);
	
	// give the edge an identifier
	cte.setIID(++EdgeId);
	
        if (!edge_bank.existsIID(cte.getIID())) {
           cerr << "Writing edge link with it " << cte.getIID() << endl;
   	   edge_bank << cte;
        }
      } // if enough links
    } // for each adjacency type
  } // for each contig pair

  edge_bank.close();
  link_bank.close();
  return(0);
} // main

