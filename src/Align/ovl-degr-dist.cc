//  A. L. Delcher
//
//  File:  ovl-degr-dist.cc
//
//  Last Modified:  22 January 2003
//
//  List probabilities for numbers of overlaps off one end of a read
//  based on binomial trials model


#include  "delcher.hh"
#include  <vector>
using namespace std;

const int  DEFAULT_MIN_OLAP_LEN = 40;
const int  DEFAULT_HI_DEGREE = 20;


struct  Dist_Entry_t
  {
   int  degree;
   double  prob, cum_prob;
   double  num_reads, cum_num_reads;
  };


static double  Genome_Len;
  // Length of the genome in bases
static int  Hi_Degree = DEFAULT_HI_DEGREE;
  // The highest value to print a probability for.  Above this
  // just prints above
static bool  Match_Input_Distrib = false;
  // If set true by the  -m  option, then read a distribution
  // of values from the input and adjust the genome length
  // to the value that best matches it.
static int  Min_Olap_Len = DEFAULT_MIN_OLAP_LEN;
  // Smallest detected overlap between two reads
static double  Num_Reads;
  // Number of reads randomly distributed in the genome
static double  Read_Len;
  // Number of bases in each read


static void  Make_Dist
    (vector <Dist_Entry_t> & dist, double N, double p, int hi);
static void  Parse_Command_Line
    (int argc, char * argv []);
static double  Score_Diff
    (const vector <Dist_Entry_t> & actual,
     const vector <Dist_Entry_t> & goal, int hi);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   double  N, p;
   double  lambda;
   vector <Dist_Entry_t>  dist;
   int  i;

   Parse_Command_Line  (argc, argv);

   fprintf (stderr, "Starting on  %s  at  %s\n", __DATE__, __TIME__);

   Verbose = 0;

   if  (Match_Input_Distrib)
       {
        vector <Dist_Entry_t>  goal;
        Dist_Entry_t  entry;
        double  best_score, score;
        int  ct;

        while  (scanf ("%d %lf", & entry . degree, & entry . num_reads) == 2)
          goal . push_back (entry);

        Hi_Degree = goal [goal . size () - 1] . degree;
        N = Num_Reads - 1;
        best_score = DBL_MAX;
        ct = 0;
        do
          {
           p = (Read_Len - Min_Olap_Len) / (Genome_Len - Read_Len);
           lambda = N * p;
           
           Make_Dist (dist, N, p, Hi_Degree);
           score = Score_Diff (dist, goal, Hi_Degree);

           printf ("%10.0f  %10.0f\n", Genome_Len, score);

           Genome_Len += 1e5;
           if  (score < best_score)
               {
                best_score = score;
                ct = 0;
               }
             else
               ct ++;
          }  while  (ct <= 3);

       }

   N = Num_Reads - 1;
   p = (Read_Len - Min_Olap_Len) / (Genome_Len - Read_Len);
   lambda = N * p;

   Make_Dist (dist, N, p, Hi_Degree);

   fprintf (stderr, "Num Reads = %.0f  Read Len = %.0f  Genome Len = %.0f\n",
            Num_Reads, Read_Len, Genome_Len);
   fprintf (stderr, "N = %e  p = %e  lambda = %e\n", N, p, lambda);
   printf ("%6s  %8s  %8s  %8s  %8s\n",
           "Degree", "Prob", "CumProb", "Reads", "CumReads");

   for  (i = 0;  i <= Hi_Degree;  i ++)
     printf ("%5d:  %8.6f  %8.6f  %8.0f  %8.0f\n", dist [i] . degree,
             dist [i] . prob, dist [i] . cum_prob, dist [i] . num_reads,
             dist [i] . cum_num_reads);

   printf ("above:  %8.6f  %8.6f  %8.0f  %8.0f\n",
           dist [Hi_Degree + 1] . prob, dist [Hi_Degree + 1] . cum_prob,
           dist [Hi_Degree + 1] . num_reads,
           dist [Hi_Degree + 1] . cum_num_reads);
   
   return  0;
  }



static void  Make_Dist
    (vector <Dist_Entry_t> & dist, double N, double p, int hi)

