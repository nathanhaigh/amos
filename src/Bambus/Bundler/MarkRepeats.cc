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

#ifdef AMOS_HAVE_BOOST
#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>
#endif //AMOS_HAVE_BOOST

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
static const int32_t  MAX_REPEAT_SIZE    =   10000;
static const int32_t  MAX_REPEAT_STDEV   =       3;
static const double   MAX_REPEAT_COV     =       0.02;
static const double   MAX_COVERAGE_ERROR =       0.05;

HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *Bundler::cte2weight = NULL;

struct config {
   string      bank;
   int32_t     debug;
   int32_t     redundancy;
   bool        doAgressiveRepeatFinding;
   bool        noPathRepeats;
   bool        noCoverageRepeats;
   double      maxRepeatCoverage;
};
config globals;
void printHelpText() {
   cerr << 
    "\n"
    "Find repetitive contigs and mark them as repeats\n"
    "\n"
    "USAGE:\n"
    "\n"
    "MarkRepeats -b[ank] <bank_name> [-agressive] [-redundancy] [-noPathRepeats] [-noCoverageRepeats]\n"
    "The -redundancy option specifies the minimum number of links between two contigs before they will be scaffolded\n"    
    "The -aggressive option will identify repeats using only the size and coverage stat (i.e. global coverage) and not the graph structure of the contigs for the coverage repeat algorithm\n"
    "The -noPathRepeats and -noCoverageRepeats will control which repeat algorithms are run. The default is to run both.\n"
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
    {"noPathRepeats",      0, 0, 'r'},
    {"noCoverageRepeats",  0, 0, 'c'},
    {0, 0, 0, 0}
  };

   globals.debug = 0;
   globals.doAgressiveRepeatFinding = false;
   globals.redundancy = 1;
   globals.noPathRepeats = false;
   globals.noCoverageRepeats = false;
   globals.maxRepeatCoverage = MAX_REPEAT_COV; 
 
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
       case 'r':
         globals.noPathRepeats = true;
         break;
       case 'c':
         globals.noCoverageRepeats = true;
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
   hash_map<ID_t, double, hash<ID_t>, equal_to<ID_t> >&numTimesOnPath, 
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
   if (globals.debug > 2) cerr << "FINDING SHORTEST PATHS" << endl;
   
   VertexName vertexNames = get(boost::vertex_name, g);
   EdgeWeight  edgeWeights = get(boost::edge_weight, g);
   
   int32_t      numVertices = boost::num_vertices(g);
   double_t*    distMap     = new double_t[numVertices];
   Vertex*      predMap     = new Vertex[numVertices];
   vector<int32_t> qarray;

   hash_map<ID_t, double, hash<ID_t>, equal_to<ID_t> >numTimesOnPath;
   pair<VertexIterator, VertexIterator> i;
   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      numTimesOnPath[vertexNames[*i.first]] = 0;
   }

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
         double_t min = std::numeric_limits<double>::max();
         Vertex minNode = boost::graph_traits<Graph>::null_vertex();
         vector<int32_t>::iterator index;
   
         for (vector<int32_t>::iterator it = qarray.begin(); it < qarray.end(); it++) {
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

   // finally adjust our path counts by node sizes, we expect larger nodes to have more connections by chance
   VertexLength vertexLength = get(boost::vertex_index1, g);
   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      numTimesOnPath[vertexNames[*i.first]] /= vertexLength[*i.first];
   }
   delete[] distMap;
   delete[] predMap;
   
   if (globals.debug > 2) cerr << "DONE SHORTEST PATHS" << endl;

   double meanOnPath = 0, varianceOnPath = 0, stdevOnPath = 0, N = 0;
   for (hash_map<ID_t, double, hash<ID_t>, equal_to<ID_t> >::iterator i = numTimesOnPath.begin(); i != numTimesOnPath.end(); i++) {
      if (i->first == 0) { continue; }

      N++;
      double delta = (i->second - meanOnPath);
      meanOnPath += (delta/N);
      varianceOnPath += delta * (i->second - meanOnPath);
   }
   varianceOnPath /= N;
   stdevOnPath = sqrt(varianceOnPath);

   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      ID_t iid = vertexNames[*i.first];
      if (iid == 0) continue;

      if (globals.debug > 1) cerr << "CONTIG " << iid << " IS NO PATH=" << numTimesOnPath[iid] << " MEAN IS " << meanOnPath << " STDEV: " << stdevOnPath << " DELTA: " << (numTimesOnPath[iid] - (meanOnPath+(MAX_REPEAT_STDEV*stdevOnPath))) << endl;
      if ((numTimesOnPath[iid] - (meanOnPath+(MAX_REPEAT_STDEV*stdevOnPath))) > 0) {
         // remove the vertices from our assembly and from the graph that is used for connected components
         repeats.insert(iid);
         boost::clear_vertex(*i.first, g);

         if (globals.debug > 1 ) cerr << "CONTIG " << iid << " AND ONPATH=" << numTimesOnPath[iid] << " AND MEAN=" << meanOnPath << " AND STDEV=" << stdevOnPath << " IS A CLASS 3 REPEAT\n";
      }
   }
}

