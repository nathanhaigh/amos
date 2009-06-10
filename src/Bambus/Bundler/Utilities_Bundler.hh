#ifndef UTILITIES_BUNDLER_HH_
#define UTILITIES_BUNDLER_HH_

#include <queue>
#include <iterator>

#ifdef AMOS_HAVE_BOOST
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graphviz.hpp>
#endif //AMOS_HAVE_BOOST

#include "datatypes_AMOS.hh"
#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"

#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

namespace Bundler
{
#ifdef AMOS_HAVE_BOOST
   /* give access to common Boost data types through our interface */
   // Vertex properties
   typedef boost::property<boost::vertex_name_t, unsigned int> VertexProperty;

   // Edge properties
   typedef boost::property<boost::edge_weight_t, unsigned int> EdgeProperty;

   // Adjacency graph using list data structure
   typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty, EdgeProperty> Graph;
   
   // Accessors
   typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
   typedef boost::graph_traits<Graph>::edge_descriptor Edge;
   typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
   typedef boost::graph_traits<Graph>::out_edge_iterator EdgeIterator;
   
   // Properties
   typedef boost::property_map<Graph, boost::vertex_name_t>::type VertexName;
   typedef boost::property_map<Graph, boost::edge_weight_t>::type EdgeWeight;
#else
   typedef int32_t Graph;
#endif //AMOS_HAVE_BOOST
   
   // Other usefull constants
   static const int32_t UNINITIALIZED = -99999999;
   static const int32_t INVALID_EDGE  = -99999998;

   
   // define constants for orientation of contigs
   enum contigOrientation {FWD, REV, NONE};
   enum edgeStatus {NULL_STATUS, BAD_LOW, BAD_RPT, BAD_ORI, BAD_DST, GOOD_EDGE};
   enum validateNeighborType {ALL, INCOMING, OUTGOING};

   enum outputType{AGP, DOT, BAMBUS};
   
   double computeArrivalRate(const std::vector<AMOS::Contig_t> &contigs);
   void buildGraph(
            Graph &g, 
            AMOS::BankStream_t &contig_stream, AMOS::BankStream_t &edge_stream, 
            AMOS::IBankable_t *node, AMOS::IBankable_t *edge,
            int32_t redundancy);
   
   bool isBadEdge(const AMOS::ContigEdge_t &cte);
   AMOS::ID_t getEdgeDestination(const AMOS::ID_t &edgeSrc, const AMOS::ContigEdge_t &cte);
   contigOrientation getOrientation(const AMOS::ID_t &contig, contigOrientation &myOrient, const AMOS::ContigEdge_t &cte);
      
   void outputResults(
            const std::string &bank,
            AMOS::Bank_t &contig_bank, 
            AMOS::Bank_t &edge_bank, 
            std::vector<AMOS::Scaffold_t> &scaffs,
            HASHMAP::hash_map<AMOS::ID_t, contigOrientation, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >& ctg2ort,
            outputType type, 
            const std::string &outputPrefix,
            int32_t debug);
}

#endif /*UTILITIES_BUNDLER_HH_*/
