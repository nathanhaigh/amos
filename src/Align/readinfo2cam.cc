// Read read-info information from stdin and a file of
// contig position information (from file named on the command line).
// Generate a celamy cam file for the reads that are in the contig file.
// Output goes to stdout

#include  <cstdio>
#include  <cstdlib>
#include  <cmath>
#include  <vector>
#include  <algorithm>
using namespace std;

const int  MAX_LINE = 1000;
const char  Unique_Col_Id [] = "1UniUtgCol";
const char  Unique_Col_Def [] = "CFF00FF";  // pink
const char  Rock_Col_Id [] = "1RockUtgCol";
const char  Rock_Col_Def [] = "CFF0000";    // red
const char  Surr_Col_Id [] = "1SurrUtgCol";
const char  Surr_Col_Def [] = "CB19B56";    // tan
const char  Degen_Col_Id [] = "1DegenUtgCol";
const char  Degen_Col_Def [] = "C00FFFF";    // blue/green
const char  Degen_Link_Col_Id [] = "1DegenScaff";
const char  Degen_Link_Col_Def [] = "C00FFFF";    // blue/green
const char  Scaff_Link_Col_Id [] = "2RegScaff";
const char  Scaff_Link_Col_Def [] = "C00FF00";    // blue
const char  Single_Read_Col_Id [] = "1SingRead";
const char  Single_Read_Col_Def [] = "C0000FF";    // blue
const char  Single_Mate_Col_Id [] = "1SingMate";
const char  Single_Mate_Col_Def [] = "C0000BF";    // blue
const char  Bad_Read_Col_Id [] = "2BadRead";
const char  Bad_Read_Col_Def [] = "CFF0000";    // red
const char  Bad_Mate_Col_Id [] = "2BadMate";
const char  Bad_Mate_Col_Def [] = "CBF0000";    // red
const char  Stretch_Read_Col_Id [] = "3StretchRead";
const char  Stretch_Read_Col_Def [] = "CFF00FF";    // pink
const char  Stretch_Mate_Col_Id [] = "3StretchMate";
const char  Stretch_Mate_Col_Def [] = "CBF00BF";    // pink
const char  Good_Read_Col_Id [] = "4GoodRead";
const char  Good_Read_Col_Def [] = "C00FF00";    // green
const char  Good_Mate_Col_Id [] = "4GoodMate";
const char  Good_Mate_Col_Def [] = "C00BF00";    // green
const char  Infer_Read_Col_Id [] = "5InferRead";
const char  Infer_Read_Col_Def [] = "CFF7F00";    // orange
const char  Infer_Mate_Col_Id [] = "5InferMate";
const char  Infer_Mate_Col_Def [] = "CBF7500";    // orange



struct  Read_Info_t
  {
   int  mate;
   int  dst;
   long long int  unitig_id;
   int  u_a_pos, u_b_pos;
   long long int  contig_id;
   int  c_a_pos, c_b_pos;
   int  cam_a, cam_b;
   bool  mark;

   Read_Info_t  ()
     {
      mate = -1;
      unitig_id = -1;
      contig_id = -1;
      cam_a = cam_b = -1;
     }
  };


struct  Unitig_Info_t
  {
   long long int  unitig_id;
   long long int  contig_id;
   int  c_a_pos, c_b_pos;
   char  typ;
   int  cam_lo, cam_hi;
   int  match;
   short  copy_num, copy_total;
   bool  cam_fwd;
  };


struct  Contig_Info_t
  {
   long long int  contig_id;
   long long int  scaff_id;
   int  scaff_place;
   char  ori;
   double  gap_mean;
   double  gap_stddev;
   int  match;

   Contig_Info_t  ()
     {
      match = -1;
     }
  };


struct  Contig_Pos_t
  {
   long long int  contig_id;
   long long int   scaff_id;
   int  a, b;
   int  len;
   char  * tag;
   bool  mark;

   Contig_Pos_t  ()
     {
      scaff_id = -1;
     }
  };


