#include "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"
#include "AMOS_Foundation.hh"

#include  <string>
#include  <vector>
using namespace std;
using namespace HASHMAP;
using namespace AMOS;

int COUNT = 0;
int LEN = 0;


#define  DEBUG  0

typedef long long unsigned Mer_t;
typedef hash_map<Mer_t, unsigned int, hash<unsigned long> > MerTable_t;

static Mer_t Filled_Mask = Mer_t (1) << (8 * sizeof (Mer_t) - 1);
static Mer_t Extract_Mask = 0;
static Mer_t Forward_Mask;
static int   Kmer_Len = 22;

static unsigned Char_To_Binary (char ch);
static void Forward_Add_Ch (Mer_t & mer, char ch);
static void Reverse_Add_Ch (Mer_t & mer, char ch);
static void Read_Mers (const char * fname, MerTable_t & mer_table);
static void MerToAscii(Mer_t mer, string & s);
static void CountMers (const string & s, MerTable_t & mer_table);
static void PrintMers(const MerTable_t & mer_table, int min_count);




int  main (int argc, char * argv [])
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version = "Version 1.0";
    string helptext = 
"Count kmers in a multifasta file or in read or contig banks.\n"
"Output is to stdout in \"meryl-style\": >count\\nmer\\n\n"
"\n"
" Usage: count-kmers [-f fasta] [-r bnk] [-c bnk]\n"
"\n"
"   -f <fasta> multifasta file to count\n"
"   -r <bnk>   Bank of reads to count\n"
"   -c <bnk>   Bank of contigs to count\n"
"   -k <len>   Length of kmer (default:22, must be <= 31) \n"
"   -m <min>   Minimum count to report (default: 1)\n"
"\n";

    string fastafile;
    string readbank;
    string contigbank;

    int min_count = 1;

    tf = new AMOS_Foundation(version, helptext, "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("f=s", &fastafile);
    tf->getOptions()->addOptionResult("r=s", &readbank);
    tf->getOptions()->addOptionResult("c=s", &contigbank);
    tf->getOptions()->addOptionResult("k=i", &Kmer_Len);
    tf->getOptions()->addOptionResult("m=i", &min_count);

    tf->handleStandardOptions();


    if (fastafile.empty() && readbank.empty() && contigbank.empty())
    {
      cerr << "You must specify an input source" << endl;
      exit(1);
    }

    if (Kmer_Len > 31 || Kmer_Len < 1)
    {
      cerr << "Kmer length must be <= 31" << endl;
      exit(1);
    }

    Forward_Mask = ((long long unsigned) 1 << (2 * Kmer_Len - 2)) - 1;

    MerTable_t mer_table;

    if (!fastafile.empty())
    {
      cerr << "Processing sequences in " << fastafile << "..." << endl;

      FILE * fp = fopen(fastafile.c_str(), "r");

      if (!fp)
      {
        cerr << "Couldn't open " << fastafile << endl;
        exit(1);
      }

      string s, tag;

      while  (Fasta_Read (fp, s, tag))
      {
        CountMers(s, mer_table);
      }
    }
    else if (!readbank.empty())
    {
      cerr << "Processing reads in " << readbank << "..." << endl;
      BankStream_t bank(Read_t::NCODE);
      bank.open(readbank, B_READ);

      Read_t red;
      while (bank >> red)
      {
        CountMers(red.getSeqString(red.getClearRange()), mer_table);
      }
    }
    else if (!contigbank.empty())
    {
      cerr << "Processing contigs in " << contigbank << "..." << endl;
      BankStream_t bank(Contig_t::NCODE);
      bank.open(contigbank, B_READ);

      Contig_t contig;
      while (bank >> contig)
      {
        CountMers(contig.getUngappedSeqString(), mer_table);
      }
    }

    cerr << COUNT << " sequences processed, " << LEN << " bp scanned" << endl;

    PrintMers(mer_table, min_count);
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
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

  return retval;
}

const char * bintoascii = "ACGT";
static void MerToAscii(Mer_t mer, string & s)
{
  s.erase();
  s.resize(Kmer_Len);

  for (int i = 0; i < Kmer_Len; i++)
  {
    s[Kmer_Len-i-1] = bintoascii[mer & 0x3];
    mer >>= 2;
  }
}



static unsigned  Char_To_Binary
    (char ch)

//  Return the binary equivalent of  ch .

  {
   switch  (tolower (ch))
     {
      case  'a' :
      case  'n' :
        return  0;
      case  'c' :
        return  1;
      case  'g' :
        return  2;
      case  't' :
        return  3;
      default :
        sprintf (Clean_Exit_Msg_Line, "Bad char = %c (ASCII %u) in Char_To_Binary",
             ch, unsigned (ch));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
     }
   return  0;
  }



static void  Forward_Add_Ch
    (Mer_t & mer, char ch)

//  Add  ch  to  mer  on the right, sliding one character
//  off the left end of  mer .

  {
   mer &= Forward_Mask;
   mer <<= 2;
   mer |= Char_To_Binary (ch);

   return;
  }

static void  Reverse_Add_Ch
    (Mer_t & mer, char ch)

//  Add the Watson-Crick complement of  ch  to  mer  on the left,
//  sliding one character off the right end of  mer .

  {
   mer >>= 2;
   mer |= ((long long unsigned) (3 ^ Char_To_Binary (ch)) << (2 * Kmer_Len - 2));

   return;
  }


static void  CountMers (const string & s, MerTable_t & mer_table)
{
   Mer_t  fwd_mer = 0, rev_mer = 0;
   int  i, j, n;

   n = s . length ();

   COUNT++;
   LEN+=n;

   if  (n < Kmer_Len) { return; }

   for  (i = 0;  i < Kmer_Len - 1;  i ++)
   {
     Forward_Add_Ch (fwd_mer, s [i]);
     Reverse_Add_Ch (rev_mer, s [i]);
   }

   MerTable_t::iterator fi;

   while (i < n)
   {
     Forward_Add_Ch (fwd_mer, s [i]);
     Reverse_Add_Ch (rev_mer, s [i]);

     if (fwd_mer < rev_mer)
     {
       fi = mer_table.find(fwd_mer);
       if (fi == mer_table.end()) { fi=mer_table.insert(make_pair(fwd_mer,0)).first; }
     }
     else
     {
       fi = mer_table.find(rev_mer);
       if (fi == mer_table.end()) { fi=mer_table.insert(make_pair(rev_mer,0)).first; }
     }

     fi->second++;

     i++;
   }

   return;
}


void PrintMers(const MerTable_t & mer_table, int min_count)
{
  cerr << mer_table.size() << " total distinct mers" << endl;
  string mer;
  int printed = 0;

  MerTable_t::const_iterator fi;
  for (fi = mer_table.begin(); fi != mer_table.end(); fi++)
  {
    if (fi->second >= min_count)
    {
      MerToAscii(fi->first, mer);
      printf(">%d\n%s\n", fi->second, mer.c_str());
      printed++;
    }
  }

  cerr << printed << " mers occur at least " << min_count << " times" << endl;
}

