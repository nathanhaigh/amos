#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>
#include <math.h>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <iterator>
#include <iostream>

#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "ContigIterator_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"

#include "Utilities_Bundler.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;
using namespace Bundler;

// constants for maximum satisfied edge
static const int32_t  MAX_STDEV         =  3;

struct config {
   bool        initAll;
   bool        compressMotifs;
   bool        doAgressiveScaffolding;
   int         redundancy;
   int         debug;
   string      bank;
   string      prefix;
   set<ID_t>   repeats;  // repeat contigs to ignore (we ignore all links to them)
};
config globals;

ID_t maxContig = 0;

void printHelpText() {
   cerr << 
    "\n"
    "USAGE:\n"
    "\n"
    "OrientContigs -b[ank] <bank_name> [-prefix] <prefix> [-all] [-noreduce] [-agressive] [-redundancy minLinks] [-repeats fileName]\n"
    "The -prefix option specifies the prefix to give generated output files\n"
    "The -all option will force initialization of all contigs, including those that have no links to them, otherwise they remain uninitialized\n"
    "The -noreduce option will turn off search for common motifs and recursively remove them, thus simplyfing the graph\n"
    "The -agressive option will not mark edges that move a contig more than 3 STDEVS away as bad and will try to reconcile the positions\n"
    "The -redundancy option specifies the minimum number of links between two contigs before they will be scaffolded\n"
    "The -repeats option specifies a file containing a list of contig IIDs which are considered repeats and whose edges will be unused\n"
       << endl;
}

bool GetOptions(int argc, char ** argv) {
   int option_index = 0;      
   static struct option long_options[] = {
    {"help",               0, 0, 'h'},
    {"h",                  0, 0, 'h'},
    {"b",                  1, 0, 'b'},
    {"bank",               1, 0, 'b'},
    {"prefix",             1, 0, 'p'},
    {"all",                0, 0, 'a'},
    {"noreduce",           0, 0, 'n'},
    {"agressive",          0, 0, 'A'},
    {"redundancy",         1, 0, 'R'},
    {"repeats",            1, 0, 'r'},    
    {"debug",              1, 0, 'd'},
    {0, 0, 0, 0}
  };

   globals.initAll = false;
   globals.compressMotifs = true;
   globals.doAgressiveScaffolding = false;
   globals.debug = 1;
   globals.redundancy = 2;
   globals.prefix = "out";
  
   int c;
   ifstream repeatsFile;
   ID_t repeatID;
   
   while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1){
      switch (c){
      case 'h':
         printHelpText();
         break;
      case 'b':
         globals.bank = string(optarg);
         break;
      case 'p':
         globals.prefix = string(optarg);
         break;
      case 'a':
         globals.initAll = true;
         break;
      case 'n':
         globals.compressMotifs = false;
         break;
      case 'A':
         globals.doAgressiveScaffolding = true;
         break;
      case 'R':
         globals.redundancy = atoi(optarg);
         break;
      case 'r':
         // read file here
         repeatsFile.open(optarg, ios::in);
         if (!repeatsFile) {
            cerr << "Can't open repeats file, will not mask repeats " << optarg << endl;
            break;
         }
         while (!repeatsFile.eof()) {
           repeatsFile >> repeatID;
           globals.repeats.insert(repeatID);
         }         
         repeatsFile.close();
         break;
       case 'd':
         globals.debug = atoi(optarg);
         break;
      case '?':
         return false;
      }
   }

   // print summary
   if (globals.debug >= 1) {
      cerr << "Options summary:" << endl;
      cerr << "Bank = \t" << globals.bank << endl;
      cerr << "Redundancy = \t" << globals.redundancy << endl;
      cerr << "InitAll = \t" << globals.initAll << endl;
      cerr << "Compress = \t" << globals.compressMotifs << endl;
      cerr << "AgressiveScf = \t" << globals.doAgressiveScaffolding << endl;
   }
   
   return true;
} // GetOptions

void swapContigs(ContigEdge_t & cte, int isReversed) {
   pair<ID_t, ID_t> ctgs;
   ctgs.first = cte.getContigs().second;
   ctgs.second = cte.getContigs().first;
   cte.setContigs(ctgs);

   // set what we expect to see, this is used to determine if we should modify the edge adjacency type when we swap the elements
   int expected = 0;
   if (cte.getAdjacency() == ContigEdge_t::ANTINORMAL || cte.getAdjacency() == ContigEdge_t::OUTIE) {
      expected = 1;
   }

   if (isReversed != expected) {
      if (cte.getAdjacency() == ContigEdge_t::NORMAL) {
         cte.setAdjacency(ContigEdge_t::ANTINORMAL);
      } else if (cte.getAdjacency() == ContigEdge_t::ANTINORMAL) {
         cte.setAdjacency(ContigEdge_t::NORMAL);
      }
   } else {
      if (cte.getAdjacency() == ContigEdge_t::OUTIE) {
         cte.setAdjacency(ContigEdge_t::INNIE);
      } else if (cte.getAdjacency() == ContigEdge_t::INNIE) {
         cte.setAdjacency(ContigEdge_t::OUTIE);
      }
   }
}

