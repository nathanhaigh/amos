//  A. L. Delcher
//
//  File:  sim-cover2.cc
//
//  Last Modified:  13 June 2003
//
//  Simulate coverage  of multiple haplotypes


#include  "delcher.hh"
#include  <vector>
#include  <algorithm>
#include  <cassert>
using namespace std;

const unsigned  DEFAULT_MIN_OLAP_LEN = 40;


struct  Pair_t
  {
   unsigned  a, b;
  };

struct  Position_t
  {
   unsigned  pos : 30;
   unsigned  haplo : 1;
   unsigned  begin : 1;   

   bool  operator <
       (const Position_t & p)  const
     {
      if  (pos < p . pos)
          return  true;
      if  (pos == p . pos && haplo < p . haplo)
          return  true;

      return  false;
     }
  };

struct  Read_t
  {
   unsigned  lo : 30;
   unsigned  haplo : 1;
   unsigned  unique : 1;
   unsigned  hi : 30;
   unsigned  deleted : 1;

   bool  operator <
       (const Read_t & r)  const
     {
      if  (lo < r . lo)
          return  true;
      if  (lo == r . lo && haplo < r . haplo)
          return  true;

      return  false;
     }
  };

struct  Result_t
  {
   double  any, both, two_one, two_two;
   double  onex, twox;
   double  c_gaps, u_gaps;
   double  c_gap_ct, u_gap_ct;

   Result_t
       ()
     {
      Clear ();
     }

   void  Clear
       (void)
     {
      any = both = two_one = two_two = onex = twox = c_gaps = u_gaps = 0.0;
      c_gap_ct = u_gap_ct = 0.0;
     }

   void  Add
       (const Result_t & r)
     {
      any += r . any;
      both += r . both;
      two_one += r . two_one;
      two_two += r . two_two;
      onex += r . onex;
      twox += r . twox;
      c_gaps += r . c_gaps;
      u_gaps += r . u_gaps;
      c_gap_ct += r . c_gap_ct;
      u_gap_ct += r . u_gap_ct;
     }

   void  Divide
       (double d)
     {
      assert (d != 0.0);

      any /= d;
      both /= d;
      two_one /= d;
      two_two /= d;
      onex /= d;
      twox /= d;
      c_gaps /= d;
      u_gaps /= d;
      c_gap_ct /= d;
      u_gap_ct /= d;
     }
  };


static double  Coverage;
  // Read_Len * Num_Reads / Genome_Len;
static double  Genome_Len;
  // Length of the genome in bases
static int  Haplo_Count = 1000;
  // Number of separate distinct-haplotype regions
static double  Haplo_Fraction = 0.40;
  // Portion of genome assumed to be distinct haplotypes
static int  Haplo_Len;
  // Length of each distinct-haplotype region
static int  Min_Contig_Count = 2;
  // Minimum number of reads to make a contig and be counted
static unsigned  Min_Olap_Len = DEFAULT_MIN_OLAP_LEN;
  // Smallest detected overlap between two reads
static double  Read_Len;
  // Number of bases in each read
static vector <Pair_t>  Unique_List;
  // Regions of genome that are completely separate haplotypes


static void  Analyze_Haplo
    (unsigned hap, vector <Read_t> read_list, Result_t & result);
static void  Find_Contigs
    (unsigned hap, vector <Read_t> & list);
static void  Get_Region_Types
    (unsigned lo, unsigned hi, unsigned & c_len, unsigned & u_len);
static bool  Hits_Any
    (unsigned lo, unsigned hi, vector <Pair_t> & list);
static void  Make_Unique_List
    (vector <Pair_t> & list);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Print_Results
    (const Result_t & a, const Result_t & b);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   vector <Read_t>  read_list;
   Read_t  read;
   Result_t  result_a, result_b, comb_a, comb_b;
   int  num_reads;
   int  iter_count = 10;
   int  i, iter;

   Parse_Command_Line  (argc, argv);

   fprintf (stderr, "Starting on  %s  at  %s\n", __DATE__, __TIME__);

   Verbose = 0;

