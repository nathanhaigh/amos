///////////////////////////////////////////////////////////////////////////////
//! \file
//! \author John T Murray
//! \date 06/24/04
//!
//! \brief Directly places files in multiple formats into the AMOS Bank
//!
//!
//!
//!
//!
////////////////////////////////////////////////////////////////////////////////

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <set>
#include <list>
#include <string>
#include <functional>
#include <fasta.hh>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "foundation_AMOS.hh"
#include <Contig_AMOS.hh>
#include <fstream>
#include <iostream>
#include <expat.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>
#include "amp.hh"
#include "boost/regex.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
using namespace AMOS;
using namespace std;
using namespace HASHMAP;
//typedef match_results<string::const_iterator> smatch;


//==============================================================================//
// Globals
//==============================================================================//
string  OPT_BankName;                       // bank name parameter
string  OPT_ContigFileName;                 // Name of
map<string, string> globals;                // Globals
BankStreamSet_t bnks;                       // All the banks;

BankStream_t lib_stream (Library_t::NCODE);
BankStream_t read_stream (Read_t::NCODE);
BankStream_t contig_stream (Contig_t::NCODE);
BankStream_t frag_stream (Fragment_t::NCODE);
//BankStream_t scaffold_stream (Scaffold_t::NCODE);

ofstream TMPSEQ;
ofstream TMPCTG;
ofstream local;
ofstream TMPSCF;
ifstream TMPSEQin;
ifstream TMPCTGin;
ifstream TMPSCFin;

int GOODQUAL = 30;
int BADQUAL = 10;
bool byaccession = false;
bool phd_opt = false;
bool arachne_scaff = false;
bool scaffile = false;

bool matesDone = false;
bool readsDone = false;
int minSeqID = 1;
int minCtgID = 1;
// this is where all my data live

