#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>
#include <stdlib.h>
#include <math.h>
#include <limits>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <iterator>
#include <iostream>

#include <sys/time.h>

#ifdef AMOS_HAVE_OPENMP
#include <omp.h>
#endif

#ifdef AMOS_HAVE_BOOST
#include <boost/version.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>

#include <boost/random.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/betweenness_centrality.hpp>

#if BOOST_VERSION / 100000 <= 1 && BOOST_VERSION / 100 % 1000 < 48
#include <boost/interprocess/detail/atomic.hpp>
#define BOOST_ATOMIC_ADD boost::interprocess::detail::atomic_add32
#define BOOST_ATOMIC_CAS boost::interprocess::detail::atomic_cas32
#else
#include <boost/interprocess/detail/atomic.hpp>
#define BOOST_ATOMIC_ADD boost::interprocess::ipcdetail::atomic_add32
#define BOOST_ATOMIC_CAS boost::interprocess::ipcdetail::atomic_cas32
#endif

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
static const uint32_t MIN_PARALLEL_SIZE  = 500000000;
static const uint32_t PARALLEL_SCALING   = 100;
static const int32_t  MAX_REPEAT_SIZE    =   10000;
static const int32_t  MAX_REPEAT_STDEV   =       3;
static const double   MAX_REPEAT_COV     =       0.02;
static const double   MAX_COVERAGE_ERROR =       0.05;
static const int32_t  MAX_K              =       4;

#ifdef AMOS_HAVE_BOOST
static const uint32_t MAX_VALUE     = numeric_limits<uint32_t>::max();
#endif

HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *Bundler::cte2weight = NULL;

struct config {
   string      bank;
   int32_t     debug;
   int32_t     redundancy;
   bool        doAgressiveRepeatFinding;
   bool        noPathRepeats;
   bool        noCoverageRepeats;
   double      maxRepeatCoverage;
   bool        weighted;
   bool        test;
   int32_t     numThreads;
   int32_t     kPath;
   bool        runOriginal;
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
    {"weightedPath",       0, 0, 'w'},
    {"test",               0, 0, 'T'},
    {"threads",            1, 0, 't'},
    {"kPath",              1, 0, 'k'},
    {"original",        0, 0, 'o'},
    {0, 0, 0, 0}
  };

   globals.debug = 0;
   globals.doAgressiveRepeatFinding = false;
   globals.redundancy = 1;
   globals.noPathRepeats = false;
   globals.noCoverageRepeats = false;
   globals.maxRepeatCoverage = MAX_REPEAT_COV; 
   globals.test = false;
   globals.weighted = false;
   globals.numThreads = 1;
   globals.kPath = 0;
   globals.runOriginal = false;
 
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
       case 'w':
         globals.weighted = true;
         break;
       case 'T':
         globals.test = true;
         break;
      case 't':
         globals.numThreads = atoi(optarg);
         if (globals.numThreads <= 0) globals.numThreads = 1;
#ifdef AMOS_HAVE_OPENMP
         if (globals.numThreads > omp_get_max_threads()) { globals.numThreads = omp_get_max_threads(); }
#endif
         if (globals.debug > 1) { cerr << "The threads is set to " << globals.numThreads << endl; }
         break;
      case 'k':
         globals.kPath = atoi(optarg);
         if (globals.kPath < 0) globals.kPath = 0;
         if (globals.kPath > MAX_K) globals.kPath = MAX_K;
         break;
      case 'o':
         globals.runOriginal = true;
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

double inline W (uint32_t k, uint32_t d, uint32_t w, uint32_t** sigma) {
   if (k == 0) {
      return pow(sigma[0][w], d);
   } else {
      uint32_t sum = 0;
      uint32_t i = 0;
      for (i = 1; i <=k; i++) {
         sum = sum - sigma[i][w] * W(k - i, d - 1, w, sigma);
      }
      return sum;
   }
}

