//  A. L. Delcher
//
//  File:  show-ma-asm.cc
//
//  Last Modified:  20 May 2003
//
//  Read Celera-format .asm and .frg files and display the
//  multialignment for a specified contig


#include  "utility_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"
#include  "align.hh"
#include  <vector>
#include  <set>
#include  <string>
#include  <algorithm>
#include  <cassert>

using namespace std;


#define  RESTRICTED_COUNTS  0


static const char  * VERSION_ID = "1.2";


static const int  ALIGN_WIDTH = 60;
  // number of characters to show on each line of multialignment
static const int  INITIAL_READ_BUFF_LEN = 2048;
  // initial length of buffer to hold reads
static const int  MAX_LINE = 1000;
  // maximum length input line
static const unsigned  SIGNIFICANT_QUAL_THRESHOLD = 30;
  // Minimum value for a quality difference to be significant
static const int  TAG_WIDTH = 10;
  // number of characters for id tags at beginning of multialignment
  // lines--should be at least 9


#if  RESTRICTED_COUNTS
  #define  COUNT_WIDTH  3
  #define  QUAL_WIDTH  16
  static const unsigned  QUAL_LIMIT = (1 << QUAL_WIDTH) - 100;
  // don't add to quality sum if its already this large
#else
  #define  COUNT_WIDTH  8
  #define  QUAL_WIDTH  32
  static const unsigned  QUAL_LIMIT = UINT_MAX - 128;
    // don't add to quality sum if its already this large
#endif

static const unsigned  COUNT_LIMIT = (1 << COUNT_WIDTH) - 1;
  // dont add to quality sum if its already this large


