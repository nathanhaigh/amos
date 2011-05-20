#include <time.h>
#include <math.h>

#include "Utilities_Bundler.hh"

#include "Library_AMOS.hh"
#include "Fragment_AMOS.hh"
#include "Overlap_AMOS.hh"
#include "Contig_AMOS.hh"
#include "ContigLink_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

using namespace Bundler;

// internal-only definitions   
HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > cte2bad;
bool cte2badInit = false;

double Bundler::computeArrivalRate(const std::vector<AMOS::Contig_t *> &contigs) {
   double result = 0;
   int32_t numFragments = 0;
 
   for (std::vector<AMOS::Contig_t *>::const_iterator i = contigs.begin(); i < contigs.end(); i++) {
      // compute global arrival rate
      result += (*i)->getAvgRho();
      numFragments += ((*i)->getReadTiling().size() > 0 ? (*i)->getReadTiling().size() - 1 : 0);
   }
   if (result == 0) {
      return 0;
   }

   return numFragments / result;
}

#ifdef AMOS_HAVE_BOOST
void Bundler::buildGraph(
            Bundler::Graph &g, 
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
         vertexLength[nodeToDescriptor[node->getIID()] ] = ctg->getUngappedLength();
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
void Bundler::buildGraph(
            Bundler::Graph &g, 
            AMOS::BankStream_t &node_stream, AMOS::BankStream_t &edge_stream, 
            AMOS::IBankable_t *node, AMOS::IBankable_t *edge,
            int32_t redundancy) {
   std::cerr << "Unable to build graph. Boost library is required. Please double check your installation and recompile AMOS" << std::endl;
}
#endif //AMOS_HAVE_BOOST

AMOS::ID_t Bundler::getEdgeDestination(const AMOS::ID_t &edgeSrc, const AMOS::ContigEdge_t &cte) {
   if (cte.getContigs().second == edgeSrc) {
      return cte.getContigs().first;
   }
   else {
      return cte.getContigs().second;
   }
}

Bundler::contigOrientation Bundler::getOrientation(contigOrientation &myOrient, const AMOS::ContigEdge_t &cte) {
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

AMOS::Size_t Bundler::adjustSizeBasedOnAdjacency(AMOS::LinkAdjacency_t edgeAdjacency, AMOS::Size_t gapSize, 
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
   return size;
}

void Bundler::checkEdgeID(const AMOS::ID_t &id) {
   if (id == 0) {
      std::cerr << "ERROR: FOUND INVALID EDGES THAT SHOULD HAVE BEEN SCREENED OUT!\n";
      std::exit(1);
   }
}

void Bundler::checkEdge(const AMOS::ContigEdge_t &cte) {
   if (cte.getContigs().first == 0 || cte.getContigs().second == 0) {
      std::cerr << "ERROR: FOUND INVALID EDGES THAT SHOULD HAVE BEEN SCREENED OUT!\n";
      std::exit(1);
   }
}

void Bundler::setEdgeStatus(AMOS::ContigEdge_t &cte, AMOS::Bank_t &edge_bank, int status) {
   setEdgeStatus(cte, edge_bank, status, true);
}

void Bundler::setEdgeStatus(AMOS::ContigEdge_t &cte, AMOS::Bank_t &edge_bank, int status, bool now) {
   cte2bad[cte.getIID()] = status;
   cte.setStatus(status);
   if (now == true) {
      edge_bank.replace(cte.getIID(), cte);
   }
}

void Bundler::flushEdgeStatus(AMOS::Bank_t &edge_bank) {
   AMOS::ContigEdge_t cte;

   for (HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >::iterator i = cte2bad.begin(); i != cte2bad.end(); i++) {
      edge_bank.fetch(i->first, cte);
      cte.setStatus(i->second);
      edge_bank.replace(i->first, cte);
   }
}

bool Bundler::isBadEdge(AMOS::ID_t cteID, AMOS::Bank_t &edge_bank) {
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

bool Bundler::isBadEdge(const AMOS::ContigEdge_t &cte) {
   if (cte.getStatus() != GOOD_EDGE && cte.getStatus() != NULL_STATUS) {
      return true;
   }
   
   return false;
}

void Bundler::resetEdges(AMOS::Bank_t &edge_bank, edgeStatus toChange) {
   AMOS::ContigEdge_t cte;
   for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ci++) {
      edge_bank.fetch(ci->iid, cte);

      if (cte.getStatus() == toChange) {
         setEdgeStatus(cte, edge_bank, GOOD_EDGE);
      }
   }
}
   
void Bundler::resetEdges(AMOS::Bank_t &edge_bank, std::set<AMOS::ID_t> &edges, edgeStatus toChange) {
   AMOS::ContigEdge_t cte;
   for (std::set<AMOS::ID_t>::iterator i = edges.begin(); i != edges.end(); i++) {
      edge_bank.fetch(*i, cte);
      if (cte.getStatus() == toChange) {
         setEdgeStatus(cte, edge_bank, GOOD_EDGE);
      }
   }
}

AMOS::Size_t Bundler::getTileOverlap(AMOS::Tile_t tileOne, AMOS::Tile_t tileTwo) {
   AMOS::Size_t start = (tileOne.offset > tileTwo.offset ? tileOne.offset : tileTwo.offset);
   AMOS::Size_t maxOne = tileOne.offset + (tileOne.range.getLength() - 1);
   AMOS::Size_t maxTwo = tileTwo.offset + (tileTwo.range.getLength() - 1);
   AMOS::Size_t end = (maxOne < maxTwo ? maxOne : maxTwo);
      
   return (end-start+1);
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

void Bundler::transitiveEdgeRemoval(std::vector<AMOS::Scaffold_t> &scaffs, 
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