//==============================================================================//
// Hashmaps
//==============================================================================//
map<int,int>                    contigs;    // contig ids to contig length map
map<string,int>                 seqids;     // seq_name to seq_id map
map<int,string>                 seqnames;   // seq_id to seq_name map
map<int,int>                    seqcontig;  // seq id to contig map
map<string, int>                insid;
map<int,vector<int> >           contigseq;  //contig id to sequence id list 
map<int,Range_t>                seq_range;  //seq id to clear range left
map<int,Range_t>                asm_range;  // seq id to range within contig
map<int,string>                 contigcons; // contig id to consensus sequence
map<int,string>                 ctgnames;   // ctg id to ctg name
map<string,int>                 ctgids;     // ctg name to ctg id
map<string,ID_t>                forw;
map<string,ID_t>                rev;
map<string,Distribution_t>      libraries;  // libid to lib range (mean, stdev) mapping
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
bool parseACEFile();
bool parseFastaFile(ifstream&, ifstream&);
bool parseTraceInfoFile(ifstream&);
bool parseMatesFile(ifstream&);
bool parseFrgFile();
bool parsePosFile();
bool parseInsertFile(ifstream &insertfile);
bool parseAsmFile();
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
        {"scaff",     required_argument,         0, 'C'},
        {"gq",        required_argument,         0, 'G'},
        {"bq",        required_argument,         0, 'B'},
        {"pos",       required_argument,         0, 'p'},
        {0,           0,                         0, 0}
      };
      
      ch = getopt_long(argc, argv, "hlb:m:c:f:x:a:t:A:i:k:q:s:M:r:C:G:B:p:", long_options, &option_index);
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
        case 'C':
          globals["scaffile"] = string(optarg);
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
  local.open("./temp.log");
  TMPSEQ.open("./temp.seq");
  if (!TMPSEQ.is_open()){
    cerr << "Error opening temp seq file" << endl;
    return 1;
  }
  TMPCTG.open("./temp.ctg");
  if (!TMPCTG.is_open()){
    cerr << "Error opening temp ctg file" << endl;
    return 1;
  }

  TMPSCF.open("./temp.scf");
  if (!TMPSCF.is_open()){
    cerr << "Error opening temp scf file" << endl;
    return 1;
  }
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
    /*
    // Figure out the mates
    if (globals["frgfile"].size() > 0) {
      
      ifstream curFile(globals["frgfile"].c_str());
      streambuf* strm_buffer = cin.rdbuf();
      cin.rdbuf (curFile.rdbuf());
      parseFrgFile();
      cin.rdbuf (strm_buffer);
    }
    */
    // Figure out the mates
  if (globals["fastafile"].size() > 0) {
    cerr << "parsing fasta file" << endl;
    ifstream curSeq(globals["fastafile"].c_str());
    ifstream curQual(globals["qualfile"].c_str());
    if (globals["qualfile"].size() > 0) {
      cerr << "parsing qual file " << endl;
      curQual.open(globals["qualfile"].c_str());
    }
    else  {
      cerr << "Outputting qual to null" << endl;
      return 0;
    }
    parseFastaFile(curSeq, curQual);
    curSeq.close();
    curQual.close();
    readsDone = true;
  }
  
  if (globals["ctgfile"].size() > 0) {
    ifstream curCtg(globals["ctgfile"].c_str());
    parseContigFile(curCtg);
    curCtg.close();
  }

  if (globals["acefile"].size() > 0) {
      parseACEFile();
      /*if(phd_opt) {
        parsePHDfiles();
      }*/
    }
          

  // now it's time for library and mates information
    if (globals["traceinfofile"].size() > 0) {
      cerr << "Parsing trace info file " << endl;
      matesDone = true;
      ifstream curTrace(globals["traceinfofile"].c_str());
      parseTraceInfoFile(curTrace);
      curTrace.close();
    }
    /*    if (! matesDone && globals["matesfile"].size() > 0) { // the mate file contains either mates
      // or regular expressions defining them.
      ifstream mates(globals["matesfile"].c_str());
      parseMatesFile(mates);
      mates.close();
      }
    if (globals["insertfile"].size() > 0) {
      ifstream insertfile(globals["insertfile"].c_str());
            parseInsertFile(insertfile);
      insertfile.close();
    }
  if (globals["libmap"].size() > 0) {
  ifstream libmapfile(globals["libmap"].c_str());
  parseLibMapFile(libmapfile);
  libmapfile.close();
  }
  
  if (globals["arachne_scaff"].size() > 0) {
  ifstream arachnefile(globals["arachne_scaff"].c_str());
  parseArachneScaff(arachnefile);
  arachnefile.close();
  }
  
  if (globals["scaffile"].size() > 0) {
  ifstream scaffile(globals["scaffile"].c_str());
  parseScaff(scaffile);
  scaffile.close();
  }  
 */              
  
  try
    {
      if (contig_stream.exists(globals["bank"])) {
        cerr << "Contig Bank exists; opening" << endl;;
        contig_stream.open(globals["bank"], B_WRITE);
      } else {
        cerr << "Contig Bank doesn't exist; creating" << endl;
        contig_stream.create(globals["bank"], B_WRITE);
      }
      if (read_stream.exists(globals["bank"])) {
        cerr << "Read bank exists; opening" << endl;
        read_stream.open(globals["bank"], B_WRITE);
      } else {
        cerr << "Read Bank doesn't exist; creating" << endl;
        read_stream.create(globals["bank"], B_WRITE);
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
      //scaffold_stream.create(globals["bank"], B_WRITE);
    }
  catch (Exception_t & e)
    {     
      cerr << "Failed to open contig account in bank " << globals["bank"]
           << ": " << endl << e << endl;
      exit(1);
    }

  cerr << "flushing file inputs" << endl;
  TMPSEQ.close();
  TMPCTG.close();
  TMPSCF.close();

  // Open files for final parsing.
  TMPSEQin.open("temp.seq");
  TMPCTGin.open("temp.ctg");
  TMPSCFin.open("temp.scf");
  // Make sure all reads have an insert
  //-----------------Ensure reads have insert-------------------------------------
  map<string,int> libid;
  int lli = minSeqID++;
  char llc[50];
  sprintf(llc,"%d",lli);
  string ll(llc);
  Distribution_t newDist;
  newDist.mean = 0;
  newDist.sd = 0;
  libraries[ll] = newDist;
  libnames[lli] = "unmated";
  //  libid[ll] = lli;
  string sname;
  map<int,string>::iterator it;
  for (it = seqnames.begin(); it != seqnames.end(); it++) {
    // If there is no seqinsert, create one.
    if (seqinsert.find(it->first) == seqinsert.end()) {
      int id = minSeqID++;
      sprintf(llc,"%d",id);
      seqinsert[it->first] = llc;
      insid[llc] = id;
      seenlib[llc] = ll;
      forw[llc] = it->first;
    }
  }

  cerr << "generated by " << argv[0] << "\n";
  //-------------------Compile Libraries-----------------------------------------
  map<string, Distribution_t>::iterator lit;  
  Library_t newLib;
  Distribution_t newDis;
  stringstream temp;
  int n=0;
  for (lit = libraries.begin(); lit != libraries.end(); lit++) {
    libid[lit->first] = minSeqID++;
    //    cerr << "Seen " << lit->first << endl;
    newLib.setIID(libid[lit->first]);
    //    cerr << "Setting new lib iid: " << libid[lit->first] << endl;
    if (libnames.find(libid[lit->first]) != libnames.end())
      newLib.setEID(libnames[libid[lit->first]]);
    else {
      newLib.setEID(lit->first);
    }
    newLib.setDistribution(lit->second);
    lib_stream << newLib;
    n++;
  }
  lib_stream.close();
  if (lib_stream.isOpen()) {
    cerr << "ERROR: Frag bank is still open " << endl;
    return 1;
  }
  //-----------------------------------------------------------------------------
  cerr << n << " Libraries written" << endl;

  //-------------Write Frags-----------------------------------------------------
  map<string, string>::iterator fit;  
  for (fit = seenlib.begin(); fit != seenlib.end(); fit++) {
    //    cerr << "Seen library loop: " << fit->first << ", " << fit->second << endl;
    Fragment_t newFrag;
    stringstream tmpStr;
    tmpStr << minSeqID;
    tmpStr >> insid[fit->first];
    newFrag.setIID(minSeqID++);
    newFrag.setEID(fit->first);
    if (libid.find(fit->second) == libid.end()) {
      cerr << "Have not seen library " << fit->second << " yet: possible error in input\n";
      return 1;
    }
    newFrag.setLibrary(libid[fit->second]);
    if (forw.find(fit->second) != forw.end() && rev.find(fit->second) != rev.end())
      newFrag.setReads(std::pair<ID_t,ID_t>(forw[fit->second], rev[fit->second]));
    if ((inserttype.find(fit->first) != inserttype.end()) && inserttype[(*fit).first] == 'T')
      newFrag.setType('T');
    else 
      newFrag.setType('I');
    frag_stream << newFrag;
  }
  //-----------------------------------------------------------------------------
  cerr << "Closing frag stream/bank" << endl;
  frag_stream.close();
  if (frag_stream.isOpen()) {
    cerr << "ERROR: Frag bank is still open " << endl;
    return 1;
  }
  //------------------Write Reads------------------------------------------------
  cerr << "handling reads" << endl;
  string curline;
  int lineNo = 0;
  int rid;
  cerr << "Current state: " << TMPSEQin.eof() << endl;
  while (!TMPSEQin.eof()) {
      Read_t newRead;
      if(TMPSEQin.peek() == '\n')
        TMPSEQin.ignore(1, '\n');
      if(TMPSEQin.peek() == '#') {
        TMPSEQin.ignore(1, '\n');
        TMPSEQin >> rid;
        //        cerr << "Pulled " << rid;
        newRead.setIID(rid);
        newRead.setEID(seqnames[rid]);
        string curseq;
        string fnseq;
        TMPSEQin.ignore(256, '\n');
        while (TMPSEQin.peek() != '#') {
          getline(TMPSEQin, curseq, '\n');
          lineNo++;
          fnseq += curseq;
        }
        TMPSEQin.ignore(256, '\n'); // ignore the '#'
        string curqual;
        string fnqual;
        while (TMPSEQin.peek() != '#') {
          getline(TMPSEQin, curqual, '\n');
          lineNo++;
          fnqual += curqual;
        }
        TMPSEQin.ignore(256, '\n');
        newRead.setSequence(fnseq, fnqual);
        /*
        if (globals["posfile"].size() > 0)
          if (posidx[rid] != 0) {
            POS.seekg(posidx[rid], ios::beg);
            POS.getline(line, 79);
            string seqname = strtok(line,"\t");
            string ver = strtok(NULL,"\t");
            string poss = strtok(NULL,"\t");
            */
        if (seqinsert.find(rid) == seqinsert.end()) {
          cerr << "Cannot find insert for " << rid << " (" << seqnames[rid] << ") " << endl;
          return 1;
        }
        if (insid.find(seqinsert[rid]) == insid.end()) {
          cerr << "Cannot find insert id for insert " << seqinsert[rid] 
               << " sequence " << rid << ", " << seqnames[rid] << endl;
          return 1;
        }
        newRead.setFragment(insid[seqinsert[rid]]);
        newRead.setClearRange(seq_range[rid]);
      } else {
        if (TMPSEQin.ignore(256, '\n') && !TMPSEQin.eof()) {
          cerr << "Weird error at line " << lineNo << " in temp.seq" << endl;
          cerr << "Last rid: " << rid << endl;
          return 1;
        }
        else {
          break;
        }
      }
      read_stream << newRead;
  }
  //cerr << "Finished reads" << endl;
  //system("rm ./temp.seq");
  //------------------------------------------------------------------------------
  cerr << "Closing read stream/bank" << endl;
  read_stream.close();
  if (read_stream.isOpen()) {
    cerr << "ERROR: Read bank is still open " << endl;
    return 1;
  }
  TMPSEQin.close();
  cerr << "Handling contigs" << endl;
  //-----------Write Contigs------------------------------------------------------
  Contig_t curContig;
  int numCtg = 0;
  lineNo = 0;
  int cid;
  Status_t sts;
  while (!TMPCTGin.eof()) {
    if (TMPCTGin.peek() == '#') {
      TMPCTGin.ignore(1);
      TMPCTGin >> cid;
      Status_t sts;
      TMPCTGin >> sts;
      curContig.setIID(cid);

      curContig.setStatus(sts);
      if (ctgnames.find(cid) != ctgnames.end()){
        curContig.setEID(ctgnames[cid]);
      }
      string sequence, quality;
      TMPCTGin.ignore(256, '\n');
      lineNo++;
      while(TMPCTGin.peek() != '#') {
        TMPCTGin >> curline;
        TMPCTGin.ignore(256, '\n');
        lineNo++;
        sequence += curline;
      }
      TMPCTGin.ignore(256, '\n'); // skip the //
      lineNo++;
      while(TMPCTGin.peek() != '#') {
        getline(TMPCTGin, curline, '\n');
        lineNo++;
        quality += curline;
      }
      curContig.setSequence(sequence,quality);
      vector<Tile_t> tiles;
      Tile_t curTile;
      while(TMPCTGin.peek() == '#') {
        curTile.clear();
        TMPCTGin.ignore(1); // Get rid of '#' for stream operations
        int rid, len, ren, cl, cr;
        if (TMPCTGin.peek() != '\n') {
          TMPCTGin >> rid;
          curTile.source = rid;
          curTile.range = asm_range[rid];
          Range_t temp = seq_range[rid];
          if (seq_range.find(rid) == seq_range.end()) {
            cerr << "Warning: No clear range for read " << rid << endl;
            continue;
          }
          if (asm_range.find(rid) == asm_range.end()) {
            cerr << "Warning: No asm range for read " << rid << endl;
            continue;
          }
          int tmp;
          if (curTile.range.isReverse()) {
            curTile.range.swap();
            temp.swap();
          }
          curTile.offset = curTile.range.begin;
          curTile.range = temp;
          string deltastring, tempstr;
          TMPCTGin.ignore(256,'\n'); // ignore line break;
          while (TMPCTGin.peek() != '#' && !TMPCTGin.eof()) {
            getline(TMPCTGin, tempstr, '\n');
            lineNo++;
            deltastring += tempstr;
          }
          Pos_t curPos = 0;
          stringstream deltastream;
          deltastream.str(deltastring);
          while (deltastream >> curPos) {
            curTile.gaps.push_back(curPos);
          }
          tiles.push_back(curTile);
          lineNo++;
        } else {
          TMPCTGin.ignore(256, '\n');
          break;
        }
      }
      //cerr << "Finished tiles; moving to next Contig" << endl;
      curContig.setReadTiling(tiles);
      contig_stream << curContig; 
      numCtg++;
    } else {
      if (lineNo == 0) {
        cerr << "Premature ending of contig file; assuming no contigs..." << endl;;
      } else {
        cerr << "Weird error at line " << lineNo << " in temp.ctg, after ";
        cerr << numCtg << " contigs, last ID: " << rid << endl;
      }
    }
  }
  cerr << "Finished handling contigs, " << numCtg <<  " handled: " << endl;
  contig_stream.close();
  if (contig_stream.isOpen()) {
    cerr << "ERROR: Contig bank is still open " << endl;
    return 1;
  } else 
    cerr << "Contig stream closed." << endl;
  TMPCTGin.close();
}


