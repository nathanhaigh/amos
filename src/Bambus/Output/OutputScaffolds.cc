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
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#endif //AMOS_HAVE_BOOST

#include "fasta.hh"
#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Utilities_Bundler.hh"
#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

#include "Position.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;

struct config {
   string      bank;
   int32_t     debug;
   int32_t     version;
};
config globals;
void printHelpText() {
   cerr <<
    "\n"
    "Output fasta sequence for Bambus scaffolds\n"
    "\n"
    "USAGE:\n"
    "\n"
    "OutputScaffolds -b[ank] <bank_name> \n"
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

   int c;
   globals.version = Bank_t::OPEN_LATEST_VERSION;

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

void outputScaffold(Scaffold_t &scf, Bank_t &contig_bank, Bank_t &edge_bank) {
        ID_t max = contig_bank.getMaxIID();
        vector<Position> edits;
        Position result;
        std::ostringstream stream;
        stream << "scf" << scf.getIID();

        for (std::vector<Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(
); tileIt++) {
           if (tileIt->source > max) {
              cerr << "Error: unknown contig id " << tileIt->source << endl;
              exit(1);
           }
           Contig_t ctg;
           contig_bank.fetch(tileIt->source, ctg);
           string gapped = ctg.getSeqString(tileIt->range);
           result = result.merge(Position(ctg.getEID(), tileIt->offset, tileIt->offset + tileIt->range.getLength(), gapped), edits);
       }
       // print the main sequence
       Fasta_Print(stdout, result.getUngappedSequence().c_str(), stream.str().c_str());
}

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

   Bank_t scaffold_bank (Scaffold_t::NCODE);
   if (! scaffold_bank.exists(globals.bank)) {
	   cerr << "No scaffold account found in bank " << globals.bank << endl;
	   exit(1);
   }
   try {
	   scaffold_bank.open(globals.bank, B_READ, globals.version);
   } catch (Exception_t & e) {
	   cerr << "Failed to open scaffold account in bank " << globals.bank << " : " << endl << e << endl;
	   scaffold_bank.close();
	   exit(1);
   }

   Bank_t motif_bank (Motif_t::NCODE);
   if (! motif_bank.exists(globals.bank)) {
           cerr << "No motif account found in bank " << globals.bank << endl;
           exit(1);
   }
   try {
           motif_bank.open(globals.bank, B_READ, globals.version);
   } catch (Exception_t & e) {
           cerr << "Failed to open motif account in bank " << globals.bank << " : " << endl << e << endl;
           motif_bank.close();
           exit(1);
   }

   // first expand motifs if we have any
   Motif_t mtf;
   Scaffold_t scf;
   HASHMAP::hash_map<AMOS::ID_t, Scaffold_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > scfMap;
   for (AMOS::IDMap_t::const_iterator ci = scaffold_bank.getIDMap().begin(); ci; ci++) {
      scaffold_bank.fetch(ci->iid, scf);
      scfMap[scf.getIID()] = scf;
   }

   for (AMOS::IDMap_t::const_iterator ci = motif_bank.getIDMap().begin(); ci; ci++) {
      motif_bank.fetch(ci->iid, mtf);
      scf = scfMap[mtf.getScf()];
      vector<Tile_t> t = mtf.getContigTiling();
      vector<ID_t> e = mtf.getContigEdges();
    
      vector<Tile_t> st = scf.getContigTiling();
      vector<ID_t> se = scf.getContigEdges();
      st.insert(st.begin(), t.begin(), t.end());
      se.insert(se.begin(), e.begin(), e.end());
      scf.setContigTiling(st);
      scf.setContigEdges(se);
      scfMap[scf.getIID()] = scf;
   }

   vector<Scaffold_t> scfs;
   for (HASHMAP::hash_map<AMOS::ID_t, Scaffold_t, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> >::iterator it = scfMap.begin(); it != scfMap.end(); ++it) {
      scfs.push_back(it->second);
   }

   // remove the motif tiles as well
   for (vector<Scaffold_t>::iterator it = scfs.begin(); it != scfs.end(); ++it) {
      vector<Tile_t> st = it->getContigTiling();
      for (vector<Tile_t>::iterator tile = st.begin(); tile != st.end(); ) {
         if (tile->source_type == Motif_t::NCODE) {
            tile = st.erase(tile);
         } else {
            ++tile;
         }
      }
      it->setContigTiling(st);
   }

   for (vector<Scaffold_t>::iterator it = scfs.begin(); it != scfs.end(); ++it) {
      outputScaffold((*it), contig_bank, edge_bank);
   }

   edge_bank.close();
   contig_bank.close();
   motif_bank.close();
   scaffold_bank.close();
   motif_bank.close();
}
