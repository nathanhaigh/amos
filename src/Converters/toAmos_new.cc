///////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Sergey Koren 
//! \date 06/24/04
//!
//! \brief Directly places files in multiple formats into the AMOS Bank
//!
//!
//!
//!
//!
////////////////////////////////////////////////////////////////////////////////

extern "C" {
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>
}

#include <map>
#include <set>
#include <list>
#include <string>
#include <functional>
#include <fasta.hh>
#include <fastq.hh>
#include <algorithm>
#include "foundation_AMOS.hh"
#include <Contig_AMOS.hh>
#include <fstream>
#include <iostream>
#include "amp.hh"

#ifdef AMOS_HAVE_CA
#include "AS_MSG_pmesg.h"
#endif

using namespace AMOS;
using namespace std;
using namespace HASHMAP;


//==============================================================================//
// Globals
//==============================================================================//
int PRINT_INTERVAL = 100000;
int GOODQUAL = 30;
int BADQUAL = 10;

int MAX_LINE_LEN = 256;
char GAP_CHAR = '-';

struct config {
  uint32_t readSurrogates;
  uint32_t readCCO;
  uint32_t readUTG;
  uint32_t fastqType;
  uint32_t surrogateAsFragment;
  string        posfile;
  string        bank;
  string        matesfile;
  string        traceinfofile;
  string        ctgfile;
  string        frgfile;
  string        asmfile;
  string        tasmfile;
  string        qualfile;
  string        fastafile;
  string        fastqfile;
  FastqQualType fastqQualityType;
  string        libmap;
  uint32_t      surroageAsFragment;
  uint32_t      layoutOnly;
  uint8_t       debugLevel;
};
config globals;

Bank_t lib_stream (Library_t::NCODE);
Bank_t read_stream (Read_t::NCODE);
Bank_t contig_stream (Contig_t::NCODE);
Bank_t frag_stream (Fragment_t::NCODE);
Bank_t link_stream (ContigLink_t::NCODE);
Bank_t edge_stream (ContigEdge_t::NCODE);
Bank_t scf_stream (Scaffold_t::NCODE);
Bank_t layout_stream (Layout_t::NCODE);

bool matesDone = false;
bool readsDone = false;
bool contigsDone = false;

int minSeqID = 1;
int minCtgID = 1;
int minScfID = 1;

// this is where all my data live
/*
map<int,vector< string > >      insertlib;  // lib id to insert list
map<string,char>                inserttype; // the type of insert, if undefined assume 'E'
map<string,string>              seenlib;    // insert id to lib id map
map<int,string>                 seqinsert;  // sequence id to insert id map
map<int,string>                 libnames;  // lib id to lib name
map<int,int>                    posidx;     // position of sequence in pos file
*/
map<string,string>              seq2qual; // a map of all the qual file names found.

//==============================================================================//
// Function Prototypes
//==============================================================================//
int *computeGapIndex(char *sequence, uint32_t length);
bool parseFastaFile();
bool parseFastqFile();
bool parseMatesFile(ifstream&);
bool parseFrgFile(string);
bool parseAsmFile(string);
bool parseContigFile(ifstream&);
void Tokenize(const string& str,
                      vector<string>& tokens,
              const string& delimiters);


//==============================================================================//
// Documentation
//==============================================================================//
void PrintHelp()
{
    cerr << "\n"
         << ".USAGE."
         << "  toAmos (-m mates|-f frg)\n"
         << "         (-c contig|-s fasta|-q qual|-Q fastq)\n"
         << "         -t fastqQualtyType [SCUFL]\n"
         << ".DESCRIPTION.\n"
         << " toAmos_new is designed as a replacement for the current perl script.\n"
	 << " It does not cover all of the formats but will encompass the main ones. \n"
         << " It inserts the output of an assembly program into the AMOS bank.\n\n"
         << " If you simply need a program to generate assembly inputs for one the\n"
         << " AMOS-based assemblers (e.g. minimus or AMOS-cmp) use tarchive2amos.\n\n"
         << " toAmos reads the inputs specified on the command line and stores  the \n"
         << " information directly into the AMOS Bank.  The following types of \n"
         << " information can be provided to toAmos: \n"
         << "   -> Sequence and quality data (options -f, -s, -q, -Q) \n"
         << "   -> Fastq data (options -Q, -t) \n"
         << "   -> Library and mate-pair data (options -m, or -f) \n"
         << "   -> Contig  data (options -c, -a) \n"
         << "   -> Scaffold data (option -a) \n"
         << ".OPTIONS. \n"
         << "  -m <matefile> - library and mate-pair information in Bambus format \n"
         << "     in Trace Archive format \n"
         << "  -b <bank> - The location where the AMOS bank will be stored.\n"
         << "  -S - include the surrogate unitigs in the .asm file as AMOS contigs.\n"
         << "  -C - include the contigs in the .asm file as AMOS contigs.\n"
         << "  -U - include the unitigs in the .asm file as AMOS contigs (implies -S and turns off -C).\n"
         << "  -F - include the surrogate unitigs as reads in the tilling for a contig. Without this option the contig may have 0-coverage regions of coverage.\n"
         << "  -L - output only the layout, not the consensus sequence for contigs. Will not output contig links or scaffolds. Implies -F.\n"
         << "  -t - fastq quality type. The currently supported types are";
         for (uint32_t i = 0; i < FASTQ_QUALITY_COUNT; i++) {
            cerr << " " << FASTQ_QUALITY_NAMES[i];
         }
         cerr << ". The default is " << FASTQ_QUALITY_NAMES[FASTQ_DEFAULT_QUALITY_TYPE] << "\n" 
         << ".KEYWORDS. \n"
         << "  converter, amos format\n"
         << endl;
}

//----------------------------------------------------- GetOptions -----------//
//! \brief Sets the global OPT_% values from the command line arguments
//!
//! \return void
//!
//----------------------------------------------------------------------- ----//

static int help_flag;
static int listed_flag;

