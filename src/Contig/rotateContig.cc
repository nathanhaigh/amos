#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

// Rotate a contig to a new offset

using namespace AMOS;
using namespace std;

bool USE_EID = 1;

int main (int argc, char ** argv)
{
  if (argc != 4)
  {
    cerr << "Usage: rotateContig bankname eid offset" << endl;
    return EXIT_FAILURE;
  }

  Bank_t contig_bank(Contig_t::NCODE);
  string bank_name = argv[1];
  string contigid = argv[2];
  Pos_t newoffset = atoi(argv[3]);

  Contig_t contig;

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    contig_bank.open(bank_name, B_READ|B_WRITE);
    contig_bank.fetch(contigid, contig);

    string cons = contig.getSeqString();
    string cqual = contig.getQualString();

    if (newoffset < 0 || newoffset >= cons.length())
    {
      cerr << "Offset: " << newoffset << " is outside of valid range" << endl;
      AMOS_THROW("Offset out of range");
    }

    // adjust consensus
    string left  = cons.substr(0, newoffset);
    string lqual = cqual.substr(0, newoffset);

    cons  = cons.substr(newoffset, cons.length() - newoffset);
    cqual = cqual.substr(newoffset, cqual.length() - newoffset);

    cons += left;
    cqual += lqual;

    // shift reads
    vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::iterator ti;
    for (ti = tiling.begin(); ti != tiling.end(); ti++)
    {
      ti->offset -= newoffset;
      while (ti->offset < 0)
      {
        ti->offset += cons.length();
      }

      while (ti->offset + ti->getGappedLength() - 1 >= cons.length())
      {
        ti->offset -= cons.length();
      }
    }

    contig.setSequence(cons, cqual);

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
