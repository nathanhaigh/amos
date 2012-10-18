//  A. L. Delcher
//
//  File:  sim-shotgun.cc
//
//  Last Modified:  4 August 2003
//
//  Simulate shotgun assembly


#include  "delcher.hh"
#include  <vector>
#include  <algorithm>
using namespace std;

const int  DEFAULT_MIN_CTG_CT = 2;
const int  DEFAULT_MIN_OLAP_LEN = 40;
const int  ITER_COUNT = 10;
  // Number of times to run simulation


struct  Read_t
  {
   int  lo, hi;

   bool  operator <
       (const Read_t & r)  const
     {
      if  (lo < r . lo)
          return  true;
      if  (lo == r . lo && hi > r . hi)
          return  true;

      return  false;
     }
  };


static double  Coverage;
  // Read_Len * Num_Reads / Genome_Len;
static double  Genome_Len;
  // Length of the genome in bases
static int  Min_Contig_Count = DEFAULT_MIN_CTG_CT;
  // Minimum number of reads to make a contig and be counted
static int  Min_Olap_Len = DEFAULT_MIN_OLAP_LEN;
  // Smallest detected overlap between two reads
static double  Read_Len;
  // Number of bases in each read
static double  Read_Std_Dev = 0.0;
  // Standard deviation in length of reads


