// $Id$

// This program reads a set of contigs from a bank then reports 
// regions where mate-pair information indicates a problem

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "foundation_AMOS.hh"
#include <getopt.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <math.h>
#include <functional>

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

#define endl "\n"

char revcompl(char c)
{
  switch (toupper(c)){
  case 'A':    return 'T';
  case 'C':    return 'G';
  case 'T':    return 'A';
  case 'G':    return 'C';
  case 'U':    return 'A';
  case 'M':    return 'K';
  case 'R':    return 'Y';
  case 'W':    return 'W';
  case 'S':    return 'S';
  case 'Y':    return 'R';
  case 'K':    return 'M';
  case 'V':    return 'B';
  case 'H':    return 'D';
  case 'D':    return 'H';
  case 'B':    return 'V';
  case 'N':    return 'N';
  case 'X':    return 'X';
  case '-':    return '-';
  default:     return 'N';
  }
} // revcompl char version


char baseHash (char c)
{
  if (c < '-')
    switch (c) {
    case 0: return 'A';
    case 1: return 'C';
    case 2: return 'G';
    case 3: return 'T';
    case 4: return 'M';
    case 5: return 'R';
    case 6: return 'W';
    case 7: return 'S';
    case 8: return 'Y';
    case 9: return 'K';
    case 10: return 'V';
    case 11: return 'H';
    case 12: return 'D';
    case 13: return 'B';
    case 14: return 'N';
    case 15: return 'X';
    case 16: return '-';
    }
  else
    switch (toupper(c)) {
    case 'A': return 0;
    case 'C': return 1;
    case 'G': return 2;
    case 'T': return 3;
    case 'M': return 4;
    case 'R': return 5;
    case 'W': return 6;
    case 'S': return 7;
    case 'Y': return 8;
    case 'K': return 9;
    case 'V': return 10;
    case 'H': return 11;
    case 'D': return 12;
    case 'B': return 13;
    case 'N': return 14;
    case 'X': return 15;
    case '-': return 16;
    }
}

string revcompl(const string & in)
{
  string out = in;
  for (int i = 0; i < in.length(); i++)
    out[in.length() - i - 1] = revcompl(in[i]);
  return out;
} // revcompl string version


struct Column_t {
  int depth;
  pair<char, char> consensus;
  vector<vector<pair<ID_t, char> > > col;

  Column_t () {
    depth = 0;
    consensus = pair<char, char> ('N', '0');
    col.resize(17); // number of possible nucleotides
  }

  // empty the column
  void clear(void) {
    depth = 0;
    consensus = pair<char, char> ('N', '0');
    for (int i = 0; i < col.size(); i++)
      col[i].clear();
  }
};

struct tiledRead_t {
  string seq, qual;
  Tile_t tile;
};

struct cmpTile : 
  public binary_function<Tile_t, Tile_t, bool> 
{
  bool operator () (Tile_t a, Tile_t b)
  {
    return (a.offset > b.offset);
  }
};

struct cmpPosIt : 
  public binary_function<pair<Pos_t, list<tiledRead_t>::iterator>, pair<Pos_t, list<tiledRead_t>::iterator>, bool> 
{
  bool operator () (pair<Pos_t, list<tiledRead_t>::iterator> a, pair<Pos_t, list<tiledRead_t>::iterator> b)
  {
    return (a.first > b.first);
  }
};

class ContigIterator_t {
private:
  Bank_t * readBank;
  Contig_t contig;
  Pos_t crt; // next position to read
  priority_queue<Tile_t, vector<Tile_t>, cmpTile > starts;
  priority_queue<pair<Pos_t, list<tiledRead_t>::iterator>, vector<pair<Pos_t, list<tiledRead_t>::iterator> >, cmpPosIt > ends;
  list<tiledRead_t> tiled;

public:
  ContigIterator_t(Contig_t & ctg, Bank_t * rdbank);
  //  ~ContigIterator_t();
  bool getNext(Column_t & column);
};

ContigIterator_t::ContigIterator_t(Contig_t & ctg, Bank_t * rdbank)
{
  crt = 0;
  readBank = rdbank;
  contig = ctg;

  for (vector<Tile_t>::iterator tle = ctg.getReadTiling().begin();
       tle != ctg.getReadTiling().end(); tle++)
    starts.push(*tle);
}


