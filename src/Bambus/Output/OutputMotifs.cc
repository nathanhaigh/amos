#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>
#include <math.h>
#include <limits>
#include <string>
#include <iostream>
#include <map>

#include "fasta.hh"
#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

#include "Utilities_Bundler.hh"
#include "Motif_Sequence.hh"
#include "Motif_Utils.hh"

using namespace HASHMAP;
using namespace std;
using namespace AMOS;

struct config {
   string      bank;
   int32_t     debug;
   int32_t     version;
   bool        allMotifs;
};
config globals;
void printHelpText() {
   cerr <<
    "\n"
    "Output fasta sequence for Bambus motifs along with alternate sequence\n"
    "\n"
    "USAGE:\n"
    "\n"
    "OutputMotifs -b[ank] <bank_name> [-version n] \n"
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
    {"allMotifs",          0, 0, 'a'},
    {0, 0, 0, 0}
  };

   globals.debug = 0;
   globals.version = Bank_t::OPEN_LATEST_VERSION;
   globals.allMotifs = false;

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
      case 'a':
         globals.allMotifs = true;
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
	   motif_bank.open(globals.bank, B_READ, globals.version);
   } catch (Exception_t & e) {
	   cerr << "Failed to open motif account in bank " << globals.bank << " : " << endl << e << endl;
	   motif_bank.close();
	   exit(1);
   }

   set<Output::MotifStats, Output::MotifOrderCmp> motifs;
   Motif_t scf;
   hash_map<ID_t, Motif_t, hash<ID_t>, equal_to<ID_t> > motifLayouts;
   Output::MotifStats stat;
   
   for (AMOS::IDMap_t::const_iterator ci = motif_bank.getIDMap().begin(); ci; ci++) {
        motif_bank.fetch(ci->iid, scf);
        if (scf.getStatus() != Bundler::MOTIF_SCAFFOLD && !globals.allMotifs) {
           continue;
        }
        Output::getMotifStats(scf, edge_bank, stat);

        // now store the info on the motif
        stat.id = scf.getIID();
        stat.name = scf.getEID();
        if (stat.name == "") { stat.name = scf.getIID(); }
        motifs.insert(stat);

        motifLayouts[scf.getIID()] = scf;
   }

   for (set<Output::MotifStats, Output::MotifOrderCmp>::const_iterator i = motifs.begin(); i != motifs.end(); i++) {
	   Output::outputMotif((std::string&)i->name, motifLayouts[i->id], motif_bank, contig_bank, edge_bank);
   }

   edge_bank.close();
   contig_bank.close();
   motif_bank.close();
}