struct  Count_t
  {
   char  majority_ch, alternate_ch;
     // In case of SNP, set  majority_ch  to the consensus
     // character and  alternate_ch  to the other character
   unsigned a_ct : COUNT_WIDTH;
   unsigned c_ct : COUNT_WIDTH;
   unsigned g_ct : COUNT_WIDTH;
   unsigned t_ct : COUNT_WIDTH;
   unsigned other_ct : COUNT_WIDTH;
   unsigned a_qual : QUAL_WIDTH;
   unsigned c_qual : QUAL_WIDTH;
   unsigned g_qual : QUAL_WIDTH;
   unsigned t_qual : QUAL_WIDTH;
   unsigned other_qual : QUAL_WIDTH;
   int  position, plus_gaps;
     //  position  is where this count is in the consensus
     //  plus_gaps  is how many gaps after that postion if
     // the position is in terms of ungapped characters

   Count_t
       ()  // Default constructor
     { Zero (); }

   bool  Check_SNP  (char cons_ch)
     // Check if these counts indicate a SNP, where
     //  cons_ch  is the consensus character.  If not,
     // return  false ; otherwise, set  majority_ch  and
     //  alternate_ch  and return  true .

     {
      unsigned  cons_ct, cons_qual, max_ch, max_ct, max_qual;
      unsigned  total_ct;

      cons_ch = tolower (cons_ch);
      max_ct = 0;
      if  (cons_ch == 'a')
          {
           cons_ct = a_ct;
           cons_qual = a_qual;
          }
      else if  (a_ct > max_ct)
          {
           max_ch = 'a';
           max_ct = a_ct;
           max_qual = a_qual;
          }
      if  (cons_ch == 'c')
          {
           cons_ct = c_ct;
           cons_qual = c_qual;
          }
      else if  (c_ct > max_ct)
          {
           max_ch = 'c';
           max_ct = c_ct;
           max_qual = c_qual;
          }
      if  (cons_ch == 'g')
          {
           cons_ct = g_ct;
           cons_qual = g_qual;
          }
      else if  (g_ct > max_ct)
          {
           max_ch = 'g';
           max_ct = g_ct;
           max_qual = g_qual;
          }
      if  (cons_ch == 't')
          {
           cons_ct = t_ct;
           cons_qual = t_qual;
          }
      else if  (t_ct > max_ct)
          {
           max_ch = 't';
           max_ct = t_ct;
           max_qual = t_qual;
          }
      if  (cons_ch == '-')
          {
           cons_ct = other_ct;
           cons_qual = other_qual;
          }
      else if  (other_ct > max_ct)
          {
           max_ch = '-';
           max_ct = other_ct;
           max_qual = other_qual;
          }
      total_ct = a_ct + c_ct + g_ct + t_ct + other_ct;
      // This is the rule for determining SNPs
      // Can be changed as desired
      if  (cons_ct >= 3 && max_ct >= 3
             || total_ct <= 8 && cons_ct >= 2 && max_ct >= 2)
          {
           majority_ch = cons_ch;
           alternate_ch = max_ch;
           return  true;
          }

      return  false;
     }

   void  Incr
       (char ch, unsigned q)
     {
      switch  (tolower (ch))
        {
         case  'a' :
           if  (a_ct < COUNT_LIMIT)
               a_ct ++;
           if  (a_qual < QUAL_LIMIT)
               a_qual += q;
           break;
         case  'c' :
           if  (c_ct < COUNT_LIMIT)
               c_ct ++;
           if  (c_qual < QUAL_LIMIT)
               c_qual += q;
           break;
         case  'g' :
           if  (g_ct < COUNT_LIMIT)
               g_ct ++;
           if  (g_qual < QUAL_LIMIT)
               g_qual += q;
           break;
         case  't' :
           if  (t_ct < COUNT_LIMIT)
               t_ct ++;
           if  (t_qual < QUAL_LIMIT)
               t_qual += q;
           break;
         default :
           if  (other_ct < COUNT_LIMIT)
               other_ct ++;
           if  (other_qual < QUAL_LIMIT)
               other_qual += q;
           break;
        }
     }
   void  Majority
       (char & ch, unsigned & qual)  const
     {
      unsigned  max, sum;

      max = sum = a_qual;
      ch = 'a';
      if  (c_qual > max)
          {
           max = c_qual;
           ch = 'c';
          }
      sum += c_qual;
      if  (g_qual > max)
          {
           max = g_qual;
           ch = 'g';
          }
      sum += g_qual;
      if  (t_qual > max)
          {
           max = t_qual;
           ch = 't';
          }
      sum += t_qual;
      if  (other_qual > max)
          {
           max = other_qual;
           ch = '-';
          }
      sum += other_qual;

      if  (sum - max <= 20 && 2 * max - sum >= 20)
          qual = 2 * max - sum;
        else
          {
           ch = ' ';
           qual = 0;
          }

      return;
     }
   int  Num_Greater
       (unsigned n)
     {
      int  ct = 0;

      if  (a_ct > n)
          ct ++;
      if  (c_ct > n)
          ct ++;
      if  (g_ct > n)
          ct ++;
      if  (t_ct > n)
          ct ++;
      if  (other_ct > n)
          ct ++;
      return  ct;
     }
   void  Print
       (FILE * fp)  const
     {
      fprintf (fp, " %2u/%-4u %2u/%-4u %2u/%-4u %2u/%-4u %2u/%-4u ",
           a_ct, a_qual, c_ct, c_qual, g_ct, g_qual, t_ct, t_qual,
           other_ct, other_qual);
     }
   void  SNP_Print
       (FILE * fp)  const
     // Print the SNP information in this count
     {
      fprintf (fp, "%8d %+3d  %c/%c ", position, plus_gaps,
           majority_ch, alternate_ch);
      Print (fp);
      fputc ('\n', fp);
      return;
     }

   const char *  SNP_String
       (void)
     {
      static char  buff [10];
      bool  first = true;

      buff [0] = '\0';
      if  (a_ct > 1)
          {
           if  (! first)
               strcat (buff, "/");
           strcat (buff, "a");
           first = false;
          }
      if  (c_ct > 1)
          {
           if  (! first)
               strcat (buff, "/");
           strcat (buff, "c");
           first = false;
          }
      if  (g_ct > 1)
          {
           if  (! first)
               strcat (buff, "/");
           strcat (buff, "g");
           first = false;
          }
      if  (t_ct > 1)
          {
           if  (! first)
               strcat (buff, "/");
           strcat (buff, "t");
           first = false;
          }
      if  (other_ct > 1)
          {
           if  (! first)
               strcat (buff, "/");
           strcat (buff, "-");
           first = false;
          }
      return  buff;
     }
   int  Sum
       (void)  const
     {
      return  a_ct + c_ct + g_ct + t_ct + other_ct;
     }
   void  Zero
       (void)
     {
      a_ct = c_ct = g_ct = t_ct = other_ct
          = a_qual = c_qual = g_qual = t_qual = other_qual = 0;
     }
  };

struct  Read_Info_t
  {
   int  id;
   int  start, end;
   int  len : 31;
   unsigned  flipped : 1;
   vector <int>  del;
   char  * seq, * qual;

   bool  operator <
       (const Read_Info_t & r)  const
     {
      return  (start < r . start);
     }
  };


static string  Desired_ID;
static char  * ASM_File_Name;
static char  * FRG_File_Name;
static FILE  * SNP_fp = NULL;
  // File to which SNP information should be written if
  //  Show_SNPs  is true.
static bool  Consensus_With_Gapped_Coords = false;
  // Determines whether coordinates after consensus line count
  // all characters (if  true ), or just non-gap characters (if  false )
static bool  Only_One = true;
  // If true then do only one contig, otherwise do them all
