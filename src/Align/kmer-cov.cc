//  A. L. Delcher
//
//  File:  kmer-cov.cc
//
//  Last Modified:  22 March 2004
//
//  Read a list of short kmers (31 bases or less) and then
//  compute what regions of the fasta sequences read from
//  stdin are covered by them (or their reverse complement).


#include  "delcher.hh"
#include  "fasta.hh"
#include  <string>
#include  <vector>
using namespace std;


#define  DEBUG  0


const int  MAX_LINE = 1000;
const double  DEFAULT_UNIQUE_CUTOFF = 50.0;
  // Default value for global  Unique_Cutoff
const double  DEFAULT_REPEAT_CUTOFF = 90.0;
  // Default value for global  Repeat_Cutoff

typedef  long long unsigned  Mer_t;

bool OPT_Features = false;
bool OPT_AllowAmbiguity = false;
int MIN_LEN  = 0;


static Mer_t  Filled_Mask = Mer_t (1) << (8 * sizeof (Mer_t) - 1);
static Mer_t  Extract_Mask = 0;
static Mer_t  Forward_Mask;
static Mer_t  Hash_Mask;
static int  Hash_Shift = 0;
static Mer_t  * Hash_Table = NULL;
static int  Hash_Table_Size = 0;
static int  Kmer_Len = 0;
static string  Kmer_File_Name;
  // Name of file kmers
static bool  Make_Fasta = false;
  // If set true by the -f option, also generate fasta files
  // of sequences that are unique, repeat and unsure.
static double  Repeat_Cutoff = DEFAULT_REPEAT_CUTOFF;
  // Strings covered >= this percent are classified repeat
  // Inbetween is unsure.
static double  Unique_Cutoff = DEFAULT_UNIQUE_CUTOFF;
  // Strings covered < this percent are classified unique


static void  Build_Hash_Table
    (const vector <Mer_t> & mer_list);
static unsigned  Char_To_Binary
    (char ch);
static void  Fasta_To_Binary
    (const string & s, Mer_t & mer);
static void  Forward_Add_Ch
    (Mer_t & mer, char ch);
static void  Reverse_Add_Ch
    (Mer_t & mer, char ch);
static int  Hash
    (Mer_t mer);
static bool  Hash_Find
    (Mer_t mer);
static void  Hash_Insert
    (Mer_t mer);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Print_Mer_Coverage
    (const string & tag, const string & s, double & percent_covered);
static void  Read_Mers
    (const char * fname, vector <Mer_t> & mer_list);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   FILE  * unique_fp, * repeat_fp, * unsure_fp;
   vector <Mer_t>  mer_list;
   string  s, tag;
   int  n;

   Parse_Command_Line (argc, argv);

   fprintf (stderr, "Repeat_Cutoff set to %.2f%%\n", Repeat_Cutoff);
   fprintf (stderr, "Unique_Cutoff set to %.2f%%\n", Unique_Cutoff);

   Read_Mers (Kmer_File_Name . c_str (), mer_list);

   n = mer_list . size ();

   Build_Hash_Table (mer_list);

#if  DEBUG
{
 int  i;

 printf ("Kmer_Len = %d  Hash_Table_Size = %d  Hash_Shift = %d\n",
      Kmer_Len, Hash_Table_Size, Hash_Shift);
 for  (i = 0;  i < n;  i ++)
   printf ("%15llo %8d\n", mer_list [i], Hash (mer_list [i]));
}
#endif

   Forward_Mask = ((long long unsigned) 1 << (2 * Kmer_Len - 2)) - 1;
   Extract_Mask = ((long long unsigned) 1 << (2 * Kmer_Len)) - 1;

