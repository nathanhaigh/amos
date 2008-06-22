// Read assembly read-info information from stdin and output a list of
// reads *not* in the assembly whose mates *are* in the assembly.
// Also output which gap in which scaffold each should go in.
// Output goes to stdout

#include  <cstdio>
#include  <cstdlib>
#include  <cmath>
#include  <vector>
#include  <algorithm>
#include  <cassert>
#include  <cstring>

using namespace std;

const int  MAX_LINE = 1000;



struct  Read_Info_t
  {
   int  id;
   int  mate_id;
   int  dst;
   int  unitig_id, contig_id;
   int  c_a_pos, c_b_pos;

   Read_Info_t  ()
     {
      mate_id = -1;
      contig_id = -1;
      unitig_id = -1;
     }
  };


struct  Contig_Info_t
  {
   int  id;
   int  scaff_id;
   int  scaff_place;  // ordinal position in scaffold, starting at 1
   char  ori;         // orientation in scaffold, forward or reverse
   double  gap_mean;  // immediately preceding gap
   double  gap_stddev;

   Contig_Info_t  ()
     {
      scaff_id = -1;
     }
  };


struct  Dst_Info_t
  {
   double  mean;
   double  stddev;
  };



bool  Contig_Cmp
    (const Contig_Info_t & a, const Contig_Info_t & b)
  { return  a . id < b . id; }

bool  Read_Cmp
    (const Read_Info_t & a, const Read_Info_t & b)
  { return  a . id < b . id; }


static int  Find_ID
    (vector <Read_Info_t> & read, int  id);



int  main
    (int argc, char * argv [])

  {
   vector <Read_Info_t>  read;
   vector <Contig_Info_t>  contig;
   vector <Dst_Info_t>  dst;
   Read_Info_t  new_read;
   char  line [MAX_LINE];
   bool  found, reads_sorted = false;
   int  id1, id2;
   int  a, b;
   int  i, n;

   while  (fgets (line, MAX_LINE, stdin) != NULL)
     {
      char  tag [10], ori [10], typ [10];
      double  mean, stddev;

      sscanf (line, "%s", tag);
      if  (strcmp (tag, "LKG") == 0)
          {
           sscanf (line + 4, "%d %d %lf %lf %s",
               & id1, & id2, & mean, & stddev, ori);

           if  (ori [0] != 'I')
               {
                fprintf (stderr, "ERROR:  Unexpected orientation \"%s\"\n",
                    ori);
                fputs (line, stderr);
                exit (EXIT_FAILURE);
               }

           n = dst . size ();
           found = false;
           for  (i = 0;  i < n && ! found;  i ++)
             if  (dst [i] . mean == mean && dst [i] . stddev == stddev)
                 {
                  found = true;
                  break;
                 }
           if  (! found)
               {
                Dst_Info_t  new_dst;

                new_dst . mean = mean;
                new_dst . stddev = stddev;
                dst . push_back (new_dst);
               }

           new_read . id = id1;
           new_read . mate_id = id2;
           new_read . dst = i;
           read . push_back (new_read);

           new_read . id = id2;
           new_read . mate_id = id1;
           read . push_back (new_read);
          }
      else if  (strcmp (tag, "CCO") == 0)
          {
           if  (! reads_sorted)
               {
                sort (read . begin (), read . end (), Read_Cmp);
                reads_sorted = true;
               }
           sscanf (line + 4, "%d %d %d %d %s",
               & id1, & id2, & a, & b, typ);

           if  (typ [0] != 'R')
               {
                fprintf (stderr, "ERROR:  Unexpected type \"%s\"\n",
                    ori);
                fputs (line, stderr);
                exit (EXIT_FAILURE);
               }

           i = Find_ID (read, id1);
           if  (i < 0)
               {
                ; // has no mate, do nothing
               }
             else
               {
                assert (read [i] . id == id1);
                read [i] . contig_id = id2;
                read [i] . c_a_pos = a;
                read [i] . c_b_pos = b;
               }
          }
      else if  (strcmp (tag, "UTG") == 0)
          {
           if  (! reads_sorted)
               {
                sort (read . begin (), read . end (), Read_Cmp);
                reads_sorted = true;
               }
           sscanf (line + 4, "%d %d %d %d %s",
               & id1, & id2, & a, & b, typ);

           if  (typ [0] != 'R')
               {
                fprintf (stderr, "ERROR:  Unexpected type \"%s\"\n",
                    ori);
                fputs (line, stderr);
                exit (EXIT_FAILURE);
               }

           i = Find_ID (read, id1);
           if  (i < 0)
               {
                ; // has no mate, do nothing
               }
             else
               {
                assert (read [i] . id == id1);
                read [i] . unitig_id = id2;
               }
          }
      else if  (strcmp (tag, "SCF") == 0)
          {
           Contig_Info_t  c;

           sscanf (line + 4, "%d %d %d %s %lf %lf",
               & id1, & id2, & a, ori, & mean, & stddev);

           assert (ori [0] == 'F' || ori [0] == 'R');
           c . id = id1;
           c . scaff_id = id2;
           c . scaff_place = a;
           c . ori = ori [0];
           c . gap_mean = mean;
           c . gap_stddev = stddev;
           contig . push_back (c);
          }
      else if  (strcmp (tag, "CON") == 0
                  || strcmp (tag, "UPS") == 0
                  || strcmp (tag, "UNI") == 0
               )
          {
           ;  // ignore
          }
        else
          {
           fprintf (stderr, "ERROR:  Unexpected tag\n");
           fputs (line, stderr);
           exit (EXIT_FAILURE);
          }
     }

   sort (contig . begin (), contig . end (), Contig_Cmp);


   // Find reads that are not in any contig and see where
   // their mates place them
   n = read . size ();
   fprintf (stderr, "### %d reads\n", n);
   for  (i = 0;  i < n;  i ++)
     {
      if  (read [i] . contig_id >= 0)
          {
           int  j;

           j = Find_ID (read, read [i] . mate_id);
           assert (j >= 0);
           if  (read [j] . unitig_id == -1)
               printf ("%9d %9d %8d %8d %9d\n", read [i] . id,
                   read [i] . contig_id, read [i] . c_a_pos,
                   read [i] . c_b_pos, read [i] . mate_id);
          }
     }

   return  0;
  }



static int  Find_ID
    (vector <Read_Info_t> & read, int  id)

//  Do a binary search of  read  and return the subscript
//  of the read whose id is  id .  Return  -1  if not found.
//   read  must already be sorted into ascending order by  id .

  {
   int  lo, hi, mid;

   lo = 0;
   hi = int (read . size () - 1);

   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (read [mid] . id == id)
          return  mid;
      else if  (id < read [mid] . id)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   return  -1;
  }
 