#ifdef AMOS_HAVE_BOOST
double* findShortestPathRepeatsParallel(Graph &g, int K) {
  if (K > MAX_K) {
     cerr << "Error: Maximum allowed k-max path is " << MAX_K << endl;
     K = MAX_K;
  }
  K++;

  int32_t      numVertices = boost::num_vertices(g);
  int32_t      numEdges = boost::num_edges(g);
  double *       B     = new double[numVertices];
  double **      delta = new double*[K];
  uint32_t **    sigma = new uint32_t*[K];
  uint32_t *     sigmaSums = new uint32_t[numVertices];
  uint32_t *     dist  = new uint32_t[numVertices];
  uint32_t **    S     = new uint32_t*[numVertices];
  uint32_t *     S_count = new uint32_t[numVertices];
  uint32_t **    child_count = new uint32_t*[K];
  uint32_t ***   child   = new uint32_t**[K];

  for (int k = 0; k < K; k++) {
     child_count[k] = new uint32_t[numVertices];
     child[k] = new uint32_t*[numVertices];
     sigma[k] = new uint32_t[numVertices];
     delta[k] = new double[numVertices];
  }

  Vertex v,w;
  EdgeIterator out_i, out_end;
  uint32_t sw, distW, deltaW, p;
  int j, d, di, dj;
  double dsw;

  #pragma omp parallel for private(j) schedule(static) collapse(2)
  for (int k = 0; k < K; k++) {
     for (j = 0; j < numVertices; j++) {
        child[k][j] = new uint32_t[numEdges];
        if (k == 0) {
           B[j] = 0;
           S[j] = new uint32_t[numVertices];
        }
     }
  }

  timeval start;
  timeval end;
  gettimeofday(&start, 0);

  for (int s = 0; s < numVertices; s++) {
     #pragma omp parallel for private(j) schedule(static) collapse(2)
     for (int k = 0; k < K; k++) {
        for (int j = 0; j < numVertices; j++) {
           child_count[k][j] = 0;
           sigma[k][j] = delta[k][j] = 0;

           if (k == 0) {
              dist[j] = MAX_VALUE;
              sigmaSums[j] = 0;
              S_count[j] = 0;
           }
        }
     }
     uint32_t phase = 0;
     sigma[0][s] = 1;
     dist[s] = 0;
     S[phase][0] = s;
     S_count[phase] = 1;
     uint32_t count = 1;

     while (count > 0) {
        S_count[phase+1] = count = 0;
        #pragma omp parallel for private (v, w, distW, deltaW, p, out_i, out_end) schedule(dynamic)
        for (int i = 0; i < S_count[phase]; i++) { // parallel
           v = S[phase][i];
           for (tie(out_i, out_end) = boost::out_edges(v, g); out_i != out_end; ++out_i) { // parallel
              w = boost::target(*out_i, g);
              distW = BOOST_ATOMIC_CAS(&dist[w], phase+1, MAX_VALUE);
              deltaW = dist[v] - distW + 1;
              if (distW == MAX_VALUE) {
                 p = BOOST_ATOMIC_ADD(&S_count[phase+1], 1);
                 BOOST_ATOMIC_ADD(&count, 1);
                 assert(p < numVertices);
                 assert(phase < numVertices - 1);
                 S[phase+1][p] = w;
                 distW = phase + 1;
                 deltaW = dist[v] - distW + 1;
              } 
              if (deltaW  < K) {
                 p = BOOST_ATOMIC_ADD(&child_count[deltaW][v], 1);
                 assert(p < numEdges);
                 assert(v < numVertices);
                 assert(deltaW < K);
                 child[deltaW][v][p] = w;
              }
              if (deltaW <= min(K-1, 1)) {
                 BOOST_ATOMIC_ADD(&sigma[deltaW][w], sigma[0][v]);
              }
           }
        } 
        phase = phase + 1;
     }

     for (int k = 1; k < K; k++) {
        for (int i = 0; i < phase; i++) {
           #pragma omp parallel for private(v, w, j, d, dj) schedule(dynamic)
           for (j = 0; j < S_count[i]; j++) { //parallel
              v = S[i][j];
              for (d = 0; d < child_count[0][v]; d++) { //parallel
                 w = child[0][v][d];
                 BOOST_ATOMIC_ADD(&sigma[k][w], sigma[k][v]);
              }
              if (k < (K-1)) {
                 for (dj = 1; dj <= k+1; dj++) { //parallel
                    for (d = 0; d < child_count[dj][v]; d++) { //parallel
                       w = child[dj][v][d];
                       BOOST_ATOMIC_ADD(&sigma[k+1][w], sigma[k+1-dj][v]);
                    }
                 }
              }
           }
        }
     }

     #pragma omp parallel for private(j) schedule(static) collapse(2)
     for (int k = 0; k < K; k++) {
        for (j = 0; j < numVertices; j++) {
           BOOST_ATOMIC_ADD(&sigmaSums[j], sigma[k][j]);
        }
     }

     phase = phase - 1;
     for (int k = 0; k < K; k++) {
        p = phase;
        for ( ; p > 0; p--) {
           #pragma omp parallel for private(v, w, d, j, di, dj) schedule(dynamic)
           for (int i = 0; i < S_count[p]; i++) { // parallel
              v = S[p][i];
              for (d = 0; d <= k; d++) { // parallel
                 for (j = 0; j < child_count[d][v]; j++) { ///parallel
                    w = child[d][v][j];
                    for (di = 0; di <= (k-d); di++) {
                       uint32_t sum = 0;
                       uint32_t e = k - d - di;
                       for (dj = 0; dj <= e; dj++) {
                          sum += W(e - dj, e, w, sigma) * sigma[dj][v];
                       }
                       delta[k][v] += (sigma[0][w] == 0 ? 0 : (sum * (delta[di][w] / pow(sigma[0][w], e+1))));
                    }
                    delta[k][v] += (sigmaSums[w] == 0 ? 0 : (double)sigma[k - d][v] / ((double)sigmaSums[w]));
                 }
              }
              B[v] += delta[k][v];
           }
        }
     }
  }

  gettimeofday(&end, 0);
  double totaltime = (((double)(end.tv_sec-start.tv_sec)) + ((double)(end.tv_usec-start.tv_usec))*1.0e-6);
  cerr << "Elapsed time threaded is " << totaltime << endl;

  delete[] sigmaSums;
  delete[] dist;
  delete[] S_count;
  #pragma opm parallel for private(j) schedule(static) collapse(2)
  for (int k = 0; k < K; k++) {
     for (j = 0; j < numVertices; j++) {
        if (k == 0) delete[] S[j];
        delete[] child[k][j];
     }
     delete[] child[k];
     delete[] sigma[k];
     delete[] delta[k];
     delete[] child_count[k];
  }
  delete[] S;
  delete[] child;
  delete[] child_count;
  delete[] sigma;
  delete[] delta;

  return B;
}

