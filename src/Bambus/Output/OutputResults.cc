#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>

#include "fasta.hh"
#include "datatypes_AMOS.hh"
#include "Bank_AMOS.hh"
#include "BankStream_AMOS.hh"

#include "Output_Utils.hh"
#include "Contig_AMOS.hh"
#include "ContigEdge_AMOS.hh"
#include "Scaffold_AMOS.hh"
#include "Motif_AMOS.hh"

using namespace std;
using namespace HASHMAP;
using namespace AMOS;
using namespace Bundler;

struct config {
   string      bank;
   string      prefix;
   int32_t     debug;
   int32_t     version;
   bool        outputBambus;
};
config globals;
void printHelpText() {
   cerr <<
    "\n"
    "Output text results for Bambus\n"
    "\n"
    "USAGE:\n"
    "\n"
    "OutputText -b[ank] <bank_name> -p <prefix> [-version n]\n"
       << endl;
}

bool GetOptions(int argc, char ** argv) {
   int option_index = 0;
   static struct option long_options[] = {
    {"help",               0, 0, 'h'},
    {"h",                  0, 0, 'h'},
    {"b",                  1, 0, 'b'},
    {"bank",               1, 0, 'b'},
    {"bambus1",            0, 0, '1'},
    {"prefix",             1, 0, 'p'},
    {"version",            1, 0, 'v'},
    {0, 0, 0, 0}
  };

   globals.prefix = "out";
   globals.version = Bank_t::OPEN_LATEST_VERSION;
   globals.outputBambus = false;

   int c;
   while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1){
      switch (c){
      case 'h':
         printHelpText();
         break;
      case 'b':
         globals.bank = string(optarg);
         break;
      case '1':
         globals.outputBambus = true;
         break;
      case 'p':
         globals.prefix = string(optarg);
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
      cerr << "Prefix = \t" << globals.prefix << endl;
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

   BankStream_t motif_stream (Motif_t::NCODE);
   if (! motif_stream.exists(globals.bank)) {
	   cerr << "No motif account found in bank " << globals.bank << endl;
	   exit(1);
   }
   try {
	   motif_stream.open(globals.bank, B_READ, globals.version);
   } catch (Exception_t & e) {
	   cerr << "Failed to open motif account in bank " << globals.bank << " : " << endl << e << endl;
	   motif_stream.close();
	   exit(1);
   }

   BankStream_t scf_stream (Scaffold_t::NCODE);
   if (! scf_stream.exists(globals.bank)) {
           cerr << "No scaffold account found in bank " << globals.bank << endl;
           exit(1);
   }
   try {
           scf_stream.open(globals.bank, B_READ, globals.version);
   } catch (Exception_t & e) {
           cerr << "Failed to open scaffold account in bank " << globals.bank << " : " << endl << e << endl;
           scf_stream.close();
           exit(1);
   }

   outputResults(globals.bank, contig_bank, edge_bank, motif_stream, scf_stream, DOT, globals.prefix, globals.debug);
   motif_stream.seekg(0,BankStream_t::BEGIN);
   scf_stream.seekg(0,BankStream_t::BEGIN);

   outputResults(globals.bank, contig_bank, edge_bank, motif_stream, scf_stream, AGP, globals.prefix, globals.debug);
   motif_stream.seekg(0,BankStream_t::BEGIN);
   scf_stream.seekg(0,BankStream_t::BEGIN);

   if (globals.outputBambus == true) {
      outputResults(globals.bank, contig_bank, edge_bank, motif_stream, scf_stream, BAMBUS, globals.prefix, globals.debug);
      motif_stream.seekg(0,BankStream_t::BEGIN);
      scf_stream.seekg(0,BankStream_t::BEGIN);
   }

   outputResults(globals.bank, contig_bank, edge_bank, motif_stream, scf_stream, MOTIFS, globals.prefix, globals.debug);

   edge_bank.close();
   contig_bank.close();
   motif_stream.close();
   scf_stream.close();
}
