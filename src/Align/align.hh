//
//  File:  align.h
//
//  Last Modified:  25 November 2002
//
//  Routines to do string alignments


#ifndef  __ALIGN_H_INCLUDED
#define  __ALIGN_H_INCLUDED


#include  "delcher.hh"
#include  "fasta.hh"
#include  "WGA_datatypes.hh"
#include  "prob.hh"
#include  "Slice.h"
#include  <string>
#include  <vector>


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
  };


class  Gapped_Alignment_t;

class  Alignment_t  :  public Base_Alignment_t
  {
  friend  Gapped_Alignment_t;
  private:
   Delta_Encoding_t  delta;
       // indicates positions of inserts/deletes

  public:
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
   void  Gapped_Alignment_t :: Convert_From
       (const Alignment_t & ali, vector <int> & tr);
   void  Convert_Skip_to_Del
       (vector <int> & del);
   void  Dump
       (FILE * fp);
   void  Dump_Skip
       (FILE * fp);
   int  Get_Skip
       (int i)  const;
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
   const char *  getConsensusString
       (void)
     // return the consensus string as a C string
     { return consensus . c_str (); }
   const char *  getQualityString
       (void)
     // return the quality string as a C string
     { return con_qual . c_str (); }

   void  Convert_Consensus
       (const Multi_Alignment_t & ma, const vector <short> & v);
   void  Convert_From
       (const Multi_Alignment_t & ma);
   void  Extract_IMP_Dels
       (vector < vector <int> > & del_list);
   void  Get_Positions
       (vector <Range_t> & pos)  const;
   void  Print
       (FILE * fp, const vector <char *> & s,
        int width = DEFAULT_FASTA_WIDTH);
   void  Print_Consensus
       (char * buff, int b1, int b2);
   void  Set_Consensus_And_Qual
       (const vector <char *> & s, const vector <char *> & q);
   void  Show_Skips
       (FILE * fp);
   void  Sort
       (vector <char *> & s, vector <int> * ref = NULL);
  };



int  Exact_Prefix_Match
    (const char * s, const char * t, int max_len);
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
void  Vote_From_Alignment
    (vector <Vote_t> & vote, const Alignment_t & ali);


#endif
