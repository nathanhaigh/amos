#include "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"

#include  <string>
#include  <vector>
using namespace std;
using namespace HASHMAP;


#define  DEBUG  0

const int  MAX_LINE = 1000;
typedef  long long unsigned  Mer_t;

typedef hash_map<Mer_t, unsigned short, hash<unsigned long> > MerTable_t;


static Mer_t  Filled_Mask = Mer_t (1) << (8 * sizeof (Mer_t) - 1);
static Mer_t  Extract_Mask = 0;
static Mer_t  Forward_Mask;
static int  Kmer_Len = 0;

static unsigned  Char_To_Binary (char ch);
static void Fasta_To_Binary (const string & s, Mer_t & mer);
static void Forward_Add_Ch (Mer_t & mer, char ch);
static void Reverse_Add_Ch (Mer_t & mer, char ch);
static void Parse_Command_Line (int argc, char * argv []);
static void Print_Mer_Coverage (const string & s, double & percent_covered);
static void Read_Mers (const char * fname, MerTable_t & mer_table);
static void Usage (const char * command);
static char BinaryToAscii(char b);
static void MerToAscii(Mer_t mer, string & s);
static void CountMers (const string & s, MerTable_t & mer_table);
static void PrintMers(const MerTable_t & mer_table);




int  main (int argc, char * argv [])
{

  Parse_Command_Line(argc, argv);
  MerTable_t mer_table;

  cerr << "Processing sequences... " << endl;

  int count = 0;
  string s, tag;

  while  (Fasta_Read (stdin, s, tag))
  {
    fprintf (stderr, ">%s\n", tag . c_str ());
    CountMers(s, mer_table);
    count++;
  }

  cerr << count << " sequences processed" << endl;

  PrintMers(mer_table);

  return  0;
}

static char BinaryToAscii(char b)
{
  switch(b)
  {
    case 0: return 'A';
    case 1: return 'C';
    case 2: return 'G';
    case 3: return 'T';
  }

  return '*';
}

static void MerToAscii(Mer_t mer, string & s)
{
  s.erase();

  for (int i = 0; i < Kmer_Len; i++)
  {
    char a = BinaryToAscii(mer & 0x3);
    mer >>= 2;

    s.append(1, a);
  }

  reverse(s.begin(), s.end());
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



static void  Fasta_To_Binary
    (const string & s, Mer_t & mer)

//  Convert string  s  to its binary equivalent in  mer .

  {
   int  i, n;

   n = s . length ();
   mer = 0;
   for  (i = 0;  i < n;  i ++)
     {
      mer <<= 2;
      mer |= Char_To_Binary (s [i]);
     }

   return;
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







static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "h")) != EOF))
     switch  (ch)
       {
        case  'h' :
          errflg = true;
          break;

        case  '?' :
          fprintf (stderr, "Unrecognized option -%c\n", optopt);

        default :
          errflg = true;
       }

   if  (errflg)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Kmer_Len = 22;
   Forward_Mask = ((long long unsigned) 1 << (2 * Kmer_Len - 2)) - 1;

   return;
  }



static void  CountMers (const string & s, MerTable_t & mer_table)
{
   Mer_t  fwd_mer = 0, rev_mer = 0;
   int  i, j, n;

   n = s . length ();

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


void PrintMers(const MerTable_t & mer_table)
{
  cerr << "Printing " << mer_table.size() << " mers" << endl;
  string mer;

  MerTable_t::const_iterator fi;
  for (fi = mer_table.begin(); fi != mer_table.end(); fi++)
  {
    MerToAscii(fi->first, mer);
    printf(">%d\n%s\n", fi->second, mer.c_str());
  }
}








static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  count-mers < fasta > mercounts\n"
           "\n"
           "Count 22-mers in a multifasta file\n"
           "\n"
           "Options:\n"
           "  -h      Print this usage message\n"
           "\n");

   return;
  }



