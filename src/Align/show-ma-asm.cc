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
#include  <vector>
#include  <hash_set>
#include  <string>
#include  <algorithm>

using namespace std;


static const int  ALIGN_WIDTH = 60;
  // number of characters to show on each line of multialignment
static const int  INITIAL_READ_BUFF_LEN = 2048;
  // initial length of buffer to hold reads
static const int  MAX_LINE = 1000;
  // maximum length input line
static const int  TAG_WIDTH = 10;
  // number of characters for id tags at beginning of multialignment
  // lines--should be at least 9


struct  Read_Info_t
  {
   int  id;
   int  start, end;
   int  len : 31;
   unsigned  flipped : 1;
   vector <int>  del;
   char  * seq;
  };


static int  Contig_ID;
static char  * ASM_File_Name;
static char  * FRG_File_Name;


static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Print_Alignment_Line
    (FILE * fp, const Read_Info_t & read, int lo, int hi,
     const char * consensus, string & diff_line);
static void  Print_Multialignment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
        const vector <Read_Info_t> & read_list, bool with_diffs = true);
static void  Print_Multialignment_Segment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
        const vector <Read_Info_t> & read_list, bool with_diffs = true);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   FILE  * fp;
   char  line [MAX_LINE];
   hash_set <int>  frg_ids;
   vector <Read_Info_t>  read_list;
   Read_Info_t  read;
   char  * consensus;
   int  consensus_len, consensus_ungapped_len;
   char  * read_buff;
   int  read_buff_len = INITIAL_READ_BUFF_LEN;
   int  contig_iid;
   bool  in_cco = false, in_consensus = false, in_mps = false, in_del = false;
   bool  in_frg = false, in_frgseq = false;
   bool  found_it = false;
   int  frg_sub;
   int  ct, dln, level;
   int  i, n;

   cerr << "Starting on " << __DATE__ << " at " << __TIME__ << endl;

   Verbose = 0;

   Parse_Command_Line (argc, argv);

   printf ("Multialignment for contig %d\n", Contig_ID);
   printf ("Using .asm file %s and .frg file %s\n",
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
      if  (in_cco && level == 0)
          in_cco = false;
      if  (in_cco && strncmp (line, "acc:", 4) == 0)
          {
           int  cid;

           cid = strtol (line + 5, NULL, 10);
           if  (cid == Contig_ID)
               {
                char  * p;

                found_it = true;
                p = strchr (line + 5, ',');
                assert (p != NULL);
                contig_iid = strtol (p + 1, NULL, 10);
               }
          }
      if  (found_it && level == 0)
          break;
      if  (found_it && strncmp (line, "len:", 4) == 0)
          {
           int  len;

           len = strtol (line + 4, NULL, 10);
           consensus = (char *) Safe_malloc (len + 1);
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
   consensus_len = ct;
   ct = 0;
   for  (i = 0;  i < consensus_len;  i ++)
     if  (isalpha (consensus [i]))
         ct ++;
   consensus_ungapped_len = ct;

   read_buff = (char *) Safe_malloc (read_buff_len);

   // Find the read sequence information from the .frg file
   // for all the reads in read_list
   fp = File_Open (FRG_File_Name, "r");
   level = 0;
   found_it = false;
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
               memmove (read_buff, read_buff + clr_start, len);
           read_buff [len] = '\0';
           if  (read_list [frg_sub] . start > read_list [frg_sub] . end)
               {
                int  save;

                Reverse_Complement (read_buff);
                save = read_list [frg_sub] . start;
                read_list [frg_sub] . start = read_list [frg_sub] . end;
                read_list [frg_sub] . end = save;
                read_list [frg_sub] . flipped = TRUE;
               }
           read_list [frg_sub] . seq = strdup (read_buff);
          }
      if  (found_it && strncmp (line, "seq:", 4) == 0)
          {
           in_frgseq = true;
           ct = 0;
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

   printf ("Contig %d (iid %d)  reads = %d  len = %d (%d without gaps)\n\n",
        Contig_ID, contig_iid, n, consensus_len, consensus_ungapped_len);
   Print_Multialignment (stdout, consensus, 0, consensus_len,
        read_list);

   return  0;
  }



static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "h")) != EOF))
     switch  (ch)
       {
        case  'h' :
          errflg = true;
          break;

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

   Contig_ID = strtol (argv [optind ++], NULL, 10);
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
          if  (read . seq [j] != consensus [i])
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
        const vector <Read_Info_t> & read_list, bool with_diffs)

//  Print to  fp  the multialignment of the reads in  read_list  to
//  the portion of the string  consensus  between positions
//   con_lo  and  con_hi  (in gapped coordinates).
//  If  with_diffs  is true, then show under the consensus where
//  there is not complete agreement in the alignment.

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
      Print_Multialignment_Segment (fp, consensus, i, hi, read_list);
     }
     
   return;
  }



static void  Print_Multialignment_Segment
    (FILE * fp, char * consensus, int con_lo, int con_hi,
        const vector <Read_Info_t> & read_list, bool with_diffs)

//  Print to  fp  the single segment of the multialignment of
//  the reads in  read_list  to the portion of the string  consensus
//  between positions  con_lo  and  con_hi  (in gapped coordinates).
//  If  with_diffs  is true, then show under the consensus where
//  there is not complete agreement in the alignment.

  {
   string  diff_line (con_hi - con_lo, ' ');
   int  i, n;

   n = read_list . size ();
   for  (i = 0;  i < n;  i ++)
     if  (read_list [i] . start < con_hi && con_lo < read_list [i] . end)
         Print_Alignment_Line (fp, read_list [i], con_lo, con_hi,
              consensus, diff_line);
   fprintf (fp, "%*s:  ", TAG_WIDTH, "consensus");
   for  (i = con_lo;  i < con_hi;  i ++)
     fputc (consensus [i], fp);
   fprintf (fp, "  %d..%d\n", con_lo, con_hi);
   if  (with_diffs)
       {
        fprintf (fp, "%*s   %s\n", TAG_WIDTH, "", diff_line . c_str ());
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
           "USAGE:  %s  <contig-id> <asm-file> <frg-file>\n"
           "\n"
           "Read Celera-format .asm file and .frg file and\n"
           "print the multialignment for the specified contig\n"
           "\n"
           "Options:\n"
           "  -h    Print this usage message\n"
           "\n",
           command);

   return;
  }



