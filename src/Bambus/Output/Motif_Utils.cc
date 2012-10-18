#include <string>
#include <map>
#include <set>
#include <queue>
#include <iterator>
#include <iostream>
#include <string>

#include "fasta.hh"
#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

#include "Utilities_Bundler.hh"

#include "Motif_Utils.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;
using namespace Output;

#ifdef AMOS_HAVE_BOOST
void computeMotifStats(AMOS::Motif_t &scf, Bank_t &edge_bank, MotifStats &stat);

void computeMotifStats(AMOS::Motif_t &scf, Bank_t &edge_bank, MotifStats& stat) {
   Graph g;
   hash_map<ID_t, Vertex, hash<ID_t>, equal_to<ID_t> > nodeToDescriptor;
   hash_map<ID_t, Tile_t, hash<ID_t>, equal_to<ID_t> > nodeToTile;

   VertexName vertexNames = get(boost::vertex_name, g);
   stat.length = stat.numSinks = stat.minWeight = stat.totalWeight = 0;
   stat.overlapRatio = 0;
   uint32_t start = 0;
   bool first = true;
   uint32_t end = 0;

   // build boost-based graph
   for (vector<Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(); tileIt++) {
       nodeToDescriptor[tileIt->source] = boost::add_vertex(g);
       vertexNames[nodeToDescriptor[tileIt->source] ] = tileIt->source;
       nodeToTile[tileIt->source] = *tileIt;

       if (first || start > tileIt->offset) { first = false; start = tileIt->offset; }
       if (end < tileIt->offset + tileIt->range.getLength()) { end = tileIt->offset + tileIt->range.getLength(); }
   }

   for (vector<ID_t>::const_iterator edgeIt = scf.getContigEdges().begin(); edgeIt < scf.getContigEdges().end(); edgeIt++) {
       ContigEdge_t cte;
       edge_bank.fetch((*edgeIt), cte);
       if (Bundler::isBadEdge(cte)) {
          continue;
       }
       if (nodeToDescriptor.find(cte.getContigs().first) == nodeToDescriptor.end() || nodeToDescriptor.find(cte.getContigs().second) == nodeToDescriptor.end()) {
          continue;
       }
       if (boost::edge(nodeToDescriptor[cte.getContigs().first], nodeToDescriptor[cte.getContigs().second], g).second || boost::edge(nodeToDescriptor[cte.getContigs().second], nodeToDescriptor[cte.getContigs().first], g).second) {
          // duplicate edge already exists, avoid adding it to avoid cycles
          continue;
       }

       pair<Edge, bool> e = boost::add_edge(nodeToDescriptor[cte.getContigs().first], nodeToDescriptor[cte.getContigs().second], g);
      if (stat.minWeight == 0 || stat.minWeight > cte.getContigLinks().size()) {
         stat.minWeight = cte.getContigLinks().size();
      }
      stat.totalWeight += cte.getContigLinks().size();
   }

   ID_t sourceID = vertexNames[computeSource(g)];
   pair<VertexIterator, VertexIterator> i;
   set<ID_t> sinks;
   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      Vertex first = *i.first;
      if (boost::out_degree(first, g) == 0) {
         sinks.insert(vertexNames[first]);
         stat.numSinks++;
      }
   }
   uint8_t* overlap = new uint8_t[end - start+1];
   uint32_t overlapCount = 0;
   memset(overlap, 0, end-start+1);
   uint32_t minPosition = 0;
   uint32_t maxPosition = 0;
   // build boost-based graph
   for (vector<Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(); tileIt++) {
       if (tileIt->source != sourceID) {
          if (minPosition == 0 || minPosition > tileIt->offset) {
             minPosition = tileIt->offset;
          } 
          if (sinks.find(tileIt->source) == sinks.end()) {
             if (maxPosition < tileIt->offset + tileIt->range.getLength()) {
                maxPosition = tileIt->offset + tileIt->range.getLength();
             }
             for (int i = tileIt->offset - start; i < tileIt->offset + tileIt->range.getLength() - start; i++) {
                if (overlap[i] > 0) { overlapCount++; }
                overlap[i]++;
             }
          }
       }
    }
    stat.length = (maxPosition == 0 || maxPosition < minPosition ? 0 : maxPosition - minPosition);
    stat.edgeRatio = boost::num_edges(g) - (boost::num_vertices(g) - 1);
    stat.overlapRatio = (stat.length != 0 ? (double)overlapCount / stat.length : 0.0); 
    delete[] overlap;
}
#endif

namespace Output
{
void getMotifStats(AMOS::Motif_t &scf, Bank_t &edge_bank, MotifStats &stat) {
#ifdef AMOS_HAVE_BOOST
   return computeMotifStats(scf, edge_bank, stat);
#else
   cerr << "Error: the boost library cannot be found. Will not be processing motif " << scf.getEID() << endl;
   return;
#endif
}
void buildMotifGraph(Graph &g, hash_map<ID_t, Vertex, hash<ID_t>, equal_to<ID_t> > &nodeToDescriptor,
                        hash_map<ID_t, Tile_t, hash<ID_t>, equal_to<ID_t> > &nodeToTile, Motif_t &scf, Bank_t &edge_bank) {
#ifdef AMOS_HAVE_BOOST
    VertexName vertexNames = get(boost::vertex_name, g);

    // build boost-based graph
    for (vector<Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(); tileIt++) {
        nodeToDescriptor[tileIt->source] = boost::add_vertex(g);
        vertexNames[nodeToDescriptor[tileIt->source] ] = tileIt->source;
        nodeToTile[tileIt->source] = *tileIt;
     }

    for (vector<ID_t>::const_iterator edgeIt = scf.getContigEdges().begin(); edgeIt < scf.getContigEdges().end(); edgeIt++) {
        ContigEdge_t cte;
        edge_bank.fetch((*edgeIt), cte);
        if (Bundler::isBadEdge(cte)) {
           continue;
        }
        if (nodeToDescriptor.find(cte.getContigs().first) == nodeToDescriptor.end() || nodeToDescriptor.find(cte.getContigs().second) == nodeToDescriptor.end()) {
           continue;
        }
        pair<Edge, bool> e = boost::add_edge(nodeToDescriptor[cte.getContigs().first], nodeToDescriptor[cte.getContigs().second], g);
    }
#else
   cerr << "Error: Boost library is not available" << endl;
#endif
}

Vertex computeSource(Graph &g) {
#ifdef AMOS_HAVE_BOOST
        Vertex source;        uint32_t counter = 0;
        hash_map<Vertex, bool, hash<Vertex>, equal_to<Vertex> > edgeIncoming;

        pair<VertexIterator, VertexIterator> i;
        for (i = boost::vertices(g); i.first != i.second; ++i.first) {
                Vertex first = *i.first;
                if (boost::in_degree(first, g) == 0) {
                        if (counter != 0) {
                                cerr << "Error: multiple source nodes for graph" << endl;
                                exit(1);
                        }
                        else {
                                counter++;
                                source = first;
                        }
                }
        }
        if (counter == 0) {
                cerr << "Error: no source node for graph" << endl;
                exit(1);
        } else {
                return (source);
        }
#else
   	cerr << "Error: Boost library is not available" << endl;
	return -1;
#endif
}
}
