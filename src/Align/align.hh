//
//  File:  align.h
//
//  Last Modified:  25 November 2002
//
//  Routines to do string alignments


#ifndef  __ALIGN_H_INCLUDED
#define  __ALIGN_H_INCLUDED


#include  "datatypes_AMOS.hh"
#include  "utility_AMOS.hh"
#include  "Contig_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"
#include  "prob.hh"
#include  "matrix.hh"
#include  "CelMsg.hh"
#include  "Slice.h"
#include  <iostream>
#include  <string>
#include  <vector>
#include  <functional>
#include  <algorithm>



//  Delta encoding shows the positions of the insertions and deletions
//  in the strings.  If the encoding is from string  A  to string  B
//  then minus values indicate an extra character in  A  and plus values
//  indicate an extra character in  B .  The absolute value is the
//  position of that extra character relative to the current positions.
//  E.g., for the alignment:
//    A:  abcxxdefghi-jklmn
//    B:  abc--dexghixjklmn
//           ^^  ^   ^
//  The delta encoding would be:  -4, -1, +7

typedef  vector <int>  Delta_Encoding_t;


const char  ALPHABET [] = "acgtn";
const int  ALPHABET_SIZE = 5;
const int  MATCH_FROM_TOP = 0;
const int  MATCH_FROM_NW = 1;
const int  MATCH_FROM_NE = -1;
const int  MATRIX_SIZE_LIMIT = 100000;
  // Largest matrix that will be used for full alignment computations
const int  MAX_ALIGN_ATTEMPTS = 3;
  // Number of times to try alignments, loosening constraints each time
const int  MAX_REFINEMENTS = 3;
  // Number of times to refine an alignment
const double  BIN_CUTOFF_PROB = 1e-6;
  // Probability cutoff for the binonial distribution
const char  MAX_QUALITY_CHAR = 60;


const int  NEG_INFTY_SCORE = ((1 << 29) - 1) * -1;
const unsigned int  FROM_LEFT = 0;
const unsigned int  FROM_DIAG = 1;
const unsigned int  FROM_TOP = 2;
const unsigned int  FROM_NOWHERE = 3;


// Flags for printing multialignments
const unsigned int  PRINT_WITH_DIFFS = 1;
  // if set true then print ^'s under consensus where not unanimous
const unsigned int  PRINT_USING_STRING_SUB = 1 << 1;
  // if set true then use string_sub value to get component string
const unsigned int  PRINT_CONSENSUS_GAP_COORDS = 1 << 2;
  // if set true then print consensus range values counting gap positions


//  Results for checking/fixing starts of alignments
enum  Fix_Status_t
  {NO_FIX_NEEDED, NEEDS_LEFT_SHIFT, SHIFTED_RIGHT, SHIFTED_LEFT};


//  Defaults values for alignment score parameters
const int  DEFAULT_MATCH_SCORE = 1;
  // Score for matching characters
const int  DEFAULT_MISMATCH_SCORE = -3;
  // Score for substitution error
const int  DEFAULT_INDEL_SCORE = -3;
  // Score for insertion or deletion
const int  DEFAULT_GAP_SCORE = -1;
  // Extra penalty for first insertion/deletion in a run


class  Align_Score_Entry_t
  {
  public:
   int  diag_score : 30;    // score where last move is match/mismatch
   unsigned int  diag_from : 2;
   int  top_score : 30;     // score where last move is from preceding row
   unsigned int  top_from : 2;
   int  left_score : 30;    // score where last move is from preceding column
   unsigned int  left_from : 2;

   void  Dump
       (FILE * fp)  const;
   void  Get_Max
       (int & max_score, unsigned int & max_from)  const;
   void  Get_Max_Left
       (int & max_score, unsigned int & max_from, int penalty)  const;
   void  Get_Max_Top
       (int & max_score, unsigned int & max_from, int penalty)  const;
  };


class  Augmented_Score_Entry_t  :  public Align_Score_Entry_t
  {
  public:
   int  top_ref;
   int  diag_ref;
   int  left_ref;
        // subscript where best alignment to here began in a designated prior row

   void  Dump
       (FILE * fp)  const;
   int  Get_Ref
       (unsigned int & from)  const;
  };