struct  Dst_Info_t
  {
   double  mean;
   double  stddev;
  };


struct  Uni_Data_t
  {
   long long int  unitig_id;
   float  coverage;
   char  typ;
   int  num_reads;
   int  len;
  };


struct  Con_Data_t
  {
   long long int  contig_id;
   char  typ;
   int  num_reads;
   int  num_unis;
   int  len;
  };



static vector <Dst_Info_t>  Dst;
static vector <Read_Info_t>  Read;



bool  Contig_Cmp
    (const Contig_Info_t & a, const Contig_Info_t & b)
  { return  a . contig_id < b . contig_id; }

bool  Contig_Pos_Cmp
    (const Contig_Pos_t & a, const Contig_Pos_t & b)
  { return  a . contig_id < b . contig_id; }

bool  Unitig_Cmp
    (const Unitig_Info_t & a, const Unitig_Info_t & b)
  { return  a . unitig_id < b . unitig_id; }

bool  Uni_Data_Cmp
    (const Uni_Data_t & a, const Uni_Data_t & b)
  { return  a . unitig_id < b . unitig_id; }


static void  Output_Mates
    (int i, int j);
static void  Output_Pair
    (int i, int j, const char * read_colour, const char * mate_colour,
     const char * tag);
static void  Output_Single_Read
    (int i, const char * read_colour, const char * mate_colour,
     int len, const char * tag);
static void  Print_Colour_Headers
    (void);
static int  Search
    (const vector <Contig_Pos_t> & pos, long long int id);
static void  Set_Position
    (int & x, int & y, int a, int b, int c, int d, int len);
static int  Uni_Data_Search
    (const vector <Uni_Data_t> & u, long long int id);
static int  Unitig_Search
    (const vector <Unitig_Info_t> & u, long long int id);



