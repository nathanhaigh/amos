#ifndef TiggerIO_HH
#define TiggerIO_HH 1

#include "foundation_AMOS.hh"
#include "Read.hh"
#include "Overlap.hh"
#include "Unitigger.hh"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace AMOS;

static bool AMOS_mode = false;
static string bankdir;

static bool UMD_mode = false;
static string read_file;
static string overlap_file;
static string contig_file;

static bool VERBOSE = false;

Unitigger tigger;


void create_test_bank(const char* p_bankdir) {
  const char* TESTSEQ = "nnnn";
  Message_t msg;
  Read_t* read;
  BankStream_t bank(Read_t::NCODE);
  
  try {
    // create read bank for testing
    bank.create(p_bankdir);

    // create 10 reads
    for(int i = 0; i < 10; i++) {
      read = new Read_t();
      read->setSequence(TESTSEQ, "0000");
      bank << *read;
    }
  
    bank.close();

  } catch (Exception_t &e) {
    cerr << "Exception: " << e << endl;
  }
}


void get_amos_reads(const string p_bankdir) {
  Read_t amos_read;
  Message_t msg;
  BankStream_t bank(Read_t::NCODE);
  int readCount = 0;

  try {
    if(bank.exists(p_bankdir)) {
      cout << " Pulling reads from bank " << p_bankdir << endl;
      bank.open(p_bankdir);

      if(bank.empty()) {
	cerr << p_bankdir << " bank is empty" << endl;
	return;
      }
      
      while(bank >> amos_read) {
	if(VERBOSE) {
	  amos_read.writeMessage(msg);
	  msg.write(cout);
	}

	tigger.add_read(amos_read);
	readCount++;
      }
      
      bank.close(); 
      cout << "Pulled " << readCount << " reads from bank " << endl;
    } else {
      cerr << " AMOS Read Bank " << p_bankdir << " does not exist " << endl;
    }
  } catch (Exception_t &e) {
    cerr << "Exception: " << e << endl;
  }
}

void get_amos_overlaps(const string p_bankdir) {
  Overlap_t amos_overlap;
  Message_t msg;
  BankStream_t bank(Overlap_t::NCODE);
  int overlapCount = 0;

  try {
    if(bank.exists(p_bankdir)) {
      cout << " Pulling overlaps from bank " << p_bankdir << endl;
      bank.open(p_bankdir);
      if(bank.empty()) {
	cout << " bank is empty" << endl;
	return;
      }
      
      while(bank >> amos_overlap) {
	if(VERBOSE) {
	  amos_overlap.writeMessage(msg);
	  msg.write(cout);
	}

	tigger.add_overlap(amos_overlap);
	overlapCount++;
      }
      
      bank.close();
      cout << "Pulled " << overlapCount << " overlaps from bank " << endl;
    } else {
      cerr << " AMOS Overlap Bank " << p_bankdir << " does not exist " << endl;
    }
  } catch (Exception_t &e) {
    cerr << "Exception: " << e << endl;
  }
}

static void parse_command_line(int argc, char* argv[]) {
  bool errflg = false;
  int ch;
  bool rflg = false;
  bool lflg = false;

  optarg = NULL;

  while (!errflg && ((ch = getopt(argc, argv, "r:l:b:hv")) != EOF)) {
    switch  (ch) {
    case 'b' :
      AMOS_mode = true;
      bankdir = optarg;
      break;

    case 'r' :
      rflg = true;
      UMD_mode = true;
      read_file = optarg;
      break;

    case 'l' :
      lflg = true;
      UMD_mode = true;
      overlap_file = optarg;
      break;
      
    case 'h' :
      errflg = true;
      break;

    case 'v':
      VERBOSE = true;
      break;
      
    case '?' :
      cerr << "Unrecognized option - " <<  optopt << endl;
      
    default :
      errflg = true;
    }
  }
  
  if(AMOS_mode  &&  (rflg || lflg)) {
    cerr << " Can't use AMOS and UMD together " << endl;
    errflg = true;
  }

  if((rflg && !lflg) || (!rflg && lflg)) {
    cerr << " Need to provide both a read file and a overlap file " << endl;
    errflg = true;
  }

  if(!AMOS_mode  &&  !UMD_mode) {
    cerr << " Need to provide either an AMOS bank or UMD files " << endl;
    errflg = true;
  }

  if(errflg) {
    cerr << " Usage: tigger -b <AMOS Bank> -r <UMD reads> -l <UMD overlaps> " << endl;
    exit(EXIT_FAILURE);
  }

  return;
}


int main(int argc, char** argv) {
  parse_command_line(argc, argv);

  if(AMOS_mode) {

    if(VERBOSE) {
      cout << " AMOS mode " << endl;
      cout << " use bank: " << bankdir << endl;
    }

    get_amos_reads(bankdir);
    get_amos_overlaps(bankdir);
    tigger.calc_contigs();
    tigger.output_amos_contigs(bankdir);

  } else if(UMD_mode) {

    cout << " UMD mode " << endl;
    cout << "\t get reads from: " << read_file << endl;
    cout << "\t get overlaps from: " << overlap_file << endl;

  } else {

    cout << " Error with inputs, no mode was set" << endl;
  }

}

#endif // #ifndef TiggerIO_HH
