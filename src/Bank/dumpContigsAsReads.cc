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
    "\n.USAGE.\n"
    "  dumpContigsAsReads -b[ank] <bank_name>\n"
    "\n.DESCRIPTION.\n"
    "  dumpContigsAsReads - dumps the contigs in a bank as reads in a new .afg\n"
    "\n.OPTIONS.\n"
    "  -h, -help     print out help message\n"
    "  -b, -bank     bank where assembly is stored\n"
    "  -E file       Dump just the contig eids listed in file\n"
    "  -I file       Dump just the contig iids listed in file\n"
    "\n.KEYWORDS.\n"
    "  AMOS bank, Converters, contigs\n"
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
    {"E",         1, 0, 'E'},
    {"I",         1, 0, 'I'},
    {0, 0, 0, 0}
  };
  
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
    case 'E':
      globals["eidfile"] = string(optarg);
      break;
    case 'I':
      globals["iidfile"] = string(optarg);
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions

void printContigAsRead(const Contig_t & ctg)
{
  Read_t nrd;
  Message_t message;

  string seq = ctg.getSeqString();
  string quals = ctg.getQualString();
  string newseq = ""; 
  string newqual = "";

  for (int i = 0; i < seq.length(); i++)
  {
    if (seq[i] == '-') { continue; }
    
    newseq += seq[i];
    newqual += quals[i];
  }

  nrd.setSequence(newseq, newqual);
  nrd.setIID(ctg.getIID());
  nrd.setEID(ctg.getEID());
  nrd.setClearRange(Range_t(0,nrd.getLength() - 1));
  
  nrd.writeMessage(message);
  message.write(cout);
} // printFasta

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

  Bank_t contig_bank (Contig_t::NCODE);
  BankStream_t contig_stream (Contig_t::NCODE);
  if (! contig_bank.exists(globals["bank"])){
    cerr << "No contig account found in bank " << globals["bank"] << endl;
    exit(1);
  }


  try 
  {
    contig_stream.open(globals["bank"], B_READ);
  } 
  catch (Exception_t & e)
  {
    cerr << "Failed to open contig account in bank " << globals["bank"] 
         << ": " << endl << e << endl;
    exit(1);
  }
  

  try
  {
    Contig_t ctg;
    ifstream file;
    string id;

    if (!globals["eidfile"].empty())
    {
      file.open(globals["eidfile"].c_str());
      
      if (!file)
      {
        throw Exception_t("Couldn't open EID File", __LINE__, __FILE__);
      }

      while (file >> id)
      {
        contig_stream.seekg(contig_stream.getIDMap().lookupBID(id));
        contig_stream >> ctg;
        printContigAsRead(ctg);
      }
    }
    else if (!globals["iidfile"].empty())
    {
      file.open(globals["iidfile"].c_str());

      if (!file)
      {
        throw Exception_t("Couldn't open IID File", __LINE__, __FILE__);
      }

      while (file >> id)
      {
        contig_stream.seekg(contig_stream.getIDMap().lookupBID(atoi(id.c_str())));
        contig_stream >> ctg;
        printContigAsRead(ctg);
      }
    }
    else
    {
      while (contig_stream >> ctg) 
      {
        printContigAsRead(ctg);
      }
    }

    contig_stream.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }


  return(0);
} // main
    
