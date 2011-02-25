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
#include <expat.h>
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
string  OPT_BankName;                       // bank name parameter
map<string, string> globals;                // Globals

Bank_t lib_stream (Library_t::NCODE);
Bank_t read_stream (Read_t::NCODE);
Bank_t contig_stream (Contig_t::NCODE);
Bank_t frag_stream (Fragment_t::NCODE);
Bank_t link_stream (ContigLink_t::NCODE);
Bank_t edge_stream (ContigEdge_t::NCODE);
Bank_t scf_stream (Scaffold_t::NCODE);

int GOODQUAL = 30;
int BADQUAL = 10;

int MAX_LINE_LEN = 256;

bool matesDone = false;
bool readsDone = false;
bool contigsDone = false;

int minSeqID = 1;
int minCtgID = 1;
// this is where all my data live

map<int,vector< string > >      insertlib;  // lib id to insert list
map<string,char>                inserttype; // the type of insert, if undefined assume 'E'
map<string,string>              seenlib;    // insert id to lib id map
map<int,string>                 seqinsert;  // sequence id to insert id map
map<int,string>                 libnames;  // lib id to lib name
map<int,int>                    posidx;     // position of sequence in pos file
map<string,string>              seq2qual; // a map of all the qual file names found.

//==============================================================================//
// Function Prototypes
//==============================================================================//
bool parseFastaFile();
bool parseTraceInfoFile(ifstream&);
bool parseMatesFile(ifstream&);
bool parseFrgFile(string);
bool parseAsmFile(string);
bool parseContigFile(ifstream&);
void Tokenize(const string& str,
                      vector<string>& tokens,
              const string& delimiters);
void textHandler(void *data, const XML_Char *s, int len);
void EndTag(void *data, const XML_Char *e1);
void StartTag(void *data, const XML_Char *e1, const XML_Char** attr);
void EndDocument(void *data, const XML_Char *e1);
void StartDocument(void *data, const XML_Char *e1);

