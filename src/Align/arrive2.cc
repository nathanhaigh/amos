#include  <cstdio>
#include  <cstdlib>
#include  <cmath>
#include  <vector>
#include  <delcher.hh>
using namespace std;

const int  DIST_MAX = 11;
  // Number of counts to keep in a distribution.  Values >= the
  // max are lumped together in the last count
const int  WINDOW_SIZE = 50;


static int  Degree_Cutoff = 6;
  // Only do counts of reads with <= this many right (3' end) overlaps
static bool  Ignore_Zeroes = false;
  // If set true by the -z option, ignore reads with no overlaps at all
static int  Lo = 5;
static int  Hi = 104;
  // Range of arrival positions to consider
static FILE  * Len_fp = NULL;
  // File from which read lengths are read
static bool  Minimize_Max_Error = FALSE;
  // If set true (by the -x option) then the max
  // error between observed and estimated proportions
  // will be used to fit distributions
static bool  Minimize_Squared_Error = FALSE;
  // If set true (by the -s option) then the square
  // of the error between observed and estimated proportions
  // will be used to fit distributions
static int  Min_Read_Len = INT_MAX;
  // Ignore reads shorter than this
static int  Max_Read_Len = 900;
  // Ignore reads longer than this

static double  Lo_Genome_Len;
static double  Hi_Genome_Len;
static double  Genome_Len_Delta;
  // Determine range of genome sizes to use for best-fit analysis
  // Values come from command line


static void  Analyze
    (int dist [], int lo, int hi, int n);
static void  Double_Fit
    (int dist [], int lo, int hi, int n);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   int  num_buckets, num_dists;
   int  a, b, a_hang, b_hang;
   int  prev;
   int  num_reads, r_degr;
   int  total_olaps;
   vector <int>  read_len;
   char  ori;
   double  e1, e2;
   int  d, i, j, k;

   Parse_Command_Line (argc, argv);

   fprintf (stderr, "Using  Lo = %d  and  Hi = %d\n", Lo, Hi);
   fprintf (stderr, "Degree cutoff = %d\n", Degree_Cutoff);
   if  (Min_Read_Len < INT_MAX)
       {
        fprintf (stderr, "Ignoring lengths < %d\n", Min_Read_Len);
        fprintf (stderr, "Degree cutoff applied to region %d .. %d\n",
             Lo, Min_Read_Len);
       }
   fprintf (stderr, "Ignoring lengths > %d\n", Max_Read_Len);
   if  (Minimize_Squared_Error)
       fprintf (stderr, "Minimizing sum of error^2 instead of sum of |error|\n");
   else if  (Minimize_Max_Error)
       fprintf (stderr, "Minimizing max error instead of sum of |error|\n");
     else
       fprintf (stderr, "Minimizing sum of |error| instead of sum of error^2\n");
   fprintf (stderr, "Using only reads with 3' overlaps degrees <= %d\n",
        Degree_Cutoff);

   if  (Len_fp != NULL)
       {
        int  expect = 1;
        int  iid, uid, c1, c2, len;

        read_len . push_back (0);
          // There is no read iid zero

        while  (fscanf (Len_fp, "%d %d %d %d %d",
                  & iid, & uid, & c1, & c2, & len) == 5)
          {
           if  (iid != expect)
               {
                read_len . resize (iid);
                for  (i = expect;  i < iid;  i ++)
                  read_len [i] = 0;
               }
           read_len . push_back (len);
           expect = iid + 1;
          }
       }

   num_buckets = int (ceil ((1.0 + Hi - Lo) / WINDOW_SIZE));
   num_dists = (num_buckets * (num_buckets + 1)) / 2;
   fprintf (stderr, "num_buckets = %d  num_dists = %d\n",
        num_buckets, num_dists);

   vector <int>  bucket (num_buckets, 0);
   vector <int *>  dist (num_dists);
   for  (d = 0;  d < num_dists;  d ++)
     dist [d] = (int *) Safe_calloc (1 + DIST_MAX, sizeof (int), __FILE__, __LINE__);

   prev = 0;
   num_reads = r_degr = total_olaps = 0;
   while  (scanf ("%d %d %c %d %d %lf %lf",
                & a, & b, & ori, & a_hang, & b_hang, & e1, & e2) == 7)
     {
      if  (a != prev)
          {
//           if  (prev != 0 && total_olaps <= Degree_Cutoff
           if  (prev != 0 && r_degr <= Degree_Cutoff
                  && (Len_fp == NULL
                        || (read_len [prev] >= Min_Read_Len
                              && read_len [prev] <= Max_Read_Len)))
               {
                d = 0;
                for  (k = 1;  k <= num_buckets;  k ++)
                  {
                   for  (i = 0;  i <= num_buckets - k;  i ++)
                     {
                      int  sum = 0;

                      for  (j = i;  j < i + k;  j ++)
                        sum += bucket [j];

                      if  (sum >= DIST_MAX)
                          sum = DIST_MAX;
                      dist [d ++] [sum] ++;
                     }
                  }
               }
           if  (prev != 0)
               num_reads ++;

           // Increment the zero count of each distribution for the
           // reads that had no overlaps at all;
           if  (! Ignore_Zeroes)
               for  (i = prev + 1;  i < a;  i ++)
                 {
                  for  (d = 0;  d < num_dists;  d ++)
                    dist [d] [0] ++;
                  num_reads ++;
                 }

           for  (i = 0;  i < num_buckets;  i ++)
             bucket [i] = 0;
           prev = a;
           r_degr = total_olaps = 0;
          }

      if  (a_hang >= Lo)
          {
           i = (a_hang - Lo) / WINDOW_SIZE;
           if  (i < num_buckets)
               bucket [i] ++;
           if  (a_hang < Min_Read_Len)
               r_degr ++;
          }
      total_olaps ++;
     }

   // Handle the last one