bool GetOptions (int argc, char ** argv)
{
  globals.readSurrogates = 0;
  globals.readCCO = 1;
  globals.readUTG = 0;
  globals.fastqQualityType = FASTQ_DEFAULT_QUALITY_TYPE;
  globals.surrogateAsFragment = 0;
  globals.debugLevel = 0;
 
  while (1)
    {
      int ch, option_index = 0;
      static struct option long_options[] = {
        {"help",      no_argument,      &help_flag, 1},
        {"list",      no_argument,      &listed_flag, 1},
        {"bank",      required_argument,         0, 'b'},
        {"qual",      required_argument,         0, 'q'},
        {"seq",       required_argument,         0, 's'},
        {"Q",         required_argument,         0, 'Q'},
        {"map",       required_argument,         0, 'M'},
        {"gq",        required_argument,         0, 'G'},
        {"bq",        required_argument,         0, 'B'},
        {"pos",       required_argument,         0, 'p'},
        {"readSurrogates", no_argument,          0, 'S'},
        {"readCCO",   no_argument,               0, 'C'},
        {"readUTG",   no_argument,               0, 'U'},
        {"surrogateUTGAsReads", no_argument,     0, 'F'},
        {"layoutsOnly",no_argument,              0, 'L'},
        {"fasqQuality", required_argument,       0, 't'},
        {"debugLevel", required_argument,        0, 'd'},
        {0,           0,                         0, 0}
      };
      
      ch = getopt_long(argc, argv, "hlb:m:c:f:x:a:t:i:k:q:s:Q:M:G:B:p:SCUFLt:", long_options, &option_index);
      if (ch == -1)
        break;

      string qualName;
      uint32_t i;

      switch (ch)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
        case 'G':
          GOODQUAL = atoi(optarg);
          break;
        case 'B':
          BADQUAL = atoi(optarg);
          break;
        case 'p':
          globals.posfile = string(optarg);
          break;
        case 'b':
          globals.bank = string(optarg);
          break;
        case 'm':
          globals.matesfile = string(optarg);
          break;
        case 'x':
          globals.traceinfofile = string(optarg);
          break;
        case 'c':
          globals.ctgfile = string(optarg);
          break;
        case 'f':
          globals.frgfile = string(optarg);
          break;
        case 'a':
          globals.asmfile = string(optarg);
          break;
        case 'q':
          globals.qualfile = string(optarg);
          break;
        case 's':
          globals.fastafile = string(optarg);
          break;
        case 'Q':
          globals.fastqfile = string(optarg);
          break;
       case 't':
          qualName = string(optarg);
          transform(qualName.begin(), qualName.end(), qualName.begin(), ::toupper);

          for (i = 0; i < FASTQ_QUALITY_COUNT; i++) {
             if (qualName == FASTQ_QUALITY_NAMES[i]) {
                globals.fastqQualityType = (FastqQualType) i;
                break;
              }
          }         
          if (i == FASTQ_QUALITY_COUNT) {
             cerr << "Unknown fastq quality type " << optarg << " specified" << endl;
             help_flag = TRUE;
          }
          break;
        case 'M':
          globals.libmap = string(optarg);
          break;
        case 'S':
          globals.readSurrogates = 1;
          break;
        case 'U':
          globals.readUTG = 1;
          globals.readCCO = 0;
          break;
        case 'C':
          globals.readCCO = 1;
          break;
       case 'F':
          globals.surrogateAsFragment = 1;
          break;
       case 'L':
          globals.surroageAsFragment = 1;
          globals.layoutOnly = 1;
          break;
       case 'd':
          globals.debugLevel = atoi(optarg);
          break;
       case 'h':
          PrintHelp();
          return (EXIT_SUCCESS);
        case '?':
          break;
        }
    }
  if (help_flag){
    PrintHelp();
    return (EXIT_SUCCESS);
  } 
  if (listed_flag && optind < argc)
    {
      cerr << "non-option ARGV-elements: " << endl;
      string curFile;
      while (optind < argc) {
        curFile = string(argv[optind++]); 
        seq2qual[curFile] = curFile.substr(0,curFile.rfind('.'));
        struct stat stFileInfo;
        bool bInReturn;
        int intStat;
        intStat = stat(string(seq2qual[curFile] + ".QUL").c_str(),&stFileInfo);
        if (intStat == 0)
          // We were able to get the file attributes,
          // so the file exists
          continue;
        else
          seq2qual[curFile] = "";
      }
    }      
  return true;
}
  