int  main
    (int argc, char * argv [])

  {
   FILE  * fp;
   vector <Unitig_Info_t>  unitig;
   vector <Contig_Info_t>  contig;
   vector <Contig_Pos_t>  contig_pos;
   vector <Uni_Data_t>  uni_data;
   vector <Con_Data_t>  con_data;
   char  line [MAX_LINE];
   bool  found;
   float  cov;
   short  copy_ct;
   int  unitig_ct;
   long long int  id1, id2, hi_id;
   int  a, b, c;
   int  hi_read = 0;
   int  i, j, n;

   if  (argc < 2)
       {
        fprintf (stderr, "USAGE:  %s <config-info-file>\n",
            argv [0]);
        exit (EXIT_FAILURE);
       }

   fp = fopen (argv [1], "r");
   assert (fp != NULL);
   while  (fscanf (fp, "%lld %d %d %s", & id1, & a, & b, line) == 4)
     {
      Contig_Pos_t  c;

      c . contig_id = id1;
      c . a = a;
      c . b = b;
      c . tag = strdup (line);
      contig_pos . push_back (c);
     }

   while  (fgets (line, MAX_LINE, stdin) != NULL)
     {
      char  tag [10], ori [10], typ [10];
      double  mean, stddev;

      sscanf (line, "%s", tag);
      if  (strcmp (tag, "LKG") == 0)
          {
           sscanf (line + 4, "%lld %lld %lf %lf %s",
               & id1, & id2, & mean, & stddev, ori);
           if  (id1 > id2)
               hi_id = id1;
             else
               hi_id = id2;
           if  (hi_id > hi_read)
               {
                hi_read = hi_id;
                Read . resize (hi_read + 1);
               }

           if  (ori [0] != 'I')
               {
                fprintf (stderr, "ERROR:  Unexpected orientation \"%s\"\n",
                    ori);
                fputs (line, stderr);
                exit (EXIT_FAILURE);
               }

           if  (Read [id1] . mate != -1 || Read [id2] . mate != -1)
               {
                fprintf (stderr, "ERROR:  Duplicate mate\n");
                fprintf (stderr, "id1 = %lld  mate = %d\n", id1, Read [id1] . mate);
                fprintf (stderr, "id2 = %lld  mate = %d\n", id2, Read [id2] . mate);
                exit (EXIT_FAILURE);
               }
           Read [id1] . mate = id2;
           Read [id2] . mate = id1;

           n = Dst . size ();
           found = false;
           for  (i = 0;  i < n && ! found;  i ++)
             if  (Dst [i] . mean == mean && Dst [i] . stddev == stddev)
                 {
                  found = true;
                  break;
                 }
           if  (! found)
               {
                Dst_Info_t  new_dst;

                new_dst . mean = mean;
                new_dst . stddev = stddev;
                Dst . push_back (new_dst);
               }
           Read [id1] . dst = i;
           Read [id2] . dst = i;
           
          }
      else if  (strcmp (tag, "UTG") == 0)
          {
           sscanf (line + 4, "%lld %lld %d %d %s",
               & id1, & id2, & a, & b, typ);
           if  (id1 > hi_read)
               {
                hi_read = id1;
                Read . resize (hi_read + 1);
               }

           if  (typ [0] != 'R')
               {
                fprintf (stderr, "ERROR:  Unexpected type \"%s\"\n",
                    ori);
                fputs (line, stderr);
                exit (EXIT_FAILURE);
               }

           assert (Read [id1] . unitig_id == -1);
           Read [id1] . unitig_id = id2;
           Read [id1] . u_a_pos = a;
           Read [id1] . u_b_pos = b;
          }
      else if  (strcmp (tag, "CCO") == 0)
          {
           sscanf (line + 4, "%lld %lld %d %d %s",
               & id1, & id2, & a, & b, typ);
           if  (id1 > hi_read)
               {
                hi_read = id1;
                Read . resize (hi_read + 1);
               }

           if  (typ [0] != 'R')
               {
                fprintf (stderr, "ERROR:  Unexpected type \"%s\"\n",
                    ori);
                fputs (line, stderr);
                exit (EXIT_FAILURE);
               }

           assert (Read [id1] . contig_id == -1);
           Read [id1] . contig_id = id2;
           Read [id1] . c_a_pos = a;
           Read [id1] . c_b_pos = b;
          }
      else if  (strcmp (tag, "UNI") == 0)
          {
           Uni_Data_t  uni;

           sscanf (line + 4, "%lld %f %s %d %d",
               & id1, & cov, typ, & a, & b);
           uni . unitig_id = id1;
           uni . coverage = cov;
           uni . typ = typ [0];
           uni . num_reads = a;
           uni . len = b;
           uni_data . push_back (uni);
          }
      else if  (strcmp (tag, "CON") == 0)
          {
           Con_Data_t  con;

           sscanf (line + 4, "%lld %s %d %d %d",
               & id1, typ, & a, & b, & c);
           con . contig_id = id1;
           con . typ = typ [0];
           con . num_reads = a;
           con . num_unis = b;
           con . len = c;
           con_data . push_back (con);
          }
      else if  (strcmp (tag, "UPS") == 0)
          {
           Unitig_Info_t  u;

           sscanf (line + 4, "%lld %lld %d %d %s",
               & id1, & id2, & a, & b, typ);

           u . unitig_id = id1;
           u . contig_id = id2;
           u . c_a_pos = a;
           u . c_b_pos = b;
           u . typ = typ [0];
           unitig . push_back (u);
          }
      else if  (strcmp (tag, "SCF") == 0)
          {
           Contig_Info_t  c;

           sscanf (line + 4, "%lld %lld %d %s %lf %lf",
               & id1, & id2, & a, ori, & mean, & stddev);

           assert (ori [0] == 'F' || ori [0] == 'R');
           c . contig_id = id1;
           c . scaff_id = id2;
           c . scaff_place = a;
           c . ori = ori [0];
           c . gap_mean = mean;
           c . gap_stddev = stddev;
           contig . push_back (c);
          }
        else
          {
           fprintf (stderr, "ERROR:  Unexpected tag\n");
           fputs (line, stderr);
           exit (EXIT_FAILURE);
          }
     }

   sort (unitig . begin (), unitig . end (), Unitig_Cmp);
   sort (contig . begin (), contig . end (), Contig_Cmp);
   sort (contig_pos . begin (), contig_pos . end (), Contig_Pos_Cmp);
   sort (uni_data . begin (), uni_data . end (), Uni_Data_Cmp);

   Print_Colour_Headers ();


   // Still Need to Do:
   // Add to  contig_pos  contigs that are not there already but
   // are in the same scaffold as contigs that are in it.


   // Set size of contigs in  contig_pos  to max unitig pos
   // in them
   // Also set  copy_num  and  copy_total  fields in  unitig  array
   n = unitig . size ();
fprintf (stderr, "### %d unitigs\n", n);

   copy_ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      j = Search (contig_pos, unitig [i] . contig_id);

      unitig [i] . match = j;

      if  (j >= 0)
          {
           if  (unitig [i] . c_a_pos > contig_pos [j] . len)
               contig_pos [j] . len = unitig [i] . c_a_pos;
           if  (unitig [i] . c_b_pos > contig_pos [j] . len)
               contig_pos [j] . len = unitig [i] . c_b_pos;
          }

      if  (i == 0 || unitig [i] . unitig_id == unitig [i - 1] . unitig_id)
          copy_ct ++;
        else
          {
           for  (j = 1;  j <= copy_ct;  j ++)
             unitig [i - j] . copy_total = copy_ct;
           copy_ct = 1;
          }
      unitig [i] . copy_num = copy_ct;
     }
   for  (j = 1;  j <= copy_ct;  j ++)
     unitig [n - j] . copy_total = copy_ct;
   

   // Output celamy lines for matching unitigs
   unitig_ct = 0;
   for  (i = 0;  i < n;  i ++)
     {
      j = unitig [i] . match;

      if  (j >= 0)
          {
           const char  * colour;
           int  k, x, y;

           Set_Position (x, y, unitig [i] . c_a_pos, unitig [i] . c_b_pos,
               contig_pos [j] . a, contig_pos [j] . b, contig_pos [j] . len);

           switch  (unitig [i] . typ)
             {
              case  'U' :
                colour = Unique_Col_Id;
                break;
              case  'R' :
                colour = Rock_Col_Id;
                break;
              case  'S' :
                colour = Surr_Col_Id;
                break;
              case  's' :
                colour = Degen_Col_Id;
                break;
              default :
                fprintf (stderr, "ERROR:  Bad unitig type = \"%c\"\n",
                    unitig [i] . typ);
                exit (EXIT_FAILURE);
             }

           k = Uni_Data_Search (uni_data, unitig [i] . unitig_id);
           if  (k < 0)
               {
                fprintf (stderr, "ERROR:  Missing unitig %lld in uni_data\n",
                    unitig [i] . unitig_id);
                exit (EXIT_FAILURE);
               }

           unitig_ct ++;
           if  (x < y)
               printf ("%dUnitig: %d A%s %d R6 # Utg%lld Ctg%lld"
                   " cov=%.1f typ=%c nfr=%d len=%d",
                   unitig_ct, x, colour, y, unitig [i] . unitig_id,
                   unitig [i] . contig_id, uni_data [k] . coverage,
                   uni_data [k] . typ, uni_data [k] . num_reads,
                   uni_data [k] . len);
             else
               printf ("%dUnitig: %d A%s %d R6 # Utg%lld Ctg%lld"
                   "  cov=%.1f typ=%c nfr=%d len=%d",
                   unitig_ct, y, colour, x, unitig [i] . unitig_id,
                   unitig [i] . contig_id, uni_data [k] . coverage,
                   uni_data [k] . typ, uni_data [k] . num_reads,
                   uni_data [k] . len);
           if  (unitig [i] . copy_total > 1)
               printf (" copy=%d of %d", unitig [i] . copy_num,
                    unitig [i] . copy_total);
           putchar ('\n');
          }
     }

   // Mark scaffolds in  contig_pos
   n = contig . size ();
