#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>

#include <set>
#include <vector>

#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"
#include "Utilities_Bundler.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;
using namespace Bundler;

struct config {
   string      bank;
   int32_t     version;
   int32_t     debug;
};

HASHMAP::hash_map<AMOS::ID_t, int32_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > *Bundler::cte2weight = NULL;

void linearizeScaffolds(std::vector<Scaffold_t> &scaffs, 
                        AMOS::Bank_t &edge_bank,
                        HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t, EdgeWeightCmp>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > &ctg2lnk,
                        int32_t debugLevel) {
   std::vector<Scaffold_t> linearScaffolds;
   AMOS::ID_t scfIID = 1;

   for(std::vector<AMOS::Scaffold_t>::iterator itScf = scaffs.begin(); itScf < scaffs.end(); ++itScf) {
      HASHMAP::hash_map<AMOS::ID_t, std::set<AMOS::ID_t>*, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > ctg2conflict;
      HASHMAP::hash_map<AMOS::ID_t, int, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > visited;               // contigs we've processed
      AMOS::ContigEdge_t cte;

      for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); ++tileIt) {
         visited[tileIt->source] = 0;

         std::set<AMOS::ID_t>* s = ctg2conflict[tileIt->source];
         if (s == NULL) { s = new std::set<AMOS::ID_t>(); ctg2conflict[tileIt->source] = s;}
cerr << "Initialize conflicts for node " << tileIt->source << endl;

         for (std::vector<AMOS::Tile_t>::const_iterator tilePrev = itScf->getContigTiling().begin(); tilePrev < tileIt; ++tilePrev) {
            if (tilePrev->offset + (tilePrev->range.getLength() - 1) > tileIt->offset) {
               // it conflicts me
               std::set<AMOS::ID_t>* s = ctg2conflict[tileIt->source];
               s->insert(tilePrev->source);
               ctg2conflict[tileIt->source] = s;
            }
         }
         for (std::vector<AMOS::Tile_t>::const_iterator tileNext = tileIt+1; tileNext < itScf->getContigTiling().end() && (tileNext->offset < tileIt->offset + (tileIt->range.getLength()-1)); ++tileNext) {
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
         for (std::vector<AMOS::Tile_t>::const_iterator tileIt = itScf->getContigTiling().begin(); tileIt < itScf->getContigTiling().end(); ++tileIt) {
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
                  for (std::set<AMOS::ID_t, EdgeWeightCmp>::iterator i = s->begin(); i != s->end(); ++i) {
                     checkEdgeID(*i);
                     edge_bank.fetch(*i, cte);
                     checkEdge(cte);

                     if (isBadEdge(cte)) { continue; }
                     AMOS::ID_t neighbor = getEdgeDestination(curr, cte);
                     if (visited[neighbor] == 1 || inCurrScf[neighbor] == 1 || ctg2conflict[neighbor] == NULL) { continue; }
                     std::set<AMOS::ID_t> *conflicts = ctg2conflict[neighbor];
                     bool hasConflicts = false;

                     for (std::set<AMOS::ID_t>::iterator j = conflicts->begin(); j != conflicts->end(); ++j) {
                        if (inCurrScf[*j] == 1) {
                          hasConflicts = true;
                          break;
                        } 
                     }

                     if (hasConflicts) {
                        setEdgeStatus(cte, edge_bank, BAD_RPT, false);
                     } else {
                        current_nodes.push(neighbor);
                        inCurrScf[neighbor] = 1;
                     }
                  } // neighbors loop
               } // current connected set of nodes

               // loop through all the contigs and edges to see if they should be assigned to the current scaffold
               for (std::vector<AMOS::Tile_t>::const_iterator scfTiles = itScf->getContigTiling().begin(); scfTiles < itScf->getContigTiling().end(); ++scfTiles) {
                  if (inCurrScf[scfTiles->source] == 1) { tiles.push_back(*scfTiles); }
               }
               for (std::vector<AMOS::ID_t>::const_iterator edgeIt = itScf->getContigEdges().begin(); edgeIt < itScf->getContigEdges().end(); ++edgeIt) {
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

config globals;
void printHelpText() {
   cerr <<
    "\n"
    "Output text results for Bambus\n"
    "\n"
    "USAGE:\n"
    "\n"
    "OutputText -b[ank] <bank_name> [-version n]\n"
       << endl;
}

bool GetOptions(int argc, char ** argv) {
   int option_index = 0;
   static struct option long_options[] = {
    {"help",               0, 0, 'h'},
    {"h",                  0, 0, 'h'},
    {"b",                  1, 0, 'b'},
    {"bank",               1, 0, 'b'},
    {"version",            1, 0, 'v'},
    {0, 0, 0, 0}
  };

   globals.version = Bank_t::OPEN_LATEST_VERSION;

   int c;
   while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1){
      switch (c){
      case 'h':
         printHelpText();
         break;
      case 'b':
         globals.bank = string(optarg);
         break;
      case 'v':
         globals.version = atoi(optarg);
         break;
      case '?':
         return false;
      }
   }

   // print summary
   if (globals.debug >= 1) {
      cerr << "Options summary:" << endl;
      cerr << "Bank = \t" << globals.bank << endl;
   }

   return true;
} // GetOptions

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
	   edge_bank.open(globals.bank, B_READ);
   } catch (Exception_t & e) {
      cerr << "Failed to open edge account in bank " << globals.bank << ": " << endl << e << endl;
      edge_bank.close();
      exit(1);
   }

   Bank_t contig_bank (Contig_t::NCODE);
   if (! contig_bank.exists(globals.bank)){
     cerr << "No contig account found in bank " << globals.bank << endl;
     exit(1);
   }
   try {
     contig_bank.open(globals.bank, B_READ);
   } catch (Exception_t & e) {
       cerr << "Failed to open contig account in bank " << globals.bank << ": " << endl << e << endl;
       contig_bank.close();
       exit(1);
   }

   Bank_t motif_bank (Motif_t::NCODE);
   if (! motif_bank.exists(globals.bank)) {
	   cerr << "No motif account found in bank " << globals.bank << endl;
	   exit(1);
   }
   try {
	   motif_bank.open(globals.bank, B_READ | B_WRITE, globals.version, false);
   } catch (Exception_t & e) {
	   cerr << "Failed to open motif account in bank " << globals.bank << " : " << endl << e << endl;
	   motif_bank.close();
	   exit(1);
   }

   BankStream_t scf_stream (Scaffold_t::NCODE);
   if (! scf_stream.exists(globals.bank)) {
           cerr << "No scaffold account found in bank " << globals.bank << endl;
           exit(1);
   }
   try {
           scf_stream.open(globals.bank, B_READ | B_WRITE, globals.version, false);
   } catch (Exception_t & e) {
           cerr << "Failed to open scaffold account in bank " << globals.bank << " : " << endl << e << endl;
           scf_stream.close();
           exit(1);
   }

   // build index of edges
   cte2weight = new hash_map<ID_t, int32_t, hash<ID_t>, equal_to<ID_t> >();
   hash_map<ID_t, set<ID_t, EdgeWeightCmp>*, hash<ID_t>, equal_to<ID_t> > ctg2lnk;     // map from contig to edges
   ContigEdge_t cte;
   for (AMOS::IDMap_t::const_iterator ci = edge_bank.getIDMap().begin(); ci; ++ci) {
      edge_bank.fetch(ci->iid, cte);

      if (isBadEdge(cte)) {
         continue;
      }

      if (cte.getIID() == 0 || cte.getContigs().first == 0 || cte.getContigs().second == 0) {
         // TODO: CTGs with links to ID 0 indicate links to/from singletons. Incorporate singletons into assembly?
         if (globals.debug >= 0) {
            cerr << "WARNING: link " << cte.getIID() << " (" << cte.getContigs().first << ", " << cte.getContigs().second << ") connects to a singleton, it is being ignored" << endl;
         }
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

   Scaffold_t scf;
   vector<Scaffold_t> scfs;
   while (scf_stream >> scf) {
      scfs.push_back(scf);
cerr << "Adding scf id " << scf.getIID() << endl;
   }
   linearizeScaffolds(scfs, edge_bank, ctg2lnk, globals.debug);

   // finally clear and output the new scaffolds
   scf_stream.clearCurrentVersion();
   for (std::vector<Scaffold_t>::iterator it = scfs.begin(); it != scfs.end(); it++) {
      scf_stream.append(*it);
   }
   Motif_t mtf;
   for (std::vector<Scaffold_t>::iterator it = scfs.begin(); it != scfs.end(); it++) {
      vector<Tile_t> st = it->getContigTiling();
      ID_t scfIID = it->getIID();
 
      for (vector<Tile_t>::iterator tile = st.begin(); tile != st.end(); tile++) {
         if (tile->source_type == Motif_t::NCODE) {
            motif_bank.fetch(tile->source, mtf);
            mtf.setScf(scfIID);
            motif_bank.replace(mtf.getIID(), mtf);
         }
      }
   }

   edge_bank.close();
   contig_bank.close();
   motif_bank.close();
   scf_stream.close();

   delete(cte2weight);
}
