#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;
using namespace std;

int main (int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: bank2contig bankname featfile" << endl;
    return EXIT_FAILURE;
  }

  Bank_t contig_bank(Contig_t::NCODE);
  Contig_t contig;
  ifstream features;

  string bank_name = argv[1];
  string feat_file = argv[2];

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  Range_t range;

  try
  {
    features.open(feat_file.c_str());

    if (!features)
    {
      AMOS_THROW_IO("Couldn't open feature file " + feat_file);
    }

    contig_bank.open(bank_name);

    string group;
    string eid;
    int end3;
    int end5;
    string count;
    string density;

    string comment;

    int featurecount = 0;

    while (features >> group >> eid >> end5 >> end3 >> count >> density)
    {
      comment = count + " " + density;

      Feature_t feat;
      feat.type = 'P';
      feat.group = group;
      feat.range.setRange(end5,end3);
      feat.comment = comment;

      contig_bank.fetch(eid.c_str(), contig);
      contig.getFeatures().push_back(feat);
      contig_bank.replace(eid.c_str(), contig);

      featurecount++;
      cerr << ".";
    }

    cerr << endl
         << "Loaded " << featurecount << " features" << endl;
    
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