void findConnectedComponentRepeats(Graph &g, Bank_t &contig_bank, set<ID_t> &repeats) {
   if (globals.debug > 2) cerr << "FINDING CONNECTED COMPONENTS PATHS" << endl;
   typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty> UndirectedGraph;
   hash_map<ID_t, vector<Contig_t *>, hash<ID_t>, equal_to<ID_t> > ctgsByComponent;
   vector<Contig_t *> allContigs;
   UndirectedGraph ug;
   boost::copy_graph(g, ug);

   VertexName vertexNames = get(boost::vertex_name, g);
   int32_t* component = new int32_t[boost::num_vertices(ug)];

   // this section is very memory heavy, we load all the contigs from the bank into memory for the ctgsByComponent code
   // if memory becomes an issue, we can instead read contigs from the bank instead
   boost::connected_components(ug, &component[0]);
   pair<VertexIterator, VertexIterator> i;
   Contig_t *ctg;
   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      if (vertexNames[*i.first] == 0) continue;

      ctg = new Contig_t();
      contig_bank.fetch(vertexNames[*i.first], (*ctg));
      allContigs.push_back(ctg);
      ctgsByComponent[component[*i.first]].push_back(ctg);
   }

   if (globals.debug > 2) cerr << "COMPUTING CONNECTED COMPONENTS COVERAGE " << endl;
   double globalArrivalRate = computeArrivalRate(allContigs);
   allContigs.clear();

double globalMean = 0, globalVariance = 0, globalStdev = 0, globalN = 0;
   for (hash_map<ID_t, vector<Contig_t *>, hash<ID_t>, equal_to<ID_t> >::iterator i = ctgsByComponent.begin(); i != ctgsByComponent.end(); i++) {
      double arrivalRate = computeArrivalRate(i->second);
      for (vector<Contig_t *>::iterator j = i->second.begin(); j < i->second.end(); j++) {
         double cov = (*j)->getCovStat(arrivalRate);

         globalN++;
         double globalDelta = cov-globalMean;
         globalMean+=(globalDelta/globalN);
         globalVariance += globalDelta * (cov - globalMean);
      }
   }
   globalVariance /= globalN;
   globalStdev = sqrt(globalVariance);

   for (hash_map<ID_t, vector<Contig_t *>, hash<ID_t>, equal_to<ID_t> >::iterator i = ctgsByComponent.begin(); i != ctgsByComponent.end(); i++) {
      double arrivalRate = computeArrivalRate(i->second);
      double mean = 0, variance = 0, stdev = 0, N = 0;
      for (vector<Contig_t *>::iterator j = i->second.begin(); j < i->second.end(); j++) {
         double cov = (*j)->getCovStat(arrivalRate);
         
         N++;
         double delta = cov - mean;
         mean += (delta/N);
         variance += delta * (cov - mean);
      }
      variance /= N;
      stdev = sqrt(variance);

      if (globals.debug > 2) cerr << "Processing Connected component " << i->first << " with " << i->second.size() << " elements with coverage " << mean << " and stdev " << stdev << " GLOBAL ARRIVAL: " << globalArrivalRate << " LOCAL: " << arrivalRate << endl;
      for (vector<Contig_t *>::iterator j = i->second.begin(); j < i->second.end(); j++) {
         double cov = (*j)->getCovStat(arrivalRate);
         double globalCov = (*j)->getCovStat(globalArrivalRate);

         if (globals.debug > 1) cerr << "CONTIG " << (*j)->getIID() << " HAS COVERAGE " << cov << " GLOBAL COV: " << globalCov << " MEAN: " << mean << " STDEV:" << stdev << " SIZE " << (*j)->getUngappedLength() << " DELTA IS " << (cov + (mean - MAX_REPEAT_STDEV*stdev)) << endl;
         if ((cov < MAX_REPEAT_COV) && (*j)->getUngappedLength() < MAX_REPEAT_SIZE) {
            if (globals.debug > 1) cerr << "CONTIG " << (*j)->getEID() << " OF SIZE " << (*j)->getUngappedLength() << " WITH " << (*j)->getReadTiling().size() << " READS AND COVERAGE " << cov << " IS TOO LOW" << endl;
            repeats.insert((*j)->getIID());
         } else if (i->second.size() == 1 && globalCov < MAX_REPEAT_COV) {
             if (globals.debug > 1) cerr << "CONTIG " << (*j)->getEID() << " OF SIZE " << (*j)->getUngappedLength() << " GLOBAL COVERAGE " << globalCov << " IS TOO LOW" << endl;
            repeats.insert((*j)->getIID());
         } else if (globals.doAgressiveRepeatFinding == true && globalCov <= MAX_REPEAT_COV && (*j)->getUngappedLength() < MAX_REPEAT_SIZE) {
             if (globals.debug > 1) cerr << "AGRESSIVE REPEAT" << cov << " IS TOO LOW" << endl;
            repeats.insert((*j)->getIID());
         }
         // free memory the contig was using as soon as we're done with it
         delete (*j);
      }
   }
   
   delete[] component;
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
   if (globals.noPathRepeats == false) {
      findShortestPathRepeats(g, contig_bank, repeats);
   }
   if (globals.noCoverageRepeats == false) {
      findConnectedComponentRepeats(g, contig_bank, repeats);
   }
#endif //AMOS_HAVE_BOOST

   for (set<ID_t>::iterator i = repeats.begin(); i != repeats.end(); i++) {
      cout << *i << endl;
   }
   contig_bank.close();

   return 0;
}