class  Errors_Score_Entry_t  :  public Augmented_Score_Entry_t
  {
  public:
   int  top_errors;
   int  diag_errors;
   int  left_errors;
        // number of errors to get to this entry in the respective direction

   void  Dump
       (FILE * fp)  const;
   int  Get_Errors
       (unsigned int & from)  const;
  };


class Phase_Entry_t
  {
  public:
   int  from, to;  // entries connected
   int  same_ct, opposite_ct, weight;

   void  Print
       (FILE * fp)  const
     {
      fprintf (fp, "from = %d  to = %d  same = %d  opp = %d  wt = %d\n",
           from, to, same_ct, opposite_ct, weight);
     }

   bool  operator >
       (const Phase_Entry_t & p)  const
     {
      if  (weight > p . weight)
          return  true;
      if  (weight == p . weight && same_ct > p . same_ct)
          return  true;
      if  (weight == p . weight && same_ct == p . same_ct
             && abs (to - from) > abs (p . to - p . from))
          return  true;

      return  false;
     }
  };


class  Distinguishing_Column_t
  {
  public:
   int  lo, hi;
       // indicate range of gapped consensus columns where there's a
       // consistent significant difference
   char  hapa_ch, hapb_ch, phase_ch;
   vector <int>  hapa_sub, hapb_sub;

   bool  Intersects
       (const Distinguishing_Column_t & d, Phase_Entry_t & p,
        int & min);
   void  Print
       (FILE * fp)
     {
      int  i, na, nb;

      na = hapa_sub . size ();
      nb = hapb_sub . size ();
      fprintf (fp, "lo/hi = %d/%d  %c %2d  %c %2d  %c\n",
           lo, hi, hapa_ch, na, hapb_ch, nb, phase_ch);
      for  (i = 0;  i < na;  i ++)
        fprintf (fp, " %3d", hapa_sub [i]);
      fputc ('\n', fp);
      for  (i = 0;  i < nb;  i ++)
        fprintf (fp, " %3d", hapb_sub [i]);
      fputc ('\n', fp);
     }
  };


class  Match_Extent_Entry_t
  {
  public:
   int  len : 30;
   int  from : 2;  
  };


class  Simple_Overlap_t
  {
  public:
   int  a_id, b_id;
   int  a_hang, b_hang;
   int  a_olap_len, b_olap_len;
   int  errors, score;
   bool  flipped;
     // a is always forward.  If b is forward flipped is false; otherwise,
     // it's true
  };


class  Vote_t
  {
  public:
    int  here [ALPHABET_SIZE + 1];
        // votes at this position; letters followed by blank
    int  after [ALPHABET_SIZE + 1];
        // votes for insertion after this position; letters followed by blank

    void  Incr_After
        (char ch, int incr_val = 1);
    void  Incr_Blank
        (int incr_val = 1);
    void  Incr_By
        (char ch, bool with_blank, int incr_val = 1);
    char  Max_After_Char
        (void);
    char  Max_Here_Char
        (void);
    void  Set_To
        (char ch, bool with_blank);
    void  Set_Zero
        (void);
  };


class  Base_Alignment_t
  {
  public:
   int  a_lo, a_hi;
       // matching region in string  a  in space-based coords
   int  b_lo, b_hi;
       // matching region in string  b  in space-based coords
   int  errors;
       // number of errors in the matching region
       // currently this is ignored
   int  string_sub;
       // subscript of string in accompanying array
   unsigned  flipped : 1;
       // 0 if original a was same orientation as b; 1 otherwise
   int  a_len : 31;
       // length of entire a string

   Base_Alignment_t
       ()  // default constructor
     {
      errors = string_sub = a_len = flipped = 0;
     }
   void  Dump
       (FILE * fp)  const;
   double  Error_Rate
       (void)  const;
   void  Flip_AB
       (void);
   bool  Is_Empty
     (void)  const;
  };


class  Gapped_Alignment_t;