//   srand48 (time (NULL));

   num_reads = int ((Coverage * Genome_Len) / Read_Len);

   printf ("Num Reads = %d  Read Len = %.0f,s.d.=100.0"
        "  Genome Len = %.0f  Coverage = %.1fx\n",
           num_reads, Read_Len, Genome_Len, Coverage);
   Haplo_Len = int (Haplo_Fraction * Genome_Len) / Haplo_Count;
   printf ("Haplos:  Fraction = %.2f  Count = %d  Len = %d\n",
        Haplo_Fraction, Haplo_Count, Haplo_Len);

   Make_Unique_List (Unique_List);

   if  (Verbose > 0)
       {
        int  n;

        n = Unique_List . size ();
        for  (i = 0;  i < n;  i ++)
          printf ("Unique  %7u .. %7u\n", Unique_List [i] . a,
               Unique_List [i] . b);
       }

   for  (iter = 1;  iter <= iter_count;  iter ++)
     {
      read_list . clear ();

      for  (i = 0;  i < num_reads;  i ++)
        {
         int  read_len;

         read_len = int (Read_Len + Pseudo_Normal () * 100.0);

         read . haplo = (drand48 () < 0.50 ? 0 : 1);
         read . lo = int (drand48 () * (Genome_Len - read_len));
         read . hi = read . lo + int (read_len);
         read . unique = Hits_Any (read . lo, read . hi, Unique_List);
         read . deleted = 0;
         read_list . push_back (read);
        }

      sort (read_list . begin (), read_list . end ());

      if  (Verbose > 0)
          {
           for  (i = 0;  i < num_reads;  i ++)
             printf ("Read %3d  %7u .. %7u   %u  %u\n", i,
                  read_list [i] . lo, read_list [i] . hi,
                  read_list [i] . haplo, read_list [i] . unique);
          }

      Analyze_Haplo (0, read_list, result_a);
      if  (Verbose > 0)
          printf ("Result A:  %7.0f %7.0f %7.0f %7.0f\n",
               result_a . any, result_a . both, result_a . two_one,
               result_a . two_two);
      Analyze_Haplo (1, read_list, result_b);
      if  (Verbose > 0)
          printf ("Result B:  %7.0f %7.0f %7.0f %7.0f\n",
               result_b . any, result_b . both, result_b . two_one,
               result_b . two_two);

      comb_a . Add (result_a);
      comb_b . Add (result_b);
     }

   comb_a . Divide (iter_count);
   comb_b . Divide (iter_count);
   Print_Results (comb_a, comb_b);

   return  0;
  }



static void  Analyze_Haplo
    (unsigned hap, vector <Read_t> read, Result_t & result)

//  Analyze the genome using unique reads of haplotype  hap  plus
//  non-unique reads.  Reads are in list  read .  Put results of
//  analysis into  result .  Note that list  read  is a *COPY* of
//  the original list--we'll change it without worrying about the original.

  {
   vector <Position_t>  list;
   Position_t  pos;
   int  cov [2];
   unsigned  prev;
   int  i, n, ct;

   result . Clear ();

   Find_Contigs (hap, read);

   n = read . size ();
   ct = 0;
   for  (i = 0;  i < n;  i ++)
     if  (! read [i] . deleted)
         {
          pos . pos = read [i] . lo;
          pos . haplo = read [i] . haplo;
          pos . begin = 1;
          list . push_back (pos);
          pos . pos = read [i] . hi;
          pos . begin = 0;
          list . push_back (pos);
          ct += 2;
         }

   if  (ct == 0)
       return;
   sort (list . begin (), list . end ());

   cov [0] = cov [1] = 0;
   prev = list [0] . pos;
   cov [list [0] . haplo] ++;
   if  (Verbose > 0)
       printf ("%7u  %u  %3d  %3d\n", list [0] . pos, list [0] . begin,
            cov [0], cov [1]);

   for  (i = 1;  i < ct;  i ++)
     {
      if  (list [i] . pos > prev)
          {
           unsigned  c_len, u_len;

           Get_Region_Types (prev, list [i] . pos, c_len, u_len);

           if  (Verbose > 0)
               printf ("Region %7d .. %7d  %5d %5d   %3d %3d\n",
                    prev, list [i] . pos, c_len, u_len, cov [0], cov [1]);

           if  (cov [0] == 0 && cov [1] == 0)
               {
                if  (u_len > 0)
                    {
                     result . u_gaps += u_len;
                     result . u_gap_ct += 1.0;
                    }
                  else
                    {
                     result . c_gaps += c_len;
                     result . c_gap_ct += 1.0;
                    }
               }
           if  (cov [0] > 0 || cov [1] > 0)
               result . any += c_len;
           if  (cov [0] > 0 && cov [1] > 0)
               {
                result . both += c_len;
                if  (cov [0] > 1 || cov [1] > 1)
                    result . two_one += c_len;
                if  (cov [0] > 1 && cov [1] > 1)
                    result . two_two += c_len;
               }
           if  (cov [hap] > 0)
               result . onex += u_len;
           if  (cov [hap] > 1)
               result . twox += u_len;
           prev = list [i] . pos;
          }
      if  (list [i] . begin)
          cov [list [i] . haplo] ++;
        else
          cov [list [i] . haplo] --;

     if  (Verbose > 0)
         printf ("%7u  %u  %3d  %3d\n", list [i] . pos, list [i] . begin,
              cov [0], cov [1]);
     }

   return;
  }



