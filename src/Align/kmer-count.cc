//  A. L. Delcher
//
//  File:  kmer-count.cc
//
//  Last Modified:  22 March 2004
//
//  For the kmer specified on the command line, count
//  how many times it occurs in the multifasta file
//  read from  stdin .


#include  "delcher.hh"
#include  "fasta.hh"
#include  <string>
#include  <vector>
using namespace std;


#define  DEBUG  0


const int  MAX_LINE = 1000;


static  char * Kmer;
  // The kmer on the command line


static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   char  * rev_kmer;
   string  s, tag;
   bool  is_palindrome;
   int  match_ct = 0, total_mers = 0;
   int  i, kmer_len;

   Parse_Command_Line (argc, argv);

   kmer_len = strlen (Kmer);
   for  (i = 0;  i < kmer_len;  i ++)
     Kmer [i] = tolower (Kmer [i]);
   rev_kmer = strdup (Kmer);
   Reverse_Complement (rev_kmer);

   is_palindrome = (strcmp (Kmer, rev_kmer) == 0);

   while  (Fasta_Read (stdin, s, tag))
     {
      const char  * p, * sp;
      int  n = s . length ();

      for  (i = 0;  i < n;  i ++)
        s [i] = tolower (s [i]);

      sp = s . c_str ();

      for  (p = strstr (sp, Kmer);  p != NULL;  p = strstr (p + 1, Kmer))
        match_ct ++;

      if  (! is_palindrome)
          {
           for  (p = strstr (sp, rev_kmer);  p != NULL;  p = strstr (p + 1, rev_kmer))
             match_ct ++;
          }

      if  (n >= kmer_len)
          total_mers += 1 + n - kmer_len;
     }

   printf  ("Kmer = %s\n", Kmer);
   if  (is_palindrome)
       printf ("  is palindrome\n");
     else
       printf ("Rev_Comp Kmer = %s", rev_kmer);
   printf ("Matches = %d\n", match_ct);
   printf ("Total mers = %d\n", total_mers);
   if  (match_ct > 0)
       printf ("One match every %.1f mers\n", double (total_mers) / match_ct);

   return  0;
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

   if  (optind > argc - 1)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Kmer = argv [optind ++];

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  kmer-cov  <kmer>\n"
           "\n"
           "For the kmer specified on the command line, count\n"
           "how many times it occurs in the multifasta file\n"
           "read from  stdin .\n"
           "\n"
           "Options:\n"
           "  -h    Print this usage message\n"
           "\n");

   return;
  }