//  Fill  dist  with the values of the binomial distribution
//  of the number of successes in  N  trials, with probability of
//  success per trial equal to  p , numbers  0 .. hi .
//  Set  dist [hi + 1]  to the sum of the remaining values, i.e.,
//  the values above  hi .

  {
   Dist_Entry_t  entry;
   double  lambda;
   double  sum, term;
   int  i;

   dist . clear ();
   lambda = N * p;

   sum = 0.0;
   term = 1.0 * exp (-1.0 * lambda);
   
   for  (i = 0;  i <= hi;  i ++)
     {
      sum += term;

      entry . degree = i;
      entry . prob = term;
      entry . cum_prob = sum;
      entry . num_reads = term * N;
      entry . cum_num_reads = sum * N;
      dist . push_back (entry);

      term *= lambda / double (i + 1);
     }

   entry . degree = hi + 1;
   entry . prob = 1.0 - sum;
   entry . cum_prob = 1.0;
   entry . num_reads = (1.0 - sum) * N;
   entry . cum_num_reads = N;
   dist . push_back (entry);

   return;
  }



static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = FALSE;
   int  ch;
   char  * p;

   optarg = NULL;

   while  (! errflg
             && ((ch = getopt (argc, argv, "hmo:")) != EOF))
     switch  (ch)
       {
        case  'h' :
          errflg = TRUE;
          break;

        case  'm' :
          Match_Input_Distrib = true;
          if  (p == optarg)
              {
               fprintf (stderr, "ERROR:  Bad min olap length \"%s\"\n",
                        optarg);
               errflg = TRUE;
              }
          break;

        case  'o' :
          Min_Olap_Len = (int) strtol (optarg, & p, 10);
          if  (p == optarg)
              {
               fprintf (stderr, "ERROR:  Bad min olap length \"%s\"\n",
                        optarg);
               errflg = TRUE;
              }
          break;

        case  '?' :
          fprintf (stderr, "Unrecognized option -%c\n", optopt);
          // fall through

        default :
          errflg = TRUE;
       }

   if  (errflg || optind != argc - 3)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Num_Reads = strtod (argv [optind], & p);
   if  (p == argv [optind])
       {
        fprintf (stderr, "ERROR:  Bad num reads \"%s\"\n",
                 optarg);
        errflg = TRUE;
       }
   optind ++;

   Read_Len = strtod (argv [optind], & p);
   if  (p == argv [optind])
       {
        fprintf (stderr, "ERROR:  Bad read len \"%s\"\n",
                 optarg);
        errflg = TRUE;
       }
   optind ++;

   Genome_Len = strtod (argv [optind], & p);
   if  (p == argv [optind])
       {
        fprintf (stderr, "ERROR:  Bad genome len \"%s\"\n",
                 optarg);
        errflg = TRUE;
       }
   optind ++;

   return;
  }



static double  Score_Diff
    (const vector <Dist_Entry_t> & actual,
     const vector <Dist_Entry_t> & goal, int hi)

//  Return the sum of the absolute values of the differences
//  in the num_reads fields in entries of  actual  and  goal
//  that have the same  degree  field .  Both are assumed to be
//  in ascending sorted order by degree.  Stop after degree value
//  of  hi .

  {
   double  score;
   int  i, j, m, n;

   i = j = 0;
   m = int (actual . size ());
   n = int (goal . size ());
   score = 0.0;

   while  (i < m && j < n && actual [i] . degree <= hi
               && goal [j] . degree <= hi)
     {
      if  (actual [i] . degree < goal [j] . degree)
          i ++;
      else if  (actual [i] . degree > goal [j] . degree)
          j ++;
        else
          {
           score += fabs (actual [i] . num_reads - goal [j] . num_reads);
           i ++;
           j ++;
          }
     }

   return  score;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <#reads> <read-len> <genome-len>\n"
           "\n"
           "List probabilities for numbers of overlaps off one end of a read\n"
           "based on binomial trials model\n"
           "\n"
           "Options:\n"
           "  -m      Adjust genome length to get best match to distrib\n"
           "            input from stdin\n"
           "  -o <n>  Set minimum overlap length to <n> (default = %d)\n"
           "\n",
           command, DEFAULT_MIN_OLAP_LEN);

   return;
  }