static bool  Show_SNPs = false;
  // If set true by the -s option, the after each multialignment
  // list the SNP values in it
static bool  Show_Unitig = false;
  // If set true by the -u option, then do alignment for unitig(s)
  // with the specified uid


static void  Clean_Consensus
    (char * consensus, int & gapped_len, int & ungapped_len);
static int  Count_SNPs
    (char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list, int & possible);
static void  Get_Reads
    (const char * file_name, vector <Read_Info_t> & read_list,
     const set <int> & frg_ids);
static void  Incr_Align_Counts
    (Count_t * count, const Read_Info_t & read, int offset);
static void  List_SNPs
    (FILE * fp, char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Print_Alignment_Line
    (FILE * fp, const Read_Info_t & read, int lo, int hi,
     const char * consensus, string & diff_line, vector <Count_t> & count);
static void  Print_Multialignment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list, int & gaps,
     vector <Count_t> & snp_list, bool with_diffs = true);
static void  Print_Multialignment_Segment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list, int & gaps,
     vector <Count_t> & snp_list, bool with_diffs = true);
static void  Print_SNP_List
    (FILE * fp, const vector <Count_t> & snp_list);
bool  Significant_Diff
    (const Count_t & x, const Count_t & y, char & x_ch, char & y_ch,
     unsigned & qual_diff);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   FILE  * fp;
   char  line [MAX_LINE], hdr_line [MAX_LINE];
   set <int>  frg_ids;
   vector <Read_Info_t>  read_list, surro_list;
   Read_Info_t  read;
   Gapped_Multi_Alignment_t  gma;
   vector <Count_t>  snp_list;
   char  * consensus = NULL;
   int  consensus_len, consensus_ungapped_len;
   int  tig_iid;
   bool  in_cco = false, in_consensus = false, in_mps = false, in_del = false;
   bool  in_utg = false;
   bool  found_it = false;
   string  cid;
   char  * p;
   time_t  now;
   int  ct, dln, gaps, level;
   int  i, n;

   now = time (NULL);
   cerr << "Starting on " << ctime (& now) << endl;

   Verbose = 0;

   Parse_Command_Line (argc, argv);

   if  (Only_One)
       fprintf (stderr, "Multialignment for %stig %s\n",
            Show_Unitig ? "uni" : "con", Desired_ID . c_str ());
     else
       fprintf (stderr, "Multialignment for all %stigs\n",
       Show_Unitig ? "uni" : "con");
   fprintf (stderr, "Using .asm file %s and .frg file %s\n",
        ASM_File_Name, FRG_File_Name);

   // Find and extract the CCO message for the specified contig
   fp = File_Open (ASM_File_Name, "r", __FILE__, __LINE__);
   level = 0;
   while  (fgets (line, MAX_LINE, fp) != NULL)
     {
      if  (line [0] == '{')
          level ++;
      else if  (line [0] == '}')
          level --;
      if  (strncmp (line, "{CCO", 4) == 0)
          in_cco = true;
      else if  (strncmp (line, "{UTG", 4) == 0)
          in_utg = true;
      if  (in_cco && level == 0)
          in_cco = false;
      else if  (in_utg && level == 0)
          in_utg = false;
      if  (in_cco && strncmp (line, "acc:", 4) == 0 && ! Show_Unitig)
          {
           p = strchr (line + 5, ',');
           assert (p != NULL);
           cid . assign (line + 5, p - line - 5);
           if  (cid == Desired_ID || ! Only_One)
               {
                found_it = true;
                tig_iid = strtol (p + 1, NULL, 10);
               }
          }
      else if  (in_utg && strncmp (line, "acc:", 4) == 0 && Show_Unitig)
          {
           p = strchr (line + 5, ',');
           assert (p != NULL);
           cid . assign (line + 5, p - line - 5);
           if  (cid == Desired_ID || ! Only_One)
               {
                found_it = true;
                tig_iid = strtol (p + 1, NULL, 10);
               }
          }
      if  (found_it && level == 0)
          {
           int  possible, snps;

           if  (Only_One)
               break;

           Clean_Consensus (consensus, consensus_len, consensus_ungapped_len);
           assert (ct == consensus_len);

           Get_Reads (FRG_File_Name, read_list, frg_ids);
           n = read_list . size ();

#if  0
           snps = Count_SNPs (consensus, 0, consensus_len, read_list, possible);
           printf ("%10s %10d  %8d  %8d %8d  %8d %8d\n",
               cid . c_str (), tig_iid, n, consensus_len, consensus_ungapped_len,
               snps, possible);
#else
           sprintf (hdr_line,
                "%stig %s (iid %d)  reads = %d  len = %d (%d without gaps)",
                Show_Unitig ? "Uni" : "Con", cid . c_str (), tig_iid, n,
                consensus_len, consensus_ungapped_len);
           printf ("\n\n%s\n\n", hdr_line);
           gaps = 0;

           sort (read_list . begin (), read_list . end ());

           snp_list . clear ();
           Print_Multialignment (stdout, consensus, 0, consensus_len,
                read_list, gaps, snp_list);
           if  (Show_SNPs)
               {
                Print_SNP_List (stdout, snp_list);

                fprintf (SNP_fp, "\n%s\n", hdr_line);
                Print_SNP_List (SNP_fp, snp_list);

//                List_SNPs (stdout, consensus, 0, consensus_len, read_list);
               }
#endif

           found_it = false;
           n = read_list . size ();
           for  (i = 0;  i < n;  i ++)
             {
              free (read_list [i] . seq);
              free (read_list [i] . qual);
             }
           read_list . clear ();
           frg_ids . clear ();
           ct = 0;
          }
      if  (found_it && strncmp (line, "len:", 4) == 0)
          {
           int  len;

           len = strtol (line + 4, NULL, 10);
           consensus = (char *) AMOS::SafeRealloc (consensus, len + 1);
           consensus [0] = '\n';
           ct = 0;     // length of consensus
          }
      if  (found_it && strncmp (line, "cns:", 4) == 0)
          in_consensus = true;
      else if  (in_consensus)
          {
           char  * s;

           s = strtok (line, " \t\n");
           if  (strcmp (s, ".") == 0)
               in_consensus = false;
             else
               {
                strcpy (consensus + ct, s);
                ct += strlen (s);
               }
          }
      if  (found_it && strncmp (line, "{MPS", 4) == 0)
          in_mps = true;
      else if  (in_mps && line [0] == '}')
          {
           in_mps = in_del = false;
           read . flipped = FALSE;
           read_list . push_back (read);
           if  (Verbose > 1)
               {
                cerr << "push read  list len = " << read_list . size ()
                     << "  level = " << level << endl;
                cerr << "start = " << read . start << "  end = " << read . end
                     << endl;
                cerr << "inserting id " << read . id << endl;
               }
           frg_ids . insert (read . id);
           read . del . clear ();
          }
      if  (in_mps && strncmp (line, "mid:", 4) == 0)
          read . id = strtol (line + 4, NULL, 10);
      if  (in_mps && strncmp (line, "pos:", 4) == 0)
          sscanf (line + 4, "%d,%d", & read . start, & read . end);
      if  (in_mps && strncmp (line, "dln:", 4) == 0)
          dln = strtol (line + 4, NULL, 10);
      if  (in_mps && strncmp (line, "del:", 4) == 0)
          in_del = true;
      else if  (in_del)
          {
           char  * p;

           for  (p = strtok (line, " \t\n");  p != NULL;  p = strtok (NULL, " \t\n"))
             read . del . push_back (strtol (p, NULL, 10));
          }
     }

   fclose (fp);

   if  (Only_One)
       {
        if  (! found_it)
            {
             fprintf (stderr, "ERROR:  Didn't find %stig %s\n",
                  Show_Unitig ? "Uni" : "Con", Desired_ID . c_str ());
             exit (EXIT_FAILURE);
            }
        Clean_Consensus (consensus, consensus_len, consensus_ungapped_len);
        assert (ct == consensus_len);

        Get_Reads (FRG_File_Name, read_list, frg_ids);

        n = read_list . size ();
        if  (Verbose > 0)
            {
             fprintf (stderr, "Num reads = %d\n", n);
             fprintf (stderr, "Consensus:\n");
             Fasta_Print (stderr, consensus, NULL);
             for  (i = 0;  i < n;  i ++)
               {
                int  j, m;

                fprintf (stderr, "Read=%d  id=%d  start=%d  end=%d  :\n",
                     i, read_list [i] . id, read_list [i] . start,
                     read_list [i] . end);
                fprintf (stderr, "dln: ");
                m = read_list [i] . del . size ();
                for  (j = 0;  j < m;  j ++)
                  fprintf (stderr, " %3d", read_list [i] . del [j]);
                fputc ('\n', stderr);
                Fasta_Print (stderr, read_list [i] . seq, NULL);
               }
            }

        printf ("%stig %s (iid %d)  reads = %d  len = %d (%d without gaps)\n\n",
             Show_Unitig ? "Uni" : "Con", Desired_ID . c_str (), tig_iid, n,
             consensus_len, consensus_ungapped_len);
        gaps = 0;

        sort (read_list . begin (), read_list . end ());

        Print_Multialignment (stdout, consensus, 0, consensus_len,
             read_list, gaps, snp_list);
        if  (Show_SNPs)
            {
             Print_SNP_List (stdout, snp_list);

             fprintf (SNP_fp, "\n%s\n", hdr_line);
             Print_SNP_List (SNP_fp, snp_list);

//                List_SNPs (stdout, consensus, 0, consensus_len, read_list);
            }
       }

   if  (Show_SNPs)
       fclose (SNP_fp);

   return  0;
  }