LinkAdjacency_t getAdjacency(contigOrientation firstOrient, contigOrientation secondOrient, contigOrientation newFirst, contigOrientation newSecond, ContigEdge_t &cte) {
   if (globals.debug >= 3) {
      cerr << "We are orienting with first orient is " << firstOrient << " and new first is " << newFirst << " and second is " << secondOrient << " and second new is " << newSecond << endl;
   }

   if (firstOrient == newFirst && secondOrient == newSecond) {
      if (globals.debug >= 3) { cerr << "Nothing changed, do nothing" << endl; }
      return cte.getAdjacency();
   }
   else if (firstOrient == newFirst) {
      if (globals.debug >= 3) { cerr << "First orientation didn't change second did " << endl; }
      switch (cte.getAdjacency()) {
      case ContigEdge_t::NORMAL:
         return ContigEdge_t::INNIE;
      case ContigEdge_t::ANTINORMAL:
         return ContigEdge_t::OUTIE;
      case ContigEdge_t::INNIE:
         return ContigEdge_t::NORMAL;
      case ContigEdge_t::OUTIE:
         return ContigEdge_t::ANTINORMAL;
      default:
         cerr << "Unknown orientation " << cte.getAdjacency() << endl;
         exit(1);
      };
   }
   // the first orientation changed
   else if (secondOrient == newSecond) {
      if (globals.debug >= 3) { cerr << "Second orient didn't change first did" << endl; }
      switch (cte.getAdjacency()) {
      case ContigEdge_t::NORMAL:
         return ContigEdge_t::OUTIE;
      case ContigEdge_t::ANTINORMAL:
         return ContigEdge_t::INNIE;
      case ContigEdge_t::INNIE:
         return ContigEdge_t::ANTINORMAL;
      case ContigEdge_t::OUTIE:
         return ContigEdge_t::NORMAL;
      default:
         cerr << "Unknown orientation " << cte.getAdjacency() << endl;
         exit(1);
      };
   } else {
      cerr << "BOTH CONTIG ORIENTATIONS CHANGED, THAT IS NOT POSSIBLE!" << endl;
      assert(0);
   }
}

int32_t adjustSizeBasedOnAdjacency(LinkAdjacency_t edgeAdjacency, int32_t gapSize, 
                  Contig_t &first, Contig_t &second,
                  contigOrientation firstOrient, contigOrientation secondOrient) {   
   assert(firstOrient != NONE);

   int32_t size = gapSize;
   contigOrientation expectedOrient = NONE;
   switch (edgeAdjacency) {
      case ContigEdge_t::NORMAL:
         size += first.getLength();
         expectedOrient = FWD;
         break;
      case ContigEdge_t::ANTINORMAL:
         size += second.getLength();         
         expectedOrient = REV;
         break;
      case ContigEdge_t::INNIE:
         size += first.getLength();
         size += second.getLength();
         expectedOrient = FWD;
         break;
      case ContigEdge_t::OUTIE:
         // no adjustment for OUTIE
         expectedOrient = REV;
         break;
      default:
         cerr << "Unknown adjacency " << edgeAdjacency << endl;
         exit(1);
   };

   // place the second contig behind the first one if we are reversed
   if (firstOrient != expectedOrient) {
      size *= -1;
   }
   
   if (globals.debug >= 3) {
      cerr << "Size is " << size << " for edge between " << first.getIID() << " and " << second.getIID() << endl;   
   }
   
   return size;
}

int32_t computePosition(int32_t position, int32_t size) {
   if (position == UNINITIALIZED) {
      return UNINITIALIZED;
   }

   return position+size;
}

int32_t reconcilePositions(int32_t oldPosition, int32_t newPosition, SD_t stdev) {
   if (newPosition == UNINITIALIZED) {
      return oldPosition;
   } else if (oldPosition == UNINITIALIZED) {
      return newPosition;
   } else {
      int32_t adjustedPosition = (oldPosition+newPosition)/2;
      double dist = abs(adjustedPosition-newPosition);
      if (dist > MAX_STDEV*stdev) {
         if (globals.doAgressiveScaffolding == true) {
            // this would move us too far, compromise and move to max dist away
            adjustedPosition = adjustedPosition>newPosition ? newPosition + MAX_STDEV*stdev : newPosition + -1*MAX_STDEV*stdev;
         } else {
            return INVALID_EDGE;
         }
      }
      
      return adjustedPosition;
   }
}

