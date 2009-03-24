#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"

using namespace AMOS;
using namespace std;

bool OPT_UseEIDs = false;
bool OPT_UseIIDs = false;
bool OPT_FASTA = false;
int  OPT_Spacer = 100;
int  OPT_MinGapsize = 100;
bool OPT_UseExpected = false;
bool OPT_UseUngapped = false;
bool OPT_CreateLinks = false;
bool OPT_Join = false;

string OPT_BankName;

string OPT_EIDFile;
string OPT_IIDFile;

void PrintUsage (const char * s)
{
  cerr << "\n.USAGE.\n" << "  " <<  s << "  [options]  <bank path>\n";
}

//------------------------------------------------------------- PrintHelp ----//
void PrintHelp (const char * s)
{
  PrintUsage(s);
  cerr << "\n.DESCRIPTION.\n"
       << "  Takes an AMOS bank directory and prints the scaffolds to stdout.\n\n"
       << "  Default format:\n"
       << "  >scaffid numcontigs scaffbases scaffspan\n"
       << "  contigid orientation contiglen leftgapsize\n"
       << "\n.OPTIONS.\n"
       << "  -h      Display help information\n"
       << "  -v      Display the compatible bank version\n"
       << "  -e      Use EIDs for names (DEFAULT)\n"
       << "  -i      Use IIDs for names\n"
       << "  -E file Print just the contig eids listed in file\n"
       << "  -I file Print just the contig iids listed in file\n"
       << "  -u      Show the ungapped contig lengths in the scaff file\n"
       << "  -l      Create a Arachne links file\n"
       << "\n"
       << "  -f      Print the scaffold consensus sequences in multi-fasta format\n"
       << "  -g      Use the estimated gaps size to space contigs\n"
       << "  -j      Join contigs spaced by less than 2 bp (needs -f, if G provided overrides 2)\n"
       << "  -G val  Gaps < val will have val N's between them (-g)\n"
       << "  -N val  Specify number of N's to place between contigs\n"
       << "\n.KEYWORDS.\n"
       << "  bank, converters, scaffolds"
       << endl;
  
}

void ParseArgs (int argc, char ** argv)
{
  int ch, errflg = 0;
  optarg = NULL;

  while ( !errflg && ((ch = getopt (argc, argv, "hveiE:I:fN:gG:ulj")) != EOF) )
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

      case 'e': OPT_UseEIDs = true;   break;
      case 'i': OPT_UseIIDs = true;   break;
      case 'E': OPT_EIDFile = optarg; break;
      case 'I': OPT_IIDFile = optarg; break;

      case 'f': OPT_FASTA   = true;            break;
      case 'g': OPT_UseExpected = true;        break;
      case 'j': OPT_Join = true;               break;
      case 'G': OPT_MinGapsize = atoi(optarg); break;
      case 'N': OPT_Spacer  = atoi(optarg);    break;
      case 'u': OPT_UseUngapped = true; break;
      case 'l': OPT_CreateLinks = true; break;

      default: errflg ++;
      }
  }

  if (OPT_UseExpected) { OPT_FASTA = true; }

  if ( errflg > 0 || optind != argc - 1 )
  {
      PrintUsage (argv[0]);
      cerr << "Try '" << argv[0] << " -h' for more information.\n";
      exit (EXIT_FAILURE);
  }

  OPT_BankName = argv [optind];

  if (!OPT_UseEIDs && !OPT_UseIIDs) { OPT_UseEIDs = true; }
}