static void  Find_Contigs
    (unsigned hap, vector <Read_t> & read)

//  Identify reads in  read  that could be part of a contig with at
//  least  Min_Contig_Count  reads.  Ignore all unique reads that are
//  not haplotype  hap .  Set  deleted  in all the others
//  to  true .  Also trim the ends of reads on the ends of contigs that
//  overlap by less than  Min_Olap_Len  bases.

  {
   vector <int>  contig;
   unsigned  adjust_pos, extent;
   int  i, j, m, n;

   adjust_pos = extent = 0;

   n = read . size ();
   for  (i = 0;  i < n;  i ++)
     {
      if  (read [i] . unique && read [i] . haplo != hap)
          {
           read [i] . deleted = 1;
           continue;
          }

      if  (extent - read [i] . lo < Min_Olap_Len)
          {  // previous contig has ended
           m = contig . size ();
           if  (m < Min_Contig_Count)
               {
                for  (j = 0;  j < m;  j ++)
                  read [contig [j]] . deleted = 1;
               }
           else if  (read [i] . lo < extent)
               {  // overlap too small--trim ends
                adjust_pos = (extent + read [i] . lo) / 2;

                for  (j = 0;  j < m;  j ++)
                  if  (read [contig [j]] . hi > adjust_pos)
                      read [contig [j]] . hi = adjust_pos;
               }

           contig . clear ();
          }

      if  (read [i] . lo < adjust_pos)
          read [i] . lo = adjust_pos;
      contig . push_back (i);

      if  (extent < read [i] . hi)
          extent = read [i] . hi;
     }

   return;
  }



static void  Get_Region_Types
    (unsigned lo, unsigned hi, unsigned & c_len, unsigned & u_len)

//  Match the region  lo .. hi  (in gapped coordinates) against
//  the regions in  Unique_List  and set  u_len  to the size of the
//  region that intersects a unique and  c_len  to the size of the
//  remainder.  This version assumes  lo .. hi  hits at most one
//  of the regions in  Unique_List .

  {
   int  i, n;

   u_len = 0;

   n = Unique_List . size ();
   for  (i = 0;  i < n;  i ++)
     if  (lo < Unique_List [i] . b && hi > Unique_List [i] . a)
         {  // found it
          u_len = Min (hi, Unique_List [i] . b) - Max (Unique_List [i] . a, lo);
          break;
         }

   c_len = hi - lo - u_len;

   return;
  }



static bool  Hits_Any
    (unsigned lo, unsigned hi, vector <Pair_t> & list)

//  Return  true  iff region  lo .. hi  (in gapped coordinates)
//  intersects any region on  list .

  {
   int  i, n;

   n = list . size ();
   for  (i = 0;  i < n;  i ++)
     if  (lo < list [i] . b && hi > list [i] . a)
         return  true;

   return  false;
  }