// # Bambus/AMOS .scaff file
// void parseScaff()
// {
//     my $IN = shift;

//     # format of scaff file
//     #>scaffid numcontigs scaffbases scaffspan
//     #contigid orientation contiglen gapsize

//     # note gapsize is gap after contig

//     my $ps = new AMOS::ParseFasta($IN, ">", "\n");
//     while (my ($head, $data) = $ps->getRecord()){
// 	$head =~ /(\S+).*/;
// 	my $scfnam = $1;
// 	my $scfid = $minSeqId++;

// 	print TMPSCF "{SCF\n";
// 	print TMPSCF "iid:$scfid\n";
// 	print TMPSCF "eid:$scfnam\n";

// 	my $offset = 0;
	
// 	my @lines = split('\n', $data);
// 	for (my $i = 0; $i <= $#lines; $i++){
// 	    my @fields = split(/\s+/, $lines[$i]);
	    
// 	    if (! exists $ctgids{$fields[0]}) {
// 		$base->logError("Cannot find id for contig $fields[0]\n");
// 		next;
// 	    }

// 	    # print contig tile
// 	    print TMPSCF "{TLE\n";
// 	    print TMPSCF "src:$ctgids{$fields[0]}\n";
// 	    print TMPSCF "off:$offset\n";
// 	    if ($fields[1] eq "BE"){
// 		print TMPSCF "clr:0,$contigs{$ctgids{$fields[0]}}\n";
// 	    } else {
// 		print TMPSCF "clr:$contigs{$ctgids{$fields[0]}},0\n";
// 	    }
// 	    print TMPSCF "}\n";
// 	    $offset += $contigs{$ctgids{$fields[0]}} + $fields[3];
// 	}