static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   vector <Read_t>  read_list;
   Read_t  read;
   time_t  now;
   double  m1, m2, m3;
   double  sd1, sd2, sd3;
   double  s1 = 0.0, s2 = 0.0, s3 = 0.0;
   double  ss1 = 0.0, ss2 = 0.0, ss3 = 0.0;
   int  num_reads;
   int  i, iter;

   Parse_Command_Line  (argc, argv);

   now = time (NULL);
   fprintf (stderr, "Starting at  %s\n", ctime (& now));

   Verbose = 0;

   srand48 (time (NULL));

   num_reads = int ((Coverage * Genome_Len) / Read_Len);

   printf ("Num Reads = %d  Read Len = %.0f,s.d.=%.1f"
        "  Genome Len = %.0f\n",
           num_reads, Read_Len, Read_Std_Dev, Genome_Len);
   printf ("Coverage = %.1fx  Min Reads in Contig = %d  Min Olap Len = %d\n",
        Coverage, Min_Contig_Count, Min_Olap_Len);

   for  (iter = 0;  iter < ITER_COUNT;  iter ++)
     {
      int  start, extent, len, num_contigs, contig_ct;
      int  cover_start;
      double  cover_sum = 0.0;
      double  avg, sum = 0.0;


      read_list . clear ();

double  rm, rs = 0.0, rss = 0.0;
      for  (i = 0;  i < num_reads;  i ++)
        {
         int  read_len;

         read_len = int (0.5 + Read_Len + Pseudo_Normal () * Read_Std_Dev);
//         read_len = int (Read_Len);
rs += read_len;
rss += read_len * read_len;

         read . lo = int (drand48 () * (Genome_Len - read_len));
         read . hi = read . lo + read_len;
         read_list . push_back (read);
        }
rm = rs / num_reads;
printf ("read avg = %.1f  std = %.1f\n", rm,
     sqrt (rss / num_reads - (rm * rm)));

      sort (read_list . begin (), read_list . end ());

      if  (Verbose > 0)
          {
           for  (i = 0;  i < num_reads;  i ++)
             printf ("Read %3d  %7u .. %7u\n", i,
                  read_list [i] . lo, read_list [i] . hi);
          }

      extent = 0;
      num_contigs = 0;
      contig_ct = 0;
      start = 0;
      for  (i = 0;  i < num_reads;  i ++)
        {
if  (Verbose > 0)
printf ("i = %d  lo = %d  hi = %d  contig_ct = %d  start = %d  extent = %d  olap = %d\n",
     i, read_list [i] . lo, read_list [i] . hi, contig_ct, start, extent,
     extent - read_list [i] . lo);

         if  (read_list [i] . lo <= extent - Min_Olap_Len)
             { // same contig
              contig_ct ++;
              if  (read_list [i] . hi > extent)
                  extent = read_list [i] . hi;
             }
           else
             { // new contig
              if  (contig_ct >= Min_Contig_Count)
                  {
                   num_contigs ++;
                   len = extent - start;
                   sum += len;
                   cover_sum += extent - cover_start;
if  (Verbose > 0)
printf ("  ctgnum = %d  len = %d  cover_sum = %.0f\n", num_contigs, len, cover_sum);
                  }
              start = read_list [i] . lo;
              cover_start = Max (start, extent);
              contig_ct = 1;
              extent = read_list [i] . hi;
             }
if  (Verbose > 0)
printf ("  start = %d  extent = %d\n", start, extent);
        }

      // do the last contig
      if  (contig_ct >= Min_Contig_Count)
          {
           num_contigs ++;
           len = extent - start;
           sum += len;
           cover_sum += extent - cover_start;
if  (Verbose > 0)
printf ("  ctgnum = %d  len = %d  cover_sum = %.0f\n", num_contigs, len, cover_sum);
          }

      if  (num_contigs > 0)
          avg = sum / num_contigs;
        else
          avg = 0.0;
      printf ("%4d %8d %10.0f %10.0f\n", iter, num_contigs, avg, cover_sum);

      s1 += num_contigs;
      ss1 += num_contigs * num_contigs;
      s2 += avg;
      ss2 += avg * avg;
      s3 += cover_sum;
      ss3 += cover_sum * cover_sum;
     }

   m1 = s1 / ITER_COUNT;
   m2 = s2 / ITER_COUNT;
   m3 = s3 / ITER_COUNT;
   sd1 = sqrt (ss1 / ITER_COUNT - (m1 * m1));
   sd2 = sqrt (ss2 / ITER_COUNT - (m2 * m2));
   sd3 = sqrt (ss3 / ITER_COUNT - (m3 * m3));
   printf ("mean %8.0f %10.0f %10.0f\n", m1, m2, m3);
   printf ("std  %8.0f %10.0f %10.0f\n", sd1, sd2, sd3);

  { // Lander-Waterman values
   double  theta = Min_Olap_Len / Read_Len;
   double  x = Coverage * (1.0 - theta);
   double  lambda = (exp (x) - 1.0) / Coverage + theta;
   double  sum;
   int  j;

   printf ("Lander-Waterman:\n");
   sum = exp (-1.0 * x);
   for  (j = 1;  j < Min_Contig_Count;  j ++)
     sum -= exp (-2.0 * x) * pow (1.0 - exp (-1.0 * x), j - 1);
   printf ("Number of contigs with >=%d reads = %.1f\n",
          Min_Contig_Count, num_reads * sum);

   if  (Min_Contig_Count > 2)
       printf ("Number of contigs with >=2 reads = %.1f\n",
           num_reads * (exp (-1.0 * x) - exp (-2.0 * x)));
   if  (Min_Contig_Count > 1)
       printf ("Number of contigs with >=1 reads = %.1f\n",
           num_reads * exp (-1.0 * x));
   printf ("Contig length = %.1f\n",
       Read_Len * lambda);
  }

   return  0;
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
             && ((ch = getopt (argc, argv, "c:ho:s:")) != EOF))
     switch  (ch)
       {
        case  'c' :
          Min_Contig_Count = (int) strtol (optarg, & p, 10);
          if  (p == optarg)
              {
               fprintf (stderr, "ERROR:  Bad min contig count \"%s\"\n",
                        optarg);
               errflg = TRUE;
              }
          break;

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

        case  's' :
          Read_Std_Dev = strtod (optarg, & p);
          if  (p == optarg)
              {
               fprintf (stderr, "ERROR:  Bad std dev \"%s\"\n",
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



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <coverage> <read-len> <genome-len>\n"
           "\n"
           "List number of contigs in true random-shotgunning\n"
           "assuming perfect assembly\n"
           "\n"
           "Options:\n"
           "  -c <n>  Set minimum reads in a contig to <n> (default = %d)\n"
           "  -h      Print this message\n"
           "  -o <n>  Set minimum overlap length to <n> (default = %d)\n"
           "  -s <n>  Set read-length std deviation to <n> (default = 0)\n"
           "\n",
           command, DEFAULT_MIN_CTG_CT, DEFAULT_MIN_OLAP_LEN);

   return;
  }
