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

    contig.reverseComplement();

    ID_t iid = contig.getIID();

    contig_bank.replace(iid, contig);
    contig_bank.close();

    Bank_t scaffold_bank(Scaffold_t::NCODE);
    try 
    {
      // try to reverse the contig in the scaffold as well
      scaffold_bank.open(bank_name, B_READ|B_WRITE);
      Scaffold_t scaffold;

      AMOS::IDMap_t::const_iterator ci;
      for (ci = scaffold_bank.getIDMap().begin();
           ci;
           ci++)
      {
        scaffold_bank.fetch(ci->iid, scaffold);

        vector<Tile_t> & tiling = scaffold.getContigTiling();
        vector<Tile_t>::iterator ti;

        for (ti =  tiling.begin();
             ti != tiling.end();
             ti++)
        {
          if (ti->source == iid)
          {
            ti->range.swap();

            scaffold_bank.replace(scaffold.getIID(), scaffold);
            cout << "Updated scaffold i" << scaffold.getIID() << endl;
          }
        }
      }
    }
    catch (Exception_t & e)
    {
      cerr << "WARNING: Scaffold not updated!" << endl;
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}
