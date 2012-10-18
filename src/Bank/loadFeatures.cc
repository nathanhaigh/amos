#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"


using namespace AMOS;
using namespace std;

int DEBUG = 0;
int LOAD = 1;

int main (int argc, char ** argv)
{
  AMOS_Foundation * tf = NULL;
  int retval = 0;

  Contig_t contig;
  ID_t contigloaded = 0;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"\n"
".USAGE.\n"
"  loadFeatures [options] bankname featfile\n"
"\n.DESCRIPTION.\n"
"  Load features into a bank.\n"
"  Format of featfile is:\n"
"  contigeid type end5 end3 [comment]\n"
"\n"
"  contigeid is the contig which has the feature\n"
"  type is the single character feature type. \n"
"  end5 and end3 are the coordinates of the feature\n"
"  [comment] is an optional comment string describing the feature\n"
"\n"
"  Standard types are: \n"
"    (B)reakpoint\n"
"    (C)overage\n"
"    (O)rf\n"
"    (S)NP\n"
"    (U)nitig\n" 
"\n"
".OPTIONS.\n"
"  -u Coordinates are ungapped\n"
"  -i Contigid is the IID\n"
"\n.KEYWORDS.\n"
"  amos bank, features"
"\n";
 
    int CONVERTUNGAPPED = 0;
    int USEIID = 0;

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("u", &CONVERTUNGAPPED);
    tf->getOptions()->addOptionResult("i", &USEIID);
    tf->getOptions()->addOptionResult("d", &DEBUG);

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 2)
    {
      cerr << "Usage: loadFeatures [options] bankname featfile" << endl;
      return 0;
    }


    Bank_t contig_bank(Contig_t::NCODE);
    Bank_t feat_bank(Feature_t::NCODE);
    ifstream features;

    string bank_name = argvv.front(); argvv.pop_front();
    string feat_file = argvv.front(); argvv.pop_front();

    cerr << "Loading features from " << feat_file << " into " << bank_name << endl;

    Range_t range;

    features.open(feat_file.c_str());

    if (!features)
    {
      AMOS_THROW_IO("Couldn't open feature file " + feat_file);
    }

    contig_bank.open(bank_name);
    if (feat_bank.exists(bank_name))
    {
      feat_bank.open(bank_name);
    }
    else
    {
      feat_bank.create(bank_name);
    }

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
             << "coment: \""  << comment << "\"" << endl << endl;
      }

      if (LOAD)
      {
        ID_t iid;

        if (USEIID)
        {
          iid = atoi(eid.c_str());

          if (!contig_bank.existsIID(iid))
          {
            iid = AMOS::NULL_ID;
          }
        }
        else
        {
          iid = contig_bank.lookupIID(eid);
        }

        if (iid != AMOS::NULL_ID)
        {
          if (CONVERTUNGAPPED)
          {
            if (iid != contigloaded)
            {
              contig_bank.fetch(iid, contig);
              contigloaded = iid;
            }

            range.begin = contig.ungap2gap(range.begin);
            range.end   = contig.ungap2gap(range.end);
            feat.setRange(range);
          }

          feat.setSource(std::make_pair(iid, Contig_t::NCODE));
          feat_bank.append(feat);
          featurecount++;
          cerr << ".";
        }
        else
        {
          cerr << "WARNING: Contig " << eid << " not found, skipping" << endl;
        }
      }
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
  catch (const ExitProgramNormally & e)
  {
    retval = 0;
  }
  catch (const amosException & e)
  {
    cerr << e << endl;
    retval = 100;
  }

  try
  {
    if (tf) delete tf;
  }
  catch (const amosException & e)
  {
    cerr << "amosException while deleting tf: " << e << endl;
    retval = 105;
  }

  return retval;;
}
