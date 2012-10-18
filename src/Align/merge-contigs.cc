//  A. L. Delcher
//
//  File:  merge-contigs.cc
//
//  Last Modified:  15 August 2003
//
//  Read Celera-format contigs and merge their multialignments


#include  "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "CelMsg.hh"
#include  "align.hh"
#include  "prob.hh"
#include  "fasta.hh"
#include  <vector>
#include  <string>
#include  <algorithm>


using namespace std;
using namespace AMOS;


const int  END_FUDGE_BASES = 5;
  // If match is within this many bases of the end, regard it as
  // matching to the end
const int  MAX_LINE = 1000;
const int  NEW_SIZE = 1000;


enum  Input_Format_t
     {SIMPLE_CONTIG_FORMAT, CELERA_MSG_FORMAT};

struct  ID_Pair_t
  {
   int  uid, iid;
  };

struct  Coord_Line_t
  {
   int  r_lo, r_hi;
   int  q_start, q_end;
   int  r_match_len, q_match_len;
   double  percent_id;
   int  r_len, q_len;
   double  r_percent_matched, q_percent_matched;
   int  r_id, q_id;
  };

// The following describes how a contig (or part of it) maps to the
// consensus of the merged contig begin built

struct  Contig_Map_Entry_t
  {
   int  id;
   int  len;
   int  start, end;
        // in ungapped consensus of merged contig
  };

struct  Unitig_Ref_t
  {
   int  uid;
   char  * seq;
  };



static string  Bank_Name;
  // Name of read bank from which reads are obtained
static string  CCO_File_Name;
  // Name of file containing input contig/unitig messages
static string  Coords_File_Name;
  // Name of file containing NUCmer coords info
static string  Map_File_Name;
  // Name of file containing uid-iid map
static bool  Output_Multialignment = false;
  // If set true by  -a  option then output the .mali file
static string  Output_Prefix;
  // Prefix used for output file names
static string  Unitig_File_Name;
  // Name of fasta file surrogate unitig sequences


static bool  Range_Precedes
    (const Ordered_Range_t & a, const Ordered_Range_t & b)
  {
   int  a_start, a_end, b_start, b_end;

   a_start = a . getBegin ();
   a_end = a . getEnd ();
   b_start = b . getBegin ();
   b_end = b . getEnd ();

   if  (a_end < a_start)
       Swap (a_start, a_end);
   if  (b_end < b_start)
       Swap (b_start, b_end);

   if  (a_start < b_start)
       return  true;
   if  (a_start == b_start && a_end < b_end)
       return  true;

   return  false;
  }


static void  Copy
    (vector <char *> & S, const vector <char *> & T);
static int  Find_Map_Entry
    (int id, const vector <Contig_Map_Entry_t> & map);
static int  Find_Unitig
    (int uid, const vector <Unitig_Ref_t> & uni_list);
static void  Get_Strings_And_Lengths
    (vector <char *> & s, vector <char *> & q, vector <int> & str_len,
     vector <char *> & tag, const Celera_Message_t & msg,
     Bank_t & read_bank, const vector <ID_Pair_t> & map,
     const vector <Unitig_Ref_t> & uni_list);
static void  Load_ID_Map
    (vector <ID_Pair_t> & map, const char * fn);
static void  Load_Unitigs
    (vector <Unitig_Ref_t> & uni_list, const char * fn);
static int  Lookup
    (int uid, const vector <ID_Pair_t> & map);