fprintf (stderr, "### %d contigs\n", n);

   for  (i = 0;  i < n;  i ++)
     {
      j = Search (contig_pos, contig [i] . contig_id);

      contig [i] . match = j;
      contig_pos [j] . scaff_id = contig [i] . scaff_id;
     }

   // Output scaffold links
   n = contig_pos . size ();

   for  (i = 0;  i < n;  i ++)
     contig_pos [i] . mark = false;

   for  (i = 0;  i < n;  i ++)
     if  (! contig_pos [i] . mark)
         {
          contig_pos [i] . mark = true;
          printf ("LNK: %s", contig_pos [i] . tag);
          if  (contig_pos [i] . scaff_id == -1)
              printf (" A%s # Degenerate scaff\n", Degen_Link_Col_Id);
            else
              {
               for  (j = i + 1;  j < n;  j ++)
                 if  (contig_pos [j] . scaff_id == contig_pos [i] . scaff_id)
                     {
                      contig_pos [j] . mark = true;
                      printf (" %s", contig_pos [j] . tag);
                     }
               printf (" A%s # Scaff %lld\n", Scaff_Link_Col_Id,
                   contig_pos [i] . scaff_id);
              }
         }

   // Find reads that are in the picture
   n = Read . size ();
fprintf (stderr, "### %d reads\n", n);
   for  (i = 0;  i < n;  i ++)
     {
      Read [i] . mark = false;
      if  (Read [i] . contig_id >= 0)
          {
           j = Search (contig_pos, Read [i] . contig_id);
           if  (j >= 0)
               {
                Set_Position (Read [i] . cam_a, Read [i] . cam_b,
                    Read [i] . c_a_pos, Read [i] . c_b_pos,
                    contig_pos [j] . a, contig_pos [j] . b,
                    contig_pos [j] . len);
               }
          }
      else if  (Read [i] . unitig_id >= 0)
          {
           bool  done;
           int  k;

           j = Unitig_Search (unitig, Read [i] . unitig_id);
           if  (j < 0)
               fprintf (stderr, "** Unitig %lld not found\n",
                    Read [i] . unitig_id);
             else
               {
                done = false;

                for  (k = j;  k < j + unitig [j] . copy_total && ! done;  k ++)
                  {
                   int  p;

                   p = Search (contig_pos, unitig [k] . contig_id);
                   if  (p >= 0)
                       {
                        int  x, y;

                        Set_Position (x, y, unitig [k] . c_a_pos,
                            unitig [k] . c_b_pos, contig_pos [p] . a,
                            contig_pos [p] . b, contig_pos [p] . len);
                        Set_Position (Read [i] . cam_a, Read [i] . cam_b,
                            Read [i] . u_a_pos, Read [i] . u_b_pos,
                            x, y, abs (x - y));
                        done = true;
                       }
                  }
               }
          }
     }

   for  (i = 0;  i < n;  i ++)
     {
      if  (Read [i] . cam_a > 0 && ! Read [i] . mark)
          {
           char  tag [200];

           j = Read [i] . mate;
           if  (j < 0)
               {
                sprintf (tag, "Single read %d (Utg%lld)", i, Read [i] . unitig_id);
                Output_Single_Read (i, Single_Read_Col_Id, Single_Mate_Col_Id,
                    2500, tag);
               }
             else 
               {
// fprintf (stderr, "%8d has mate %8d  cam_a [j] = %d\n", i, j, Read [j] . cam_a);
                if  (Read [j] . cam_a > 0)
                    Output_Mates (i, j);
                  else
                    {
                     int  mean;

                     sprintf (tag, "Read %d (Utg%lld) mate %d (Utg%lld) not in picture",
                         i, Read [i] . unitig_id, j, Read [j] . unitig_id);
                     mean = int (Dst [Read [i] . dst] . mean);
                     Output_Single_Read (i, Infer_Read_Col_Id, Infer_Mate_Col_Id,
                          mean, tag);
                    }
                Read [j] . mark = true;
               }
          }
      Read [i] . mark = true;
     }

   return  0;
  }



