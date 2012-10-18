//  A. L. Delcher
//
//  File:  running-cmp.cc
//
//  Last Modified:  9 July 2003
//
//  Read 2 files of numbers, sort them into descending order
//  and print out the running difference between their prefix sums.


#include  "delcher.hh"
#include  <vector>
#include  <algorithm>
using namespace std;

const int  MAX_LINE = 10000;


static bool  Reverse_Order
    (const double & a, const double & b)
  {
   return  (b < a);
  }


int  main
    (int argc, char * argv [])

  {
   FILE  * fp;
   vector <double>  a, b;
   double  x, sum, s2;
   char  line [MAX_LINE];
   int  a_len, b_len, min_len;
   int  i;

   Verbose = 0;

   if  (argc < 3)
       {
        fprintf (stderr, "USAGE:  running-cmp <file1> <file2>\n");
        exit (EXIT_FAILURE);
       }

   fp = File_Open (argv [1], "r");
   // skip the first line which is a comment
   fgets (line, MAX_LINE, fp);
   while  (fscanf (fp, "%lf", & x) == 1)
     a . push_back (x);
   fclose (fp);

   fp = File_Open (argv [2], "r");
   // skip the first line which is a comment
   fgets (line, MAX_LINE, fp);
   while  (fscanf (fp, "%lf", & x) == 1)
     b . push_back (x);
   fclose (fp);

   sort (a . begin (), a . end (), Reverse_Order);
   sort (b . begin (), b . end (), Reverse_Order);

   a_len = a . size ();
   b_len = b . size ();
   min_len = Min (a_len, b_len);

   sum = 0.0;

   for  (i = 0;  i < min_len;  i ++)
    {
     sum += a [i] - b [i];
     printf ("%6d %10.0f %10.0f %10.0f\n", i, a [i], b [i], sum);
    }

   s2 = 0.0;
   if  (min_len < a_len)
       {
        for  ( ;  i < a_len;  i ++)
          s2 += a [i];
        printf ("File 1 has %d additional entries:  sum =  %.0f  average = %.0f\n",
             a_len - min_len, s2, s2 / (a_len - min_len));
       }
   else if  (min_len < b_len)
       {
        for  ( ;  i < b_len;  i ++)
          s2 += b [i];
        printf ("File 2 has %d additional entries:  sum =  %.0f  average = %.0f\n",
             b_len - min_len, s2, s2 / (b_len - min_len));
       }

   return  0;
  }