//==============================================================================//
// Documentation
//==============================================================================//
void PrintHelp()
{
    cerr << "\n"
         << ".USAGE."
         << "  toAmos (-m mates|-x traceinfo.xml|-f frg|-acc)\n"
         << "         (-c contig|-ta tasm|-ace ace|-s fasta|-q qual)\n"
         << "         [-i insertfile | -map dstmap]\n"
         << "         [-gq goodqual] [-bq badqual]\n"
         << "         [-pos posfile] [-phd]\n"
         << ".DESCRIPTION.\n"
         << " toAmos_experimental is designed as a replacement for the current perl script.\n"
	 << " It does not cover all of the formats but will encompass the main ones. \n"
         << " It inserts the output of an assembly program into the AMOS bank.\n\n"
         << " If you simply need a program to generate assembly inputs for one the\n"
         << " AMOS-based assemblers (e.g. minimus or AMOS-cmp) use tarchive2amos.\n\n"
         << " toAmos reads the inputs specified on the command line and stores  the \n"
         << " information directly into the AMOS Bank.  The following types of \n"
         << " information can be provided to toAmos: \n"
         << "   -> Sequence and quality data (options -f, -s,  -q, -gq, or -bq) \n"
         << "   -> Library and mate-pair data (options -x, or -f) \n"
         << "   -> Contig  data (options -c, -a, -ta, or -ace) \n"
         << "   -> Scaffold data (option -a) \n"
         << ".OPTIONS. \n"
         << "  -o <outfile> - place output in <outfile> \n"
         << "  -m <matefile> - library and mate-pair information in Bambus format \n"
         << "  -x <trace.xml> - ancilliary data (library, mate-pair, clear range) \n"
         << "     in Trace Archive format \n"
         << "  -b <bank> - The location where the AMOS bank will be stored.\n"
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
 
  globals["readSurrogates"] = "N";
  globals["readCCO"] = "Y";
  globals["readUTG"] = "N";
 
  while (1)
    {
      int ch, option_index = 0;
      static struct option long_options[] = {
        {"help",      no_argument,      &help_flag, 1},
        {"list",      no_argument,      &listed_flag, 1},
        {"ace",       required_argument,         0, 'A'},
        {"bank",      required_argument,         0, 'b'},
        {"qual",      required_argument,         0, 'q'},
        {"seq",       required_argument,         0, 's'},
        {"map",       required_argument,         0, 'M'},
        {"arachne",   required_argument,         0, 'r'},
        {"gq",        required_argument,         0, 'G'},
        {"bq",        required_argument,         0, 'B'},
        {"pos",       required_argument,         0, 'p'},
        {"readSurrogates", no_argument,          0, 'S'},
        {"readCCO",   no_argument,               0, 'C'},
        {"readUTG",   no_argument,               0, 'U'},
        {0,           0,                         0, 0}
      };
      
      ch = getopt_long(argc, argv, "hlb:m:c:f:x:a:t:A:i:k:q:s:M:r:G:B:p:SCU", long_options, &option_index);
      if (ch == -1)
        break;

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
          globals["posfile"] = string(optarg);
          break;
        case 'I':
          minSeqID = atoi(optarg);
          break;
        case 'b':
          globals["bank"] = string(optarg);
          break;
        case 'm':
          globals["matesfile"] = string(optarg);
          break;
        case 'x':
          globals["traceinfofile"] = string(optarg);
          break;
        case 'c':
          globals["ctgfile"] = string(optarg);
          break;
        case 'f':
          globals["frgfile"] = string(optarg);
          break;
        case 'a':
          globals["asmfile"] = string(optarg);
          break;
        case 't':
          globals["tasmfile"] = string(optarg);
          break;
        case 'A':
          globals["acefile"] = string(optarg);
          break;
        case 'q':
          globals["qualfile"] = string(optarg);
          break;
        case 's':
          globals["fastafile"] = string(optarg);
          break;
        case 'i':
          globals["insertfile"] = string(optarg);
          break;
        case 'M':
          globals["libmap"] = string(optarg);
          break;
        case 'r':
          globals["arachne_scaff"] = string(optarg);
          break;
        case 'S':
          globals["readSurrogates"] = "Y";
          break;
        case 'U':
          globals["readUTG"] = "Y";
          globals["readCCO"] = "N";
          break;
        case 'C':
          globals["readCCO"] = "Y";
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

  if (globals.find("bank") == globals.end()){ // No bank specified
    cerr << "A bank must be specified" << endl;
    exit(1);
  }

  try
    {
      if (globals["asmfile"].length() > 0 || globals["ctgfile"].length() > 0) {
         if (contig_stream.exists(globals["bank"])) {
            cerr << "Contig Bank exists; opening" << endl;;
            contig_stream.open(globals["bank"], B_WRITE);
         } else {
            cerr << "Contig Bank doesn't exist; creating" << endl;
            contig_stream.create(globals["bank"], B_WRITE);
         }
      }
      if (read_stream.exists(globals["bank"])) {
        cerr << "Read bank exists; opening" << endl;
        read_stream.open(globals["bank"], B_READ | B_WRITE);
      } else {
        cerr << "Read Bank doesn't exist; creating" << endl;
        read_stream.create(globals["bank"], B_READ | B_WRITE);
      }
      if (frag_stream.exists(globals["bank"])) {
        cerr << "frag bank exists; opening" << endl;
        frag_stream.open(globals["bank"], B_WRITE);
      } else {
        cerr << "frag Bank doesn't exist; creating" << endl;
        frag_stream.create(globals["bank"], B_WRITE);
      }
      if (lib_stream.exists(globals["bank"])) {
        cerr << "lib bank exists; opening" << endl;
        lib_stream.open(globals["bank"], B_WRITE);
      } else {
        cerr << "lib Bank doesn't exist; creating" << endl;
        lib_stream.create(globals["bank"], B_WRITE);
      }

      if (globals["asmfile"].length() > 0) {
         if (scf_stream.exists(globals["bank"])) {
            cerr << "scf bank exists; opening" << endl;
            scf_stream.open(globals["bank"], B_WRITE);
          } else {
             cerr << "scf Bank doesn't exist; creating" << endl;
             scf_stream.create(globals["bank"], B_WRITE);
          }
          if (link_stream.exists(globals["bank"])) {
             cerr << "link bank exists; opening" << endl;
             link_stream.open(globals["bank"], B_WRITE);
          } else {
              cerr << "link Bank doesn't exist; creating" << endl;
              link_stream.create(globals["bank"], B_WRITE);
          }
          if (edge_stream.exists(globals["bank"])) {
             cerr << "edge bank exists; opening" << endl;
             edge_stream.open(globals["bank"], B_WRITE);
          } else {
             cerr << "edge Bank doesn't exist; creating" << endl;
             edge_stream.create(globals["bank"], B_WRITE);
          }
       } 
    }
  catch (Exception_t & e)
    {    
      cerr << "Failed to open contig account in bank " << globals["bank"]
           << ": " << endl << e << endl;
      exit(1);
    }
 
  // figure out the reads 
  if (globals["fastafile"].size() > 0) {
    cerr << "parsing fasta file" << endl;
    parseFastaFile();
    readsDone = true;
  }

  if (! matesDone && globals["matesfile"].size() > 0) { // the mate file contains either mates
      // or regular expressions defining them.
      ifstream mates(globals["matesfile"].c_str());
      parseMatesFile(mates);
      mates.close();

      matesDone = true;
  }
 
  if (!readsDone && globals["frgfile"].size() > 0) {
     parseFrgFile(globals["frgfile"]);
     readsDone = true;
  }
 
  if (globals["ctgfile"].size() > 0) {
    ifstream curCtg(globals["ctgfile"].c_str());
    parseContigFile(curCtg);
    curCtg.close();
    contigsDone = true;
  }

  if (!contigsDone && globals["asmfile"].size() > 0) {
    parseAsmFile(globals["asmfile"]);
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
  if (globals["asmfile"].length() > 0 || globals["ctgfile"].length() > 0) {
     cerr << "Handling contigs" << endl;
     contig_stream.close();
     if (contig_stream.isOpen()) {
        cerr << "ERROR: Contig bank is still open " << endl;
     } else 
       cerr << "Contig stream closed." << endl;
   }

  if (globals["asmfile"].length() > 0) {
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
      errno = 0;
      inFile = popen(cmd, permission);
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

  FILE* fastafile = fileOpen(globals["fastafile"].c_str(), "r");
  FILE* qualFile = NULL;
  if (globals["qualfile"].size() > 0)
    qualFile = fileOpen(globals["qualfile"].c_str(), "r");

  while (Fasta_Read(fastafile,tempSeqBuff,tempSeqHeader) != 0) {
    if (counter % 1000000 == 0) {
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
    if (globals["qualfile"].length() > 0) {
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
        tempQualBuff += char('0' + BADQUAL);
      for (int i = cll; i < clr; i++)
        tempQualBuff += char('0' + GOODQUAL);
      for (int i = clr; i < tempSeqBuff.length(); i++)
        tempQualBuff  += char('0' + BADQUAL);
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

    string templateID = "";

    if (frg1.find_last_of("_") != string::npos) {
       templateID = frg1.substr(0, frg1.find_last_of("_"));
    }
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
    frag.setEID(templateID);
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
   }
}

/*
bool parseMatesFile(ifstream &matesFile) {
  cerr << "in mates";
  vector<boost::regex> libregexp;
  vector<int> libids;
  vector< pair< boost::regex,boost::regex > > pairregexp;
  stringstream insstream;
  string insname;
  int inscount = 1;
  insstream << inscount;
  insstream >> insname;
  string temp;
  int lineNo = 0;
  while (!matesFile.eof()) {
    lineNo++;
    char peekChar = matesFile.peek();
    if (peekChar == ' ' || peekChar == '\n' || peekChar == '\t') { // empty line
      matesFile.ignore(256,'\n'); 
      continue;
    }
    if (peekChar == 'l'){
      matesFile >> temp;
      if (temp == "library") {
        typedef vector< string > split_vector_type;
        split_vector_type splitVec;
        char vectInput[256];
        matesFile.getline(vectInput, 256, '\n');
        // Line ends;
        split( splitVec, vectInput, boost::is_any_of("\t ") );
        if (splitVec.size() < 3 || splitVec.size() > 4) {
          cerr << "Only " << splitVec.size() << " fields" << endl;
          cerr << "Improperly formated line $. in " << globals["matesfile"]
               << ".\nMaybe you didn't use TABs to separate fields\n";
          continue;
        }	    
        if (splitVec.size() == 4){
          libregexp.push_back(boost::regex(splitVec[4]));
          libids.push_back(atoi(splitVec[1].c_str()));
        }
        Pos_t mean = (atoi(splitVec[2].c_str()) + atoi(splitVec[3].c_str())) / 2;
        SD_t stdev = (atoi(splitVec[3].c_str()) - atoi(splitVec[2].c_str())) / 6;
        Distribution_t dist;
        dist.mean = mean;
        dist.sd = stdev;
        libraries[splitVec[1]] = dist;
        continue;
      }
    } // if library
    else if (peekChar == 'p')
      matesFile >> temp;
      if (temp == "pair"){
      typedef vector< string > split_vector_type;
      split_vector_type splitVec;
      char vectInput[256];
      matesFile.getline(vectInput, 256, '\n');
      // Line ends;
      split( splitVec, vectInput, boost::is_any_of("\t ") );
      if (splitVec.size() != 2){
        cerr << "Improperly formated line $. in \"$matesfile\".\nMaybe you didn't use TABs to separate fields\n";
        continue;
      }
      pairregexp.push_back(pair<boost::regex,boost::regex>(boost::regex(splitVec[1]),boost::regex(splitVec[2])));
      continue;
    }
    if (peekChar=='#') { // comment
      matesFile.ignore(256,'\n');
      continue;
    }
    
    // now we just deal with the pair lines
    typedef vector< string > split_vector_type;
    split_vector_type splitVec;
    char vectInput[256];
    matesFile.getline(vectInput, 256, '\n');
    // Line break
    split( splitVec, vectInput, boost::is_any_of("\t ") );
    
    if (splitVec.size() < 1 || splitVec.size() > 2){
      cerr << "Improperly formated line in \"" << globals["matesfile"] 
           << "\".\nMaybe you didn't use TABs to separate fields\n";
      continue;
    }
    
    // make sure we've seen these sequences
    if (seqids.find(splitVec[0]) == seqids.end()){
      cerr << "Sequence " << splitVec[0] << "has no ID at line " << lineNo << " in \"" << globals["matesfile"] << "\"";
      continue;
    }
    if (seqids.find(splitVec[1]) == seqids.end()){
      cerr << "Sequence " << splitVec[1] << " has no ID at line " << lineNo << " in \" " << globals["matesfile"] << "\"";
        continue;
      }
      
      if (splitVec.size() > 1){
        //	    insertlib[splitVec[2]].push_back(insname);
        seenlib[insname] = splitVec[2];
      } else {
        cerr << insname << " has no library\n";
      }
      
      forw[insname] = seqids[splitVec[0]];
      rev[insname] = seqids[splitVec[1]];
      
      seqinsert[seqids[splitVec[0]]] = insname;
      seqinsert[seqids[splitVec[1]]] = insname;
      
      inscount++;
      insstream << inscount;
      insstream >> insname;
  } // while <IN>
  
  // now we have to go through all the sequences and assign them to
  // inserts
  map<string,int>::iterator iter;
  
  for(iter = seqids.begin(); iter != seqids.end(); iter++) {
    string nm = iter->first;
    int sid = iter->second;
    for (int r = 0; r < pairregexp.size(); r++){
      boost::regex freg = pairregexp[r].first;
      boost::regex revreg = pairregexp[r].second;
      boost::smatch results;
      cerr << "trying " << freg << " and " << revreg << " on " << nm << endl;
      if (regex_match(nm,results,freg)) {
        cerr << "got forw " << results[0];
        if (forw.find(results[1]) == forw.end()){
          forw[results[1]] = sid;
          seqinsert[sid] = results[1];
        }
        break;
      }
      if (regex_match(nm,results,revreg)) {
        cerr << "got rev " << results[1] << endl;
        if (rev.find(results[1]) == rev.end()) {
          rev[results[1]] = sid;
          seqinsert[sid] = results[1];
        }
        break;
      }
    } // for each pairreg
  } // for each seqids
  
  
  map<string,ID_t>::iterator iter2;
  for(iter2 = forw.begin(); iter != seqids.end(); iter++) {
    string ins = iter2->first;
    string seqname;
    ID_t nm = iter2->second;
    insstream << ins;
    insstream >> insname;
    if (insid.find(insname) == insid.end()){
      insid[insname] = minSeqID++;
    }
    if (seenlib.find(ins) == seenlib.end()){
      int found = 0;
      
      seqname = seqnames[nm];
      boost::smatch results;
      for (int l = 0; l < libregexp.size(); l++){
        local << "Trying " << libregexp[l] << " on " << seqname << "\n";
        if (regex_match(seqname,results,libregexp[l])){
          cerr << "found " << libids[l] << "\n";
          // insertlib{libids[l]} .= "ins ";
          seenlib[ins] = libids[l];
          found = 1;
          break;
        }
      }
      if (found == 0){
        cerr << "Cannot find library for \"" << seqname << "\"";
        continue;
      }
    }
  }

  map<string,ID_t>::iterator iter3;
  for (iter3 = rev.begin(); iter3 != rev.end(); iter3++) {
    string nm = iter3->first;
    ID_t ins = iter3->second;
    insstream << ins;
    insstream >> insname;
    boost::smatch results;
    if (insid.find(insname) == insid.end()){
      insid[insname] = minSeqID++;
    }
    if (seenlib.find(insname) == seenlib.end()){
      int found = 0;
      string nm = seqnames[atoi(nm.c_str())];
      
      for (int l = 0; l < libregexp.size(); l++){
        local << "Trying " << libregexp[l] << " on " << nm << endl;
        if (regex_match(nm,results,libregexp[l])){
          local << "found " << libids[l] << endl;
          // insertlib{libids[l]} .= "ins ";
          seenlib[insname] = libids[l];
          found = 1;
          break;
        }
      }
      if (found == 0){
        cerr << "Cannot find library for \"nm\"";
        continue;
      }
    }
  }
} // parseMateFile;
*/

#ifdef AMOS_HAVE_CA 
bool parseFrgFile(string fileName) {
    GenericMesg     *pmesg             = NULL;
    FILE *f = fileOpen(fileName.c_str(), "r");
    map<string, string> readToLib;

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
          readToLib[AS_UID_toString(fmesg->eaccession)] = AS_UID_toString(fmesg->library_uid);
       }
       Read_t read;
       read.setEID(AS_UID_toString(fmesg->eaccession));
       read.setIID(minSeqID++);
       read.setClearRange(Range_t(fmesg->clear_rng.bgn, fmesg->clear_rng.end));
       read.setSequence(fmesg->sequence, fmesg->quality);
       read_stream.append(read);
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
          if (readToLib[AS_UID_toString(lmesg->frag1)] != readToLib[AS_UID_toString(lmesg->frag1)]) {
             cerr << "Fragments " << AS_UID_toString(lmesg->frag1) << " AND " << AS_UID_toString(lmesg->frag2) << " are in different libraries" << endl;
             continue;
          }
          libEID = readToLib[AS_UID_toString(lmesg->frag1)];
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
       read_stream.replace(read.getIID(), read);

       read_stream.fetch(read2, read);
       read.setFragment(frag.getIID());
       read_stream.replace(read.getIID(), read);
    } else if (pmesg->t == MESG_PLC) {
        // ignore
    } else if (pmesg->t == MESG_VER) {
        //  ignore
    } else {
        //  Ignore messages we don't understand
    }
   }
   fileClose(f);
}

bool parseAsmFile(string fileName) {
    GenericMesg     *pmesg             = NULL;
    FILE *f = fileOpen(fileName.c_str(), "r");
    map<ID_t, uint32_t> ctgLens;
    bool readUTG = globals["readUTG"] == "Y";
    bool readCCO = globals["readCCO"] == "Y";
    bool readSurrogates = globals["readSurrogates"] == "Y";

    while (EOF != ReadProtoMesg_AS(f, &pmesg)) {
       // utg
       if (pmesg->t == MESG_UTG && (readUTG || readSurrogates)) {
          SnapUnitigMesg *utg = (SnapUnitigMesg *)pmesg->m;
          if (utg->status == AS_SEP && !readSurrogates) {
             continue;
          }
          if (utg->status != AS_SEP && !readUTG) {
             continue;
          }

          Contig_t contig;
          vector<Tile_t> reads;
          
          contig.setEID(AS_UID_toString(utg->eaccession));
          contig.setIID(minSeqID++);
          contig.setSequence(utg->consensus, utg->quality);

          for (int i = 0; i < utg->num_frags; i++) {
             ID_t readID = read_stream.lookupIID(AS_UID_toString(utg->f_list[i].eident));
             if (readID == NULL_ID) {
                cerr << "Error fragments " << AS_UID_toString(utg->f_list[i].eident) << " are not defined" << endl;
                continue;
             }
             Tile_t tile;
             tile.source = readID;
             int start = (utg->f_list[i].position.bgn < utg->f_list[i].position.end ? utg->f_list[i].position.bgn : utg->f_list[i].position.end);
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
         }
         contig.setReadTiling(reads);
         contig_stream.append(contig);
       } else if (pmesg->t == MESG_CCO && readCCO) {
          SnapConConMesg *ctg = (SnapConConMesg *)pmesg->m;
          Contig_t contig;
          vector<Tile_t> reads;

          contig.setEID(AS_UID_toString(ctg->eaccession));
          contig.setIID(minSeqID++);
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
             reads.push_back(tile);
         }
         contig.setReadTiling(reads);
         ctgLens[contig.getIID()] = contig.getUngappedLength();
         contig_stream.append(contig);
       } else if (pmesg->t == MESG_CLK && readCCO) {
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
       }else if (pmesg->t == MESG_SCF && readCCO) {
          SnapScaffoldMesg *scf = (SnapScaffoldMesg *)pmesg->m;
          Scaffold_t scaffold;

          scaffold.setEID(AS_UID_toString(scf->eaccession));
          scaffold.setIID(minSeqID++);
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

/*
// MATES PARSING FUNCTIONS

// parse Trace Archive style XML files
string tag;
string library;
string templateID;
Pos_t clipl;
Pos_t clipr;
Pos_t mean;
SD_t stdev;
string end;
int seqId;

bool parseTraceInfoFile(ifstream& xmlFile) {
  XML_Parser p = XML_ParserCreate(NULL);
  if (! p) {
    cerr << "Couldn't allocate memory for parser!" << endl;
    exit(-1);
  }
  
  XML_UseParserAsHandlerArg(p);
  XML_SetElementHandler(p, StartTag, EndTag);
  XML_SetCharacterDataHandler(p, textHandler);
  char Buff[512];
  for (;;) {
    int done;
    int len;
    xmlFile.getline(Buff, sizeof(Buff), '\n');
    len = strlen(Buff);
    if (ferror(stdin))
      cerr << "Read error" << endl;
    done = xmlFile.eof();
    if (! XML_Parse(p, Buff, len, done)) {
      cerr << "Parse error at line " << XML_GetCurrentLineNumber(p) << XML_ErrorString(XML_GetErrorCode(p)) << endl;
      exit(-1);
    }
    
    if(done)
      break;
  }
}


//---------------------------------------------------------------
// XML parser functions
//-------------------------------------------------------------

void StartTag(void *data, const XML_Char *e1, const XML_Char **attr)
{
  tag = string(e1);
    
    if (tag == "trace"){
      library = "";
      templateID = "";
      clipl = -1;
      clipr = -1;
      mean = -1;
      stdev = 0;
      end = "";
      seqId = -1;
    }
}


void EndTag(void *data, const XML_Char *e1)
{
  tag = string(e1);
  if (tag == "trace"){
    if (seqId == -1){
      cerr << "trace has no name???" << endl;
    }
    if (library == ""){
      cerr << "trace " << seqId << " has no library" << endl;
    }
    if (mean == -1){
      cerr << "library " << library << " has no mean" << endl;
    } 
    
      if (stdev == 0){
      cerr << "library " << library << " has no stdev" << endl;
    }
    
    if (mean != -1 && stdev != 0){
      Distribution_t newDist;
      newDist.mean = mean;
      newDist.sd = stdev;
      libraries[library] = newDist;
    }
    if (templateID == ""){
      cerr << "trace " << seqId << " has no template" << endl;
    } 
    
    if (end == "") {
      cerr << "trace " << seqId << " has no end\n";
    }
    
    if (end[0] == 'R'){
      if (rev.find(templateID) == rev.end() ||
          seqnames[seqId].size() > seqnames[rev[templateID]].size())
        {
          rev[templateID] = seqId;
        }
    }
    
    if (end[0] == 'F'){
      if (forw.find(templateID) != forw.end() ||
          seqnames[seqId] > seqnames[forw[templateID]]){
        forw[templateID] = seqId;
      }
    }
    
    seqinsert[seqId] = templateID;
    //	insertlib{library} .= "template ";
    seenlib[templateID] = library;
    //    cerr << "seen library: " << templateID << " = " << library;
    if (clipl != -1 && clipr != -1){
      seq_range[seqId] = Range_t(clipl,clipr);
    }
  }
  tag = "";
}


void textHandler(void *data, const XML_Char *s, int len)
{
  string text(s,len);
  if (tag != ""){
    if (tag == "insert_size"){
      mean = atoi(text.c_str());
      //      cerr << "mean: " << mean << endl;
    } else if (tag == "insert_stdev"){
      stdev = atoi(text.c_str());
      //      cerr << "stdev: " << stdev << endl;
    } else if (tag == "trace_name"){
      string seqName = text;
      //      cerr << "seqName: " << seqName << endl;
      seqId = minSeqID++;
      // cerr << "seqID: " << seqId << endl;
      seqids[seqName] = seqId;
      seqnames[seqId] = seqName;
    } else if (tag == "library_id"){
      library = text;
      //cerr << "library: " << library;
    } else if (tag == "seq_lib_id") {
      if (library == "") {
        library = text;
      }
    } else if (tag == "template_id"){
      templateID = text;
      //cerr << "templateID: " << templateID << endl;
    } else if (tag == "trace_end"){
      end = text;
      //      cerr << "trace end: " << end << endl;
    } else if (tag == "clip_quality_left" ||
               tag == "clip_vector_left"){
      if (clipl == -1 || atoi(text.c_str()) > clipl){
        clipl = atoi(text.c_str());
      }
    } else if (tag == "clip_quality_right" ||
             tag == "clip_vector_right"){
      if (clipr == -1 || atoi(text.c_str()) < clipr){
        clipr = atoi(text.c_str());
      }
    }
  }
}
*/