static void  Output_Mates
    (int i, int j)

//  Output a celamy line for mated reads  Read [i]  and  Read [j]

  {
   int  mean, sd, diff;
   char  tag [200];
   
   mean = int (Dst [Read [i] . dst] . mean);
   sd = int (Dst [Read [i] . dst] . stddev);
   if  (Read [i] . cam_a <= Read [i] . cam_b)
       {
        if  (Read [j] . cam_a <= Read [j] . cam_b
               || Read [j] . cam_a < Read [i] . cam_a + 200
               || Read [j] . cam_a > Read [i] . cam_a + mean + 10 * sd)
            {
             sprintf (tag, "Read %d (Utg%lld) has bad mate %d (Utg%lld) at coord %d",
                 i, Read [i] . unitig_id, j, Read [j] . unitig_id,
                 (Read [j] . cam_a + Read [j] . cam_b) / 2);
             Output_Single_Read (i, Bad_Read_Col_Id, Bad_Mate_Col_Id, mean, tag);
                 
             sprintf (tag, "Read %d (Utg%lld) has bad mate %d (Utg%lld) at coord %d",
                 j, Read [j] . unitig_id, i, Read [j] . unitig_id,
                 (Read [i] . cam_a + Read [i] . cam_b) / 2);
             Output_Single_Read (j, Bad_Read_Col_Id, Bad_Mate_Col_Id, mean, tag);
            }
        else if  (Read [j] . cam_a > Read [j] . cam_b)
            {
             diff = Read [j] . cam_a - Read [i] . cam_a;
             sprintf (tag, "Mated reads %d (Utg%lld) and %d (Utg%lld)"
                 "  diff=%d mn=%d sd=%d distort=%+.1fsd",
                 i, Read [i] . unitig_id, j, Read [j] . unitig_id,
                 diff, mean, sd, double (diff - mean) / sd);
             if  (mean - 3 * sd <= diff && diff <= mean + 3 * sd)
                 Output_Pair (i, j, Good_Read_Col_Id, Good_Mate_Col_Id, tag);
               else
                 Output_Pair (i, j, Stretch_Read_Col_Id, Stretch_Mate_Col_Id, tag);
            }
       }
     else
       {
        if  (Read [j] . cam_a > Read [j] . cam_b
               || Read [i] . cam_a < Read [j] . cam_a + 200
               || Read [i] . cam_a > Read [j] . cam_a + mean + 10 * sd)
            {
             sprintf (tag, "Read %d (Utg%lld) has bad mate %d (Utg%lld) at coord %d",
                 i, Read [i] . unitig_id, j, Read [j] . unitig_id,
                 (Read [j] . cam_a + Read [j] . cam_b) / 2);
             Output_Single_Read (i, Bad_Read_Col_Id, Bad_Mate_Col_Id, mean, tag);
                 
             sprintf (tag, "Read %d (Utg%lld) has bad mate %d (Utg%lld) at coord %d",
                 j, Read [j] . unitig_id, i, Read [i] . unitig_id,
                 (Read [i] . cam_a + Read [i] . cam_b) / 2);
             Output_Single_Read (j, Bad_Read_Col_Id, Bad_Mate_Col_Id, mean, tag);
            }
        else if  (Read [j] . cam_a <= Read [j] . cam_b)
            {
             diff = Read [i] . cam_a - Read [j] . cam_a;
             sprintf (tag, "Mated reads %d (Utg%lld) and %d (Utg%lld)"
                 "  diff=%d mn=%d sd=%d distort=%+.1fsd",
                 j, Read [j] . unitig_id, i, Read [i] . unitig_id,
                 diff, mean, sd, double (diff - mean) / sd);
             if  (mean - 3 * sd <= diff && diff <= mean + 3 * sd)
                 Output_Pair (j, i, Good_Read_Col_Id, Good_Mate_Col_Id, tag);
               else
                 Output_Pair (j, i, Stretch_Read_Col_Id, Stretch_Mate_Col_Id, tag);
            }
       }
   return;
  }



