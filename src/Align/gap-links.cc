// Read two files (named on command line):  first is list of
// contig pairs (presumably bounding a gap in a scaffold); second
// is a sorted list of all contig links (in same order as .asm file).
// For each pair in the first file, print the corresponding entries from
// the second file.  Output goes to stdout

#include  "delcher.hh"
#include  <vector>
#include  <algorithm>
using namespace std;


const int  MAX_LINE = 1000;



struct  Link_t
  {
   int  id1, id2;
   char  * line;
  };


static void  Print_Matches
    (int id1, int id2, const vector <Link_t> & link);



int  main
    (int argc, char * argv [])

  {
   FILE  * fp;
   vector <Link_t>  link;
   Link_t  new_link;
   char  line [MAX_LINE];
   int  id1, id2;

   fp = File_Open (argv [2], "r");

   while  (fgets (line, MAX_LINE, fp) != NULL)
     {
      sscanf (line, "%d %d", & new_link . id1, & new_link . id2);
      new_link . line = strdup (line);
      link . push_back (new_link);
     }

   fclose (fp);

cerr << "num links = " << link . size () << endl;

   fp = File_Open (argv [1], "r");
   while  (fgets (line, MAX_LINE, fp) != NULL)
     {
      fputs (line, stdout);
      sscanf (line, "%d %d", & id1, & id2);
      if  (id1 < id2)
          Print_Matches (id1, id2, link);
        else
          Print_Matches (id2, id1, link);
     }


   return  0;
  }



static void  Print_Matches
    (int id1, int id2, const vector <Link_t> & link)

// Find entries in  link  that match  id1  and  id2  and
// print their  line  strings to stdout

  {
   bool  found = false;
   int  lo, hi, mid, n;

   n = link . size ();

   lo = 0;  hi = n - 1;
   while  (lo <= hi && ! found)
     {
      mid = (lo + hi) / 2;
      if  (link [mid] . id1 == id1)
          {
           if  (link [mid] . id2 == id2)
               found = true;
           else if  (id2 < link [mid] . id2)
               hi = mid - 1;
             else
               lo = mid + 1;
          }
      else if  (id1 < link [mid] . id1)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   if  (found)
       {
        while  (mid > 0 && link [mid - 1] . id1 == id1
                  && link [mid - 1] . id2 == id2)
          mid --;
        while  (mid < n && link [mid] . id1 == id1
                  && link [mid] . id2 == id2)
          fputs (link [mid ++] . line, stdout);
       }

   return;
  }



