#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>
#include <math.h>
#include <limits>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <iterator>
#include <iostream>

#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"

#include "Utilities_Bundler.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;
using namespace Bundler;

// constants for repeat resolution
static const int32_t  MAX_REPEAT_SIZE   =  1000;
static const int32_t  MAX_REPEAT_STDEV  =  4;
static const int32_t  MAX_REPEAT_COV    =  5;

struct config {
   string      bank;
   int32_t     debug;
   int32_t     redundancy;
   bool        doAgressiveRepeatFinding;
};
config globals;
void printHelpText() {
   cerr << 
    "\n"
    "USAGE:\n"
    "\n"
    "MarkRepeats -b[ank] <bank_name> [-agressive] [-redundancy]\n"
    "The -redundancy option specifies the minimum number of links between two contigs before they will be scaffolded\n"    
    "The -aggressive option will agressive identify repeats using only the size and coverage stat and not the graph structure of the contigs\n"
       << endl;
}

bool GetOptions(int argc, char ** argv) {
   int option_index = 0;      
   static struct option long_options[] = {
    {"help",               0, 0, 'h'},
    {"h",                  0, 0, 'h'},
    {"b",                  1, 0, 'b'},
    {"bank",               1, 0, 'b'},
    {"agressive",          0, 0, 'R'},
    {"redundancy",         1, 0, 'p'},
    {"debug",              1, 0, 'd'},
    {0, 0, 0, 0}
  };

   globals.debug = 1;
   globals.doAgressiveRepeatFinding = false;
   globals.redundancy = 1;
  
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
         globals.redundancy = atoi(optarg);
         break;
       case 'R':
         globals.doAgressiveRepeatFinding = true;
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
      cerr << "AgressiveRepats = \t" << globals.doAgressiveRepeatFinding << endl;
   }
   
   return true;
} // GetOptions

#ifdef AMOS_HAVE_BOOST
void visitAllPaths(
   hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >&numTimesOnPath, 
   Graph const&g, 
   VertexName &vertexNames,
   const Vertex* predMap,
   Vertex const i,
   Vertex const j) {

   if (i==j) {
      return;
   } else if (predMap[j] == boost::graph_traits<Graph>::null_vertex()) {
      return;
   } else {
      visitAllPaths(numTimesOnPath, g, vertexNames, predMap, i, predMap[j]);
      numTimesOnPath[vertexNames[j]]++;
   }
}

void findShortestPathRepeats(Graph &g, Bank_t &contig_bank, set<ID_t> &repeats) {
   // find the all-pairs shortest paths
   cerr << "FINDING SHORTEST PATHS" << endl;
   
   VertexName vertexNames = get(boost::vertex_name, g);
   EdgeWeight  edgeWeights = get(boost::edge_weight, g);
   
   int32_t      numVertices = boost::num_vertices(g);
   int32_t*     distMap     = new int32_t[numVertices];
   Vertex*      predMap     = new Vertex[numVertices];
   vector<int32_t> qarray;

   hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >numTimesOnPath;

   pair<VertexIterator, VertexIterator> i;
   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      Vertex source = *i.first;
      // initialize arrays     
      qarray.clear();      
      pair<VertexIterator, VertexIterator> j;
      for (j = boost::vertices(g); j.first != j.second; ++j.first) {
         Vertex vertex = *j.first;
         
         qarray.push_back(vertex);
         distMap[vertex] = std::numeric_limits<int>::max();
         predMap[vertex] = boost::graph_traits<Graph>::null_vertex();
      }
         
      // initialize distance to the source to be 0
      distMap[source] = 0;
      while (qarray.size() != 0) {
         int32_t min = std::numeric_limits<int>::max();
         Vertex minNode = boost::graph_traits<Graph>::null_vertex();
         vector<int32_t>::iterator index;
   
         for (vector<int32_t>::iterator it = qarray.begin(); it != qarray.end(); it++) {
            // for all nodes we haven't processed yet, check if they have shortest next distance
            if (distMap[*it] < min) {
               min = distMap[*it];
               minNode = *it;
               index = it;
            }
         }
         
         if (minNode == boost::graph_traits<Graph>::null_vertex() || min == std::numeric_limits<int32_t>::max()) {
            break;
         }
   
         qarray.erase(index);
         EdgeIterator out_i, out_end;
         for (tie(out_i, out_end) = boost::out_edges(minNode, g); out_i != out_end; ++out_i) {
            Vertex t = boost::target(*out_i, g);
               if (distMap[minNode] + edgeWeights[*out_i] < distMap[t]) {
                  distMap[t] = distMap[minNode] + edgeWeights[*out_i];
                  predMap[t] = minNode;
               }
         }
      }     
      for (j = boost::vertices(g); j.first != j.second; ++j.first) {
         visitAllPaths(numTimesOnPath, g, vertexNames, predMap, source, *j.first);
      }
   }

   delete[] distMap;
   delete[] predMap;
   
   cerr << "DONE SHORTEST PATHS" << endl;

   double meanOnPath = 0, varianceOnPath = 0, stdevOnPath = 0, N = 0;
   for (hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >::iterator i = numTimesOnPath.begin(); i != numTimesOnPath.end(); i++) {
      N++;
      double delta = i->second - meanOnPath;
      meanOnPath += (delta/N);
      varianceOnPath += delta * (i->second - meanOnPath);
   }
   varianceOnPath /= N;
   stdevOnPath = sqrt(varianceOnPath);

   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      ID_t iid = vertexNames[*i.first];
      
      if (numTimesOnPath[iid] > meanOnPath+(MAX_REPEAT_STDEV*stdevOnPath)) {
         // remove the vertices from our assembly and from the graph that is used for connected components
         repeats.insert(iid);
         boost::clear_vertex(*i.first, g);
         boost::remove_vertex(*i.first, g);
         
         cerr << "CONTIG " << iid << " AND ONPATH=" << numTimesOnPath[iid] << " AND MEAN=" << meanOnPath << " AND STDEV=" << stdevOnPath << " IS A CLASS 3 REPEAT\n";
      }
   }
}

