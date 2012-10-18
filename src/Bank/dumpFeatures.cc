#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;

int USEIID = 0;
int GFF = 0;

void printFeature(Bank_t & contig_bank, ID_t iid, char type, int b, int e, const string & comment)
{
  if (GFF)
  {
    if (USEIID) { cout << iid; }
    else        { cout << contig_bank.lookupEID(iid); }

    cout << "\t" << "AMOS"          // program
         << "\t" << "misc_feature"  // feature type
         << "\t" << b               // beginning
         << "\t" << e               // end
         << "\t" << "."             // score
         << "\t" << "+"             // strand
         << "\t" << "."             // frame
         << "\t" 
         << "Experiment \""
         << type << "\";" // feature type
         << "Note \""
         << comment << "\"" << endl;  // comment
  }
  else
  {
    if (USEIID)
    {
      cout << iid  << " " 
           << type << " " 
           << b    << " " 
           << e    << " " 
           << comment << endl;
    }
    else
    {
      cout << contig_bank.lookupEID(iid) << " "
           << type   << " "
           << b      << " "
           << e      << " "
           << comment << endl;
    }
  }
}

int main (int argc, char ** argv)
{
  AMOS_Foundation * tf = NULL;
  int retval = 0;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"\n.USAGE.\n"
"  dumpFeatures [options] bankname\n"
"\n.DESCRIPTION.\n"
"  Dump contig features from a bank as a tab deliminated list. Scaffold\n"
"  features are converted to appropriate contig locatations.\n\n"
"  Format of output is:\n"
"  contigeid type end5 end3 [comment]\n"
"  \n"
"  contigeid is the contig which has the feature\n"
"  type is the single character feature type. \n"
"  end5 and end3 are the gapped coordinates of the feature\n"
"  [comment] is an optional comment string describing the feature\n"
"\n.OPTIONS.\n"
"  -i   report contig iids instead of eids\n"
"  -u   report ungapped coordinates instread of gapped coordinates\n"
"  -g   report features in .gff format\n"
"\n.KEYWORDS.\n"
"  amos bank, converters\n";



    int CONVERTUNGAPPED = 0;

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("u", &CONVERTUNGAPPED);
    tf->getOptions()->addOptionResult("i", &USEIID);
    tf->getOptions()->addOptionResult("g", &GFF);

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 1)
    {
      cerr << "Usage: dumpFeatures [options] bankname" << endl;
      return 0;
    }

    BankStream_t feat_bank(Feature_t::NCODE);
    Bank_t contig_bank(Contig_t::NCODE);
    Bank_t scaffold_bank(Scaffold_t::NCODE);

    Feature_t feat;
    Contig_t contig;
    Scaffold_t scaffold;

    string bank_name = argvv.front(); argvv.pop_front();

    cerr << "Processing " << bank_name << " at " << Date() << endl;

    if (!feat_bank.exists(bank_name))
    {
      cerr << "WARNING: No features in bank" << endl;
      exit(0);
    }

    feat_bank.open(bank_name, B_READ);

    if (scaffold_bank.exists(bank_name))
    {
      scaffold_bank.open(bank_name, B_READ);
    }

    contig_bank.open(bank_name, B_READ);


    ID_t contigloaded = 0;
    int featcount = 0;

    while (feat_bank >> feat)
    {
      if (scaffold_bank.isOpen() && 
          feat.getSource().second == Scaffold_t::NCODE)
      {
        int b = feat.getRange().begin;
        int e = feat.getRange().end;

        ID_t iid = feat.getSource().first;

        scaffold_bank.fetch(iid, scaffold);

        vector<Tile_t> & tiling = scaffold.getContigTiling();
        vector<Tile_t>::iterator ti;

        for (ti = tiling.begin(); ti != tiling.end(); ti++)
        {
          if ((ti->offset < e) && (ti->getRightOffset() > b))
          {
            // This contig overlaps the feature
            int cb = b - ti->offset;
            int ce = e - ti->offset;

            if (CONVERTUNGAPPED)
            {
              cerr << "Converting to ungapped coordinates is unsupported for scaffold features" << endl;
            }

            printFeature(contig_bank, ti->source, feat.getType(), cb, ce, feat.getComment());
          }
        }
      }
      else if (feat.getSource().second == Contig_t::NCODE)
      {
        featcount++;

        int b = feat.getRange().begin;
        int e = feat.getRange().end;

        ID_t iid = feat.getSource().first;

        if (CONVERTUNGAPPED)
        {
          if (iid != contigloaded)
          {
            contig_bank.fetch(iid, contig);
            contigloaded = iid;
          }

          b = contig.gap2ungap(b);
          e = contig.gap2ungap(b);
        }

        printFeature(contig_bank, iid, feat.getType(), b, e, feat.getComment());
      }
    }

    cerr << "Printed " << featcount << " features" << endl;
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

  return EXIT_SUCCESS;
}