//   if  (total_olaps <= Degree_Cutoff
   if  (r_degr <= Degree_Cutoff
           && (Len_fp == NULL
                 || (read_len [prev] > Min_Read_Len
                        && read_len [prev] <= Max_Read_Len)))
       {
        d = 0;
        for  (k = 1;  k <= num_buckets;  k ++)
          {
           for  (i = 0;  i <= num_buckets - k;  i ++)
             {
              int  sum = 0;

              for  (j = i;  j < i + k;  j ++)
                sum += bucket [j];

              if  (sum >= DIST_MAX)
                  sum = DIST_MAX;
              dist [d ++] [sum] ++;
             }
          }
       }
   num_reads ++;

   // Print the distributions
   printf ("num_reads = %d\n", num_reads);
   d = 0;
   for  (k = 1;  k <= num_buckets;  k ++)
     {
      for  (i = 0;  i <= num_buckets - k;  i ++)
        {
         a = Lo + i * WINDOW_SIZE;
         b = Lo + (i + k) * WINDOW_SIZE - 1;

         Analyze (dist [d], a, b, num_reads);
         Double_Fit (dist [d], a, b, num_reads);

         d ++;
        }
     }


#if  0
   best_error = 1e9;
   for  (num_reads = 2.0 * sum;  num_reads >= sum / 2.0;  num_reads -= 500.0)
     {
//      printf ("# reads = %.0f\n", num_reads);
      for  (genome = 10.0e6;  genome <= 100.0e6;  genome += 1.0e6)
        {
         double  p, q, prev;

         q = exp ((num_reads - 1.0) * log ((genome - 1.0) / genome));
         p = 1 - q;

         prev = p * pow (q, Lo);
         error = 0.0;
         for  (i = Lo;  i <= Hi;  i ++)
           {
            error += fabs (prev - a [i]);
            prev *= q;
           }

//         printf ("G = %10.3e  error = %9.4f\n", genome, error);
         if  (error < best_error)
             {
              best_error = error;
              best_genome = genome;
              best_num_reads = num_reads;
             }
        }
     }

   printf ("Bests:  G = %10.3e  N = %.0f  error = %9.6e\n",
       best_genome, best_num_reads, best_error);

  {
   double  p, q, prev;

   q = exp ((best_num_reads - 1.0) * log ((best_genome - 1.0) / best_genome));
   p = 1 - q;

   prev = p * pow (q, Lo);
   for  (i = Lo;  i <= Hi;  i ++)
     {
      printf ("%5d:  %8.6f %8.6f  %8.6f\n", i, a [i], prev, a [i] - prev);
      prev *= q;
     }
  }
#endif   

   return  0;
  }



void  Analyze
    (int dist [], int lo, int hi, int n)

