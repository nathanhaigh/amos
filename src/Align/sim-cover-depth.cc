//  A. L. Delcher
//
//  File:  sim-cover-depth.cc
//
//  Last Modified:  11 March 2003
//
//  Simulate coverage depth for random reads from genome.


#include  "delcher.hh"
#include  <deque>
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
static int  Min_Olap_Len = DEFAULT_MIN_OLAP_LEN;
  // Smallest detected overlap between two reads
static double  Num_Reads;
  // Number of reads randomly distributed in the genome
static double  Read_Len;
  // Number of bases in each read


static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   double  start_prob;
   double  pos, prev_event;
   double  total_coverage = 0.0;
   deque <double>  event_list;
     // This is a list of positions of pending end-of-read events
   vector <int>  cover_ct (1 + Hi_Degree, 0);
   int  total_ct = 0;
   vector <double>  cover_len (1 + Hi_Degree, 0.0);
   int  coverage = 0, prev_coverage = 0;
   int  i;

   Parse_Command_Line  (argc, argv);

   fprintf (stderr, "Starting on  %s  at  %s\n", __DATE__, __TIME__);

   Verbose = 0;

   srand48 (time (NULL));

   printf ("Num Reads = %.0f  Read Len = %.0f  Genome Len = %.0f\n",
           Num_Reads, Read_Len, Genome_Len);

   start_prob = Num_Reads / (1.0 + Genome_Len - Read_Len);

   prev_event = 0.0;
   for  (pos = 0.0;  pos <= Genome_Len;  pos += 1.0)
     {
      bool  have_event = false;

      if  (event_list . size () > 0 && event_list . front () == pos)
          {
           coverage --;
           have_event = true;
           event_list . pop_front ();
          }
      if  (pos <= Genome_Len - Read_Len   // read could start here
            && drand48 () < start_prob)   // and does
          {
           if  (event_list . size () > 0
                  && event_list . back () - pos < Min_Olap_Len)
               {
                 // When the last overlap is less than Min_Olap_Len
                 // then the region is arbitrarily ascribed to
                 // the read that starts here by prematurely
                 // truncating the pending events.
                coverage -= event_list . size ();
                event_list . clear ();
               }
           event_list . push_back (pos + Read_Len);
           coverage ++;
           have_event = true;
          }
      if  (have_event)
          {
           if  (pos > prev_event)
               {
                if  (prev_coverage > Hi_Degree)
                    prev_coverage = Hi_Degree;
                cover_ct [prev_coverage] ++;
                total_ct ++;
                cover_len [prev_coverage] += pos - prev_event;
               }
           prev_event = pos;
           prev_coverage = coverage;
          }
     }

   if  (prev_event < Genome_Len)
       {
        if  (prev_coverage > Hi_Degree)
            prev_coverage = Hi_Degree;
        cover_ct [prev_coverage] ++;
        total_ct ++;
        cover_len [prev_coverage] += Genome_Len - prev_event;
       }


   printf ("%8s  %14s  %19s  %10s\n",
           "Coverage", "# Regions", "Total Length", "Avg Len");
   for  (i = 0;  i < Hi_Degree;  i ++)
     {
      printf ("  %3d:  %8d (%5.1f%%)  %10.0f (%5.1f%%)  %8.1f\n",
              i, cover_ct [i], Percent (cover_ct [i], total_ct),
              cover_len [i], Percent (cover_len [i], Genome_Len),
              Ratio (cover_len [i], cover_ct [i]));
      total_coverage += i * cover_len [i];
     }
   printf (">=%3d:  %8d (%5.1f%%)  %10.0f (%5.1f%%)  %8.1f\n",
           Hi_Degree, cover_ct [Hi_Degree],
           Percent (cover_ct [Hi_Degree], total_ct),
           cover_len [Hi_Degree],
           Percent (cover_len [Hi_Degree], Genome_Len),
           Ratio (cover_len [Hi_Degree], cover_ct [Hi_Degree]));
   total_coverage += i * cover_len [Hi_Degree];
   printf ("\nEffective coverage = %.1fx\n", total_coverage / Genome_Len);

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
             && ((ch = getopt (argc, argv, "hmo:")) != EOF))
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
