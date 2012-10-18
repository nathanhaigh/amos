#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;
using namespace std;

string OPT_BankName;

string OPT_EIDFile;
string OPT_IIDFile;
string OPT_scaffeid;
int    OPT_scaffiid;

void PrintUsage (const char * s)
{
  cerr << "\nUSAGE: " << s << "  [options]  <bank path>\n\n";
}

//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage(s);
  cerr << "-h      Display help information\n"
       << "-v      Display the compatible bank version\n"
       << "-E file Just normalize the scaffolds eids listed in file\n"
       << "-I file Just normalize the scaffolds iids listed in file\n\n"
       << "-e eid  Just normalize the specified scaffold eid\n"
       << "-i iid  Just normalize the specified scaffold iid\n"
       << endl;
  
  cerr << "Normalizes the scaffolds in a AMOS bank by ensuring all of the\n"
       << "contigs are forward oriented. Also fixes contig features, but not\n"
       << "contig links\n"
       << endl;
}

void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hvE:I:e:i:")) != EOF) )
  {
    switch (ch)
    {
      case 'h':
        PrintHelp (argv[0]);
        exit (EXIT_SUCCESS);
        break;

      case 'v':
        PrintBankVersion (argv[0]);
        exit (EXIT_SUCCESS);
      break;

      case 'E': OPT_EIDFile = optarg; break;
      case 'I': OPT_IIDFile = optarg; break;
      case 'i': OPT_scaffiid = atoi(optarg); break;
      case 'e': OPT_scaffeid = optarg; break;

      default: errflg ++;
      }
  }

  if ( errflg > 0 || optind != argc - 1 )
  {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
  }

  OPT_BankName = argv [optind];
}


map<ID_t, int> contigFeaturesToFlip;


bool normalizeScaffold(Scaffold_t & scaff, Bank_t & contig_bank)
{
  bool changed = false;
  vector<Tile_t> & contigs = scaff.getContigTiling();
  vector<Tile_t>::iterator ci;

  sort(contigs.begin(), contigs.end(), TileOrderCmp());

  for (ci = contigs.begin(); ci != contigs.end(); ci++)
  {
    if (ci->range.isReverse())
    {
      Contig_t contig;
      contig_bank.fetch(ci->source, contig);

      cerr << "Flipping ciid: " << ci->source 
           << " ceid: " << contig.getEID()
           << endl;

      ci->range.swap();
      contig.reverseComplement();

      contig_bank.replace(ci->source, contig);

      changed = true;
      contigFeaturesToFlip.insert(make_pair(ci->source, contig.getLength()));
    }
  }

  return changed;
}




int main (int argc, char ** argv)
{
  ParseArgs (argc, argv);

  Bank_t scaff_bank(Scaffold_t::NCODE);
  Bank_t contig_bank(Contig_t::NCODE);
  BankStream_t feat_bank(Feature_t::NCODE);

  cerr << "Processing " << OPT_BankName << " at " << Date() << endl;

  try
  {
    scaff_bank.open(OPT_BankName, B_READ | B_WRITE);
    contig_bank.open(OPT_BankName, B_READ | B_WRITE);

    Scaffold_t scaffold;
    ifstream file;
    string id;

    if (!OPT_scaffeid.empty()) { OPT_scaffiid = scaff_bank.lookupIID(OPT_scaffeid); }

    if (OPT_scaffiid)
    {
      scaff_bank.fetch(OPT_scaffiid, scaffold);
      bool changed = normalizeScaffold(scaffold, contig_bank);
      if (changed) { scaff_bank.replace(OPT_scaffiid, scaffold); }
    }
    else if (!OPT_EIDFile.empty())
    {
      file.open(OPT_EIDFile.c_str());
      
      if (!file)
      {
        throw Exception_t("Couldn't open EID File", __LINE__, __FILE__);
      }

      while (file >> id)
      {
        scaff_bank.fetch(id, scaffold);
        bool changed = normalizeScaffold(scaffold, contig_bank);
        if (changed) { scaff_bank.replace(id, scaffold); }
      }
    }
    else if (!OPT_IIDFile.empty())
    {
      file.open(OPT_IIDFile.c_str());

      if (!file)
      {
        throw Exception_t("Couldn't open IID File", __LINE__, __FILE__);
      }

      while (file >> id)
      {
        scaff_bank.fetch(atoi(id.c_str()), scaffold);
        bool changed = normalizeScaffold(scaffold, contig_bank);
        if (changed) { scaff_bank.replace(scaffold.getIID(), scaffold); }
      }
    }
    else
    {
      AMOS::IDMap_t::const_iterator si;
      for (si = scaff_bank.getIDMap().begin(); si; si++)
      {
        scaff_bank.fetch(si->iid, scaffold);
        bool changed = normalizeScaffold(scaffold, contig_bank);
        if (changed) { scaff_bank.replace(scaffold.getIID(), scaffold); }
      }
    }

    if (!contigFeaturesToFlip.empty())
    {
      Feature_t feat;

      feat_bank.open(OPT_BankName, B_READ | B_WRITE);

      while (feat_bank >> feat)
      {
        ID_t bid = feat_bank.tellg()-1;
        if  (feat.getSource().second == Contig_t::NCODE)
        {
          map<ID_t, int>::iterator mi = contigFeaturesToFlip.find(feat.getSource().first);

          if (mi != contigFeaturesToFlip.end())
          {
            Range_t rng = feat.getRange();
            rng.begin = mi->second - rng.begin - 1;
            rng.end = mi->second - rng.end - 1;

            feat.setRange(rng);
          }

          feat_bank.replaceByBID(bid, feat);
        }
      }
    }

    feat_bank.close();
    contig_bank.close();
    scaff_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}