int computeContigPositions(
      Contig_t &first, Contig_t &second, ContigEdge_t &cte,
      hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> >& ctg2srt,
      hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
   int32_t size = adjustSizeBasedOnAdjacency(
                     cte.getAdjacency(), 
                     cte.getSize(), 
                     first, 
                     second, 
                     ctg2ort[first.getIID()], 
                     ctg2ort[second.getIID()]);
   
   // compute the contig positions
   int32_t firstPosition = computePosition(ctg2srt[second.getIID()], -size);
   int32_t secondPosition = computePosition(ctg2srt[first.getIID()], size);

   if (globals.debug >= 3) {
      cerr << "Edge between " << cte.getContigs().first << " and " << cte.getContigs().second << " and size " << cte.getSize() << " and sd " << cte.getSD() << " of type " << cte.getAdjacency() << endl;
      cerr << "The adjusted size of the edge is " << size << endl;
      cerr << "I am positioning " << first.getIID() << " at " << firstPosition << " and " << second.getIID() << " at " << secondPosition << endl;
      cerr << "Currently they are positioned at " << ctg2srt[first.getIID()] << " and " << ctg2srt[second.getIID()] << endl;
      cerr << "The first orient is " << ctg2ort[first.getIID()] << " and second is " << ctg2ort[second.getIID()] << " and link type " << cte.getAdjacency() << endl;
   }
   firstPosition = reconcilePositions(ctg2srt[first.getIID()], firstPosition, cte.getSD());
   secondPosition = reconcilePositions(ctg2srt[second.getIID()], secondPosition, cte.getSD());

   if (firstPosition != INVALID_EDGE && secondPosition != INVALID_EDGE) {
      ctg2srt[first.getIID()] = firstPosition;
      ctg2srt[second.getIID()] = secondPosition;

      if (globals.debug >= 3) {
         cerr << "The position " << ctg2srt[second.getIID()] << " and the other position is " << ctg2srt[first.getIID()] << endl;
      }
      
      return 0;
   } else {
      return INVALID_EDGE;
   }
}

void addTile(std::vector<Tile_t> &tiles, ID_t contig, Tile_t &newTile) {
   vector<Tile_t>::iterator it = tiles.begin();
   bool insert = true;

   for(; it < tiles.end(); ) {
      if (it->source == contig) {
         if ((*it) == newTile) {
            insert = false;
            it++;
         } else {
            it = tiles.erase(it);
         }
      } else {
         it++;
      }
   }
   if (insert) { tiles.push_back(newTile); }
}

bool validateNeighbors(ID_t node, vector<ID_t>& neighbors, Bank_t &edge_bank, set<ID_t> &mySet, validateNeighborType type) {
   ContigEdge_t cte;

   for (vector<ID_t>::iterator i = neighbors.begin(); i < neighbors.end(); i++) {
      edge_bank.fetch(*i, cte);
      if (isBadEdge(cte)) { continue; }

      // push the node so it can be processed
      if (((type == ALL || type == INCOMING)&& cte.getContigs().second == node) ||
          ((type == ALL || type == OUTGOING) && cte.getContigs().first == node)) {
         ID_t otherID = getEdgeDestination(node, cte);

         set<ID_t>::iterator tIt = mySet.find(otherID);
         if (tIt == mySet.end()) { 
            return false;
         }
      }
   }
   
   return true;
}

ID_t findNeighborOfNeighbors(ID_t node, ID_t source, vector<ID_t>& neighbors, Bank_t &edge_bank) {
   int numNeighbors = 0;
   ID_t sink = 0;
   ContigEdge_t cte;
      
   for (vector<ID_t>::iterator i = neighbors.begin(); i < neighbors.end(); i++) {
      edge_bank.fetch(*i, cte);
      if (isBadEdge(cte)) { continue; }
      
      // continue by looking at the outgoing edge neighbors
      if (cte.getContigs().first == node) {
         ID_t otherID = getEdgeDestination(node, cte);

         if (otherID != source && otherID != sink) {
            numNeighbors++;
            sink = otherID;
         }
      }
   }
   
   if (numNeighbors == 1) {
      return sink;
   }
   else {
      return 0;
   }
}

