//  A. L. Delcher
//
//  File:  show-ma-asm.cc
//
//  Last Modified:  20 May 2003
//
//  Read Celera-format .asm and .frg files and display the
//  multialignment for a specified contig


#include  "delcher.hh"
#include  "fasta.hh"
#include  "align.hh"
#include  <vector>
#include  <hash_set>
#include  <string>
#include  <algorithm>

using namespace std;



static const char  * VERSION_ID = "1.2";


static const int  ALIGN_WIDTH = 60;
  // number of characters to show on each line of multialignment
static const int  INITIAL_READ_BUFF_LEN = 2048;
  // initial length of buffer to hold reads
static const int  MAX_LINE = 1000;
  // maximum length input line
static const unsigned  QUAL_LIMIT = (1 << 16) - 1000;
  // dont add to quality sum if its already this large
static const unsigned  SIGNIFICANT_QUAL_THRESHOLD = 30;
  // Minimum value for a quality difference to be significant
static const int  TAG_WIDTH = 10;
  // number of characters for id tags at beginning of multialignment
  // lines--should be at least 9


struct  Count_t
  {
   unsigned a_ct : 3;
   unsigned c_ct : 3;
   unsigned g_ct : 3;
   unsigned t_ct : 3;
   unsigned other_ct : 3;
   unsigned a_qual : 16;
   unsigned c_qual : 16;
   unsigned g_qual : 16;
   unsigned t_qual : 16;
   unsigned other_qual : 16;

   void  Incr
       (char ch, unsigned q)
     {
      switch  (tolower (ch))
        {
         case  'a' :
           if  (a_ct < 7)
               a_ct ++;
           if  (a_qual < QUAL_LIMIT)
               a_qual += q;
           break;
         case  'c' :
           if  (c_ct < 7)
               c_ct ++;
           if  (c_qual < QUAL_LIMIT)
               c_qual += q;
           break;
         case  'g' :
           if  (g_ct < 7)
               g_ct ++;
           if  (g_qual < QUAL_LIMIT)
               g_qual += q;
           break;
         case  't' :
           if  (t_ct < 7)
               t_ct ++;
           if  (t_qual < QUAL_LIMIT)
               t_qual += q;
           break;
         default :
           if  (other_ct < 7)
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
       (FILE * fp)
     {
      fprintf (fp, " %u/%-3u %u/%-3u %u/%-3u %u/%-3u %u/%-3u ",
           a_ct, a_qual, c_ct, c_qual, g_ct, g_qual, t_ct, t_qual,
           other_ct, other_qual);
     }
   const char *  SNP_String
       (void)
     {
      char  buff [10] = "";
      bool  first = true;
      
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
     const hash_set <int> & frg_ids);
static void  Incr_Align_Counts
    (Count_t * count, const Read_Info_t & read, int offset);
static void  List_SNPs
    (FILE * fp, char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Print_Alignment_Line
    (FILE * fp, const Read_Info_t & read, int lo, int hi,
     const char * consensus, string & diff_line);
static void  Print_Multialignment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list, int & gaps,
     bool with_diffs = true);
static void  Print_Multialignment_Segment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
     const vector <Read_Info_t> & read_list, int & gaps,
     bool with_diffs = true);
bool  Significant_Diff
    (const Count_t & x, const Count_t & y, char & x_ch, char & y_ch,
     unsigned & qual_diff);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   FILE  * fp;
   char  line [MAX_LINE];
   hash_set <int>  frg_ids;
   vector <Read_Info_t>  read_list, surro_list;
   Read_Info_t  read;
   Gapped_Multi_Alignment_t  gma;
   char  * consensus = NULL;
   int  consensus_len, consensus_ungapped_len;
   int  tig_iid;
   bool  in_cco = false, in_consensus = false, in_mps = false, in_del = false;
   bool  in_utg = false;
   bool  found_it = false;
   string  cid;
   char  * p;
   int  ct, dln, gaps, level;
   int  i, n;

   cerr << "Starting on " << __DATE__ << " at " << __TIME__ << endl;

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
   fp = File_Open (ASM_File_Name, "r");
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

           snps = Count_SNPs (consensus, 0, consensus_len, read_list, possible);
#if  0
           printf ("%10s %10d  %8d  %8d %8d  %8d %8d\n",
               cid . c_str (), tig_iid, n, consensus_len, consensus_ungapped_len,
               snps, possible);
#else
           printf ("\n\n%stig %s (iid %d)  reads = %d  len = %d (%d without gaps)\n\n",
                Show_Unitig ? "Uni" : "Con", cid . c_str (), tig_iid, n,
                consensus_len, consensus_ungapped_len);
           gaps = 0;

           sort (read_list . begin (), read_list . end ());

           Print_Multialignment (stdout, consensus, 0, consensus_len,
                read_list, gaps);
           List_SNPs (stdout, consensus, 0, consensus_len, read_list);
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
           consensus = (char *) Safe_realloc (consensus, len + 1);
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
             read_list, gaps);
        List_SNPs (stdout, consensus, 0, consensus_len, read_list);
       }

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
        count = (Count_t *) Safe_calloc (cons_len, sizeof (Count_t));
        count_len = cons_len;
       }
   else
       {
        if  (cons_len > count_len)
            {
             count = (Count_t *) Safe_realloc (count, cons_len * sizeof (Count_t));
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
     const hash_set <int> & frg_ids)

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
       read_buff = (char *) Safe_malloc (read_buff_len);
   if  (qual_buff == NULL)
       qual_buff = (char *) Safe_malloc (qual_buff_len);

   // Find the read sequence information from the .frg file
   // for all the reads in read_list
   fp = File_Open (FRG_File_Name, "r");
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
                          = (char *) Safe_realloc (read_buff, read_buff_len);
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
                          = (char *) Safe_realloc (qual_buff, qual_buff_len);
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
        count_a = (Count_t *) Safe_calloc (cons_len, sizeof (Count_t));
        count_b = (Count_t *) Safe_calloc (cons_len, sizeof (Count_t));
        count_len = cons_len;
       }
   else
       {
        if  (cons_len > count_len)
            {
             count_a = (Count_t *) Safe_realloc (count_a, cons_len * sizeof (Count_t));
             count_b = (Count_t *) Safe_realloc (count_b, cons_len * sizeof (Count_t));
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

   while  (! errflg && ((ch = getopt (argc, argv, "ghsuV")) != EOF))
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
     const char * consensus, string & diff_line)

//  Print to  fp  the single-line portion of the alignment in
//   read  to the portion of the consensus string between
//   lo  and  hi  (in gapped coordinates).
//   consensus  is the consensus string.  Put a '^' in each
//  position of  diff_line  that the alignment line disagrees with
//  the consensus.

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
          fputc ('-', fp);
          diff_line [i - lo] = '^';
          k ++;
         }
       else
         {
          fputc (read . seq [j], fp);
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
        bool with_diffs)

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
           with_diffs);
     }
     
   return;
  }



static void  Print_Multialignment_Segment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
        const vector <Read_Info_t> & read_list, int & gaps,
        bool with_diffs)

//  Print to  fp  the single segment of the multialignment of
//  the reads in  read_list  to the portion of the string  consensus
//  between positions  con_lo  and  con_hi  (in gapped coordinates).
//  If  with_diffs  is true, then show under the consensus where
//  there is not complete agreement in the alignment.   gaps  is
//  the number of gaps that have been in  consensus  before  con_lo
//  so that consensus coordinates without gaps can be printed.
//   gaps  is updated at the end.

  {
   string  diff_line (con_hi - con_lo, ' ');
   int  new_gaps;
   int  i, n;

   n = read_list . size ();
   for  (i = 0;  i < n;  i ++)
     if  (read_list [i] . start < con_hi && con_lo < read_list [i] . end)
         Print_Alignment_Line (fp, read_list [i], con_lo, con_hi,
              consensus, diff_line);
   fprintf (fp, "%*s:  ", TAG_WIDTH, "consensus");
   new_gaps = 0;
   for  (i = con_lo;  i < con_hi;  i ++)
     {
      fputc (consensus [i], fp);
      if  (consensus [i] == '-')
          new_gaps ++;
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



