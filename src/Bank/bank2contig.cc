#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;
using namespace std;

bool USE_EID = 1;

Pos_t getUngappedPos(const string & str, Pos_t offset)
{
  Pos_t retval = 1;

  for (Pos_t gindex = 0; gindex < offset; gindex++)
  {
    if (str[gindex] != '-')
    {
      retval++;
    }
  }

  return retval;
}

int main (int argc, char ** argv)
{
  if (argc != 2)
  {
    cerr << "Usage: bank2contig bankname" << endl;
    return EXIT_FAILURE;
  }

  BankStream_t contig_bank(Contig_t::NCODE);
  Bank_t read_bank(Read_t::NCODE);
  string bank_name = argv[1];

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    read_bank.open(bank_name, B_READ);
    contig_bank.open(bank_name, B_READ);

    Contig_t contig;
    Read_t read; 
    
    while (contig_bank >> contig)
    {
      const std::vector<Tile_t> & tiling = contig.getReadTiling();

      cout << "##";

      if (USE_EID) 
      { 
        string s(contig.getEID());
        if (s.empty()) { cout << contig.getIID(); }
        else           { cout << s; }
      }
      else { cout << contig.getIID(); }

      const string cons = contig.getSeqString();

      cout << " "  << tiling.size()
           << " "  << cons.length()
           << " bases, 00000000 checksum." << endl;

      Fasta_Print(stdout, cons.c_str(), NULL, 60);

      vector<Tile_t>::const_iterator i;
      for (i =  tiling.begin();
           i != tiling.end();
           i++)
      {
        bool rc = 0;
        read_bank.fetch(i->source, read);
        Range_t range = i->range;
        Range_t clr = range;
        Pos_t gappedLen = i->range.getLength() + i->gaps.size();

        if (i->range.begin > i->range.end) { clr.end++; rc = 1; } 
        else                               { clr.begin++; }

        cout << "#";

        if (USE_EID) 
        { 
          string s(read.getEID());
          int i = s.find(' ');
          if (i != s.npos) { s = s.substr(0,i); }
          cout << s;
        }
        else { cout << read.getIID(); }

        cout << "(" << i->offset 
             << ((rc) ? ") [RC] " : ") [] ") << gappedLen
             << " bases, 00000000 checksum."
             << " {" << clr.begin << " " << clr.end << "}"
             << " <" << getUngappedPos(cons, i->offset)
             << " "  << getUngappedPos(cons, i->offset + gappedLen - 1)
             << ">"  << endl;

        if (rc) { range.swap(); }
        string sequence = read.getSeqString(range);
        if (rc) { Reverse_Complement(sequence); }

        Pos_t gapcount = 0;

        vector<Pos_t>::const_iterator g;
        for (g  = i->gaps.begin();
             g != i->gaps.end();
             g++)
        {
          sequence.insert(*g+gapcount, "-", 1);
          gapcount++;
        }

        Fasta_Print(stdout, sequence.c_str(), NULL, 60);
      }
    }

    read_bank.close();
    contig_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}
