//TODO: Fix How we compute positions (need to account for stddev when merging edges)
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

#include "Library_AMOS.hh"
#include "Fragment_AMOS.hh"

#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"
#include "ContigIterator_AMOS.hh"

#include "Utilities_Bundler.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;
using namespace Bundler;

// constants for maximum satisfied edge
static const int32_t  MAX_STDEV         =  3;
static const int32_t  INITIAL_STDEV     =  3;
static const double   STDEV_STEP_SIZE   =  0.5;
static const double   MIN_OVL_FOR_MOTIF = 0.020;

hash_map<ID_t, Size_t, hash<ID_t>, equal_to<ID_t> > *global_contig_len = NULL;
struct ContigOrderCmp
{
  bool operator () (const AMOS::ID_t & a, const AMOS::ID_t & b)
  {
      assert(global_contig_len);
      Size_t sizeA, sizeB;
      sizeA = (*global_contig_len)[a];
      sizeB = (*global_contig_len)[b];
      return (sizeA >= sizeB);
  }
};

struct config {
   bool        initAll;
   bool        compressMotifs;
   bool        doAgressiveScaffolding;
   bool        skipLowWeightEdges;
   int32_t     redundancy;   
   int32_t     debug;
   string      bank;
   int32_t     maxOverlap; // disallow contig overlaps
   set<ID_t>   repeats;  // repeat contigs to ignore (we ignore all links to them)
};
config globals;

ID_t maxContig = 0;
HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *Bundler::cte2weight = NULL;

void printHelpText() {
   cerr << 
    "\n"
    "Determine contig order and orientation\n"
    "\n"
    "USAGE:\n"
    "\n"
    "OrientContigs -b[ank] <bank_name> [-all] [-noreduce] [-agressive] [-redundancy minLinks] [-repeats fileName] [-skip]\n"
    "The -all option will force initialization of all contigs, including those that have no links to them, otherwise they remain uninitialized\n"
    "The -noreduce option will turn off search for common motifs and recursively remove them, thus simplyfing the graph\n"
    "The -agressive option will not mark edges that move a contig more than 3 STDEVS away as bad and will try to reconcile the positions\n"
    "The -redundancy option specifies the minimum number of links between two contigs before they will be scaffolded\n"
    "The -repeats option specifies a file containing a list of contig IIDs which are considered repeats and whose edges will be unused\n"
    "The -skip option will skip edges that have too low a weight relative to the weights of the other edges connecting their respective nodes. \n"
       << endl;
}

bool GetOptions(int argc, char ** argv) {
   int option_index = 0;      
   static struct option long_options[] = {
    {"help",               0, 0, 'h'},
    {"h",                  0, 0, 'h'},
    {"b",                  1, 0, 'b'},
    {"bank",               1, 0, 'b'},
    {"all",                0, 0, 'a'},
    {"noreduce",           0, 0, 'n'},
    {"agressive",          0, 0, 'A'},
    {"redundancy",         1, 0, 'R'},
    {"repeats",            1, 0, 'r'},    
    {"maxOverlap",         1, 0, 'o'},
    {"skip",               0, 0, 's'},
    {"debug",              1, 0, 'd'},
    {0, 0, 0, 0}
  };

   globals.initAll = false;
   globals.compressMotifs = true;
   globals.doAgressiveScaffolding = false;
   globals.debug = 1;
   globals.redundancy = 0;
   globals.skipLowWeightEdges = true;
   globals.maxOverlap = -1;
 
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
         if (globals.redundancy < 0) { globals.redundancy = 0; }
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
       case 's':
         globals.skipLowWeightEdges = false;
         break;
       case 'o':
         globals.maxOverlap = atoi(optarg);
         break;
       case 'd':
         globals.debug = atoi(optarg);
         if (globals.debug < 0) { globals.debug = 0; }
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
      cerr << "Max Overlap = \t" << globals.maxOverlap << endl;
   }
   
   return true;
} // GetOptions