void updateEdge(contigOrientation &newTileOrient, Tile_t &newTile, bool updateFirst, ContigEdge_t &cte, hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
   ID_t oldID;
   if (updateFirst) {
      oldID = cte.getContigs().second;
   } else {
      oldID = cte.getContigs().first;
   }

   if (newTileOrient == NONE) {
      newTileOrient = getOrientation(0, ctg2ort[oldID], cte);
      ctg2ort[newTile.source] = newTileOrient;

      if (newTileOrient == REV) {
         newTile.range.swap(); 
      }

      if (globals.debug >= 3) {
         cerr << "Oriented new tile " << newTile.source << " to be " << newTileOrient << endl;
      }
   }

   pair<ID_t, ID_t> ctgs;
   if (updateFirst) {
      cte.setAdjacency(getAdjacency(ctg2ort[cte.getContigs().first], ctg2ort[cte.getContigs().second], newTileOrient, ctg2ort[oldID], cte));
      ctgs.first = newTile.source;
      ctgs.second = oldID;
   } else {
      cte.setAdjacency(getAdjacency(ctg2ort[cte.getContigs().first], ctg2ort[cte.getContigs().second], ctg2ort[oldID], newTileOrient, cte));
      ctgs.first = oldID;
      ctgs.second = newTile.source;
   }
   cte.setContigs(ctgs);
   
   if (globals.debug >= 3) {
      cerr << "CTE IS UPDATED TO BE (" << cte.getContigs().first << ", " << cte.getContigs().second << ") orientation=" << cte.getAdjacency() << endl;
   }
}

// replace a specified list of nodes by a single tiling
void mergeContigs(ID_t newIID, 
                  set<ID_t> &toMerge, ID_t source, 
                  vector<Tile_t> &tiles, vector<ID_t> &edges, 
                  Bank_t &edge_bank, /*string comment,*/
                  hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
   Tile_t newTile;
   newTile.source = newIID;
   newTile.offset = UNINITIALIZED;
   newTile.range.begin = 1;
   newTile.range.end = 0;
   //newTile.comment = comment;
   
   //TODO: turn this into a class so that the ctg2ort is always accessible
   int lastEnd = 0;
   for (vector<Tile_t>::iterator i = tiles.begin(); i < tiles.end(); ) {
      set<ID_t>::iterator it = toMerge.find(i->source);

      if (it != toMerge.end()) {
         if (globals.debug >= 3) {
            cerr << "MERGING TO CREATE A NEW TILE FROM " << i->source << " " << i->range.begin << ", " << i->range.end << " AND OFFSET " << i->offset << " AND ORIENT IS " << i->range.isReverse() << endl;
         }
         int gapSize = 0;
         
         if (lastEnd != 0) {
            if (i->offset < lastEnd) {
               gapSize = newTile.offset - (i->offset + i->range.getLength()) - 1;
            } else {
               gapSize = i->offset - lastEnd - 1;
            }
         }
         newTile.range.end += i->range.getHi();
         newTile.range.end += gapSize;
         if (newTile.offset > i->offset || newTile.offset == UNINITIALIZED) {
            newTile.offset = i->offset;
         }
         lastEnd = newTile.offset + newTile.range.getLength();

         if (globals.debug >= 3) {
            cerr << "NEW TILE IS NOW " << newTile.source << " " << newTile.range.begin << ", " << newTile.range.end << " AND OFFSET " << newTile.offset << " AND ORIENT IS " << newTile.range.isReverse() << endl;
         }
         i = tiles.erase(i);
      } else {
         i++;
      }

   }
   
   addTile(tiles, newIID, newTile);
   //TODO: add the contig to the bank for real
   contigOrientation newTileOrient = NONE;

   // now update the edges
   for (vector<ID_t>::iterator i = edges.begin(); i < edges.end(); ) {
      ContigEdge_t cte;
      edge_bank.fetch(*i, cte);
      bool skipEdge = true;

      //TODO: don't erase the edge, remove it from tiling but then add a new edge to the tiling and bank
      // while we're at it, orient by the first edge we see to the source contig since they should all be consistent edges
      if (globals.debug >= 3) {
         cerr << "**EDGE: " << cte.getIID() << ": " << cte.getContigs().first << "->" << cte.getContigs().second << " AND THE ADJACENCY IS " << cte.getAdjacency() << endl;      
      }
      set<ID_t>::iterator it = toMerge.find(cte.getContigs().first);
      if (it != toMerge.end()) {
         updateEdge(newTileOrient, newTile, true, cte, ctg2ort);
         skipEdge = false;
      }
      it = toMerge.find(cte.getContigs().second);
      if (it != toMerge.end()) {
         updateEdge(newTileOrient, newTile, false, cte, ctg2ort);
         skipEdge = false;
      }

      if (skipEdge == false) {
         if (cte.getContigs().first == cte.getContigs().second) {
            if (globals.debug >= 3) { cerr << "REMOVING EDGE " << (*i) << endl; }
            edge_bank.remove(*i);
            i = edges.erase(i);
         } else {
            if (globals.debug >= 3) { cerr << "UPDATING EDGE " << (*i) << endl; }
            edge_bank.replace(*i, cte);
            i++;
         }
      } else {
         i++;
      }
   }
}