class  Alignment_t  :  public Base_Alignment_t
  {
  friend class Gapped_Alignment_t;
  private:
   Delta_Encoding_t  delta;
       // indicates positions of inserts/deletes

  public:
   const Delta_Encoding_t &  getDelta
       (void)  const
     { return delta; }

   void  setDelta
       (const Delta_Encoding_t & d)
     { delta = d; }

   int  B_Position
       (int a)  const;
   void  Clear
       (void);
   void  Combine
       (const Alignment_t & a1, const Alignment_t & a2);
   void  Dump
       (FILE * fp)  const;
   void  Dump_Delta
       (FILE * fp)  const;
   void  Check_Fix_Start
       (const char * s , int s_len, const char * t, int t_len,
        Fix_Status_t & status);
   void  Flip_AB
       (void);
   void  Incr_Votes
       (vector <Vote_t> & vote, char * a, int incr_val = 1);
   void  Offset_A
       (int n);
   void  Print
       (FILE * fp, const char * a, const char * b,
        int width = DEFAULT_FASTA_WIDTH);
   void  Set_Empty_A
       (int a_start, int b_start, int b_end);
   void  Set_Exact
       (int a_start, int b_start, int len);
   void  Set_From_VS_Matrix
       (Match_Extent_Entry_t * * t, int e, int col);
   void  Set_Inserts
       (vector <short> & v)  const;
   void  Set_To_Identity
       (int len);
   void  Shift_First_Delta
       (int offset);
  };


class  Gapped_Alignment_t  :  public Base_Alignment_t
  {
  friend class Alignment_t;
  protected:
   vector <int>  skip;
       // indicates positions of inserts/deletes

  public:
   void  setSkip
       (const vector <int> & s)
     { skip = s; }

   void  Clear
       (void);
   void  Convert_From
       (const Alignment_t & ali, vector <int> & tr);
   void  Convert_Skip_to_Del
       (vector <int> & del);
   void  Dump
       (FILE * fp)  const;
   void  Dump_Skip
       (FILE * fp)  const;
   void  Flip
       (int a_len, int b_len);
   void  Incr_Column_Chars
       (Matrix <unsigned char> & count, const char * s);
   char  Get_Aligning_Char
       (int b, char * s);
   int  Get_Skip
       (int i)  const;
   void  Make_Sub_Alignment
       (Gapped_Alignment_t & sub_ali, int b1, int b2)  const;
   void  Modify_For_B_Inserts
       (const vector <int> & insert_sub);
   void  Pass_Through
       (const Alignment_t & ali);
   void  Print_Subalignment_Line
       (char * buff, int b1, int b2, char * s, int & a1, int & a2);
   void  Shift_Skip
       (int offset);
  };


class  Gapped_MA_Bead_t
  {
  public:
   int  a_pos, b_pos, skip_pos;
   int  skip_i;
   char  seq_ch, qual_ch;
   const char  * seq, * qual;
   const Gapped_Alignment_t  * ga;
   bool  active;

   void  Advance
       (void);
  };


class  Gapped_Multi_Alignment_t;

class  Multi_Alignment_t
  {
  friend class Gapped_Multi_Alignment_t;
  private:
   string  id;
   string  consensus;
       // consensus of each column of the multialignment
   vector <Alignment_t>  align;
       // alignment of each string to the consensus

  public:
   const char *  getConsensus (void)
     { return consensus . c_str (); }
   const char *  getID
       (void)
     // return the id string as a C string
     { return id . c_str (); }

   void  setID
       (const string & s)
     { id = s; }

   bool  Check_Subsequent_Overlaps
     (const char * cons, int cons_len, const vector <char *> & s, int i, 
      const vector <int> & offset, int n, int curr_offset, int wiggle, int min_olap,
      double error_rate, int offset_sum, int virtual_cons_len);
   void  Clear
     (void);
   int  Estimate_Offset_Position
     (int i, const vector <int> & offset);
   int  Estimate_Offset_With_Expels
     (int i, const vector <int> & offset, const vector <bool> & expel);
   void  Print_Alignments_To_Consensus
     (FILE * fp, vector <char *> s);
   void  Reset_From_Votes
     (const vector <char *> & s,
      int offset_delta, double error_rate,
      vector <Vote_t> & vote, bool & changed);
   void  Set_Consensus
     (char * s);
   void  Set_Initial_Consensus
     (const vector <char *> & s, const vector <int> & offset,
      int offset_delta, double error_rate, int min_overlap,
      vector <Vote_t> & vote, vector <char *> * tag_list = NULL,
      bool allow_expels = FALSE);
  };



