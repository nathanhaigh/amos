#include <time.h>
#include <math.h>

#include "Utilities_Bundler.hh"

#include "Library_AMOS.hh"
#include "Fragment_AMOS.hh"
#include "Overlap_AMOS.hh"
#include "Contig_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include "ContigEdge_AMOS.hh"

namespace Bundler
{
// internal-only definitions   
AMOS::Bank_t *global_edge_bank = NULL;
HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > cte2bad;
bool cte2badInit = false;

void outputEdge(std::ofstream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t iid, const char *start, const char *end, const char *direction);
void outputEdges(std::ostream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t currCtg, int32_t edgeTypes[], int32_t debug);
void outputLibrary(const std::string &outputPrefix, int32_t debug);
void outputEvidenceXML(const std::string &bank, AMOS::Bank_t &contig_bank, const std::string &outputPrefix, int32_t debug);
void outputOutXML(AMOS::Bank_t &edge_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string &outputPrefix, int32_t debug);

void outputAGP(AMOS::Bank_t &contig_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string& outputPrefix, int32_t debug);
void outputDOT(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               std::vector<AMOS::Scaffold_t> &scaffs, 
               const std::string &outputPrefix,
               int32_t debug);
void outputBAMBUS(const std::string &bank, 
               AMOS::Bank_t &contig_bank, 
               AMOS::Bank_t &edge_bank, 
               const std::vector<AMOS::Scaffold_t> &scaffs, 
               const std::string &outputPrefix, 
               int32_t debug);


struct EdgeOrderCmp
{
  bool operator () (const AMOS::ID_t & a, const AMOS::ID_t & b)
  {
      assert(global_edge_bank);
      AMOS::ContigEdge_t ctgA, ctgB;
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

double computeArrivalRate(const std::vector<AMOS::Contig_t> &contigs) {
   double result = 0;
   int32_t numFragments = 0;
   
   for (std::vector<AMOS::Contig_t>::const_iterator i = contigs.begin(); i < contigs.end(); i++) {
      // compute global arrival rate
      result += i->getAvgRho();
      numFragments += (i->getReadTiling().size() > 0 ? i->getReadTiling().size() - 1 : 0);
   }
   if (result == 0) {
      return 0;
   }

   return numFragments / result;
}

#ifdef AMOS_HAVE_BOOST
void buildGraph(
            Graph &g, 
            AMOS::BankStream_t &node_stream, AMOS::BankStream_t &edge_stream, 
            AMOS::IBankable_t *node, AMOS::IBankable_t *edge,
            int32_t redundancy) {
   // build boost-based graph   
   HASHMAP::hash_map<AMOS::ID_t, Vertex, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > nodeToDescriptor;
   VertexName vertexNames = get(boost::vertex_name, g);
   VertexLength vertexLength = get(boost::vertex_index1, g);
   EdgeWeight edgeWeights = get(boost::edge_weight, g);
   
   // build boost-based graph
   // add node 0, it represents links outside of the set of contings (for example to singletons)
   nodeToDescriptor[0] = boost::add_vertex(g);
   vertexNames[nodeToDescriptor[0] ] = 0;
   vertexLength[nodeToDescriptor[0] ] = 0;

   while (node_stream >> (*node)) {
      assert(node->getIID() != 0);
      nodeToDescriptor[node->getIID()] = boost::add_vertex(g);
      vertexNames[nodeToDescriptor[node->getIID()] ] = node->getIID();
      if (dynamic_cast<AMOS::Contig_t *>(node) != NULL) {
         AMOS::Contig_t *ctg = dynamic_cast<AMOS::Contig_t *>(node);
         vertexLength[nodeToDescriptor[node->getIID()] ] = ctg->getLength();
      } else {
         vertexLength[nodeToDescriptor[node->getIID()] ] = 1;
      }
   }
   node_stream.seekg(0,AMOS::BankStream_t::BEGIN);

   while (edge_stream >> (*edge)) {
      AMOS::ID_t firstNode, secondNode;
      double_t weight = 0;
      if (dynamic_cast<AMOS::ContigEdge_t *>(edge) != NULL) {
         AMOS::ContigEdge_t* cte = dynamic_cast<AMOS::ContigEdge_t *>(edge);
         if (cte->getContigLinks().size() < redundancy || cte->getContigLinks().size() == 0) {
            continue;
         }
         
         firstNode = cte->getContigs().first;
         secondNode = cte->getContigs().second;
         weight = 1;/*((double)1 / cte->getContigLinks().size());*/
      }
      else if (dynamic_cast<AMOS::Overlap_t *>(edge) != NULL) {
         AMOS::Overlap_t* overlap = dynamic_cast<AMOS::Overlap_t*>(edge);
         firstNode = overlap->getReads().first;
         secondNode = overlap->getReads().second;
         weight = 1;
      }
      else {
         std::cerr << "buildGraph(): error, unknown type of edge bank " << edge_stream.getType() << " skipping record" << std::endl;
         continue;
      }
      std::pair<Edge, bool> e = boost::add_edge(nodeToDescriptor[firstNode], nodeToDescriptor[secondNode], g);
      edgeWeights[e.first] = weight;
   }
   edge_stream.seekg(0,AMOS::BankStream_t::BEGIN);
}
#else
void buildGraph(
            Graph &g, 
            AMOS::BankStream_t &node_stream, AMOS::BankStream_t &edge_stream, 
            AMOS::IBankable_t *node, AMOS::IBankable_t *edge,
            int32_t redundancy) {
   std::cerr << "Unable to build graph. Boost library is required. Please double check your installation and recompile AMOS" << std::endl;
}
#endif //AMOS_HAVE_BOOST

AMOS::ID_t getEdgeDestination(const AMOS::ID_t &edgeSrc, const AMOS::ContigEdge_t &cte) {
   if (cte.getContigs().second == edgeSrc) {
      return cte.getContigs().first;
   }
   else {
      return cte.getContigs().second;
   }
}

contigOrientation getOrientation(contigOrientation &myOrient, const AMOS::ContigEdge_t &cte) {
   assert(myOrient != NONE);

   switch (cte.getAdjacency()) {
      case AMOS::ContigEdge_t::NORMAL:
      case AMOS::ContigEdge_t::ANTINORMAL:
         // incase of normal or anti-normal, our directions match
         return myOrient;
         break;
      case AMOS::ContigEdge_t::INNIE:
      case AMOS::ContigEdge_t::OUTIE:
         if (myOrient == FWD) { return REV;} else { return FWD;}
         break;
      default:
         std::cerr << "Unknown orientation " << cte.getAdjacency() << std::endl;
         exit(1);
   };
}

AMOS::Size_t adjustSizeBasedOnAdjacency(AMOS::LinkAdjacency_t edgeAdjacency, AMOS::Size_t gapSize, 
                  AMOS::Size_t firstLength, AMOS::Size_t secondLength,
                  contigOrientation firstOrient, contigOrientation secondOrient,
                  AMOS::SD_t fudgeFactor) {   
   assert(firstOrient != NONE);

   AMOS::Size_t size = gapSize + fudgeFactor;
   contigOrientation expectedOrient = NONE;
   switch (edgeAdjacency) {
      case AMOS::ContigEdge_t::NORMAL:
         size += firstLength;
         expectedOrient = FWD;
         break;
      case AMOS::ContigEdge_t::ANTINORMAL:
         size += secondLength;
         expectedOrient = REV;
         break;
      case AMOS::ContigEdge_t::INNIE:
         size += firstLength;
         size += secondLength;
         expectedOrient = FWD;
         break;
      case AMOS::ContigEdge_t::OUTIE:
         // no adjustment for OUTIE
         expectedOrient = REV;
         break;
      default:
         std::cerr << "Unknown adjacency " << edgeAdjacency << std::endl;
         exit(1);
   };

   // place the second contig behind the first one if we are reversed
   if (firstOrient != expectedOrient) {
      size *= -1;
   }
std::cerr << "PROCESSING EDGE OF LENGTH " << gapSize << " ORI " << edgeAdjacency << " AND THE SIZE IS ADJUSTED TO BE " << size << std::endl;
   return size;
}

void checkEdgeID(const AMOS::ID_t &id) {
   if (id == 0) {
      std::cerr << "ERROR: FOUND INVALID EDGES THAT SHOULD HAVE BEEN SCREENED OUT!\n";
      std::exit(1);
   }
}

void checkEdge(const AMOS::ContigEdge_t &cte) {
   if (cte.getContigs().first == 0 || cte.getContigs().second == 0) {
      std::cerr << "ERROR: FOUND INVALID EDGES THAT SHOULD HAVE BEEN SCREENED OUT!\n";
      std::exit(1);
   }
}

void setEdgeStatus(AMOS::ContigEdge_t &cte, AMOS::Bank_t &edge_bank, int status) {
   setEdgeStatus(cte, edge_bank, status, true);
}

void setEdgeStatus(AMOS::ContigEdge_t &cte, AMOS::Bank_t &edge_bank, int status, bool now) {
   cte2bad[cte.getIID()] = status;
   cte.setStatus(status);
   if (now == true) {
      edge_bank.replace(cte.getIID(), cte);
   }
}

void flushEdgeStatus(AMOS::Bank_t &edge_bank) {
   AMOS::ContigEdge_t cte;

   for (HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >::iterator i = cte2bad.begin(); i != cte2bad.end(); i++) {
      edge_bank.fetch(i->first, cte);
      cte.setStatus(i->second);
      edge_bank.replace(i->first, cte);
   }
}

bool isBadEdge(AMOS::ID_t cteID, AMOS::Bank_t &edge_bank) {
   if (cte2badInit = false) {
      AMOS::ContigEdge_t cte;
      for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
         edge_bank.fetch(ci->iid, cte);
         cte2bad[ci->iid] = cte.getStatus();
      }
      cte2badInit = true;
   }

   return (cte2bad[cteID] != GOOD_EDGE && cte2bad[cteID] != NULL_STATUS);
}

bool isBadEdge(const AMOS::ContigEdge_t &cte) {
   if (cte.getStatus() != GOOD_EDGE && cte.getStatus() != NULL_STATUS) {
      return true;
   }
   
   return false;
}

void resetEdges(AMOS::Bank_t &edge_bank, edgeStatus toChange) {
   AMOS::ContigEdge_t cte;
   for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
      edge_bank.fetch(ci->iid, cte);

      if (cte.getStatus() == toChange) {
         setEdgeStatus(cte, edge_bank, GOOD_EDGE);
      }
   }
}
   
void resetEdges(AMOS::Bank_t &edge_bank, std::set<AMOS::ID_t> &edges, edgeStatus toChange) {
   AMOS::ContigEdge_t cte;
   for (std::set<AMOS::ID_t>::iterator i = edges.begin(); i != edges.end(); i++) {
      edge_bank.fetch(*i, cte);
      if (cte.getStatus() == toChange) {
         setEdgeStatus(cte, edge_bank, GOOD_EDGE);
      }
   }
}

AMOS::Size_t getTileOverlap(AMOS::Tile_t tileOne, AMOS::Tile_t tileTwo) {
   AMOS::Size_t start = (tileOne.offset > tileTwo.offset ? tileOne.offset : tileTwo.offset);
   AMOS::Size_t maxOne = tileOne.offset + (tileOne.range.getLength() - 1);
   AMOS::Size_t maxTwo = tileTwo.offset + (tileTwo.range.getLength() - 1);
   AMOS::Size_t end = (maxOne < maxTwo ? maxOne : maxTwo);
      
   return (end-start+1);
}

void linearizeScaffolds(std::vector<AMOS::Scaffold_t> &scaffs, 
                        AMOS::Bank_t &edge_bank,
                        HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t, EdgeWeightCmp>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > &ctg2lnk,
                        int32_t debugLevel) {
   std::vector<AMOS::Scaffold_t> linearScaffolds;
   AMOS::ID_t scfIID = 0;

   for(std::vector<AMOS::Scaffold_t>::iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctg2conflict;
      HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > visited;               // contigs we've processed
      AMOS::ContigEdge_t cte;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
         visited[tileIt->source] = 0;

         std::set<AMOS::ID_t>* s = ctg2conflict[tileIt->source];
         if (s == NULL) { s = new std::set<AMOS::ID_t>(); ctg2conflict[tileIt->source] = s;}

         for (std::vector<AMOS::Tile_t>::const_iterator tilePrev = itScf->getContigTiling().begin(); tilePrev < tileIt; tilePrev++) {
            if (tilePrev->offset + (tilePrev->range.getLength() - 1) > tileIt->offset) {
               // it conflicts me
               std::set<AMOS::ID_t>* s = ctg2conflict[tileIt->source];
               s->insert(tilePrev->source);
               ctg2conflict[tileIt->source] = s;
            }
         }
         for (std::vector<AMOS::Tile_t>::const_iterator tileNext = tileIt+1; tileIt < itScf->getContigTiling().end() && (tileNext->offset < tileIt->offset + (tileIt->range.getLength()-1)); tileNext++) {
            // it conflicts me
            std::set<AMOS::ID_t>* s = ctg2conflict[tileIt->source];
            s->insert(tileNext->source);
            ctg2conflict[tileIt->source] = s;
         }
      }

      // now that we have conflicting links we can go and add contigs and all their neighbors until we find a conflict
      bool done = false;
      while (!done) {
         done = true;
         for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
            if (visited[tileIt->source] == 0) {
               // create a new scaffold here
               AMOS::Scaffold_t scaff;
               std::vector<AMOS::Tile_t> tiles;
               std::vector<AMOS::ID_t> edges;

               done = false;

               HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > inCurrScf;
               inCurrScf[tileIt->source] = 1;

               std::queue<AMOS::ID_t> current_nodes;
               current_nodes.push(tileIt->source);

               while (current_nodes.size() != 0) {
                  AMOS::ID_t curr = current_nodes.front();
                  current_nodes.pop();

                  if (visited[curr] == 1) continue;
                  visited[curr] = 1;

                  std::set<AMOS::ID_t, EdgeWeightCmp>* s = ctg2lnk[curr];
                  if (s == NULL) { continue; }
                  for (std::set<AMOS::ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
                     checkEdgeID(*i);
                     edge_bank.fetch(*i, cte);
                     checkEdge(cte);

                     if (isBadEdge(cte)) { continue; }
                     AMOS::ID_t neighbor = getEdgeDestination(curr, cte);
                     if (visited[neighbor] == 1 || inCurrScf[neighbor] == 1) { continue; }
                     std::set<AMOS::ID_t> *conflicts = ctg2conflict[neighbor];
                     bool hasConflicts = false;

                     for (std::set<AMOS::ID_t>::iterator j = conflicts->begin(); j != conflicts->end(); j++) {
                        if (inCurrScf[*j] == 1) {
                          hasConflicts = true;
                          break;
                        } 
                     }

                     if (hasConflicts) {
                        setEdgeStatus(cte, edge_bank, BAD_RPT);
                     } else {
                        current_nodes.push(neighbor);
                        inCurrScf[neighbor] = 1;
                     }
                  } // neighbors loop
               } // current connected set of nodes

               // loop through all the contigs and edges to see if they should be assigned to the current scaffold
               for (std::vector<AMOS::Tile_t>::const_iterator scfTiles = itScf->getContigTiling().begin(); scfTiles < itScf->getContigTiling().end(); scfTiles++) {
                  if (inCurrScf[scfTiles->source] == 1) { tiles.push_back(*scfTiles); }
               }
               for (std::vector<AMOS::ID_t>::const_iterator edgeIt = itScf->getContigEdges().begin(); edgeIt < itScf->getContigEdges().end(); edgeIt++) {
                  edge_bank.fetch(*edgeIt, cte);
                  if (inCurrScf[cte.getContigs().first] == 1 && inCurrScf[cte.getContigs().second] == 1) { edges.push_back(cte.getIID()); }
               }

               scaff.setContigEdges(edges);
               scaff.setContigTiling(tiles);
               scaff.setIID(scfIID);
               linearScaffolds.push_back(scaff);
               scfIID++;
            } // if node is not visited
         } // for all nodes in the current scaffolds
      } // while we are not done with the current scaffold
   }

   scaffs = linearScaffolds;
}

HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *cte2top;
struct EdgeTopoCmp
{
   bool operator () (const AMOS::ContigEdge_t * a, const AMOS::ContigEdge_t* b) const
   {
      assert(cte2top);
      int32_t posA = (*cte2top)[a->getContigs().second];
      assert(posA);
      int32_t posB = (*cte2top)[b->getContigs().second];
      assert(posB);

      if (posA < posB) {
         return true;
      }
      else {
         return false;
      }
   }
};

bool topoSortRecursive(AMOS::ID_t curr, 
              AMOS::Bank_t &edge_bank,
              HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t, EdgeWeightCmp>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctg2lnk,
              HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& inProcess,
              HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& visited,
              std::vector<AMOS::ID_t> &sorted,
              int32_t debugLevel)
{
   bool result = true;
   
   if (visited[curr] != 1) {
      inProcess[curr] = 1;

      std::set<AMOS::ID_t, EdgeWeightCmp>* s = ctg2lnk[curr];
      if (s != NULL) {      
         for (std::set<AMOS::ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
            checkEdgeID(*i);
            AMOS::ContigEdge_t *cte = new AMOS::ContigEdge_t();
            edge_bank.fetch(*i, *cte);
            checkEdge(*cte);

            if (isBadEdge(*cte)) { continue; }
            
            // outgoing edge
            if (cte->getContigs().first == curr) {
               if (visited[cte->getContigs().second] == 1) {
                  // do nothing this node is finished
               } else if (inProcess[cte->getContigs().second] == 1) {
                  result = false;
               } else {
                  result &= topoSortRecursive(cte->getContigs().second, edge_bank, ctg2lnk, inProcess, visited, sorted, debugLevel);
               }
            }              
         }
      }
      sorted.insert(sorted.begin(), curr);
   }
   
   visited[curr] = 1;
   inProcess[curr] = 0;
   return result;
}