void reduceGraph(std::vector<Scaffold_t>& scaffs, Bank_t &contig_bank, Bank_t &edge_bank, hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
   ID_t maxIID = contig_bank.getMaxIID()+1;
   int numUpdated = 0;
   int itNum = 0;
   
   do {
      hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > visited;
      numUpdated = 0;
      itNum++;
      
      for(vector<Scaffold_t>::iterator s = scaffs.begin(); s < scaffs.end(); s++) {
         for (vector<Tile_t>::iterator i = s->getContigTiling().begin(); i < s->getContigTiling().end(); ) {
            if (visited[i->source] == 0) {
               visited[i->source] = 1;

               ID_t sink = 0;
               int numNeighbors;
               set<ID_t> t;
               std::stringstream oss;
               oss << i->source << " ";

               if (globals.debug >= 3) { cerr << "Checking node " << i->source << endl; }
               
               for (vector<ID_t>::iterator j = s->getContigEdges().begin(); j < s->getContigEdges().end(); j++) {
                  ContigEdge_t cte;
                  edge_bank.fetch(*j, cte);
                  if (isBadEdge(cte)) { continue; }
   
                  if (cte.getContigs().first == i->source) {
                     if (globals.debug >= 3) { cerr << "Checking neighbor for edge " << cte.getContigs().first << " to " << cte.getContigs().second << endl; }
                     ID_t otherID = getEdgeDestination(i->source, cte);
                     int neighbor = findNeighborOfNeighbors(otherID, i->source, s->getContigEdges(), edge_bank);

                     if (neighbor != 0) {
                        if (globals.debug >= 3) { cerr << "Found neighbor " << neighbor << " and sink is " << sink << endl; }
                        if (neighbor == sink || sink == 0) {
                           sink = neighbor;

                           t.insert(sink);
                           t.insert(otherID);
                           t.insert(i->source);

                           oss << otherID << " ";
                        }
                     }
                  }
               }
               oss << sink << " ";
               
               // double check that all the nodes have no incoming/outgoing edges outside the set
               // note that for the source we can have incoming edges and for outgoing we can have incoming
               for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
                  bool validated = false;
                  
                  if (*k == i->source) {
                     if (globals.debug >= 3) { cerr << "Validating source " << *k << endl; }
                     validated = validateNeighbors(i->source, s->getContigEdges(), edge_bank, t, OUTGOING);
                  } else if (*k == sink) {
                     if (globals.debug >= 3) { cerr << "Validating sink " << *k << endl; }
                     validated = validateNeighbors(sink, s->getContigEdges(), edge_bank, t, INCOMING);
                  } else {
                     if (globals.debug >= 3) { cerr << "Validating node " << *k << endl; }
                     validated = validateNeighbors(*k, s->getContigEdges(), edge_bank, t, ALL);
                  }
                  if (validated == false) {
                     if (globals.debug >= 3) {
                        cerr << "The source is " << i->source << " and sink is " << sink << endl;
                        cerr << "GIVING UP ON SET ****: ";
                        for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
                           cerr << *k << " ";
                        }
                        cerr << endl;
                     }
                     t.clear();
                     break;
                  }
               }

               if (t.size() > 0) {
                  if (globals.debug >= 1) {
                     cerr << "SET OF : ";
                     for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
                        cerr << *k << " ";
                     }
                  }
                  numUpdated++;
                  mergeContigs(maxIID++, t, i->source, s->getContigTiling(), s->getContigEdges(), edge_bank, ctg2ort);
                  
                  if (globals.debug >= 1) {
                     cerr << " HAS BEEN REPLACED WITH SINGLE NODE " << (maxIID-1) << endl;
                  }
               } else {
                  i++;
               }
            }
         }
      }
   } while (numUpdated != 0);
}

