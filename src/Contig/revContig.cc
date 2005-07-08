#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

// Reverse complement a given contig

using namespace AMOS;
using namespace std;

bool USE_EID = 1;

int main (int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: revcontig bankname eid" << endl;
    return EXIT_FAILURE;
  }

  Bank_t contig_bank(Contig_t::NCODE);
  string bank_name = argv[1];
  string contigid = argv[2];

  Contig_t contig;

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    contig_bank.open(bank_name, B_READ|B_WRITE);
    contig_bank.fetch(contigid, contig);

    string qual(contig.getQualString());
    string seq(contig.getSeqString());
    Reverse_Complement(seq);
    reverse(qual.begin(), qual.end());
    contig.setSequence(seq, qual);

    std::vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::iterator i;
    for (i =  tiling.begin();
         i != tiling.end();
         i++)
    {
      i->range.swap();
      i->offset = seq.length() - (i->offset + i->getGappedLength());

      Pos_t len = i->range.getLength();

      vector<Pos_t>::iterator g;
      for (g =  i->gaps.begin();
           g != i->gaps.end();
           g++)
      {
        *g = len - *g;
      }

      reverse(i->gaps.begin(), i->gaps.end());
    }

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