static void  Output_Variant
     (FILE * fp, int ref_id, int q_id, bool q_flipped,
      int r_lo, int r_hi, int q_lo, int q_hi,
      const Gapped_Multi_Alignment_t & gma);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   Bank_t  read_bank (Read_t::NCODE);
   Celera_Message_t  msg;
   Read_t  read;
   FILE  * cco_fp, * coords_fp;
   FILE  * rpos_fp, * mali_fp, * ref_fp, * vary_fp;
   vector <char *>  string_list, qual_list, tag;
   vector <char *>  ref_sl, sl2;
   vector <char *>  ref_tag, tg2;
   vector <int>  sl2_place;
   vector <int>  offset;
   vector <int>  ref, str_len;
   vector <Range_t>  pos;
   vector < vector <int> >  del_list;
   Gapped_Multi_Alignment_t  ref_gma, gma2;
   vector <ID_Pair_t>  map;
   vector <Unitig_Ref_t>  uni_list;
   vector <Distinguishing_Column_t> dc;
   vector <Contig_Map_Entry_t>  contig_map;
   Contig_Map_Entry_t  map_entry;
   Coord_Line_t  c;
   string  ref_string;
   int  acc, len, prefix_len_added, right_tail;
   bool  result, same_ref, same_query, q_flipped;
   bool  full_merge_left = false, full_merge_right = false;
   bool  first = true, skip_left_variant;
   time_t  now;
   char  buff [MAX_LINE];
   int  orig_ref_tag_ct;
   int  prev_ref_id = -1, prev_q_id = -1, right_string_id = -1;
   int  prev_ref_end = -1, prev_q_hi = 0, prev_q_len;
   int  q_lo, q_hi;
   int  match_ct = 0;

   now = time (NULL);
   cerr << "Starting at " << ctime (& now) << endl;

   Verbose = 0;

   Parse_Command_Line (argc, argv);

   cerr << "Read bank is " << Bank_Name << endl;
   cerr << "Processing NUCmer coords from file " << Coords_File_Name << endl;
   cerr << "Processing CCO messages from file " << CCO_File_Name << endl;
   cerr << "Map file is " << Map_File_Name << endl;

   Load_ID_Map (map, Map_File_Name . c_str ());
   Load_Unitigs (uni_list, Unitig_File_Name . c_str ());

   coords_fp = File_Open (Coords_File_Name . c_str (), "r");
   cco_fp = File_Open (CCO_File_Name . c_str (), "r");
   read_bank . open (Bank_Name, B_READ);

   sprintf (buff, "%s.rpos", Output_Prefix . c_str ());
   rpos_fp = File_Open (buff, "w");
   if  (Output_Multialignment)
       {
        sprintf (buff, "%s.mali", Output_Prefix . c_str ());
        mali_fp = File_Open (buff, "w");
       }
   sprintf (buff, "%s.ref", Output_Prefix . c_str ());
   ref_fp = File_Open (buff, "w");
   sprintf (buff, "%s.vary", Output_Prefix . c_str ());
   vary_fp = File_Open (buff, "w");

   ref_gma . setPrintFlag (PRINT_WITH_DIFFS);
   ref_gma . setPrintFlag (PRINT_USING_STRING_SUB);
   gma2 . setPrintFlag (PRINT_WITH_DIFFS);
   gma2 . setPrintFlag (PRINT_USING_STRING_SUB);

   while  (fscanf (coords_fp, "%d %d %d %d %d %d %lf %d %d %lf %lf %d %d",
        & c . r_lo, & c . r_hi, & c . q_start, & c . q_end,
        & c . r_match_len, & c . q_match_len, & c . percent_id,
        & c . r_len, & c . q_len, & c . r_percent_matched,
        & c . q_percent_matched, & c . r_id, & c . q_id) == 13)
     {
      match_ct ++;

      same_ref = (c . r_id == prev_ref_id);
      same_query = (same_ref && c . q_id == prev_q_id);

      if  (! first && ! same_query && ! full_merge_right
              && right_string_id != prev_q_id && q_hi < prev_q_len)
          {  // need to output variant to end of query string
           Output_Variant (vary_fp, prev_ref_id, prev_q_id, q_flipped,
                prev_ref_end, -1, prev_q_hi, prev_q_len, gma2);
          }

      if  (! same_ref)
          {
           if  (! first)
               {
                // output info for this merged contig
//                ref_gma . Haplo_Sep (sl1, dc, & tg1);
                if  (Output_Multialignment)
                    {
                     fprintf (mali_fp, "\nContig %d:\n\n", prev_ref_id);
                     ref_gma . Print (mali_fp, ref_sl, 60, & ref_tag);
//                     ref_gma . Print (mali_fp, ref_sl, 60);
                    }
                ref_gma . Get_Ungapped_Consensus (ref_string);
                len = ref_string . length ();
                ref_gma . Output_Read_Positions (rpos_fp,
                     prev_ref_id, ref_tag, orig_ref_tag_ct, len, 0);
                sprintf (buff, "%d merged  len=%d  reads=%d",
                     prev_ref_id, len, int (ref_sl . size ()));
                Fasta_Print (ref_fp, ref_string . c_str (), buff);
                printf ("\nFinished merge for ref %d\n", prev_ref_id);
               }
           contig_map . clear ();
           ref_gma . Clear ();
           gma2 . Clear ();
           prefix_len_added = 0;
           full_merge_left = full_merge_right = false;

           // Read next cco message and convert to gma
           result = msg . read (cco_fp);
           if  (! result || msg . getMsgType () != CCO_MSG)
               {
                sprintf (Clean_Exit_Msg_Line,
                     "ERROR:  Failed to read expected CCO message for id = %d",
                     c . r_id);
                Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
               }
           acc = strtol (msg . getAccession () . c_str () + 1, NULL, 10);
           if  (acc != c . r_id)
               {
                sprintf (Clean_Exit_Msg_Line,
                     "ERROR:  Expecting ref CCO %d  read %d instead",
                     c . r_id, acc);
                Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
               }
           Get_Strings_And_Lengths
               (string_list, qual_list, str_len, tag, msg, read_bank, map,
                uni_list);
           ref_gma . Make_From_CCO_Msg (msg, str_len);
           Copy (ref_sl, string_list);
           Copy (ref_tag, tag);
           orig_ref_tag_ct = tag . size ();
           ref_gma . Set_String_Subs ();
           len = ref_gma . Ungapped_Consensus_Len ();
           map_entry . id = c . r_id;
           map_entry . len = len;
           map_entry . start = 0;
           map_entry . end = len;
           contig_map . push_back (map_entry);

           ref_string . erase ();
           right_string_id = -1;

           if  (Verbose > 2)
               {
                printf ("\nRef Contig %d\n", c . r_id);
                ref_gma . Print (stdout, ref_sl, 60, & ref_tag);
               }
           printf ("\nStart merge for ref %d\n", c . r_id);
          }
           
      if  (! same_query)
          {
           if  (same_ref)
               printf ("\nFinished query %d\n", prev_q_id);
           result = msg . read (cco_fp);
           if  (! result || msg . getMsgType () != CCO_MSG)
               {
                sprintf (Clean_Exit_Msg_Line,
                     "ERROR:  Failed to read expected CCO message for id = %d",
                     c . q_id);
                Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
               }
           acc = strtol (msg . getAccession () . c_str () + 1, NULL, 10);
           if  (acc != c . q_id)
               {
                sprintf (Clean_Exit_Msg_Line,
                     "ERROR:  Expecting query CCO %d  read %d instead",
                     c . q_id, acc);
                Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
               }
           Get_Strings_And_Lengths
               (string_list, qual_list, str_len, tag, msg, read_bank, map,
                uni_list);
           gma2 . Make_From_CCO_Msg (msg, str_len);
           Copy (sl2, string_list);
           Copy (tg2, tag);
           sl2_place . clear ();
           sl2_place . resize (string_list . size (), -1);
           prev_ref_end = -1;
           prev_q_hi = 0;

           if  (Verbose > 2)
               {
                printf ("\nQuery Contig %d\n", c . q_id);
                gma2 . Print (stdout, sl2, 60, & tg2);
               }
           printf ("\nStart merge for query %d\n", c . q_id);
          }
           
      prev_ref_id = c . r_id;
      prev_q_id = c . q_id;

      // adjust if previously added a prefix to this reference string
      right_tail = c . r_len - c . r_hi;
      c . r_lo += prefix_len_added;
      c . r_hi += prefix_len_added;

      // do overlaps
      if  (c . q_start < c . q_end)
          {
           q_lo = c . q_start - 1;
           q_hi = c . q_end;
           q_flipped = false;
          }
        else
          {
           if  (! same_query)
               gma2 . Reverse_Complement (sl2);
           q_lo = c . q_len - c . q_start;
           q_hi = 1 + c . q_len - c . q_end;
           q_flipped = true;
          }

      skip_left_variant = false;
      if  (! full_merge_left && c . r_lo <= 1 + END_FUDGE_BASES
               && 0 <= q_lo - c . r_lo)
          {
           int  adjusted_q_lo;
             // low bound on where match is allowed to start

           adjusted_q_lo = Max (0, 1 + q_lo - c . r_lo - END_FUDGE_BASES);
           
           ref_gma . Full_Merge_Left (gma2, adjusted_q_lo, q_lo, q_hi,
                c . r_lo - 1, c . r_hi, prefix_len_added, ref_sl, sl2, sl2_place,
                & ref_tag, & tg2);

           c . r_lo += prefix_len_added;
           c . r_hi += prefix_len_added;
           full_merge_left = skip_left_variant = true;
          }
      else if  (! full_merge_right && right_tail <= END_FUDGE_BASES
                    && right_tail < c . q_len - q_hi)
          {
           ref_gma . Full_Merge_Right (gma2, q_lo, c . r_lo - 1,
                ref_sl, sl2, sl2_place, & ref_tag, & tg2);

           right_string_id = c . q_id;
           full_merge_right = true;
          }
        else
          {
           ref_gma . Partial_Merge (gma2, q_lo, q_hi, c . r_lo - 1,
                c . r_hi, ref_sl, sl2, sl2_place, & ref_tag, & tg2);
          }

      if  (! skip_left_variant && 0 < q_lo
              && (prev_ref_end < c . r_lo || prev_q_hi < q_lo))
          Output_Variant (vary_fp, prev_ref_id, prev_q_id, q_flipped,
               prev_ref_end, c . r_lo - 1, prev_q_hi, q_lo, gma2);

      prev_ref_end = c . r_hi;
      prev_q_hi = q_hi;
      prev_q_len = c . q_len;
      first = false;

      if  (Verbose > 2)
          {
           printf ("\nRef after first merge\n");
           ref_gma . Print (stdout, ref_sl, 60, & ref_tag);
          }

      // push appropriate entries onto  contig_map;
     }

   // handle any leftover matches at end of file
   if  (! first)
       {
        if  (right_string_id != prev_q_id && ! full_merge_right
                && q_hi < prev_q_len)
            {  // need to output variant to end of query string
             Output_Variant (vary_fp, prev_ref_id, prev_q_id, q_flipped,
                  prev_ref_end, -1, prev_q_hi, prev_q_len, gma2);
            }
        // output info for this merged contig
//                ref_gma . Haplo_Sep (sl1, dc, & tg1);
        if  (Output_Multialignment)
            {
             fprintf (mali_fp, "\nContig %d:\n\n", prev_ref_id);
             ref_gma . Print (mali_fp, ref_sl, 60, & ref_tag);
//             ref_gma . Print (mali_fp, true, false, true, 60);
            }
        ref_gma . Get_Ungapped_Consensus (ref_string);
        len = ref_string . length ();
        ref_gma . Output_Read_Positions (rpos_fp,
             prev_ref_id, ref_tag, orig_ref_tag_ct, len, 0);
        sprintf (buff, "%d merged  len=%d  reads=%d",
             prev_ref_id, len, int (ref_sl . size ()));
        Fasta_Print (ref_fp, ref_string . c_str (), buff);
        printf ("\nFinished merge for ref %d\n", prev_ref_id);
       }


   fclose (coords_fp);
   fclose (cco_fp);
   fclose (rpos_fp);
   if  (Output_Multialignment)
       fclose (mali_fp);
   fclose (ref_fp);
   fclose (vary_fp);
   read_bank . close ();