static void  Clean_Consensus
    (char * consensus, int & gapped_len, int & ungapped_len)

//  Set  gapped_len  to length of string  consensus .  Set
//   ungapped_len  to the number of non-hyphens in  consensus .
//  Convert  consensus  to lowercase.

  {
   int  i, ct;

   gapped_len = strlen (consensus);
   ct = 0;
   for  (i = 0;  i < gapped_len;  i ++)
     if  (isalpha (consensus [i]))
         {
          consensus [i] = tolower (consensus [i]);
          ct ++;
         }
   ungapped_len = ct;

   return;
  }



static int  Count_SNPs
    (char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list, int & possible)

//  Return the number of potential SNPs in  consensus
//  in region  con_lo .. con_hi .  Use the reads in
//  read_list .  Set  possible  to the number of positions
//  that had enough coverage to possibly meet the conditions
//  for a SNP.

  {
   static Count_t  * count = NULL;
   static int  count_len;
   int  cons_len, ct;
   int  i, n;

   cons_len = con_hi - con_lo;
   if  (count == NULL)
       {
        count = (Count_t *) AMOS::SafeCalloc (cons_len, sizeof (Count_t));
        count_len = cons_len;
       }
   else
       {
        if  (cons_len > count_len)
            {
             count = (Count_t *) AMOS::SafeRealloc (count, cons_len * sizeof (Count_t));
             count_len = cons_len;
            }
        for  (i = 0;  i < cons_len;  i ++)
          count [i] . Zero ();
       }

   n = read_list . size ();
   for  (i = 0;  i < n;  i ++)
     Incr_Align_Counts (count, read_list [i], con_lo);

   possible = 0;
   for  (i = con_lo;  i < con_hi;  i ++)
     if  (count [i] . Sum () >= 4)
         possible ++;

   ct = 0;
   for  (i = con_lo;  i < con_hi;  i ++)
     if  (count [i] . Num_Greater (2) >= 2)
         ct ++;

   return  ct;
  }



