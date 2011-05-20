#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <getopt.h>
#include <map>
#include <set>
#include <list>
#include <functional>
#include "foundation_AMOS.hh"

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

map<string, string> globals; // global variables

void printHelpText()
{
  cerr << 
    "\n"
    "Partition a bank into <bank name>_1..N"
    "\n"
    "USAGE:\n"
    "\n"
    "partition-bank -b[ank] <bank_name> <-partitions N>\n"
       << endl;
}


bool GetOptions(int argc, char ** argv)
{  
  int option_index = 0;
  static struct option long_options[] = {
    {"help",  0, 0, 'h'},
    {"h",     0, 0, 'h'},
    {"b",     1, 0, 'b'},
    {"bank",  1, 0, 'b'},
    {"partitions", 1, 0, 'p'},
    {0, 0, 0, 0}
  };
  
  int c;
  while ((c = getopt_long_only(argc, argv, "", long_options, &option_index))!= -
	 1){
    switch (c){
    case 'h':
      printHelpText();
      break;
    case 'b':
      globals["bank"] = string(optarg);
      break;
    case 'p':
      globals["partitions"] = string(optarg);
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions

void closeBanks(Bank_t &library_bank, Bank_t &frag_bank, Bank_t &red_bank, Bank_t &layout_bank) {
  if (layout_bank.isOpen()) 
     layout_bank.close();
  if (frag_bank.isOpen()) 
    frag_bank.close();
  if (library_bank.isOpen()) 
    library_bank.close();
  if (red_bank.isOpen()) 
     red_bank.close();
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
int main(int argc, char **argv)
{
  ID_t LinkId = 0; // link IIDs

  if (!GetOptions(argc, argv)){
    cerr << "Command line parsing failed" << endl;
    printHelpText();
    exit(1);
  }

  if (globals.find("bank") == globals.end()){ // no bank was specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  Bank_t library_bank (Library_t::NCODE);
  Bank_t read_bank (Read_t::NCODE);
  BankStream_t frag_bank (Fragment_t::NCODE);
  Bank_t layout_bank (Layout_t::NCODE);
  BankStream_t layout_stream (Layout_t::NCODE);

  try {
  if (! library_bank.exists(globals["bank"])){
    cerr << "No library account found in bank " << globals["bank"] << endl;
  } else {
    try {
       library_bank.open(globals["bank"], B_READ);
    } catch (Exception_t & e) {
       cerr << "Failed to open library account in bank " << globals["bank"] 
   	    << ": " << endl << e << endl;
       exit(1);
     }
  }
  
  if (! layout_bank.exists(globals["bank"])){
    cerr << "No contig account found in bank " << globals["bank"] << endl;
    exit(1);
  } 
  try {
    layout_bank.open(globals["bank"], B_READ);
    layout_stream.open(globals["bank"], B_READ);
  } catch (Exception_t & e) {
      cerr << "Failed to open contig account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
  }

  if (! read_bank.exists(globals["bank"])){
    cerr << "No read account found in bank " << globals["bank"] << endl;
  } else {
  try {
    read_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open read account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }
  }

  if (! frag_bank.exists(globals["bank"])){
    cerr << "No fragment account found in bank " << globals["bank"] << endl;
  } else {
  try {
    frag_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open fragment account in bank " << globals["bank"] 
	   << ": " << endl << e << endl;
      exit(1);
    }
   }

   // figure out how many layout we want to create and then partition the banks
   ID_t maxLayout = (layout_bank.getSize() / atoi(globals["partitions"].c_str())) + 1; 
   Layout_t lay;
   uint32_t count = 0;
   uint32_t partition = 1;

   Bank_t tmp_lib (Library_t::NCODE);
   Bank_t tmp_frag (Fragment_t::NCODE);
   Bank_t tmp_read (Read_t::NCODE);
   Bank_t tmp_layout (Layout_t::NCODE);
   while (layout_stream >> lay) {
      if (count % maxLayout == 0) {
         // create the partitions      
         if (tmp_lib.isOpen()) {
            closeBanks(tmp_lib, tmp_frag, tmp_read, tmp_layout);
         }
         stringstream ss;
         ss << globals["bank"] << "_partition" << partition << ".bnk";
         if (tmp_lib.exists(ss.str()) || tmp_frag.exists(ss.str()) || tmp_read.exists(ss.str()) || tmp_layout.exists(ss.str())) {
            cerr << "Error partition " << ss.str() << " already exists" << endl;
            exit(1);
         }
         tmp_lib.create(ss.str(), B_READ | B_WRITE);
         tmp_frag.create(ss.str(), B_READ | B_WRITE);
         tmp_read.create(ss.str(), B_READ | B_WRITE);
         tmp_layout.create(ss.str(), B_READ | B_WRITE);
         partition++;
      }

      // store the object to the bank
      tmp_layout.append(lay);
      vector<Tile_t> tiling = lay.getTiling();
      for (std::vector<Tile_t>::iterator i = tiling.begin(); i != tiling.end(); ++i) {
         assert(i->source != NULL_ID);
         // fetch the read, frag, and library if needed
         Read_t read;
         read_bank.fetch(i->source, read);
         if (tmp_read.getIDMap().exists(i->source)) {
            continue;
         }
         tmp_read.append(read);
         if (read.getFragment() != NULL_ID) {
            Fragment_t frag;
            frag_bank.fetch(read.getFragment(), frag);
            tmp_frag.append(frag);
            
            if (frag.getLibrary() != NULL_ID) {
               Library_t lib;
               library_bank.fetch(frag.getLibrary(), lib);
               tmp_lib.append(lib);
            }
         }
      }

      count++;
   }
   closeBanks(tmp_lib, tmp_frag, tmp_read, tmp_layout);

   } catch (Exception_t & e)
    {
      cerr << "Failed to open fragment account in bank " << globals["bank"]
           << ": " << endl << e << endl;
      exit(1);
    }
   layout_stream.close();
   closeBanks(library_bank, frag_bank, read_bank, layout_bank);

   return(0);
} // main
