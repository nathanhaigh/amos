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

#include "fasta.hh"
#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigIterator_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

#include "Utilities_Bundler.hh"

#include "Position.hh"
#include "Motif_Utils.hh"

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

void outputScaffold(
   Scaffold_t &scf, 
   Bank_t &contig_bank, Bank_t &edge_bank,
   HASHMAP::hash_map<AMOS::ID_t, string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > &motifSeq,
   HASHMAP::hash_map<AMOS::ID_t, string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > &motifName) {
        ID_t max = contig_bank.getMaxIID();
        vector<Position> edits;
        Position result;
        std::ostringstream stream;
        stream << "scf" << scf.getIID();

        for (std::vector<Tile_t>::const_iterator tileIt = scf.getContigTiling().begin(); tileIt < scf.getContigTiling().end(
); tileIt++) {
           string gapped;
           string eid;
           if (tileIt->source > max) {
              if (motifSeq.find(tileIt->source) == motifSeq.end()) {
                 cerr << "Error: unknown contig id " << tileIt->source << endl;
                 exit(1);
               }
               gapped = motifSeq[tileIt->source];
               eid = motifName[tileIt->source];
           } else {
              Contig_t ctg;
              contig_bank.fetch(tileIt->source, ctg);
              gapped = ctg.getSeqString(tileIt->range);
              eid = ctg.getEID();
           }
           result = result.merge(Position(eid, tileIt->offset, tileIt->offset + tileIt->range.getLength(), gapped), edits);
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
   string name;
   HASHMAP::hash_map<AMOS::ID_t, string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > motifSeq;
   HASHMAP::hash_map<AMOS::ID_t, string, HASHMAP::hash<AMOS::ID_t>, HASHMAP::equal_to<AMOS::ID_t> > motifName;

   for (AMOS::IDMap_t::const_iterator ci = motif_bank.getIDMap().begin(); ci; ci++) {
      motif_bank.fetch(ci->iid, mtf);
      motifSeq[ci->iid] = Bundler::outputMotif(name, mtf, motif_bank, contig_bank, edge_bank, false);
      motifName[ci->iid] = name;
   }

   for (AMOS::IDMap_t::const_iterator ci = scaffold_bank.getIDMap().begin(); ci; ci++) {
      scaffold_bank.fetch(ci->iid, scf);
      outputScaffold(scf, contig_bank, edge_bank, motifSeq, motifName);
   }

   edge_bank.close();
   contig_bank.close();
   motif_bank.close();
   scaffold_bank.close();
   motif_bank.close();
}