void sortContigs(std::vector<Scaffold_t>& scaffs, Bank_t &contig_bank, Bank_t &edge_bank) {
   for(vector<Scaffold_t>::iterator s = scaffs.begin(); s < scaffs.end(); s++) {
      Pos_t adjust = UNINITIALIZED;
      hash_map<ID_t, Pos_t, hash<ID_t>, equal_to<ID_t> > locations;
      hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > orientations;

      sort(s->getContigTiling().begin(), s->getContigTiling().end(), TileOrderCmp());
      for (vector<Tile_t>::iterator i = s->getContigTiling().begin(); i < s->getContigTiling().end(); i++) {
         // if the offset is not 0, make it 0 and adjust all the other positions, otherwise, do nothing
         if (adjust == UNINITIALIZED) {
            if (i->offset == 0) { // do nothing
               adjust = 0;
            }
            else {
               adjust = 0 - i->offset;
            }
         }
         i->offset += adjust;
         locations[i->source] = i->offset;
         orientations[i->source] = i->range.isReverse();
      }
      
      // here adjust the edges too so they all point from current to next since it is earlier in scaff
      for (vector<ID_t>::iterator i = s->getContigEdges().begin(); i < s->getContigEdges().end(); i++) {
         ContigEdge_t cte;     
         edge_bank.fetch(*i, cte);
         
         // reverse edge if necessary
         if (locations[cte.getContigs().second] <= locations[cte.getContigs().first]) {
            swapContigs(cte, orientations[cte.getContigs().first]);
            edge_bank.replace(*i, cte);
         }
      }
   }   
}

