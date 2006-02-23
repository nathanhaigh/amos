#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;
using namespace std;

int main (int argc, char ** argv)
{
  if (argc != 2)
  {
    cerr << "Usage: dumpFeatures bankname" << endl;
    return EXIT_FAILURE;
  }

  BankStream_t feat_bank(Feature_t::NCODE);
  Bank_t contig_bank(Contig_t::NCODE);

  Feature_t feat;

  string bank_name = argv[1];

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    contig_bank.open(bank_name, B_READ);
    feat_bank.open(bank_name, B_READ);

    while (feat_bank >> feat)
    {
      if (feat.getSource().second == Contig_t::NCODE)
      {
        cout << contig_bank.lookupEID(feat.getSource().first) << " "
             << feat.getType()        << " "
             << feat.getRange().begin << " "
             << feat.getRange().end   << " "
             << feat.getComment()
             << endl;
      }
    }

    contig_bank.close();
    feat_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}