class  Gapped_Multi_Alignment_t
  {
  friend class Multi_Alignment_t;
  private:
   unsigned int  print_flags;
   string  id;
   string  consensus;
       // consensus of each column of the multialignment
   string  con_qual;
       // quality value of each character of consensus
   vector <Gapped_Alignment_t>  align;
       // alignment of each string to the consensus

  public:
   Gapped_Multi_Alignment_t
       ()  // default constructor
     { print_flags = 0; }

   int  getConsensusLen
       (void)  const
     // return the length of the consensus string
     { return consensus . length (); }
   const char *  getConsensusString
       (void)
     // return the consensus string as a C string
     { return consensus . c_str (); }
   const char *  getQualityString
       (void)
     // return the quality string as a C string
     { return con_qual . c_str (); }
   const char *  getID
       (void)
     // return the id string as a C string
     { return id . c_str (); }

   void  setConsensusString
       (const string & s)
     { consensus = s; }
   void  setID
       (const string & s)
     { id = s; }
   void  setPrintFlag
       (unsigned int  f)
     { print_flags |= f; }
   void  setQualityString
       (const string & q)
     { con_qual = q; }
   void  unsetPrintFlag
       (unsigned int  f)
     { print_flags &= (~ f); }

   void  TA_Print
       (FILE * fp, const vector <char *> & s,
        const vector <AMOS :: Range_t> & clr_list,
        int width, const vector <char *> * tag, const string & id);
   

   void  Add_Aligned_Seqs
       (const Gapped_Multi_Alignment_t & m, const Alignment_t & ali,
        int cons_lo, int cons_hi, vector <char *> & sl1,
        const vector <char *> & sl2, vector <int> & sl2_place,
        vector <char *> * tg1 = NULL, vector <char *> * tg2 = NULL);
   void  Clear
       (void);
   void  Clear_All_Print_Flags
       (void)
     { print_flags = 0; }
   void  Consensus_To_Lower
       (void);
   void  Convert_Consensus
       (const Multi_Alignment_t & ma, const vector <short> & v);
   void  Convert_From
       (const Multi_Alignment_t & ma);
   void  Count_Column_Chars
       (Matrix <unsigned char> & count,
        const vector <char *> & sl);
   void  Dump_Aligns
       (FILE * fp);
   void  Expand_Consensus
       (const Alignment_t & ali);
   void  Extract_Gapped_Region
       (int lo, int hi, string & s, int & gapped_lo, int & gapped_hi)  const;
   void  Extract_IMP_Dels
       (vector < vector <int> > & del_list);
   void  Full_Merge_Left
       (const Gapped_Multi_Alignment_t & m, int adj_a_lo, int a_lo, int a_hi,
        int b_lo, int b_hi, int & prefix_len_added, vector <char *> & sl1,
        const vector <char *> & sl2, vector <int> & sl2_place,
        vector <char *> * tg1, vector <char *> * tg2);
   void  Full_Merge_Right
       (const Gapped_Multi_Alignment_t & m, int a_lo,
        int b_lo, vector <char *> & sl1,
        const vector <char *> & sl2, vector <int> & sl2_place,
        vector <char *> * tg1 = NULL, vector <char *> * tg2 = NULL);
   void  Get_Distinguishing_Columns
       (vector <Distinguishing_Column_t> & dc,
        const vector <char *> & sl);
   void  Get_Element_Subs
       (Distinguishing_Column_t & d, const vector <char *> & sl);
   void  Get_Partial_Ungapped_Consensus
       (string & s, int lo, int hi)  const;
   void  Get_Positions
       (vector <AMOS::Range_t> & pos)  const;
   void  Get_Ungapped_Consensus
       (string & s);
   void  Haplo_Sep
       (const vector <char *> & sl, vector <Distinguishing_Column_t> & dc,
        vector <char *> * tg = NULL);
   void  Make_From_CCO_Msg
       (const Celera_Message_t & msg, const vector <int> & slen);
   void  Make_Gapped_Ungapped_Lookup
       (vector <int> & lookup);
   void  Merge
       (const Gapped_Multi_Alignment_t & m);
   void  Output_Read_Positions
       (FILE * fp, int id, const vector <char *> & tag, int orig_tag_ct,
        int len, int offset = 0);
   void  Partial_Merge
       (const Gapped_Multi_Alignment_t & m, int a_lo, int a_hi,
        int b_lo, int b_hi, vector <char *> & sl1,
        const vector <char *> & sl2, vector <int> & sl2_place,
        vector <char *> * tg1 = NULL, vector <char *> * tg2 = NULL);
   void  Print
       (FILE * fp, const vector <char *> & s, int width = DEFAULT_FASTA_WIDTH,
        vector <char *> * tag = NULL);
   void  Print_Consensus
       (char * buff, int b1, int b2);
   int  Print_Empty_Aligns
       (FILE * fp, const vector <char *> & tag, const char * contig_id);
   void  Print_Ungapped_Consensus
       (FILE * fp, char * hdr, int width = DEFAULT_FASTA_WIDTH);
   void  Reverse_Complement
       (vector <char *> & s);
   void  Set_Consensus_And_Qual
       (const vector <char *> & s, const vector <char *> & q);
   void  Set_Flipped
       (const vector <AMOS :: Range_t> & clr);
   void  Set_Phase
       (vector <Distinguishing_Column_t> & dc);
   void  Set_String_Subs
       (void);
   void  Shift_B_Right
       (int offset);
   void  Show_Skips
       (FILE * fp);
   void  Sort
       (vector <char *> & s, vector <int> * ref = NULL,
        vector <char *> * tag_list = NULL);

   int  Ungapped_Consensus_Len
       (void)  const;
   void Make_AMOS_Contig(const vector <AMOS :: Range_t> & clr_list,
			 const vector <char *> & tag,
			 AMOS::Contig_t & out) const;
  };