//  Analyze the distribution of counts in  dist [0 .. DIST_MAX]
//  representing hits to the range of positions  lo .. hi
//   n  is the total number of reads that could have contributed
//  to the entries in  dist [] .

  {
   double  lambda, term;
   double  error, square_error, mx_error;
   double  genome_len;
   double  best_error, best_square_error, best_mx_error, best_genome_len;
   double  diff;
   int  num_contams, max_contams, best_contams;
   int  sum, new_sum;
   int  j;

   printf ("\nRange %d .. %d:\n", lo, hi);

   lambda = 0.0;
   sum = 0;
   for  (j = 0;  j <= DIST_MAX;  j ++)
     sum += dist [j];

   for  (j = 0;  j < DIST_MAX;  j ++)
     lambda += j * double (dist [j]) / sum;

   printf (" Hits   --- Observed ---   --- Expected ---\n");
   term = exp (-1.0 * lambda);
   error = square_error = mx_error = 0.0;
   for  (j = 0;  j < DIST_MAX;  j ++)
     {
      printf ("  %3d:  %8d  %6.4f   %8d  %6.4f\n", j, dist [j],
           (1.0 * dist [j]) / sum, int (term * sum + 0.5), term);
      diff = fabs ((1.0 * dist [j]) / sum - term);
      error += diff;
      square_error += diff * diff;
      mx_error = Max (mx_error, diff);
      term *= lambda / (j + 1.0);
     }

   printf (">=%3d:  %8d  %6.4f   %8d  %6.4f\n", DIST_MAX, dist [DIST_MAX],
        (1.0 * dist [DIST_MAX]) / sum, int (term * sum + 0.5), term);
   diff = fabs ((1.0 * dist [DIST_MAX]) / sum - term);
   error += diff;
   square_error += diff * diff;
   mx_error = Max (mx_error, diff);
   printf ("Total:  %8d\n", sum);
   printf ("Lambda = %.4f\n", lambda);
   printf ("Estimated genome length = %.0f\n", ((1 + hi - lo) * n) / lambda);
   printf ("Error = %.6e  Squared error = %.6e  Max error = %.6e\n",
       error, square_error, mx_error);

   best_error = best_square_error = best_mx_error = DBL_MAX;
   for  (genome_len = Lo_Genome_Len;  genome_len <= Hi_Genome_Len;
           genome_len += Genome_Len_Delta)
     {
      bool  update;

      lambda = (n * (1 + hi - lo)) / genome_len;
      term = exp (-1.0 * lambda);
      error = square_error = mx_error = 0.0;
      for  (j = 0;  j < DIST_MAX;  j ++)
        {
         diff = fabs ((1.0 * dist [j]) / sum - term);
         error += diff;
         square_error += diff * diff;
         mx_error = Max (mx_error, diff);
         term *= lambda / (j + 1.0);
        }

      update = false;
      if  (Minimize_Squared_Error && square_error < best_square_error)
          update = true;
      else if  (Minimize_Max_Error && mx_error < best_mx_error)
          update = true;
      else if  (! Minimize_Squared_Error && ! Minimize_Max_Error
                  && error < best_error)
          update = true;

      if  (update)
          {
           best_error = error;
           best_square_error = square_error;
           best_mx_error = mx_error;
           best_genome_len = genome_len;
          }
     }

   printf ("\n");
   printf (" Hits   --- Observed ---   --- Best Fit ---\n");
   lambda = (n * (1 + hi - lo)) / best_genome_len;
   term = exp (-1.0 * lambda);
   error = square_error = 0.0;
   for  (j = 0;  j < DIST_MAX;  j ++)
     {
      printf ("  %3d:  %8d  %6.4f   %8d  %6.4f\n", j, dist [j],
           (1.0 * dist [j]) / sum, int (term * sum + 0.5), term);
      error += fabs ((1.0 * dist [j]) / sum - term);
      square_error += pow ((1.0 * dist [j]) / sum - term, 2);
      term *= lambda / (j + 1.0);
     }
   printf ("Total:  %8d\n", sum);
   printf ("Lambda = %.4f\n", lambda);
   printf ("Best-fit genome length = %.0f\n", best_genome_len);
   printf ("Error = %.6e  Squared error = %.6e  Max error = %.6e\n",
        best_error, best_square_error, best_mx_error);


   // Re-do calculation allowing various numbers of reads to be "contaminant",
   // which reduces the number of zero-degree hits

   max_contams = Min (80000, dist [0]);

   best_error = best_square_error = DBL_MAX;
   for  (genome_len = Lo_Genome_Len;  genome_len <= Hi_Genome_Len;
         genome_len += Genome_Len_Delta)
     for  (num_contams = 0;  num_contams <= max_contams;  num_contams += 1000)
       {
        lambda = ((n - num_contams) * (1 + hi - lo)) / genome_len;
        term = exp (-1.0 * lambda);
        new_sum = sum - num_contams;
        error = fabs ((1.0 * (dist [0] - num_contams)) / new_sum - term);
        square_error = pow ((1.0 * (dist [0] - num_contams)) / new_sum - term, 2);

        for  (j = 1;  j < DIST_MAX;  j ++)
          {
           term *= lambda / j;
           error += fabs ((1.0 * dist [j]) / new_sum - term);
           square_error += pow ((1.0 * dist [j]) / new_sum - term, 2);
          }

        if  (Minimize_Squared_Error && square_error < best_square_error
               || ! Minimize_Squared_Error && error < best_error)
            {
             best_error = error;
             best_square_error = square_error;
             best_genome_len = genome_len;
             best_contams = num_contams;
            }
       }

   printf ("\n");
   printf ("        Assuming %d contaminant reads\n", best_contams);
   printf (" Hits   --- Observed ---   --- Best Fit ---\n");
   lambda = ((n - best_contams) * (1 + hi - lo)) / best_genome_len;
   term = exp (-1.0 * lambda);
   new_sum = sum - best_contams;

   printf ("  %3d:  %8d  %6.4f   %8d  %6.4f\n", 0, (dist [0] - best_contams),
        (1.0 * (dist [0] - best_contams)) / new_sum,
        int (term * new_sum + 0.5), term);
   error = fabs ((1.0 * (dist [0] - num_contams)) / new_sum - term);
   square_error = pow ((1.0 * (dist [0] - num_contams)) / new_sum - term, 2);

   for  (j = 1;  j < DIST_MAX;  j ++)
     {
      term *= lambda / j;
      printf ("  %3d:  %8d  %6.4f   %8d  %6.4f\n", j, dist [j],
           (1.0 * dist [j]) / new_sum, int (term * new_sum + 0.5), term);

      error += fabs ((1.0 * dist [j]) / new_sum - term);
      square_error += pow ((1.0 * dist [j]) / new_sum - term, 2);
     }
   printf ("Total:  %8d\n", new_sum);
   printf ("Lambda = %.4f\n", lambda);
   printf ("Best-fit genome length = %.0f\n", best_genome_len);
   printf ("Error = %.6e  Squared error = %.6e\n", best_error, best_square_error);

   return;
  }



