//  A. L. Delcher
//
//  File:  test-align.cc
//
//  Last Modified:  25 November 2002
//
//  Test program to generate a random sequence and then pick
//  a subsequence of it with errors.  Then find the alignment
//  between the two and print it.


#include  "delcher.hh"
#include  "fasta.hh"
#include  "align.hh"
#include  "utility_AMOS.hh"
#include  <vector>


const int  NEW_SIZE = 1000;


int  main
    (int argc, char * argv [])

  {
   Alignment_t  ali;
   Align_Score_Entry_t  last_entry;
   char  alphabet [] = "acgt";
   double  error;
   char  * a, * b;
   int  a_len, b_len, a_max;
   int  start, stop;
   int  lo, hi;
   int  err_ct;
   bool  ans;
   time_t  now;
   int  i, j, k;

   now = time (NULL);
   cerr << "Starting at " << ctime (& now) << endl;

   Verbose = 5;

   a = "aag";
   b = "aagaaa";
   printf ("a=%s\n", a);
   printf ("b=%s\n", b);
   a_len = strlen (a);
   b_len = strlen (b);

   while  (true)
     {
      Alignment_t  ali;
      int  t_lo, t_hi;

      cerr << "Enter t_lo and t_hi" << endl;
      cin >> t_lo >> t_hi;

      Overlap_Align_Full_Matrix
        (a, a_len, b, t_lo, t_hi, b_len, 1, -3, -2, -2, ali);

      ali . Dump (stdout);
      ali . Print (stdout, a, b);
     }
   exit (-3);


   printf ("\nFull_Matrix:\n");
   Complete_Align_Full_Matrix (a, 0, strlen (a), b, 0, 1, strlen (b),
        2, -3, -2, -3, NULL, last_entry, ali);
   ali . Dump (stdout);
   ali . Print (stdout, a, b);

   printf ("\nSave_Space:\n");
   Complete_Align_Save_Space (a, 0, strlen (a), b, 0, 1, strlen (b),
        2, -3, -2, -3, NULL, last_entry, ali);
   ali . Dump (stdout);
   ali . Print (stdout, a, b);
   exit (-3);

{
 vector < vector <int> >  v_list;
 int  i, j, k;

 for  (i = 0;  i < 10000;  i ++)
   {
    vector <int>  v;

    cerr << "i = " << i << endl;
    v_list . clear ();
    for  (j = 0;  j < 1000;  j ++)
      {
       v . clear ();

       for  (k = 0;  k < 1000;  k ++)
         v . push_back (rand ());
      }

    v_list . push_back (v);
   }
}



   a_len = atoi (argv [1]);
   b_len = atoi (argv [2]);
   error = strtod (argv [3], NULL);

   a_max = int (10 + a_len * (1.0 + 2.0 * error));
   a = (char *) AMOS::SafeMalloc (1 + a_max);
   b = (char *) AMOS::SafeMalloc (1 + b_len);

   for  (i = 0;  i < b_len;  i ++)
     b [i] = alphabet [lrand48 () % 4];
   b [i] = '\0';

   start = j = lrand48 () % (b_len - a_len);
   i = 0;
   err_ct = 0;
   for  (k = 0;  k < a_len && j < b_len;  k ++)
     {
      if  (drand48 () >= error)
          a [i ++] = b [j ++];
        else
          {
           double  p;

           err_ct ++;

           p = drand48 ();
           if  (p < 0.333)
               {
                int  q;

                q = lrand48 () % 4;
                if  (alphabet [q] == b [j])
                    a [i] = alphabet [(q + 1 + lrand48 () % 3) % 4];
                  else
                    a [i] = alphabet [q];
                i ++;
                j ++;
               }
           else if  (p < 0.667)
               j ++;
             else
               a [i ++] = alphabet [lrand48 () % 4];
          }
     }
   a [i] = '\0';
   a_len = i;
   stop = j;

   lo = Max (0, start - 5);
   hi = Min (b_len - 1, start + 5);

   a_len = strlen (a);
   b_len = strlen (b);

//   ans = Substring_Match_VS
//           (a, a_len, b, b_len, lo, hi, err_ct + 1, ali);

   Global_Align
        (a, a_len, b, 0, b_len, 10, -2, -2, -20, ali);

   if  (! ans)
       printf ("Not found\n");
     else
       {
        printf ("start = %d  stop = %d  errors = %d\n",
                start, stop, err_ct);
        printf ("Found at %d\n", ali . b_lo);
        ali . Print (stdout, a, b);
       }

   return  0;
  }
