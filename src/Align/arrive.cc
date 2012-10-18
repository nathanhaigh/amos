#include  <cstdio>
#include  <cstdlib>
#include  <cmath>
#include  <vector>
using namespace std;

int  Lo = 0;
int  Hi = 100;
  // Range of entries to match with distribution;


int  main
    (int argc, char * argv [])

  {
   vector <double> a, expect;
   double  sum = 0.0;
   double  x;
   double  genome, num_reads, error;
   double  best_error, best_genome, best_num_reads;
   int  i, ct;

   if  (argc == 3)
       {
        Lo = strtol (argv [1], NULL, 10);
        Hi = strtol (argv [2], NULL, 10);
       }
   fprintf (stderr, "Using  Lo = %d  and  Hi = %d\n", Lo, Hi);

   while  (scanf ("%d %d %lf", & i, & ct, & x) == 3)
     {
      a . push_back (double (ct));
      sum += ct;
     }

   for  (i = Lo;  i <= Hi;  i ++)
     a [i] /= sum;

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

   printf ("Bests:  G = %10.3e  N = %.0f  error = %9.4f\n",
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
   

   return  0;
  }