int main(int argc, char ** argv)
{
  // Handle all initial parsing of hashmaps and temp files via options parsing
  if (! GetOptions (argc, argv)) {
    cerr << "Command line parsing failed" << endl;
    PrintHelp();
    exit(1);
  }

  if (globals.bank.length() == 0){ // No bank specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  try
    {
      if (globals.asmfile.length() > 0 || globals.ctgfile.length() > 0) {
         int maxLayout = 0;

         if (contig_stream.exists(globals.bank)) {
            cerr << "Contig Bank exists; opening" << endl;;
            contig_stream.open(globals.bank, B_WRITE);
         } else {
            cerr << "Contig Bank doesn't exist; creating" << endl;
            contig_stream.create(globals.bank, B_WRITE);
         }
         if (globals.layoutOnly) {
            if (layout_stream.exists(globals.bank)) {
               layout_stream.open(globals.bank, B_WRITE);
            }
            else {
               layout_stream.create(globals.bank, B_WRITE);
            }
            maxLayout = layout_stream.getMaxIID();
         }

         minCtgID = contig_stream.getMaxIID();
         if (maxLayout >= minCtgID) {
            minCtgID = maxLayout+1;
         };
      }
      if (read_stream.exists(globals.bank)) {
        cerr << "Read bank exists; opening" << endl;
        read_stream.open(globals.bank, B_READ | B_WRITE);
      } else {
        cerr << "Read Bank doesn't exist; creating" << endl;
        read_stream.create(globals.bank, B_READ | B_WRITE);
      }

      int maxRead = read_stream.getMaxIID();
      if (maxRead >= minSeqID) {
         minSeqID = maxRead+1;
      }
      if (frag_stream.exists(globals.bank)) {
        cerr << "frag bank exists; opening" << endl;
        frag_stream.open(globals.bank, B_WRITE);
      } else {
        cerr << "frag Bank doesn't exist; creating" << endl;
        frag_stream.create(globals.bank, B_WRITE);

        int maxFrg = frag_stream.getMaxIID();
        if (maxFrg >= minSeqID) {
           minSeqID = maxFrg + 1;
        }
      }
      if (lib_stream.exists(globals.bank)) {
        cerr << "lib bank exists; opening" << endl;
        lib_stream.open(globals.bank, B_WRITE);
      } else {
        cerr << "lib Bank doesn't exist; creating" << endl;
        lib_stream.create(globals.bank, B_WRITE);
      }
      int maxLib = lib_stream.getMaxIID();
      if (maxLib >= minSeqID) {
         minSeqID = maxLib +1;
      }

      if (globals.asmfile.length() > 0) {
         if (scf_stream.exists(globals.bank)) {
            cerr << "scf bank exists; opening" << endl;
            scf_stream.open(globals.bank, B_WRITE);
          } else {
             cerr << "scf Bank doesn't exist; creating" << endl;
             scf_stream.create(globals.bank, B_WRITE);
          }
          if (link_stream.exists(globals.bank)) {
             cerr << "link bank exists; opening" << endl;
             link_stream.open(globals.bank, B_WRITE);
          } else {
              cerr << "link Bank doesn't exist; creating" << endl;
              link_stream.create(globals.bank, B_WRITE);
          }
          if (edge_stream.exists(globals.bank)) {
             cerr << "edge bank exists; opening" << endl;
             edge_stream.open(globals.bank, B_WRITE);
          } else {
             cerr << "edge Bank doesn't exist; creating" << endl;
             edge_stream.create(globals.bank, B_WRITE);
          }
          int maxScf = scf_stream.getMaxIID();
          int maxLink = link_stream.getMaxIID();
          int maxEdge = edge_stream.getMaxIID();
          if (maxScf >= minScfID) {
             minScfID = maxScf + 1;
          }
          if (maxLink >= minScfID) { 
             minScfID = maxLink + 1; 
          }
          if (maxEdge >= minScfID) {
             minScfID = maxEdge + 1;
          }
       } 
    }
  catch (Exception_t & e)
    {    
      cerr << "Failed to open contig account in bank " << globals.bank
           << ": " << endl << e << endl;
      exit(1);
    }
 
  // figure out the reads 
  if (globals.fastafile.size() > 0) {
    cerr << "parsing fasta file" << endl;
    parseFastaFile();
    readsDone = true;
  }

  // parse fastq file
  if (globals.fastqfile.size() > 0) {
    cerr << "parsing fastq file" << endl;
    parseFastqFile();
    readsDone = true;
  }
 
  if (!readsDone && globals.frgfile.size() > 0) {
     parseFrgFile(globals.frgfile);
     readsDone = true;
  }
 
  if (! matesDone && globals.matesfile.size() > 0) { // the mate file contains either mates
      // or regular expressions defining them.
      ifstream mates(globals.matesfile.c_str());
      parseMatesFile(mates);
      mates.close();
      matesDone = true;
  }
 
  if (globals.ctgfile.size() > 0) {
    ifstream curCtg(globals.ctgfile.c_str());
    parseContigFile(curCtg);
    curCtg.close();
    contigsDone = true;
  }

  if (!contigsDone && globals.asmfile.size() > 0) {
    parseAsmFile(globals.asmfile);
    contigsDone = true;
  }

  lib_stream.close();
  if (lib_stream.isOpen()) {
     cerr << "ERROR: Lib bank is still open" << endl;
  }
  //-----------------------------------------------------------------------------
  cerr << "Closing frag stream/bank" << endl;
  frag_stream.close();
  if (frag_stream.isOpen()) {
    cerr << "ERROR: Frag bank is still open " << endl;
  }
  //------------------------------------------------------------------------------
  cerr << "Closing read stream/bank" << endl;
  read_stream.close();
  if (read_stream.isOpen()) {
    cerr << "ERROR: Read bank is still open " << endl;
  }
  if (globals.asmfile.length() > 0 || globals.ctgfile.length() > 0) {
     cerr << "Handling contigs" << endl;
     contig_stream.close();
     if (contig_stream.isOpen()) {
        cerr << "ERROR: Contig bank is still open " << endl;
     } else 
       cerr << "Contig stream closed." << endl;
   }
  if (globals.layoutOnly) {
     layout_stream.close();
  }

  if (globals.asmfile.length() > 0) {
     link_stream.close();
     edge_stream.close();
     scf_stream.close();

     if (link_stream.isOpen() || edge_stream.isOpen() || scf_stream.isOpen()) {
        cerr << "ERROR: Scaffold, Link, or Edge stream still open" << endl;
     } else {
        cerr << "Scaffold, link, and edge streams closed" << endl;
     }
  }
}

map<FILE*, bool> fileCompressed;

FILE *fileOpen(const char *name, const char *permission) {
    FILE *inFile = NULL;
    bool isCompressed = false;

   if (strcasecmp(name + strlen(name) - 3, ".gz") == 0) {
      char  cmd[1024];
      sprintf(cmd, "gzip -dc %s", name);
      errno = 0;      inFile = popen(cmd, permission);
      isCompressed = 1;
   } else if (strcasecmp(name + strlen(name) - 4, ".bz2") == 0) {
      char  cmd[1024];
      sprintf(cmd, "bzip2 -dc %s", name);
      inFile = popen(cmd, permission);
      isCompressed = 1;
   } else {
      errno = 0;
      inFile = fopen(name, permission);
   }

   fileCompressed[inFile] = isCompressed;
   return inFile;
}

void fileClose(FILE *file) {
   if (fileCompressed[file] == true) {
      pclose(file);
   } else {
      fclose(file);
   }
}

bool parseFastaFile() {
  int counter = 0;
  string tempSeqHeader;
  string tempSeqBuff;
  string tempQualHeader;
  string tempQualBuff;
  string seqname;
  int cll = -1;
  int clr = -1;
  int temp1 = 0;
  int temp2 = 0;
  int id = 0;

  FILE* fastafile = fileOpen(globals.fastafile.c_str(), "r");
  FILE* qualFile = NULL;
  if (globals.qualfile.size() > 0)
    qualFile = fileOpen(globals.qualfile.c_str(), "r");

  while (Fasta_Read(fastafile,tempSeqBuff,tempSeqHeader) != 0) {
    if (counter % PRINT_INTERVAL == 0 && globals.debugLevel > 0) {
       cerr << "Read " << counter << " reads " << endl;
    }
    counter++;
    
    stringstream qualheaderstream (stringstream::in | stringstream::out);
    stringstream seqheaderstream (stringstream::in);
    seqheaderstream.str(tempSeqHeader.c_str());
    seqheaderstream >> seqname;
    if (!seqheaderstream.eof()) {
      seqheaderstream >> temp1;
      seqheaderstream >> temp2;
      if (!seqheaderstream.eof()) {
        seqheaderstream.ignore(5, ' ');
        seqheaderstream >> cll;
        seqheaderstream >> clr;
      } else {
        cll = temp1;
        clr = temp2;
      }
    } else {
      cll = 0;
      clr = tempSeqBuff.length();
      //cerr << "Format not recognized" << endl;
    } 
 
    // So we don't overwrite an externally provided clear range
    if (cll == -1) {
      cll = 0;
      clr = tempSeqBuff.length();
    }
    Read_t read;
    read.setIID(minSeqID++);
    read.setEID(seqname);
   
    string tempheader;
    if (globals.qualfile.length() > 0) {
      Fasta_Qual_Read(qualFile,tempQualBuff,tempQualHeader);
      qualheaderstream.str(tempQualHeader.c_str());
      qualheaderstream >> tempheader;
      if (tempheader != seqname) {
        cerr << "Sequence and quality records must agree: " << seqname << " != " << tempheader << endl;
        return 1;
      }
      if (tempQualBuff.length() != tempSeqBuff.length()) {
        cerr << "Sequence and quality records must have same length for " << seqname << ": "
             << tempSeqBuff.length() << " vs " << tempQualBuff.length() << endl;
        return 1; 
      }

    } else {
      tempQualBuff.clear();
      for (int i = 0; i < cll; i++) 
        tempQualBuff += char(AMOS::MIN_QUALITY + BADQUAL);
      for (int i = cll; i < clr; i++)
        tempQualBuff += char(AMOS::MIN_QUALITY + GOODQUAL);
      for (int i = clr; i < tempSeqBuff.length(); i++)
        tempQualBuff  += char(AMOS::MIN_QUALITY + BADQUAL);
    }
    read.setClearRange(Range_t(cll, clr));
    read.setSequence(tempSeqBuff.c_str(), tempQualBuff.c_str());
    read_stream.append(read);

    // reset the clear ranges
    cll = clr = -1;
  }

  fileClose(fastafile);
  if (qualFile != NULL) {
     fileClose(qualFile);
  }
}

bool parseFastqFile() {
  int counter = 0;
  string tempSeqHeader;
  string tempSeqBuff;
  string tempQualHeader;
  string tempQualBuff;
  string seqname;
  int cll = -1;
  int clr = -1;
  int temp1 = 0;
  int temp2 = 0;
  int id = 0;

  FILE* fastqfile = fileOpen(globals.fastqfile.c_str(), "r");

  while (Fastq_Read(fastqfile,tempSeqBuff,tempSeqHeader,tempQualBuff,tempQualHeader, globals.fastqQualityType) != 0) {
    if (counter % PRINT_INTERVAL == 0 && globals.debugLevel > 0) {
       cerr << "Read " << counter << " reads " << endl;
    }
    counter++;
    
    stringstream seqheaderstream (stringstream::in);
    seqheaderstream.str(tempSeqHeader.c_str());
    seqheaderstream >> seqname;
    if (!seqheaderstream.eof()) {
      seqheaderstream >> temp1;
      seqheaderstream >> temp2;
      if (!seqheaderstream.eof()) {
        seqheaderstream.ignore(5, ' ');
        seqheaderstream >> cll;
        seqheaderstream >> clr;
      } else {
        cll = temp1;
        clr = temp2;
      }
    } else {
      cll = 0;
      clr = tempSeqBuff.length();
      //cerr << "Format not recognized" << endl;
    } 
 
    // So we don't overwrite an externally provided clear range
    if (cll == -1) {
      cll = 0;
      clr = tempSeqBuff.length();
    }
    Read_t read;
    read.setIID(minSeqID++);
    read.setEID(seqname);

    if (tempQualBuff.length() != tempSeqBuff.length()) {
        cerr << "Sequence and quality records must have same length for " << seqname << ": "
             << tempSeqBuff.length() << " vs " << tempQualBuff.length() << endl;
        return 1; 
    }

    read.setClearRange(Range_t(cll, clr));
    read.setSequence(tempSeqBuff.c_str(), tempQualBuff.c_str());
    read_stream.append(read);

    // reset the clear ranges
    cll = clr = -1;
  }
  cerr << "Finished reading " << counter << " reads " << endl;

  fileClose(fastqfile);
}


vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    return split(s, delim, elems);
}

