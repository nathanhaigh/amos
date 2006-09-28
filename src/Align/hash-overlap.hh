//  A. L. Delcher
//
//  File:  hash-overlap.hh
//
//  Last Modified:  Tue May 25 09:45:25 EDT 2004
//                  Thu Jul  1 17:29:28 EDT 2004 (amp)
//                  Tue Aug 24 15:30:52 EDT 2004 (amp)
//
//  Declarations for  simple-overlap.cc


#ifndef  __HASH_OVERLAP_HH_INCLUDED
#define  __HASH_OVERLAP_HH_INCLUDED

#include  "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "align.hh"
#include  "fasta.hh"
#include  <iostream>
#include  <vector>
#include  <string>
#include  <algorithm>

using namespace std;
using namespace AMOS;
using namespace HASHMAP;

const double  DEFAULT_ERROR_RATE = 0.06;
  // Default fraction of errors allowed in overlap alignment
const int  DEFAULT_MIN_OVERLAP_LEN = 40;
  // Default minimum number of bases by which two sequences must overlap
const int  ALIGNMENT_BAND_RADIUS = 5;
  // Half the width of the alignment band
const int  OFFSET_WIGGLE = 3;
  // Number of bases difference in offsets to be considered the same
const int  MAX_LINE = 1000;
const int  NEW_SIZE = 1000;


struct  Reference_t
  {
   int  string_num, pos;
  };


struct  Hash_Entry_t
  {
   int  ct;
   Reference_t  * ref;
  };


struct  Offset_Range_t
  {
   int  string_num, lo_offset, hi_offset;
  };


struct  Offset_Entry_t
  {
   int  ct;
   Offset_Range_t  * off;

   void  Add_Offset
       (int s, int offset);
  };


class  Minimizer_t
  {
  private:
   static const int  signature_len = 16;

   unsigned int  buff;
   unsigned int  * signature;
     // a priority queue of the lexicographic minimum bit patterns
     // in the window
   int  * delay;
     // how much additional offset each signature is from the best
   int  signature_ct;
     // the number of entries on the  signature  priority queue
   int  window_len;
     // the number of characters in  window
   int  window_offset;
     // the offset into window where the min signature pattern occurs
     // measured in characters
   unsigned int  signature_mask;
     // bit pattern that matches used part of  signature
     // in case it's shorter than a full unsigned int
   unsigned int  order_mask;
     // bit pattern to XOR with text bit pattern to create pattern
     // for lexicographic comparison

  public:
   Minimizer_t
       (int wl);     // Constructor
   ~ Minimizer_t
       ();           // Destructor

   unsigned int  Get_Signature
       (void)  const
     { return signature [0] ^ order_mask; }
   int  Get_Window_Offset
       (void)  const
     { return window_offset; }

   void  Advance
       (char ch);
   void  Dump
       (FILE * fp);
   void  Init
       (const char * s);
  };


static unsigned int  Bit_Pattern
    (char ch);
static int  By_String_Num_Then_Pos
    (const void * a, const void * b);
static void  Check_IDs
    (void);
static void  Find_Fwd_Overlaps
    (const vector <char *> & string_list,
     hash_map <unsigned int, Hash_Entry_t> & ht,
     const vector <ID_t> & id_list, BankStream_t & overlap_bank);
static void  Find_Rev_Overlaps
    (vector <char *> & string_list,
     hash_map <unsigned int, Hash_Entry_t> & ht,
     const vector <ID_t> & id_list, BankStream_t & overlap_bank);
static void  Get_Strings_From_Bank
    (vector <char *> & s, vector <char *> & q,
     vector <Range_t> & clr_list, vector <ID_t> & id_list,
     vector <char *> & tag_list, BankStream_t & read_bank);
static void  Get_Strings_From_Bank_ByID
    (vector <char *> & s, vector <char *> & q,
     vector <Range_t> & clr_list, vector <ID_t> & id_list,
     vector <char *> & tag_list, Bank_t & read_bank, vector <ID_t> & sel_list);
static void  Get_Strings_From_Bank_ByEID
    (vector <char *> & s, vector <char *> & q,
     vector <Range_t> & clr_list, vector <ID_t> & id_list,
     vector <char *> & tag_list, Bank_t & read_bank, vector <string> & sel_list);
static void  Map_Minimizers
    (const vector <char *> & string_list,
     hash_map <unsigned int, Hash_Entry_t> & ht);
static void  Merge_Overlapping_Bands
    (Offset_Entry_t & oe, int rad);
static void  Output
    (ostream & os, BankStream_t & overlap_bank, const Simple_Overlap_t & olap);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Read_Fasta_Strings
    (vector <char *> & s, vector <ID_t> & id_list,
     vector <char *> & tag_list, const string & fn);
static void  Shift_In
    (unsigned int & u, char ch, unsigned int mask = UINT_MAX);
static void  Usage
    (const char * command);

#endif