#if  DEBUG
{
 printf ("Forward_Mask = %15llo\n", Forward_Mask);
 printf ("Extract_Mask = %15llo\n", Extract_Mask);
}
#endif

   if  (Make_Fasta)
       {
        unique_fp = File_Open ("unique.cov.fasta", "w", __FILE__, __LINE__);
        repeat_fp = File_Open ("repeat.cov.fasta", "w", __FILE__, __LINE__);
        unsure_fp = File_Open ("unsure.cov.fasta", "w", __FILE__, __LINE__);
       }

   while  (Fasta_Read (stdin, s, tag))
     {
      FILE  * fp;
      double  percent_covered;

      if (!OPT_Features) { printf (">%s\n", tag . c_str ()); }
      Print_Mer_Coverage (tag, s, percent_covered);

      if  (Make_Fasta)
          {
           if  (percent_covered < Unique_Cutoff)
               fp = unique_fp;
           else if  (percent_covered >= Repeat_Cutoff)
               fp = repeat_fp;
             else
               fp = unsure_fp;
           fprintf (fp, ">%s\n", tag . c_str ());
           Fasta_Print (fp, s . c_str ());
          }
     }

   if  (Make_Fasta)
       {
        fclose (unique_fp);
        fclose (repeat_fp);
        fclose (unsure_fp);
       }

   return  0;
  }



static void  Build_Hash_Table
    (const vector <Mer_t> & mer_list)

//  Add entries in  mer_list  to the global
//   Hash_Table .

  {
   int  i, n;

   n = mer_list . size ();
   Hash_Shift = 1;
   for  (i = 2;  i < 3 * n;  i *= 2)
     Hash_Shift ++;
   Hash_Table_Size = i;
   Hash_Mask = Hash_Table_Size - 1;

   Hash_Table = (Mer_t *) Safe_calloc (Hash_Table_Size, sizeof (Mer_t),
        __FILE__, __LINE__);
   for  (i = 0;  i < n;  i ++)
     Hash_Insert (mer_list [i]);

   return;
  }



static unsigned  Char_To_Binary
    (char ch)