double* findShortestPathRepeatsParallel(Graph &g) {
  return findShortestPathRepeatsParallel(g, 0);
}

void visitAllPaths(
   double *numTimesOnPath,
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

void testShortestPaths() {
  typedef boost::rand48 RandomGenerator;
  RandomGenerator gen;

  size_t N = 100;
  size_t M = 100;
  double p = 0.05;
  double* B = NULL;
  Graph g;

  timeval start;
  timeval end;

  for (int i = 0; i < 100; i++) {
    if (i % 2 == 0) {
       g = Graph(boost::erdos_renyi_iterator<RandomGenerator, Graph>(gen, N, p), boost::erdos_renyi_iterator<RandomGenerator, Graph>(), 100);
       gettimeofday(&start, 0);
       B = findShortestPathRepeatsParallel(g, 0);
    }
    else {
       g = Graph(boost::small_world_iterator<RandomGenerator, Graph>(gen, N, M, p), boost::small_world_iterator<RandomGenerator, Graph>(), 100);
       gettimeofday(&start, 0);
       B = findShortestPathRepeatsParallel(g, 0);
    }
    gettimeofday(&end, 0);
    double totaltime = (((double)(end.tv_sec-start.tv_sec)) + ((double)(end.tv_usec-start.tv_usec))*1.0e-6);
    cerr << "Parallel program done in " << totaltime << endl;

    int32_t      numVertices = boost::num_vertices(g);
    std::vector<double> centrality(numVertices);
    // compute centrality using known algorithm
    gettimeofday(&start, 0);
    brandes_betweenness_centrality(g,
      boost::centrality_map(boost::make_iterator_property_map(centrality.begin(), get(boost::vertex_index, g), double())));
    gettimeofday(&end, 0);
    totaltime = (((double)(end.tv_sec-start.tv_sec)) + ((double)(end.tv_usec-start.tv_usec))*1.0e-6);
    cerr << "Elapsed time linear is " << totaltime << endl;

    for (int i = 0; i < numVertices; i++) {
       if (abs(centrality[i] - B[i]) > 0.001) {
          cerr << "Error: incorrectly computed centrality for node " << i << " should be: " << centrality[i] << " and instead it is: " << B[i] << endl;
          assert(0);
       }
    }
    delete[] B;
  }
}

double* computeShortestPaths(Graph &g) {
   VertexName vertexNames = get(boost::vertex_name, g);
   EdgeWeight  edgeWeights = get(boost::edge_weight, g);
   
   int32_t      numVertices = boost::num_vertices(g);
   double_t*    distMap     = new double_t[numVertices];
   Vertex*      predMap     = new Vertex[numVertices];
   vector<int32_t> qarray;

   double *numTimesOnPath = new double[numVertices];
   pair<VertexIterator, VertexIterator> i;
   for (i = boost::vertices(g); i.first != i.second; ++i.first) {
      numTimesOnPath[vertexNames[*i.first]] = 0;
   }

  timeval start;
  timeval end;
  gettimeofday(&start, 0);

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
   gettimeofday(&end, 0);
   double totaltime = (((double)(end.tv_sec-start.tv_sec)) + ((double)(end.tv_usec-start.tv_usec))*1.0e-6);
   cerr << "Elapsed time is " << totaltime << endl;

   delete[] distMap;
   delete[] predMap;

   return numTimesOnPath;
}

// compute shortest paths using one of the specified methods
void findShortestPathRepeats(Graph &g, Bank_t &contig_bank, set<ID_t> &repeats) {
   // find the all-pairs shortest paths
   timeval start;
   timeval end;
   if (globals.debug > 2) cerr << "FINDING SHORTEST PATHS" << endl;
   gettimeofday(&start, 0);

   double* numTimesOnPath;
   int32_t      numVertices = boost::num_vertices(g);
   int32_t      numEdges = boost::num_edges(g);

   if (globals.debug > 3) {
      double minNeighbors = numVertices;
      double maxNeighbors = 0;
      double meanOnPath = 0, varianceOnPath = 0, stdevOnPath = 0, N = 0;
      for(pair<VertexIterator, VertexIterator> i = boost::vertices(g); i.first != i.second; ++i.first) {
         uint32_t out = boost::out_degree(*i.first, g);
         if (out < minNeighbors) { minNeighbors = out; }
         if (out > maxNeighbors) { maxNeighbors = out; } 
         N++;
         double delta = (out - meanOnPath);
         meanOnPath += (delta/N);
         varianceOnPath += delta * (out - meanOnPath);
      }
      varianceOnPath /= N;
      stdevOnPath = sqrt(varianceOnPath);
      cerr << "The graph with " << numVertices << " nodes and " << numEdges << " edges has min " << minNeighbors << " max neighbors " << maxNeighbors << " mean " << meanOnPath << "+-" << stdevOnPath << endl;
   }

   if (globals.runOriginal) {
      if (globals.debug > 1) { cerr << "Using linear repeat detection" << endl; }
      numTimesOnPath = computeShortestPaths(g);
   } else {
      if ((numVertices * numEdges) <= MIN_PARALLEL_SIZE) {
         if (globals.debug > 1) { cerr << "Using 1 thread" << endl; }
         globals.numThreads = 1;
#ifdef AMOS_HAVE_OPENMP
         omp_set_num_threads(globals.numThreads);
#endif
      }
      else {
         // compute in parallel
         if (globals.debug > 1) { cerr << "Using parallel repeat detection" << endl; }
      }
      numTimesOnPath = findShortestPathRepeatsParallel(g, globals.kPath);
   }

   // finally adjust our path counts by node sizes, we expect larger nodes to have more connections by chance
   VertexLength vertexLength = get(boost::vertex_index1, g);
   VertexName vertexNames = get(boost::vertex_name, g);
   pair<VertexIterator, VertexIterator> i;
   for(i = boost::vertices(g); i.first != i.second; ++i.first) {
      numTimesOnPath[vertexNames[*i.first]] /= vertexLength[*i.first];
   }

   gettimeofday(&end, 0);
   double totaltime = (((double)(end.tv_sec-start.tv_sec)) + ((double)(end.tv_usec-start.tv_usec))*1.0e-6);
   if (globals.debug > 2) cerr << "DONE SHORTEST PATHS in " << totaltime << " seconds" << endl;

   double meanOnPath = 0, varianceOnPath = 0, stdevOnPath = 0, N = 0;
   for (int j = 1; j < numVertices; j++) {
      N++;
      double delta = (numTimesOnPath[j] - meanOnPath);
      meanOnPath += (delta/N);
      varianceOnPath += delta * (numTimesOnPath[j] - meanOnPath);
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

   delete[] numTimesOnPath;
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

cerr << "The number of components is " << ctgsByComponent.size() << endl;

   globals.numThreads = (globals.numThreads < (int)ctgsByComponent.size() / PARALLEL_SCALING ? globals.numThreads : (int)ctgsByComponent.size() / PARALLEL_SCALING);
   if (globals.numThreads < 1) { globals.numThreads = 1; }
#ifdef AMOS_HAVE_OPENMP
   omp_set_num_threads(globals.numThreads);
#endif

   #pragma opm parallel for private(j, arrivalRate, mean, variance, stdev, N) schedule(dynamic)
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
            #pragma omp critical
            {
               repeats.insert((*j)->getIID());
            }
         } else if (i->second.size() == 1 && globalCov < MAX_REPEAT_COV) {
             if (globals.debug > 1) cerr << "CONTIG " << (*j)->getEID() << " OF SIZE " << (*j)->getUngappedLength() << " GLOBAL COVERAGE " << globalCov << " IS TOO LOW" << endl;
            #pragma omp critical
            {
               repeats.insert((*j)->getIID());
            }
         } else if (globals.doAgressiveRepeatFinding == true && globalCov <= MAX_REPEAT_COV && (*j)->getUngappedLength() < MAX_REPEAT_SIZE) {
             if (globals.debug > 1) cerr << "AGRESSIVE REPEAT" << cov << " IS TOO LOW" << endl;
            #pragma omp critical
            {
               repeats.insert((*j)->getIID());
            }
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

#ifdef AMOS_HAVE_OPENMP
   omp_set_dynamic(false);
   omp_set_nested(true);
   omp_set_num_threads(globals.numThreads);
#endif

   if (globals.test == true) {
#ifdef AMOS_HAVE_BOOST
#ifdef AMOS_HAVE_OPENMP
      testShortestPaths();
#endif
#endif
      return 0;
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
   buildGraph(g, contig_stream, edge_stream, &ctg, &cte, globals.redundancy, globals.weighted);
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