void  Double_Fit
    (int dist [], int lo, int hi, int n)

//  Fit the distribution of counts in  dist [0 .. DIST_MAX]
//  representing hits to the range of positions  lo .. hi
//   n  is the total number of reads that could have contributed
//  to the entries in  dist [] .  Assume hits could come from
//  a combination of single and two-copy elements in the genome

  {
   double  lambda1, lambda2, term1, term2;
   double  error, square_error;
   double  genome_len;
   double  best_error, best_square_error, best_genome_len;
   double  dbl_copy_p, best_dcp;
   vector <double>  observe (DIST_MAX);
   int  sum;
   int  j;

   printf ("*** Double Fit Analysis ***\n");
   printf ("\nRange %d .. %d:\n", lo, hi);

   sum = 0;
   for  (j = 0;  j <= DIST_MAX;  j ++)
     sum += dist [j];

   // Save observed distribution as proportions
   for  (j = 0;  j < DIST_MAX;  j ++)
     observe [j] = (1.0 * dist [j]) / sum;

   best_error = best_square_error = DBL_MAX;
   for  (genome_len = Lo_Genome_Len;  genome_len <= Hi_Genome_Len;
           genome_len += Genome_Len_Delta)
     for  (dbl_copy_p = 0.00;  dbl_copy_p <= 0.80;  dbl_copy_p += 0.01)
       {
        lambda1 = (n * (1 + hi - lo)) / genome_len;
        lambda2 = 2.0 * lambda1;
        term1 = exp (-1.0 * lambda1);
        term2 = exp (-1.0 * lambda2);

        error = square_error = 0.0;

        for  (j = 0;  j < DIST_MAX;  j ++)
          {
           double  expect;

           expect = (1.0 - dbl_copy_p) * term1 + dbl_copy_p * term2;
           error += fabs (observe [j] - expect);
           square_error += pow (observe [j] - expect, 2);
           term1 *= lambda1 / (j + 1.0);
           term2 *= lambda2 / (j + 1.0);
          }

        if  (Minimize_Squared_Error ? (square_error < best_square_error)
               : (error < best_error))
            {
             best_error = error;
             best_square_error = square_error;
             best_genome_len = genome_len;
             best_dcp = dbl_copy_p;
            }
       }

   printf ("\n");
   printf ("        Assuming %.0f%% 1-copy and %.0f%% 2-copy\n",
        100.0 * (1.0 - best_dcp), 100.0 * best_dcp);
   printf (" Hits   --- Observed ---   --- One Copy ---   --- Two Copy ---   --- Combined ---\n");
   lambda1 = (n * (1 + hi - lo)) / best_genome_len;
   lambda2 = 2.0 * lambda1;
   term1 = exp (-1.0 * lambda1);
   term2 = exp (-1.0 * lambda2);

   error = square_error = 0.0;

   for  (j = 0;  j < DIST_MAX;  j ++)
     {
      double  e;

      printf ("  %3d:  %8d  %6.4f   %8d  %6.4f   %8d  %6.4f   %8d  %6.4f\n",
           j, dist [j], observe [j],
           int (term1 * sum * (1.0 - best_dcp) + 0.5), term1,
           int (term2 * sum * best_dcp + 0.5), term2,
           int (term1 * sum * (1.0 - best_dcp)
                  + term2 * sum * best_dcp + 0.5),
           (1.0 - best_dcp) * term1 + best_dcp * term2);

      e = fabs (observe [j] - ((1.0 - best_dcp) * term1 + best_dcp * term2));
      error += e;
      square_error += pow (e, 2);
      term1 *= lambda1 / (j + 1.0);
      term2 *= lambda2 / (j + 1.0);
     }
   printf ("Total:  %8d\n", sum);
   printf ("Lambda1 = %.4f  Lambda2 = %.4f\n", lambda1, lambda2);
   printf ("Best-fit genome length = %.0f\n", best_genome_len);
   printf ("Error = %.6e  Squared error = %.6e\n", best_error, best_square_error);

   return;
  }