static void  Output_Pair
    (int i, int j, const char * read_colour, const char * mate_colour,
     const char * tag)

//  Output a celamy line for mated reads  Read [i]  and  Read [j] ,
//  where  i   is on the left.  Use read_colour  for the read parts and
//   mate_colour  for the gap.  Use  tag  as the celamy comment.

  {
   static int  pair_ct = 0;

   assert (Read [i] . cam_a <= Read [i] . cam_b);
   assert (Read [j] . cam_a > Read [j] . cam_b);
   assert (Read [i] . cam_a < Read [j] . cam_a);

   pair_ct ++;

   if  (Read [j] . cam_b <= Read [i] . cam_b)
       // no gap
       printf ("%dReadPair: %d A%s %d R10 # %s\n", pair_ct, Read [i] . cam_a,
           read_colour, Read [j] . cam_a, tag);
     else
       printf ("%dReadPair: %d A%s %d A%s %d A%s %d R10 # %s\n", pair_ct,
           Read [i] . cam_a, read_colour, Read [i] . cam_b,
           mate_colour, Read [j] . cam_b, read_colour, Read [j] . cam_a,
           tag);

   return;
  }



static void  Output_Single_Read
    (int i, const char * read_colour, const char * mate_colour, int len,
     const char * tag)

//  Output a celamy line for just read  Read [i]  without a mate
//  using  read_colour  for the read part and  mate_colour
//  for where the mate should be,  len  away from the beginning of the
//  read.  Use  tag  as the celamy comment

  {
   static int  single_ct = 0;

   single_ct ++;
   if  (Read [i] . cam_a < Read [i] . cam_b)
       printf ("%dSingRead: %d A%s %d A%s %d R10 # %s\n",
           single_ct, Read [i] . cam_a, read_colour,
           Read [i] . cam_b, mate_colour,
           Read [i] . cam_a + len, tag);
     else
       printf ("%dSingRead: %d A%s %d A%s %d R10 # %s\n",
           single_ct, Read [i] . cam_a - len, mate_colour,
           Read [i] . cam_b, read_colour,
           Read [i] . cam_a, tag);

   return;
  }



