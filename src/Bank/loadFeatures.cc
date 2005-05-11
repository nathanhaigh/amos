#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;
using namespace std;

int DEBUG = 0;
int LOAD = 1;

int main (int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: bank2contig bankname featfile" << endl;
    return EXIT_FAILURE;
  }

  Bank_t contig_bank(Contig_t::NCODE);
  Bank_t feat_bank(Feature_t::NCODE);
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
    feat_bank.open(bank_name);

    string eid;
    int end3;
    int end5;
    string count;
    string density;
    char type;


    int featurecount = 0;

    while (features >> eid 
                    >> type 
                    >> end5 
                    >> end3) 
    {
      string comment;
      char delim;
      features.get(delim);

      if (delim != '\n')
      {
        getline(features, comment);
      }

      range.begin = end5;
      range.end = end3;

      Feature_t feat;
      feat.setType(type);
      feat.setRange(range);
      feat.setComment(comment);


      if (DEBUG)
      {
        cerr << "contigeid: " << eid << endl
             << "type: "      << type << endl
             << "range: "     << end5 << "," << end3 << endl
             << "coment: \""  << comment << "\"" << endl;
      }

      if (LOAD)
      {
        ID_t iid = contig_bank.lookupIID(eid);
        feat.setSource(std::make_pair(iid, Contig_t::NCODE));
        feat_bank.append(feat);
      }

      featurecount++;
      cerr << ".";
    }

    cerr << endl
         << "Loaded " << featurecount << " features" << endl;
    
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