// parse only the non-regex mates file for now
bool parseMatesFile(ifstream &matesFile) {
   string temp;
   uint32_t counter = 0;

   while (matesFile >> temp) {
    char peekChar = temp[0];
    
    if (peekChar == ' ' || peekChar == '\n' || peekChar == '\t') { // empty line
      matesFile.ignore(MAX_LINE_LEN,'\n');
      continue;
    }
    if (peekChar=='#') { // comment
      matesFile.ignore(MAX_LINE_LEN,'\n');
      continue;
    }

    if (peekChar == 'l'){
       if (temp == "library") {
          string name;
          float min, max;
          matesFile >> name;
          matesFile >> min;
          matesFile >> max;
          matesFile.ignore(MAX_LINE_LEN, '\n');

          Pos_t mean = (min + max) / 2;
          SD_t stdev = (max - min) / 6;
          Distribution_t dist;
          dist.mean = mean;
          dist.sd = stdev;

          Library_t lib;
          lib.setDistribution(dist);  
          lib.setIID(minSeqID++);
          lib.setEID(name);          
          lib_stream.append(lib);
 
          continue;
       }
    }
    if (peekChar == 'p') {
       // skip regex for now
       continue;
    }

    // now that we've handled everything else, this must be a simple pair of reads
    string frg1 = temp;
    string frg2, name;
    matesFile >> frg2;
    matesFile >> name;
    matesFile.ignore(MAX_LINE_LEN, '\n');

    stringstream templateID(stringstream::in | stringstream::out);

    Size_t offset = 0; 
    for (offset = 0; offset < frg1.length(); ++offset) {
       if (frg2.length() < offset) {
          break;
       }
       if (frg1[offset] != frg2[offset]) {
          break;
       }
    }
    if (offset == 0) {
       cerr << "Error fragments " << frg1 << " AND " << frg2 << " do not have any common template substring" << endl;
       continue;
    }
    templateID.str(frg1.substr(0, offset));
    templateID << "_" << counter;
    ID_t libID = lib_stream.lookupIID(name);
    if (libID == NULL_ID) {
      cerr << "Error library " << name << " is not defined" << endl;
      continue;
    }
    ID_t read1 = read_stream.lookupIID(frg1);
    ID_t read2 = read_stream.lookupIID(frg2);
    if (read1 == NULL_ID || read2 == NULL_ID) {
       cerr << "Error fragments " << frg1 << " AND " << frg2 << " are not defined" << endl;
       continue;
    }

    Fragment_t frag;
    frag.setLibrary(libID); 
    frag.setIID(minSeqID++);
    frag.setEID(templateID.str());
    frag.setReads(std::pair<ID_t, ID_t>(read1, read2));
    frag.setType(Fragment_t::INSERT);
    frag_stream.append(frag);

    // also update the reads
    Read_t read;
    read_stream.fetch(read1, read);
    read.setFragment(frag.getIID());
    read_stream.replace(read.getIID(), read);

    read_stream.fetch(read2, read);
    read.setFragment(frag.getIID());
    read_stream.replace(read.getIID(), read);
    counter++;
   }
}