//  Return the binary equivalent of  ch .

  {
   switch  (tolower (ch))
     {
      case  'a' :
      case  'n' : return  0;
      case  'c' : return  1;
      case  'g' : return  2;
      case  't' : return  3;

      default :
      if (OPT_AllowAmbiguity) { return 0; }
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



static int  Hash
    (Mer_t mer)

//  Convert  mer  to a subscript in the global  Hash_Table .

  {
   unsigned  result = 0;

   while  (mer > 0)
     {
      result ^= mer & Hash_Mask;
      mer >>= Hash_Shift;
     }

   return  int (result);
  }



static bool  Hash_Find
    (Mer_t mer)

//  Return  true  iff  mer  occurs in the global  Hash_Table .

  {
   int  i;

   i = Hash (mer);

   while  (Hash_Table [i] & Filled_Mask)
     {
      if  ((Hash_Table [i] & Extract_Mask) == mer)
          return  true;
      i ++;
      if  (i == Hash_Table_Size)
          i = 0;
     }

   return  false;
  }



static void  Hash_Insert
    (Mer_t mer)

//  Insert  mer  into the global  Hash_Table .

  {
   int  i;

   i = Hash (mer);

   while  (Hash_Table [i] & Filled_Mask)
     {
      i ++;
      if  (i == Hash_Table_Size)
          i = 0;
     }

   Hash_Table [i] = (mer | Filled_Mask);

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

   while  (! errflg && ((ch = getopt (argc, argv, "Afhr:u:FL:")) != EOF))
     switch  (ch)
       {
        case 'A' : OPT_AllowAmbiguity = true; break;
        case 'f' : Make_Fasta = true; break;
        case 'F' : OPT_Features = true; break;
        case 'h' : errflg = true; break;
        case 'r' : Repeat_Cutoff = strtod (optarg, NULL); break;
        case 'u' : Unique_Cutoff = strtod (optarg, NULL); break;
        case 'L' : MIN_LEN = atoi(optarg); break;
        case '?' :
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



static void  Print_Mer_Coverage
    (const string & tag, const string & s, double & percent_covered)

//  Print regions in string  s  that are covered
//  by mers (or their reverse-complements) in
//  the global hash table.  Set  percent_covered  to
//  the percentage of the entire read covered by the mers

  {
   Mer_t  fwd_mer = 0, rev_mer = 0;
   int  lo, hi, total = 0;
   int  i, j, n;

   n = s . length ();

   if  (n < Kmer_Len)
       {
        percent_covered = 0.0;
        if (!OPT_Features)
        {
          printf ("Total %d of %d (%.1f%%)\n", 0, n, Percent (0, n));
        }
        return;
       }

   for  (i = 0;  i < Kmer_Len - 1;  i ++)
     {
      Forward_Add_Ch (fwd_mer, s [i]);
      Reverse_Add_Ch (rev_mer, s [i]);
     }

   lo = 0;
   hi = -1;
   for  (j = 0;  i < n;  i ++, j ++)
     {
      Forward_Add_Ch (fwd_mer, s [i]);
      Reverse_Add_Ch (rev_mer, s [i]);

#if  DEBUG
{
 printf ("%4d  %c  %15llo  %15llo  %c  %c\n",
      i, s [i], fwd_mer, rev_mer,
      Hash_Find (fwd_mer) ? 'T' : 'F',
      Hash_Find (rev_mer) ? 'T' : 'F');
}
#endif

      if  (Hash_Find (fwd_mer) || Hash_Find (rev_mer))
          {
           if  (hi < j)
               {
                if  (hi > 0)
                    {
                     if (OPT_Features)
                     {
                       if (hi-lo > MIN_LEN)
                       {
                         printf("%s K %d %d KMER_COV %d\n", tag.c_str(), lo, hi, hi-lo);
                       }
                     }
                     else
                     {
                       printf ("%8d %8d %8d\n", lo, hi, hi - lo);
                     }
                     total += hi - lo;
                    }
                lo = j;
               }
           hi = i + 1;
          }
     }

   if  (hi > 0)
       {
         total += hi - lo;
         if (OPT_Features)
         {
           if (hi-lo > MIN_LEN)
           {
             printf("%s K %d %d KMER_COV %d\n", tag.c_str(), lo, hi, hi-lo);
           }
         }
         else
         {
           printf ("%8d %8d %8d\n", lo, hi, hi - lo);
         }
       }

   percent_covered = Percent (total, n);
   if (!OPT_Features)
   {
     printf ("Total %d of %d (%.1f%%)\n", total, n, percent_covered);
   }

   return;
  }



static void  Read_Mers
    (const char * fname, vector <Mer_t> & mer_list)

//  Read kmers from file name  fname  and save them
//  in binary form in  mer_list .  Input format is
//  a multi-fasta file.  Mers are assumed to contain only
//  ACGT's

  {
   FILE  * fp;
   string  s, tag;
   Mer_t  mer;

   fp = File_Open (fname, "r", __FILE__, __LINE__);

   mer_list . clear ();

   while  (Fasta_Read (fp, s, tag))
     {
      if  (Kmer_Len == 0)
          Kmer_Len = s . length ();
      else if  (Kmer_Len != int (s . length ()))
          {
           sprintf (Clean_Exit_Msg_Line, "New kmer \"%s\" length is %d instead of %d",
                s . c_str (), s . length (), Kmer_Len);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      Fasta_To_Binary (s, mer);
      mer_list . push_back (mer);
     }

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
           "USAGE:  kmer-cov  <kmer-file>\n"
           "\n"
           "Read a list of short kmers (31 bases or less) from <kmer-file>\n"
           "and then compute what regions of the fasta sequences read from\n"
           "stdin are covered by them (or their reverse complement).\n"
           "\n"
           "Options:\n"
           "  -F      Output regions as Features\n"
           "  -L <len> Min Length to report as a feature\n"
           "  -A     Allow Ambiguity Codes (treated as A's)\n"
           "\n"
           "  -f      Output unique/repeat/unsure fasta sequences\n"
           "  -h      Print this usage message\n"
           "  -r <x>  Repeats are > <x>%% covered by kmers\n"
           "  -u <x>  Uniques are <= <x>%% covered by kmers\n"
           "\n");

   return;
  }



