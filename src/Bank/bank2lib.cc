// $Id$

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
#include <fstream>
#include <ctype.h>

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

// endl is a sham: die endl, die!
#define endl "\n"

map<string, string> globals; // global variables

void printHelpText()
{
  cerr << "\n.USAGE.\n"
       << "  bank2libs -b <bank_name> -o <prefix>\n"
       << "\n.DESCRIPTION.\n"
       << "  bank2lbs - output the read to lib mapping in the bank. Also output list of libs\n"
       << "\n.OPTIONS.\n"
       << "  -h, -help     print out help message\n"
       << "  -b <bank_name>, -bank     bank where assembly is stored\n"
       << "  -eid          report eids\n"
       << "  -iid          report iids (default)\n"
       << "  -o prefix     output prefix\n"
       << "\n.KEYWORDS.\n"
       << "  AMOS bank, Converters\n"
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
    {"o",         1, 0, 'o'},
    {0, 0, 0, 0}
  };
 
  globals["iid"] = "true";
  globals["eid"] = "false";
  globals["prefix"] = "asm"; 
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -1){
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
    case 'o':
      globals["prefix"] = string(optarg);
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

  Bank_t frag_bank (Fragment_t::NCODE);
  Bank_t lib_bank (Library_t::NCODE);
  BankStream_t read_stream (Read_t::NCODE);
  if (! read_stream.exists(globals["bank"]) || ! frag_bank.exists(globals["bank"]) || ! lib_bank.exists(globals["bank"])) {
    cerr << "No read account found in bank " << globals["bank"] << endl;
    exit(1);
  }


  try 
  {
    read_stream.open(globals["bank"], B_SPY);
    lib_bank.open(globals["bank"], B_SPY);
    frag_bank.open(globals["bank"], B_SPY);
  } 
  catch (Exception_t & e)
  {
    cerr << "Failed to open contig account in bank " << globals["bank"] 
         << ": " << endl << e << endl;
    exit(1);
  }
  

  try
  {
    Read_t read;
    Fragment_t frag;
    Library_t lib;
    ofstream file;
    ofstream libs;
    file.open(string(globals["prefix"] + ".readToLib").c_str(), ios::out);
    libs.open(string(globals["prefix"] + ".libList").c_str(), ios::out);
    map<ID_t, int> printedLibs;

     while (read_stream >> read) 
     {
        ID_t fragID = read.getFragment();
        ID_t libID = NULL_ID;

        if (fragID == NULL_ID) {
           continue;
        } else {
           frag_bank.fetch(fragID, frag);
           libID = frag.getLibrary();
        }
        if (libID == NULL_ID) {
           continue;
        }

        if (globals["iid"] == "true") {
           file << read.getIID() << "\t" << libID << endl;
           if (printedLibs[libID] == 0) {
               libs << libID << endl;
               printedLibs[libID] = 1;
            }
        } else {
           lib_bank.fetch(libID, lib);
           string libEID = lib.getEID();
           file << read.getEID() << "\t" << libEID << endl;
           if (printedLibs[libID] == 0) {
              libs << libEID << endl;
              printedLibs[libID] = 1;
           }
        }
     }

     file.close();
     libs.close();

    read_stream.close();
    frag_bank.close();
    lib_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }


  return(0);
} // main
    