void printScaffold(Scaffold_t & scaff, Bank_t & contig_bank)
{
  vector<Tile_t> & contigs = scaff.getContigTiling();
  vector<Tile_t>::const_iterator ci;
  int chunk = 0;
  string names = "";

  sort(contigs.begin(), contigs.end(), TileOrderCmp());

  if (!OPT_CreateLinks)
  {
    cout << ">";
    if (OPT_UseEIDs) { cout << scaff.getEID(); }
    else             { cout << scaff.getIID(); }
    //    if (OPT_Join) {cout << "_" << ++chunk;}
  }

  int bases = 0, span = 0, numcontigs = contigs.size();

  for (ci = contigs.begin(); ci != contigs.end(); ci++)
  {
    bases += ci->getGappedLength();
    int right = ci->getRightOffset();

    if (right > span) { span = right; }
  }

  if (!OPT_CreateLinks && !OPT_Join)
  {
    cout << " " << contigs.size() << " " << bases << " " << span << endl;
  }

  string seq;
  Contig_t contig;

  int lastrightoffset;
  int leftgapsize = 0;
  int contignum = 0;

  for (ci = contigs.begin(); ci != contigs.end(); ci++)
  {
    contignum++;

    if (ci != contigs.begin())
    {
      leftgapsize = ci->offset - lastrightoffset;
    } //else if (OPT_Join) {
    //      names.append(contig_bank.getIDMap().lookupEID(ci->source));
    //}

    lastrightoffset = ci->getRightOffset();

    if (OPT_CreateLinks)
    {
      int rightgapsize = 0;
      vector<Tile_t>::const_iterator ni = ci;
      ni++;

      if (ni != contigs.end())
      {
        rightgapsize = ni->offset - lastrightoffset;
      }

      if (OPT_UseEIDs)
      {
        cout << scaff.getEID() << "\t" 
             << bases << "\t"
             << numcontigs << "\t"
             << contignum << "\t"
             << contig_bank.getIDMap().lookupEID(ci->source) << "\t"
             << ci->range.getLength() << "\t"
             << leftgapsize << "\t"
             << rightgapsize << "\t" << endl;
      }
      else
      {
        cout << scaff.getIID() << "\t" 
             << bases << "\t"
             << numcontigs << "\t"
             << contignum << "\t"
             << ci->source << "\t"
             << ci->range.getLength() << "\t"
             << leftgapsize << "\t"
             << rightgapsize << "\t" << endl;
      }
    }
    else if (OPT_FASTA)
    {
      if (ci != contigs.begin())
      {
        int spaces = OPT_Spacer;

        if (OPT_UseExpected)
        {
          spaces = leftgapsize;

          if (leftgapsize < OPT_MinGapsize)
          {
            spaces = OPT_MinGapsize;
          }
        }
	if (OPT_Join){
	  if (leftgapsize <= ((OPT_MinGapsize != 100)?OPT_MinGapsize:2))
	  {
	    //	    cerr << "Names is: " << names << " gap is " << leftgapsize <<endl;
	      names.append("+");
	      names.append(contig_bank.getIDMap().lookupEID(ci->source));
	  } else {
	    //	    cerr << "ElseNames is: " << names << " gap is " << leftgapsize <<endl;
	    ++chunk;
	    cout << "_" << chunk << " " << names << endl;
	    Fasta_Print(stdout, seq.c_str(), NULL, 60);
	    cout << ">";
	    if (OPT_UseEIDs) { cout << scaff.getEID(); }
	    else             { cout << scaff.getIID(); }
 	    
	    names = "";
	    names.append(contig_bank.getIDMap().lookupEID(ci->source));
	    seq = "";
	  }
	} else
	  seq.append(spaces, 'N');
      } else if (OPT_Join) {
	names.append(contig_bank.getIDMap().lookupEID(ci->source));
      }

      contig_bank.fetch(ci->source, contig);

      string s = contig.getUngappedSeqString();
      
      if (ci->range.isReverse()) { ReverseComplement(s); }

      seq.append(s);
    }
    else
    {
      if (ci != contigs.begin())
      {
        cout << " " <<  leftgapsize << endl;
      }

      if (OPT_UseEIDs)
      {
        cout << contig_bank.getIDMap().lookupEID(ci->source);
      }
      else
      {
        cout << ci->source;
      }

      if (ci->range.isReverse()) { cout << " EB "; }
      else                       { cout << " BE "; }

      if (OPT_UseUngapped)
      {
        contig_bank.fetch(ci->source, contig);
        string s = contig.getUngappedSeqString();
        cout << s.length();
      }
      else
      {
        cout << ci->range.getLength();
      }
    }
  }

  if (OPT_FASTA)
  {
    if (!OPT_Join)
      Fasta_Print(stdout, seq.c_str(), NULL, 60);
    else {
      if (! seq.empty()){
	cout << "_" << ++chunk << " " << names <<endl;
	Fasta_Print(stdout, seq.c_str(), NULL, 60);
      }
    }
  }
  else if (!OPT_CreateLinks)
  {
    leftgapsize = 0; // No gap after last contig
    cout << " " <<  leftgapsize << endl;
  }
}




int main (int argc, char ** argv)
{
  ParseArgs (argc, argv);

  BankStream_t scaff_bank(Scaffold_t::NCODE);
  Bank_t contig_bank(Contig_t::NCODE);

  cerr << "Processing " << OPT_BankName << " at " << Date() << endl;

  if (OPT_CreateLinks)
  {
    cout << "#super_id	num_bases_in_super	num_contigs_in_super	ordinal_num_of_contig_in_super	contig_id	length_of_contig	estimated_gap_before_contig estimated_gap_after_contig" << endl;
  }

  try
  {
    scaff_bank.open(OPT_BankName, B_READ);
    contig_bank.open(OPT_BankName, B_READ);

    Scaffold_t scaffold;
    ifstream file;
    string id;

    if (!OPT_EIDFile.empty())
    {
      file.open(OPT_EIDFile.c_str());
      
      if (!file)
      {
        throw Exception_t("Couldn't open EID File", __LINE__, __FILE__);
      }

      while (file >> id)
      {
        scaff_bank.seekg(scaff_bank.getIDMap().lookupBID(id));
        scaff_bank >> scaffold;
        printScaffold(scaffold, contig_bank);
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
        scaff_bank.seekg(scaff_bank.getIDMap().lookupBID(atoi(id.c_str())));
        scaff_bank >> scaffold;
        printScaffold(scaffold, contig_bank);
      }
    }
    else
    {
      while (scaff_bank >> scaffold)
      {
        printScaffold(scaffold, contig_bank);
      }
    }

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