#ifdef AMOS_HAVE_CA 
bool parseFrgFile(string fileName) {
    GenericMesg     *pmesg             = NULL;
    FILE *f = fileOpen(fileName.c_str(), "r");
    map<uint64, uint64> readToLib;
    map<uint64, ID_t> readToFrag;
    uint32_t counter = 0;

    while (EOF != ReadProtoMesg_AS(f, &pmesg)) {
    if        (pmesg->t == MESG_DST) {
        DistanceMesg *dst = (DistanceMesg *)pmesg->m;
        if (dst->action == AS_ADD) {
           Library_t lib;
           Distribution_t dist;
           dist.mean = dst->mean;
           dist.sd = dst->stddev;

           lib.setDistribution(dist);
           lib.setIID(minSeqID++);
           lib.setEID(AS_UID_toString(dst->eaccession));
           lib_stream.append(lib);
        }
    } else if (pmesg->t == MESG_LIB) {
        LibraryMesg *dst = (LibraryMesg *)pmesg->m;
        if (dst->action != AS_ADD) {
           continue;
        }
        // set unknown ones to default to iniie
        if (dst->link_orient.isUnknown()) {
           dst->link_orient.setIsInnie();
        }
        if (!dst->link_orient.isInnie()) {
             cerr << "Error, library " << AS_UID_toString(dst->eaccession) << " has orientation that is not innie. Only innie mates are currently supported" << endl;
             continue;
        }
        Library_t lib;
        Distribution_t dist;
        dist.mean = dst->mean;
        dist.sd = dst->stddev;

        lib.setDistribution(dist);
        lib.setIID(minSeqID++);
        lib.setEID(AS_UID_toString(dst->eaccession));
        lib_stream.append(lib);
    } else if (pmesg->t == MESG_FRG) {
       FragMesg *fmesg = (FragMesg *)pmesg->m;
       if (fmesg->action != AS_ADD) {
          continue;
       }

       if (AS_UID_isDefined(fmesg->library_uid) == TRUE) {
          readToLib[AS_UID_toInteger(fmesg->eaccession)] = AS_UID_toInteger(fmesg->library_uid);
       }
       Read_t read;
       read.setEID(AS_UID_toString(fmesg->eaccession));
       read.setIID(minSeqID++);
       read.setClearRange(Range_t(fmesg->clear_rng.bgn, fmesg->clear_rng.end));
       read.setSequence(fmesg->sequence, fmesg->quality);
       readToFrag[AS_UID_toInteger(fmesg->eaccession)] = 0;
       read_stream.append(read);

       if (counter % PRINT_INTERVAL == 0 && globals.debugLevel > 0) {
          cerr << "Read " << counter << " fragments" << endl; 
       }
       counter++;
    } else if (pmesg->t == MESG_LKG) {
       LinkMesg *lmesg = (LinkMesg *)pmesg->m;
       if (lmesg->action != AS_ADD) {
          continue;
       }

       ID_t read1 = read_stream.lookupIID(AS_UID_toString(lmesg->frag1));
       ID_t read2 = read_stream.lookupIID(AS_UID_toString(lmesg->frag2));
       if (read1 == NULL_ID || read2 == NULL_ID) {
          cerr << "Error fragments " << AS_UID_toString(lmesg->frag1) << " AND " << AS_UID_toString(lmesg->frag2) << " are not defined" << endl;
          continue;
       }

       // in version 1 frgs library is specified by LKG record, in version 2 it's by fragment. Figure out which one we want and use that
       string libEID = "0";
       if (AS_UID_isDefined(lmesg->distance) == TRUE) {
          libEID = AS_UID_toString(lmesg->distance);
       } else {
          if (readToLib[AS_UID_toInteger(lmesg->frag1)] != readToLib[AS_UID_toInteger(lmesg->frag1)]) {
             cerr << "Fragments " << AS_UID_toString(lmesg->frag1) << " AND " << AS_UID_toString(lmesg->frag2) << " are in different libraries" << endl;
             continue;
          }
          libEID = AS_UID_toString(AS_UID_fromInteger(readToLib[AS_UID_toInteger(lmesg->frag1)]));
       }
  
       if (libEID == "0") {
          // special case for CA with version 1 fragments
          // they end up with library 0 which equals unmated fragments so we just don't create a frg record for them
          cerr << "Skipping invalid library " << libEID << endl;
          continue;
       }

       ID_t libID = lib_stream.lookupIID(libEID);
       if (libID == NULL_ID) {
         cerr << "Error library " << libEID << " is not defined" << endl;
         continue;
       }

       Fragment_t frag;
       frag.setLibrary(libID);
       frag.setIID(minSeqID++);
       string uid = AS_UID_toString(lmesg->frag1);
       uid += "_";
       uid += AS_UID_toString(lmesg->frag2);
       frag.setEID(uid);
       frag.setReads(std::pair<ID_t, ID_t>(read1, read2));
       frag.setType(Fragment_t::INSERT);
       frag_stream.append(frag);

       // also update the reads
       Read_t read;
       read_stream.fetch(read1, read);
       read.setFragment(frag.getIID());
       readToFrag[AS_UID_toInteger(lmesg->frag1)] = frag.getIID();
       read_stream.replace(read.getIID(), read);

       read_stream.fetch(read2, read);
       read.setFragment(frag.getIID());
       readToFrag[AS_UID_toInteger(lmesg->frag2)] = frag.getIID();
       read_stream.replace(read.getIID(), read);
    } else if (pmesg->t == MESG_PLC) {
        // ignore
    } else if (pmesg->t == MESG_VER) {
        //  ignore
    } else {
        //  Ignore messages we don't understand
    }
   }

   // finally create fragments for unmated reads to associate them to a library
   for (map<uint64, ID_t>::iterator i = readToFrag.begin(); i != readToFrag.end(); ++i) {
      if (i->second == 0) {
         ID_t read1 = read_stream.lookupIID(AS_UID_toString(AS_UID_fromInteger(i->first)));
         if (read1 == NULL_ID) {
            cerr << "Cannot find record for read " << i->first << endl;
            continue;
         }

         ID_t libID;
         if (readToLib.find(i->first) == readToLib.end()) {
            continue;
         } else {
            libID = lib_stream.lookupIID(AS_UID_toString(AS_UID_fromInteger(readToLib[i->first])));
         }
         Fragment_t frag;
         frag.setLibrary(libID);
         frag.setIID(minSeqID++);
         string uid = i->first + "_frag";
         frag.setEID(uid);
         frag.setType(Fragment_t::INSERT);
         frag_stream.append(frag);

         // also update the reads
         Read_t read;
         read_stream.fetch(read1, read);
         read.setFragment(frag.getIID());
         readToFrag[i->first] = frag.getIID();
         read_stream.replace(read.getIID(), read);
      }
   }
   fileClose(f);

   cerr << "Finished reading " << counter << " fragments" << endl;
}

