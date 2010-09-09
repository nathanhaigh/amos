#include "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"
#include "AMOS_Foundation.hh"

#include  <string>
#include  <vector>
using namespace std;
using namespace HASHMAP;
using namespace AMOS;

extern char **environ;

long long COUNT = 0;
long long LEN = 0;
int BAD_CHAR = 0;

// always print as "mer count" rather than meryl format
int PRINT_SIMPLE = 1;


#undef BIGMER

#ifdef BIGMER
typedef vector<bool> Mer_t;
typedef map<Mer_t, double> MerTable_t;
#else
typedef long long unsigned Mer_t;
typedef hash_map<Mer_t, double, hash<unsigned long> > MerTable_t;
#endif

static Mer_t Forward_Mask;
static int   Kmer_Len = 22;

// limit size
static int bytes_per_kmer = 42;
static float gb_limit = 0;

static unsigned Char_To_Binary (char ch);
static void Forward_Add_Ch (Mer_t & mer, char ch);
static void Reverse_Add_Ch (Mer_t & mer, char ch);
static void MerToAscii(Mer_t mer, string & s);
static void CountMers (const string & s, const string & q, MerTable_t & mer_table);
static void PrintMers(const MerTable_t & mer_table, int min_count);

// mine
static bool Fastq_Read(FILE * fp, string & s, string & hdr, string & q);


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
"  count-kmers-q [-f fasta] [-r bnk] [-c bnk] [-n bnk]\n"
"\n"
".DESCRIPTION.\n"
"  Count kmers in a fastq file or in read or contig banks.\n"
"  Output is to stdout in simple nmer count format: mer count\n"
"\n.OPTIONS.\n"
"  -f <fastq> multifasta file to count\n"
"  -r <bnk>   Bank of reads to count\n"
"  -c <bnk>   Bank of contigs to count\n"
"  -n <bnk>   Report normalized counts (readmercount/contigmercount)\n"
"  -k <len>   Length of kmer \n"
"  -m <min>   Minimum count to report (default: 1)\n"
"  -S         Print using simple nmer count format: mer count\n"
"  -l <limit> Gigabyte limit on RAM. If limited, the output will contain redundancies\n"
"\n.KEYWORDS.\n"
"  kmers, fasta\n";

    string fastqfile = "-";
    string readbank;
    string contigbank;
    string normalizedbank;

    int min_count = 0;
    int show_env = 0;

    tf = new AMOS_Foundation(version, helptext, "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("f=s", &fastqfile);
    tf->getOptions()->addOptionResult("r=s", &readbank);
    tf->getOptions()->addOptionResult("c=s", &contigbank);
    tf->getOptions()->addOptionResult("n=s", &normalizedbank);
    tf->getOptions()->addOptionResult("k=i", &Kmer_Len);
    tf->getOptions()->addOptionResult("m=i", &min_count);
    tf->getOptions()->addOptionResult("S",   &PRINT_SIMPLE);
    tf->getOptions()->addOptionResult("l=f", &gb_limit);    
    tf->getOptions()->addOptionResult("e",   &show_env);    

    tf->handleStandardOptions();

    /*
    if ((fastafile.empty() +
         readbank.empty() +
         contigbank.empty() +
         normalizedbank.empty()) != 3)
    {
      cerr << "You must specify one input source" << endl;
      //exit(1);
    }
    */

    if (show_env)
    {
      char **env;
      for (env = environ; *env != NULL; ++env)
        fprintf(stderr, "%s\n", *env);
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

    /*
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
	//cerr << ".";
      }
      cerr << endl;
    }
    */

    FILE * fp;
    if(fastqfile == "-")
      fp = stdin;
    else {
      fp = fopen(fastqfile.c_str(), "r");
      if (!fp)
      {
        cerr << "Couldn't open " << fastqfile << endl;
        exit(1);
      }
    }

    cerr << "Processing sequences..." << endl;

    string s, q, tag;
    unsigned long mb_limit = (unsigned long)(1024.0*gb_limit);
    unsigned long kmer_limit = mb_limit * 1048576UL / (unsigned long)bytes_per_kmer;

    //while(Fasta_Read(stdin, s, tag))
    while(Fastq_Read(fp, s, tag, q)) {
      CountMers(s, q, mer_table);
      if(gb_limit > 0 && mer_table.size() > kmer_limit) {
	// print table
	cerr << COUNT << " sequences processed, " << LEN << " bp scanned" << endl;
    fprintf(stderr, "reporter:counter:asm,flush,1\n");
	PrintMers(mer_table, min_count);
	// clear table
	mer_table.clear();
      }
    }

    cerr << COUNT << " sequences processed, " << LEN << " bp scanned" << endl;
    fprintf(stderr, "reporter:counter:asm,flush,1\n");

    if (BAD_CHAR)
    {
      cerr << "WARNING: Input had " << BAD_CHAR << " non-DNA (ACGT) characters" << endl;
    }

    PrintMers(mer_table, min_count);

    fprintf(stderr, "reporter:counter:asm,reads_total,%ld\n", COUNT);
    fprintf(stderr, "reporter:counter:asm,reads_bp,%ld\n",    LEN);
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
      fprintf(stderr, "reporter:counter:asm,error,1\n");
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


////////////////////////////////////////////////////////////
// CountMers
//
// I edited this function to detect non ACGT's and ignore
// the Kmer_Len affected kmers.
////////////////////////////////////////////////////////////
static void  CountMers (const string & s, const string & q, MerTable_t & mer_table)
{
   Mer_t  fwd_mer, rev_mer;
   int  i, j, n;
   int non_acgt_buffer = 0;

   // convert quality values
   vector<double> quals;
   double quality = 1.0;
   for(i = 0; i < q.size(); i++)
     quals.push_back(max(.25, 1.0-pow(10.0,-(q[i]-33)/10.0)));
     //quals.push_back(max(.25, 1.0-pow(10.0,-(q[i]-64)/10.0)));

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

     quality *= quals[i];

     if(strchr(bintoascii, s[i]) == NULL)
       non_acgt_buffer = Kmer_Len;
     else if(non_acgt_buffer > 0)
       non_acgt_buffer--;
   }

   MerTable_t::iterator fi;

   while (i < n)
   {
     Forward_Add_Ch (fwd_mer, s [i]);
     Reverse_Add_Ch (rev_mer, s [i]);

     if(i == Kmer_Len-1)
       quality *= quals[i];
     else
       quality *= (quals[i] / quals[i - Kmer_Len]);

     if(strchr(bintoascii, s[i]) == NULL)
       non_acgt_buffer = Kmer_Len;
     else if(non_acgt_buffer > 0)
       non_acgt_buffer--;

     if(non_acgt_buffer == 0) {
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

       fi->second += quality;
     }
     
     i++;
   }

   return;
}


void PrintMers(const MerTable_t & mer_table, int min_count)
{
  cerr << mer_table.size() << " total distinct mers" << endl;
  string mer;
  int printed = 0;
  int skip = 0;

  MerTable_t::const_iterator fi;
  for (fi = mer_table.begin(); fi != mer_table.end(); fi++)
  {
    if (fi->second > min_count)
    {
      MerToAscii(fi->first, mer);
      if (PRINT_SIMPLE)
      {
        printf("%s\t%f\n", mer.c_str(), fi->second);
      }
      else
      {
        printf(">%f\n%s\n", fi->second, mer.c_str());
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

bool Fastq_Read(FILE * fp, string & s, string & hdr, string & q)
{
  int ch;

  s.erase();
  hdr.erase();
  q.erase();

  // skip till next '@' if necessary
  while  ((ch = fgetc (fp)) != EOF && ch != '@')
    ;

  if(ch == EOF)
    return false;

  // skip spaces if any
  while  ((ch = fgetc (fp)) != EOF && ch == ' ')
    ;
  if  (ch == EOF)
    return  false;
  ungetc (ch, fp);

  // put rest of line into  hdr
  while  ((ch = fgetc (fp)) != EOF && ch != '\n')
    hdr . push_back (char (ch));

  // put everything up till next '\n' into  s
  while  ((ch = fgetc (fp)) != EOF && ch != '\n')
  {
    if  (! isspace (ch))
      s . push_back (char (ch));
  }

  // skep next line
  while  ((ch = fgetc (fp)) != EOF && ch != '\n')
    ;

  // put everything up till next '\n' into  q
  while  ((ch = fgetc (fp)) != EOF && ch != '\n')
  {
    if  (! isspace (ch))
      q . push_back (char (ch));
  }
  
  if  (ch == '@')
    ungetc (ch, fp);
  
  return  true;
}