static void  Print_Colour_Headers
    (void)

//  Print the celamy descriptions of the colours referred to later

  {
   printf ("%s: %s T3 S # %s\n",
       Unique_Col_Id, Unique_Col_Def, "UniqueUtg");
   printf ("%s: %s T3 S # %s\n",
       Rock_Col_Id, Rock_Col_Def, "RockUtg");
   printf ("%s: %s T3 S # %s\n",
       Surr_Col_Id, Surr_Col_Def, "SurrUtg");
   printf ("%s: %s T3 S # %s\n",
       Degen_Col_Id, Degen_Col_Def, "DegenUtg");
   printf ("%s: %s T2 S # %s\n",
       Degen_Link_Col_Id, Degen_Link_Col_Def, "DegenScaff");
   printf ("%s: %s T2 S # %s\n",
       Scaff_Link_Col_Id, Scaff_Link_Col_Def, "QueryScaff");
   printf ("%s: %s T2 S # %s\n",
       Single_Read_Col_Id, Single_Read_Col_Def, "SingleRead");
   printf ("%s: %s T1 S # %s\n",
       Single_Mate_Col_Id, Single_Mate_Col_Def, "SingleMate");
   printf ("%s: %s T2 S # %s\n",
       Bad_Read_Col_Id, Bad_Read_Col_Def, "BadRead");
   printf ("%s: %s T1 S # %s\n",
       Bad_Mate_Col_Id, Bad_Mate_Col_Def, "BadMate");
   printf ("%s: %s T2 S # %s\n",
       Stretch_Read_Col_Id, Stretch_Read_Col_Def, "StretchRead");
   printf ("%s: %s T1 S # %s\n",
       Stretch_Mate_Col_Id, Stretch_Mate_Col_Def, "StretchMate");
   printf ("%s: %s T2 S # %s\n",
       Good_Read_Col_Id, Good_Read_Col_Def, "GoodRead");
   printf ("%s: %s T1 S # %s\n",
       Good_Mate_Col_Id, Good_Mate_Col_Def, "GoodMate");
   printf ("%s: %s T2 S # %s\n",
       Infer_Read_Col_Id, Infer_Read_Col_Def, "InferRead");
   printf ("%s: %s T1 S # %s\n",
       Infer_Mate_Col_Id, Infer_Mate_Col_Def, "InferMate");

   return;
  }