// 	print TMPSCF "}\n";
//     }

// } # parseScaff

bool parseFastaFile(ifstream& seq, ifstream& qual) {
  string qh;
  string tempSeqHeader;
  string tempSeqBuff;
  string tempQualHeader;
  string tempQualBuff;
  Read_t read;
  Scaffold_t scaffold;
  vector<Tile_t> tiling;
  int count = 0;
  int curQual;
  string seqname;
  int cll = -1;
  int clr = -1;
  int temp1 = 0;
  int temp2 = 0;
  int id = 0;
  FILE* fastafile = fopen(globals["fastafile"].c_str(), "r");
  FILE* qualFile;
  if (globals["qualfile"].size() > 0)
    qualFile = fopen(globals["qualfile"].c_str(), "r");
  while (Fasta_Read(fastafile,tempSeqBuff,tempSeqHeader) != 0) {
    stringstream qualheaderstream (stringstream::in | stringstream::out);
    stringstream seqstream (stringstream::in | stringstream::out);
    seqstream.str(tempSeqBuff.c_str());
    stringstream seqheaderstream (stringstream::in);
    seqheaderstream.str(tempSeqHeader.c_str());
    seqheaderstream >> seqname;
    if (!seqstream.eof()) {
      seqheaderstream >> temp1;
      seqheaderstream >> temp2;
      if (!seqstream.eof()) {
        seqheaderstream.ignore(5, ' ');
        seqheaderstream >> cll;
        seqheaderstream >> clr;
      } else {
        cll = temp1;
        clr = temp2;
      }
    } else
      cerr << "Format not recognized" << endl;
    
    // so we don't overwrite an externally provided clear range
    if (seq_range.find(id) == seq_range.end()) {
      seq_range[id] = Range_t(cll, clr);
    }
    else
      {
        cll = seq_range[id].getBegin();
        clr = seq_range[id].getEnd();
      }
   
    // So we don't overwrite an externally provided clear range
    if (cll == -1) {
      cll = 0;
      clr = tempSeqBuff.length();
    }
    
    id = minSeqID++;
    seqnames[id] = seqname;
    //    cerr << "got " << seqname << id << endl;
    seqids[seqname] = id;
    
    TMPSEQ << "#" << id << endl;
    while(!seqstream.eof()) {
      char buff[61];
      seqstream.get(buff, 61);
      TMPSEQ << buff << endl;
    }
    TMPSEQ << "#" << endl;
    vector<char> quals;
    string buf; // Have a buffer string
    string qualdata;
    string tempheader;
    if (globals["qualfile"].length() > 0) {
      Fasta_Qual_Read(qualFile,tempQualBuff,tempQualHeader);
      qualheaderstream.str(tempQualHeader.c_str());
      qualheaderstream >> tempheader;
      if (tempheader != seqname) {
        cerr << "Sequence and quality records must agree: " << seqname << " != " << tempheader << endl;
        return 1;
      }
      stringstream qualstream(stringstream::in | stringstream::out);
      if (tempQualBuff.length() != tempSeqBuff.length()) {
        cerr << "Sequence and quality records must have same length for " << seqname << ": "
             << tempSeqBuff.length() << " vs " << tempQualBuff.length() << endl;
        return 1; 
      }

    } else {
      for (int i = 0; i < cll; i++) 
        tempQualBuff += char('0' + BADQUAL);
      for (int i = cll; i < clr; i++)
        tempQualBuff += char('0' + GOODQUAL);
      for (int i = clr; i < tempSeqBuff.length(); i++)
        tempSeqBuff  += char('0' + BADQUAL);
    }
    char buffer[61];
    int size = tempQualBuff.size();
    for (int i = 0; i <= size; i += 60) {
      char buff[61];
      seqstream.get(buff, 60);
      int j = (i+60 > size) ? size - i : 60;
      TMPSEQ << tempQualBuff.substr(i, j) << endl;
    }
    TMPSEQ << "#" << endl;
  }
}
/*
// LIBRARY NAME PARSING
bool parseInsertFile(ifstream &insertfile) {
  char curline[256];
  //  boost::smatch results;
  //  boost::regex libexp("GenomicLibrary Id=\"(\\S+)\" acc=\"(\\d+)\"");
  while (insertfile.getline(curline, '\n')){
    //    if (boost::regex_match(string(curline),results,libexp)){
    //libnames[atoi(results[2].str().c_str())] = results[1];
    //      cerr << "lib-id = " << results[2] << "; lib-name = " << results[1] << endl;
    }
  }
} // parseInsertFile
*/

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    return split(s, delim, elems);
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

  /*
    bool parseFrgFile() {
    map<int,Library_t> seqlibrary;
    map liborientation;
    while 
    }*/
  
  bool parseContigFile(ifstream &curCtg) {
    
    string ctg; 
    int len;
    string sname;
    Pos_t alend;
    Pos_t arend;
    Pos_t slend;
    Pos_t srend;
    int sid;
    int incontig = 0;
    stringstream consensus(stringstream::in | stringstream::out);
    stringstream buffer(stringstream::in | stringstream::out);
    vector<int> sdels;
    int ndel;
    int slen;
    int iid;
    char curline[80];
    string line;
    char buff[61];
    char* token;
    bool first = true;
    while (curCtg.getline(curline, '\n')) {
      if (curline[0] == '#' && curline[1] == '#') {
        if (!first) {
          TMPCTG << "#" << sid << endl;
          for (int i = 0; i < sdels.size(); i++)
            TMPCTG << sdels[i] << " ";
          TMPCTG << endl << '#' << endl;
          arend = alend + slen;
          asm_range[sid] = Range_t(alend, arend);
        }
        first = false;
        consensus.clear();
        ctg = string(strtok(curline+2, " "));
        iid = minCtgID++;
        ctgids[ctg] = iid;
        ctgnames[iid] = ctg;
        contigs[iid] = atoi(strtok(NULL, " "));
        incontig = true;
        slen = 0;
        continue;
      }
      
      if (curline[0] == '#') {
        if (incontig) {
          TMPCTG << "#" << iid << " C" << endl;
          for (int c = 0; c < consensus.str().size(); c+= 60) {
            consensus.get(buff, 61);
            TMPCTG << buff << endl;
          }
          TMPCTG << "#" << endl;
          for (int c = 0; c < consensus.str().size(); c+= 60) {
            for (int b = 0; b < 60; b++) {
              if (b + c >= consensus.str().size())
                break;
              TMPCTG << "X";
            }
            TMPCTG << endl;
          }
        } else {
          TMPCTG << "#" << sid << endl;
          for (int i = 0; i < sdels.size(); i++)
            TMPCTG << sdels[i] << " ";
          TMPCTG << endl;
          arend = alend + slen;
          asm_range[sid] = Range_t(alend, arend);
          slen = 0;
        }
        incontig = false;
        line = string(curline+1);
        // Extract the name of the contig.
        sname = line.substr(0,line.find('('));
        sdels.clear();
        ndel = 0;
        if (seqids[sname] == 0 && seqnames[0] != sname) {
          cerr << "Cannot find ID for sequence " << sname << endl;
          return 1;
        } else {
          sid = seqids[sname];
        }
        
        seqcontig[sid] = iid;
        contigseq[iid].push_back(sid);
        int curPos = 0; // Keep track of the position in curline (where we're parsing)
        buffer.str("");
        buffer << line.substr((curPos = line.find_first_of('(')+1),curPos = (line.find_first_of(')') - curPos));
        buffer >> alend;
        char delim[]=" \t{}<>";
        vector<string> tokens;
        curPos = line.find_first_of('{');
        Tokenize(line.substr(curPos,line.length()-curPos), tokens, " {}<>");
          int rets[2];
          buffer.str("");
          buffer << tokens[0];
          buffer >> rets[0];
          buffer << tokens[1];
          buffer >> rets[1];

        if (rets[0] < rets[1]) {
          slend = rets[0]-1;
          srend = rets[1];
          }
        else
          {
            slend = rets[0];
            srend = rets[1] -1;
          }
        seq_range[sid] = Range_t(slend, srend);
        continue;
      }

      if (incontig) {
        consensus << curline;
      } else { // Sequence record
        slen += line.length();
        for (int s = 0; s < line.length(); s++) {
          if(line[s] == '-') {
            sdels.push_back(ndel);
          } else {
            ndel++;
          }
        }
      }        
    } // while in
    // Process break sequence
    TMPCTG << "#" << sid << endl;
    for (int i = 0; i < sdels.size(); i++)
      TMPCTG << sdels[i] + " ";
    TMPCTG << endl;
    asm_range[sid] = Range_t(alend, arend);
    
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

bool parseACEFile() {
  cerr << "Parsing ACE file now " << endl;
  string in;
  string ctg;
  int len;
  string sname, tmp;
  int alend;
  int arend;
  int srend;
  int sid;

  bool inContig = false;
  bool inSequence = false;
  bool inQual = false;

  string contigName;
  int contigLen;
  int contigSeqs;
  string seqName;
  map<string,int> offsetMap;
  map<string,char> rc;
  string seq;
  vector<int> gaps;
  int iid;
  bool first = true;
  char firstletter;
  string qual = "";
  ifstream aceFile (globals["acefile"].c_str());
  cerr << globals["acefile"] << endl;
  if (aceFile == NULL)
    cerr << "File NOT opened" << endl;
  while (!aceFile.eof()) {
    firstletter = aceFile.peek();
    if((!inContig && !inSequence) && firstletter == 'C') {
      aceFile >> tmp;
      if (tmp == "CO") {
        if (!first)
          TMPCTG << "#" << endl;
        first = false;
        aceFile >> contigName;
        iid = minCtgID++;
        ctgnames[iid] = contigName;
        ctgids[contigName] = iid;
        aceFile >> contigLen;
        aceFile >> contigSeqs;
        aceFile.ignore(256, '\n');
        inContig = true;
        seq = "";
        //      offset;
        offsetMap.clear();
        continue;
      }
    }
   
    if (firstletter != '\n' && (inContig || inSequence)) {
      string::size_type pos = in.find_first_of("\n \t");
      aceFile >> in;
      in = in.substr(0, pos-1);
      seq += in;
      aceFile.ignore(256, '\n');
      continue;
    }
    if (firstletter == 'B') {
      string tmp;
      aceFile >> tmp;
      if (tmp == "BQ") {
        inQual = true;
        qual = "";
        aceFile.ignore(200, '\n');
        continue;
      } 
    }
    if (firstletter == '\n') {
      if (inContig) {
        inContig = false;
        replace(seq.begin(), seq.end(), '*', '-');
        vector<int> gaps;
        int gap  = seq.find("-", 0);
        while (gap != string::npos){ // make cll ungapped
          gaps.push_back(gap);
          gap = seq.find("-", gap + 1);
        }
        contigs[iid] = contigLen;
        // print consensus record
        TMPCTG << "#" << iid << " C" << endl;
        for (int c = 0; c < seq.size(); c+=60)
          TMPCTG << seq.substr(c,60) << endl;
        TMPCTG << "#" << endl;
        aceFile.ignore(256, '\n');
        continue;
      }
      else if (inSequence) {
        inSequence = false;
        aceFile.ignore(256, '\n');
      }
      else if (inQual) {
        inQual = false;
        string tmpqual = qual;
        qual = "";
        int q = 0;
        for (int c = 0; c < seq.size(); c++) {
          if (seq.substr(c,1) == "-"){
            if (q >= tmpqual.size()){
              q = tmpqual.size() -1;
            }
            qual += tmpqual.substr(q, 1);
          } else {
            qual += tmpqual.substr(q++, 1);
          }
        }
        for (int c = 0; c < qual.size(); c+=60) 
          TMPCTG << qual.substr(c, 60) << endl;
        aceFile.ignore(256, '\n');
      }
    }
    
    if (!inContig && firstletter == 'A') {
      string tmp2;
      aceFile >> tmp2;
      if (tmp2 == "AF") {
        string seqId;
        char seqrc;
        int offset;
        aceFile >> seqId;
        aceFile >> seqrc;
        aceFile >> offset;
        offsetMap[seqId] = offset-1;
        rc[seqId] = seqrc;
        aceFile.ignore(250, '\n');
        continue;
      }
    }
    if (inQual) {
      int val;
      string buff;
      getline(aceFile, buff);
      stringstream buffstream(buff);
      while (buffstream >> val)
        {
          qual += char('0' + val);
        }
      continue;
    }
    if (firstletter == 'R') {
      aceFile >> tmp;
      if (tmp == "RD") {
        // Indicates start of a sequence record
        inSequence = true;
        aceFile >> seqName;
        aceFile.ignore(256,'\n');
        seq = "";
        continue;
      }
    }
    
    if (aceFile.peek() == 'Q') {
      aceFile >> tmp;
      if (tmp == "QA") {
        if (aceFile.peek() == '-')
          aceFile.ignore(1, '-');
        int offset = offsetMap[seqName], end5, end3, cll, clr;
        aceFile >> end5;
        aceFile >> end3;
        aceFile >> cll;
        cll -= 1;
        aceFile >> clr;
        aceFile.ignore(250, '\n');
        // ACE files contain *s instead of -s
        replace(seq.begin(), seq.end(), '*', '-');
        // shift offset to beginning of clear range
        // otherwise it points to the beginning of the entire sequence
        // shown in the file
        
        len = seq.length();
        offset += cll;
        
        int ndel = 0;
        vector<int> sdels;
        string allseq = seq;
        
        seq = seq.substr(cll, clr-cll);
        for (int c = 0; c < seq.size(); c++) {
          if (seq.substr(c,1) == "-")
            sdels.push_back(ndel);
          else
            ndel++;
        }
        if (rc[seqName] == 'C') {
          ReverseComplement(seq);
          ReverseComplement(allseq);
          // if read is reversed, swap cll and clr
          int tmp = len - cll;
          cll = len - clr;
          clr = tmp;
        }
        
        string pref = allseq.substr(0, cll); // Prefix of sequence
        
        int i = 0;
        int asml = offset;
        int asmr = asml + clr - cll;
        
        int gap  = pref.find("-", 0);
        while (gap != string::npos){ // make cll ungapped
          gap = pref.find("-", gap + 1);
          cll--;
          clr--;
        }     
        gap = seq.find('-', 0);
        while (gap != string::npos) { // make clr ungapped
          gap = seq.find('-', gap+1);
          clr--;
        }
        
        // if reverse complemented, reverse the assembly range
        if (rc[seqName] == 'C')
          {
            int tmp = asml;
            asml = asmr;
            asmr = tmp;
          }
        // assign sequence id and populate all necessary data-structures
        int seqId;
        if (seqids.find(seqName) == seqids.end()) {
          seqId = minSeqID++;
          seqids[seqName] = seqId;
          seqnames[seqId] = seqName;
        } else
          seqId = seqids[seqName];
        seqcontig[seqId] = iid;
        contigseq[iid].push_back(seqId);
        if (cll > clr) {
          seq_range[seqId] = Range_t(clr, cll);
        } else {
          seq_range[seqId] = Range_t(cll, clr);
        }
        asm_range[seqId] = Range_t(asml, asmr);
        if (!readsDone) {
          string qualdata = "";
          int breakpos=0, temppos=0;
          string newseq;
          while (breakpos != string::npos) { // remove gap characters
            temppos = breakpos+1;
            breakpos = allseq.find('-', breakpos+1);
            newseq += allseq.substr(temppos, breakpos-temppos-1);
          }
          allseq = newseq;
          TMPSEQ << "#" << seqId << endl;
          for (int i = 0; i <= allseq.length(); i += 60)
            TMPSEQ << allseq.substr(i, 60) << endl;
          TMPSEQ << "#" << endl;
          for (int i = 0; i < cll; i++) {
            qualdata += char('0' + BADQUAL);
          }
          for (int i = cll; i < clr; i++)
            qualdata += char('0' + GOODQUAL);
          for (int i = clr; i < allseq.length(); i++)
            qualdata += char('0' + BADQUAL);
          for (int i = 0; i <= qualdata.length(); i+= 60)
            TMPSEQ << qualdata.substr(i, 60) << endl;
          TMPSEQ << "#" << endl;
        } // if readsdone == 0
        TMPCTG << "#" << seqId << endl;
        for (int i = 0; i < sdels.size(); i++)
          TMPCTG << sdels[i] << " ";
        TMPCTG << endl;
        continue;
      }
    }
    string temp;
    getline(aceFile, temp);
  } // while <IN>
} //parseAceFile

// CONTIG PARSING FUNCTIONS
//
// Each function parses either a file or a database table and
// fills in the following hashes:
// 
// contigs - contig_ids and sizes
// seqids - seq_name to seq_id
// seqnames - seq_id to seq_name
// seq_range - seq_id to seq_range 
// asm_range - seq_id to asm_range as blank delimited string
// seqcontig - seq_id to contig
// contigcons - contig consensus for each contig

/*
  struct record {
  string type;
  map<string,string> fields;
  }*/
/*
// Celera .asm
// populates %contigs, %asm_range, %s==contig, %contigcons
// expects %s==_range to be populated

void parseAsmFile(ifstream &asmFile)
{
  while (record = getRecord(asmFile))
    {
      record = parseRecord(record);
      
      if ((record.type == "CCO") || 
          (INCLUDE_SURROGATE && (type == "UTG") && (fields{sta} == "S")) ||
          (UTG_MESSAGES && (type == "UTG")))
        {
          int sts = "C";
          sts = fields{sta} if type == "UTG";
          sts = fields{pla} if type == "CCO";
          
          int id = getCAId(fields{acc});
          int iid = minCtgId++;
          int contiglen = fields{len};
          
          ctgnames{iid} = id;
          ctgids{id} = iid;
          
          int coord;
          
          int consensus = fields{cns};
          int consensus = split('\n', consensus);
          consensus = join('', @consensus);
          
          TMPCTG << "//iid sts" << endl;;
          TMPCTG << fields{cns};
          TMPCTG << "#" << endl;;
          TMPCTG << fields{qlt};
          
          contigs{iid} = contiglen;
          
          for (int i = 0; i <= //recs; i++)
            {
              int (sid, sfs, srecs) = parseRecord(recs[i]);
              if (sid == "MPS")
                {
                  if (! exists s==ids{getCAId(sfs{mid})}){
                    die ("Have not seen s==uence with id " . getCAId(sfs{mid}));
                  }
                  
                  int fid = s==ids{getCAId(sfs{mid})};
                  TMPCTG << "//fid" << endl;;
                  TMPCTG << sfs{del};
                  s==contig{fid} = id;
                  contigs=={id} .= "fid ";
                  
                  int (asml, asmr) = split(',', sfs{pos});
                  asm_range{fid} = "asml asmr";
                }
            }
                 TMPCTG << "#" << endl;;
               } 
          elsif (type == "SCF")
            {
              int off = 0;
              TMPSCF << "{SCF" << endl;;
              int id = getCAId(fields{acc});
              int iid = minCtgId++;
              ctgids{id} = iid;
              ctgnames{iid} = id;
              TMPSCF << "iid:iid" << endl;;
              TMPSCF << "eid:id" << endl;;
              for (int i = 0; i <= //recs; i++)
                {
                  int (sid, sfs, srecs) = parseRecord(recs[i]);
                  if (sid == "CTP")
                    {
                      TMPSCF << "{TLE" << endl;;
                      TMPSCF << "src:ctgids{sfs{ct1}}" << endl;;
                      TMPSCF << "off:off" << endl;;
                      
                      if (sfs{ori} == "N" ||
                          sfs{ori} == "I")
                        {
                          TMPSCF << "clr:0,contigs{ctgids{sfs{ct1}}}" << endl;;
                        } 
                      else 
                        {
                          TMPSCF << "clr:contigs{ctgids{sfs{ct1}}},0" << endl;;
                        }
                      
                      TMPSCF << "}" << endl;;
                      off += contigs{ctgids{sfs{ct1}}};
                      off += int(sfs{mea});
                      
                      if (i == //recs && sfs{ct1} != sfs{ct2})
                        {
                          TMPSCF << "{TLE" << endl;;
                          TMPSCF << "src:ctgids{sfs{ct2}}" << endl;;
                          TMPSCF << "off:off" << endl;;
                          
                          if (sfs{ori} == "N" ||
                              sfs{ori} == "O")
                            {
                              TMPSCF << "clr:0,contigs{ctgids{sfs{ct2}}}" << endl;;
                            } 
                          else 
                            {
                              TMPSCF << "clr:contigs{ctgids{sfs{ct2}}},0" << endl;;
                            }
                          
                          TMPSCF << "}" << endl;;
                        }
                        }
                    }
                  TMPSCF << "}" << endl;;
                } 
                     elsif (type == "CLK")
                {
                  TMPSCF << "{CTE" << endl;;
                  TMPSCF << "nds:ctgids{fields{co1}},ctgids{fields{co2}}" << endl;;
                  TMPSCF << "adj:fields{ori}" << endl;;
                  TMPSCF << "sze:fields{mea}" << endl;;
                  TMPSCF << "std:fields{std}" << endl;;
                  TMPSCF << "typ:M" << endl;;
                  TMPSCF << "}" << endl;;
                }
                     elsif (type == "AFG")
                {
                  // Grab the clear range from the AFG records if present
                  
                  if (exists fields{clr})
                    {
                      int acc = getCAId(fields{acc});
                      int iid = s==ids{acc};
                      
                      int (s==l, s==r) = split(/,/, fields{clr});
                      int clrstr = "s==l s==r";
                      
                      // if (!defined s==_range{iid} || s==_range{iid} ne clrstr)
                      // {
                      //   int  origclr = s==_range{iid};
                      //   print STDERR "Updating acc clr range from origclr to clrstr" << endl;;
                      // }
                      
                      s==_range{iid} = clrstr;
                    }
                }
                     }
            } // parseAsmFile
          
      
      /*=item B<my(id, fields, recs) = parseRecord(rec);>
  
Parses a record and returns a triplet consisting of
- record type
- hash of fields and values
- array of sub-records
(
*/
/*
  struct record {
  }
  
  void parseRecord()
  {
    my record = shift;

    my @lines = split('\n', record);

    my type;
    my %fields;
    my @recs;

    // get record type
    lines[0] =~ /\{(\w+)/;
    if (! defined 1){
        die ("Weird start of record: record" << endl;);
    }
    type = 1;

    if (lines[//lines] !~ /^\s*\}/){
      die ("Weird end of record: record" << endl;);
    }

    my level = 0;
    my fieldname;
    for (my i = 1; i < //lines; i++){
      if (regex_match(lines[i],results,^(\w+):(.+))){   // simple field
          fields{1} = 2;
      } // simple field
      if (regex_match(lines[i],results,^(\w+):)){ // complex field
          fieldname = 1;
          fields{fieldname} = "";
          i++;
          while (i < //lines && (lines[i] !~ /^\./)){
            fields{fieldname} .= "lines[i]" << endl;;
            i++;
          }
      } // complex field
      if (regex_match(lines[i],results,^\s*\{)){ // subrecord
          my level = 1;

          my thisrec = ++//recs;
          
          recs[thisrec] .= "lines[i]" << endl;;
          i++;
          while (level > 0 && i < //lines){
            if (regex_match(lines[i],results,^\s*\{)){
                level++;
            }
            if (regex_match(lines[i],results,^\s*\})){
                level--;
            }
            recs[thisrec] .= "lines[i]" << endl;;
            if (level == 0){
                break;
            } else {
                i++;
            }
          }
          if (level != 0){
            die ("Error parsing sub_record in: record" << endl;);
          }
      } // subrecord
    } // for i...
    
    return (type, \%fields, \@recs);
} // parseRecord
*/
/*
void getRecord(ifstream &file)
{
    string head;
    string data;
    string tl;
    
    if (! defined self->{buf} || self->{buf} !~ /^self->{headsep}/){ // record must start with a separator
	return ();
    }
    head = self->{buf};
    head =~ s/^self->{headsep}//;
    tl = tell(file);
    self->{buf} = <file>;
    chomp self->{buf};
    while (defined self->{buf} && self->{buf} !~ /^self->{headsep}/){
	data .= self->{buf} . self->{linesep};
	tl = tell(file);
	self->{buf} = <file>;
	if (defined self->{buf}){chomp self->{buf}};
    }
    self->{tell} = tl;
    return (head, data);
}
*/
      

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

