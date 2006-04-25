#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "ContigUtils.hh"

// Reverse complement a given contig

using namespace AMOS;
using namespace std;

bool USE_EID = 1;

int main (int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: revcontig bankname eid" << endl;
    cerr << endl
         << "Reverse complements the selected contig and associated" << endl
         << "reads, scaffold, features info" << endl
         << "TODO: Fix contig graph as well" << endl;

    return EXIT_FAILURE;
  }

  string bank_name = argv[1];
  string contigid = argv[2];


  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    Bank_t contig_bank(Contig_t::NCODE);
    Bank_t scaffold_bank(Scaffold_t::NCODE);
    BankStream_t feat_bank(Feature_t::NCODE);

    contig_bank.open(bank_name, B_READ|B_WRITE);
    if (scaffold_bank.exists(bank_name))
    {
      scaffold_bank.open(bank_name, B_READ|B_WRITE);
    }

    if (feat_bank.exists(bank_name))
    {
      feat_bank.open(bank_name, B_READ|B_WRITE);
    }

    ID_t contigiid = lookupIID(contig_bank, contigid);

    reverseContig(contig_bank, scaffold_bank, feat_bank, contigiid);
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}