//TODO: Process Edges in memory (that way don't have to write to BANK)
int main(int argc, char *argv[]) {
   if (!GetOptions(argc, argv)){
      cerr << "Command line parsing failed" << endl;
      printHelpText();
      exit(1);
   }
  
   if (globals.bank == ""){ // no bank was specified
      cerr << "A bank must be specified" << endl;
      exit(1);
   }

   Bank_t edge_bank (ContigEdge_t::NCODE);
   if (! edge_bank.exists(globals.bank)){
      cerr << "No edge account found in bank " << globals.bank << endl;
      exit(1);
   }
   try {
      edge_bank.open(globals.bank, B_READ |B_WRITE);
   } catch (Exception_t & e) {
      cerr << "Failed to open edge account in bank " << globals.bank << ": " << endl << e << endl;
      edge_bank.close();
      exit(1);
   }

   Bank_t contig_bank (Contig_t::NCODE);
   BankStream_t contig_stream (Contig_t::NCODE);
   if (! contig_stream.exists(globals.bank)){
     cerr << "No contig account found in bank " << globals.bank << endl;
     exit(1);
   }
   try {
     contig_bank.open(globals.bank, B_READ);
     contig_stream.open(globals.bank, B_READ);     
   } catch (Exception_t & e) {
       cerr << "Failed to open contig account in bank " << globals.bank << ": " << endl << e << endl;
       contig_bank.close();
       contig_stream.close();
       exit(1);
   }
  
   ContigEdge_t cte;
   hash_map<ID_t, set<ID_t>*, hash<ID_t>, equal_to<ID_t> > ctg2lnk;     // map from contig to edges

   for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
      edge_bank.fetch(ci->iid, cte);

      if (globals.repeats.find(cte.getContigs().first) != globals.repeats.end() || 
         globals.repeats.find(cte.getContigs().second) != globals.repeats.end()) {
         if (globals.debug >= 0) {
            cerr << "WARNING: IGNORING EDGE " << cte.getIID() << " between " << cte.getContigs().first << " and " << cte.getContigs().second << " because one was listed as a repeat" << endl;;
         }
         cte.setStatus(BAD_RPT);
         edge_bank.replace(cte.getIID(), cte);
         
         continue;
      }

      if (cte.getContigLinks().size() < globals.redundancy) {
         // link is not have enough redundancy, ignore it
         // note we can do this since if it was consistent with other links of similar size and orientation
         // it would have been bundled into a larger (and thus more believable) link
         if (globals.debug >= 0) { 
            cerr << "WARNING: link " << cte.getIID() << " (" << cte.getContigs().first << ", " << cte.getContigs().second << ") is too low weight: " << cte.getLinks().size() << " cutoff was: " << globals.redundancy << endl;
         }
         cte.setStatus(BAD_LOW);
         edge_bank.replace(cte.getIID(), cte);
                  
         continue;
      }

      set<ID_t>* s = ctg2lnk[cte.getContigs().first];
      if (s == NULL) { s = new set<ID_t>();};
      s->insert(cte.getIID());
      ctg2lnk[cte.getContigs().first] = s;

      s = ctg2lnk[cte.getContigs().second];
      if (s == NULL) { s = new set<ID_t>();};
      s->insert(cte.getIID());
      ctg2lnk[cte.getContigs().second] = s;
   }
   int badCount = 0, goodCount = 0;
   Contig_t ctg;
   
   hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> > ctg2ort;     // map from contig to orientation
   map<string, LinkAdjacency_t> ctg2edges;                                 // map from pair of contigs to edge type connecting them
                                                                           // need this so we can know that we picked an N edge and not a A edge for two contigs
   hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > ctg2srt;               // map from contig to start position
   hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > visited;               // contigs we've processed
   std::vector<Scaffold_t> scaffs;
   ID_t scfIID = 0;

   queue<ID_t> current_nodes;

   // initialize position and orientation
   while (contig_stream >> ctg) {
      ctg2ort[ctg.getIID()] = NONE;
      ctg2srt[ctg.getIID()] = UNINITIALIZED;
   }
   contig_stream.seekg(0,BankStream_t::BEGIN);
   
   while (contig_stream >> ctg) {
      if (visited[ctg.getIID()] == 1) { continue; }

      Scaffold_t scaff;
      std::vector<Tile_t> tiles;
      std::vector<ID_t> edges;
      hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > visitedEdges;
      
      if (ctg2lnk[ctg.getIID()] == NULL || ctg2lnk[ctg.getIID()]->size() == 0) {
         if (globals.initAll == true) {
            Tile_t tile;
            tile.source = ctg.getIID();
            tile.offset = 0;
            tile.range.begin = 0;
            tile.range.end = ctg.getLength();
            tiles.push_back(tile);
   
            scaff.setContigEdges(edges);
            scaff.setContigTiling(tiles);
            scaff.setIID(scfIID);
            scaffs.push_back(scaff);
            scfIID++;
         }

         continue;
      }

      // orient if we aren't yet
      if (ctg2ort[ctg.getIID()] == NONE && ctg2srt[ctg.getIID()] == UNINITIALIZED) {
         ctg2ort[ctg.getIID()] = FWD;
         ctg2srt[ctg.getIID()] = 0;
      }
         
      current_nodes.push(ctg.getIID());
      if (globals.debug >= 2) { 
         cerr << "PUSHING NODE " << ctg.getIID() << " AND SIZE IS " << current_nodes.size() << endl;
      }
      
      // process all the nodes in this connected component
      while (current_nodes.size() != 0) {
         ID_t myID = current_nodes.front();
         current_nodes.pop();

         if (visited[myID] == 1) { continue; }
         visited[myID] = 1;
         
         set<ID_t>* s = ctg2lnk[myID];
         for (set<ID_t>::iterator i = s->begin(); i != s->end(); i++) {
            //TODO: I am not sure why there are a bunch of links with ID = 0 that the bank cannot retreive, ignore them
            if (*i != 0) {
               edge_bank.fetch(*i, cte);
               
               // TODO: again like with CTGs why are there links to contigs with id 0 that cannot be retreived?!
               if (cte.getContigs().first == 0 || cte.getContigs().second == 0) {
                  visitedEdges[cte.getIID()] = 1;
                  continue;
               }
               
               // push the node so it can be processed
               ID_t otherID = getEdgeDestination(myID, cte);
               // orient the node
               contigOrientation orient = getOrientation(myID, ctg2ort[myID], cte);

               // add the edge
               if (visitedEdges[cte.getIID()] == 0) {
                  std::stringstream oss;
                  oss << myID << "_" << otherID;

                  if (globals.debug >= 2) {
                     cerr << "LOOKING AT EDGE BETWEEEN " << cte.getContigs().first << " and " << cte.getContigs().second << endl;
                     cerr << "LOOKING AT EDGE BETWEEEN " << cte.getContigs().second << " and " << cte.getContigs().first << endl;
                  }
                  // determine bad edges
                  // an edge is bad if either
                  // 1. It orients the otherID contig inconsistently with it's current orientation
                  // 2. It orients the myID contig inconsistently given otherID orientation
                  // 3. There is already an edge between two nodes of a different adjacency
                  if (ctg2ort[otherID] != NONE && 
                      ((orient != ctg2ort[otherID] || getOrientation(otherID, ctg2ort[otherID], cte) != ctg2ort[myID]) || 
                       (ctg2edges[oss.str()] != ContigEdge_t::NULL_ADJACENCY && ctg2edges[oss.str()] != cte.getAdjacency()))) {
                     //inconsistent edge
                     cerr << "BAD EDGE: " << cte.getIID() << " between " << cte.getContigs().first << " and " << cte.getContigs().second << " with dist " << cte.getSize() << " and std " << cte.getSD() << " and the orientation is " << cte.getAdjacency() << endl;
                     badCount++;
                     
                     // update the edge in the bank so it is marked bad
                     cte.setStatus(BAD_ORI);
                  }
                  else {
                     ctg2ort[otherID] = orient;
                     ctg2edges[oss.str()] = cte.getAdjacency();

                     // only position using the good edges
                     Contig_t first, second;
                     contig_bank.fetch(cte.getContigs().first, first);
                     contig_bank.fetch(cte.getContigs().second, second);
                     if (computeContigPositions(first, second, cte, ctg2srt, ctg2ort) != 0) {
                        cerr << "BAD EDGE: " << cte.getIID() << " between " << cte.getContigs().first << " and " << cte.getContigs().second << " with dist " << cte.getSize() << " and std " << cte.getSD() << " and the orientation is " << cte.getAdjacency() << endl;
                        badCount++;
                        
                        // update the edge in the bank so it is marked bad
                        cte.setStatus(BAD_DST);
                     } else {
                        // update the edge in the bank so it is marked good
                        cte.setStatus(GOOD_EDGE);
                        goodCount++;
                     
                        // add tiling info
                        // offset is always in terms of the lowest position in scaffold of the contig, that is if we have ---> <---- then the offset of the second
                        // contig is computed from the head of the arrow, not the tail
                        Tile_t tile;
                        tile.source = myID;
                        
                        int length = (myID == first.getIID() ? first.getLength() : second.getLength()) - 1;
                        if (ctg2ort[myID] == FWD) {
                           tile.range.begin = 1;
                           tile.range.end = length;
                           tile.offset = ctg2srt[myID];
                        }
                        else {
                           tile.range.begin = length;
                           tile.range.end = 1;
                           tile.offset = ctg2srt[myID] - length;
                        }
                        addTile(tiles, myID, tile);
                        tile.clear();
                        
                        // now add the other contig's tile to the scaffold
                        tile.source = otherID;
                        
                        length = (otherID == first.getIID() ? first.getLength() : second.getLength()) - 1;
                        if (ctg2ort[otherID] == FWD) {
                           tile.range.begin = 1;
                           tile.range.end = length;
                           tile.offset = ctg2srt[otherID];
                        }
                        else {
                           tile.range.begin = length;
                           tile.range.end = 1;
                           tile.offset = ctg2srt[otherID] - length;
                        }
                        addTile(tiles, otherID, tile);
                     }
                  }
                  edges.push_back(cte.getIID());
                  visitedEdges[cte.getIID()] = 1;
               }
               
               // update link information
               edge_bank.replace(*i, cte);
               if (!isBadEdge(cte)) {
                  current_nodes.push(otherID);
                  
                  if (globals.debug >= 2) {
                     cerr << "PUSHING NODE " << otherID << " WHILE PROCESSING " << myID << " AND EDGE WAS " << cte.getStatus() << " AND SIZE IS " << current_nodes.size() << endl;
                  }
               }
            }
         }
      }
      if (tiles.size() == 0) {
         Tile_t tile;
         tile.source = ctg.getIID();
         tile.offset = 0;
         tile.range.begin = 0;
         tile.range.end = ctg.getLength();
         tiles.push_back(tile);
      }
      
      scaff.setContigEdges(edges);
      scaff.setContigTiling(tiles);
      scaff.setIID(scfIID);
      scaffs.push_back(scaff);
      scfIID++;
   }
   
   // preform graph simplification
   sortContigs(scaffs, contig_bank, edge_bank);   
   if (globals.compressMotifs == true) {
      reduceGraph(scaffs, contig_bank, edge_bank, ctg2ort);
      sortContigs(scaffs, contig_bank, edge_bank);
   }
   
   // output results
   outputResults(globals.bank, contig_bank, edge_bank, scaffs, ctg2ort, DOT, globals.prefix, globals.debug);
   outputResults(globals.bank, contig_bank, edge_bank, scaffs, ctg2ort, AGP, globals.prefix, globals.debug);
   outputResults(globals.bank, contig_bank, edge_bank, scaffs, ctg2ort, BAMBUS, globals.prefix, globals.debug);

   edge_bank.close();
   contig_stream.close();
   contig_bank.close();

   // clear the edge lists
   for (hash_map<ID_t, set<ID_t>*, hash<ID_t>, equal_to<ID_t> >::iterator i = ctg2lnk.begin(); i != ctg2lnk.end(); i++) {
      delete (i->second);
   }
   
   cerr << "Finished! Had " << goodCount << " Good links and " << badCount << " bad ones." << endl;
   return 0;
}
