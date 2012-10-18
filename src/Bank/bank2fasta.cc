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
       << "  bank2fasta -b <bank_name>\n"
       << "\n.DESCRIPTION.\n"
       << "  bank2fasta - generates a .fasta (and .qual) file from the contigs in a bank\n"
       << "\n.OPTIONS.\n"
       << "  -h, -help     print out help message\n"
       << "  -b <bank_name>, -bank     bank where assembly is stored\n"
       << "  -eid          report eids\n"
       << "  -iid          report iids (default)\n"
       << "  -E file       Dump just the contig eids listed in file\n"
       << "  -I file       Dump just the contig iids listed in file\n"
       << "  -q file       Report qualities in file\n"
       << "  -d            Show contig details (num reads, coverage) on fasta header line\n"
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
    {"d",         0, 0, 'd'},
    {"b",         1, 0, 'b'},
    {"bank",      1, 0, 'b'},
    {"eid",       0, 0, 'e'},
    {"iid",       0, 0, 'i'},
    {"E",         1, 0, 'E'},
    {"I",         1, 0, 'I'},
    {"q",         1, 0, 'q'},
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
    case 'd':
      globals["details"] = "true";
      break;
    case 'e':
      globals["eid"] = "true";
      globals["iid"] = "false";
      break;
    case 'i':
      globals["iid"] = "true";
      globals["eid"] = "false";
      break;
    case 'E':
      globals["eidfile"] = string(optarg);
      break;
    case 'I':
      globals["iidfile"] = string(optarg);
      break;
    case 'q':
      globals["qualfile"] = string(optarg);
      break;
    case '?':
      return false;
    }
  }

  return true;
} // GetOptions

void printFasta(const Contig_t & ctg, ofstream & qual)
{
  string seq = ctg.getSeqString();
  string quals = ctg.getQualString();

  if (globals["eid"] == "true") { 
    cout << ">" << ctg.getEID(); 
    if (globals.find("qualfile") != globals.end())
      qual << ">" << ctg.getEID();
  } else { 
    cout << ">" << ctg.getIID(); 
    if (globals.find("qualfile") != globals.end())
      qual << ">" << ctg.getIID();
  }

  if (globals.find("details") != globals.end())
  {
    int len = ctg.getUngappedLength();

    std::vector<Tile_t>::const_iterator ti;
    const std::vector<Tile_t> & tiling = ctg.getReadTiling();

    int nreads = ctg.getReadTiling().size();
    double covsum = 0.0;

    for (ti = tiling.begin();
         ti != tiling.end();
         ti++)
    {
      covsum += ti->range.getLength();
    }

    double cov = len ? covsum / len : 0.0;

    cout << " len=" << len
         << " nreads=" << nreads
         << " cov=" << cov;
  }

  int nout = 0;
  for (int i = 0; i < seq.length(); i++)
  {
    if (seq[i] == '-') { continue; }
    if (nout % 60 == 0) 
      cout << endl; 

    if (nout % 20 == 0 && globals.find("qualfile") != globals.end())
      qual << endl;
    
    nout++;
    cout << seq[i];
    if (globals.find("qualfile") != globals.end())
      qual << (int) (toascii(quals[i]) - toascii('0')) << " ";
  }
  cout << endl;
  if (globals.find("qualfile") != globals.end())
    qual << endl;
  
} // printFasta

//----------------------------------------------
int main(int argc, char **argv)
{
  ofstream outqual;
  globals["iid"] = "true";
  globals["eid"] = "false";

  if (! GetOptions(argc, argv)){
    cerr << "Command line parsing failed" << endl;
    printHelpText();
    exit(1);
  }

  if (globals.find("qualfile") != globals.end()){ // have qual file will travel
    outqual.open(globals["qualfile"].c_str(), ofstream::out | ofstream::trunc);
    if (! outqual.is_open()){
      cerr << "Could not open " << globals["qualfile"] << endl;
      exit(1);
    }
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
        printFasta(ctg, outqual);
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
        printFasta(ctg, outqual);
      }
    }
    else
    {
      while (contig_stream >> ctg) 
      {
        printFasta(ctg, outqual);
      }
    }

    contig_stream.close();
    if (globals.find("qualfile") != globals.end())
      outqual.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }


  return(0);
} // main
    
