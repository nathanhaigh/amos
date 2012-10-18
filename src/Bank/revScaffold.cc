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
       << "\n"
       << "-e eid  Flip scaffold eid\n"
       << "-i iid  Flip scaffold iid\n"
       << "-E file Flip the scaffolds eids listed in file\n"
       << "-I file Flip the scaffolds iids listed in file\n\n"
       << endl;
  
  cerr << "Reverse the specified scaffold(s) including underlying contigs\n"
       << "Doesn't update scaffold features, nor contig links\n"
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
      case 'e': OPT_scaffeid = optarg; break;
      case 'i': OPT_scaffiid = atoi(optarg); break;

      default: errflg ++;
      }
  }
  cerr << "Done" << endl;

  if ( errflg > 0 || optind != argc - 1 )
  {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
  }

  OPT_BankName = argv [optind];
}


void flipScaffold(Scaffold_t & scaff)
{
  vector<Tile_t> & contigs = scaff.getContigTiling();
  vector<Tile_t>::iterator ci;

  sort(contigs.begin(), contigs.end(), TileOrderCmp());
  reverse(contigs.begin(), contigs.end());

  int newoffset = 0;
  int newgap = 0;

  int span = contigs.begin()->getRightOffset();

  for (ci = contigs.begin(); ci != contigs.end(); ci++)
  {
    ci->offset = span - ci->getRightOffset();
    ci->range.swap();
  }
}




int main (int argc, char ** argv)
{
  ParseArgs (argc, argv);

  Bank_t scaff_bank(Scaffold_t::NCODE);

  cerr << "Processing " << OPT_BankName << " at " << Date() << endl;

  try
  {
    scaff_bank.open(OPT_BankName, B_READ | B_WRITE);

    Scaffold_t scaffold;
    ifstream file;
    string id;

    if (!OPT_scaffeid.empty()) { OPT_scaffiid = scaff_bank.lookupIID(OPT_scaffeid); }

    if (OPT_scaffiid)
    {
      scaff_bank.fetch(OPT_scaffiid, scaffold);
      flipScaffold(scaffold);
      scaff_bank.replace(OPT_scaffiid, scaffold);
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
        flipScaffold(scaffold);
        scaff_bank.replace(id, scaffold);
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
        flipScaffold(scaffold);
        scaff_bank.replace(scaffold.getIID(), scaffold);
      }
    }
    else
    {
      cerr << "No Scaffold specified!" << endl;
    }

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