void transitiveEdgeRecursive(AMOS::ID_t curr, 
                             AMOS::ID_t edge,
                             AMOS::Bank_t &edge_bank,
                             HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t, EdgeWeightCmp>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctg2lnk,
                             HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& inProcess,
                             HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& visited,
                             HASHMAP::hash_map<AMOS::ID_t, std::pair<double, double>, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& mean,
                             HASHMAP::hash_map<AMOS::ID_t, AMOS::Size_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& size,
                             int32_t debugLevel) {

   std::set<AMOS::ContigEdge_t*, EdgeTopoCmp> edges;
   AMOS::ContigEdge_t currEdge;
   inProcess[curr] = 1;
   if (edge != UNINITIALIZED) { edge_bank.fetch(edge, currEdge); }

   if (debugLevel >= 1)  { std::cerr << "VISITING NODE " << curr << " USING EDGE ID " << edge << " WITH SIZE " << currEdge.getSize() << std::endl; }
   std::set<AMOS::ID_t, EdgeWeightCmp>* s = ctg2lnk[curr];
   if (s != NULL) {
      for (std::set<AMOS::ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); i++) {
         checkEdgeID(*i);
         AMOS::ContigEdge_t *cte = new AMOS::ContigEdge_t();
         edge_bank.fetch(*i, *cte);
         checkEdge(*cte);

         if (isBadEdge(*cte)) { continue; }

         // incoming edge
         if (cte->getContigs().second == curr) {
            if (cte->getIID() == edge) {
               // do nothing for this edge
               }
            else if (inProcess[cte->getContigs().first] == 1) {
               assert(edge != UNINITIALIZED);
               double distance = mean[cte->getContigs().first].first; // mean distance from start of traversal to our incoming node
               distance -= mean[currEdge.getContigs().first].first;   // mean distance to the node in the middle of transitive reduction
               // distance is now the distance between the start of the transitive reduction and the middle node
               distance += currEdge.getSize();
               distance += size[currEdge.getContigs().first];
               
               // repeat for stdev
               double sd = mean[cte->getContigs().first].second;
               sd -= mean[currEdge.getContigs().first].second;
               sd += currEdge.getSD();

               if (debugLevel >= 1) {   
                  std::cerr << "COMPUTE DISTANCE IS " << distance << " MEAN: " << sd << " AND EDG EIS " << cte->getSize() << " AND SIZE OF " << currEdge.getContigs().first << " IS " << size[currEdge.getContigs().first] << std::endl;
                  std::cerr << "CTE - DIST IS " << fabs(cte->getSize() - distance) << " AND SD IS " << (cte->getSD() + sd) << std::endl;
               }
               if (fabs(cte->getSize() - distance) <= (cte->getSD() + sd)) {
                  if (debugLevel >= 1) { std::cerr << "DELETING EDGE " << cte->getIID() << " BETWEEN CONTIGS " << cte->getContigs().first << " AND " << cte->getContigs().second << std::endl; }
                  setEdgeStatus(*cte, edge_bank, BAD_TRNS);
               }
            }
            delete cte;
         } else {
            edges.insert(cte);
         }
      }     
      
      // process outgoing edges
      for (std::set<AMOS::ContigEdge_t*, EdgeTopoCmp>::iterator i = edges.begin(); i != edges.end(); i++) {
         AMOS::ContigEdge_t *cte =*i;
         if (cte->getContigs().first != curr) { std::cerr << "Error, edge is neither incoming nor outgoing " << cte->getIID() << " for node " << curr << std::endl; exit(1); }
         
         if (isBadEdge(*cte)) { continue; }
         if (visited[cte->getContigs().second] == 1) { continue; }
         
         if (inProcess[cte->getContigs().second] == 1) {
            // we have found a loop, do not go down this path
         }
         else {
            // update distances to include the edge we're traversing
            mean[curr].first = cte->getSize();
            mean[curr].second = cte->getSD();
            for (HASHMAP::hash_map<AMOS::ID_t, std::pair<double, double>, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >::iterator i = mean.begin(); i != mean.end(); i++) {
               if (i->first != curr) {
                  i->second.first += cte->getSize();
                  i->second.second += cte->getSD();
               }
            }
            transitiveEdgeRecursive(cte->getContigs().second, cte->getIID(), edge_bank, ctg2lnk, inProcess, visited, mean, size, debugLevel);

            // now remove the distance of the edge just finished in the recursive call above
            mean[curr].first = 0;
            mean[curr].second = 0;
            for (HASHMAP::hash_map<AMOS::ID_t, std::pair<double, double>, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >::iterator i = mean.begin(); i != mean.end(); i++) {
               if (i->first != curr) {
                  i->second.first -= cte->getSize();
                  i->second.second -= cte->getSD();
               }
            }
         }
         delete cte;
      }
   }
      
   inProcess[curr] = 0;
   visited[curr] = 1;
}