void findConnectedComponentRepeats(Graph &g, Bank_t &contig_bank, set<ID_t> &repeats) {
   cerr << "FINDING CONNECTED COMPONENTS PATHS" << endl;
   hash_map<ID_t, vector<Contig_t>, hash<ID_t>, equal_to<ID_t> > ctgsByComponent;

   VertexName vertexNames = get(boost::vertex_name, g);
   int32_t* component = new int32_t[boost::num_vertices(g)];

   boost::strong_components(g, component); 
   pair<VertexIterator, VertexIterator> i;
   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      Contig_t ctg;
      contig_bank.fetch(vertexNames[*i.first], ctg);
      //ctgsByComponent[component[*i.first]].push_back(ctg);
   }

   cerr << "COMPUTING CONNECTED COMPONENTS COVERAGE" << endl;
   for (hash_map<ID_t, vector<Contig_t>, hash<ID_t>, equal_to<ID_t> >::iterator i = ctgsByComponent.begin(); i != ctgsByComponent.end(); i++) {
      double arrivalRate = computeArrivalRate(i->second);
      double mean = 0, variance = 0, stdev = 0, N = 0;
      for (vector<Contig_t>::iterator j = i->second.begin(); j < i->second.end(); j++) {
         double cov = j->getCovStat(arrivalRate);
         
         N++;
         double delta = cov - mean;
         mean += (delta/N);
         variance += delta * (cov - mean);
      }
      variance /= N;
      stdev = sqrt(variance);
      
      for (vector<Contig_t>::iterator j = i->second.begin(); j < i->second.end(); j++) {
         double cov = j->getCovStat(arrivalRate);

         if (cov < mean-(2*MAX_REPEAT_STDEV*stdev) && cov <= MAX_REPEAT_COV) {
            repeats.insert(j->getIID());
         } else if (globals.doAgressiveRepeatFinding == true && i->second.size() == 1 && cov <= MAX_REPEAT_COV && j->getLength() < MAX_REPEAT_SIZE) {
            repeats.insert(j->getIID());
         }
      }
   }
}
#endif //AMOS_HAVE_BOOST

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

   BankStream_t edge_stream (ContigEdge_t::NCODE);
   if (! edge_stream.exists(globals.bank)){
      cerr << "No edge account found in bank " << globals.bank << endl;
      exit(1);
   }
   try {
      edge_stream.open(globals.bank, B_READ);
   } catch (Exception_t & e) {
      cerr << "Failed to open edge account in bank " << globals.bank << ": " << endl << e << endl;
      edge_stream.close();
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
  
   Graph g;
   Contig_t ctg;
   ContigEdge_t cte;
   buildGraph(g, contig_stream, edge_stream, &ctg, &cte, globals.redundancy);
   edge_stream.close();
   contig_stream.close();

   set<ID_t> repeats;
#ifdef AMOS_HAVE_BOOST
   findShortestPathRepeats(g, contig_bank, repeats);
   findConnectedComponentRepeats(g, contig_bank, repeats);
#endif //AMOS_HAVE_BOOST

   for (set<ID_t>::iterator i = repeats.begin(); i != repeats.end(); i++) {
      cout << *i << endl;
   }
   contig_bank.close();

   return 0;
}