if  (0)
{
 int  i, n;

 n = ref_tag . size ();
 for  (i = 0;  i < n;  i ++)
   ref_tag [i] [0] = 'a';

 n = tg2 . size ();
 for  (i = 0;  i < n;  i ++)
   tg2 [i] [0] = 'b';
}

#if  0
  {
   int  cons_len = gma2 . Ungapped_Consensus_Len ();

//   gma1 . Partial_Merge (gma2, cons_len - 3731, cons_len - 2452, 1322, 2638,
//        sl1, sl2, sl2_place, & tg1, & tg2);

   gma2 . Reverse_Complement (sl2);
   cout << endl << endl << "Contig #2 after reverse" << endl;
   gma2 . Print (stdout, sl2, true, true, 60);

   gma1 . Partial_Merge (gma2, cons_len - 3731, cons_len - 2566, 1322, 2487,
        sl1, sl2, sl2_place, & tg1, & tg2);
   gma1 . Partial_Merge (gma2, cons_len - 2474, cons_len - 236, 3080, 5324,
        sl1, sl2, sl2_place, & tg1, & tg2);

   cout << endl << endl << "Contig #1 after merge" << endl;
   gma1 . Print (stdout, sl1, true, true, 60, & tg1);
  }
#endif


   return  0;
  }