void transitiveEdgeRemoval(std::vector<AMOS::Scaffold_t> &scaffs, 
                           AMOS::Bank_t &edge_bank,
                           HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t, EdgeWeightCmp>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& ctg2lnk,
                           int32_t debugLevel) {

   HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > inProcess;
   HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > visited;
   HASHMAP::hash_map<AMOS::ID_t, std::pair<double, double>, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > mean;
   HASHMAP::hash_map<AMOS::ID_t, AMOS::Size_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > size;
   cte2top = new HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >();

   for(std::vector<AMOS::Scaffold_t>::iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      if (itScf->getContigTiling().size() > 1) {
         for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
            size[tileIt->source] = tileIt->range.getLength();
         }
   
         for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
            if (visited[tileIt->source] != 1) {
               HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > topoVisited;
               std::vector<AMOS::ID_t> sorted;

               if (topoSortRecursive(tileIt->source, edge_bank, ctg2lnk, inProcess, topoVisited, sorted, debugLevel)) {
                  for (uint32_t i = 0; i < sorted.size(); i++) {
                     (*cte2top)[sorted[i]] = i;
                  }
                  mean.clear();
                  transitiveEdgeRecursive(tileIt->source, UNINITIALIZED, edge_bank, ctg2lnk, inProcess, visited, mean, size, debugLevel);
               }
            }               
         }
      }
   }

   delete cte2top;
}