static void  Get_Reads
    (const char * file_name, vector <Read_Info_t> & read_list,
     const set <int> & frg_ids)

//  Open and read sequence information from .frg file  file_name
//  and put the relevant information into  read_list .
//   frg_ids  contains the ids of the reads to get.

  {
   static char *  read_buff = NULL;
   static int  read_buff_len = INITIAL_READ_BUFF_LEN;
   static char *  qual_buff = NULL;
   static int  qual_buff_len = INITIAL_READ_BUFF_LEN;
   FILE  * fp;
   char  line [MAX_LINE];
   int  level;
   int  frg_sub;
   bool  found_it, in_frg, in_frgseq, in_qualseq;
   int  i, n, ct, q_ct;

   if  (read_buff == NULL)
       read_buff = (char *) AMOS::SafeMalloc (read_buff_len);
   if  (qual_buff == NULL)
       qual_buff = (char *) AMOS::SafeMalloc (qual_buff_len);

   // Find the read sequence information from the .frg file
   // for all the reads in read_list
   fp = File_Open (FRG_File_Name, "r", __FILE__, __LINE__);
   level = 0;
   found_it = in_frg = in_frgseq = in_qualseq = false;
   while  (fgets (line, MAX_LINE, fp) != NULL)
     {
      if  (line [0] == '{')
          level ++;
      else if  (line [0] == '}')
          level --;
      if  (level == 1 && strncmp (line, "{FRG", 4) == 0)
          in_frg = true;
      if  (in_frg && level == 0)
          in_frg = found_it = false;
      if  (found_it && strncmp (line, "clr:", 4) == 0)
          {
           int  clr_start, clr_end, len;

           sscanf (line + 4, "%d,%d", & clr_start, & clr_end);
           read_list [frg_sub] . len = len = clr_end - clr_start;
           if  (clr_start > 0)
               {
                memmove (read_buff, read_buff + clr_start, len);
                memmove (qual_buff, qual_buff + clr_start, len);
               }
           read_buff [len] = '\0';
           qual_buff [len] = '\0';
           if  (read_list [frg_sub] . start > read_list [frg_sub] . end)
               {
                int  save;

                Reverse_Complement (read_buff);
                Reverse_String (qual_buff);
                save = read_list [frg_sub] . start;
                read_list [frg_sub] . start = read_list [frg_sub] . end;
                read_list [frg_sub] . end = save;
                read_list [frg_sub] . flipped = TRUE;
               }
           read_list [frg_sub] . seq = strdup (read_buff);
           read_list [frg_sub] . qual = strdup (qual_buff);
           for  (i = 0;  i < len;  i ++)
             read_list [frg_sub] . qual [i] -= '0';
          }
      if  (found_it && strncmp (line, "seq:", 4) == 0)
          {
           in_frgseq = true;
           ct = 0;
          }
      else if  (found_it && strncmp (line, "qlt:", 4) == 0)
          {
           in_qualseq = true;
           q_ct = 0;
          }
      else if  (in_frgseq)
          {
           char  * s;

           s = strtok (line, " \t\n");
           if  (strcmp (s, ".") == 0)
               in_frgseq = false;
             else
               {
                int  add;

                add = strlen (s);
                if  (ct + add >= read_buff_len)
                    {
                     read_buff_len *= 2;
                     read_buff
                          = (char *) AMOS::SafeRealloc (read_buff, read_buff_len);
                    }
                strcpy (read_buff + ct, s);
                ct += strlen (s);
               }
          }
      else if  (in_qualseq)
          {
           char  * s;

           s = strtok (line, " \t\n");
           if  (strcmp (s, ".") == 0)
               in_qualseq = false;
             else
               {
                int  add;

                add = strlen (s);
                if  (q_ct + add >= qual_buff_len)
                    {
                     qual_buff_len *= 2;
                     qual_buff
                          = (char *) AMOS::SafeRealloc (qual_buff, qual_buff_len);
                    }
                strcpy (qual_buff + q_ct, s);
                q_ct += strlen (s);
               }
          }
      if  (in_frg && strncmp (line, "acc:", 4) == 0)
          {
           int  fid;

           fid = strtol (line + 4, NULL, 10);
           if  (frg_ids . find (fid) != frg_ids . end ())
               {
                if  (Verbose > 1)
                    cerr << "*** Found " << fid << endl;
                found_it = true;
                n = read_list . size ();
                for  (i = 0;  i < n;  i ++)
                  if  (read_list [i] . id == fid)
                      {
                       frg_sub = i;
                       break;
                      }
                if  (i == n)
                    {
                     cerr << "ERROR:  Didn't find read id " << fid << endl;
                     exit (EXIT_FAILURE);
                    }
               }
          }
     }

   fclose (fp);

   return;
  }



