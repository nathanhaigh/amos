//
//  File:  align.h
//
//  Last Modified:  25 November 2002
//
//  Routines to do string alignments


#ifndef  __ALIGN_H_INCLUDED
#define  __ALIGN_H_INCLUDED


#include  "delcher.hh"
#include  "matrix.hh"
#include  "fasta.hh"
#include  "WGA_datatypes.hh"
#include  "CelMsgWGA.hh"
#include  "prob.hh"
#include  "Slice.h"
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
const char  MAX_QUALITY_CHAR = 60;


const int  NEG_INFTY_SCORE = ((1 << 29) - 1) * -1;
const unsigned int  FROM_LEFT = 0;
const unsigned int  FROM_DIAG = 1;
const unsigned int  FROM_TOP = 2;
const unsigned int  FROM_NOWHERE = 3;


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


class  Vote_t
  {
  public:
    int  here [ALPHABET_SIZE + 1];
        // votes at this position; letters followed by blank
    int  after [ALPHABET_SIZE + 1];
        // votes for insertion after this position; letters followed by blank

    void  Incr_After
        (char ch);
    void  Incr_Blank
        (void);
    void  Incr_By
        (char ch, bool with_blank);
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

   void  Dump
       (FILE * fp)  const;
  };


class  Gapped_Alignment_t;

class  Alignment_t  :  public Base_Alignment_t
  {
  friend  Gapped_Alignment_t;
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

   void  Dump
       (FILE * fp)  const;
   void  Dump_Delta
       (FILE * fp)  const;
   void  Incr_Votes
       (vector <Vote_t> & vote, char * a);
   void  Print
       (FILE * fp, const char * a, const char * b,
        int width = DEFAULT_FASTA_WIDTH);
   void  Set_Exact
       (int a_start, int b_start, int len);
   void  Set_From_VS_Matrix
       (Match_Extent_Entry_t * * t, int e, int col);
   void  Set_Inserts
       (vector <short> & v)  const;
   void  Set_To_Identity
       (int len);
  };


class  Gapped_Alignment_t  :  public Base_Alignment_t
  {
  friend  Alignment_t;
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
  friend  Gapped_Multi_Alignment_t;
  private:
   string  consensus;
       // consensus of each column of the multialignment
   vector <Alignment_t>  align;
       // alignment of each string to the consensus

  public:
   void  Clear
       (void);
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
        int offset_delta, double error_rate,
        vector <Vote_t> & vote);
  };



class  Gapped_Multi_Alignment_t
  {
  friend  Multi_Alignment_t;
  private:
   string  consensus;
       // consensus of each column of the multialignment
   string  con_qual;
       // quality value of each character of consensus
   vector <Gapped_Alignment_t>  align;
       // alignment of each string to the consensus

  public:
// Temporary
void  Check  (void)
  {
   int  i;

   for  (i = 0;  i < 20;  i ++)
     putchar (consensus [i]);
   putchar ('\n');
  }

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

   void  setConsensusString
       (const string & s)
     { consensus = s; }
   void  setQualityString
       (const string & q)
     { con_qual = q; }

   void  Add_Aligned_Seqs
       (const Gapped_Multi_Alignment_t & m, const Alignment_t & ali,
        int cons_lo, int cons_hi, vector <char *> & sl1,
        const vector <char *> & sl2, vector <char *> * tg1 = NULL,
        vector <char *> * tg2 = NULL);
   void  Clear
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
   void  Get_Distinguishing_Columns
       (vector <Distinguishing_Column_t> & dc,
        const vector <char *> & sl);
   void  Get_Element_Subs
       (Distinguishing_Column_t & d, const vector <char *> & sl);
   void  Get_Positions
       (vector <Range_t> & pos)  const;
   void  Haplo_Sep
       (const vector <char *> & sl, vector <Distinguishing_Column_t> & dc,
        vector <char *> * tg = NULL);
   void  Make_From_CCO_Msg
       (const Celera_Message_t & msg, const vector <int> & slen);
   void  Merge
       (const Gapped_Multi_Alignment_t & m);
   void  Partial_Merge
       (const Gapped_Multi_Alignment_t & m, int a_lo, int a_hi,
        int b_lo, int b_hi, vector <char *> & sl1,
        const vector <char *> & sl2, vector <char *> * tg1 = NULL,
        vector <char *> * tg2 = NULL);
   void  Print
       (FILE * fp, const vector <char *> & s, bool with_diffs = false,
        int width = DEFAULT_FASTA_WIDTH,
        vector <char *> * tag = NULL);
   void  Print_Consensus
       (char * buff, int b1, int b2);
   void  Reverse_Complement
       (vector <char *> & s);
   void  Set_Consensus_And_Qual
       (const vector <char *> & s, const vector <char *> & q);
   void  Set_Phase
       (vector <Distinguishing_Column_t> & dc);
   void  Show_Skips
       (FILE * fp);
   void  Sort
       (vector <char *> & s, vector <int> * ref = NULL);
   int  Ungapped_Consensus_Len
       (void)  const;
  };



void  Best_Spanning_Tree
    (int n, const vector <Phase_Entry_t> & edge_list,
     vector <int> & tree_edge);
void  Classify_Reads
    (const vector <Distinguishing_Column_t> & dc, int n,
     vector <char> & side, vector <int> & segment);
int  DNA_Char_To_Sub
    (char ch);
int  Exact_Prefix_Match
    (const char * s, const char * t, int max_len);
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
    (vector <char *> & s, vector <int> & offset, int offset_delta,
     double error_rate, Gapped_Multi_Alignment_t & ma,
     vector <int> * ref = NULL);
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
void  Sort_Strings_And_Offsets
    (vector <char *> & s, vector <int> & offset, vector <int> * ref = NULL);
bool  Substring_Match_VS
    (const char * s, int s_len, const char * t, int t_len,
     int lo, int hi, int max_errors, Alignment_t & align);
char  Sub_To_DNA_Char
    (int i);
void  Traverse
    (int v, int par, vector < vector <Phase_Entry_t> > & tree,
     vector <Distinguishing_Column_t> & dc, char ch, int & sum);
int  UF_Find
    (int i, vector <int> & uf);
int  UF_Find_With_Parity
    (int i, vector <int> & uf, int & parity);
void  UF_Union
    (int i, int j, vector <int> & uf);


#endif