pair<string, string> insertGaps(Read_t & rd, Tile_t & tle)
{
  string seq, qual;
  if (tle.range.getBegin() < tle.range.getEnd()){
    seq = rd.getSeqString(tle.range);
    qual = rd.getQualString(tle.range);
  } else {
    seq = rd.getSeqString(Range_t(tle.range.getEnd(), tle.range.getBegin()));
    qual = rd.getQualString(Range_t(tle.range.getEnd(), tle.range.getBegin()));
    seq = revcompl(seq);
    reverse(qual.begin(), qual.end());
  }

  for (int i = 0; i < tle.gaps.size(); i++){
    seq.insert(tle.gaps[i] + i, "-");
    qual.insert(tle.gaps[i] + i, "0");
  }

  return pair<string, string>(seq, qual);
}

bool ContigIterator_t::getNext(Column_t & column)
{
  if (starts.empty() && ends.empty())
    return false; // nothing to be done

  //  cerr << "got here\n";
  while (! starts.empty() && starts.top().offset <= crt){ // add to list
    Read_t rd;
    if (! readBank->existsIID(starts.top().source)){
      cerr << "Cannot find read with IID: " << starts.top().source << endl;
      exit(1);
    }
    //    cerr << "Adding " << starts.top().source << " " << starts.top().offset << " because " << crt << endl;
    
    readBank->fetch(starts.top().source, rd);

    tiledRead_t trd;
    Tile_t top = starts.top();

    pair<string, string> rdq = insertGaps(rd, top);

    trd.seq = rdq.first;
    trd.qual = rdq.second;
    trd.tile = starts.top();

    list<tiledRead_t>::iterator ins = tiled.insert(tiled.end(), trd);
    
    Pos_t end = starts.top().offset + trd.seq.length();

    ends.push(pair<Pos_t, list<tiledRead_t>::iterator> (end, ins));

    starts.pop(); // get rid of tile I've already processed
  }

  //  cerr << "Done inserting\n";

  while (! ends.empty() && ends.top().first <= crt){ // remove from list
    tiled.erase(ends.top().second);
    ends.pop();
  }

  //  cerr << "Done removing\n";

  // now it's time to visit the tiling at crt
  if (tiled.empty())
    return false;  // we've gone past the end of the tiling


  column.clear();
  column.consensus = pair<char, char>(contig.getSeqString()[crt],
				      contig.getQualString()[crt]);

  //  cerr << "Processing tile\n";

  for (list<tiledRead_t>::iterator tle = tiled.begin();
       tle != tiled.end(); tle++){ // for each read
    assert(crt >= tle->tile.offset && 
	   crt < tle->tile.offset + tle->seq.length());

    Pos_t off = crt - tle->tile.offset;
    char base, qual;

    base = tle->seq[off];
    qual = tle->qual[off];

    column.depth++;
    column.col[baseHash(base)].push_back(pair<ID_t, char> (tle->tile.source, qual));
  }

  crt++;

  //  cerr << "Done\n";

  return true;
}


map<string, string> globals; // global variables

void printHelpText()
{
  cerr << 
    "\n"
    ".NAME.\n"
    "ContigIterator - iterates through colums within contigs\n"
    "\n.USAGE.\n"
    "ContigIterator -b[ank] <bank_name>\n"
    "\n.OPTIONS.\n"
    "-h, -help     print out help message\n"
    "-b, -bank     bank where assembly is stored\n"
    "\n.DESCRIPTION.\n"
    "\n.KEYWORDS.\n"
    "AMOS bank\n"
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

  BankStream_t contig_stream (Contig_t::NCODE);
  if (! contig_stream.exists(globals["bank"])){
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

  
  Bank_t read_bank (Read_t::NCODE);
  if (! read_bank.exists(globals["bank"])){
    cerr << "No read account found in bank " << globals["bank"] << endl;
    exit(1);
  }
  try {
    read_bank.open(globals["bank"], B_READ);
  } catch (Exception_t & e)
    {
      cerr << "Failed to open read account in bank " << globals["bank"] 
           << ": " << endl << e << endl;
      exit(1);
    }


  // Stream through the contigs and retrieve map of read positions within ctgs.
  Contig_t ctg;
  while (contig_stream >> ctg) {
    ContigIterator_t ci(ctg, &read_bank);
    Column_t col;

    cout << "Doing contig " << ctg.getIID() << endl;

    Pos_t column = 0;
    while (ci.getNext(col)){
      // print column information
      column++;
      cout << "Column " << column << ":" << endl;
      cout << "Consensus: " << col.consensus.first << ", " << col.consensus.second << endl;
      for (int i = 0; i < col.col.size(); i++)
	if (!col.col[i].empty()) {
	  cout << "Base " << baseHash(i) << ": ";
	  for (int j = 0; j < col.col[i].size(); j++)
	    cout << col.col[i][j].first << "(" << col.col[i][j].second << ") ";
	  cout << endl;
	}
    }

    cout << endl;
  }

  return(0);
} // main
    
