//  A. L. Delcher
//
//  File:  test.cc
//
//  Last Modified:  25 November 2002
//
//  Test program to generate a random sequence.  Then generate
//  multiple sequences from it (with errors) and find a multialignment
//  of those sequences.


#include  "delcher.hh"
#include  "fasta.hh"
#include  "align.hh"
#include  "prob.hh"
#include  "utility_AMOS.hh"
#include  <vector>


const int  NEW_SIZE = 1000;


static char  Alphabet [] = "acgt";
  // Characters from which strings are generated



static void  Make_Copy_With_Errors
    (const char * ref, char * a, int len, double error);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   Gapped_Multi_Alignment_t  gma;
   Alignment_t  ali;
   vector <char *>  s;
   vector <int>  offset;
   double  error;
   char  * ref, * a;
   int  ref_len;
   int  base_len, delta, num_strings, slop;
   int  i;

   fprintf (stderr, "Starting on  %s  at  %s\n", __DATE__, __TIME__);

   Verbose = 1;

   if  (argc < 5)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   num_strings = atoi (argv [1]);
   base_len = atoi (argv [2]);
   delta = atoi (argv [3]);
   error = strtod (argv [4], NULL);

   fprintf (stderr, "error = %.5f\n", error);

   // generate  num_strings  of approximate length  base_len
   // (may vary because of indels) each chosen starting  delta
   // letters after the preceding from a single reference string
   // with error rate of  error 

   slop = int (10 + base_len * 2.0 * error);
       // extra length to allow for indels
   ref_len = base_len + (num_strings - 1) * delta + slop;
   a = (char *) AMOS::SafeMalloc (1 + base_len + slop);
   ref = (char *) AMOS::SafeMalloc (1 + ref_len);
   
   for  (i = 0;  i < ref_len;  i ++)
     ref [i] = Alphabet [lrand48 () % 4];
   ref [i] = '\0';

   for  (i = 0;  i < num_strings;  i ++)
     {
      char  * p;
      int  off;

      Make_Copy_With_Errors (ref + i * delta, a, base_len, error);
      p = strdup (a);
      s . push_back (p);
      if  (i == 0)
          off = 0;
        else
          off = delta;
      offset . push_back (off);
     }

   if  (Verbose > 1)
       {
        char  tag [1000];

        sprintf (tag, "Reference");
        Fasta_Print (stdout, ref, tag);

        for  (i = 0;  i < num_strings;  i ++)
          {
           sprintf (tag, "%d  offset = %d", i + 1, offset [i]);
           Fasta_Print (stdout, s [i], tag);
          }
       }

   Multi_Align ("Test", s, offset, 5, 0.04, 10, gma);

   gma . Print (stderr, s, 60);

   for  (i = 0;  i < num_strings - 1;  i ++)
     {
      bool  ans;

      ans = Overlap_Match_VS
              (s [i + 1], strlen (s [i + 1]), s [i], strlen (s [i]),
               offset [i + 1] - 4, offset [i + 1] + 4, base_len / 2, slop, ali);

      if  (! ans)
          printf ("Not found\n");
        else
          {
           printf ("\nFound at %d\n", ali . b_lo);
           ali . Print (stdout, s [i + 1], s [i]);
          }
     }

   return  0;
  }



static void  Make_Copy_With_Errors
    (const char * ref, char * a, int len, double error)

//  Copy the first  len  characters of  ref  into  a  but insert errors
//  with probability  error .

  {
   int  i, j;

   i = 0;
   for  (j = 0;  j < len && ref [j] != '\0'; )
     {
      if  (drand48 () >= error)
          a [i ++] = ref [j ++];
        else
          {
           double  p;

           p = drand48 ();
           if  (p < 0.333)
               {
                int  q;

                q = lrand48 () % 4;
                if  (Alphabet [q] == ref [j])
                    a [i] = Alphabet [(q + 1 + lrand48 () % 3) % 4];
                  else
                    a [i] = Alphabet [q];
                i ++;
                j ++;
               }
           else if  (p < 0.667)
               j ++;
             else
               a [i ++] = Alphabet [lrand48 () % 4];
          }
     }
   a [i] = '\0';

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <num_strings> <base_len> <delta> <error>\n"
           "\n"
           "Generate <num_strings> of approximate length <base_len>\n"
           "(may vary because of indels) each chosen starting <delta>\n"
           "letters after the preceding from a single reference string\n"
           "with error rate of <error> \n"
           "\n"
           "Options:\n"
           "  none\n"
           "\n",
           command);

   return;
  }