static void  Make_Unique_List
    (vector <Pair_t> & list)

//  Put into  list  Haplo_Count  evenly spaced regions of size
//   Haplo_Len  representing a total of  Haplo_Fraction  of
//   Genome_Len .

  {
   int  i, lo, skip;

   list . clear ();

   skip = int (((1.0 - Haplo_Fraction) * Genome_Len) / Haplo_Count);

   lo = skip / 2;
   for  (i = 0;  i < Haplo_Count;  i ++)
     {
      Pair_t  p;

      p . a = lo;
      p . b = lo + Haplo_Len;

      list . push_back (p);

      lo += skip + Haplo_Len;
     }

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
             && ((ch = getopt (argc, argv, "ho:")) != EOF))
     switch  (ch)
       {
        case  'h' :
          errflg = TRUE;
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

   Coverage = strtod (argv [optind], & p);
   if  (p == argv [optind])
       {
        fprintf (stderr, "ERROR:  Bad coverage value \"%s\"\n",
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



static void  Print_Results
    (const Result_t & a, const Result_t & b)

//  Print to  stdout  the combined results in  a  (for haplotype A)
//  and  b  (for haplotype B).

  {
   double  region_len;
   double  x, y;
   
   printf ("Common (SNP) Regions:\n");
   region_len = (1.0 - Haplo_Fraction) * Genome_Len;
   x = (a . any + b . any) / 2.0;
   printf ("%20s  %9.0f %6.2f%%\n", "Total Coverage", x, Percent (x, region_len));
   x = (a . both + b . both) / 2.0;
   printf ("%20s  %9.0f %6.2f%%\n", "Both Haplotypes", x, Percent (x, region_len));
   x = (a . two_one + b . two_one) / 2.0;
   printf ("%20s  %9.0f %6.2f%%\n", "Two + One", x, Percent (x, region_len));
   x = (a . two_two + b . two_two) / 2.0;
   printf ("%20s  %9.0f %6.2f%%\n", "Two + Two", x, Percent (x, region_len));
   x = (a . c_gap_ct + b . c_gap_ct) / 2.0;
   printf ("%20s  %9.0f\n", "Num Gaps", x);
   y = (a . c_gaps + b . c_gaps) / 2.0;
   printf ("%20s  %9.0f\n", "Avg Gap Len", x > 0.0 ? y / x : 0.0);

   printf ("Haplotype A Regions:\n");
   region_len = Haplo_Fraction * Genome_Len;
   printf ("%20s  %9.0f %6.2f%%\n", "Total Coverage", a . onex ,
        Percent (a . onex, region_len));
   printf ("%20s  %9.0f %6.2f%%\n", ">=2x Coverage", a . twox ,
        Percent (a . twox, region_len));
   x = a . u_gap_ct;
   printf ("%20s  %9.0f\n", "Num Gaps", x);
   printf ("%20s  %9.0f\n", "Avg Gap Len", x > 0.0 ? a . u_gaps / x : 0.0);
   
   printf ("Haplotype B Regions:\n");
   printf ("%20s  %9.0f %6.2f%%\n", "Total Coverage", b . onex ,
        Percent (b . onex, region_len));
   printf ("%20s  %9.0f %6.2f%%\n", ">=2x Coverage", b . twox ,
        Percent (b . twox, region_len));
   x = b . u_gap_ct;
   printf ("%20s  %9.0f\n", "Num Gaps", x);
   printf ("%20s  %9.0f\n", "Avg Gap Len", x > 0.0 ? b . u_gaps / x : 0.0);
   
   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <coverage> <read-len> <genome-len>\n"
           "\n"
           "List coverage by multiple haplotypes of a genome assuming\n"
           "perfect assembly\n"
           "\n"
           "Options:\n"
           "  -h      Print this message\n"
           "  -o <n>  Set minimum overlap length to <n> (default = %d)\n"
           "\n",
           command, DEFAULT_MIN_OLAP_LEN);

   return;
  }