static void  Incr_Align_Counts
    (Count_t * count, const Read_Info_t & read, int offset)

//  Increment positions in  count  for each position in  read  that
//  aligns to them.   offset  is the consensus position corresponding
//  to the first entry in  count .

  {
   int  i, j, k, m;

   assert (offset <= read . start);

   m = read . del . size ();
   j = 0;  // position in the read
   k = 0;  // del entry subscript

   for  (i = read . start;  i < read . end;  i ++)
     if  (k < m && j == read . del [k])
         {
          count [i - offset] . Incr ('-',
               Min (read . qual [j - 1], read . qual [j]));
          k ++;
         }
       else
         {
          count [i - offset] . Incr (read . seq [j], read . qual [j]);
          j ++;
         }

   return;
  }



static void  List_SNPs
    (FILE * fp, char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list)

//  Print to  fp  positions in  consensus [con_lo .. con_hi]
//  that have >=2 reads for each of 2 or more different values
//  at a column.  Use reads in  read_list.

  {
   static Count_t  * count_a = NULL, * count_b = NULL;
   static int  count_len;
   int  cons_len, ct, possible;
   int  i, n;

   cons_len = con_hi - con_lo;
   if  (count_a == NULL)
       {
        count_a = (Count_t *) AMOS::SafeCalloc (cons_len, sizeof (Count_t));
        count_b = (Count_t *) AMOS::SafeCalloc (cons_len, sizeof (Count_t));
        count_len = cons_len;
       }
   else
       {
        if  (cons_len > count_len)
            {
             count_a = (Count_t *) AMOS::SafeRealloc (count_a, cons_len * sizeof (Count_t));
             count_b = (Count_t *) AMOS::SafeRealloc (count_b, cons_len * sizeof (Count_t));
             count_len = cons_len;
            }
        for  (i = 0;  i < cons_len;  i ++)
          {
           count_a [i] . Zero ();
           count_b [i] . Zero ();
          }
       }

   n = read_list . size ();
   for  (i = 0;  i < n;  i ++)
     {
      if  (read_list [i] . id < 100000)
          Incr_Align_Counts (count_a, read_list [i], con_lo);
        else
          Incr_Align_Counts (count_b, read_list [i], con_lo);
     }

   possible = 0;
   for  (i = con_lo;  i < con_hi;  i ++)
     if  (count_a [i] . Sum () > 0 && count_b [i] . Sum () > 0)
         possible ++;

   ct = 0;
   for  (i = con_lo;  i < con_hi;  i ++)
     {
      char  a_ch, b_ch;
      unsigned  qual_diff;
      
//      if  (count_a [i] . Num_Greater (2) >= 2)
      if  (Significant_Diff (count_a [i], count_b [i], a_ch, b_ch, qual_diff))
          {
//           char  buff [10];

           fprintf (fp, "%7d  %c/%c  %3d  ", i + 1, a_ch, b_ch, qual_diff);
           count_a [i] . Print (fp);
           fprintf (fp, "  ");
           count_b [i] . Print (fp);
//           strcpy (buff, count_a [i] . SNP_String ());
//           fprintf (fp, "  %s\n", buff);
           fputc ('\n', fp);
           ct ++;
          }
     }

   fprintf (fp, "%d SNPs out of %d possible positions\n", ct, possible);

   return;
  }