void  Align_Row_Update
    (vector <Augmented_Score_Entry_t> & align_row, char ch, const char * s,
     int s_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, int first_score);
void  Align_Row_Update_With_Errors
    (vector <Errors_Score_Entry_t> & align_row, char ch, const char * s,
     int s_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, int first_score, int first_error);
void  Banded_Overlap
    (const char * s, int s_len, const char * t, int t_len,
     int  lo_offset, int hi_offset, Simple_Overlap_t & olap,
     int match_score = DEFAULT_MATCH_SCORE,
     int mismatch_score = DEFAULT_MISMATCH_SCORE,
     int indel_score = DEFAULT_INDEL_SCORE,
     int gap_score = DEFAULT_GAP_SCORE);
void  Best_Spanning_Tree
    (int n, const vector <Phase_Entry_t> & edge_list,
     vector <int> & tree_edge);
void  Classify_Reads
    (const vector <Distinguishing_Column_t> & dc, int n,
     vector <char> & side, vector <int> & segment);
void  Complete_Align
    (const char * s, int s_lo, int s_hi, const char * t, int t_lo, int t_slip,
     int t_hi, int match_score, int mismatch_score, int indel_score,
     int gap_score, Align_Score_Entry_t * first_entry,
     Align_Score_Entry_t & last_entry, Alignment_t & align);
void  Complete_Align_Full_Matrix
    (const char * s, int s_lo, int s_hi, const char * t, int t_lo, int t_slip,
     int t_hi, int match_score, int mismatch_score, int indel_score,
     int gap_score, Align_Score_Entry_t * first_entry,
     Align_Score_Entry_t & last_entry, Alignment_t & align);
void  Complete_Align_Save_Space
    (const char * s, int s_lo, int s_hi, const char * t, int t_lo, int t_slip,
     int t_hi, int match_score, int mismatch_score, int indel_score,
     int gap_score, Align_Score_Entry_t * first_entry,
     Align_Score_Entry_t & last_entry, Alignment_t & align);
int  DNA_Char_To_Sub
    (char ch);
int  Exact_Prefix_Match
    (const char * s, const char * t, int max_len);