static void  Copy
    (vector <char *> & S, const vector <char *> & T)

//  Copy contents of  T  into  S  allocating new memory
//  for each string

  {
   int  i, n;

   n = S . size ();
   for  (i = 0;  i < n;  i ++)
     free (S [i]);
   S . clear ();

   n = T . size ();
   for  (i = 0;  i < n;  i ++)
     S . push_back (strdup (T [i]));

   return;
  }



static int  Find_Map_Entry
    (int id, const vector <Contig_Map_Entry_t> & map)

//  Search for  id  in  map  and return the subscript of
//  where it was found, or else  -1  if it wasn't found.

  {
   int  i, n;

   n = map . size ();
   for  (i = 0;  i < n;  i ++)
     if  (map [i] . id == id)
         return  i;

   return  -1;
  }



static int  Find_Unitig
    (int uid, const vector <Unitig_Ref_t> & uni_list)

//  Search for the entry in  uni_list  whose  uid  field matches
//   uid  and return the subscript of that entry.  Return  -1
//  if none is found.   uni_list  is assumed to be in sorted order by
//   uid  so a binary search is done.

  {
   int  lo, hi, mid;

   lo = 0;
   hi = uni_list . size () - 1;
   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (uid == uni_list [mid] . uid)
          return  mid;
      else if  (uid < uni_list [mid] . uid)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   return  -1;
  }



