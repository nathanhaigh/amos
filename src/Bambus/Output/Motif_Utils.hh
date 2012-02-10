#ifndef MOTIF_UTILS_HH_
#define MOTIF_UTILS_HH_

#include <string>
#include <map>

#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"
#include "Motif_AMOS.hh"

#ifdef AMOS_HAVE_BOOST
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#endif //AMOS_HAVE_BOOST

namespace Output 
{
#ifdef AMOS_HAVE_BOOST
/* give access to common Boost data types through our interface */
// Vertex properties
typedef boost::property<boost::vertex_name_t, uint32_t> VertexProperty;
// Adjacency graph using list data structure
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperty> Graph;
// Accessors
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<Graph>::out_edge_iterator EdgeIterator;

// Properties
typedef boost::property_map<Graph, boost::vertex_name_t>::type VertexName;
#else
typedef uint32_t Vertex;
typedef uint32_t Graph;
#endif //AMOS_HAVE_BOOST

   struct MotifStats {
      std::string name;
      AMOS::ID_t id;
      uint32_t length;
      double   overlapRatio;
      uint32_t edgeRatio;
      uint32_t numSinks;
      uint32_t minWeight;
      uint32_t totalWeight;
   };

   struct MotifOrderCmp
   {
      bool operator () (const MotifStats& a, const MotifStats& b)
      {
         assert(a.numSinks != 0 && a.edgeRatio >= 0);
         assert(b.numSinks != 0 && b.edgeRatio >= 0);

         // first the one with the fewest sinks wins
         if (a.numSinks != b.numSinks) {
            return a.numSinks < b.numSinks;
         }
         if (a.edgeRatio != b.edgeRatio) {
            return a.edgeRatio > b.edgeRatio;
         }
         if (a.totalWeight != b.totalWeight) {
            return a.totalWeight > b.totalWeight;
         }
         // next the one with the highest low link wins
         if (a.minWeight != b.minWeight) {
            return a.minWeight > b.minWeight;
         }
         if (a.overlapRatio != b.overlapRatio) {
            return a.overlapRatio > b.overlapRatio;
         }
         if (a.length != b.length) {
            return a.length > b.length;
         }
         return false;
      }
   };

   void buildMotifGraph(Graph &g, HASHMAP::hash_map<AMOS::ID_t, Vertex, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > &nodeToDescriptor,
                        HASHMAP::hash_map<AMOS::ID_t, AMOS::Tile_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > &nodeToTile, 
			AMOS::Motif_t &scf, AMOS::Bank_t &edge_bank);
   Vertex computeSource(Graph &g);
   void getMotifStats(AMOS::Motif_t &scf, AMOS::Bank_t &edge_bank, MotifStats &stat);
}
#endif