void outputAGP(AMOS::Bank_t &contig_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string &outputPrefix, int32_t debug) {
   AMOS::Contig_t ctg;

   std::string outputFile = outputPrefix + ".agp";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "# DESCRIPTION: WGS SCAFFOLDS GENERATED FROM AMOS BANK" << std::endl;

   for(std::vector<AMOS::Scaffold_t>::const_iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      int32_t counter = 0;
      int32_t lastEnd = 0;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
         // output the gap
         if (lastEnd != 0 && (tileIt->offset - lastEnd != 0)) {
            stream << itScf->getIID();
            stream << "\t" << lastEnd << "\t" << tileIt->offset;
            stream << "\t" << counter;
            stream << "\t" << (tileIt->offset - lastEnd + 1);
            stream << "\tfragment\tyes";
            
            stream << std::endl;
            counter++;
         }

         std::string sign = "+";
         if (tileIt->range.isReverse()) { sign = "-"; }

         stream << itScf->getIID();
         stream << "\t" << (tileIt->offset+1) << "\t" << (tileIt->offset+tileIt->range.getLength());
         stream << "\t" << counter << "\tW";

         int32_t outputID = 0;
         if (tileIt->source <= contig_bank.getMaxIID()) {
            contig_bank.fetch(tileIt->source, ctg);
            if (ctg.getEID().size() != 0) {
               stream << "\t" << ctg.getEID();
               outputID = 1;
            }
         }
         if (outputID == 0) stream << "\t" << tileIt->source;

         stream << "\t" << tileIt->range.getLo()+1 << "\t" << tileIt->range.getHi();
         stream << "\t" << sign;
         stream << std::endl;
         
         counter++;
         lastEnd = tileIt->offset+tileIt->range.getLength()+1;
      }
   }
   
   stream.close();
}

