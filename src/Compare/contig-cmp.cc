//  A. L. Delcher
//
//  File:  contig-cmp.cc
//
//  Last Modified:  7 January 2003
//
//  Compare contigs based on the list of fragments in each


#include  "delcher.hh"
#include  <vector>
#include  <algorithm>
using namespace std;


struct  Unitig_t
  {
   unsigned  sub : 30;
     // subscript of containing contig if there is only one;
     // otherwise, subscript of Overflow list of multiple contigs
   unsigned  status : 2;
     //  0  means empty;  1  means only one contig;  2  means
     // use Overflow list of contigs

   Unitig_t  ()   // default constructor
     {
      status = 0;
     }
  };

struct  Compare_t
  {
   int  a_sub, b_sub;
   int  a_only, b_only, both;

   void  Clear
       (void)
     {
      a_only = b_only = both = 0;
     }
  };

struct  Contig_t
  {
   char  * id;
   vector <int>  list;
     // of unitigs contained in it
   vector <bool>  matched;
   int  unmatched;
  };



static vector < vector <int> >  Overflow;
  // Lists of contigs for unitigs that are contained in
  // multiple contigs



static void  Compare_Lists
    (Contig_t & a, Contig_t & b, Compare_t & cmp);
static void  Make_Intersecting_Contig_List
    (vector <int> & v, const vector <int> & u_list,
     const vector <Unitig_t> & contig, int max_id);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   vector <Compare_t>  exact;
   vector <Compare_t>  close;
   vector <Compare_t>  contained;
   vector <Compare_t>  all_unmatched;
   vector < vector <Compare_t> >  other;
   vector <Contig_t>  a, b;
   vector <int>  v;
   Compare_t  cmp, save;
   vector <Compare_t>  cmp_list;
   Contig_t  con;
   char  * p;
   char  line [50000];
   FILE  * fp;
   int total_a_frags = 0, total_b_frags = 0;
   int total_a_unmatched = 0, total_b_unmatched = 0;
   int  aun, bun, bother, ct;
   int  max_id;
   int  i, j, k, m, n, num;

   fprintf (stderr, "Starting on  %s  at  %s\n", __DATE__, __TIME__);

   Verbose = 3;

   if  (argc < 3)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   // read contigs from file a
   fp = File_Open (argv [1], "r");

   max_id = 0;
   while  (fgets (line, 50000, fp) != NULL)
     {
      con . list . clear ();
      con . matched . clear ();

      p = strtok (line, " \t\n");
      con . id = strdup (p);

      p = strtok (NULL, " \t\n");
      n = strtol (p, NULL, 10);

      for  (i = 0;  i < n;  i ++)
        {
         p = strtok (NULL, " \t\n");
         num = strtol (p, NULL, 10);
         con . list . push_back (num);
         if  (num > max_id)
             max_id = num;
        }

      sort (con . list . begin (), con . list . end ());
      m = con . list . size ();
      for  (i = k = 1;  i < m;  i ++)
        if  (con . list [i] != con . list [i - 1])
            {
             if  (i != k)
                 con . list [k] = con . list [i];
             k ++;
            }
      for  ( ;  k < m;  k ++)
        con . list . pop_back ();
      m = con . list . size ();
      for  (i = 0;  i < m;  i ++)
        con . matched . push_back (false);

      con . unmatched = 0;
      a . push_back (con);
     }

   fclose (fp);

   fprintf (stderr, "%u a contigs\n", a . size ());
   vector <Unitig_t>  a_contig (max_id + 1);
   n = a . size ();
   for  (i = 0;  i < n;  i ++)
     {
      m = a [i] . list . size ();
      for  (j = 0;  j < m;  j ++)
        {
         int  u = a [i] . list [j];

         switch  (a_contig [u] . status)
           {
            case  0 :
              a_contig [u] . sub = i;
              a_contig [u] . status = 1;
              break;
            case  1 :
              v . clear ();
              v . push_back (a_contig [u] . sub);
              v . push_back (i);
              a_contig [u] . sub = Overflow . size ();
              Overflow . push_back (v);
              a_contig [u] . status = 2;
              break;
            case  2 :
              Overflow [a_contig [u] . sub] . push_back (i);
              break;
            default :
              assert (0);
           }
        }
     }
   for  (i = 1;  i <= max_id;  i ++)
     if  (a_contig [i] . status == 2)
         {
          fprintf (stderr, "%7d in", i);
          k = a_contig [i] . sub;
          m = Overflow [k] . size ();
          for  (j = 0;  j < m;  j ++)
            fprintf (stderr, "  %7d", Overflow [k] [j]);
          fprintf (stderr, "\n");
         }


   // read contigs from file b
   fp = File_Open (argv [2], "r");

   max_id = 0;
   while  (fgets (line, 50000, fp) != NULL)
     {
      con . list . clear ();
      con . matched . clear ();

      p = strtok (line, " \t\n");
      con . id = strdup (p);

      p = strtok (NULL, " \t\n");
      n = strtol (p, NULL, 10);

      for  (i = 0;  i < n;  i ++)
        {
         p = strtok (NULL, " \t\n");
         num = strtol (p, NULL, 10);
         con . list . push_back (num);
         if  (num > max_id)
             max_id = num;
        }

      sort (con . list . begin (), con . list . end ());
      m = con . list . size ();
      for  (i = k = 1;  i < m;  i ++)
        if  (con . list [i] != con . list [i - 1])
            {
             if  (i != k)
                 con . list [k] = con . list [i];
             k ++;
            }
      for  ( ;  k < m;  k ++)
        con . list . pop_back ();
      m = con . list . size ();
      for  (i = 0;  i < m;  i ++)
        con . matched . push_back (false);

      con . unmatched = 0;
      b . push_back (con);
     }

   fclose (fp);

   fprintf (stderr, "%u b contigs\n", b . size ());
   vector <Unitig_t>  b_contig (max_id + 1);
   n = b . size ();
   for  (i = 0;  i < n;  i ++)
     {
      m = b [i] . list . size ();
      for  (j = 0;  j < m;  j ++)
        {
         int  u = b [i] . list [j];

         switch  (b_contig [u] . status)
           {
            case  0 :
              b_contig [u] . sub = i;
              b_contig [u] . status = 1;
              break;
            case  1 :
              v . clear ();
              v . push_back (b_contig [u] . sub);
              v . push_back (i);
              b_contig [u] . sub = Overflow . size ();
              Overflow . push_back (v);
              b_contig [u] . status = 2;
              break;
            case  2 :
              Overflow [b_contig [u] . sub] . push_back (i);
              break;
            default :
              assert (0);
           }
        }
     }
   for  (i = 1;  i <= max_id;  i ++)
     if  (b_contig [i] . status == 2)
         {
          fprintf (stderr, "%7d in", i);
          k = b_contig [i] . sub;
          m = Overflow [k] . size ();
          for  (j = 0;  j < m;  j ++)
            fprintf (stderr, "  %7d", Overflow [k] [j]);
          fprintf (stderr, "\n");
         }


   // compare contigs
   m = a . size ();
   n = b . size ();


   // first get unmatched counts
   for  (i = 0;  i < m;  i ++)
     {
      assert (a [i] . list . size () == a [i] . matched . size ());

      Make_Intersecting_Contig_List (v, a [i] . list, b_contig, max_id);

      k = v . size ();
      for  (j = 0;  j < k;  j ++)
        Compare_Lists (a [i], b [v [j]], cmp);

      k = a [i] . matched . size ();
      for  (j = 0;  j < k;  j ++)
        if  (! a [i] . matched [j])
            a [i] . unmatched ++;
      total_a_unmatched += a [i] . unmatched;
      total_a_frags += a [i] . list . size ();
     }

   for  (j = 0;  j < n;  j ++)
     {
      k = b [j] . matched . size ();
      for  (i = 0;  i < k;  i ++)
        if  (! b [j] . matched [i])
            b [j] . unmatched ++;
      total_b_unmatched += b [j] . unmatched;
      total_b_frags += b [j] . list . size ();
     }

   fprintf (stderr, "total_a_unmatched = %d\n", total_a_unmatched);
   fprintf (stderr, "total_b_unmatched = %d\n", total_b_unmatched);


   // re-do comparisons to get the other matches
   for  (i = 0;  i < m;  i ++)
     {
      cmp . a_sub = i;
      save = cmp;
      cmp_list . clear ();

      Make_Intersecting_Contig_List (v, a [i] . list, b_contig, max_id);

      k = v . size ();
      for  (j = 0;  j < k;  j ++)
        {
         Compare_Lists (a [i], b [v [j]], cmp);

         if  (cmp . both > 0)
             {
              cmp . b_sub = v [j];
              cmp_list . push_back (cmp);
              save = cmp;
             }
           else
             fprintf (stderr, "cmp . both == 0 for i = %d  v [j] = %d\n",
                      i, v [j]);
        }

      if  (cmp_list . size () == 0)
          {
           save . a_only = a [i] . unmatched;
           all_unmatched . push_back (save);
          }
      else if  (cmp_list . size () == 1)
          {
           assert (save . a_only == a [i] . unmatched);
           if  (save . b_only == 0 && save . a_only == 0)
               exact . push_back (save);
           else if  (b [save . b_sub] . unmatched == save . b_only)
               close . push_back (save);
             else
               contained . push_back (save);
          }
        else
           other . push_back (cmp_list);
     }

   printf ("\n%-25s %6s\n", "Exact Matches:", "match");
   n = exact . size ();
   ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      printf ("%12s %12s %6d\n", a [exact [i] . a_sub] . id,
              b [exact [i] . b_sub] . id, exact [i] . both);
      ct += exact [i] . both;
     }
   printf ("Contigs = %d  match = %d\n", n, ct);

   printf ("\n%-25s %6s %6s %6s\n", "Close Matches:", "match", "Aunmat", "Bunmat");
   n = close . size ();
   aun = bun = ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      printf ("%12s %12s %6d %6d %6d\n",
              a [close [i] . a_sub] . id, b [close [i] . b_sub] . id,
              close [i] . both, close [i] . a_only, close [i] . b_only);
      ct += close [i] . both;
      aun += close [i] . a_only;
      bun += close [i] . b_only;
     }
   printf ("Contigs = %d  match = %d  Aunmat = %d  Bunmat = %d  tot = %d\n",
           n, ct, aun, bun, ct + aun + bun);

   printf ("\n%-25s %6s %6s %6s %6s\n", "Contained Matches:", "match", "Aunmat",
           "Bother", "Bunmat");
   n = contained . size ();
   aun = bun = bother = ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      j = contained [i] . b_sub;
      printf ("%12s %12s %6d %6d %6d %6d\n",
              a [contained [i] . a_sub] . id, b [contained [i] . b_sub] . id,
              contained [i] . both, contained [i] . a_only,
              contained [i] . b_only - b [j] . unmatched,
              b [j] . unmatched);
      ct += contained [i] . both;
      aun += contained [i] . a_only;
      bother += contained [i] . b_only - b [j] . unmatched;
      bun += b [j] . unmatched;
     }
   printf ("Contigs = %d  match = %d  Aun = %d  Bother = %d  Bun = %d  tot = %d\n",
           n, ct, aun, bother, bun, ct + aun + bother + bun);

   printf ("\n%-14s %6s\n", "All Unmatched:", "frags");
   n = all_unmatched . size ();
   ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      printf ("%14s %6d\n", a [all_unmatched [i] . a_sub] . id,
              all_unmatched [i] . a_only);
      ct += all_unmatched [i] . a_only;
     }
   printf ("Contigs = %d  Frags = %d\n", n, ct);

   printf ("\n%-18s %6s %6s %6s\n", "Other Matches:", "match",
           "Bother", "Bunmat");
   n = other . size ();
   ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      int  q;

      q = other [i] [0] . a_sub;
      printf ("%s:\n", a [q] . id);
      m = other [i] . size ();
      for  (j = 0;  j < m;  j ++)
        {
         int  k;

         k = other [i] [j] . b_sub;
         printf ("%18s %6d %6d %6d\n", b [k] . id,
                 other [i] [j] . both,
                 other [i] [j] . b_only - b [k] . unmatched,
                 b [k] . unmatched);
         ct += other [i] [j] . both;
        
        }
      if  (a [q] . unmatched > 0)
          {
           printf ("%18s %6d\n", "unmatched", a [q] . unmatched);
           ct += a [q] . unmatched;
          }
     }
   printf ("Contigs = %d  Frags = %d\n", n, ct);


   printf ("\nTotal A contigs = %d\n", int (a . size ()));
   printf ("Total A frags = %d\n", total_a_frags);
   printf ("Total B contigs = %d\n", int (b . size ()));
   printf ("Total B frags = %d\n", total_b_frags);
   printf ("Total A unmatched = %d\n", total_a_unmatched);
   printf ("Total B unmatched = %d\n", total_b_unmatched);

   fprintf (stderr, "Finished on  %s  at  %s\n", __DATE__, __TIME__);

   return  0;
  }



