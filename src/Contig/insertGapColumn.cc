#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

// Reverse complement a given contig

using namespace AMOS;
using namespace std;

bool USE_EID = 1;

int main (int argc, char ** argv)
{
  if (argc != 4)
  {
    cerr << "Usage: insertgapcontig bankname eid gindex" << endl;
    return EXIT_FAILURE;
  }

  Bank_t contig_bank(Contig_t::NCODE);
  string bank_name = argv[1];
  string contigid = argv[2];
  int gindex = atoi(argv[3]);

  Contig_t contig;

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    contig_bank.open(bank_name, B_READ|B_WRITE);
    contig_bank.fetch(contigid, contig);

    string qual(contig.getQualString());
    string seq(contig.getSeqString());

    seq.insert(gindex, "-", 1); 
    qual.insert(gindex, "N", 1); 

    cerr << "Reseting consensus";
    contig.setSequence(seq, qual);
    cerr << "." << endl;

    cerr << "Adjusting tiling";
    std::vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::iterator i;
    for (i =  tiling.begin();
         i != tiling.end();
         i++)
    {
      if (i->offset > gindex)
      {
        // insert before read, shift over 1
        i->offset++;
      }
      else if (i->offset + i->getGappedLength() - 1 < gindex)
      {
        // insert after read, nothing to do
      }
      else
      {
        // gap inserted into read
        int gseqpos = gindex - i->offset;

        // create a new vector of gaps, with a gap at gseqpos
        vector<Pos_t> newgaps; 

        // count gaps that occur before gap we are inserting
        int gapcount = 0;
        vector<Pos_t>::iterator g;
        for (g =  i->gaps.begin();
             g != i->gaps.end();
             g++, gapcount++)
        {
          int cgseqpos = *g+gapcount;
          if (cgseqpos > gseqpos) { break; }

          // gap precedes gap we are inserting
          newgaps.push_back(*g);
        }

        newgaps.push_back(gseqpos-gapcount);

        for (;
             g != i->gaps.end();
             g++)
        {
          newgaps.push_back(*g);
        }

        i->gaps = newgaps;
      }
    }

    cerr << "." << endl;

    contig_bank.replace(contig.getIID(), contig);
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
