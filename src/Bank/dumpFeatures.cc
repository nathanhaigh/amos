#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;
using namespace std;

int main (int argc, char ** argv)
{
  if (argc != 2)
  {
    cerr << "Usage: bank2contig bankname" << endl;
    return EXIT_FAILURE;
  }

  BankStream_t contig_bank(Contig_t::NCODE);
  Contig_t contig;

  string bank_name = argv[1];

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    contig_bank.open(bank_name, B_READ);

    while (contig_bank >> contig)
    {
      const vector <AMOS::Feature_t> & feats = contig.getFeatures();
      vector<AMOS::Feature_t>::const_iterator fi;

      for (fi = feats.begin(); fi != feats.end(); fi++)
      {
        cout << contig.getEID() << " "
             << fi->type        << " "
             << fi->group       << " "
             << fi->range.begin << " "
             << fi->range.end   << " "
             << fi->comment
             << endl;
      }
    }

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
