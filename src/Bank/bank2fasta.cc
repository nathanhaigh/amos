// $Id$

// This program reads a set of contigs from a bank then reports 
// regions where mate-pair information indicates a problem

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <math.h>
#include <functional>
#include "foundation_AMOS.hh"

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

// endl is a sham: die endl, die!
#define endl "\n"

map<string, string> globals; // global variables

void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "bank2fasta - generates a .fasta file from a bank\n"
    "\n.USAGE.\n"
    "bank2fasta -b[ank] <bank_name>\n"
    "\n.OPTIONS.\n"
    "-h, -help     print out help message\n"
    "-b, -bank     bank where assembly is stored\n"
    "-eid          report eids\n"
    "-iid          report iids (default)\n"
    "\n.DESCRIPTION.\n"
    "\n.KEYWORDS.\n"
    "AMOS bank, Converters\n"
       << endl;
}

bool GetOptions(int argc, char ** argv)
{  
  int option_index = 0;
  static struct option long_options[] = {
    {"help",      0, 0, 'h'},
    {"h",         0, 0, 'h'},
    {"b",         1, 0, 'b'},
    {"bank",      1, 0, 'b'},
    {"eid",       0, 0, 'e'},
    {"iid",       0, 0, 'i'},
    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -
         1){
    switch (c){
    case 'h':
      printHelpText();
      exit(0);
      break;
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case 'e':
      globals["eid"] = "true";
      globals["iid"] = "false";
      break;
    case 'i':
      globals["iid"] = "true";
      globals["eid"] = "false";
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions

//----------------------------------------------
int main(int argc, char **argv)
{
  globals["iid"] = "true";
  globals["eid"] = "false";

  if (! GetOptions(argc, argv)){
    cerr << "Command line parsing failed" << endl;
    printHelpText();
    exit(1);
  }

  // open necessary files
  if (globals.find("bank") == globals.end()){ // no bank was specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  Bank_t contig_bank (Contig_t::NCODE);
  BankStream_t contig_stream (Contig_t::NCODE);
  if (! contig_bank.exists(globals["bank"])){
    cerr << "No contig account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    contig_stream.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open contig account in bank " << globals["bank"] 
           << ": " << endl << e << endl;
      exit(1);
    }

  Contig_t ctg;
  while (contig_stream >> ctg) {
    string seq = ctg.getSeqString();
    if (globals["eid"] == "true")
      cout << ">" << ctg.getEID();
    else
      cout << ">" << ctg.getIID();

    int nout = 0;
    for (int i = 0; i < seq.length(); i++){
      if (seq[i] == '-')
	continue;
      if (nout % 60 == 0)
	cout << endl;
      nout++;

      cout << seq[i];
    }
    cout << endl;
  }

  contig_stream.close();

  return(0);
} // main
    