static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "ghs:uV")) != EOF))
     switch  (ch)
       {
        case  'g' :
          Consensus_With_Gapped_Coords = true;
          break;

        case  'h' :
          Usage (argv [0]);
          exit (EXIT_SUCCESS);

        case  's' :
          Show_SNPs = true;
          SNP_fp = File_Open (optarg, "w", __FILE__, __LINE__);
          break;

        case  'u' :
          Show_Unitig = true;
          break;

        case  'V' :
          fprintf (stderr, "Version = %s\n", VERSION_ID);
          exit (EXIT_SUCCESS);

        case  '?' :
          fprintf (stderr, "Unrecognized option -%c\n", optopt);

        default :
          errflg = true;
       }

   if  (errflg)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   if  (optind > argc - 3)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   if  (strcmp (argv [optind], "ALL") == 0)
       {
        Only_One = false;
        Desired_ID = "BAD";
        optind ++;
       }
     else
       Desired_ID = argv [optind ++];
   ASM_File_Name = argv [optind ++];
   FRG_File_Name = argv [optind ++];

   return;
  }



static void  Print_Alignment_Line
    (FILE * fp, const Read_Info_t & read, int lo, int hi,
     const char * consensus, string & diff_line, vector <Count_t> & count)

//  Print to  fp  the single-line portion of the alignment in
//   read  to the portion of the consensus string between
//   lo  and  hi  (in gapped coordinates).
//   consensus  is the consensus string.  Put a '^' in each
//  position of  diff_line  that the alignment line disagrees with
//  the consensus.  Set counts and quality sums in  count
//  for the corresponding entries.

  {
   int  read_lo, read_hi;
   int  i, j, k, m;

   if  (read . end <= lo || hi <= read . start)
       return;  // there's nothing here


   read_lo = read_hi = 0;

   // Print the id tag of this read at the beginning of the line
   fprintf (fp, "%*d:  ", TAG_WIDTH, read . id);

   m = read . del . size ();
   j = 0;  // position in the read
   k = 0;  // del entry subscript

   if  (lo < read . start)
       {  // put blanks to get to the start of this read
        for  (i = lo;  i < read . start;  i ++)
          fputc (' ', fp);
       }
     else
       {  // skip the start of this read before where this line begins
        for  (i = read . start;  i < lo;  i ++)
          if  (k < m && j == read . del [k])
              k ++;
            else
              j ++;
       }
   read_lo = j;

   // print actual alignment
   for  ( ;  i < read . end && i < hi;  i ++)
     if  (k < m && j == read . del [k])
         {
          unsigned  nbr_qual;

          fputc ('-', fp);
          nbr_qual = read . qual [j];
          if  (j > 0 && unsigned (read . qual [j - 1]) < nbr_qual)
              nbr_qual = read . qual [j - 1];
          // Use smaller of the two bounding qualities as the quality
          // for the gap
          count [i - lo] . Incr ('-', nbr_qual);
          diff_line [i - lo] = '^';
          k ++;
         }
       else
         {
          fputc (read . seq [j], fp);
          count [i - lo] . Incr (read . seq [j], read . qual [j]);
          if  (tolower (read . seq [j]) != tolower (consensus [i]))
              diff_line [i - lo] = '^';
          j ++;
         }
   read_hi = j;

   // print blanks at end if necessary
   for  ( ;  i < hi;  i ++)
     fputc (' ', fp);

   if  (read . flipped)
       {
        read_lo = read . len - read_lo;
        read_hi = read . len - read_hi;
       }
   fprintf (fp, "  %d..%d\n", read_lo, read_hi);

   return;
  }



static void  Print_Multialignment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
        const vector <Read_Info_t> & read_list, int & gaps,
        vector <Count_t> & snp_list, bool with_diffs)