void outputEdge(std::ostream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t iid, const char *start, const char *end, const char *direction) {
   AMOS::ContigEdge_t outputEdge;
   edge_bank.fetch(iid, outputEdge);

   stream << outputEdge.getContigs().first << ":" << start << "->" << outputEdge.getContigs().second << ":" << end;
   stream << " [ label=\"" << outputEdge.getIID() << " orientation=" << outputEdge.getAdjacency() << " weight=" << outputEdge.getContigLinks().size() << " distance=" << outputEdge.getSize() << " stdev=" << outputEdge.getSD();

   if (outputEdge.getStatus() == BAD_TRNS) {
      stream << " (REDUNDANT) ";
   }
   else if (isBadEdge(outputEdge)) {
      stream << " (UNUSED) ";
   }
   stream << "\" dir=\"" << direction << "\" fontsize = 8 solid color=\"black\" ]\n";   
}

void outputEdges(std::ostream &stream, AMOS::Bank_t &edge_bank, AMOS::ID_t currCtg, int32_t edgeTypes[], int32_t debug) {
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
      } else if (debug >= 3) {
         std::cerr << "WARNING: NO GOOD EDGE BETWEEN CONTIGS" << std::endl;
      }
   }
}

void outputDOT(AMOS::Bank_t &contig_bank, AMOS::Bank_t &edge_bank,
               std::vector<AMOS::Scaffold_t> &scaffs, 
               const std::string &outputPrefix,
               int32_t debug)
{
   AMOS::Contig_t ctg;

   std::string outputFile = outputPrefix + ".dot";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "digraph ROOT {" << std::endl;
   stream << "  rankdir = LR" << std::endl;
   stream << "  rotate = 90" << std::endl;
   stream << "  ranksep = 0.01" << std::endl;
   stream << "  nodesep = 0.01" << std::endl;
   stream << "  fontsize = 8" << std::endl;
   stream << "  margin = \".01,.01\"" << std::endl;
   stream << "  ratio = fill" << std::endl;
   stream << "  size = \"11,8.5\"" << std::endl;
   
   HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > visited;
   for(std::vector<AMOS::Scaffold_t>::iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctginscf;
      HASHMAP::hash_map<AMOS::ID_t, contigOrientation, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctgori;
      
      stream << "subgraph cluster_" << itScf->getIID() << "{" << std::endl;
      stream << "\tlabel = \"" << itScf->getIID() << "\"" << std::endl;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
         ctginscf[tileIt->source] = 1;
         ctgori[tileIt->source] = (tileIt->range.isReverse() ? REV : FWD);

         int32_t angle = -90;
         if (tileIt->range.isReverse()) { angle = 90; }

         int32_t outputID = 0;
         stream << "\t" << tileIt->source << " [label=\"" ;
         if (tileIt->source <= contig_bank.getMaxIID()) {
            contig_bank.fetch(tileIt->source, ctg);
            if (ctg.getEID().size() != 0) { 
	       stream << ctg.getEID();
               outputID = 1;
            }
         }
         if (outputID == 0) stream << tileIt->source;

         stream << " position=(" << tileIt->offset << "," << tileIt->offset + tileIt->range.getLength() - 1;
         stream << ") length=" << tileIt->range.getLength() << "\" height=0.2, fontsize=8, shape=\"house\", ";
         stream << "orientation=" << angle << " ]" << std::endl;         
      }
      AMOS::ID_t currCtg = 0;
      AMOS::ID_t secondCtg = 0;
      int32_t edgeTypes[4];
      
      // sort before output
      global_edge_bank = &edge_bank;
      sort(itScf->getContigEdges().begin(), itScf->getContigEdges().end(), EdgeOrderCmp());
      global_edge_bank = NULL;
      
      for (std::vector<AMOS::ID_t>::const_iterator edgeIt = itScf->getContigEdges().begin(); edgeIt < itScf->getContigEdges().end(); edgeIt++) {
         AMOS::ContigEdge_t cte;
         edge_bank.fetch(*edgeIt, cte);

         if (ctginscf[cte.getContigs().first] != 1 || ctginscf[cte.getContigs().second] != 1) {
            continue;
         }

         if (currCtg != cte.getContigs().first || secondCtg != cte.getContigs().second) {
            outputEdges(stream, edge_bank, currCtg, edgeTypes, debug);

            currCtg = cte.getContigs().first;
            secondCtg = cte.getContigs().second;
            memset(edgeTypes, 0, sizeof(int32_t)*4);
         }
         if (visited[cte.getIID()] == 0) {
            // output the edges based on the orient of the contigs
            contigOrientation edgeFirstCtgOrient = ctgori[cte.getContigs().first];
            contigOrientation edgeSecondCtgOrient = ctgori[cte.getContigs().second];

            contigOrientation first = FWD;
            if (cte.getAdjacency() == AMOS::ContigEdge_t::ANTINORMAL || cte.getAdjacency() == AMOS::ContigEdge_t::OUTIE) {
               first = REV;
            }
            contigOrientation secondCtgOrient = getOrientation(first, cte);

            if (debug >= 3) {
               std::cerr << "****The firstOrient is " << edgeFirstCtgOrient << " and second is " << edgeSecondCtgOrient << std::endl;
               std::cerr << "The edge type is " << cte.getAdjacency() << " and we initialized first to be " << first << std::endl;
               std::cerr << "****The orient we get are " << first << " and second is " << secondCtgOrient << std::endl;
            }
            if (first == edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               edgeTypes[0] = cte.getIID();
               
               if (debug >= 3) { std::cerr << "*** MAIN EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
            }
            else if (first != edgeFirstCtgOrient && secondCtgOrient == edgeSecondCtgOrient) {
               if (debug >= 3) { std::cerr << "*** D, S EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl;}
               edgeTypes[1] = cte.getIID();
            }
            else if (first == edgeFirstCtgOrient && secondCtgOrient != edgeSecondCtgOrient) {
               if (debug >= 3) { std::cerr << "*** S, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
               edgeTypes[2] = cte.getIID();
            }
            else {
               if (debug >= 3) { std::cerr << "*** D, D EDGE BETWEEN " << cte.getContigs().first << " AND " << cte.getContigs().second << " IS " << cte.getAdjacency() <<std::endl; }
               edgeTypes[3] = cte.getIID();
            }
         } else {
               std::cerr << "IN SCAFFOLD " << itScf->getIID() << " ENCOUNTERED EDGE " << cte.getIID() << "THAT IVE ALREADY SEEN!!" << std::endl;
               assert(0);
         }
         visited[cte.getIID()] = 1;
      }
      outputEdges(stream, edge_bank, currCtg, edgeTypes, debug);
      stream << "}" << std::endl;
   }
   stream  << "}" << std::endl;
   stream.close();
}

void outputLibrary(const std::string &bank, const std::string &outputPrefix, int32_t debug) {
   AMOS::BankStream_t library_stream (AMOS::Library_t::NCODE);
   if (!library_stream.exists(bank)){
      std::cerr << "No library account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      library_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open library account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }
   
   std::string outputFile = outputPrefix + ".library";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   AMOS::Library_t lib;
   while (library_stream >> lib) {
      stream << lib.getEID() << " " << lib.getIID() << std::endl;
   }
   
   stream.close();
   library_stream.close();
}

void outputEvidenceXML(const std::string &bank, AMOS::Bank_t &contig_bank, const std::string &outputPrefix, int32_t debug) {
   AMOS::BankStream_t library_stream (AMOS::Library_t::NCODE);
   if (!library_stream.exists(bank)){
      std::cerr << "No library account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      library_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open library account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }
   AMOS::BankStream_t frag_stream (AMOS::Fragment_t::NCODE);
   if (!frag_stream.exists(bank)){
      std::cerr << "No library account found in bank " << bank << std::endl;
      exit(1);
   }
   try {
      frag_stream.open(bank, AMOS::B_READ);
   } catch (AMOS::Exception_t & e) {
      std::cerr << "Failed to open library account in bank " << bank << ": " << std::endl << e << std::endl;
      exit(1);
   }

   std::string outputFile = outputPrefix + ".evidence.xml";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   time_t t;
   time(&t);
   stream << "<?xml version=\"1.0\" ?>" << std::endl;
   stream << std::endl;
   stream << "<EVIDENCE ID=\"" + outputPrefix << "\"" << std::endl;
   stream << "\t\tDATE=\"" << ctime(&t) << "\"" << std::endl;
   stream << "\t\tPROJECT=\"catXML\"" << std::endl;
   stream << "\t\tPARAMETERS=\"" << outputFile << "\"" << std::endl;
   stream << ">" << std::endl;

   // output libraries
   // build a map for library to fragments
   AMOS::Fragment_t frg;
   HASHMAP::hash_map<AMOS::ID_t, std::vector<AMOS::Fragment_t>, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > lib2frg;
   while (frag_stream >> frg) {
      lib2frg[frg.getLibrary()].push_back(frg);
   }
   
   AMOS::Library_t lib;
   while (library_stream >> lib) {
      double min = lib.getDistribution().mean - (3*lib.getDistribution().sd);
      double max = lib.getDistribution().mean + (3*lib.getDistribution().sd);
      stream << "\t<LIBRARY ID=\"" << lib.getIID() << "\" NAME=\"" << lib.getEID() << "\" MIN=\"" << min << "\" MAX=\"" << max << "\">" << std::endl;
      
      for (std::vector<AMOS::Fragment_t>::const_iterator i = lib2frg[lib.getIID()].begin(); i < lib2frg[lib.getIID()].end(); i++) {
         stream << "\t\t<INSERT ID=\"ins_" << i->getIID() << "\" NAME=\"" << i->getEID() << "\">" << std::endl;
         stream << "\t\t\t<SEQUENCE ID=\"seq_" << i->getMatePair().first << "\" NAME=\"" << i->getEID() << "_L\"></SEQUENCE>" << std::endl;
         stream << "\t\t\t<SEQUENCE ID=\"seq_" << i->getMatePair().second << "\" NAME=\"" << i->getEID() << "_R\"></SEQUENCE>" << std::endl;
         stream << "\t\t</INSERT>" << std::endl;
      }
      
      stream << "\t</LIBRARY>" << std::endl;
   }
   frag_stream.close();
   library_stream.close();
   
   // output contigs
   for (AMOS::IDMap_t::const_iterator ci = contig_bank.getIDMap().begin(); ci; ci++) {
      AMOS::Contig_t ctg;
      contig_bank.fetch(ci->iid, ctg);
         
      stream << "\t<CONTIG ID=\"contig_" << ctg.getIID() << "\" NAME=\"" << ctg.getEID() << "\" LEN=\"" << ctg.getLength() << "\">" << std::endl;
      const std::vector<AMOS::Tile_t> &tiling = ctg.getReadTiling();
      for (std::vector<AMOS::Tile_t>::const_iterator  i = tiling.begin(); i < tiling.end(); i++) {
         stream << "\t\t<SEQUENCE ID=\"seq_" << i->source 
                << "\" ORI=\"" << (i->range.isReverse() ? "EB" : "BE") << "\" ASM_LEND=\"" << i->range.getLo() << "\" ASM_REND=\"" << i->range.getHi() 
                << "\"></SEQUENCE>" << std::endl;
      }
      stream << "\t</CONTIG>" << std::endl;
   }
   
   stream << "</EVIDENCE>" << std::endl;
   stream.close();   
}

void outputOutXML(AMOS::Bank_t &edge_bank, const std::vector<AMOS::Scaffold_t> &scaffs, const std::string &outputPrefix, int32_t debug) {
   std::string outputFile = outputPrefix + ".out.xml";
   std::ofstream stream;
   stream.open(outputFile.c_str(), std::ios::out);

   stream << "<GROUPING>" << std::endl;
   for(std::vector<AMOS::Scaffold_t>::const_iterator itScf = scaffs.begin(); itScf < scaffs.end(); itScf++) {
      stream << "\t<SCAFFOLD ID = \"scaff_" << itScf->getIID() << "\">"  << std::endl;
      
      // output the contigs
      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); tileIt++) {
         std::string ori = "BE";
         if (tileIt->range.isReverse()) { ori = "EB"; }

         stream << "\t\t<CONTIG ID=\"contig_" << tileIt->source << "\"" << std::endl;
         stream << "\tX=\"" << tileIt->offset << "\"" << std::endl;
         stream << "\tORI=\"" << ori << "\"" << std::endl;
         stream << "></CONTIG>" << std::endl;
      }

      // output the links in the scaffold
      for (std::vector<AMOS::ID_t>::const_iterator edgeIt = itScf->getContigEdges().begin(); edgeIt < itScf->getContigEdges().end(); edgeIt++) {
         AMOS::ContigEdge_t cte;
         edge_bank.fetch(*edgeIt, cte);


         if (!isBadEdge(cte)) {
            std::vector<AMOS::ID_t>::const_iterator linkIt = cte.getContigLinks().begin();
            for (; linkIt < cte.getContigLinks().end(); linkIt++) {
               stream << "\t\t<LINK ID=\"ins_" << *linkIt << "\"" << std::endl;
               stream << "\tVALID=\"VALID\"" << std::endl;
               stream << "\tTAG=\"T\"" << std::endl;
               stream << "></LINK>" << std::endl;
            }
         }
      }
      stream << "\t</SCAFFOLD>" << std::endl;
   }
   
   // output the bad edges
   stream << "\t<UNUSED>" << std::endl;
   for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
      AMOS::ContigEdge_t cte;
      edge_bank.fetch(ci->iid, cte);

      std::string status;
      if (!isBadEdge(cte)) {
         continue;
      }
      
      switch (cte.getStatus()) {
         case BAD_THRESH:
         case BAD_RPT:
            status = "UNSEEN";
            break;
         case BAD_SKIP:
         case BAD_SCF:
            status = "UNUSED";
            break;
         case BAD_DST:
            status = "DST";
            break;
         case BAD_ORI:
            status = "ORI";
            break;
         case BAD_TRNS:
            status = "TRANSITIVE";
            break;
         default:
            assert(0);
            break;
      }
      
      for (std::vector<AMOS::ID_t>::const_iterator linkIt = cte.getContigLinks().begin(); linkIt < cte.getContigLinks().end(); linkIt++) {
         stream << "\t\t<LINK ID=\"ins_" << *linkIt << "\"" << std::endl;
         stream << "\t\t\tVALID=\"" << status << "\"" << std::endl;
         stream << "\t\t\tTAG=\"T\"" << std::endl;
         stream << "\t\t></LINK>" << std::endl;
      }
   }

   stream << "\t</UNUSED>" << std::endl;
   stream << "</GROUPING>" << std::endl;
   stream.close();
}

void outputBAMBUS(
         const std::string &bank, 
         AMOS::Bank_t &contig_bank, 
         AMOS::Bank_t &edge_bank, 
         const std::vector<AMOS::Scaffold_t> &scaffs, 
         const std::string &outputPrefix, 
         int32_t debug) 
{
   outputLibrary(bank, outputPrefix, debug);
   outputEvidenceXML(bank, contig_bank, outputPrefix, debug);
   outputOutXML(edge_bank, scaffs, outputPrefix, debug);
}

void outputResults(
         const std::string &bank,
         AMOS::Bank_t &contig_bank,
         AMOS::Bank_t &edge_bank, 
         std::vector<AMOS::Scaffold_t> &scaffs, 
         outputType type,
         const std::string &outputPrefix,
         int32_t debug) 
{
   switch (type) {
      case AGP:
         outputAGP(contig_bank, scaffs, outputPrefix, debug);
         break;
      case DOT:
         outputDOT(contig_bank, edge_bank, scaffs, outputPrefix, debug);
         break;
      case BAMBUS:
         outputBAMBUS(bank, contig_bank, edge_bank, scaffs, outputPrefix, debug);
         break;
   }
 }
}
