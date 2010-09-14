//  A. L. Delcher
//
//  File:  kmer-cov.cc
//
//  Last Modified:  22 March 2004
//

#include "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"

#include  <string>
#include  <vector>
using namespace std;
using namespace HASHMAP;


#define  DEBUG  0

bool OPT_DisplayKmers = false;


const int  MAX_LINE = 1000;
typedef  long long unsigned  Mer_t;

typedef hash_map<Mer_t, unsigned int, hash<unsigned long> > MerTable_t;


static Mer_t  Filled_Mask = Mer_t (1) << (8 * sizeof (Mer_t) - 1);
static Mer_t  Extract_Mask = 0;
static Mer_t  Forward_Mask;
static int  Kmer_Len = 0;
static string  Kmer_File_Name;
  // Name of file kmers

static unsigned  Char_To_Binary (char ch);
static void  Fasta_To_Binary (const string & s, Mer_t & mer);
static void  Forward_Add_Ch (Mer_t & mer, char ch);
static void  Reverse_Add_Ch (Mer_t & mer, char ch);
static void  Parse_Command_Line (int argc, char * argv []);
static void  Print_Mer_Coverage (const string & s, double & percent_covered);
static void  Read_Mers (const char * fname, MerTable_t & mer_table);
static void  Usage (const char * command);
static void  Compute_Mer_Coverage (const string & s, const MerTable_t & mer_table);
static char BinaryToAscii(char b);
static void MerToAscii(Mer_t mer, string & s);




int  main (int argc, char * argv [])
{
  FILE  * unique_fp, * repeat_fp, * unsure_fp;
  MerTable_t mer_table;
 
  string  s, tag;

  Parse_Command_Line (argc, argv);

  cerr << "Loading mer counts... ";
  Read_Mers (Kmer_File_Name . c_str (), mer_table);
  cerr << mer_table.size() << " mers loaded" << endl;

  cerr << "Processing sequence... ";

  int count = 0;

  while  (Fasta_Read (stdin, s, tag))
  {
    printf (">%s\n", tag . c_str ());
    Compute_Mer_Coverage(s, mer_table);
    count++;
  }

  cerr << count << " sequences processed" << endl;

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
      default:
        return  0;
      case  'c' :
        return  1;
      case  'g' :
        return  2;
      case  't' :
        return  3;
        /*
      default :
        sprintf (Clean_Exit_Msg_Line, "Bad char = %c (ASCII %u) in Char_To_Binary",
             ch, unsigned (ch));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
        */
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






static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "hK")) != EOF))
     switch  (ch)
       {

         case 'K' : OPT_DisplayKmers = true; break;

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

   if  (optind > argc - 1)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Kmer_File_Name = argv [optind ++];

   return;
  }



static void  Compute_Mer_Coverage (const string & s, const MerTable_t & mer_table)
{
   Mer_t  fwd_mer = 0, rev_mer = 0;
   int  i, j, n;

   n = s . length ();

   if  (n < Kmer_Len)
   {
     for (int i = 0; i < n; i++)
     {
       printf("%d 0 0\n", i);
     }

     return;
   }

   for  (i = 0;  i < Kmer_Len - 1;  i ++)
   {
     Forward_Add_Ch (fwd_mer, s [i]);
     Reverse_Add_Ch (rev_mer, s [i]);
   }

   string fmer, rmer;
   MerTable_t::const_iterator fi, ri;
   unsigned int fcount = 0;
   unsigned int rcount = 0;

   for  (j = 0;  i < n;  i ++, j ++)
   {
     Forward_Add_Ch (fwd_mer, s [i]);
     Reverse_Add_Ch (rev_mer, s [i]);

     fcount = 0;
     rcount = 0;

     fi = mer_table.find(fwd_mer);
     ri = mer_table.find(rev_mer);

     if (fi != mer_table.end()) { fcount = fi->second; }
     if (ri != mer_table.end()) { rcount = ri->second; }

     unsigned int mcount = (fcount > rcount) ? fcount : rcount;

     if (OPT_DisplayKmers)
     {
       MerToAscii(fwd_mer, fmer);
       MerToAscii(rev_mer, rmer);

       printf("%d\t%d\t%d\t%d\t%s\t%s\n", 
              i-Kmer_Len+1, mcount, fcount, rcount, 
              fmer.c_str(), rmer.c_str()); 
     }
     else
     {
       printf("%d\t%d\t%d\t%d\n", 
              i-Kmer_Len+1, mcount, fcount, rcount);
     }
   }

   return;
}



static void  Read_Mers (const char * fname, MerTable_t & mer_table)

//  Read kmers from file name  fname  and save them
//  in binary form in  mer_list .  Input format is
//  a multi-fasta file.  Mers are assumed to contain only
//  ACGT's

{
  FILE  * fp;
  string  s, tag;
  Mer_t  mer;

  fp = File_Open (fname, "r", __FILE__, __LINE__);

  mer_table . clear ();

  while  (Fasta_Read (fp, s, tag))
  {
    unsigned int mercount = atol(tag.c_str());

    if  (Kmer_Len == 0)
    {
      Kmer_Len = s . length ();
    }
    else if  (Kmer_Len != int (s . length ()))
    {
      sprintf (Clean_Exit_Msg_Line, "New kmer \"%s\" length is %d instead of %d",
               s . c_str (), s . length (), Kmer_Len);
      Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
    }
    Fasta_To_Binary (s, mer);

 //  MerToAscii(mer, tag);
 //   fprintf(stderr, "orig: %s mer: %032llx asc: %s\n", s.c_str(), mer, tag.c_str());

    mer_table.insert(make_pair(mer, mercount));
   }

   Forward_Mask = ((long long unsigned) 1 << (2 * Kmer_Len - 2)) - 1;

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



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  kmer-cov-plot  <kmer-file>\n"
           "\n"
           "Read a list of short kmers (31 bases or less) from <kmer-file>\n"
           "and a fasta file on stdin and print the kmer coverage at each \n"
           "position (prints max (forkmercov, revkmercov))\n"
           "\n"
           "Options:\n"
           "  -h      Print this usage message\n"
           "  -K      Display the actual kmer in addtion to the counts\n"
           "\n");

   return;
  }