static void  Get_Strings_And_Lengths
    (vector <char *> & s, vector <char *> & q, vector <int> & str_len,
     vector <char *> & tag, const Celera_Message_t & msg,
     Bank_t & read_bank, const vector <ID_Pair_t> & map,
     const vector <Unitig_Ref_t> & uni_list)

//  Populate  s  with reads for the contig in  msg  with reads coming
//  from  read_bank , which must already be opened.  Put the corresponding
//  quality-value strings for the reads into  q , put the lengths of
//  the reads into  str_len , and put an identifying tag for
//  each string in  tag .  Use  map  to convert uids
//  (which is what's in the CCO messages) to iids (which is what's in
//  the read bank).  Get surrogate unitig sequences from  uni_list .

  {
   const vector <Celera_MPS_Sub_Msg_t> &  frgs = msg . getMPSList ();
   const vector <Celera_UPS_Sub_Msg_t> &  unis = msg . getUPSList ();
   vector <int>  id_list;
   Read_t  read;
   Ordered_Range_t  position;
   char  tag_buff [MAX_LINE];
   char  * tmp;
   int  len;
   int  a, b, i, j, n;

   n = s . size ();
   for  (i = 0;  i < n;  i ++)
     free (s [i]);
   s . clear ();

   n = q . size ();
   for  (i = 0;  i < n;  i ++)
     free (q [i]);
   q . clear ();

   n = tag . size ();
   for  (i = 0;  i < n;  i ++)
     free (tag [i]);
   tag . clear ();

   str_len . clear ();

   n = msg . getNumFrags ();
   for  (i = 0;  i < n;  i ++)
     {
      string  seq;
      string  qual;
      Range_t  clear;
      int  uid;
      int  qlen;

      position = frgs [i] . getPosition ();
      a = position . getBegin ();
      b = position . getEnd ();
      
      uid = frgs [i] . getId ();
      read_bank . fetch (Lookup (uid,map), read);
      clear = read . getClearRange ();
      if  (Verbose > 2)
	cerr << read;
      seq = read . getSeqString (clear);
      qual = read . getQualString (clear);
      if  (b < a)
          {
           Reverse_Complement (seq);
           reverse (qual . begin (), qual . end ());
          }

      len = seq . length ();
      tmp = strdup (seq . c_str ());
      for  (j = 0;  j < len;  j ++)
        tmp [j] = tolower (tmp [j]);
      s . push_back (tmp);

      qlen = qual . length ();
      if  (len != qlen)
          {
           sprintf (Clean_Exit_Msg_Line,
                "ERROR:  Sequence length (%d) != quality length (%d) for read %d",
                len, qlen, uid);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      tmp = strdup (qual . c_str ());
      q . push_back (tmp);

      str_len . push_back (len);

      sprintf (tag_buff, "r%d", uid);
      tag . push_back (strdup (tag_buff));
     }

   n = msg . getNumUnitigs ();
   if  (Verbose > 2)
       printf ("Num unitigs = %d\n", n);
   for  (i = 0;  i < n;  i ++)
     {
      Celera_Unitig_Type_t  u_type;
      int  uid;

      position = unis [i] . getPosition ();
      a = position . getBegin ();
      b = position . getEnd ();
      
      uid = unis [i] . getId ();
      u_type = unis [i] . getUnitigType ();
      if  (Verbose > 2)
          printf ("Unitig %8d  %c\n", uid, Convert_Char (u_type));
      if  (u_type == STONE_UNI_T)
          {
           j = Find_Unitig (uid, uni_list);
           if  (j < 0)
               {
                sprintf (Clean_Exit_Msg_Line,
                     "ERROR:  Could not find unitig %d in unitig list",
                         uid);
                Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
               }
           
           len = strlen (uni_list [j] . seq);
           tmp = strdup (uni_list [j] . seq);
           for  (j = 0;  j < len;  j ++)
             tmp [j] = tolower (tmp [j]);
           if  (b < a)
               Reverse_Complement (tmp);
           s . push_back (tmp);

           // Make artificial quality string of all 10's
           tmp = (char *) SafeMalloc (len + 1);
           memset (tmp, MIN_QUALITY + 10, len);
           tmp [len] = '\0';
           q . push_back (tmp);

           str_len . push_back (len);

           sprintf (tag_buff, "u%d", uid);
           tag . push_back (strdup (tag_buff));
          }
     }

   return;
  }



static void  Load_ID_Map
    (vector <ID_Pair_t> & map, const char * fn)

//  Load the  iid-uid  map from the file name  fn  into
//   map .

  {
   FILE  * fp;
   char  tag [MAX_LINE];
   ID_Pair_t  pair;

   map . clear ();
   fp = File_Open (fn, "r");
   while  (fscanf (fp, "%d %d %s", & pair . uid, & pair . iid, tag) == 3)
     if  (strcmp (tag, "AFG") == 0)
         map . push_back (pair);

   fclose (fp);

   return;
  }



static void  Load_Unitigs
    (vector <Unitig_Ref_t> & uni_list, const char * fn)

//  Load unitigs from the file named  fn  and store them in  uni .

  {
   FILE  * fp;
   string  s, tag;
   Unitig_Ref_t  u;

   uni_list . clear ();
   fp = File_Open (fn, "r");
   while  (Fasta_Read (fp, s, tag))
     {
      u . uid = int (strtol (tag . c_str (), NULL, 10));
      if  (u . uid == 0)
          {
           sprintf (Clean_Exit_Msg_Line,
                "ERROR:  Bad unitig uid \"%s\"",
                    tag . c_str ());
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      u . seq = strdup (s . c_str ());
      uni_list . push_back (u);
     }

   fclose (fp);

   return;
  }



static int  Lookup
    (int uid, const vector <ID_Pair_t> & map)

//  Find  uid  in sorted list  map  and return the corresponding
//  iid .  Fail if not found.

  {
   int  lo, hi, mid;

   lo = 0;  hi = map . size () - 1;
   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (map [mid] . uid == uid)
          return  map [mid] . iid;
      else if  (uid < map [mid] . uid)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   cerr << "Lookup of uid " << uid << " failed" << endl;
   exit (EXIT_FAILURE);
  }



static void  Output_Variant
     (FILE * fp, int ref_id, int q_id, bool q_flipped,
      int r_lo, int r_hi, int q_lo, int q_hi,
      const Gapped_Multi_Alignment_t & gma)

//  Output to  fp  an indication of the variant sequence between
//  the reference region  r_lo .. r_hi  and the query sequence
//  in the consensus of  gma  for region  q_lo .. q_hi .
//   ref_id  and  q_id  are the ids of the reference and query
//  sequences, respectively.   q_flipped  indicates whether the
//  query sequence is reverse complemented wrt the reference.
//  All coordinates are gapped.

  {
   string  seq;
   char  buff [MAX_LINE];
   int  q_start, q_end, q_len;

   q_len = gma . Ungapped_Consensus_Len ();
   if  (q_flipped)
       {
        q_start = q_len - q_lo;
        q_end = q_len - q_hi;
       }
     else
       {
        q_start = q_lo;
        q_end = q_hi;
       }

   sprintf (buff, "%d  %d  %d  %d  %d  %d",
       ref_id, q_id, r_lo, r_hi, q_start, q_end);

   gma . Get_Partial_Ungapped_Consensus (seq, q_lo, q_hi);

   Fasta_Print (fp, seq . c_str (), buff);

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

   while  (! errflg && ((ch = getopt (argc, argv, "ah")) != EOF))
     switch  (ch)
       {
        case  'a' :
          Output_Multialignment = true;
          break;

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

   if  (optind > argc - 6)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Coords_File_Name = argv [optind ++];
   CCO_File_Name = argv [optind ++];
   Bank_Name = argv [optind ++];
   Map_File_Name = argv [optind ++];
   Unitig_File_Name = argv [optind ++];
   Output_Prefix = argv [optind ++];

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  merge-contigs  <coords-file> <cco-file> <bank-name>\n"
           "                       <map-file> <unitig-file> <out-prefix>\n"
           "\n"
           "Read NUCmer coords output from <coords-file> and use it to\n"
           "output merged contigs.  Contigs come from CCO messages in\n"
           "<cco-file> and must be in the order of appearance in <coords-file>.\n"
           "Reads are obtained from <bank-name>.  <map-file>  gives the uid-iid\n"
           "mapping for reads.  <unitig-file> is the fasta sequence of\n"
           "surrogate unitigs\n"
           "Output files produced are:\n"
           "  <out-prefix>.rpos   contig lengths and positions of reads in them\n"
           "  <out-prefix>.mali   multialignment of reads to contigs\n"
           "  <out-prefix>.ref    reference sequence for contigs\n"
           "  <out-prefix>.vary   indications of SNPs are larger variations\n"
           "                      relative to the reference\n"
           "\n"
           "Options:\n"
           "  -a    Output the <out-prefix>.mali file\n"
           "  -h    Print this usage message\n"
           "\n");

   return;
  }