int *computeGapIndex(char *sequence, uint32_t length) {
   // compute the gap index for the consensus sequence
   int *gapIndex = (int*) safe_malloc(sizeof(int) * length);
   int gapCounter = 0;
   for (int i = 0; i < length; i++) {
      if (sequence[i] == GAP_CHAR) {
         gapCounter++;
       }
       gapIndex[i] = gapCounter;
   }

   return gapIndex;
}

/** 
  * Parse Celera Assembler output file
  * This assumes quality values are compatible between the two which they currently are
  * Both are offset by '0' or AMOS::MIN_QUALITY
**/
bool parseAsmFile(string fileName) {
    GenericMesg     *pmesg             = NULL;
    FILE *f = fileOpen(fileName.c_str(), "r");
    map<ID_t, uint32_t> ctgLens;
    bool readUTG = globals.readUTG == 1;
    bool readCCO = globals.readCCO == 1;
    bool readSurrogates = globals.readSurrogates == 1;
    bool storeSurrogatesAsFragments = globals.surrogateAsFragment == 1;
    bool storeLayoutOnly = globals.layoutOnly == 1;
    uint32_t counter = 0;

    while (EOF != ReadProtoMesg_AS(f, &pmesg)) {
       // utg
       if (pmesg->t == MESG_UTG) {
          SnapUnitigMesg *utg = (SnapUnitigMesg *)pmesg->m;

          if (readCCO && utg->status != AS_UNIQUE) {
             // we need to read this unitig as a read to use it in consensus potentially
             Read_t read;
             read.setIID(minSeqID++);
             read.setEID(AS_UID_toString(utg->eaccession));

             // remove gaps from sequence
             char *sequence = (char *) safe_malloc(sizeof(char) * (utg->length + 1));
             char *qual = (char *) safe_malloc(sizeof(char) * (utg->length + 1));
             int j = 0;
             for (int i = 0; i < utg->length; i++) {
                if (utg->consensus[i] != GAP_CHAR) {
                   sequence[j] = utg->consensus[i];
                   qual[j] = utg->quality[i];
                   if (j > i || j >=utg->length) {
                      AMOS_THROW("Error: The ungapped position is smaller than the gapped: " + i); 
                   }
                   j++;
                } 
             }
             sequence[j]='\0';
             qual[j] = '\0';
             read.setSequence(sequence, qual);
             read.setClearRange(Range_t(0, j));
             read_stream.append(read);
             safe_free(sequence);
             safe_free(qual);
          } 
          if (utg->status == AS_SEP && !readSurrogates && !readUTG) {
             continue;
          }
          if (utg->status != AS_SEP && !readUTG) {
             continue;
          }

          if (counter % PRINT_INTERVAL == 0 && globals.debugLevel > 0) {
             cerr << "Read " << counter << " unitigs" << endl;
          }
          counter++;

          Contig_t contig;
          vector<Tile_t> reads;
          vector<Tile_t> readsForLayout;
          
          contig.setEID(AS_UID_toString(utg->eaccession));
          contig.setIID(minCtgID++);
          contig.setSequence(utg->consensus, utg->quality);

          int *gapIndex = computeGapIndex(utg->consensus, utg->length);
          for (int i = 0; i < utg->num_frags; i++) {
             ID_t readID = read_stream.lookupIID(AS_UID_toString(utg->f_list[i].eident));
             if (readID == NULL_ID) {
                cerr << "Error fragments " << AS_UID_toString(utg->f_list[i].eident) << " are not defined" << endl;
                continue;
             }
             int start = (utg->f_list[i].position.bgn < utg->f_list[i].position.end ? utg->f_list[i].position.bgn : utg->f_list[i].position.end); 

             Tile_t tile;
             tile.source = readID;
             tile.offset = start;
             if (utg->f_list[i].position.bgn < utg->f_list[i].position.end) {
                utg->f_list[i].position.end -= utg->f_list[i].delta_length;
             } else {
                utg->f_list[i].position.bgn -= utg->f_list[i].delta_length;
             }
             tile.range.begin = utg->f_list[i].position.bgn - start;
             tile.range.end = utg->f_list[i].position.end - start;

             for (int j = 0; j < utg->f_list[i].delta_length; j++) {
                tile.gaps.push_back(utg->f_list[i].delta[j]);
             }
             reads.push_back(tile);

             // store the ungapped tile as well if we need to save the layout
             Tile_t layoutTile = tile;
             layoutTile.offset = start - gapIndex[start];
             layoutTile.gaps.clear();
             readsForLayout.push_back(layoutTile);
         }

         if (storeLayoutOnly == true) {
            Layout_t lay;
            lay.setEID(AS_UID_toString(utg->eaccession));
            lay.setIID(minCtgID++);
            lay.setTiling(readsForLayout);
            layout_stream.append(lay);
         } else  {
            contig.setReadTiling(reads);
            contig_stream.append(contig);
         }
         safe_free(gapIndex);
       } else if (pmesg->t == MESG_CCO && readCCO) {
          SnapConConMesg *ctg = (SnapConConMesg *)pmesg->m;
          Contig_t contig;
          vector<Tile_t> reads;
          vector<Tile_t> readsForLayout;
          int *gapIndex = (int*) computeGapIndex(ctg->consensus, ctg->length);

          contig.setEID(AS_UID_toString(ctg->eaccession));
          contig.setIID(minCtgID++);
          contig.setSequence(ctg->consensus, ctg->quality);
          for (int i = 0; i < ctg->num_pieces; i++) {
             ID_t readID = read_stream.lookupIID(AS_UID_toString(ctg->pieces[i].eident));
             if (readID == NULL_ID) {
                cerr << "Error fragments " << AS_UID_toString(ctg->pieces[i].eident) << " are not defined" << endl;
                continue;
             }
             Tile_t tile;
             tile.source = readID;

             int start = (ctg->pieces[i].position.bgn < ctg->pieces[i].position.end ? ctg->pieces[i].position.bgn : ctg->pieces[i].position.end);
             tile.offset = start;

             if (ctg->pieces[i].position.bgn < ctg->pieces[i].position.end) {
                ctg->pieces[i].position.end -= ctg->pieces[i].delta_length;
             } else {
                ctg->pieces[i].position.bgn -= ctg->pieces[i].delta_length;
             }
             tile.range.begin = ctg->pieces[i].position.bgn - start;
             tile.range.end = ctg->pieces[i].position.end - start;

             for (int j = 0; j < ctg->pieces[i].delta_length; j++) {
                tile.gaps.push_back(ctg->pieces[i].delta[j]);
             }

             Tile_t layoutTile = tile;
             layoutTile.offset = start - gapIndex[start];
             layoutTile.gaps.clear();
             reads.push_back(tile);
             readsForLayout.push_back(layoutTile);
         }
         for (int i = 0; i < ctg->num_unitigs; i++) {
            ID_t utgID = read_stream.lookupIID(AS_UID_toString(ctg->unitigs[i].eident));
            if (utgID == NULL_ID) {
               // skip this unitig, if its not a read its because it was not a surrogate
               continue;
            }
            Tile_t tile;

            tile.source = utgID;

            int start = (ctg->unitigs[i].position.bgn < ctg->unitigs[i].position.end ? ctg->unitigs[i].position.bgn : ctg->unitigs[i].position.end);
             tile.offset = start;

             if (ctg->unitigs[i].position.bgn < ctg->unitigs[i].position.end) {
                ctg->unitigs[i].position.end -= ctg->unitigs[i].delta_length;
             } else {
                ctg->unitigs[i].position.bgn -= ctg->unitigs[i].delta_length;
             }
             tile.range.begin = ctg->unitigs[i].position.bgn - start;
             tile.range.end = ctg->unitigs[i].position.end - start;

             for (int j = 0; j < ctg->unitigs[i].delta_length; j++) {
                tile.gaps.push_back(ctg->unitigs[i].delta[j]);
             }

             Tile_t layoutTile = tile;
             layoutTile.offset = start - gapIndex[start];
             reads.push_back(tile);
             readsForLayout.push_back(layoutTile);
         }
         if (storeLayoutOnly == true) {
            Layout_t lay;
            lay.setEID(AS_UID_toString(ctg->eaccession));
            lay.setIID(minCtgID++);
            lay.setTiling(readsForLayout);
            layout_stream.append(lay);
         } else {
            contig.setReadTiling(reads);
            contig_stream.append(contig);
         }
         ctgLens[contig.getIID()] = contig.getUngappedLength();
         safe_free(gapIndex);
       } else if (pmesg->t == MESG_CLK && readCCO && !storeLayoutOnly) {
          SnapContigLinkMesg *clk = (SnapContigLinkMesg *)pmesg->m;

          ID_t ctg1 = contig_stream.lookupIID(AS_UID_toString(clk->econtig1));
          ID_t ctg2 = contig_stream.lookupIID(AS_UID_toString(clk->econtig2));
          if (ctg1 == NULL_ID || ctg2 == NULL_ID) {
             cerr << "Error contigs " << AS_UID_toString(clk->econtig1) << " AND " << AS_UID_toString(clk->econtig2) << " are not defined" << endl;
             continue;
          }
          ContigEdge_t cte;
          ID_t iid = minCtgID++;
          stringstream uidseq(stringstream::in | stringstream::out);
          uidseq << AS_UID_toString(clk->econtig1) << "_" << AS_UID_toString(clk->econtig2) << "_" << iid;
          cte.setEID(uidseq.str());
          cte.setIID(iid);
          cte.setContigs(pair<ID_t, ID_t>(ctg1, ctg2));
          cte.setAdjacency(clk->orientation.toLetter());
          cte.setSize(clk->mean_distance);
          cte.setSD(clk->std_deviation);
          cte.setType(ContigEdge_t::MATEPAIR);

          int32_t numLinks = clk->num_contributing;
          vector<ID_t> links;
          ContigLink_t ctl;
          if (clk->overlap_type != AS_NO_OVERLAP) {
             numLinks--;
             uidseq.str("");
             uidseq << cte.getEID() << "_OVL";
             ctl.setEID(uidseq.str());
             ctl.setIID(minSeqID++);
             ctl.setContigs(pair<ID_t, ID_t>(ctg1, ctg2));
             ctl.setAdjacency(clk->orientation.toLetter());
             ctl.setSize(clk->mean_distance);
             ctl.setSD(clk->std_deviation);
             ctl.setType(ContigLink_t::OVERLAP);
             links.push_back(ctl.getIID());
             link_stream.append(ctl);
          }

          for (int i = 0; i < numLinks; i++) {
             Read_t read;
             read_stream.fetch(AS_UID_toString(clk->jump_list[i].in1), read);

             uidseq.str("");
             uidseq << cte.getEID() << "_" << i;
             ctl.setEID(uidseq.str());
             ctl.setIID(minSeqID++);
             ctl.setContigs(pair<ID_t, ID_t>(ctg1, ctg2));
             ctl.setSource(pair<ID_t, NCode_t>(read.getFragment(), Fragment_t::NCODE));
             ctl.setAdjacency(clk->orientation.toLetter());
             ctl.setSize(clk->mean_distance);
             ctl.setSD(clk->std_deviation);
             ctl.setType(ContigLink_t::MATEPAIR);
             links.push_back(ctl.getIID());
             link_stream.append(ctl);
          }
          cte.setContigLinks(links);
          edge_stream.append(cte);
       }else if (pmesg->t == MESG_SCF && readCCO && !storeLayoutOnly) {
          SnapScaffoldMesg *scf = (SnapScaffoldMesg *)pmesg->m;
          Scaffold_t scaffold;

          scaffold.setEID(AS_UID_toString(scf->eaccession));
          scaffold.setIID(minScfID++);
          vector<Tile_t> contigs;
          int32_t offset = 0;

          for (int i = 0; i < (scf->num_contig_pairs == 0 ? 1 : scf->num_contig_pairs); i++) {
             ID_t ctg1 = contig_stream.lookupIID(AS_UID_toString(scf->contig_pairs[i].econtig1));
             ID_t ctg2 = contig_stream.lookupIID(AS_UID_toString(scf->contig_pairs[i].econtig2));

             if (ctg1 == NULL_ID || ctg2 == NULL_ID) {
                cerr << "Error contigs " << AS_UID_toString(scf->contig_pairs[i].econtig1) << " AND " << AS_UID_toString(scf->contig_pairs[i].econtig2) << " are not defined" << endl;
                continue;
             }
             
             Tile_t tile;
             if (i == 0) {
                tile.source = ctg1;
                tile.offset = offset;
                if (scf->contig_pairs[i].orient.isUnknown() || scf->contig_pairs[i].orient.isNormal() || scf->contig_pairs[i].orient.isInnie()) {
                   tile.range.begin = offset;
                   tile.range.end = offset + ctgLens[ctg1];
                } else if (scf->contig_pairs[i].orient.isOuttie() || scf->contig_pairs[i].orient.isAnti()) {
                   tile.range.begin = offset + ctgLens[ctg1];
                   tile.range.end = offset;
                }
                contigs.push_back(tile);
                offset += ctgLens[ctg1];
             }
             offset += scf->contig_pairs[i].mean;

             if (ctg1 != ctg2) {
                tile.source = ctg2;
                tile.offset = offset;
                if (scf->contig_pairs[i].orient.isUnknown() || scf->contig_pairs[i].orient.isNormal() || scf->contig_pairs[i].orient.isOuttie()) {
                   tile.range.begin = offset;
                   tile.range.end = offset + ctgLens[ctg2];
                } else if (scf->contig_pairs[i].orient.isInnie() || scf->contig_pairs[i].orient.isAnti()) {
                   tile.range.begin = offset + ctgLens[ctg2];
                   tile.range.end = offset;
                }
                contigs.push_back(tile);
                offset += ctgLens[ctg2];
             } 
          }
          scaffold.setContigTiling(contigs);
          scf_stream.append(scaffold);
       }
    }

    cerr << "Finished reading " << counter << " unitigs" << endl;
}
#else
bool parseFrgFile(string fileName) {
   cerr << "Error: Celera Assembler was not available when AMOS was compiled. Please recompile AMOS with the --enable-CA option" << endl;
}
bool parseAsmFile(string fileName) {
   cerr << "Error: Celera Assembler was not available when AMOS was compiled. Please recompile AMOS with the --enable-CA option" << endl;
}
#endif // if defined CA
  
  bool parseContigFile(ifstream &curCtg) {
    Contig_t ctg;
    vector<Tile_t> reads;
    string id;

    stringstream buffer(stringstream::in | stringstream::out);
    stringstream consensus(stringstream::in | stringstream::out);
    stringstream qualseq(stringstream::in | stringstream::out);

    char curline[MAX_LINE_LEN];
    string line;
    bool first = true;
    bool readConsensus = true;
 
    while (curCtg.getline(curline, MAX_LINE_LEN, '\n')) {
      line = curline;

      if (curline[0] == '#' && curline[1] == '#') {
        if (!first) {
          ctg.setEID(id);
          ctg.setIID(minCtgID++);
          ctg.setReadTiling(reads);
          ctg.setSequence(consensus.str().c_str(), qualseq.str().c_str());
          contig_stream.append(ctg);
        }
        first = false;
        reads.clear();
        consensus.str("");
        qualseq.str("");
        id = line.substr(2, line.find_first_of(' ')-2);
        readConsensus = true;
      }
      else if (curline[0] == '#') {
        readConsensus = false;

        Tile_t read;
        string eid = line.substr(1, line.find('(')-1);
        ID_t iid = read_stream.lookupIID(eid);
        if (iid == NULL_ID) {
           cerr << "Error could not find eid " << eid << " in bank, skipping" << endl;
           continue;
        }
        read.source = iid;
        buffer.str("");
        buffer << line.substr(line.find_first_of('(')+1, line.find_first_of(')'));
        buffer >> read.offset;
        
        buffer.str("");
        buffer << line.substr(line.find_first_of('{')+1, line.find_last_of('}'));
        buffer >> read.range.begin;
        buffer >> read.range.end;
        read.range.begin--;
        read.range.end--;
        reads.push_back(read);
      }
      else if (readConsensus == true) {
         consensus << line;
         string qual(line.length(), char('0' + GOODQUAL));
         qualseq << qual;
      }    
    } // while in
   
    // output last sequence 
    if (reads.size() != 0) {
          ctg.setEID(id);
          ctg.setIID(minCtgID++);
          ctg.setReadTiling(reads);
          ctg.setSequence(consensus.str().c_str(), qualseq.str().c_str());
          contig_stream.append(ctg);
     }
} // parseContigFile
    
void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type breakPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, breakPos);

    while (string::npos != pos || string::npos != breakPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(breakPos, pos - breakPos));
        // Skip delimiters.  Note the "not_of"
        breakPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, breakPos);
    }
}