static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false;
   char  * p;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "d:hL:m:r:sxz")) != EOF))
     switch  (ch)
       {
        case  'd' :
          Degree_Cutoff = strtol (optarg, NULL, 10);
          if  (Degree_Cutoff < 0)
              {
               fprintf (stderr, "\nERROR:  Bad degree cutoff value \"%s\"\n",
                    optarg);
               errflg = true;
              }
          break;

        case  'h' :
          errflg = true;
          break;

        case  'L' :
          Len_fp = File_Open (optarg, "r");
          break;

        case  'm' :
          Min_Read_Len = strtol (optarg, NULL, 10);
          if  (Min_Read_Len < 0)
              {
               fprintf (stderr, "\nERROR:  Bad minimum read len \"%s\"\n",
                    optarg);
               errflg = true;
              }
          break;

        case  'r' :
          if  (sscanf (optarg, "%d-%d", & Lo, & Hi) != 2
                 || Lo < 0 || Hi < Lo)
              {
               fprintf (stderr, "\nERROR:  Bad window range \"%s\"\n",
                    optarg);
               errflg = true;
              }
          break;

        case  's' :
          Minimize_Squared_Error = true;
          break;

        case  'x' :
          Minimize_Max_Error = true;
          break;

        case  'z' :
          Ignore_Zeroes = true;
          break;

        case  '?' :
          fprintf (stderr, "Unrecognized option -%c\n", optopt);

        default :
          errflg = true;
       }

   if  (errflg || optind != argc - 3)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Lo_Genome_Len = strtod (argv [optind], & p);
   if  (p == argv [optind])
       {
        fprintf (stderr, "ERROR:  Bad low genome length value \"%s\"\n",
             argv [optind]);
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   optind ++;
   Hi_Genome_Len = strtod (argv [optind], & p);
   if  (p == argv [optind])
       {
        fprintf (stderr, "ERROR:  Bad high genome length value \"%s\"\n",
             argv [optind]);
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   optind ++;
   Genome_Len_Delta = strtod (argv [optind], & p);
   if  (p == argv [optind])
       {
        fprintf (stderr, "ERROR:  Bad genome length delta value \"%s\"\n",
             argv [optind]);
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <lo> <hi> <del>\n"
           "\n"
           "Read overlap information (from stdin in format produced\n"
           "by dump-olaps program) and estimate genome length from it\n"
           "based on arrivals in windows on reads.  Do best-fit trying\n"
           "genomes with lengths from <lo> to <hi> in increments of <del>\n"
           "\n"
           "Options:\n"
           "  -d <n>   Ignore 3' overlap degrees > <n>\n"
           "  -h       Print this usage message\n"
           "  -L <fn>  Get read length info from file <fn>\n"
           "  -m <n>   Ignore reads shorter than <n> bp\n"
           "  -r <m>-<n>  Use windows in the range <m> .. <n>  of reads\n"
           "  -s       Fit by minimizing sum of squared errors instead of |error|\n"
           "  -x       Fit by minimizing max |error| instead of sum of |error|\n"
           "  -z       Ignore reads with no overlaps at all\n"
           "\n",
           command);

   return;
  }