static int  Search
    (const vector <Contig_Pos_t> & pos, long long int id)

//  Do a binary search of  pos  for entry with  contig_id == id
//  Return subscript of entry if found;  -1 , if not found
//  Assumes  pos  has been sorted into ascending order by  contig_id .

  {
   int  lo, hi, mid;

   lo = 0;
   hi = pos . size () - 1;
   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (id == pos [mid] . contig_id)
          return  mid;
      else if  (id < pos [mid] . contig_id)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   return  -1;
  }



static void  Set_Position
    (int & x, int & y, int a, int b, int c, int d, int len)

//  Set  x  and  y  to the start and end positions, resp., of
//  an object that starts/ends  at  a/b  within an object
//  whose start/end postions are  c/d .   c/d  may have been
//  stretched/shrunk by partial matches to another object, and
//   len  is the natural length that  a/b  refer to.

  {
   double  factor;

   if  (c <= d)
       {
        factor = double (d - c) / len;
        x = int (floor (0.5 + c + a * factor));
        y = int (floor (0.5 + c + b * factor));
       }
     else
       {
        factor = double (c - d) / len;
        x = int (floor (0.5 + c - a * factor));
        y = int (floor (0.5 + c - b * factor));
       }

   return;
  }



static int  Uni_Data_Search
    (const vector <Uni_Data_t> & u, long long int id)

//  Do a binary search of  u  for entries with  unitig_id == id
//  Return subscript of entry if found;  -1 , if not found.
//  Assumes  u  has been sorted into ascending order by  unitig_id .

  {
   int  lo, hi, mid;

   lo = 0;
   hi = u . size () - 1;
   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (id == u [mid] . unitig_id)
          return  mid;
      else if  (id < u [mid] . unitig_id)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   return  -1;
  }



static int  Unitig_Search
    (const vector <Unitig_Info_t> & u, long long int id)

//  Do a binary search of  u  for entries with  unitig_id == id
//  Return subscript of first entry if found;  -1 , if not found.
//  Assumes  u  has been sorted into ascending order by  unitig_id .

  {
   int  lo, hi, mid, n;

   lo = 0;
   n = hi = u . size () - 1;
   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (id == u [mid] . unitig_id)
          {
           while  (mid > 0 && u [mid] . unitig_id == u [mid - 1] . unitig_id)
             mid --;

           return  mid;
          }
      else if  (id < u [mid] . unitig_id)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   return  -1;
  }