void swapContigs(ContigEdge_t & cte, bool isReversed) {
   pair<ID_t, ID_t> ctgs;
   ctgs.first = cte.getContigs().second;
   ctgs.second = cte.getContigs().first;
   cte.setContigs(ctgs);

   // set what we expect to see, this is used to determine if we should modify the edge adjacency type when we swap the elements
   bool expected = false;
   if (cte.getAdjacency() == ContigEdge_t::ANTINORMAL || cte.getAdjacency() == ContigEdge_t::OUTIE) {
      expected = true;
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

double getMaxWeightEdge(ID_t nodeID, hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk, Bank_t &edge_bank) {
   assert(cte2weight);
   set<ID_t, EdgeWeightCmp>* s = ctg2lnk[nodeID];
   if (s == NULL || s->size() == 0) {
      return 0;
   }
   else {
      for (set<ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
        if (!isBadEdge(*i, edge_bank)) {
           return (*cte2weight)[(*i)];
        }
      }
      return 0;
   }
}

double getTotalWeightEdge(ID_t nodeID, hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk, Bank_t &edge_bank) {
   double total = 0;
   assert(cte2weight);

   set<ID_t, EdgeWeightCmp>* s = ctg2lnk[nodeID];
   if (s == NULL || s->size() == 0) {
      // do nothing, nothing to sum
   }
   else {
      for (set<ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
        if (!isBadEdge(*i, edge_bank)) {
           total += (*cte2weight)[(*i)];
        }
      }
   }
   
   return total;
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

contigOrientation getOrientationByAllEdges(ID_t nodeID, 
                     hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> > &ctg2ort,
                     hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk, 
                     Bank_t &edge_bank) {
                        
   set<ID_t, EdgeWeightCmp>* s = ctg2lnk[nodeID];
   ContigEdge_t cte;
   double orientWeights[2] = {0, 0};
   double max = 0;
   contigOrientation maxOrient = NONE;
   
   if (s == NULL || s->size() == 0) {
      // do nothing, nothing to sum
   }
   else {
      for (set<ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
        edge_bank.fetch(*i, cte);
        
        if (!isBadEdge(*i, edge_bank) && ctg2ort[getEdgeDestination(nodeID, cte)] != NONE) {
           contigOrientation orient = getOrientation(ctg2ort[getEdgeDestination(nodeID, cte)], cte);
           orientWeights[orient] += (*cte2weight)[(*i)];
           if (orientWeights[orient] > max) {
            max = orientWeights[orient];
            maxOrient = orient;
           }
        }
      }
   }
   
   return maxOrient;
}

int32_t computePosition(int32_t position, int32_t size) {
   if (position == UNINITIALIZED) {
      return UNINITIALIZED;
   }

   return position+size;
}

int32_t reconcilePositions(int32_t oldPosition, int32_t newPosition, double weight, SD_t stdev) {
   if (newPosition == UNINITIALIZED) {
      return oldPosition;
   } else if (oldPosition == UNINITIALIZED) {
      return newPosition;
   } else {
      int32_t adjustedPosition = (int32_t) round((oldPosition * (1 - weight)) + (newPosition * (weight)));
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

edgeStatus isEdgeConsistent(
                     const Contig_t &first,                     const Contig_t &second,
                     const ContigEdge_t &cte,
                     hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
                     hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >& ctg2srt,
                     hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> >& ctg2scf)
{
   // make sure these nodes are in the same scaffold
   if (ctg2scf[first.getIID()] != ctg2scf[second.getIID()] || ctg2scf[first.getIID()] == UNINITIALIZED) {
      return BAD_SCF;
   }

   // check the orientation
   if ((getOrientation(ctg2ort[first.getIID()], cte) != ctg2ort[second.getIID()]) || (getOrientation(ctg2ort[second.getIID()], cte) != ctg2ort[first.getIID()])) {
      return BAD_ORI;
   }

   // now check the size
   int32_t size = abs(adjustSizeBasedOnAdjacency(
                     cte.getAdjacency(),
                     cte.getSize(),
                     first.getLength(),
                     second.getLength(),
                     ctg2ort[first.getIID()],
                     ctg2ort[second.getIID()],
                     cte.getSD()/* * MAX_STDEV*/));
   size -= abs(ctg2srt[second.getIID()] - ctg2srt[first.getIID()]);
   if (abs(size) >= (MAX_STDEV * cte.getSD())) {
      return BAD_DST;
   }

   return GOOD_EDGE;
}

edgeStatus isEdgeConsistent(
                     const Contig_t &first,
                     const Contig_t &second,
                     const ContigEdge_t &cte,
                     hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
                     hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >& ctg2srt)
{
   hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > ctg2scf;
   ctg2scf[first.getIID()] = ctg2scf[second.getIID()] = 1;

   return isEdgeConsistent(first, second, cte, ctg2ort, ctg2srt, ctg2scf);
}

bool verifyEdgeStatus(Contig_t target, set<ID_t, EdgeWeightCmp>* s,
              hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >& ctg2srt,
              hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
              hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk,
              Bank_t &edge_bank, Bank_t &contig_bank)
{
   if (s == NULL || s->size() == 0) {
      return true;
   }

   bool result = true;
   ID_t myID = target.getIID();
   ContigEdge_t cte;
   Contig_t first;

   for (set<ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
      edge_bank.fetch(*i, cte);
      // only use incoming edges to position ourselves on the second pass
      if (cte.getContigs().second != myID) {
         continue;
      }

      if (!isBadEdge(*i, edge_bank) && ctg2srt[getEdgeDestination(myID, cte)] != UNINITIALIZED) {
         // fetch the other node
         contig_bank.fetch(getEdgeDestination(myID, cte), first);

         result = result & (isEdgeConsistent(first, target, cte, ctg2ort, ctg2srt) == GOOD_EDGE);
         if (result == false) {
            break;
         }
      }
   }

   return result;
}

int32_t computeContigPositions(
      Contig_t &first, Contig_t &second, ContigEdge_t &cte,
      hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >& ctg2srt,
      hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
      hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk, 
      Bank_t &edge_bank, double initialSD) {

   Range_t nullRange(0,0);
   Range_t overlap(10,15);
   double sd = initialSD;
   double stepSize = STDEV_STEP_SIZE;

   int32_t firstPosition = INVALID_EDGE;
   int32_t secondPosition = INVALID_EDGE;
   int32_t initialFirst = INVALID_EDGE;
   int32_t initialSecond = INVALID_EDGE;

  // try moving the node both backwards and forwards as much as we can
  for (int i = FWD; i < REV+1; i++) {
    double adjustment = (i == FWD ? cte.getSD() : -cte.getSD());

   while (nullRange != overlap && sd <= MAX_STDEV) {  
   int32_t size = adjustSizeBasedOnAdjacency(
                     cte.getAdjacency(), 
                     cte.getSize(), 
                     first.getLength(),
                     second.getLength(),
                     ctg2ort[first.getIID()], 
                     ctg2ort[second.getIID()],
                     adjustment * sd);
   
   // compute the contig positions
   firstPosition = computePosition(ctg2srt[second.getIID()], -size);
   secondPosition = computePosition(ctg2srt[first.getIID()], size);

   if (globals.debug >= 3) {
      cerr << "Edge between " << cte.getContigs().first << " and " << cte.getContigs().second << " and size " << cte.getSize() << " and sd " << cte.getSD() << " of type " << cte.getAdjacency() << endl;
      cerr << "The adjusted size of the edge is " << size << endl;
      cerr << "I am positioning " << first.getIID() << " at " << firstPosition << " and " << second.getIID() << " at " << secondPosition << endl;
      cerr << "Currently they are positioned at " << ctg2srt[first.getIID()] << " and " << ctg2srt[second.getIID()] << endl;
      cerr << "The first orient is " << ctg2ort[first.getIID()] << " and second is " << ctg2ort[second.getIID()] << " and link type " << cte.getAdjacency() << endl;
   }

   /*
   double weight = 0.5;
   firstPosition = reconcilePositions(ctg2srt[first.getIID()], firstPosition, weight, cte.getSD());
   secondPosition = reconcilePositions(ctg2srt[second.getIID()], secondPosition, weight, cte.getSD());
   */
   // go on its good reconcile it
   double weight = (double)cte.getContigLinks().size()/getTotalWeightEdge(first.getIID(), ctg2lnk, edge_bank);
   firstPosition = reconcilePositions(ctg2srt[first.getIID()], firstPosition, weight, cte.getSD());
   weight = (double)cte.getContigLinks().size()/getTotalWeightEdge(second.getIID(), ctg2lnk, edge_bank);
   secondPosition = reconcilePositions(ctg2srt[second.getIID()], secondPosition, weight, cte.getSD());
   /*
   The code above was meant to better reconcile positions between edges. That is if an edge had a higher weight, it
   would contribute more to the position. However, this is still a problem if the node is initially positioned poorly.

   Example in B. suis: node 6 places node 170 12 bases ahead of itself (using an edge with SD 971)
                     : node 86 tries to place node 170 approximately 1100 bases (reconciled to 600) but has only an SD of 95 so it cannot be satisfied
   We need to incorporate other edge's SD when reconciling
   */

   // check for overlap
   Range_t firstRange(firstPosition, first.getLength()+firstPosition);
   Range_t secondRange(secondPosition, second.getLength()+secondPosition);
   overlap = (firstRange & secondRange);

   if (initialFirst == INVALID_EDGE) {
      initialFirst = firstPosition;
      initialSecond = secondPosition;
   }

   sd += stepSize;
   }
if (firstPosition != INVALID_EDGE && secondPosition != INVALID_EDGE) {
break;
}
}

   // if we never found a non-overlapping contig, just go back to what we started with
   if (overlap != nullRange) {
      firstPosition =  initialFirst;
      secondPosition = initialSecond;
   }

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


int32_t computeContigPositionUsingAllEdges(ID_t myID,
	      hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >& ctg2srt,
	      hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
	      hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk,
	      Bank_t &edge_bank, Bank_t &contig_bank, double initialSD, bool allowBad) {
   set<ID_t, EdgeWeightCmp>* s = ctg2lnk[myID];
   ContigEdge_t cte;
   Contig_t first;
   Contig_t second;

   contig_bank.fetch(myID, first);
   if (s == NULL || s->size() == 0) {
      // do nothing, nothing to sum
   }
   else {
      // should we sort the edges by SD first and then weight?
     int oldPosition = ctg2srt[myID];

      // if we're making a second pass, reset our position and let it be computed again
      if (allowBad == false) { 
         ctg2srt[myID] = UNINITIALIZED;
      }
 
      for (set<ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
         edge_bank.fetch(*i, cte);
         // only use incoming edges to position ourselves on the second pass
         if (allowBad == false && cte.getContigs().second != myID) {
            continue;
         }

         if (!isBadEdge(*i, edge_bank) && ctg2srt[getEdgeDestination(myID, cte)] != UNINITIALIZED) {
            // fetch the other node
            contig_bank.fetch(getEdgeDestination(myID, cte), second);
       
            int32_t result = (myID == cte.getContigs().first ? computeContigPositions(first, second, cte, ctg2srt, ctg2ort, ctg2lnk, edge_bank, initialSD) : computeContigPositions(second, first, cte, ctg2srt, ctg2ort, ctg2lnk, edge_bank, initialSD));
 
            if (result == INVALID_EDGE) {
               if (allowBad == false) {
                  ctg2srt[myID] = oldPosition;
                  break;
               }

       	       // mark edge as bad
               cerr << "BAD DST EDGE: " << cte.getIID() << " between " << cte.getContigs().first << " and " << cte.getContigs().second << " with dist " << cte.getSize() << " and std " << cte.getSD() << " and the orientation is " << cte.getAdjacency() << endl;
               // update the edge in the bank so it is marked bad
               setEdgeStatus(cte, edge_bank, BAD_DST);
            } else {
if (allowBad == false) {
   if (verifyEdgeStatus(first, s, ctg2srt, ctg2ort, ctg2lnk, edge_bank, contig_bank) == false) {
cerr << "CANT MOVE NODES " << first.getIID() << " AND " << second.getIID() << " ANY CLOSER IT MESSES UP AN EDGE" << endl;
      ctg2srt[myID] = oldPosition;
      break;
   }
}



               // update the edge in the bank so it is marked good
               setEdgeStatus(cte, edge_bank, GOOD_EDGE);
            }
         }
      }
      if (ctg2srt[myID] == UNINITIALIZED) {
         ctg2srt[myID] = oldPosition;
      }
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

bool validateNeighbors(ID_t node, set<ID_t, EdgeWeightCmp>& neighbors, Bank_t &edge_bank, set<ID_t> &mySet, validateNeighborType type, bool validateLowWeight) {
   ContigEdge_t cte;

   for (set<ID_t>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
      edge_bank.fetch(*i, cte);
      if (isBadEdge(cte)) {
         if (cte.getStatus() != BAD_SKIP || !validateLowWeight) { 
            continue; 
         }
      }

      // process the node
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

ID_t findNeighborOfNeighbors(ID_t node, ID_t source, set<ID_t, EdgeWeightCmp>& neighbors, Bank_t &edge_bank, set<ID_t> &lowWeight, bool &validateLowWeight, uint32_t &numNeighbors) {
   //uint32_t numNeighbors = 0;
   ID_t sink = 0;
   ContigEdge_t cte;
   uint32_t badEdges = 0;
      
   for (set<ID_t>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
      edge_bank.fetch(*i, cte);
      if (isBadEdge(cte)) {
         if (cte.getStatus() == BAD_SKIP && cte.getContigs().first == node) {
            badEdges++;
         }
         continue;
      }
      
      // continue by looking at the outgoing edge neighbors
      if (cte.getContigs().first == node) {
         ID_t otherID = getEdgeDestination(node, cte);

         if (otherID != source && otherID != sink) {
            numNeighbors++;
            sink = otherID;
         }
      }
   }

   if (numNeighbors == 0 && badEdges > 0) {
      // check if our low weight edges can let us restore the connection, if so do it
      for (set<ID_t>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
         edge_bank.fetch(*i, cte);
         if (cte.getStatus() == BAD_SKIP) { 
            // continue by looking at the outgoing edge neighbors
            if (cte.getContigs().first == node) {
               lowWeight.insert(cte.getIID());               
               ID_t otherID = getEdgeDestination(node, cte);
      
               if (otherID != source && otherID != sink) {
                  numNeighbors++;
                  sink = otherID;
               }
            }
         }
      }
cerr << "USING BAD EDGES " << cte.getIID() << " FROM NODE " << node << " FOUND A SINK " << sink << " NEIGHBORS: " << numNeighbors << endl;
      validateLowWeight = true;
   }
   
   if (numNeighbors == 1) {
      return sink;
   }
   else {
      return 0;
   }
}

void updateEdge(
               contigOrientation &newTileOrient, 
               Tile_t &newTile, 
               bool updateFirst, 
               ContigEdge_t &cte, 
               hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
               int32_t edgeAdjustment) {
   ID_t oldID;
   
   if (updateFirst) {
      oldID = cte.getContigs().second;
   } else {
      oldID = cte.getContigs().first;
   }

   if (newTileOrient == NONE && !isBadEdge(cte)) {
      newTileOrient = getOrientation(ctg2ort[oldID], cte);
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
      if (!isBadEdge(cte)) { 
         cte.setAdjacency(getAdjacency(ctg2ort[cte.getContigs().first], ctg2ort[cte.getContigs().second], newTileOrient, ctg2ort[oldID], cte));
      }
      ctgs.first = newTile.source;
      ctgs.second = oldID;
   } else {
      if (!isBadEdge(cte)) {
         cte.setAdjacency(getAdjacency(ctg2ort[cte.getContigs().first], ctg2ort[cte.getContigs().second], ctg2ort[oldID], newTileOrient, cte));
      }
      ctgs.first = oldID;
      ctgs.second = newTile.source;
   }
   cte.setContigs(ctgs);

   if (globals.debug >= 3) {
      cerr << "UPDATING EDGE " << cte.getIID() << " FROM " << cte.getContigs().first << " AND " << cte.getContigs().second << " OF LEN " << cte.getSize() << " BY " << edgeAdjustment << endl;
   }
   cte.setSize(cte.getSize() - edgeAdjustment);
   if (globals.debug >= 3) {
      cerr << "ADJUSTED EDGE SIZE IS " << cte.getSize() << " AND SD " << cte.getSD() << endl;
      cerr << "CTE IS UPDATED TO BE (" << cte.getContigs().first << ", " << cte.getContigs().second << ") orientation=" << cte.getAdjacency() << endl;
   }
}

// replace a specified list of nodes by a single tiling
double mergeContigs(ID_t scfIID,
                  ID_t newIID, 
		  ID_t& maxEdgeIID,
                  set<ID_t> &toMerge, ID_t source, 
                  vector<Tile_t> &tiles, vector<ID_t> &edges, 
                  Bank_t &edge_bank, /*string comment,*/
                  hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
                  hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk,
                  vector<Motif_t> &motifs,
                  Status_t status) {
   hash_map<Pos_t, uint32_t, hash<Pos_t>, equal_to<Pos_t> > newTileGapIndex;
   Motif_t motifScaffold;
   vector<Tile_t> motifTiling;
   vector<ID_t> motifEdges;
 
   Tile_t newTile;
   newTile.source = newIID;
   newTile.source_type = Motif_t::NCODE;
   newTile.offset = UNINITIALIZED;
   newTile.range.begin = 0;
   newTile.range.end = 0;
   //newTile.comment = comment;
   
   hash_map<ID_t, uint32_t, hash<ID_t>, equal_to<ID_t> > tileLayout;
   hash_map<ID_t, uint32_t, hash<ID_t>, equal_to<ID_t> > tileLength;

   uint32_t totalOverlap = 0;   
   uint32_t lastEnd = 0;
   for (vector<Tile_t>::iterator i = tiles.begin(); i < tiles.end(); ) {
      set<ID_t>::iterator it = toMerge.find(i->source);

      if (it != toMerge.end()) {
         if (globals.debug >= 3) {
            cerr << "MERGING TO CREATE A NEW TILE FROM " << i->source << " " << i->range.begin << ", " << i->range.end << " AND OFFSET " << i->offset << " AND ORIENT IS " << i->range.isReverse() << endl;
         }
         int32_t gapSize = 0;
         bool contained = false;
        
         // save this tile in the motif scaffold
         motifTiling.push_back(*i);
         if (lastEnd != 0) {
            // new low contig
            if (i->offset < newTile.offset) {
               uint32_t currEnd = i->offset + i->range.getLength();
               if (currEnd > newTile.offset + newTile.range.getLength()) {
                  currEnd = newTile.offset + newTile.range.getLength();
               }
               gapSize = newTile.offset - currEnd;
            } else if ((i->offset + i->range.getLength()) < (newTile.offset + newTile.range.getLength())) {
            // contained addition
               gapSize = 0 - i->range.getLength();
               contained = true;
            } else {
            // normal case
               gapSize = i->offset - lastEnd;
            }
            if (globals.debug >= 3) { cerr << "WHILE MERGING LAST END IS " << lastEnd << " AND OFFSET IS " << i->offset << " AND GAP SIZE IS " << gapSize << endl; }
         }
         // count total overlaps between tiles
         if (gapSize < 0) {
            totalOverlap += (-1 * gapSize);
         } else {
            uint32_t offset = (i->offset < newTile.offset ? (i->offset + i->range.getLength()) : (newTile.offset + newTile.range.end));
            for (int gapUpdate = 0; gapUpdate < gapSize - 1; gapUpdate++) {
               newTileGapIndex[offset + gapUpdate] = 1;
            }
            if (globals.debug >= 3) { cerr << "ADDED GAP FROM " << offset << " TO " << (offset + gapSize - 1) << endl; }
         }
         if (globals.debug >= 3) { cerr << "TOTAL OVERLAP IS " << totalOverlap << " WHILE PROCESSING CONTIG " << i->source << " AT POSITION " << i->offset << " CURR TILE IS " << newTile.offset << " LEN " << newTile.range.getLength() << endl; }
         // update gap indexes
         hash_map<Pos_t, uint32_t, hash<Pos_t>, equal_to<Pos_t> > gapIndex;
         for (vector<Pos_t>::iterator tileGap = i->gaps.begin(); tileGap < i->gaps.end(); tileGap++) {
            gapIndex[((*tileGap) + i->offset)] = 1;
            if (((*tileGap) + i->offset) < lastEnd && (((*tileGap) + i->offset) > newTile.offset)) {
               totalOverlap--;
            }
         }
         if (globals.debug >= 3) {cerr << "AFTER ADJUST FOR GAPS IN INPUT CONTIG WITH " << i->gaps.size() << " THE TOTAL IS " << totalOverlap << endl; }
         for (hash_map<Pos_t, uint32_t, hash<Pos_t>, equal_to<Pos_t> >::iterator newTileGap = newTileGapIndex.begin(); newTileGap != newTileGapIndex.end(); newTileGap++) {
            if (newTileGap->second == 1 && (newTileGap->first > i->offset) && (newTileGap->first < i->offset + i->range.getLength())) {
               if (gapIndex[newTileGap->first] == 0) {
                  totalOverlap--;
               }
            }
         }
         if (globals.debug >= 3) { cerr << "AFTER ADJUST FOR GAPS IN NEWTILE CONTIG WITH " << newTileGapIndex.size() << " THE TOTAL IS " << totalOverlap << endl; }
         for (int tileIndex = 0; tileIndex < i->range.getLength(); tileIndex++) {
            if (newTileGapIndex[i->offset + tileIndex] == 1) {
               if (gapIndex[i->offset + tileIndex] == 0) {
                  newTileGapIndex[i->offset + tileIndex] = 0;
               }
            } else {
               if (gapIndex[i->offset + tileIndex] == 1) {
                  newTileGapIndex[i->offset + tileIndex] = 1;
               }
            }
         }
         if (globals.debug >= 3) { cerr << "AFTER UPDATE FOR GAPS IN NEW CONTIG THE COUNT IS " << newTileGapIndex.size() << endl; }
         // update the size of the tile we're creating
         if (!contained) {
            newTile.range.end += i->range.getHi();
            newTile.range.end += gapSize;
         }
         if (newTile.offset > i->offset || newTile.offset == UNINITIALIZED) {
            // when the offset is moved to a new node, all the other nodes are now that much further from the start of the new contig
            if (newTile.offset != UNINITIALIZED) {
               for (hash_map<ID_t, uint32_t, hash<ID_t>, equal_to<ID_t> >::iterator t = tileLayout.begin(); t != tileLayout.end() ; t++) {
                  t->second += (newTile.offset - i->offset);
               }
            }
            newTile.offset = i->offset;            
         }
         lastEnd = newTile.offset + newTile.range.getLength();
         tileLayout[i->source] = i->offset - newTile.offset;
         tileLength[i->source] = i->range.getLength();

         if (globals.debug >= 3) {
            cerr << "NEW TILE IS NOW " << newTile.source << " " << newTile.range.begin << ", " << newTile.range.end << " AND OFFSET " << newTile.offset << " AND ORIENT IS " << newTile.range.isReverse() << endl;
         }
         i = tiles.erase(i);
      } else {
         i++;
      }
   }
   // finally update gaps in the new tile
   for (hash_map<Pos_t, uint32_t, hash<Pos_t>, equal_to<Pos_t> >::iterator newTileGap = newTileGapIndex.begin(); newTileGap != newTileGapIndex.end(); newTileGap++) {
      if (newTileGap->second == 1) {
         if (newTileGap->first - newTile.offset < 0) {
            cerr << "ERROR: NEW TILE IS NEGATIVE WHILE CREATING " << newTile.source << " GAP POSITION " << newTileGap->first << endl;
            exit(1);
         }
         newTile.gaps.push_back(newTileGap->first - newTile.offset);
      }
   }

   contigOrientation newTileOrient = NONE;

   // now update the edges
   set<ID_t, EdgeWeightCmp>* s = ctg2lnk[newTile.source];
   if (s == NULL) { s = new set<ID_t, EdgeWeightCmp>();}
   for (vector<ID_t>::iterator i = edges.begin(); i < edges.end(); ) {
      ContigEdge_t cte;
      edge_bank.fetch(*i, cte);
      bool skipEdge = true;

      // while we're at it, orient by the first edge we see to the source contig since they should all be consistent edges
      if (globals.debug >= 3) {
         cerr << "**EDGE: " << cte.getIID() << ": " << cte.getContigs().first << "->" << cte.getContigs().second << " AND THE ADJACENCY IS " << cte.getAdjacency() << endl;      
      }
      // store an old copy of this edge
      ContigEdge_t oldEdge = cte;
      std::ostringstream stream;
      stream << maxEdgeIID++;
      oldEdge.setIID(maxEdgeIID);
      oldEdge.setEID(stream.str());
cerr << "CREATED COPY OF EDGE " << cte.getIID() << " WITH ID " << oldEdge.getIID() << " FROM " << edge_bank.getIDMapSize() << endl;
      set<ID_t>::iterator it = toMerge.find(cte.getContigs().first);
      if (it != toMerge.end()) {
         // since we're the first node, adjust the size based on our distance to the end of the new contig
         int32_t edgeAdjustment = (newTile.range.getLength() - (tileLayout[cte.getContigs().first] + tileLength[cte.getContigs().first]));
         updateEdge(newTileOrient, newTile, true, cte, ctg2ort, edgeAdjustment);
         skipEdge = false;
      }
      it = toMerge.find(cte.getContigs().second);
      if (it != toMerge.end()) {
         // since we're the second node, adjust the size based on our distance to the front of the new contig
         int32_t edgeAdjustment = tileLayout[cte.getContigs().first];
         updateEdge(newTileOrient, newTile, false, cte, ctg2ort, edgeAdjustment);
         skipEdge = false;
      }

      if (skipEdge == false) {
         edge_bank.append(oldEdge);
         motifEdges.push_back(oldEdge.getIID());
         if (cte.getContigs().first == cte.getContigs().second) {
            if (globals.debug >= 3) { cerr << "REMOVING EDGE " << (*i) << endl; }
            edge_bank.remove(*i);
            i = edges.erase(i);
         } else {
            if (globals.debug >= 3) { cerr << "UPDATING EDGE " << (*i) << endl; }
            edge_bank.replace(*i, cte);

            // update edge links for the new node we created
            s->insert(cte.getIID());
            i++;
         }
      } else {
         i++;
      }
   }
   ctg2lnk[newTile.source] = s;
   
   std::ostringstream result;
   result << newIID;

   motifScaffold.setStatus(status);
   motifScaffold.setIID(newIID);
   motifScaffold.setScf(scfIID);
   motifScaffold.setEID(result.str()); 
   motifScaffold.setContigTiling(motifTiling);
   motifScaffold.setContigEdges(motifEdges);
   // reset type if necessary
   if ((double)totalOverlap / newTile.range.getLength() <= MIN_OVL_FOR_MOTIF) {
      motifScaffold.setStatus(LINEAR_SCAFFOLD);
   } 

   motifs.push_back(motifScaffold);

   addTile(tiles, newIID, newTile);
   return (double)totalOverlap / newTile.range.getLength();
}

void validateMotif(set<ID_t> &t, ID_t source, ID_t sink, bool validateLowWeight, Bank_t &edge_bank, hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk) {
   // double check that all the nodes have no incoming/outgoing edges outside the set
   // note that for the source we can have incoming edges and for outgoing we can have outgoing
   for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
      bool validated = false;
      
      if (*k != 0) {
         if (*k == source) {
            if (globals.debug >= 3) { cerr << "Validating source " << *k << endl; }
            validated = validateNeighbors(source, *ctg2lnk[source], edge_bank, t, OUTGOING, validateLowWeight);
         } else if (*k == sink) {
            if (globals.debug >= 3) { cerr << "Validating sink " << *k << endl; }
            validated = validateNeighbors(sink, *ctg2lnk[sink], edge_bank, t, INCOMING, validateLowWeight);
         } else {
            if (globals.debug >= 3) { cerr << "Validating node " << *k << endl; }
            validated = validateNeighbors(*k, *ctg2lnk[*k], edge_bank, t, ALL, validateLowWeight);
         }
         if (validated == false) {
            if (globals.debug >= 3) {
               cerr << "The source is " << source << " and sink is " << sink << endl;
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
   }
}

void reduceGraph(std::vector<Scaffold_t>& scaffs, 
                 Bank_t &contig_bank, 
                 Bank_t &edge_bank,
                 hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
                 hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk,
                 vector<Motif_t> &motifs) {
   ID_t maxIID = contig_bank.getMaxIID()+1;
   ID_t maxEdgeIID = edge_bank.getMaxIID()+1;
   uint32_t numUpdated = 0;
   uint32_t itNum = 0;

   // first reduce all straight paths
   {
      hash_map<ID_t, bool, hash<ID_t>, equal_to<ID_t> > visited;
      numUpdated = 0;
      itNum++;
      if (globals.debug >= 1) { cerr << "BEGINNING PATH ITERATION #" << itNum << endl; }
      
      for(vector<Scaffold_t>::iterator s = scaffs.begin(); s < scaffs.end(); s++) {
         if (globals.debug >= 1) { cerr << "PROCESSING SCAFFOLD " << s->getIID() << endl; }
         for (vector<Tile_t>::iterator i = s->getContigTiling().begin(); i < s->getContigTiling().end(); ) {
            if (visited[i->source] == false) {
               if (globals.debug >= 1) { cerr << "PROCESSING CONTIG " << i->source << endl; }
               visited[i->source] = true;
               ID_t prev = UNINITIALIZED;
               ID_t curr = i->source;
               ID_t next = UNINITIALIZED;

               set<ID_t> t;
               bool extendPath = true;
               
               while (extendPath) {
                  uint32_t numOut = 0;
                  uint32_t numIn = 0;
                  for (set<ID_t>::iterator j = ctg2lnk[curr]->begin(); j != ctg2lnk[curr]->end(); j++) {
                     ContigEdge_t cte;
                     edge_bank.fetch(*j, cte);

                     if (isBadEdge(cte)) { continue; }
                     if (cte.getContigs().first == curr) {
                        next = cte.getContigs().second;
                        numOut++;
                     } else if (cte.getContigs().second == curr) {
                        if (prev != UNINITIALIZED && prev != cte.getContigs().first) {
                           numIn++;
                        }
                     }
                  }
                  extendPath = false;
                  if (numIn == 0) {
                     t.insert(curr);

                     if (numOut == 1) {
                        prev = curr;
                        curr = next;
                        extendPath = true;
                     }
                  } else {
                     // go back one since the current wasn't included in our path
                     curr = prev;
                  }
               }
               if (globals.debug >= 1) {
                  cerr << "TESTING LINEAR PATH : ";
                  for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
                     cerr << *k << " ";
                  }
                  cerr << endl;
               }
               validateMotif(t, i->source, curr, false, edge_bank, ctg2lnk);
               if (t.size() > 1) {
                  if (globals.debug >= 1) { cerr << "COLLAPSING LINEAR PATH : " << endl; }
                  mergeContigs(s->getIID(), maxIID++, maxEdgeIID, t, i->source, s->getContigTiling(), s->getContigEdges(), edge_bank, ctg2ort, ctg2lnk, motifs, LINEAR_SCAFFOLD);               
               } else {
                  i++;
               }
            }
         }
      }
   }

   // now that linear paths are done, look for more interesting motifs
   do {
      hash_map<ID_t, bool, hash<ID_t>, equal_to<ID_t> > visited;
      numUpdated = 0;
      itNum++;
      if (globals.debug >= 1) { cerr << "BEGINNING COMPRESSION ITERATION #" << itNum << endl; }
      
      for(vector<Scaffold_t>::iterator s = scaffs.begin(); s < scaffs.end(); s++) {
         if (globals.debug >= 1) { cerr << "PROCESSING SCAFFOLD " << s->getIID() << endl; }
         for (vector<Tile_t>::iterator i = s->getContigTiling().begin(); i < s->getContigTiling().end(); ) {
            if (visited[i->source] == false) {
               if (globals.debug >= 1) { cerr << "PROCESSING CONTIG " << i->source << endl; }
               visited[i->source] = true;

               ID_t sink = 0;
               //uint32_t numNeighbors;
               set<ID_t> t;
               std::stringstream oss;
               uint32_t badEdges = 0;
               oss << i->source << " ";

               bool redoWithBadEdges = false;
               bool validateSkippedEdges = false;
               set<ID_t> skippedEdges;
               set<ID_t> skippedTwoDeepEdges;

               for (int redo = 0; redo < 1 || (redo < 2 && redoWithBadEdges); redo++) {
                  // see if we can find a motif
                  // the function below is allowed to use low weight edges we eliminated to try to make the motif
                  // if it uses low weight edges, all low weight edges in the set must still be a motif and will be reset to good
                  for (set<ID_t>::iterator j = ctg2lnk[i->source]->begin(); j != ctg2lnk[i->source]->end(); j++) {
                     ContigEdge_t cte;
                     edge_bank.fetch(*j, cte);
   
                     if (isBadEdge(cte)) {
                        if (cte.getStatus() == BAD_SKIP && cte.getContigs().first == i->source) {
                           badEdges++;
                           if (redoWithBadEdges == false) {
                              continue;
                           }
                           skippedEdges.insert(cte.getIID());
                        } else {
                           continue;
                        }
                     }
      
                     if (cte.getContigs().first == i->source) {
                        // 2-deep motif code
                        ID_t neighbor = 0;
                        
                        for (set<ID_t>::iterator k = ctg2lnk[cte.getContigs().second]->begin(); k != ctg2lnk[cte.getContigs().second]->end(); k++) {
                           ContigEdge_t nextCte;
                           edge_bank.fetch(*k, nextCte);
                           if (isBadEdge(nextCte)) {
                              if (nextCte.getStatus() == BAD_SKIP && nextCte.getContigs().first == i->source) {
                                 badEdges++;
                                 if (redoWithBadEdges == false) {
                                    continue;
                                 }
                                 skippedTwoDeepEdges.insert(nextCte.getIID());
                              } else {
                                 continue;
                              }
                           }
                        
                           // if we're the outgoing edge of the first neighbors
                           if (nextCte.getContigs().first == cte.getContigs().second) {
                              uint32_t numNeighbors = 0;
                              ID_t otherID = getEdgeDestination(cte.getContigs().second, nextCte);
                              neighbor = findNeighborOfNeighbors(otherID, cte.getContigs().second, *ctg2lnk[otherID], edge_bank, skippedEdges, validateSkippedEdges, numNeighbors);

// hack to get nodes that dead-end as motifs
                              if (neighbor != 0 || (neighbor == 0 && numNeighbors == 0)) {
                                    if (neighbor == sink || sink == 0) {
                                       sink = neighbor;
                        
                                       t.insert(sink);
                                       t.insert(otherID);
                                       t.insert(cte.getContigs().second);
                                       t.insert(i->source);
                        
                                       oss << otherID << " ";
                                    }         
                              }
                           }
                        }
                        //*/
                        
                        /* Original motif code
                        if (globals.debug >= 3) { cerr << "Checking neighbor for edge " << cte.getContigs().first << " to " << cte.getContigs().second << endl; }
                        ID_t otherID = getEdgeDestination(i->source, cte);
                        ID_t neighbor = findNeighborOfNeighbors(otherID, i->source, *ctg2lnk[otherID], edge_bank, skippedEdges, validateSkippedEdges);
   
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
                        
                        */
                     }
                  }
                  oss << sink << " ";
                  validateMotif(t, i->source, sink, validateSkippedEdges, edge_bank, ctg2lnk);

                  // try 1-deep motifs 
                  if (t.size() == 0) {
                     oss.clear();
                     oss << i->source << " ";
                     ID_t neighbor = 0;
                     sink = 0;
                     skippedTwoDeepEdges.clear();
                     
                     // see if we can find a motif
                     // the function below is allowed to use low weight edges we eliminated to try to make the motif
                     // if it uses low weight edges, all low weight edges in the set must still be a motif and will be reset to good
                     for (set<ID_t>::iterator j = ctg2lnk[i->source]->begin(); j != ctg2lnk[i->source]->end(); j++) {
                        ContigEdge_t cte;
                        edge_bank.fetch(*j, cte);
      
                        if (isBadEdge(cte)) {
                           if (cte.getStatus() == BAD_SKIP && cte.getContigs().first == i->source) {
                              badEdges++;
                              if (redoWithBadEdges == false) {
                                 continue;
                              }
                              skippedEdges.insert(cte.getIID());
                           } else {
                              continue;
                           }
                        }
         
                        if (cte.getContigs().first == i->source) {
                           ID_t otherID = getEdgeDestination(i->source, cte);
                           uint32_t numNeighbors = 0;
                           
                           neighbor = findNeighborOfNeighbors(otherID, i->source, *ctg2lnk[otherID], edge_bank, skippedEdges, validateSkippedEdges, numNeighbors);
                           if (neighbor != 0 || (neighbor == 0 && numNeighbors == 0)) {
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
                  } else {
                     skippedEdges.insert(skippedTwoDeepEdges.begin(), skippedTwoDeepEdges.end());
                  }
                  oss << sink << " ";
                  validateMotif(t, i->source, sink, validateSkippedEdges, edge_bank, ctg2lnk);

                  if (t.size() == 0 && badEdges != 0) {
                     redoWithBadEdges = validateSkippedEdges = true;
                  }
               }
               if (t.size() > 0) {
                  if (globals.debug >= 1) {
                     cerr << "SET OF : ";
                     for (set<ID_t>::iterator k = t.begin(); k != t.end(); k++) {
                        cerr << *k << " ";
                     }
                  }
                  // merge the contigs, if we were able to rescue low-weight edges, mark them
                  numUpdated++;
                  if (validateSkippedEdges) { resetEdges(edge_bank, skippedEdges, BAD_SKIP); }
                  double overlapInMotif = mergeContigs(s->getIID(), maxIID++, maxEdgeIID, t, i->source, s->getContigTiling(), s->getContigEdges(), edge_bank, ctg2ort, ctg2lnk, motifs, MOTIF_SCAFFOLD);

                  if (globals.debug >= 1) {
                     cerr << " WITH OVERLAP " << overlapInMotif << " HAS BEEN REPLACED WITH SINGLE NODE " << (maxIID-1) << endl;
                  }
               } else {
                  i++;
               }
            }
         }
      }
      if (globals.debug >= 1) { cerr << "END COMPRESSION ITERATION #" << itNum << " UPDATED: " << numUpdated << endl; }
   } while (numUpdated != 0);
}

void sortContigs(std::vector<Scaffold_t>& scaffs, Bank_t &contig_bank, Bank_t &edge_bank) {
   for(vector<Scaffold_t>::iterator s = scaffs.begin(); s < scaffs.end(); s++) {
      Pos_t adjust = UNINITIALIZED;
      hash_map<ID_t, Pos_t, hash<ID_t>, equal_to<ID_t> > locations;
      hash_map<ID_t, bool, hash<ID_t>, equal_to<ID_t> > orientations;

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

void compressGaps(std::vector<Scaffold_t> &scaffs,
              hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >& ctg2srt,
              hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> >& ctg2ort,
              hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > &ctg2lnk,
              Bank_t &edge_bank, Bank_t &contig_bank) {

   sortContigs(scaffs, contig_bank, edge_bank);
   for(vector<Scaffold_t>::iterator s = scaffs.begin(); s < scaffs.end(); s++) {
      Pos_t adjust = UNINITIALIZED;
      hash_map<ID_t, Pos_t, hash<ID_t>, equal_to<ID_t> > locations;
      hash_map<ID_t, bool, hash<ID_t>, equal_to<ID_t> > orientations;

      sort(s->getContigTiling().begin(), s->getContigTiling().end(), TileOrderCmp());
      for (vector<Tile_t>::iterator i = s->getContigTiling().begin(); i < s->getContigTiling().end(); i++) {
         int32_t pos = ctg2srt[i->source];
         double sd = 0;
         while (pos == ctg2srt[i->source] && sd < INITIAL_STDEV) {
            computeContigPositionUsingAllEdges(i->source, ctg2srt, ctg2ort, ctg2lnk, edge_bank, contig_bank, sd, false);
            sd++;
         }
         i->offset = (i->range.isReverse() ? ctg2srt[i->source] - i->range.getLength() : ctg2srt[i->source]);
      }
   }
}

//TODO: Store scaffolds in bank as well as new contigs (as scaffolds), split this code into several sections
// Add class that holds the data structure we use in memory so it's not always passed around
// Also, keep tiles on disk rather than in memory
// RepeatFinder : find repeats and mark the contigs in the bank rather than outputting their list
// Scaffolder   : build the initial scaffolds and save them (after topological sort and sorting)
// Reducer      : find patterns to reduce in the build scaffolds
// Avoid making destructive changes to the bank as a result of these operations

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
      edge_bank.open(globals.bank, B_READ |B_WRITE, 0, false);
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

   BankStream_t motif_stream (Motif_t::NCODE);
   try {
      if (! motif_stream.exists(globals.bank)){
         motif_stream.create(globals.bank);
      }
      motif_stream.open(globals.bank, B_READ | B_WRITE);
      motif_stream.clearCurrentVersion();
   } catch (Exception_t & e) {
       cerr << "Failed to open motif account in bank " << globals.bank << ": " << endl << e << endl;
       motif_stream.close();
       exit(1);
   }

   BankStream_t scaff_stream (Scaffold_t::NCODE);
   if (!scaff_stream.exists(globals.bank)) {
       scaff_stream.create(globals.bank);
   }
   try {
     scaff_stream.open(globals.bank, B_READ | B_WRITE);
     scaff_stream.clearCurrentVersion();
   } catch (Exception_t & e) {
       cerr << "Failed to open scaffold account in bank " << globals.bank << ": " << endl << e << endl;
       scaff_stream.close();
       exit(1);
   }

   // initialize contig data structures
   Contig_t ctg;
   hash_map<ID_t, Size_t, hash<ID_t>, equal_to<ID_t> > ctg2len;            // map from contig to length
   hash_map<ID_t, contigOrientation, hash<ID_t>, equal_to<ID_t> > ctg2ort; // map from contig to orientation
   hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> > ctg2srt;           // map from contig to start position
   hash_map<ID_t, ID_t, hash<ID_t>, equal_to<ID_t> > ctg2scf;              //quick lookup for which scaffold a contig belongs to
   vector<Motif_t> motifs;            // scaffold structures representing the simplified motifs
   vector<ID_t> contigProcessingOrder;

   // initialize position and orientation
   while (contig_stream >> ctg) {
      ctg2ort[ctg.getIID()] = NONE;
      ctg2srt[ctg.getIID()] = UNINITIALIZED;
      ctg2scf[ctg.getIID()] = UNINITIALIZED;
      ctg2len[ctg.getIID()] = ctg.getLength();
      contigProcessingOrder.push_back(ctg.getIID());
   }

   // initialize scaffold data structure
   ID_t scfIID = 1;
   std::vector<Scaffold_t> scaffs;

   // initialize edge data structures
   hash_map<ID_t, bool, hash<ID_t>, equal_to<ID_t> > visitedEdges;
   hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > ctg2lnk;     // map from contig to edges
   cte2weight = new hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >();

   uint32_t badCount = 0, goodCount = 0;
   ContigEdge_t cte;

   // try to pick a redundancy cutoff
   double mean = 0;
   double count = 0;
   if (globals.redundancy == 0) {
      for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
         edge_bank.fetch(ci->iid, cte);

         if (isBadEdge(cte)) {
            continue;
         }
         if (globals.maxOverlap > 0 && cte.getSize() + globals.maxOverlap < 0 && cte.getSize() < globals.maxOverlap) {
            continue;
         }
         if (globals.repeats.find(cte.getContigs().first) != globals.repeats.end() ||
               globals.repeats.find(cte.getContigs().second) != globals.repeats.end()) {
            continue;
         }
         if (cte.getIID() == 0 || cte.getContigs().first == 0 || cte.getContigs().second == 0) {
            continue;
         }
         if (cte.getContigLinks().size() > 0) {
            mean += cte.getContigLinks().size();
            count++;
         }
      }
      mean /= count;
      globals.redundancy = (uint32_t)round(mean);
      if (globals.debug >= 1) { cerr << "Picked mean cutoff as " << mean << " rounded " << globals.redundancy << endl; }
   }

   // If we wanted to process edges sequentially, we can insert a loop here
   // However, this requires a merge scaffolds function as we may see a new edge linking two separate scaffolds so all the scaffolds need to be rectified again
   // Therefore, it doesn't seem to be simpler than implementing least squares
   for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
      edge_bank.fetch(ci->iid, cte);

      if (isBadEdge(cte)) {
         cerr << "Edge " << cte.getIID() << " ALREADY MARKED BAD, SKIPPING" << endl;
         continue;
      }

      if (globals.repeats.find(cte.getContigs().first) != globals.repeats.end() || 
         globals.repeats.find(cte.getContigs().second) != globals.repeats.end()) {
         if (globals.debug >= 0) {
            cerr << "WARNING: IGNORING EDGE " << cte.getIID() << " between " << cte.getContigs().first << " and " << cte.getContigs().second << " because one was listed as a repeat" << endl;;
         }
         setEdgeStatus(cte, edge_bank, BAD_RPT, false);
         continue;
      }


     if (globals.maxOverlap > 0 && cte.getSize() + globals.maxOverlap < 0 && cte.getSize() < globals.maxOverlap) {
        cerr << "WARNING: IGNORING EDGE " << cte.getIID() << " between " << cte.getContigs().first << " and " << cte.getContigs().second << " because it is below overlap threshold of " << globals.maxOverlap << endl;
        setEdgeStatus(cte, edge_bank, BAD_THRESH, false);
        continue;
      }

      if (cte.getContigLinks().size() < globals.redundancy) {
         // link is not have enough redundancy, ignore it
         // note we can do this since if it was consistent with other links of similar size and orientation
         // it would have been bundled into a larger (and thus more believable) link
         if (globals.debug >= 0) { 
            cerr << "WARNING: link " << cte.getIID() << " (" << cte.getContigs().first << ", " << cte.getContigs().second << ") is too low weight: " << cte.getLinks().size() << " cutoff was: " << globals.redundancy << endl;
         }
         setEdgeStatus(cte, edge_bank, BAD_THRESH, false);
         continue;
      }
      if (cte.getIID() == 0 || cte.getContigs().first == 0 || cte.getContigs().second == 0) {
         // TODO: CTGs with links to ID 0 indicate links to/from singletons. Incorporate singletons into assembly?
         if (globals.debug >= 0) {
            cerr << "WARNING: link " << cte.getIID() << " (" << cte.getContigs().first << ", " << cte.getContigs().second << ") connects to a singleton, it is being ignored" << endl;
         }
         setEdgeStatus(cte, edge_bank, BAD_THRESH, false);
         continue;
      }
      (*cte2weight)[cte.getIID()] = cte.getContigLinks().size();
      set<ID_t, EdgeWeightCmp>* s = ctg2lnk[cte.getContigs().first];
      if (s == NULL) { s = new set<ID_t, EdgeWeightCmp>();}
      s->insert(cte.getIID());
      ctg2lnk[cte.getContigs().first] = s;

      s = ctg2lnk[cte.getContigs().second];
      if (s == NULL) { s = new set<ID_t, EdgeWeightCmp>();};
      s->insert(cte.getIID());
      ctg2lnk[cte.getContigs().second] = s;
   }
   flushEdgeStatus(edge_bank);
   
   map<string, LinkAdjacency_t> ctg2edges;                                 // map from pair of contigs to edge type connecting them
                                                                           // need this so we can know that we picked an N edge and not a A edge for two contigs
   // track visisted nodes
   hash_map<ID_t, bool, hash<ID_t>, equal_to<ID_t> > visited;              // contigs we've processed

   priority_queue<pair<ID_t, uint32_t>, vector<pair<ID_t, uint32_t> >, EdgePairCmp> current_nodes;
   pair<ID_t, uint32_t> nodeToWeight;

   Scaffold_t scaff;
   std::vector<Tile_t> tiles;
   std::vector<ID_t> edges;
   
   // pull initial contig and initialize bank for subsequent processing
   global_contig_len = &ctg2len;
   stable_sort(contigProcessingOrder.begin(), contigProcessingOrder.end(), ContigOrderCmp());
   global_contig_len = NULL;

   for (vector<ID_t>::iterator iter = contigProcessingOrder.begin(); iter != contigProcessingOrder.end(); iter++) {
      contig_bank.fetch(*iter, ctg);

      // if we see any contigs without any edges, make sure we allocate an empty set of edges for them
      if (ctg2lnk[ctg.getIID()] == NULL) {
         ctg2lnk[ctg.getIID()] = new set<ID_t, EdgeWeightCmp>();
      }

      // if we've already processed the node in this iteration, dont do it again
      if (visited[ctg.getIID()] == true) { continue; }

      tiles.clear();
      edges.clear();
                        
      if (ctg2lnk[ctg.getIID()]->size() == 0) {
         continue;
      }

      // orient if we aren't yet
      if (ctg2ort[ctg.getIID()] == NONE && ctg2srt[ctg.getIID()] == UNINITIALIZED) {
         ctg2ort[ctg.getIID()] = FWD;
         ctg2srt[ctg.getIID()] = 0;
      }
       
      nodeToWeight.first = ctg.getIID();
      nodeToWeight.second = MAX_SIZE;  
      current_nodes.push(nodeToWeight);
      if (globals.debug >= 2) { 
         cerr << "PUSHING NODE " << ctg.getIID() << " AND SIZE IS " << current_nodes.size() << endl;
      }
      // process all the nodes in this connected component
      while (current_nodes.size() != 0) {
         ID_t myID = current_nodes.top().first;
         current_nodes.pop();
         visited[myID] = true;

int max = 0;
         // keep track of max weight edge for this contig, if we drop too low, ignore it
         set<ID_t, EdgeWeightCmp>* s = ctg2lnk[myID];
         for (set<ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
            //TODO: CA links incorporated as links with ID 0. Should they be skipped since we generate our own?
            checkEdgeID(*i);
            edge_bank.fetch(*i, cte);
            checkEdge(cte);
            ID_t otherID = getEdgeDestination(myID, cte);

            // do not process this edge if we have a higher weight node waiting, just add it to our queue
            if (current_nodes.size() > 0 && current_nodes.top().second > cte.getLinks().size()) {
               if (visitedEdges[cte.getIID()] == 0) {
                  nodeToWeight.first = otherID;
                  nodeToWeight.second = cte.getLinks().size();
                  current_nodes.push(nodeToWeight);
                }
                continue;
             }

             // if we hit a node that is not yet initialized, try to initialize it using it's highest neighbor
             // TODO: it would be better to implement scaffold merging instead and initialize the node since its highest initialized neighbor is not necessarely its highest neighbor
             if (ctg2ort[myID] == NONE && visitedEdges[cte.getIID()] == 0) {
if (max < cte.getLinks().size()) { max = cte.getLinks().size(); } 
                if (ctg2ort[otherID] != NONE) {
                  if (globals.debug >=1) {
                     cerr << "WE HIT UNINITIALIZED NODE " << myID << " using edge " << cte.getIID() <<  " (" << cte.getContigs().first << ", " << cte.getContigs().second << ") AND IM BACKPEDALING TO NODE " << otherID << " THIS NODE IS " << (max-cte.getLinks().size()) << " WORSE THAN BEST UNINITIALIZED" << endl;
                  }
                  nodeToWeight.first = otherID;
                  nodeToWeight.second = cte.getLinks().size(); 
                  current_nodes.push(nodeToWeight);
                }
                continue;
            }

            // orient the node
            contigOrientation orient = getOrientationByAllEdges(otherID, ctg2ort, ctg2lnk, edge_bank);
            // add the edge
            if (visitedEdges[cte.getIID()] == 0) {
               std::stringstream oss;
               oss << myID << "_" << otherID;

               if (globals.debug >= 2) {
                  cerr << "LOOKING AT EDGE BETWEEEN " << cte.getContigs().first << " and " << cte.getContigs().second << endl;
                  cerr << "LOOKING AT EDGE BETWEEEN " << cte.getContigs().second << " and " << cte.getContigs().first << endl;
               }

               double maxWeight = (getMaxWeightEdge(myID, ctg2lnk, edge_bank) < getMaxWeightEdge(otherID, ctg2lnk, edge_bank) ? getMaxWeightEdge(otherID, ctg2lnk, edge_bank) : getMaxWeightEdge(myID, ctg2lnk, edge_bank));
               if (globals.debug >= 1) {
                  cerr << "PROCESSING EDGE WITH ID " << cte.getIID() << " BETWEEN CONTIGS " << cte.getContigs().first << " AND " << cte.getContigs().second << " WEIGHT " << cte.getLinks().size() << " LAST WEIGHT WAS " << maxWeight << " AND INT VERSION OF LAST IS " << round((double)maxWeight / 4) << endl;
               }

               if (globals.skipLowWeightEdges && (double)cte.getLinks().size() / (double)maxWeight <= 0.15) {
                  if (globals.debug >= 1) { cerr << "SKIPPING EDGE " << cte.getIID() << endl; }
                  setEdgeStatus(cte, edge_bank, BAD_SKIP);
                  badCount++;
               }
               // determine bad edges
               // an edge is bad if either
               // 1. It orients the otherID contig inconsistently with it's current orientation
               // 2. It orients the myID contig inconsistently given otherID orientation
               // 3. There is already an edge between two nodes of a different adjacency
               else if (ctg2ort[otherID] != NONE && 
                   ((orient != ctg2ort[otherID] || getOrientation(ctg2ort[otherID], cte) != ctg2ort[myID]) || 
                    (ctg2edges[oss.str()] != ContigEdge_t::NULL_ADJACENCY && ctg2edges[oss.str()] != cte.getAdjacency()))) {
                  //inconsistent edge
                  cerr << "BAD ORI EDGE: " << cte.getIID() << " between " << cte.getContigs().first << " and " << cte.getContigs().second << " with dist " << cte.getSize() << " and std " << cte.getSD() << " and the orientation is " << cte.getAdjacency() << endl;
                  badCount++;
                  
                  // update the edge in the bank so it is marked bad
                  setEdgeStatus(cte, edge_bank, BAD_ORI);
               }
               // an edge linking to a node in another scaffold is a bad scaffold edge
               else if (ctg2scf[otherID] != UNINITIALIZED && ctg2scf[otherID] != scfIID) {
                  setEdgeStatus(cte, edge_bank, BAD_SCF);
                  badCount++;
               }
               else {
                  ctg2ort[otherID] = orient;
                  ctg2edges[oss.str()] = cte.getAdjacency();

                  computeContigPositionUsingAllEdges(otherID, ctg2srt, ctg2ort, ctg2lnk, edge_bank, contig_bank, INITIAL_STDEV, true);
                  if (!isBadEdge(cte.getIID(), edge_bank)) {
                     // add tiling info
                     // offset is always in terms of the lowest position in scaffold of the contig, that is if we have ---> <---- then the offset of the second
                     // contig is computed from the head of the arrow, not the tail
                     Tile_t tile;
                     tile.source = myID;
                     tile.source_type = Contig_t::NCODE;
 
                     uint32_t length = ctg2len[myID];
                     if (ctg2ort[myID] == FWD) {
                        tile.range.begin = 0;
                        tile.range.end = length;
                        tile.offset = ctg2srt[myID];
                     }
                     else {
                        tile.range.begin = length;
                        tile.range.end = 0;
                        tile.offset = ctg2srt[myID] - length;
                     }
                     ctg2scf[myID] = scfIID;
                     addTile(tiles, myID, tile);
                     tile.clear();
                     
                     // now add the other contig's tile to the scaffold
                     tile.source = otherID;
                     tile.source_type = Contig_t::NCODE;
 
                     length = ctg2len[otherID];
                     if (ctg2ort[otherID] == FWD) {
                        tile.range.begin = 0;
                        tile.range.end = length;
                        tile.offset = ctg2srt[otherID];
                     }
                     else {
                        tile.range.begin = length;
                        tile.range.end = 0;
                        tile.offset = ctg2srt[otherID] - length;
                     }
                     ctg2scf[otherID] = scfIID;
                     addTile(tiles, otherID, tile);
                     goodCount++; 
                  } else {
                     badCount++;
                  }
               }
               edges.push_back(cte.getIID());
               visitedEdges[cte.getIID()] = 1;
            
               if (!isBadEdge(cte)) {
                  nodeToWeight.first = otherID;
                  nodeToWeight.second = cte.getLinks().size();
                  current_nodes.push(nodeToWeight);
               
                  if (globals.debug >= 2) {
                     cerr << "PUSHING NODE " << otherID << " WHILE PROCESSING " << myID << " AND EDGE WAS " << cte.getStatus() << " AND SIZE IS " << current_nodes.size() << endl;
                  }
               }
            }
         }
      }
      if (tiles.size() == 0) {
         ctg2ort[ctg.getIID()] = NONE;
         ctg2srt[ctg.getIID()] = UNINITIALIZED;
      } else {         
         scaff.setContigEdges(edges);
         scaff.setContigTiling(tiles);
         scaff.setIID(scfIID);
         scaffs.push_back(scaff);
         scfIID++;
      }
   } // end of streaming over the contig bank
      
   // finally initialize any contigs not in scaffolds if we were asked to do it
   if (globals.initAll == true) {
      Scaffold_t scaff;
      std::vector<Tile_t> tiles;
      std::vector<ID_t> edges;

      contig_stream.seekg(0,BankStream_t::BEGIN);
      while (contig_stream >> ctg) {
         if (ctg2ort[ctg.getIID()] == NONE && ctg2srt[ctg.getIID()] == UNINITIALIZED) {
            Tile_t tile;
            tile.source = ctg.getIID();
            tile.source_type = Contig_t::NCODE;
            tile.offset = 0;
            tile.range.begin = 0;
            tile.range.end = ctg.getLength();
            ctg2ort[ctg.getIID()] = FWD;
            ctg2srt[ctg.getIID()] = 0;
            ctg2scf[ctg.getIID()] = scfIID;

            tiles.clear();
            edges.clear();            
            tiles.push_back(tile);
   
            scaff.setContigEdges(edges);
            scaff.setContigTiling(tiles);
            scaff.setIID(scfIID);
            scaffs.push_back(scaff);
            scfIID++;
         }
      }
   }

   // compress gap if we can
   compressGaps(scaffs, ctg2srt, ctg2ort, ctg2lnk, edge_bank, contig_bank);

   // preform graph simplification
   sortContigs(scaffs, contig_bank, edge_bank);

   if (globals.debug >= 1) { cerr << "BEGIN COMPRESSION" << endl; }
   if (globals.compressMotifs == true) {
      transitiveEdgeRemoval(scaffs, edge_bank, ctg2lnk, globals.debug);
      // allow low-weight edges to be rescued
      AMOS::ContigEdge_t cte;
      for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
         edge_bank.fetch(ci->iid, cte);
         
         // TODO: ideally this shouldn't rely on the ctg2srt hash table but instead look it up in the scaffold tiling
         // for that we need to find which scaffold contains the tile
         // also scaffold should store a hash table that returns the tile given an id quickly
         if (cte.getStatus() == BAD_SKIP) {
            Contig_t first, second;
            contig_bank.fetch(cte.getContigs().first, first);
            contig_bank.fetch(cte.getContigs().second, second);

            edgeStatus st = isEdgeConsistent(first, second, cte, ctg2ort, ctg2srt, ctg2scf);
            st = (st == GOOD_EDGE ? BAD_SKIP : st);
            setEdgeStatus(cte, edge_bank, st, false);
            if (globals.debug >= 1) { cerr << "FOR SKIPPED EDGE " << cte.getIID() << " SET EDGE STATUS TO BE " << st << endl; }
         }
      }
      flushEdgeStatus(edge_bank);
      reduceGraph(scaffs, contig_bank, edge_bank, ctg2ort, ctg2lnk, motifs);
      sortContigs(scaffs, contig_bank, edge_bank);
      // reset the transitive edges because we may have collapsed nodes so old transitive edges are no longer transitive
      resetEdges(edge_bank, BAD_TRNS);
      transitiveEdgeRemoval(scaffs, edge_bank, ctg2lnk, globals.debug);
   }
   if (globals.debug >= 1) { cerr << "DONE COMPRESSION" << endl; }

   // finally output to the bank
   for (vector<Scaffold_t>::iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      scaff_stream.append(*itScf);
   }

   for(vector<Motif_t>::iterator itScf = motifs.begin(); itScf < motifs.end(); itScf++) {
      motif_stream.append(*itScf);
   }

   // clear data
   ctg2scf.clear();
   ctg2srt.clear();
   ctg2ort.clear();
   // clear the edge lists
   for (hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> >::iterator i = ctg2lnk.begin(); i != ctg2lnk.end(); i++) {
      delete (i->second);
   }
   delete(cte2weight);

   edge_bank.close();
   contig_stream.close();
   contig_bank.close();
   motif_stream.close();
   scaff_stream.close();

   cerr << "Finished! Had " << goodCount << " Good links and " << badCount << " bad ones." << endl;
   return 0;
}