static void  Compare_Lists
    (Contig_t & a, Contig_t & b, Compare_t & cmp)

//  Compare contents of lists in  a  and  b  and save in
//   cmp  the number of entries that are in  a  only,  b only,
//  and in both.  Also mark entries in  a  and  b  that are
//  matched.

  {
   int  i, j, m, n;

   m = a . list . size ();
   n = b . list . size ();

   cmp . Clear ();

   i = j = 0;
   while  (i < m && j < n)
     {
      if  (a . list [i] < b . list [j])
          {
           cmp . a_only ++;
           i ++;
          }
      else if  (b . list [j] < a . list [i])
          {
           cmp . b_only ++;
           j ++;
          }
        else
          {
           cmp . both ++;
           a . matched [i] = b . matched [j] = true;
           i ++;
           j ++;
          }
     }
   while  (i < m)
     {
      cmp . a_only ++;
      i ++;
     }
   while  (j < n)
     {
      cmp . b_only ++;
      j ++;
     }

   return;
  }



static void  Make_Intersecting_Contig_List
    (vector <int> & v, const vector <int> & u_list,
     const vector <Unitig_t> & contig, int max_id)

//  Make list  v  contain the subscripts of all the contigs
//  that unitigs in  u_list  belong to, using the information
//  in  contig  to find those contigs.   max_id  is the highest
//  subscript in  contig .

  {
   vector <int>  c_list;
   int  i, n, ct;

   v . clear ();
   ct = 0;

   n = u_list . size ();
   for  (i = 0;  i < n;  i ++)
     {
      int  j, k, m, u;

      u = u_list [i];
      if  (u > max_id || contig [u] . status == 0)
          continue;
      c_list . clear ();
      if  (contig [u] . status == 1)
          c_list . push_back (contig [u] . sub);
        else
          c_list = Overflow [contig [u] . sub];
         
      m = c_list . size ();
      for  (k = 0;  k < m;  k ++)
        {
         for  (j = 0;  j < ct && c_list [k] != v [j];  j ++)
           ;

         if  (j == ct)
             {
              v . push_back (c_list [k]);
              ct ++;
             }
        }
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
           "USAGE:  %s  <a-file> <b-file>\n"
           "\n"
           "Compare contigs in file <a-file> with those in <b-file>\n"
           "by comparing their lists of fragments\n"
           "\n"
           "Options:\n"
           "  none\n"
           "\n",
           command);

   return;
  }