//  Print to  fp  the multialignment of the reads in  read_list  to
//  the portion of the string  consensus  between positions
//   con_lo  and  con_hi  (in gapped coordinates).
//  If  with_diffs  is true, then show under the consensus where
//  there is not complete agreement in the alignment.   gaps  is
//  the number of gaps that have been in  consensus  before  con_lo
//  so that consensus coordinates without gaps can be printed.
//   gaps  is updated at the end.

  {
   int  i;

   for  (i = con_lo;  i < con_hi;  i += ALIGN_WIDTH)
     {
      int  hi;

      if  (i > con_lo)
          fputc ('\n', fp);
      hi = i + ALIGN_WIDTH;
      if  (hi > con_hi)
          hi = con_hi;
      Print_Multialignment_Segment (fp, consensus, i, hi, read_list, gaps,
           snp_list, with_diffs);
     }
     
   return;
  }



static void  Print_Multialignment_Segment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
        const vector <Read_Info_t> & read_list, int & gaps,
        vector <Count_t> & snp_list, bool with_diffs)

//  Print to  fp  the single segment of the multialignment of
//  the reads in  read_list  to the portion of the string  consensus
//  between positions  con_lo  and  con_hi  (in gapped coordinates).
//  If  with_diffs  is true, then show under the consensus where
//  there is not complete agreement in the alignment.   gaps  is
//  the number of gaps that have been in  consensus  before  con_lo
//  so that consensus coordinates without gaps can be printed.
//   gaps  is updated at the end.   snp_list  is where the
//  SNP locations are saved if  Show_SNPs  is true.

  {
   string  diff_line (con_hi - con_lo, ' ');
   vector <Count_t>  count (con_hi - con_lo);
   int  new_gaps, consecutive_gaps;
   int  i, j, n;

   n = read_list . size ();
   for  (i = 0;  i < n;  i ++)
     if  (read_list [i] . start < con_hi && con_lo < read_list [i] . end)
         Print_Alignment_Line (fp, read_list [i], con_lo, con_hi,
              consensus, diff_line, count);
   fprintf (fp, "%*s:  ", TAG_WIDTH, "consensus");
   j = new_gaps = consecutive_gaps = 0;
   for  (i = con_lo;  i < con_hi;  i ++, j ++)
     {
      fputc (consensus [i], fp);
      if  (consensus [i] == '-')
          {
           new_gaps ++;
           consecutive_gaps ++;
          }
        else
          consecutive_gaps = 0;

      if  (Show_SNPs && count [j] . Check_SNP (consensus [i]))
          {
           diff_line [j] = '#';
           count [j] . position = i - gaps - new_gaps;
           count [j] . plus_gaps = consecutive_gaps;
           snp_list . push_back (count [j]);
          }
     }
   
   if  (Consensus_With_Gapped_Coords)
       fprintf (fp, "  %d..%d\n", con_lo, con_hi);
     else
       fprintf (fp, "  %d..%d\n", con_lo - gaps, con_hi - gaps - new_gaps);
   gaps += new_gaps;
   if  (with_diffs)
       {
        fprintf (fp, "%*s   %s\n", TAG_WIDTH, "", diff_line . c_str ());
       }
     
   return;
  }



static void  Print_SNP_List
    (FILE * fp, const vector <Count_t> & snp_list)

//  Print the SNPs in  snp_list  to  fp .

  {
   int  i, n;

   n = snp_list . size ();

   fprintf (fp, "SNPs:\n");
   for  (i = 0;  i < n;  i ++)
     snp_list [i] . SNP_Print (fp);
   fprintf (fp, "Total SNPs = %d\n", n);

   return;
  }



bool  Significant_Diff
    (const Count_t & x, const Count_t & y, char & x_ch, char & y_ch,
     unsigned & qual_diff)

//  Return  true  iff counts  x  and  y  are different enough to
//  indicate a SNP

  {
   unsigned  x_qual, y_qual;

   x . Majority (x_ch, x_qual);
   y . Majority (y_ch, y_qual);

   qual_diff = Min (x_qual, y_qual);

   return  (x_ch != y_ch && x_ch != ' ' && y_ch != ' '
        && qual_diff > SIGNIFICANT_QUAL_THRESHOLD);
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s [options] <uid> <asm-file> <frg-file>\n"
           "\n"
           "Read Celera-format .asm file and .frg file and\n"
           "print the multialignment for the specified contig/unitig\n"
           "If <uid> is ALL, then print alignments of all contigs/unitigs\n"
           "\n"
           "Options:\n"
           "  -g    Count gaps in printed consensus positions\n"
           "  -h    Print this usage message\n"
           "  -s    Print SNPs in addition to multialignments\n"
           "  -u    Print alignments for unitig(s) with specfied uid(s)\n"
           "  -V    Display code version\n"
           "\n",
           command);

   return;
  }



