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

#include "ContigLink_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

map<string, string> globals; // global variables
bool initAll = false;
ID_t maxContig = 0;
Bank_t *global_edge_bank = NULL;

// DEBUG output constant
static const int DEBUG = 0;

// define uninitialized value to be used for contig position or sizes
static const Size_t UNINITIALIZED = -99999999;

// define constants for orientation of contigs
#define FWD  "forward"
#define REV  "reverse"
#define NONE ""

#define BAD_EDGE 'b'
#define GOOD_EDGE 'g'

#define MAX_STDEV 3

void printHelpText() {
   cerr << 
    "\n"
    "USAGE:\n"
    "\n"
    "OrientContigs -b[ank] <bank_name>\n"
       << endl;
}

bool GetOptions(int argc, char ** argv) {
   int option_index = 0;
   static struct option long_options[] = {
    {"help",  0, 0, 'h'},
    {"h",     0, 0, 'h'},
    {"b",     1, 0, 'b'},
    {"bank",  1, 0, 'b'},
    {"a",     2, 0, 'a'},
    {"all",   2, 0, 'a'},
    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1){
    switch (c){
    case 'h':
      printHelpText();
      break;
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case 'a':
      initAll = true;
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions

bool isBadEdge(ContigEdge_t &cte) {
   if (cte.getStatus() == BAD_EDGE) {
      return true;
   }
   
   return false;
}

void swapContigs(ContigEdge_t & cte) {
   pair<ID_t, ID_t> ctgs;
   ctgs.first = cte.getContigs().second;
   ctgs.second = cte.getContigs().first;
   cte.setContigs(ctgs);

   if (cte.getAdjacency() == ContigEdge_t::NORMAL) {
      cte.setAdjacency(ContigEdge_t::ANTINORMAL);
   } else if (cte.getAdjacency() == ContigEdge_t::ANTINORMAL) {
      cte.setAdjacency(ContigEdge_t::NORMAL);
   }
}

ID_t getEdgeDestination(ID_t& edgeSrc, ContigEdge_t& cte) {
   if (cte.getContigs().second == edgeSrc) {
      return cte.getContigs().first;
   }
   else {
      return cte.getContigs().second;
   }
}

string getOrientation(ID_t contig, const string &myOrient, ContigEdge_t &cte) {
   assert(myOrient != NONE);

   switch (cte.getAdjacency()) {
      case ContigEdge_t::NORMAL:
      case ContigEdge_t::ANTINORMAL:
         // incase of normal or anti-normal, our directions match
         return myOrient;
         break;
      case ContigEdge_t::INNIE:
      case ContigEdge_t::OUTIE:
         if (myOrient == FWD) { return REV;} else { return FWD;}
         break;
      default:
         cerr << "Unknown orientation " << cte.getAdjacency() << endl;
         exit(1);
   };
}

LinkAdjacency_t getAdjacency(string firstOrient, string secondOrient, string newFirst, string newSecond, ContigEdge_t &cte) {
   if (DEBUG >= 1) {
      cerr << "We are orienting with first orient is " << firstOrient << " and new first is " << newFirst << " and second is " << secondOrient << " and second new is " << newSecond << endl;
   }

   if (firstOrient == newFirst && secondOrient == newSecond) {
      if (DEBUG >= 1) { cerr << "Nothing changed, do nothing" << endl; }
      return cte.getAdjacency();
   }
   else if (firstOrient == newFirst) {
      if (DEBUG >= 1) { cerr << "First orientation didn't change second did " << endl; }
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
      if (DEBUG >= 1) { cerr << "Second orient didn't change first did" << endl; }
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

Size_t adjustSize(LinkAdjacency_t contigOrientation, Size_t gapSize, 
                  Contig_t &first, Contig_t &second,
                  string &firstOrient, string &secondOrient) {   
   assert(firstOrient != NONE);

   Size_t size = gapSize;
   string expectedOrient = NONE;
   switch (contigOrientation) {
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
         cerr << "Unknown orientation " << contigOrientation << endl;
         exit(1);
   };

   // place the second contig behind the first one if we are reversed
   if (firstOrient != expectedOrient) {
      size *= -1;
   }

cerr << "Size is " << size << " for edge between " << first.getIID() << " and " << second.getIID() << endl;   
   return size;
}

Size_t computePosition(int position, Size_t size) {
   if (position == UNINITIALIZED) {
      return UNINITIALIZED;
   }

   return position+size;
}

Size_t reconcilePositions(Size_t oldPosition, Size_t newPosition, SD_t stdev) {
   if (newPosition == UNINITIALIZED) {
      return oldPosition;
   } else if (oldPosition == UNINITIALIZED) {
      return newPosition;
   } else {
      Size_t adjustedPosition = (oldPosition+newPosition)/2;
      double dist = abs(adjustedPosition-newPosition);
      if (dist > MAX_STDEV*stdev) {
         // this would move us too far, compromise and move to max dist away
         adjustedPosition = adjustedPosition>newPosition ? newPosition + MAX_STDEV*stdev : newPosition + -1*MAX_STDEV*stdev;
      }
      
      return adjustedPosition;
   }
}

void computeContigPositions(
      Contig_t &first, Contig_t &second, ContigEdge_t &cte,
      hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> >& ctg2srt,
      hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
   Size_t size = adjustSize(cte.getAdjacency(), cte.getSize(), first, second, ctg2ort[first.getIID()], ctg2ort[second.getIID()]);
   
   // compute the contig positions
   Size_t firstPosition = computePosition(ctg2srt[second.getIID()], -size);
   Size_t secondPosition = computePosition(ctg2srt[first.getIID()], size);

   if (DEBUG >= 1) {
      cerr << "Edge between " << cte.getContigs().first << " and " << cte.getContigs().second << " and size " << cte.getSize() << " and sd " << cte.getSD() << " of type " << cte.getAdjacency() << endl;
      cerr << "The adjusted size of the edge is " << size << endl;
      cerr << "I am positioning " << first.getIID() << " at " << firstPosition << " and " << second.getIID() << " at " << secondPosition << endl;
      cerr << "The first orient is " << ctg2ort[first.getIID()] << " and second is " << ctg2ort[second.getIID()] << " and link type " << cte.getAdjacency() << endl;
   }
   ctg2srt[first.getIID()] = reconcilePositions(ctg2srt[first.getIID()], firstPosition, cte.getSD());
   ctg2srt[second.getIID()] = reconcilePositions(ctg2srt[second.getIID()], secondPosition, cte.getSD());

   if (DEBUG >= 1) {
      cerr << "The position " << ctg2srt[second.getIID()] << " and the other position is " << ctg2srt[first.getIID()] << endl;
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

bool validateNeighbors(ID_t node, vector<ID_t>& neighbors, Bank_t &edge_bank, set<ID_t> &mySet, int validateSet) {
   ContigEdge_t cte;

   for (vector<ID_t>::iterator i = neighbors.begin(); i < neighbors.end(); i++) {
      edge_bank.fetch(*i, cte);
      if (isBadEdge(cte)) { continue; }

      // push the node so it can be processed
      if (((validateSet == 0 || validateSet == 1)&& cte.getContigs().second == node) ||
          ((validateSet == 0 || validateSet == 2) && cte.getContigs().first == node)) {
         ID_t otherID = getEdgeDestination(node, cte);

         set<ID_t>::iterator tIt = mySet.find(otherID);
         if (tIt == mySet.end()) { 
            return false;
         }
      }
   }
   
   return true;
}

ID_t checkNeighbors(ID_t node, ID_t source, vector<ID_t>& neighbors, Bank_t &edge_bank) {
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

bool updateEdge(string &newTileOrient, Tile_t &newTile, bool updateFirst, ContigEdge_t &cte, hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
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

      if (DEBUG >= 1) {
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
   
   if (DEBUG >= 1) {
      cerr << "CTE IS UPDATED TO BE (" << cte.getContigs().first << ", " << cte.getContigs().second << ") orientation=" << cte.getAdjacency() << endl;
   }

   return false;
}

// replace a specified list of nodes by a single tiling
void mergeContigs(ID_t newIID, 
                  set<ID_t> &toMerge, ID_t source, 
                  vector<Tile_t> &tiles, vector<ID_t> &edges, 
                  Bank_t &edge_bank, /*string comment,*/
                  hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
   Tile_t newTile;
   newTile.source = newIID;
   newTile.offset = -1;
   newTile.range.begin = 1;
   newTile.range.end = 0;
   //newTile.comment = comment;
   
   //TODO: turn this into a class so that the ctg2ort is always accessible
   int lastEnd = 0;
   for (vector<Tile_t>::iterator i = tiles.begin(); i < tiles.end(); ) {
      set<ID_t>::iterator it = toMerge.find(i->source);

      if (it != toMerge.end()) {
         if (DEBUG >= 1) {
            cerr << "MERGING TO CREATE A NEW TILE FROM " << i->source << " " << i->range.begin << ", " << i->range.end << " AND OFFSET " << i->offset << " AND ORIENT IS " << i->range.isReverse() << endl;
         }
         int gapSize = 0;
         
         if (lastEnd != 0) {
            gapSize = i->offset - lastEnd - 1;
         }
         newTile.range.end += i->range.getHi();
         newTile.range.end += gapSize;
         if (newTile.offset > i->offset || newTile.offset == -1) {
            newTile.offset = i->offset;
         }
         lastEnd = i->offset + i->range.getLength();

         if (DEBUG >= 1) {
            cerr << "NEW TILE IS NOW " << newTile.source << " " << newTile.range.begin << ", " << newTile.range.end << " AND OFFSET " << newTile.offset << " AND ORIENT IS " << newTile.range.isReverse() << endl;
         }
         i = tiles.erase(i);
      } else {
         i++;
      }

   }
   
   addTile(tiles, newIID, newTile);
   //TODO: add the contig to the bank for real
   string newTileOrient = NONE;

   // now update the edges
   for (vector<ID_t>::iterator i = edges.begin(); i < edges.end(); ) {
      ContigEdge_t cte;
      edge_bank.fetch(*i, cte);
      bool skipEdge = true;

      //TODO: don't erase the edge, remove it from tiling but then add a new edge to the tiling and bank
      // while we're at it, orient by the first edge we see to the source contig since they should all be consistent edges
      if (DEBUG >= 1) {
         cerr << "**EDGE: " << cte.getIID() << ": " << cte.getContigs().first << "->" << cte.getContigs().second << " AND THE ADJACENCY IS " << cte.getAdjacency() << endl;      
      }
      set<ID_t>::iterator it = toMerge.find(cte.getContigs().first);
      if (it != toMerge.end()) {
         skipEdge = updateEdge(newTileOrient, newTile, true, cte, ctg2ort);
      }
      it = toMerge.find(cte.getContigs().second);
      if (it != toMerge.end()) {
         skipEdge = updateEdge(newTileOrient, newTile, false, cte, ctg2ort);
      }

      if (skipEdge == false) {
         if (cte.getContigs().first == cte.getContigs().second) {
            if (DEBUG >= 1) { cerr << "REMOVING EDGE " << (*i) << endl; }
            edge_bank.remove(*i);
            i = edges.erase(i);
         } else {
            if (DEBUG >= 1) { cerr << "UPDATING EDGE " << (*i) << endl; }
            edge_bank.replace(*i, cte);
            i++;
         }
      } else {
         i++;
      }
   }
}

void reduceGraph(std::vector<Scaffold_t>& scaffs, Bank_t &contig_bank, Bank_t &edge_bank, hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {   
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
               std:stringstream oss;
               oss << i->source << " ";

               if (DEBUG >= 1) { cerr << "Checking node " << i->source << endl; }
               
               for (vector<ID_t>::iterator j = s->getContigEdges().begin(); j < s->getContigEdges().end(); j++) {
                  ContigEdge_t cte;
                  edge_bank.fetch(*j, cte);
                  if (isBadEdge(cte)) { continue; }
   
                  if (cte.getContigs().first == i->source) {
                     if (DEBUG >= 1) { cerr << "Checking neighbor for edge " << cte.getContigs().first << " to " << cte.getContigs().second << endl; }
                     ID_t otherID = getEdgeDestination(i->source, cte);
                     int neighbor = checkNeighbors(otherID, i->source, s->getContigEdges(), edge_bank);

                     if (neighbor != 0) {
                        if (DEBUG >= 1) { cerr << "Found neighbor " << neighbor << " and sink is " << sink << endl; }
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
               // note that for the source we can have incoming edges and for outgoing we can have 
               for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
                  bool validated = false;
                  
                  if (*k == i->source) {
                     if (DEBUG >= 1) { cerr << "Validating source " << *k << endl; }
                     validated = validateNeighbors(i->source, s->getContigEdges(), edge_bank, t, 2);
                  } else if (*k == sink) {
                     if (DEBUG >= 1) { cerr << "Validating sink " << *k << endl; }
                     validated = validateNeighbors(sink, s->getContigEdges(), edge_bank, t, 1);
                  } else {
                     if (DEBUG >= 1) { cerr << "Validating node " << *k << endl; }
                     validated = validateNeighbors(*k, s->getContigEdges(), edge_bank, t, 0);
                  }
                  if (validated == false) {
                     if (DEBUG >= 1) {
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
                  if (DEBUG >= 1) {
                     cerr << "SET IS *******: ";
                     for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
                        cerr << *k << " ";
                     }
                     cerr << endl;
                  }
                  numUpdated++;
                  mergeContigs(maxIID++, t, i->source, s->getContigTiling(), s->getContigEdges(), edge_bank, ctg2ort);
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
      }
      
      // here adjust the edges too so they all point from current to next since it is earlier in scaff
      for (vector<ID_t>::iterator i = s->getContigEdges().begin(); i < s->getContigEdges().end(); i++) {
         ContigEdge_t cte;     
         edge_bank.fetch(*i, cte);
         
         // reverse edge if necessary
         if (locations[cte.getContigs().second] <= locations[cte.getContigs().first]) {
            swapContigs(cte);
            edge_bank.replace(*i, cte);
         }
      }
   }   
}

struct EdgeOrderCmp
{
  bool operator () (const ID_t & a, const ID_t & b)
  {
      ContigEdge_t ctgA, ctgB;
      global_edge_bank->fetch(a, ctgA);
      global_edge_bank->fetch(b, ctgB);

      if (ctgA.getContigs().first != ctgB.getContigs().first) {
         if (ctgA.getContigs().first < ctgB.getContigs().first) return true;
         if (ctgA.getContigs().first > ctgB.getContigs().first) return false;
      }
      if (ctgA.getContigs().second != ctgB.getContigs().second) {
         if (ctgA.getContigs().second < ctgB.getContigs().second) return true;
         if (ctgA.getContigs().second > ctgB.getContigs().second) return false;
      }
      
      if (!isBadEdge(ctgA)) return true;

      return false;
  }
};

void outputEdge(ostream &stream, Bank_t &edge_bank, ID_t iid, const char *start, const char *end, const char *direction) {
   ContigEdge_t outputEdge;
   edge_bank.fetch(iid, outputEdge);

   stream << outputEdge.getContigs().first << ":" << start << "->" << outputEdge.getContigs().second << ":" << end;
   stream << " [ label =\"" << outputEdge.getIID() << " orientation=" << outputEdge.getAdjacency();

   if (isBadEdge(outputEdge)) {
      stream << " (UNUSED) ";
   }
   stream << "\" dir=\"" << direction << "\" fontsize = 8 solid color=\"black\" ]\n";   
}

void outputEdges(ostream &stream, Bank_t &edge_bank, ID_t currCtg, int edgeTypes[]) {
   if (currCtg != 0) {
      if (edgeTypes[3] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[3], "w", "w", "back");
      }
      if (edgeTypes[2] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[2], "e", "e", "forward");
      } 
      if (edgeTypes[1] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[1], "w", "e", "back");
      }
      if (edgeTypes[0] != 0) {
         outputEdge(stream, edge_bank, edgeTypes[0], "e", "w", "forward");
      } else {
         cerr << "WARNING: NO GOOD EDGE BETWEEN CONTIGS" << endl;
      }
   }
}

void outputGVZ(ostream &stream, std::vector<Scaffold_t> &scaffs, Bank_t &edge_bank,
               hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> >& ctg2ort) {
   stream << "digraph ROOT {" << endl;
   stream << "  rankdir = LR" << endl;
   stream << "  orientation = landscape" << endl;
   stream << "  ranksep = 0.3" << endl;
   stream << "  nodesep = 0.3" << endl;
   stream << "  fontsize = 8" << endl;;
   stream << "  margin = \".2,.2\"" << endl;;
   stream << "  ratio = auto" << endl;
   stream << "  page = \"8.5,11\"" << endl;

   vector<Scaffold_t>::iterator itNum = scaffs.begin();
   hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > visited;
   
   for(; itNum < scaffs.end(); itNum++) {
      stream << "subgraph cluster_" << itNum->getIID() << "{" << endl;
      stream << "\tlabel = \"" << itNum->getIID() << "\"" << endl;
      
      vector<Tile_t>::iterator tileIt = itNum->getContigTiling().begin();
      for (; tileIt < itNum->getContigTiling().end(); tileIt++) {
         int angle = -90;
         if (tileIt->range.isReverse()) { angle = 90; }
         
         stream << "\t" << tileIt->source << " [label=\"" ;
         //if (tileIt->comment.size() == 0)
            stream << tileIt->source;
         //else 
         //   stream << tileIt->comment;

         stream << " position=(" << tileIt->offset << "," << tileIt->offset + tileIt->range.getLength() - 1;
         stream << ") length=" << tileIt->range.getLength() << "\" height=0.2, fontsize=8, shape=\"house\", ";
         stream << "orientation=" << angle << " ]" << endl;         
      }
      vector<ID_t>::iterator edgeIt = itNum->getContigEdges().begin();
      
      ID_t currCtg = 0;
      ID_t secondCtg = 0;
      int edgeTypes[4];
      
      // sort before output
      global_edge_bank = &edge_bank;
      sort(itNum->getContigEdges().begin(), itNum->getContigEdges().end(), EdgeOrderCmp());
      
      for (; edgeIt < itNum->getContigEdges().end(); edgeIt++) {
         ContigEdge_t cte;
         edge_bank.fetch(*edgeIt, cte);

         if (currCtg != cte.getContigs().first || secondCtg != cte.getContigs().second) {
            outputEdges(stream, edge_bank, currCtg, edgeTypes);

            currCtg = cte.getContigs().first;
            secondCtg = cte.getContigs().second;
            memset(edgeTypes, 0, sizeof(int)*4);
         }
         if (visited[cte.getIID()] == 0) {
            // output the edges based on the orient of the contigs
            string edgeFirstCtgOrient = ctg2ort[cte.getContigs().first];
            string edgeSecondCtgOrient = ctg2ort[cte.getContigs().second];

            string first = FWD;
            if (cte.getAdjacency() == ContigEdge_t::ANTINORMAL || cte.getAdjacency() == ContigEdge_t::OUTIE) {
               first = REV;
            }
            string secondCtgOrient = getOrientation(cte.getContigs().first, first, cte);

            if (DEBUG >= 1) {
               cerr << "****The firstOrient is " << edgeFirstCtgOrient << " and second is " << edgeSecondCtgOrient << endl;
               cerr << "The edge type is " << cte.getAdjacency() << " and we initialized first to be " << first << endl;
               cerr << "****The orient we get are " << first << " and second is " << secondCtgOrient << endl;
            }
            if (first == edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               edgeTypes[0] = cte.getIID();
               
               if (DEBUG >= 1) { cerr << "*** MAIN EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<endl; }
            }
            else if (first != edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               if (DEBUG >= 1) { cerr << "*** D, S EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<endl;}
               edgeTypes[1] = cte.getIID();
            }
            else if (first == edgeFirstCtgOrient && secondCtgOrient != edgeSecondCtgOrient) {
               if (DEBUG >= 1) { cerr << "*** S, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<endl; }
               edgeTypes[2] = cte.getIID();
            }
            else {
               if (DEBUG >= 1) { cerr << "*** D, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<endl; }
               edgeTypes[3] = cte.getIID();
            }
         }
         visited[cte.getIID()] = 1;
      }
      
      stream << "}" << endl;
      outputEdges(stream, edge_bank, currCtg, edgeTypes);
   }
   stream  << "}" << endl;
}

void outputAGP(ostream &stream, std::vector<Scaffold_t> &scaffs) {
   stream << "# DESCRIPTION: WGS SCAFFOLDS GENERATED FROM AMOS BANK" << endl;

   vector<Scaffold_t>::iterator itNum = scaffs.begin();
   hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > visited;
   
   for(; itNum < scaffs.end(); itNum++) {
      int counter = 0;
      int lastEnd = 0;

      vector<Tile_t>::iterator tileIt = itNum->getContigTiling().begin();
      for (; tileIt < itNum->getContigTiling().end(); tileIt++) {
         // output the gap
         if (lastEnd != 0 && (tileIt->offset - lastEnd != 0)) {
            stream << itNum->getIID();
            stream << "\t" << lastEnd << "\t" << tileIt->offset;
            stream << "\t" << counter;
            stream << "\t" << (tileIt->offset - lastEnd + 1);
            stream << "\tfragment\tyes";
            
            stream << endl;
            counter++;
         }

         string sign = "+";
         if (tileIt->range.isReverse()) { sign = "-"; }
         
         stream << itNum->getIID();
         stream << "\t" << (tileIt->offset+1) << "\t" << (tileIt->offset+tileIt->range.getLength());
         stream << "\t" << counter << "\tW";
         //if (tileIt->comment.size() == 0)
            stream << "\t" << tileIt->source;
         //else 
         //   stream << "\t" << tileIt->comment;

         stream << "\t" << tileIt->range.getLo() << "\t" << tileIt->range.getHi();
         stream << "\t" << sign;
         stream << endl;
         
         counter++;
         lastEnd = tileIt->offset+tileIt->range.getLength()+1;
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

  if (globals.find("bank") == globals.end()){ // no bank was specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  BankStream_t edge_stream (ContigEdge_t::NCODE);
  if (! edge_stream.exists(globals["bank"])){
    cerr << "No edge account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    edge_stream.open(globals["bank"], B_READ |B_WRITE);
  } catch (Exception_t & e) {
      cerr << "Failed to open edge account in bank " << globals["bank"] << ": " << endl << e << endl;
      edge_stream.close();
      exit(1);
  }

   ContigEdge_t cte;
   hash_map<ID_t, set<ID_t>*, hash<ID_t>, equal_to<ID_t> > ctg2lnk;     // map from contig to edges
     
   while (edge_stream >> cte) {
      set<ID_t>* s = ctg2lnk[cte.getContigs().first];
      if (s == NULL) { s = new set<ID_t>();};
      s->insert(cte.getIID());
      ctg2lnk[cte.getContigs().first] = s;

      s = ctg2lnk[cte.getContigs().second];
      if (s == NULL) { s = new set<ID_t>();};
      s->insert(cte.getIID());
      ctg2lnk[cte.getContigs().second] = s;
   }
   edge_stream.close();
   Bank_t edge_bank (ContigEdge_t::NCODE);
   Bank_t contig_bank (Contig_t::NCODE);
   BankStream_t contig_stream (Contig_t::NCODE);
   if (! contig_stream.exists(globals["bank"])){
     cerr << "No contig account found in bank " << globals["bank"] << endl;
     exit(1);
   }
   try {
     contig_bank.open(globals["bank"], B_READ);
     contig_stream.open(globals["bank"], B_READ);
     edge_bank.open(globals["bank"], B_READ |B_WRITE);
   } catch (Exception_t & e) {
       cerr << "Failed to open contig account in bank " << globals["bank"] << ": " << endl << e << endl;

       edge_bank.close();
       contig_bank.close();
       contig_stream.close();
       exit(1);
   }

   int badCount = 0, goodCount = 0;
   Contig_t ctg;
   
   hash_map<ID_t, string, hash<ID_t>, equal_to<ID_t> > ctg2ort;     // map from contig to orientation
   map<string, LinkAdjacency_t> ctg2edges;                          // map from pair of contigs to edge type connecting them
                                                                    // need this so we can know that we picked an N edge and not a A edge for two contigs
   hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > ctg2srt;        // map from contig to start position
   hash_map<ID_t, int, hash<ID_t>, equal_to<ID_t> > visited;        // contigs we've processed
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
         if (initAll == true) {
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
                              
               // push the node so it can be processed
               ID_t otherID = getEdgeDestination(myID, cte);
               // orient the node
               string orient = getOrientation(myID, ctg2ort[myID], cte);

               // add the edge
               if (visitedEdges[cte.getIID()] == 0) {               
                  // mark edges that don't match chosen orient as bad
                  std:stringstream oss;
                  oss << myID << "_" << otherID;

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
                     cte.setStatus(BAD_EDGE);
                  }
                  else {
                     ctg2ort[otherID] = orient;
                     ctg2edges[oss.str()] = cte.getAdjacency();
                     goodCount++;
   
                     // only position using the good edges
                     Contig_t first, second;
                     contig_bank.fetch(cte.getContigs().first, first);
                     contig_bank.fetch(cte.getContigs().second, second);
                     computeContigPositions(first, second, cte, ctg2srt, ctg2ort);
   
                     // update the edge in the bank so it is marked bad
                     cte.setStatus(GOOD_EDGE);
                     
                     // add tiling info
                     // offset is always in terms of the same edge, that is if we have ---> <---- then the offset of the second
                     // contig is computed from the tail of the arrow, not the head
                     Tile_t tile;
                     
                     tile.source = myID;
                     tile.offset = ctg2srt[myID];
                     if (ctg2ort[myID] == FWD) {
                        tile.range.begin = 1;
                        tile.range.end = (myID == first.getIID() ? first.getLength() : second.getLength() - 1);
                     }
                     else {
                        tile.range.begin = (myID == first.getIID() ? first.getLength() : second.getLength() - 1);
                        tile.range.end = 1;
                     }
                     addTile(tiles, myID, tile);
   
                     tile.clear();
                     tile.source = otherID;
                     tile.offset = ctg2srt[otherID];
                     if (ctg2ort[otherID] == FWD) {
                        tile.range.begin = 1;
                        tile.range.end = (otherID == first.getIID() ? first.getLength() : second.getLength() - 1);
                     }
                     else {
                        tile.range.begin = (otherID == first.getIID() ? first.getLength() : second.getLength() -1);
                        tile.range.end = 1;
                     }
                     addTile(tiles, otherID, tile);
                  }
                  edges.push_back(cte.getIID());
                  visitedEdges[cte.getIID()] = 1;
               }
               
               // update link information
               edge_bank.replace(*i, cte);
               current_nodes.push(otherID);
            }
         }
      }
      scaff.setContigEdges(edges);
      scaff.setContigTiling(tiles);
      scaff.setIID(scfIID);
      scaffs.push_back(scaff);
      scfIID++;
   }
   
   sortContigs(scaffs, contig_bank, edge_bank);
   reduceGraph(scaffs, contig_bank, edge_bank, ctg2ort);
   sortContigs(scaffs, contig_bank, edge_bank);

   outputGVZ(cout, scaffs, edge_bank, ctg2ort);
   outputAGP(cerr, scaffs);

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
