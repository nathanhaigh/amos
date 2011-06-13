#ifndef UTILITIES_BUNDLER_HH_
#define UTILITIES_BUNDLER_HH_ 1

#include <set>
#include <queue>
#include <iterator>

#ifdef AMOS_HAVE_BOOST
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#endif //AMOS_HAVE_BOOST

#include "Contig_AMOS.hh"
#include "datatypes_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include <set>

#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

namespace Bundler
{
#ifdef AMOS_HAVE_BOOST
   /* give access to common Boost data types through our interface */
   // Vertex properties
   typedef boost::property<boost::vertex_name_t, unsigned int, 
           boost::property<boost::vertex_index1_t, AMOS::Size_t> > VertexProperty;

   // Edge properties
   typedef boost::property<boost::edge_weight_t, double> EdgeProperty;

   // Adjacency graph using list data structure
   typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty, EdgeProperty> Graph;
   
   // Accessors
   typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
   typedef boost::graph_traits<Graph>::edge_descriptor Edge;
   typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
   typedef boost::graph_traits<Graph>::out_edge_iterator EdgeIterator;
   
   // Properties
   typedef boost::property_map<Graph, boost::vertex_name_t>::type VertexName;
   typedef boost::property_map<Graph, boost::vertex_index1_t>::type VertexLength;
   typedef boost::property_map<Graph, boost::edge_weight_t>::type EdgeWeight;
#else
   typedef int32_t Graph;
#endif //AMOS_HAVE_BOOST
   
   // Other usefull constants
   static const int32_t UNINITIALIZED = -99999999;
   static const int32_t INVALID_EDGE  = -99999998;

   static const AMOS::Status_t MOTIF_SCAFFOLD = 'M';
   static const AMOS::Status_t LINEAR_SCAFFOLD = 'L';
 
   // define constants for orientation of contigs
   enum contigOrientation {FWD, REV, NONE};
   enum edgeStatus {NULL_STATUS, BAD_THRESH, BAD_SKIP, BAD_RPT, BAD_ORI, BAD_SCF, BAD_DST, BAD_TRNS, GOOD_EDGE};
   enum validateNeighborType {ALL, INCOMING, OUTGOING};

   extern HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *cte2weight;

   struct EdgeWeightCmp
   {
      bool operator () (const AMOS::ID_t & a, const AMOS::ID_t & b) const
      {
         assert(cte2weight);
         int32_t weightA = (*cte2weight)[a];
         assert(weightA);
         int32_t weightB = (*cte2weight)[b];
         assert(weightB);

         if (weightA < weightB) {
            return false;
         }
         else {
            return true;
         }
      }
   };

   struct EdgePairCmp
   {
      bool operator() (const std::pair<AMOS::ID_t, uint32_t> & a, const std::pair<AMOS::ID_t, uint32_t> &b) const
      {
         return a.second < b.second;
      }
   };
  
   double computeArrivalRate(const std::vector<AMOS::Contig_t *> &contigs);
   void buildGraph(
            Graph &g, 
            AMOS::BankStream_t &contig_stream, AMOS::BankStream_t &edge_stream, 
            AMOS::IBankable_t *node, AMOS::IBankable_t *edge,
            int32_t redundancy);

   // get edge statuses
   // note: edge status are cached in memory so they must be updated through setEdgeStatus
   void setEdgeStatus(AMOS::ContigEdge_t &cte, AMOS::Bank_t &edge_bank, int status);
   void setEdgeStatus(AMOS::ContigEdge_t &cte, AMOS::Bank_t &edge_bank, int status, bool now);
   void flushEdgeStatus(AMOS::Bank_t &edge_bank);

   bool isBadEdge(AMOS::ID_t cteID, AMOS::Bank_t &edge_bank);
   bool isBadEdge(const AMOS::ContigEdge_t &cte);
   void checkEdgeID(const AMOS::ID_t &id);
   void checkEdge(const AMOS::ContigEdge_t &cte);
   void resetEdges(AMOS::Bank_t &edge_bank, edgeStatus toChange);
   void resetEdges(AMOS::Bank_t &edge_bank, std::set<AMOS::ID_t> &edges, edgeStatus toChange);
   
   AMOS::ID_t getEdgeDestination(const AMOS::ID_t &edgeSrc, const AMOS::ContigEdge_t &cte);
   contigOrientation getOrientation(contigOrientation &myOrient, const AMOS::ContigEdge_t &cte);

   AMOS::Size_t adjustSizeBasedOnAdjacency(AMOS::LinkAdjacency_t edgeAdjacency, AMOS::Size_t gapSize, 
                     AMOS::Size_t firstLength, AMOS::Size_t secondLength,
                     contigOrientation firstOrient, contigOrientation secondOrient,
                     AMOS::SD_t fudgeFactor);

   AMOS::Size_t getTileOverlap(AMOS::Tile_t tileOne, AMOS::Tile_t tileTwo);
   
   void transitiveEdgeRemoval(std::vector<AMOS::Scaffold_t> &scaffs, 
                              AMOS::Bank_t &edge_bank,
                              HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t, EdgeWeightCmp>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& ctg2lnk,
                              int32_t debugLevel);
}
#endif /*UTILITIES_BUNDLER_HH_*/