int  Gapped_Equivalent
    (int pos, const string & s);
void  Global_Align
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align);
void  Incr_Opposite
    (vector <Phase_Entry_t> & v, int from, int to);
void  Incr_Same
    (vector <Phase_Entry_t> & v, int from, int to);
bool  Is_Distinguishing
    (unsigned char ct [5], char & ch1, int & ch1_ct,
     char & ch2, int & ch2_ct);
int  Match_Count
    (const vector <int> & a, const vector <int> & b);
void  Multi_Align
    (const string & id, vector <char *> & s, vector <int> & offset,
     int offset_delta, double error_rate, int min_overlap,
     Gapped_Multi_Alignment_t & ma, vector <int> * ref = NULL,
     vector <char *> * tag_list = NULL, bool allow_expels = FALSE);
void  Overlap_Align
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int t_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align);
void  Overlap_Align_Full_Matrix
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int t_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align);
void  Overlap_Align_Save_Space
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int t_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align);
bool  Overlap_Match_VS
    (const char * s, int s_len, const char * t, int t_len,
     int lo, int hi, int min_len, int max_errors,
     Alignment_t & align);
void  Print_Align_Lines_Pair
    (FILE * fp, const string & s, const string & t, int len,
     const char * s_label, const char * t_label,
     int s_lo, int s_hi, int t_lo, int t_hi);
void  Print_VS_Table
    (FILE * fp, Match_Extent_Entry_t * * tab, int t_lo, int t_hi, int e,
     int best_i, bool found);
bool  Range_Intersect
    (int a_lo, int a_hi, int b_lo, int b_hi);
void  Simple_Overlap
    (const char * s, int s_len, const char * t, int t_len,
     Simple_Overlap_t & olap, int match_score = DEFAULT_MATCH_SCORE,
     int mismatch_score = DEFAULT_MISMATCH_SCORE,
     int indel_score = DEFAULT_INDEL_SCORE,
     int gap_score = DEFAULT_GAP_SCORE);
void  Sort_Strings_And_Offsets
    (vector <char *> & s, vector <int> & s_len, vector <int> & offset,
     vector <int> * ref, vector <char *> * tag_list);
bool  Substring_Match_VS
    (const char * s, int s_len, const char * t, int t_len,
     int lo, int hi, int max_errors, Alignment_t & align);
char  Sub_To_DNA_Char
    (int i);
void  Trace_Back_Align_Path
    (const vector < vector <Align_Score_Entry_t> > & a, int & r, int & c,
     int r_start, int c_start, vector <int> & delta, int & errors,
     const char * s, const char * t);
void  Traverse
    (int v, int par, vector < vector <Phase_Entry_t> > & tree,
     vector <Distinguishing_Column_t> & dc, char ch, int & sum);
int  UF_Find
    (int i, vector <int> & uf);
int  UF_Find_With_Parity
    (int i, vector <int> & uf, int & parity);
void  UF_Union
    (int i, int j, vector <int> & uf);
int  Ungapped_Positions
    (const string & s, int lo, int hi);
void  Update_Banded_Row
    (vector <Errors_Score_Entry_t> & band, char ch, const char * s,
     int s_len, int left_col, int right_col, int match_score,
     int mismatch_score, int indel_score, int gap_score, int first_score,
     int first_error);



template <class DT>
void  Permute
    (vector <DT> & v, const vector <int> & p)

//  Permute the entries in  v  according to the values in  p .

  {
   int  i, m, n;
   DT  save;

   m = v . size ();
   n = p . size ();
   if  (m != n)
       {
        sprintf (Clean_Exit_Msg_Line,
             "ERROR:  Permute size mismatch.  v.size = %d  p.size = %d\n",
             m, n);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   vector <bool>  done (n, false);

   for  (i = 0;  i < n;  i ++)
     if  (p [i] != i && ! done [i])
         {
          int  j, k;

          save = v [i];

          for  (j = i;  p [j] != i;  j = k)
            {
             k = p [j];
             v [j] = v [k];
             done [j] = true;
            }

          v [j] = save;
          done [j] = true;
         }

   return;
  }



#endif
