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
int BAD_CHAR = 0;

int PRINT_SIMPLE = 0;
int PRINT_STATS = 0;


#undef BIGMER
//#define BIGMER 1

#ifdef BIGMER
typedef vector<bool> Mer_t;
typedef map<Mer_t, unsigned int> MerTable_t;
#else
typedef long long unsigned Mer_t;
typedef hash_map<Mer_t, unsigned int, hash<unsigned long> > MerTable_t;
#endif

static Mer_t Forward_Mask;
static int   Kmer_Len = 22;

static unsigned Char_To_Binary (char ch);
static void Forward_Add_Ch (Mer_t & mer, char ch);
static void Reverse_Add_Ch (Mer_t & mer, char ch);
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
"\n"
".USAGE.\n"
"  count-kmers [-f fasta] [-r bnk] [-c bnk] [-n bnk]\n"
"\n"
".DESCRIPTION.\n"
"  Count kmers in a multifasta file or in read or contig banks.\n"
"  Output is to stdout in \"meryl-style\": >count\\nmer\\n\n"
"\n.OPTIONS.\n"
"  -f <fasta> multifasta file to count\n"
"  -r <bnk>   Bank of reads to count\n"
"  -c <bnk>   Bank of contigs to count\n"
"  -n <bnk>   Report normalized counts (readmercount/contigmercount)\n"
"  -k <len>   Length of kmer \n"
"  -m <min>   Minimum count to report (default: 1)\n"
"  -S         Print using simple nmer count format: mer count\n"
"  -s         Just print statistics on unique mers\n";
"\n.KEYWORDS.\n"
"  kmers, fasta\n";

    string fastafile;
    string readbank;
    string contigbank;
    string normalizedbank;

    int min_count = 1;

    tf = new AMOS_Foundation(version, helptext, "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("f=s", &fastafile);
    tf->getOptions()->addOptionResult("r=s", &readbank);
    tf->getOptions()->addOptionResult("c=s", &contigbank);
    tf->getOptions()->addOptionResult("n=s", &normalizedbank);
    tf->getOptions()->addOptionResult("k=i", &Kmer_Len);
    tf->getOptions()->addOptionResult("m=i", &min_count);
    tf->getOptions()->addOptionResult("S",   &PRINT_SIMPLE);
    tf->getOptions()->addOptionResult("s",   &PRINT_STATS);

    tf->handleStandardOptions();


    if ((fastafile.empty() +
         readbank.empty() +
         contigbank.empty() +
         normalizedbank.empty()) != 3)
    {
      cerr << "You must specify one input source" << endl;
      exit(1);
    }


    #ifndef BIGMER
    if (Kmer_Len > 31 || Kmer_Len < 1)
    {
      cerr << "Kmer length must be <= 31" << endl;
      exit(1);
    }
    Forward_Mask = ((long long unsigned) 1 << (2 * Kmer_Len - 2)) - 1;
    #endif

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
        cerr << ".";
      }
      cerr << endl;
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
    else if (!normalizedbank.empty())
    {
      BankStream_t rbank(Read_t::NCODE);
      rbank.open(normalizedbank, B_READ);

      BankStream_t cbank(Contig_t::NCODE);
      cbank.open(normalizedbank, B_READ);

      cerr << "Processing reads in " << normalizedbank << "..." << endl;
      Read_t red;
      while (rbank >> red)
      {
        CountMers(red.getSeqString(red.getClearRange()), mer_table);
      }


      cerr << "Processing contigs in " << normalizedbank << "..." << endl;
      MerTable_t consmers;
      Contig_t contig;
      while (cbank >> contig)
      {
        CountMers(contig.getUngappedSeqString(), consmers);
      }


      cerr << "Normalizing counts" << endl;
      MerTable_t::iterator ri;
      MerTable_t::iterator ci;

      for (ri = mer_table.begin(); ri != mer_table.end(); ri++)
      {
        ci = consmers.find(ri->first);

        if (ci != consmers.end())
        {
          ri->second /= ci->second;
        }
        else
        {
          ri->second = 0;
        }
      }
    }

    cerr << COUNT << " sequences processed, " << LEN << " bp scanned" << endl;

    if (BAD_CHAR)
    {
      cerr << "WARNING: Input had " << BAD_CHAR << " non-DNA (ACGT) characters" << endl;
    }

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
  catch (...)
    {
      cerr << "uncaught exception\n"; 
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




//  Return the binary equivalent of  ch .
static unsigned  Char_To_Binary (char ch)
{
  switch  (tolower (ch))
  {
    case  'a' : return  0;
    case  'c' : return  1;
    case  'g' : return  2;
    case  't' : return  3;
    default: BAD_CHAR++; return 0;
  };

  return  0;
}


char RC(char ch)
{
  switch(toupper(ch))
  {
    case 'A': return 'T';
    case 'T': return 'A';
    case 'C': return 'G';
    case 'G': return 'C';

    default: return 'T';
  };

  return 0;
}

char NORM(char ch)
{
  switch(toupper(ch))
  {
    case 'A': return 'A';
    case 'C': return 'C';
    case 'G': return 'G';
    case 'T': return 'T';
    default: return 'A';
  };

  return 0;
}


#ifdef BIGMER

void InitMer(Mer_t & mer)
{
  mer.clear();
  mer.resize(Kmer_Len*2);
}

static void MerToAscii(Mer_t mer, string & s)
{
  s.erase();
  s.resize(Kmer_Len);

  for (int i = 0; i < Kmer_Len; i++)
  {
    char m = 0;
    m |= mer[i*2+1] << 1;
    m |= mer[i*2];

    s[Kmer_Len-i-1] = bintoascii[m];
  }
}

//  Add  ch  to  mer  on the right, sliding one character
//  off the left end of  mer .
static void  Forward_Add_Ch(Mer_t & mer, char ch)
{
  // shift left
  for (int i = Kmer_Len * 2-1; i > 1; i--)
  {
    mer[i] = mer[i-2];
  }

  // append to end
  ch = Char_To_Binary(ch);
  mer[1] = ch & 2;
  mer[0] = ch & 1;
}


//  Add the Watson-Crick complement of  ch  to  mer  on the left,
//  sliding one character off the right end of  mer .
static void  Reverse_Add_Ch(Mer_t & mer, char ch)
{
  // shift right
  for (int i = 0; i < Kmer_Len*2-2; i++)
  {
    mer[i] = mer[i+2];
  }

  // append to front
  ch = 3^Char_To_Binary(ch);
  mer[Kmer_Len*2-1] = ch&2;
  mer[Kmer_Len*2-2] = ch&1;
}


#else

void InitMer(Mer_t & mer)
{
  mer = 0;
}

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

//  Add  ch  to  mer  on the right, sliding one character
//  off the left end of  mer .
static void  Forward_Add_Ch(Mer_t & mer, char ch)
{
  mer &= Forward_Mask;
  mer <<= 2;
  mer |= Char_To_Binary (ch);
}

//  Add the Watson-Crick complement of  ch  to  mer  on the left,
//  sliding one character off the right end of  mer .
static void  Reverse_Add_Ch(Mer_t & mer, char ch)
{
  mer >>= 2;
  mer |= ((long long unsigned) (3 ^ Char_To_Binary (ch)) << (2 * Kmer_Len - 2));
}

#endif


static void  CountMers (const string & s, MerTable_t & mer_table)
{
   Mer_t  fwd_mer, rev_mer;
   int  i, j, n;

   InitMer(fwd_mer);
   InitMer(rev_mer);

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

     string f,r;

     MerToAscii(fwd_mer, f);
     MerToAscii(rev_mer, r);

     if (f < r)
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
  if (PRINT_STATS)
  {
    long long unique = 0;

    MerTable_t::const_iterator fi;
    for (fi = mer_table.begin(); fi != mer_table.end(); fi++)
    {
      if (fi->second == 1)
      {
        unique++;
      }
    }

    cout << "n="  << COUNT
         << " l=" << LEN
         << " k=" << Kmer_Len
         << " d=" << mer_table.size()
         << " u=" << unique
         << endl;
  }
  else
  {
    cerr << mer_table.size() << " total distinct mers" << endl;
    string mer;
    int printed = 0;
    int skip = 0;

    MerTable_t::const_iterator fi;
    for (fi = mer_table.begin(); fi != mer_table.end(); fi++)
    {
      if (fi->second >= min_count)
      {
        MerToAscii(fi->first, mer);
        if (PRINT_SIMPLE)
        {
          printf("%s\t%d\n", mer.c_str(), fi->second);
        }
        else
        {
          printf(">%d\n%s\n", fi->second, mer.c_str());
        }
        printed++;
      }
      else
      {
        skip++;
      }
    }

    cerr << printed << " mers occur at least " << min_count << " times" << endl;
    cerr << "Skipped " << skip << endl;
  }
}

