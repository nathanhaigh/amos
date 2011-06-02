//
//  File:  align.cc
//
//  Last Modified:  25 November 2002
//
//  Routines to do string alignments


#include  "align.hh"
#include  <stack>
#include  <cassert>
using namespace AMOS;
using namespace std;


// ###  Align_Score_Entry_t  methods  ###


void  Align_Score_Entry_t :: Dump
    (FILE * fp)  const

//  Print to  fp  the contents of this score entry.

  {
   fprintf (fp, "L = %d:%u  D = %d:%u  T = %d:%u\n",
        left_score, left_from, diag_score, diag_from,
        top_score, top_from);

   return;
  }



void  Align_Score_Entry_t :: Get_Max
    (int & max_score, unsigned int & max_from)  const

//  Set  max_score  to the highest score in this entry
//  and  max_from  to the sub_entry it came from.

  {
   if  (left_score <= diag_score)
       {
        max_score = diag_score;
        max_from = FROM_DIAG;
       }
     else
       {
        max_score = left_score;
        max_from = FROM_LEFT;
       }

   if  (max_score < top_score)
       {
        max_score = top_score;
        max_from = FROM_TOP;
       }

   return;
  }



void  Align_Score_Entry_t :: Get_Max_Left
    (int & max_score, unsigned int & max_from, int penalty)  const

//  Set  max_score  to the highest score in this entry
//  and  max_from  to the sub_entry it came from, where  penalty
//  is added to  max_score  if  max_from  is not  FROM_LEFT .

  {
   if  (top_score <= diag_score)
       {
        max_score = diag_score;
        max_from = FROM_DIAG;
       }
     else
       {
        max_score = top_score;
        max_from = FROM_TOP;
       }
   max_score += penalty;

   if  (max_score <= left_score)
       {
        max_score = left_score;
        max_from = FROM_LEFT;
       }

   return;
  }



void  Align_Score_Entry_t :: Get_Max_Top
    (int & max_score, unsigned int & max_from, int penalty)  const

//  Set  max_score  to the highest score in this entry
//  and  max_from  to the sub_entry it came from, where  penalty
//  is added to  max_score  if  max_from  is not  FROM_TOP .

  {
   if  (left_score <= diag_score)
       {
        max_score = diag_score;
        max_from = FROM_DIAG;
       }
     else
       {
        max_score = left_score;
        max_from = FROM_LEFT;
       }
   max_score += penalty;

   if  (max_score < top_score)
       {
        max_score = top_score;
        max_from = FROM_TOP;
       }

   return;
  }



void  Augmented_Score_Entry_t :: Dump
    (FILE * fp)  const

//  Print to  fp  the contents of this score entry.

  {
   Align_Score_Entry_t :: Dump (fp);
   fprintf (fp, "Refs:  %d  %d  %d\n", left_ref, diag_ref, top_ref);

   return;
  }



int  Augmented_Score_Entry_t :: Get_Ref
    (unsigned int & from)  const

//  Return the  ref  entry corresponding to  from .

  {
   switch  (from)
     {
      case  FROM_TOP :
        return  top_ref;
      case  FROM_DIAG :
        return  diag_ref;
      case  FROM_LEFT :
        return  left_ref;
      default :
        sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad from value = %d in  Get_Ref\n",
                 from);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
     }

   return  -1;
  }



void  Errors_Score_Entry_t :: Dump
    (FILE * fp)  const

//  Print to  fp  the contents of this score entry.

  {
   Augmented_Score_Entry_t :: Dump (fp);
   fprintf (fp, "Errors:  %d  %d  %d\n", left_errors, diag_errors, top_errors);

   return;
  }



int  Errors_Score_Entry_t :: Get_Errors
    (unsigned int & from)  const

//  Return the  ref  entry corresponding to  from .

  {
   switch  (from)
     {
      case  FROM_TOP :
        return  top_errors;
      case  FROM_DIAG :
        return  diag_errors;
      case  FROM_LEFT :
        return  left_errors;
      default :
        sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad from value = %d in  Get_Errors\n",
                 from);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
     }

   return  -1;
  }




// ###  Distinguishing_Column_t  methods  ###


bool  Distinguishing_Column_t :: Intersects
    (const Distinguishing_Column_t & d, Phase_Entry_t & p,
     int & min)

//  See if any of the subscripts in this  column  match
//  those in  d .  If not, return  false .  Otherwise, return
//   true  and set the counts in  p  for how many match a/a and b/b,
//  and how many match a/b and b/a.  Set  min  to the smaller
//  number of component matches in the larger of  same_ct  and
//   opposite_ct .

  {
   int  aa, ab, ba, bb;

   aa = Match_Count (hapa_sub, d . hapa_sub);
   bb = Match_Count (hapb_sub, d . hapb_sub);

   ab = Match_Count (hapa_sub, d . hapb_sub);
   ba = Match_Count (hapb_sub, d . hapa_sub);

   p . same_ct = aa + bb;
   p . opposite_ct = ab + ba;

   if  (p . same_ct >= p . opposite_ct)
       min = Min (aa, bb);
     else
       min = Min (ab, ba);

   return  (p . same_ct > 0 || p . opposite_ct > 0);
  }




// ###  Vote_t  methods  ###


void  Vote_t :: Incr_After
    (char ch, int incr_val)

//  Add  incr_val  to the  after  count in this  Vote_t  for  ch .

  {
   const char  * p;

   p = strchr (ALPHABET, tolower (ch));
   if  (p == NULL)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad character %c (ASCII %d)\n",
                 ch, ch);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   after [p - ALPHABET] += incr_val;

   return;
  }



void  Vote_t :: Incr_Blank
    (int incr_val)

//  Add  incr_val  to the blank entry in the  here  counts of this  Vote_t .

  {
    here [ALPHABET_SIZE] += incr_val;

    return;
  }



void  Vote_t :: Incr_By
    (char ch, bool with_blank, int incr_val)

//  Add  incr_val  to the  here  count in this  Vote_t  for  ch .
//  If  with_blank  is true then also add  incr_val  to the after
//  count for a blank.

  {
   const char  * p;

   p = strchr (ALPHABET, tolower (ch));
   if  (p == NULL)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad character %c (ASCII %d)\n",
                 ch, ch);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   here [p - ALPHABET] += incr_val;
   if (with_blank)
     after [ALPHABET_SIZE] += incr_val;

   return;
  }



char  Vote_t :: Max_After_Char
    (void)

//  Return the character corresponding to the maximum count
//  in  here .  If all zeroes, return a blank.

  {
   int  i, max;

   max = 0;
   for  (i = 1;  i <= ALPHABET_SIZE;  i ++)
     if  (after [max] < after [i])
         max = i;

   if  (after [max] == 0 || max == ALPHABET_SIZE)
       return  ' ';
     else
       return  ALPHABET [max];
  }



char  Vote_t :: Max_Here_Char
    (void)

//  Return the character corresponding to the maximum count
//  in  after .  If all zeroes, return a blank.

  {
   int  i, max;

   max = 0;
   for  (i = 1;  i <= ALPHABET_SIZE;  i ++)
     if  (here [max] < here [i])
         max = i;

   if  (here [max] == 0 || max == ALPHABET_SIZE)
       return  ' ';
     else
       return  ALPHABET [max];
  }



void  Vote_t :: Set_To
    (char ch, bool with_blank)

//  Make the counts in this  Vote_t  all zero except set the
//   here  count for  ch  to  1 .  If  with_blank  is true then
//  also set the  after  count for a blank to  1 .

  {
   int  i;
   const char * p;

   for  (i = 0;  i <= ALPHABET_SIZE;  i ++)
     here [i] = after [i] = 0;

   p = strchr (ALPHABET, tolower (ch));
   if  (p == NULL)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad character %c (ASCII %d)\n",
                 ch, ch);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   here [p - ALPHABET] = 1;
   if  (with_blank)
       after [ALPHABET_SIZE] = 1;

   return;
  }



void  Vote_t :: Set_Zero
    (void)

//  Make the counts in this  Vote_t  all zero.

  {
   int  i;

   for  (i = 0;  i <= ALPHABET_SIZE;  i ++)
     here [i] = after [i] = 0;

   return;
  }




// ###  Base_Alignment_t  methods  ###


void  Base_Alignment_t :: Dump
    (FILE * fp)  const

//  Print to  fp  the contents of this base alignent.

  {
   fprintf (fp, "### a_lo/hi = %d/%d  b_lo/hi = %d/%d  errors = %d  string_sub = %d\n",
            a_lo, a_hi, b_lo, b_hi, errors, string_sub);

   return;
  }



double  Base_Alignment_t :: Error_Rate
    (void)  const

//  Return the number of errors divided by the average
//  length of this alignment.

  {
   double  len;

   len = (a_hi - a_lo + b_hi - b_lo) / 2.0;
   if  (len == 0)
       return  0.0;
     else
       return  errors / len;
  }



void  Base_Alignment_t :: Flip_AB
    (void)

//  Switch the roles of  a  and  b .  No change to  errors ,  string_sub,
//   flipped  or  a_len .

  {
   Swap (a_lo, b_lo);
   Swap (a_hi, b_hi);
  }



bool  Base_Alignment_t :: Is_Empty
  (void)  const

  // Return  true  iff  a_lo/hi  and  b_lo/hi  are all 0

{
  return (a_lo == 0 && a_hi == 0 && b_lo == 0 && b_hi == 0);
}




// ###  Alignment_t  methods  ###


int  Alignment_t :: B_Position
    (int a)  const

//  Return the postion in the b string that corresponds to
//  position  a  in the a string.  Must have  a_lo <= a < a_hi .
//  In case of insertions in the a string, return the lowest
//  position in the b string that corresponds.

  {
   int  d, i, j, n;

   i = a_lo;
   j = b_lo;
   n = delta . size ();

   for  (d = 0;  d < n;  d ++)
     {
      int  extent;

      extent = abs (delta [d]) - 1;
      if  (a - i <= extent)
          return  j + a - i;
      i += extent;
      j += extent;
      if  (delta [d] < 0)
          i ++;
        else
          j ++;
     }

   return  j + a - i;
  }



void  Alignment_t :: Clear
    (void)

//  Make this alignment empty.

  {
   a_lo = a_hi = b_lo = b_hi = errors = 0;
   delta . clear ();

   return;
  }



void  Alignment_t :: Combine
    (const Alignment_t & a1, const Alignment_t & a2)

//  Concatenate alignments  a1  and  a2  into this single alignment.

  {
   int  a_used, b_used, extent;
   int  a_left, b_left;
   int  i, n;

   if  (Verbose > 3)
       {
        cerr << "Combine:" << endl;
        a1 . Dump (stderr);
        a2 . Dump (stderr);
       }

   assert (a1 . a_hi == a2 . a_lo);
   assert (a1 . b_hi == a2 . b_lo);

   delta = a1 . delta;
   a_lo = a1 . a_lo;
   a_hi = a2 . a_hi;
   b_lo = a1 . b_lo;
   b_hi = a2 . b_hi;
   errors = a1 . errors + a2 . errors;
   string_sub = a1 . string_sub;
   flipped = a1 . flipped;
   a_len = a1 . a_len;

   if  (a2 . delta . size () == 0)
       return;

   // Compute the number of characters in the  a  and  b  strings
   // accounted for by the delta values in  a1 .
   n = a1 . delta . size ();
   a_used = b_used = 0;
   for  (i = 0;  i < n;  i ++)
     {
      extent = abs (a1 . delta [i]) - 1;
      a_used += extent;
      b_used += extent;
      if  (delta [i] < 0)
          a_used ++;
        else
          b_used ++;
     }

   a_left = a1 . a_hi - a1 . a_lo - a_used;
   b_left = a1 . b_hi - a1 . b_lo - b_used;
   assert (a_left == b_left);

   if  (a2 . delta [0] < 0)
       delta . push_back (a2 . delta [0] - a_left);
     else
       delta . push_back (a2 . delta [0] + a_left);

   n = a2 . delta . size ();
   for  (i = 1;  i < n;  i ++)
     delta . push_back (a2 . delta [i]);

   return;
  }



void  Alignment_t :: Dump
    (FILE * fp)  const

//  Print to  fp  the entries of this alignment.

  {
   Base_Alignment_t :: Dump (fp);
   fprintf (fp, "Delta:\n");
   Dump_Delta (fp);

   return;
  }



void  Alignment_t :: Dump_Delta
    (FILE * fp)  const

//  Print to  fp  the  delta  entries of this alignment.

  {
   int  i, n;

   n = delta . size ();
   for  (i = 0;  i < n;  i ++)
     fprintf (fp, "  %3d: %5d\n", i, delta [i]);

   return;
  }



void  Alignment_t :: Check_Fix_Start
    (const char * s , int s_len, const char * t, int t_len,
     Fix_Status_t & status)

//  Check for errors/indels at the start of this alignment.
//  If there aren't any, set  status  to  NO_FIX_NEEDED  and return.
//  If there are gaps in  s  then shift the alignment to the
//  right and set  status to  SHIFTED_RIGHT .  If there are
//  gaps in  t  or suspiciously many errors, then set  status  to
//   NEEDS_LEFT_SHIFT .
//  The alignment aligns  s  as the a-string to  t  as the b-string.

  {
   int  i, j, n, shift, indels, matches;

   n = delta . size ();

   // if there are no indels or there isn't one at the beginning
   // of the alignment, there's nothing to do
   // This is oversimplified.  There may be a match for the first
   // character or two, but then too many indels, which would
   // indicate a shift is needed

   if  (n == 0 || abs (delta [0]) > 1)
       {
        status = NO_FIX_NEEDED;
        return;
       }

   if  (delta [0] == 1)
       {  // shift the alignment to the right;

        for  (j = 1;  j < n && delta [j] == 1;  j ++)
          ;
        // There are  j  consecutive  1 's at the start.
        // Just remove them and shift the alignment  j  positions
        // to the right

        shift = j;
        for  (i = 0;  j < n;  i++, j++)
          delta [i] = delta [j];
        delta . resize (n - shift);
        b_lo += shift;
        if (shift <= errors)
          errors -= shift;   // errors is generally not used and set to zero

        status = SHIFTED_RIGHT;
        return;
       }

   if (delta [0] == -1)
     {  // first count how many -1's are at the start
       for (j = 1; j < n && delta [j] == -1; j ++)
         ;
       if (j <= b_lo)
         {  // count the number of matches
           indels = j;
           i = b_lo - indels;
           matches = 0;
           for (j = 0; j < indels; j ++)
             if (s [a_lo + j] == t [i + j])
               matches ++;

           if (indels - 1 <= matches)
             {  // all but one character matches so can just shift the start
               shift = j = indels;
               for (i = 0; j < n; i ++, j ++)
                 delta [i] = delta [j];
               delta . resize (n - shift);
               b_lo -= shift;
               if (indels - matches <= errors)
                 errors -= indels - matches;
               // errors is generally not used and set to zero

               status = SHIFTED_LEFT;
               return;
             }
         }
       status = NEEDS_LEFT_SHIFT;
       // needs a more complicated shift
       return;
     }

   return;
  }



void  Alignment_t :: Flip_AB
    (void)

//  Switch the roles of  a  and  b  in this alignment.

  {
   int  i, n;

   Base_Alignment_t :: Flip_AB ();

   n = delta . size ();
   for  (i = 0;  i < n;  i ++)
     delta [i] *= -1;

   return;
  }



void  Alignment_t :: Incr_Votes
    (vector <Vote_t> & vote, char * a, int incr_val)

// Use the entries in the alignment to increment the entries in  vote
// that correspond to the  b  string in the alignment.
// Vote values are from the string  a .  Value to increment by
// is  incr_val .

  {
   int  d, i, j, k, n;


   i = a_lo;
   j = b_lo;
   n = delta . size ();

   // First do the entries in the delta encoding
   for  (d = 0;  d < n;  d ++)
     {
      int  extent;

      if  (Verbose > 3)
          fprintf (stderr, "d = %d  delta = %d\n", d, delta [d]);

      extent = abs (delta [d]) - 1;

      for  (k = 0;  k < extent;  k ++)
        {
         vote [j] . Incr_By (a [i], (k < extent - 1), incr_val);
         i ++;
         j ++;
        }

      if  (delta [d] < 0)
          {
           if  (extent > 0)
               vote [j - 1] . Incr_After (a [i], incr_val);
           i ++;
          }
        else
          {
           vote [j] . Incr_Blank (incr_val);
           j ++;
          }
     }

   if  (Verbose > 3)
       {
        fprintf (stderr, "i = %d  a_hi = %d  j = %d  b_hi = %d\n",
                 i, a_hi, j, b_hi);
       }

   // Now finish off what's left
   while  (i < a_hi && j < b_hi)
     {
      vote [j] . Incr_By (a [i], (j < b_hi - 1), incr_val);
      i ++;
      j ++;
     }

   if  (i != a_hi)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "ERROR:  Bad alignment end  i = %d  a_hi = %d", i, a_hi);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }
   if  (j != b_hi)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "ERROR:  Bad alignment end  j = %d  b_hi = %d", j, b_hi);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   return;
  }



void  Alignment_t :: Offset_A
    (int n)

//  Add  n  to the  a_lo  and  a_hi  positions of this alignment.

  {
   a_lo += n;
   a_hi += n;

   return;
  }



void  Alignment_t :: Print
    (FILE * fp, const char * a, const char * b, int width)

//  Show this alignment between strings  a  and  b
//  showing just the alignment portion of the two strings,
//  with index positions in the right-hand margin.
//  Use  width  characters per line for alignment.
//  Output goes to  fp .

  {
   string  s (width, ' '), t (width, ' ');
   int  a_begin, b_begin;
   int  d, i, j, k, n, q;


   i = a_lo;
   j = b_lo;
   a_begin = a_lo;
   b_begin = b_lo;
   q = 0;
   n = delta . size ();

   // First do the entries in the delta encoding
   for  (d = 0;  d < n;  d ++)
     {
      int  extent;

      if  (Verbose > 3)
          fprintf (stderr, "d = %d  delta = %d\n", d, delta [d]);

      extent = abs (delta [d]) - 1;

      for  (k = 0;  k < extent;  k ++)
        {
         s [q] = a [i ++];
         t [q ++] = b [j ++];
         if  (q == width)
             {
              Print_Align_Lines_Pair
                  (fp, s, t, q, "a:", "b:", a_begin, i, b_begin, j);
              a_begin = i;
              b_begin = j;
              q = 0;
             }
        }

      if  (delta [d] < 0)
          {
           s [q] = a [i ++];
           t [q ++] = '-';
           if  (q == width)
               {
                Print_Align_Lines_Pair
                    (fp, s, t, q, "a:", "b:", a_begin, i, b_begin, j);
                a_begin = i;
                b_begin = j;
                q = 0;
               }
          }
        else
          {
           s [q] = '-';
           t [q ++] = b [j ++];
           if  (q == width)
               {
                Print_Align_Lines_Pair
                    (fp, s, t, q, "a:", "b:", a_begin, i, b_begin, j);
                a_begin = i;
                b_begin = j;
                q = 0;
               }
          }
     }

   if  (Verbose > 1)
       {
        fprintf (stderr, "i = %d  a_hi = %d  j = %d  b_hi = %d\n",
                 i, a_hi, j, b_hi);
       }

   // Now finish off what's left
   while  (i < a_hi && j < b_hi)
     {
      s [q] = a [i ++];
      t [q ++] = b [j ++];
      if  (q == width)
          {
           Print_Align_Lines_Pair
               (fp, s, t, q, "a:", "b:", a_begin, i, b_begin, j);
           a_begin = i;
           b_begin = j;
           q = 0;
          }
     }

   if  (q > 0)
       Print_Align_Lines_Pair
           (fp, s, t, q, "a:", "b:", a_begin, i, b_begin, j);

   if  (i != a_hi)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "ERROR:  Bad alignment end  i = %d  a_hi = %d", i, a_hi);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }
   if  (j != b_hi)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "ERROR:  Bad alignment end  j = %d  b_hi = %d", j, b_hi);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   return;
  }



void  Alignment_t :: Set_Empty_A
    (int a_start, int b_start, int b_end)

//  Make this an alignment with an empty  a  string at position
//   a_start  against the portion of  b  string between  b_start
//  and  b_end .

  {
   int  i;

   a_lo = a_hi = a_start;
   b_lo = b_start;
   b_hi = b_end;
   errors = b_hi - b_lo;
   delta . clear ();
   for  (i = b_lo;  i < b_hi;  i ++)
     delta . push_back (1);

   return;
  }



void  Alignment_t :: Set_Exact
    (int a_start, int b_start, int len)

//  Make this alignment represent an exact match starting
//  at  a_start  in string  a  and  b_start  in string  b  and
//  extending for length  len .

  {
   a_lo = a_start;
   a_hi = a_start + len;
   b_lo = b_start;
   b_hi = b_start + len;
   errors = 0;
   delta . clear ();

   return;
  }



void  Alignment_t :: Set_From_VS_Matrix
    (Match_Extent_Entry_t * * t, int e, int j)

//  Make this alignment represent the match in  t  starting
//  at entry  t [e] [j]  and working back to  t [0] [0] .
//   t  is the matrix of the Vishkin-Schieber algorithm in which
//   t [e] [j]  is the most positions of string  a
//  that can be matched with  e  errors and ending aligned at
//  offset  j  in string  b .

  {
   stack <int>  s;
   int  end_match;
   
   a_lo = 0;
   a_hi = t [e] [j] . len;
   b_hi = a_hi + j;
   errors = e;

   delta . clear ();

   // First get the reverse encoding (i.e., reading strings a and b
   // from right to left) and push it onto a stack

   end_match = t [e] [j] . len;
   for  ( ;  e > 0;  e --)
     switch  (t [e] [j] . from)
       {
        case  MATCH_FROM_TOP :   // no delta entry needed because
                                 // no shift in diagonal
          break;

        case  MATCH_FROM_NW :
          j --;                  // extra character in string  b
          s . push (1 + (end_match - t [e - 1] [j] . len));
          end_match = t [e - 1] [j] . len;
          break;

        case  MATCH_FROM_NE :
          j ++;                  // extra character in string  a
          s . push (- (end_match - t [e - 1] [j] . len));
          end_match = t [e - 1] [j] . len;
          break;

        default :
          sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad switch value  from = %d\n",
                   t [e] [j] . from);
          Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   b_lo = a_lo + j;

   // Now convert reverse encoding to forward encoding by popping
   // values off the stack

   end_match ++;
   while  (! s . empty ())
     {
      int  k;

      k = s . top ();
      if  (Verbose > 1)
          fprintf (stderr, "stack top = %d  end_match = %d\n", k, end_match);
      s . pop ();
      if  (k < 0)
          delta . push_back (- end_match);
        else
          delta . push_back (+ end_match);

      end_match = abs (k);
     }

   return;
  }



void  Alignment_t :: Set_Inserts
    (vector <short> & v)  const

//  Make sure the values in  v  are large enough to hold the number
//  of inserts specified in this alignment.

  {
   int  ct;
   int  d, j, n;

   j = b_lo;
   n = delta . size ();
   ct = 0;

   for  (d = 0;  d < n;  d ++)
     {
      j += abs (delta [d]) - 1;

      if  (delta [d] != -1)
          ct = 0;
      if  (delta [d] < 0)
          {
           ct ++;
           if  (ct > v [j])
               v [j] = ct;
          }
        else
          j ++;
     }

   return;
  }



void  Alignment_t :: Set_To_Identity
    (int len)

//  Set this alignment to represent an identity between
//  two strings of length  len .

  {
   a_lo = b_lo = 0;
   a_hi = b_hi = len;
   errors = 0;
   a_len = len;

   delta . clear ();

   return;
  }



void  Alignment_t :: Shift_First_Delta
    (int offset)

//  Adjust the first delta value (if any) in this alignment
//  by  offset .

  {
   if  (delta . size () > 0)
       {
        if  (delta [0] > 0)
            delta [0] += offset;
          else
            delta [0] -= offset;
       }

   return;
  }




// ###  Gapped_Alignment_t  methods  ###


void  Gapped_Alignment_t :: Clear
    (void)

//  Make this alignment empty.

  {
   a_lo = a_hi = b_lo = b_hi = errors = 0;
   skip . clear ();

   return;
  }



void  Gapped_Alignment_t :: Convert_From
    (const Alignment_t & ali, vector <int> & tr)

//  Convert the alignment in  ali  (which represents
//  indels w.r.t. an ungapped sequence) to one
//  representing positions w.r.t. a consensus
//  with gaps inserted.  Thus the delta will be
//  a sequence of positive numbers indicating
//  the positions in the b string that have no
//  match to the a string.
//   tr  is a translation vector that maps the ungapped
//  positions in the b string to the positions after
//  gaps are inserted.

  {
   int  ct;
   int  d, i, j, k, n, p, q, r;

   i = a_lo = ali . a_lo;
   j = ali . b_lo;
   a_hi = ali . a_hi;
   p = b_lo = tr [ali . b_lo];
   errors = ali . errors;
   flipped = ali . flipped;
   a_len = ali . a_len;

   skip . clear ();

   n = ali . delta . size ();
   ct = 0;

   if  (Verbose > 2)
       {
        int  i;

        fprintf (stderr, "Convert_From:  a_lo = %d  a_hi = %d\n", a_lo, a_hi);
        fprintf (stderr, "  Delta:\n");
        for  (i = 0;  i < n;  i ++)
          fprintf (stderr, "  %3d:  %5d\n", i, ali . delta [i]);
        for  (i = ali . b_lo;  i < ali . b_hi;  i ++)
          fprintf (stderr, "  tr [%3d] = %5hd\n", i, tr [i]);
       }

   // adjust b_lo if alignment begins with insertions
   for  (d = 0;  d < n && ali . delta [d] == -1;  d ++)
     {
      i ++;
      b_lo --;
     }
   if  (b_lo < 0)
//##ALD   if  (d > 0 && (j == 0 || tr [j] - d <= tr [j - 1]))
       {
        sprintf (Clean_Exit_Msg_Line,
                 "ERROR:  Convert_From had too many initial inserts\n"
                 "  d = %d  j = %d  tr [j] = %d  tr [j - 1] = %d\n",
                 d, j, tr [j], j > 0 ? tr [j - 1] : 0);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   for  ( ;  d < n;  d ++)
     {
      k = abs (ali . delta [d]) - 1;
      for  (q = 0;  q < k;  q ++)
        {
         if  (j > ali . b_lo)
             for  (r = tr [j - 1] + 1 + ct;  r < tr [j];  r ++)
               {
                skip . push_back (p);
                p ++;
               }
         i ++;
         j ++;
         p ++;
         ct = 0;
        }

      if  (ali . delta [d] != -1)
          ct = 0;
      if  (ali . delta [d] < 0)
          {
           ct ++;
           i ++;
           p ++;
          }
        else
          {
           for  (q = tr [j - 1];  q < tr [j];  q ++)
             {
              skip . push_back (p);
              p ++;
             }
           j ++;
          }
     }

   // Now finish off the remaining indel-free tail
   while  (i < a_hi && j < ali . b_hi)
     {
      if  (j > ali . b_lo)
          for  (q = tr [j - 1] + 1 + ct;  q < tr [j];  q ++)
            {
             skip . push_back (p);
             p ++;
            }
      i ++;
      j ++;
      p ++;
      ct = 0;
     }

   b_hi = p;

   if  (Verbose > 2)
       {
        fprintf (stderr, "Convert_From:  a_lo = %d  a_hi = %d\n", a_lo, a_hi);
        fprintf (stderr, "  b_lo = %d  b_hi = %d\n", b_lo, b_hi);
        fprintf (stderr, "  Skip:\n");
        Dump_Skip (stderr);
       }

   return;
  }



void  Gapped_Alignment_t :: Convert_Skip_to_Del
    (vector <int> & del)

//  Set  del  to the entries in an IMP submessage that correspond
//  to the skip values in this alignment.

  {
   int  i, n;

   del . clear ();

   n = skip . size ();
   for  (i = 0;  i < n;  i ++)
     del . push_back (a_lo + skip [i] - b_lo - i);

   return;
  }



void  Gapped_Alignment_t :: Dump
    (FILE * fp)  const

//  Print to  fp  the entries of this alignment.

  {
   Base_Alignment_t :: Dump (fp);
   fprintf (fp, "Skip:\n");
   Dump_Skip (fp);

   return;
  }



void  Gapped_Alignment_t :: Dump_Skip
    (FILE * fp)  const

//  Print to  fp  the  skip  entries of this alignment.

  {
   int  i, n;

   n = skip . size ();
   for  (i = 0;  i < n;  i ++)
     fprintf (fp, "  %3d: %5d\n", i, skip [i]);

   return;
  }



void  Gapped_Alignment_t :: Flip
    (int a_length, int b_length)

//  Change this alignment to reflect that both strings
//  have been reversed.   a_length  is the length of the
//  a string (i.e., that a coordinates refer to) and
//  b_length is the length of the b string.

  {
   int  i, n, tmp;

   flipped = 1 - flipped;

   tmp = a_length - a_lo;
   a_lo = a_length - a_hi;
   a_hi = tmp;

   tmp = b_length - b_lo;
   b_lo = b_length - b_hi;
   b_hi = tmp;

   reverse (skip . begin (), skip . end ());
   n = skip . size ();
   for  (i = 0;  i < n;  i ++)
     skip [i] = b_length - 1 - skip [i];

   return;
  }



char  Gapped_Alignment_t :: Get_Aligning_Char
    (int b, char * s)

//  Return the character in string  s  that aligns to
//  consensus position  b .

  {
   int  d, i, n;

   assert (b_lo <= b && b < b_hi);

   n = skip . size ();
   for  (d = 0;  d < n && skip [d] < b;  d ++)
     ;

   i = a_lo + b - b_lo - d;
   if  (d < n && b == skip [d])
       return '-';
     else
       return  s [i];
  }



int  Gapped_Alignment_t :: Get_Skip
    (int i)  const

//  Return the skip value associated with subscript  i , if there
//  is one; otherwise, return  INT_MAX .

  {
   int  n;

   n = skip . size ();
   if  (i < n)
       return  skip [i];
     else
       return  INT_MAX;
  }



void  Gapped_Alignment_t :: Incr_Column_Chars
    (Matrix <unsigned char> & count, const char * s)

//  Increment entries in  count  corresponding to this alignment
//  of  s  to the consensus string.

  {
   int  skip_ct = skip . size ();
   int  a, b, d, j;

   a = a_lo;
   d = 0;
   for  (b = b_lo;  b < b_hi;  b ++)
     {
      if  (d < skip_ct && b == skip [d])
          {
           Incr_Limited (count . get (b, 4), (unsigned char) UCHAR_MAX);   // '-'
           d ++;
          }
        else
          {
           j = DNA_Char_To_Sub (s [a]);
           if  (j >= 0)
               Incr_Limited (count . get (b, j), (unsigned char) UCHAR_MAX);
           a ++;
          }
     }

   return;
  }



void  Gapped_Alignment_t :: Make_Sub_Alignment
    (Gapped_Alignment_t & sub_ali, int b1, int b2)  const

//  Set  sub_ali  to the portion of this alignment that intersects
//   b1 .. b2  of the b sequence.

  {
   int  d, n;

   sub_ali . Clear ();

   n = skip . size ();
   d = 0;
   if  (b1 <= b_lo)
       {
        sub_ali . b_lo = b_lo;
        sub_ali . a_lo = a_lo;
       }
     else
       {
        while  (d < n && skip [d] < b1)
          d ++;
        sub_ali . b_lo = b1;
        sub_ali . a_lo = a_lo + b1 - b_lo - d;
       }

   sub_ali . b_hi = Min (b2, b_hi);

   for  ( ;  d < n && skip [d] <= sub_ali . b_hi;  d ++)
     sub_ali . skip . push_back (skip [d]);

   sub_ali . a_hi = a_lo + sub_ali . b_hi - b_lo - d;

   sub_ali . string_sub = string_sub;
   sub_ali . flipped = flipped;
   sub_ali . a_len = a_len;

   assert (sub_ali . a_lo < sub_ali . a_hi);
   assert (sub_ali . b_lo <= sub_ali . b_hi);

   return;
  }



void  Gapped_Alignment_t :: Modify_For_B_Inserts
    (const vector <int> & insert_sub)

//  Change this alignment to reflect additional '-'s in the
//  consensus sequence at positions in  insert_sub .

  {
   vector <int> :: iterator  p;
   vector <int>  new_skip;
   int  i, n;

   if  (Verbose > 2)
       {
        n = insert_sub . size ();
        printf ("insert_sub  n = %d\n", n);
        for  (i = 0;  i < n;  i ++)
          printf ("%3d: %5d\n", i, insert_sub [i]);

        printf ("Before Modify_For_B_Inserts:\n");
        Dump (stdout);
       }

   n = insert_sub . size ();
   for  (i = 0;  i < n && insert_sub [i] < b_lo;  i ++)
     b_lo ++;
   // Note:  can't wait till end of loop to increment b_lo by
   //   i because loop may end prematurely

   p = skip . begin ();
   while  (p != skip . end () && i < n)
     {
      if  (insert_sub [i] <= (* p) + i)
          {
           new_skip . push_back (insert_sub [i]);
           i ++;
          }
        else
          {
           new_skip . push_back ((* p) + i);
           p ++;
          }
     }

   for  ( ;  p != skip . end ();  p ++)
     new_skip . push_back ((* p) + i);

   b_hi += i;

   while  (i < n && insert_sub [i] < b_hi)
     {
      new_skip . push_back (insert_sub [i]);
      i ++;
      b_hi ++;
     }

   skip = new_skip;

   if  (Verbose > 2)
       {
        printf ("After Modify_For_B_Inserts:\n");
        Dump (stdout);
       }

   return;
  }



void  Gapped_Alignment_t :: Pass_Through
    (const Alignment_t & ali)

//  Convert this alignment to refer to the b sequence in  ali ,
//  which maps this alignment's consensus (as its a sequence)
//  to another sequence.  Assume that the other sequence has
//  had gaps inserted where needed.

  {
   vector <int>  new_skip;
   bool  done_b_lo = false;
   int  orig_b_lo;
   int  prev_new_skip = -1, skip_len;
   int  d, i, j, k, n;

   if  (Verbose > 2)
       {
        printf ("ali . a_lo/hi = %d/%d  ali . b_lo/hi = %d/%d\n",
             ali . a_lo, ali . a_hi, ali . b_lo, ali . b_hi);
        printf ("a_lo/hi = %d/%d  b_lo/hi = %d/%d\n",
             a_lo, a_hi, b_lo, b_hi);
       }
   assert (ali . a_lo <= b_lo);
   assert (b_hi <= ali . a_hi);

   orig_b_lo = b_lo;
   i = ali . a_lo;
   j = ali . b_lo;
   n = ali . delta . size ();
   skip_len = skip . size ();
   for  (d = k = 0;  d < int (ali . delta . size ()) && i < b_hi;  d ++)
     {
      i += abs (ali . delta [d]) - 1;
      j += abs (ali . delta [d]) - 1;
      if  (! done_b_lo && b_lo <= i)
          {
           b_lo += j - i;
           done_b_lo = true;
          }
      while  (k < skip_len && skip [k] <= i)
        {
         if  (Verbose > 2)
              printf ("k = %d  i = %d  j = %d  old_skip = %d  new_skip = %d\n",
                   k, i, j, skip [k], skip [k] + j - i);
         new_skip . push_back (skip [k] + j - i);
         prev_new_skip = skip [k] + j - i;
         k ++;
        }
      if  (ali . delta [d] < 0)
          {
           i ++;
           j ++;
           // Advance both because we assume the "j" sequence has
           // had gaps inserted
          }
      else if  (i >= b_hi)
           break;
         else
          {
           if  (orig_b_lo <= i)
               {
                if  (Verbose > 2)
                    printf ("k = %d  i = %d  j = %d  b_lo = %d  b_hi = %d"

                            "  adding new_skip = %d\n", k, i, j, b_lo, b_hi, j);
                if  (j == prev_new_skip)
                    new_skip . push_back (j + 1);
                  else
                    new_skip . push_back (j);
               }
           j ++;
          }
     }

   while  (k < skip_len)
     {
      if  (Verbose > 2)
          printf ("k = %d  i = %d  j = %d  old_skip = %d  new_skip = %d\n",
               k, i, j, skip [k], skip [k] + j - i);
      new_skip . push_back (skip [k] + j - i);
      k ++;
     }

   if  (! done_b_lo)
       b_lo += j - i;
   b_hi += j - i;
   skip = new_skip;

   return;
  }



void  Gapped_Alignment_t :: Print_Subalignment_Line
    (char * buff, int b1, int b2, char * s, int & a1, int & a2)

//  Write into  buff  the portion of this alignment of string  s
//  that lies over subrange  b1 .. b2  of a gapped consensus string.
//  Set  a1  and  a2  to the range of this sequence that corresponds
//  to what is printed.

  {
   int  ct, limit;
   int  d, i, n;

   ct = 0;   // keep track of what's been written into  buff
   a1 = a2 = -1;  // indicate no match unless changed
   
   // fill buff with blanks to get to the start of characters
   limit = Min (b_lo, b2);
   while  (b1 < limit)
     {
      buff [ct ++] = ' ';
      b1 ++;
     }

   if  (b1 == b2)
       return;

   n = skip . size ();

   for  (d = 0;  d < n && skip [d] < b1;  d ++)
     ;
   
   i = a1 = a_lo + b1 - b_lo - d;

   limit = Min (b_hi, b2);
   while  (b1 < limit)
     {
      if  (d < n && b1 == skip [d])
          {
           buff [ct ++] = '-';
           d ++;
          }
        else
          buff [ct ++] = s [i ++];
      b1 ++;
     }

   a2 = i;

   // pad with spaces if alignment ends before b2
   while  (b1 < b2)
     {
      buff [ct ++] = ' ';
      b1 ++;
     }

   buff [ct] = '\0';

   return;
  }



void  Gapped_Alignment_t :: Shift_Skip
    (int offset)

//  Add  offset  to all the values in this alignment's  skip  vector.

  {
   int  i, n;

   n = skip . size ();
   for  (i = 0;  i < n;  i ++)
     skip [i] += offset;

   return;
  }




// ###  Gapped_MA_Bead_t  methods  ###


void  Gapped_MA_Bead_t :: Advance
    (void)

//  Set  seq_ch  and  qual_ch  to the next character in the
//  alignment referred to by this bead.  Also update the other
//  members to track the change.

  {
   b_pos ++;

   if  (b_pos == skip_pos)
       {
        seq_ch = '-';
        qual_ch = Min (qual [a_pos], qual [a_pos + 1]);
        skip_i ++;
        skip_pos = ga -> Get_Skip (skip_i);
       }
     else
       {
        a_pos ++;
        seq_ch = seq [a_pos];
        qual_ch = qual [a_pos];
       }

   if  (a_pos >= ga -> a_hi || b_pos >= ga -> b_hi)
       active = false;

   return;
  }




// ###  Multi_Alignment_t  methods  ###


bool  Multi_Alignment_t :: Check_Subsequent_Overlaps
  (const char * cons, int cons_len, const vector <char *> & s, int i, 
   const vector <int> & offset, int n, int curr_offset, int wiggle, int min_overlap,
   double erate, int offset_sum, int virtual_cons_len)

  // Check if one of the strings  s [i .. (n-1)]  might overlap
  // string  cons  (whose length is  cons_len ) based on their
  // relative offsets in  offset  w.r.t the current offset of string  i - 1
  // in  cons  of  curr_offset  and actual alignments.  Offset positions
  // are allowed  wiggle  characters in each direction and the minimum
  // overlap is  min_overlap  characters.   erate  is the allowed
  // error fraction in overlaps.  Overlap must extend  at
  // least as far as string  i - 1  would have.   offset_sum  is the
  // sum of entries in  offset [0 .. (i - 1)]  and  virtual_cons_len  is
  // the length of a perfect consensus using just the (non-expelled)
  // values in  offset  and the lengths of the strings to be multi-aligned.

{
  Alignment_t  ali;
  int  add_offset, min_olap, needed_extension;
  int  j, mid;

  needed_extension = curr_offset + strlen (s [i - 1]);
  // this may include an extension past the end of the consensus string
  // subsequent overlap must get at least this far in order to
  // safely expel string  i - 1

  min_olap = Min (cons_len, min_overlap);
  // Adjust in case the consensus is too short.  Don't adjust if a
  // subsequent string is too short, though.  It will be used if it
  // overlaps well enough in the main routine, but it shouldn't justify
  // getting rid of this string.

  mid = cons_len - min_olap;
  add_offset = curr_offset;

  for (j = i; j < n; j ++)
    {
      bool  matched;
      int  lo, hi, exp_olap_len, error_limit, len;

      len = strlen (s [j]);
      offset_sum += offset [i];

      add_offset += offset [j];
      if (cons_len - add_offset + wiggle < min_olap
          || virtual_cons_len <= offset_sum)
        // The first test is that string i might overlap the actual consensus.
        // The second is that the implied position of string i could be found in
        // the consensus in  Estimate_Offset  or  Estimate_Offset_With_Expels .
        // The second test is necessary when the actual offsets vary significantly
        // from the ones provided in  offset .
        return false;   // It's hopeless from here on since offsets only increase

      lo = Max (0, Min (add_offset - wiggle, mid));
      hi = Min (Max (lo + 1, mid), add_offset + wiggle);
      exp_olap_len = Min (cons_len - lo, len);

      error_limit = Binomial_Cutoff (exp_olap_len, erate, BIN_CUTOFF_PROB);
      matched = Overlap_Match_VS (s [j], len, cons, cons_len, lo, hi,
                                  0, error_limit, ali);

      if (matched && ali . Error_Rate () <= erate
          && needed_extension <= ali . b_hi + len - ali . a_hi)
        return true;
    }

  return false;
}



void  Multi_Alignment_t :: Clear
    (void)

//  Make this multi-alignment empty.

  {
   consensus . assign ("");
   align . clear ();

   return;
  }



int  Multi_Alignment_t :: Estimate_Offset_Position
    (int i, const vector <int> & offset)

//  Estimate the position at which  offset [i]  would be in the
//  consensus of this multialignment using the previous alignments
//  in  align [0 .. (i-1)] .  The values in  offset  represent the
//  leftmost position of each string relative to the preceding string.

  {
   int  j, off, len;

   off = offset [i];
   for  (j = i - 1;  j >= 0;  j --)
     {
      len = align [j] . a_hi - align [j] . a_lo;
      if  (off < len)
          return  align [j] . B_Position (off);

      off += offset [j];
     }

   if (Verbose > 0)
     {
       off = offset [i];
       fprintf (stderr, "%5s %5s %5s  %5s %5s\n", "j", "off", "len", "a_lo", "a_hi");
       for (j = i; j >= 0; j --)
         {
           len = align [j] . a_hi - align [j] . a_lo;
           fprintf (stderr, "%5d %5d %5d  %5d %5d\n", j, off, len, align [j] . a_lo,
                    align [j] . a_hi);
           off += offset [j];
         }
     }

   sprintf (Clean_Exit_Msg_Line,
        "ERROR:  Impossible  offset = %d  i = %d  in  Estimate_Offset_Position\n",
        offset [i], i);
   Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);

   return  -1;
  }



int  Multi_Alignment_t :: Estimate_Offset_With_Expels
    (int i, const vector <int> & offset, const vector <bool> & expel)

// Estimate the position at which  offset [i]  would be in the
// consensus of this multialignment using the previous alignments
// in  align [0 .. (i-1)] , but not any whose corresponding  expel  entry
// is true.  The values in  offset  represent the
// leftmost position of each string relative to the preceding string.

{
  int  j, off, len;

  off = offset [i];
  for (j = i - 1; j >= 0; j --)
    {
      if (! expel [j])
        {
          len = align [j] . a_hi - align [j] . a_lo;

          if  (off < len)
            return  align [j] . B_Position (off);
        }

      off += offset [j];
    }

   if (Verbose > 0)
     {
       off = offset [i];
       fprintf (stderr, "%5s %5s  %5s %5s  %5s %5s\n", "j", "expel", "off",
                "len", "a_lo", "a_hi");
       for (j = i; j >= 0; j --)
         {
           len = align [j] . a_hi - align [j] . a_lo;
           fprintf (stderr, "%5d %5s  %5d %5d  %5d %5d\n", j,
                    (expel [j] ? "yes" : "no"),
                    off, len, align [j] . a_lo, align [j] . a_hi);
           off += offset [j];
         }
     }

  sprintf (Clean_Exit_Msg_Line,
           "ERROR:  Impossible  offset = %d  i = %d  in  Estimate_Offset_With_Expels\n",
           offset [i], i);
  Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);

  return  -1;
}



void  Multi_Alignment_t :: Print_Alignments_To_Consensus
    (FILE * fp, vector <char *> s)

//  Show the individual alignments of the strings in  s  to
//  the consensus sequence of this alignment

  {
   const char  * con;
   int  i, n;

   n = align . size ();
   con = consensus . c_str ();

   fprintf (fp, "\nConsensus len = %d\n", consensus . length ());

   for  (i = 0;  i < n;  i ++)
     {
      fprintf (fp, "\nString #%d:\n", i);

      align [i] . Print (fp, s [i], con);
     }


   return;
  }



void  Multi_Alignment_t :: Reset_From_Votes
    (const vector <char *> & s,
     int offset_delta, double error_rate,
     vector <Vote_t> & vote, bool & changed)

//  Reset the consensus string in this multialignment from the
//  votes in  vote .  Redo the individual alignments of the
//  strings, which are in  s .  Allow  offset_delta  play either
//  direction at the start when re-doing the alignments and
//  assume an error-rate of  error_rate .  Save the votes of
//  the new alignments in  vote .  Set  changed  true  iff the new
//  consensus string differs from the previous one.

  {
   string  new_cons;
   char  * cons;
   int  adj, attempts, cons_len, wiggle;
   int  min_b_lo, max_b_hi;
   int  i, n;

   n = vote . size ();

   vector <short>  adjust (n + 1, 0);
   adj = 0;
       // Keep track of adjustments in positions caused by indels so
       // that when alignments are recalculated, we can start at
       // the correct place

   for  (i = 0;  i < n;  i ++)
     {
      char  ch;

      adjust [i] = adj;

      ch = vote [i] . Max_Here_Char ();
      if  (ch != ' ')
          new_cons . push_back (ch);
        else
          adj --;

      ch = vote [i] . Max_After_Char ();
      if  (ch != ' ')
          {
           new_cons . push_back (ch);
           adj ++;
          }
     }

   changed = (consensus != new_cons);
   if  (! changed)
       return;

   consensus = new_cons;
   cons = strdup (new_cons . c_str ());
   cons_len = new_cons . length ();

   vote . resize (cons_len);
   for  (i = 0;  i < cons_len;  i ++)
     vote [i] . Set_Zero ();

   n = s . size ();
   min_b_lo = INT_MAX;
   max_b_hi = 0;
   for  (i = 0;  i < n;  i ++)
     {
      bool  ok, needs_shift;
      int  error_limit, len, off;
      int  lo, hi;

      if (align [i] . Is_Empty ())
        {
          if (Verbose > 1)
            cerr << "In Reset_From_Votes skipping empty alignment " << i << endl;
          continue;
        }

      len = strlen (s [i]);
      error_limit = Binomial_Cutoff (len, error_rate, BIN_CUTOFF_PROB);

      // need to adjust b_lo here because of indels ****

      if  (Verbose > 3)
          {
           char  tag [1000];

           sprintf (tag, "cons");
           Fasta_Print (stderr, cons, tag);

           sprintf (tag, "s[%d]  offset = %d", i, align [i] . b_lo);
           Fasta_Print (stderr, s [i], tag);

           fprintf (stderr, "error_limit = %d  b_lo = %d\n",
                    error_limit, align [i] . b_lo);
          }

      attempts = 0;
      wiggle = offset_delta;
      needs_shift = false;
      do
        {
         Fix_Status_t  status;

         off = align [i] . b_lo + adjust [align [i] . b_lo];
         lo = Max (off - wiggle, 0);
         hi = Min (off + wiggle, cons_len);
         ok = Substring_Match_VS
                   (s [i], len, cons, cons_len, lo, hi,
                    error_limit, align [i]);

         if  (! ok)
             {
              wiggle *= 2;
              error_limit += Min (1, int (error_limit * 0.10));
             }
           else
             {
              align [i] . Check_Fix_Start (s [i] , len, cons, cons_len, status);
              if (status == NEEDS_LEFT_SHIFT)
                needs_shift = true;
             }

        }  while  (! ok && ++ attempts < MAX_ALIGN_ATTEMPTS);

      if (! ok || needs_shift)
        {
         Fix_Status_t  fix_status;

//##ALD  Replace this with a Substring_Align
         Overlap_Align (s [i], len, cons, lo, hi, cons_len,
              1, -3, -2, -2, align [i]);
         align [i] . a_len = len;
         align [i] . Check_Fix_Start (s [i] , len, cons, cons_len,
              fix_status);
        }

      if (! ok)
        {
         iostream::fmtflags  ios_status;

         cerr << "In  Reset_From_Votes  in contig " << id << endl
              << "  Forced alignment of string subscript " << i
              << " to consensus\n";
         ios_status = cerr . setf (ios :: fixed);
         cerr << "  with " << align [i] . errors << " errors ("
              << setprecision (2) << 100.0 * align [i] . Error_Rate ()
              << "% error)" << endl;
         cerr . setf (ios_status);

#if  0
          sprintf (Clean_Exit_Msg_Line,
                   "Failed on string %d in  Reset_From_Votes", i);
          throw AlignmentException_t (Clean_Exit_Msg_Line, __LINE__, __FILE__,
                                      -1, i);
#endif
        }

      align [i] . Incr_Votes (vote, s [i]);
      if  (align [i] . b_lo < min_b_lo)
          min_b_lo = align [i] . b_lo;
      if  (align [i] . b_hi > max_b_hi)
          max_b_hi = align [i] . b_hi;
     }

   if  (max_b_hi < cons_len)
       consensus . resize (max_b_hi);

   if  (min_b_lo > 0)
       {
        consensus . erase (0, min_b_lo);
        for  (i = 0;  i < n;  i ++)
          {
           align [i] . b_lo -= min_b_lo;
           align [i] . b_hi -= min_b_lo;
          }
       }

   free (cons);

   return;
  }



void  Multi_Alignment_t :: Set_Consensus
    (char * s)

//  Make this multialignment's  consensus  string be  s .

  {
   consensus . assign (s);
  }



void  Multi_Alignment_t :: Set_Initial_Consensus
    (const vector <char *> & s, const vector <int> & offset,
     int offset_delta, double error_rate, int min_overlap,
     vector <Vote_t> & vote, vector <char *> * tag_list, bool allow_expels)

// Create an initial consensus string in this multialignment from the
// strings in  s  with nominal relative offsets in  offset .  Offsets
// are allowed to vary by +/-  offset_delta  and the allowed error rate
// in alignments is  error_rate .  Strings must overlap by
// at least  min_overlap  bases.  Set  vote  to the votes of the strings
// at each position of the consensus.  Create the consensus by greedily
// tiling the strings in order, appending the extension of any
// string that aligns past the end of the consensus.  Store the
// initial alignments in this consensus, too.  If  tag_list  isn't
//  NULL , then use its values to identify strings.  If  allow_expels  is
// true, then reads can be omitted from the multialignment if they would
// have to be forced and they do not disconnect the alignment.

  {
   Vote_t  v;
   Alignment_t  ali;
   vector <bool>  expel;
   char  * cons;
   bool  wrote_contig_id = false;
   int  cons_len, prev_cons_len = 0;
   int  virtual_cons_len, prev_virtual_cons_len = 0;
   int  offset_sum = 0;
   int  num_strings, prev_sub;
   int  i, j;

   Clear ();
   vote . clear ();

   cons = strdup (s [0]);
   virtual_cons_len = cons_len = strlen (cons);
   // virtual_cons_len  is how long the consensus would be if all the
   // offset values were correct and everything aligned perfectly
   for  (j = 0;  j < cons_len - 1;  j ++)
     {
      v . Set_To (s [0] [j], true);
      vote . push_back (v);
     }
   v . Set_To (s [0] [cons_len - 1], false);
   vote . push_back (v);

   ali . Set_To_Identity (cons_len);
   align . push_back (ali);
   prev_sub = 0;

   num_strings = s . size ();
       // where the last string started in the consensus
   for (i = 0; i < num_strings; i ++)
     expel . push_back (false);

   for  (i = 1;  i < num_strings;  i ++)
     {
      Fix_Status_t  fix_status;
      bool  wrote_string_tag = false, can_skip = false, retry_worked = false;
      bool  matched;
      double  erate;
      int  error_limit, len, exp_olap_len, min_olap;
      int  attempts, curr_offset, wiggle;
      int  lo, mid, hi;

      len = strlen (s [i]);
      attempts = 0;
      wiggle = offset_delta;
      erate = error_rate;
      offset_sum += offset [i];

      if (allow_expels)
        curr_offset = Min (cons_len, Estimate_Offset_With_Expels (i, offset, expel));
      else
        curr_offset = Min (cons_len, Estimate_Offset_Position (i, offset));
        // where offset position hits the consensus, based on prior
        // string alignment(s)

      min_olap = Min (min_overlap, Min (cons_len, len));
      // reduce the minimum overlap length if the strings are too short
      // to achieve it
      mid = cons_len - min_olap;

      do
        {
         lo = Max (0, Min (curr_offset - wiggle, mid));
         hi = Min (Max (lo + 1, mid), curr_offset + wiggle);
         exp_olap_len = Min (cons_len - lo, len);
         error_limit = Binomial_Cutoff (exp_olap_len, erate, BIN_CUTOFF_PROB);
         // Add the current read to the consensus if there is an overlap
         matched = Overlap_Match_VS (s [i], len, cons, cons_len, lo, hi,
                        0, error_limit, ali);
         matched = matched && ali . Error_Rate () <= erate;

         if  (Verbose > 0 && matched && attempts > 0)
             fprintf (stderr,
                  "Matched with wiggle = %d  error_rate = %.1f%%\n",
                  wiggle, 100.0 * ali . Error_Rate ());

         // Increase the wiggle and error rate if alignment is not good enough
         if  (! matched)
             {
              if  (Verbose > 0)
                  {
                   if  (! wrote_contig_id)
                       {
                        cerr << endl
                             << "In Set_Initial_Consensus for contig " << id
                             << ":" << endl;
                        wrote_contig_id = true;
                       }
                   if  (tag_list != NULL && ! wrote_string_tag)
                       {
                        cerr << "For string " << (* tag_list) [i] << endl;
                        wrote_string_tag = true;
                       }
                   cerr << "Alignment failed with wiggle = " << wiggle
                        << "  error_limit = " << error_limit << endl;
                  }
              wiggle *= 2;
              if (erate <= 0.99)
                  erate += 0.01;

             }

        }  while  (! matched && ++ attempts < MAX_ALIGN_ATTEMPTS);

      if (matched)
        ali . Check_Fix_Start (s [i], len, cons, cons_len, fix_status);
      else
        {
          if (allow_expels)
            {
              can_skip = Check_Subsequent_Overlaps
                (cons, cons_len, s, i + 1, offset, num_strings, curr_offset,
                 offset_delta, min_overlap, error_rate, offset_sum,
                 virtual_cons_len);
                // Use the original min-overlap value since this string
                // might be a short contained string.
                // Also use the original wiggle (offset_delta) and erate values.
                // Maybe this is too conservative??
              if (! can_skip && 0 < prev_sub && prev_cons_len < cons_len)
                {
                  // try overlapping without most recent prior consensus
                  min_olap = Min (min_overlap, Min (prev_cons_len, len));
                  mid = prev_cons_len - min_olap;
                  lo = Max (0, Min (curr_offset - offset_delta, mid));
                  hi = Min (Max (lo + 1, mid), curr_offset + wiggle);
                  exp_olap_len = Min (prev_cons_len - lo, len);

                  error_limit = Binomial_Cutoff (exp_olap_len, error_rate, BIN_CUTOFF_PROB);
                  retry_worked = Overlap_Match_VS
                    (s [i], len, cons, prev_cons_len, lo, hi, 0, error_limit, ali);
                  retry_worked = retry_worked && ali . Error_Rate () <= error_rate;
                  if (retry_worked)
                    { // Remove prev_sub and its votes
                      align [prev_sub] . Incr_Votes (vote, s [prev_sub], -1);
                      align [prev_sub]  . Clear ();
                      vote . resize (prev_cons_len);
                      cons_len = prev_cons_len;
                      virtual_cons_len = prev_virtual_cons_len;
                      expel [prev_sub] = true;
                    }
                }
            }

          if (can_skip)
            expel [i] = true;
          else if (! retry_worked)
            {
              iostream::fmtflags status;
              int  a_width, b_width, max_width;

              if  (Verbose > 0)
                cerr << "No more Mr. Nice Guy--forcing alignment" << endl;

              Overlap_Align (s [i], len, cons, lo, hi, cons_len,
                             1, -3, -2, -2, ali);
              ali . a_len = len;

              a_width = ali . a_hi - ali . a_lo;
              b_width = ali . b_hi - ali . b_lo;
              max_width = Max (a_width, b_width);

              cerr << "In contig " << id << " forced alignment of "
                   << a_width << " bases of string ";
              if  (tag_list != NULL)
                cerr << (* tag_list) [i] << " ";
              cerr << "subscript " << i;
              cerr << " to " << b_width << " bases of consensus\n";
              status = cerr . setf (ios :: fixed);
              cerr << "  with " << ali . errors << " errors ("
                   << setprecision (2) << Percent (ali . errors, max_width)
                   << "% error)" << endl;
              cerr . setf (status);

#if  0
              fprintf (stderr,
                       "\nERROR:  Failed to find overlap between this string:\n");
              Fasta_Print (stderr, s [i]);
              fprintf (stderr, "\nand this portion of prior consensus:\n");
              Fasta_Print (stderr, cons + lo);
              fprintf (stderr,
                       "\nstarting in the first %d bases with at most %d errors\n\n",
                       hi - lo, error_limit);
              sprintf (Clean_Exit_Msg_Line,
                       "Failed on string %d in  Set_Initial_Consensus", i);
              throw AlignmentException_t (Clean_Exit_Msg_Line, __LINE__, __FILE__,
                                          -1, i);
#endif
            }
        }

      if (expel [i])
        ali . Clear ();
      else
        {
          ali . Incr_Votes (vote, s [i]);

          prev_sub = i;
          prev_cons_len = cons_len;
          prev_virtual_cons_len = virtual_cons_len;

          if (virtual_cons_len < offset_sum + len)
            virtual_cons_len = offset_sum + len;

          if (ali . a_hi < len)
            {  // s [i] extends past the end of the current consensus
              int  extra;

              extra = len - ali . a_hi;
              cons = (char *) Safe_realloc (cons, cons_len + extra + 1,
                                            __FILE__, __LINE__);
              strcpy (cons + cons_len, s [i] + ali . a_hi);
              cons_len += extra;

              for (j = ali . a_hi; j < len - 1; j ++)
                {
                  v . Set_To (s [i] [j], true);
                  vote . push_back (v);
                }
              v . Set_To (s [i] [len - 1], false);
              vote . push_back (v);

              ali . a_hi = len;
              ali . b_hi = cons_len;
            }

          ali . a_len = len;
        }
      align . push_back (ali);
     }

   if (Verbose > 1)
     {
       fprintf (stderr, "*** Votes ***\n");
       fprintf (stderr, "%5s: %2s  ------ Here -------  ------ After ------\n",
                "Pos", "Ch");
       for (j = 0; j < cons_len; j ++)
         fprintf (stderr, "%5d:  %c  %3d %3d %3d %3d %3d  %3d %3d %3d %3d %3d\n",
                  j, cons [j],
                  vote [j] . here [0], vote [j] . here [1], 
                  vote [j] . here [2], vote [j] . here [3], 
                  vote [j] . here [4],
                  vote [j] . after [0], vote [j] . after [1], 
                  vote [j] . after [2], vote [j] . after [3], 
                  vote [j] . after [4]);
     }

   consensus . assign (cons);
   free (cons);

   return;
  }




// ###  Gapped_Multi_Alignment_t  methods  ###


void  Gapped_Multi_Alignment_t :: Add_Aligned_Seqs
    (const Gapped_Multi_Alignment_t & m, const Alignment_t & ali,
     int cons_lo, int cons_hi, vector <char *> & sl1,
     const vector <char *> & sl2, vector <int> & sl2_place,
     vector <char *> * tg1, vector <char *> * tg2)

//  Add sequences in  m  that align to positions  cons_lo .. cons_hi
//  in  m . consensus  to this multialignment.  Use alignment  ali
//  to map  m . consensus  to  this -> consensus .  Check  sl2_place
//  to see if the actual sequence is already in  sl1 .  If not, add it
//  onto the end of  sl1  and set the corresponding  sl2_place  entry
//  to indicate where it is.  If
//   tg1  and  tg2  are not both NULL, also add the tags from
//   tg2  onto the end of  tg1  (unless  sl2_place  indicates they're
//  already there).

  {
   Gapped_Alignment_t  sub_ali;
   bool  use_tgs = (tg1 != NULL && tg2 != NULL);
   int  i, j, n;

   if  (Verbose > 2)
       {
        printf ("ali:\n");
        ali . Dump (stdout);
       }
   n = m . align . size ();
   for  (i = 0;  i < n;  i ++)
     if  (Range_Intersect (cons_lo, cons_hi, m . align [i] . b_lo,
               m . align [i] . b_hi))
         {
          if  (Verbose > 2)
              {
               printf ("m . align [%d]:\n", i);
               m . align [i] . Dump (stdout);
              }

          m . align [i] . Make_Sub_Alignment (sub_ali, cons_lo, cons_hi);

          if  (Verbose > 2)
              {
               printf ("First sub_ali:\n");
               sub_ali . Dump (stdout);
              }

          sub_ali . Pass_Through (ali);

          if  (Verbose > 2)
              {
               printf ("sub_ali after Pass_Through:\n");
               sub_ali . Dump (stdout);
              }

          if  (sl2_place [i] >= 0)
              j = sl2_place [i];
            else
              {
               sl1 . push_back (strdup (sl2 [i]));
               if  (use_tgs)
                   (* tg1) . push_back (strdup ((* tg2) [i]));
               j = sl1 . size () - 1;
               sl2_place [i] = j;
              }

          sub_ali . string_sub = j;
          align . push_back (sub_ali);
         }

   return;
  }



void  Gapped_Multi_Alignment_t :: Clear
    (void)

//  Clear this gapped multi-alignment.

  {
   consensus . erase ();
   con_qual . erase ();
   align . clear ();

   return;
  }



void  Gapped_Multi_Alignment_t :: Consensus_To_Lower
    (void)

//  Convert the consensus sequence in this multialignment
//  to lowercase.

  {
   int  i, n;

   n = consensus . length ();
   for  (i = 0;  i < n;  i ++)
     consensus [i] = tolower (consensus [i]);

   return;
  }



void  Gapped_Multi_Alignment_t :: Convert_Consensus
    (const Multi_Alignment_t & ma, const vector <short> & v)

//  Convert the (ungapped) consensus string in  ma  to a gapped
//  one in this gapped multialignment by inserting extra '-'s.
//  The number of hyphens to insert at each position is specified
//  in  v .

  {
   int  len;
   int  i, j, n;

   len = ma . consensus . length ();
   n = v . size ();

   if  (n <= len)
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Gapped_Multi_Alignment_t :: Convert_Consensus\n"
            "  insert vector shorter than consensus string\n"
            "  len = %d  insert size = %d",
            len, int (v . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   consensus . assign ("");
   for  (i = 0;  i < len;  i ++)
     {
      for  (j = 0;  j < v [i];  j ++)
        consensus . push_back ('-');
      consensus . push_back (ma . consensus [i]);
     }

   for  (j = 0;  j < v [len];  j ++)
     consensus . push_back ('-');

   return;
  }



void  Gapped_Multi_Alignment_t :: Convert_From
    (const Multi_Alignment_t & ma)

//  Convert the mulitalignment  ma  (where the consensus has no gaps)
//  to a gapped multialignment where the consensus has gaps (enough
//  to hold all insertions of all strings in the multialignment).

  {
   Gapped_Alignment_t  ga;
   int  len;
   int  i, n;

   id = ma . id;

   len = 1 + ma . consensus . length ();

   vector <short>  insert (len, 0);
   vector <int>  tr (len, 0);

   n = ma . align . size ();
   for  (i = 0;  i < n;  i ++)
     ma . align [i] . Set_Inserts (insert);

   Convert_Consensus (ma, insert);

   // build translation table for each position in ungapped consensus
   // to its corresponding position in the gapped consensus
   tr [0] = insert [0];
   for  (i = 1;  i < len;  i ++)
     tr [i] = tr [i - 1] + insert [i] + 1;

   align . clear ();
   for  (i = 0;  i < n;  i ++)
     {
      ga . Convert_From (ma . align [i], tr);
      align . push_back (ga);
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Count_Column_Chars
    (Matrix <unsigned char> & count,
     const vector <char *> & sl)

//  Set entries in  count  to number of each character in
//  corresponding alignment columns.   sl  has the actual
//  sequences.  Assume entries in  count  are all already zero.

  {
   int  len;
   int  i, n;

   if  (sl . size () != align . size ())
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Count_Column_Chars called with %d strings and %d alignments",
            int (sl . size ()), int (align . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   n = align . size ();
   len = getConsensusLen ();
   if  (count . NumRows () < len)
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Count_Column_Chars called with count vector = %d\n"
            " and consensus len = %d",
            count . NumRows (), len);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   for  (i = 0;  i < n;  i ++)
     align [i] . Incr_Column_Chars (count, sl [i]);

   return;
  }



void  Gapped_Multi_Alignment_t :: Dump_Aligns
    (FILE * fp)

//  Print to  fp  the alignments in this multialignment.

  {
   int  i, n;

   n = align . size ();
   fprintf (fp, "### num aligns = %d\n", n);
   for  (i = 0;  i < n;  i ++)
     align [i] . Dump (fp);

   return;
  }



void  Gapped_Multi_Alignment_t :: Expand_Consensus
    (const Alignment_t & ali)

//  Adjust the consensus sequence of this multialignment to add
//  gaps where needed to accommodate the alignment in  ali ,
//  which represents the alignment of another consensus to this one.
//  Also adjust the individual read alignments for the new gaps added.

  {
   Delta_Encoding_t  delta = ali . getDelta ();
   vector <int>  insert_sub;
   int  prev;
   int  i, n;

   n = delta . size ();
   prev = ali . b_lo;
   for  (i = 0;  i < n;  i ++)
     if  (delta [i] > 0)
         prev += delta [i];
       else
         {
          prev -= delta [i];
          insert_sub . push_back (prev - 1);
          consensus . insert (prev - 1, 1, '-');
         }

   n = insert_sub . size ();

   if  (n == 0)
       return;  // nothing to do

   if  (Verbose > 2)
       for  (i = 0;  i < n;  i ++)
         printf ("insert_sub [%d] = %d\n", i, insert_sub [i]);

   n = align . size ();
   for  (i = 0;  i < n;  i ++)
     if  (insert_sub [0] < align [i] . b_hi)
         align [i] . Modify_For_B_Inserts (insert_sub);
   return;
  }



void  Gapped_Multi_Alignment_t :: Extract_Gapped_Region
    (int lo, int hi, string & s, int & gapped_lo, int & gapped_hi)  const

//  Set  s  to the substring of the gapped consensus
//  between  lo  and  hi , where  lo  and  hi  are
//  *UN*gapped positions.  Set  gapped_lo  and  gapped_hi
//  to indicate the region in the gapped consensus that
//  was extracted.

  {
   bool  is_first = true;
   int  i, j, n;

   s . erase ();

   n = consensus . length ();
   for  (i = j = 0;  i < n && j < hi;  i ++)
     {
      if  (consensus [i] != '-')
          j ++;
      if  (j > lo && j <= hi)
          {
           s . push_back (consensus [i]);
           if  (is_first)
               {
                gapped_lo = i;
                is_first = false;
               }
           gapped_hi = i + 1;
          }
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Extract_IMP_Dels
    (vector < vector <int> > & del_list)

//  Set  del_list  to be the del entries in IMP submessages
//  corresponding to the skip values in this alignment.

  {
   vector <int>  del;
   int  i, n;

   del_list . clear ();

   n = align . size ();
   for  (i = 0;  i < n;  i ++)
     {
      align [i] . Convert_Skip_to_Del (del);
      del_list . push_back (del);
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Full_Merge_Left
    (const Gapped_Multi_Alignment_t & m, int adj_a_lo, int a_lo, int a_hi,
     int b_lo, int b_hi, int & prefix_len_added, vector <char *> & sl1,
     const vector <char *> & sl2, vector <int> & sl2_place,
     vector <char *> * tg1, vector <char *> * tg2)

//  Merge the prefix of the alignment in  m  onto the front
//  of this alignment.  The region  a_lo .. a_hi
//  in the  m  consensus matches the prefix of this
//  alignment  b_lo .. b_hi .  Note that  b_lo  may be
//  slightly bigger than 0, so some "play" is allowed
//  for finding where the beginning of this multialignment
//  consensus actually hits  m 's consensus.  The search for the
//  start of the alignment begins between  adj_a_lo  and  a_lo + 1 .
//   sl1  and  sl2  are the sequences of the reads for this
//  multialignment and multialignment  m , respectively.
//  Set  prefix_len_added  to the number of *ungapped* positions
//  in the prefix prepended to this consensus sequence.
//  Must have  a_lo <= a_hi  and  b_lo <= b_hi .
//   sl2_place  indicates whether and where  sl2  strings are already
//  in  sl1 .  Any new strings are added
//  from  sl2  to end of  sl1  and  sl2_place  is updated.
//  If neither  tg1  nor  tg2  is  NULL , then also add relevant
//  sequences from  tg2  to the end of  tg1 , mirroring  sl1
//  and  sl2 .

  {
   Alignment_t  ali;
   Align_Score_Entry_t  last_entry;
   string  x, y;
   int  a_gapped_lo, a_gapped_hi, b_gapped_lo, b_gapped_hi;
   int  x_len, y_len, x_slip, extra;

   // First align the consensus sequences (also aligning the gap
   //   positions based on the characters that are there) in the
   // overlap region

   // Pull out the respective consensus regions to the end of each
   // sequence.
   // Need to add some indication of what characters aligned with the
   //   gap position
   m . Extract_Gapped_Region (adj_a_lo, a_hi, x, a_gapped_lo, a_gapped_hi);
   x_len = x . length ();

   Extract_Gapped_Region (0, b_hi, y, b_gapped_lo, b_gapped_hi);
   y_len = y . length ();

   // Align them
   // Need to add something to take into account characters associated
   //   with gap positions
   // The alignments must end exactly where indicated

   x_slip = 1 + Gapped_Equivalent (a_lo - adj_a_lo, x);
   Complete_Align (y . c_str (), 0, y_len, x . c_str (), 0,
        x_slip, x_len, 2, -3, -2, -1, NULL, last_entry, ali);

   if  (Verbose > 2)
       {
        printf ("Full_Merge_Left:  Alignment before flip:\n");
        ali . Print (stdout, y . c_str (), x . c_str ());
       }

   ali . Flip_AB ();

   if  (Verbose > 2)
       {
        printf ("Full_Merge_Left:  Alignment after flip:\n");
        ali . Print (stdout, x . c_str (), y . c_str ());
       }

   extra = a_gapped_lo + ali . a_lo;

   // Prepend the leftover part of  a  to the front of this consensus sequence
   // Because the added leftover part is identical, only need to adjust
   // the first delta value.

   assert (extra >= 0);
   if  (extra > 0)
       {
        consensus . insert (0, m . consensus, 0, extra);
        ali . Shift_First_Delta (extra);
        Shift_B_Right (extra);
       }

   ali . a_lo = 0;
   ali . a_hi = a_gapped_hi;
   assert (ali . b_lo == 0);
   ali . b_hi += extra;

   Expand_Consensus (ali);

   if  (Verbose > 2)
       {
        cout << endl << endl << "Contig in  Full_Merge_Left  after Expand_Consensus"
             << endl;
        cout << "sl1 . size () = " << sl1 . size () << endl;
        cout << "sl2 . size () = " << sl2 . size () << endl;
        Print (stdout, sl1, 60);
       }

   Add_Aligned_Seqs (m, ali, 0, a_gapped_hi, sl1, sl2, sl2_place,
        tg1, tg2);

   if  (Verbose > 2)
       {
        int  i, n;

        n = align . size ();
        cout << endl << "Align after  Full_Merge_Right  n = " << n << endl;
        cout << "sl1 . size () = " << sl1 . size () << endl;
        cout << "sl2 . size () = " << sl2 . size () << endl;
        for  (i = 0;  i < n;  i ++)
          {
           printf ("align [%d]:\n", i);
           align [i] . Dump (stdout);
          }
        
        n = sl2_place . size ();
        cout << endl << "sl2_place  n = " << n << endl;
        for  (i = 0;  i < n;  i ++)
          cout << sl2_place [i] << endl;
       }

   // Eventually re-call the consensus here

   prefix_len_added = Ungapped_Positions (consensus, 0, extra);

   return;
  }



void  Gapped_Multi_Alignment_t :: Full_Merge_Right
    (const Gapped_Multi_Alignment_t & m, int a_lo,
     int b_lo, vector <char *> & sl1,
     const vector <char *> & sl2, vector <int> & sl2_place,
     vector <char *> * tg1, vector <char *> * tg2)

//  Merge the entire alignment in  m  starting at  a_lo
//  into this alignment starting at  b_lo .  The presumption
//  is that the alignment will go to the right end of this
//  multialignment and beyond to extend it.
//   sl1  and  sl2  are the sequences of the reads for this
//  multialignment and multialignment  m , respectively.
//  Must have  a_lo <= a_hi  and  b_lo <= b_hi .
//   sl2_place  indicates whether and where  sl2  strings are already
//  in  sl1 .  Any new strings are added
//  from  sl2  to end of  sl1  and  sl2_place  is updated.
//  If neither  tg1  nor  tg2  is  NULL , then also add relevant
//  sequences from  tg2  to the end of  tg1 , mirroring  sl1
//  and  sl2 .

  {
   Alignment_t  ali;
   string  x, y;
   int  a_gapped_lo, a_gapped_hi, b_gapped_lo, b_gapped_hi;
   int  a_hi, b_hi, x_len, y_len, extra;

   // First align the consensus sequences (also aligning the gap
   //   positions based on the characters that are there) in the
   // overlap region

   // Pull out the respective consensus regions to the end of each
   // sequence.
   // Need to add some indication of what characters aligned with the
   //   gap position
   a_hi = m . Ungapped_Consensus_Len ();
   m . Extract_Gapped_Region (a_lo, a_hi, x, a_gapped_lo, a_gapped_hi);
   x_len = x . length ();

   b_hi = Ungapped_Consensus_Len ();
   Extract_Gapped_Region (b_lo, b_hi, y, b_gapped_lo, b_gapped_hi);
   y_len = y . length ();

   // Align them
   // Need to add something to take into account characters associated
   //   with gap positions
   // This version has no flexibility--the alignments must start exactly
   // where indicated
   Overlap_Align (x . c_str (), x_len, y . c_str (), 0, 1, y_len,
        2, -3, -2, -1, ali);

   if  (Verbose > 2)
       ali . Print (stdout, x . c_str (), y . c_str ());

   extra = x_len - ali . a_hi;

   // Append the leftover part of  a  to the end of this sequence
   if  (extra > 0)
       {
        assert (ali . b_hi == y_len);
        consensus . append (x, ali . a_hi, extra);
       }
     else
       extra = 0;

   // Because the added leftover part is identical, the alignment is
   // correct for the entire region, too.

   ali . a_lo += a_gapped_lo;
   ali . a_hi += a_gapped_lo + extra;
   ali . b_lo += b_gapped_lo;
   ali . b_hi += b_gapped_lo + extra;

   Expand_Consensus (ali);

   if  (Verbose > 2)
       {
        cout << endl << endl << "Contig in  Full_Merge_Right  after Expand_Consensus"
             << endl;
        cout << "sl1 . size () = " << sl1 . size () << endl;
        cout << "sl2 . size () = " << sl2 . size () << endl;
        Print (stdout, sl1, 60);
       }

   Add_Aligned_Seqs (m, ali, a_gapped_lo, a_gapped_hi, sl1, sl2, sl2_place,
        tg1, tg2);

   if  (Verbose > 2)
       {
        int  i, n;

        n = align . size ();
        cout << endl << "Align after  Full_Merge_Right  n = " << n << endl;
        cout << "sl1 . size () = " << sl1 . size () << endl;
        cout << "sl2 . size () = " << sl2 . size () << endl;
        for  (i = 0;  i < n;  i ++)
          {
           printf ("align [%d]:\n", i);
           align [i] . Dump (stdout);
          }
        
        n = sl2_place . size ();
        cout << endl << "sl2_place  n = " << n << endl;
        for  (i = 0;  i < n;  i ++)
          cout << sl2_place [i] << endl;
       }

   // Eventually re-call the consensus here

   return;
  }



void  Gapped_Multi_Alignment_t :: Get_Distinguishing_Columns
    (vector <Distinguishing_Column_t> & dc,
     const vector <char *> & sl)

//  Fill  dc  with the distinguishing columns in this multialignment.
//  A distinguishing column has a confirmed SNP.
//  It may actually be a range of adjacent columns.
//   sl  holds the component sequences in the multialignment and

  {
   Distinguishing_Column_t  d;
   int  i, len;

   dc . clear ();

   len = getConsensusLen ();

   Matrix <unsigned char>  count (len, 5);
        // counts of a, c, g, t and - at each position of consensus
        // automatically set to zero

   Count_Column_Chars (count, sl);

   for  (i = 0;  i < len;  i ++)
     {
      char  ch1, ch2;
      int  ch1_ct, ch2_ct;

      if  (Is_Distinguishing (count . getRow (i), ch1, ch1_ct, ch2, ch2_ct))
          {
           d . lo = i;
           d . hi = i + 1;   // restrict to single characters for now
           d . hapa_ch = ch1;
           d . hapb_ch = ch2;
           d . phase_ch = ' ';
           Get_Element_Subs (d, sl);

           dc . push_back (d);
          }
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Get_Element_Subs
    (Distinguishing_Column_t & d, const vector <char *> & sl)

//  Fill   hapa_sub  and  hapb_sub  in  d  with the subscripts
//  of the strings in  sl  that have  d . hapa_ch  and  d . hapb_ch ,
//  respectively, at the position indicated in  d .

  {
   int  i, n;

   d . hapa_sub . clear ();
   d . hapb_sub . clear ();

   n = align . size ();
   for  (i = 0;  i < n;  i ++)
     if  (Range_Intersect (align [i] . b_lo, align [i] . b_hi, d . lo, d . hi))
         {
          char  ch;

          ch = align [i] . Get_Aligning_Char (d . lo, sl [i]);
          if  (ch == d . hapa_ch)
              d . hapa_sub . push_back (i);
          else if  (ch == d . hapb_ch)
              d . hapb_sub . push_back (i);
         }

   return;
  }



void  Gapped_Multi_Alignment_t :: Get_Partial_Ungapped_Consensus
    (string & s, int lo, int hi)  const

//  Set  s  to the ungapped portion of the consensus of this
//  multialignment between positions  lo  and  hi  (in gap
//  coordinates).

  {
   int  i, j, n;

   s . erase ();
   n = consensus . length ();
   for  (i = j = 0;  i < n && j < hi;  i ++)
     if  (consensus [i] != '-')
         {
          if  (lo <= j)
              s . push_back (consensus [i]);
          j ++;
         }

   return;
  }



void  Gapped_Multi_Alignment_t :: Get_Positions
    (vector <AMOS :: Range_t> & pos)  const

//  Set  pos  to the list of  b_lo  and  b_hi  values in
//  the  align  vector of this alignment

  {
   int  i, n;

   n = align . size ();
   pos . resize (n);

   for  (i = 0;  i < n;  i ++)
     pos [i] . setRange (align [i] . b_lo, align [i] . b_hi);

   return;
  }



void  Gapped_Multi_Alignment_t :: Get_Ungapped_Consensus
    (string & s)

//  Set  s  to the ungapped consensus sequence of this
//  multialignment.

  {
   int  i, n;

   s . erase ();
   n = consensus . length ();
   for  (i = 0;  i < n;  i ++)
     if  (consensus [i] != '-')
         s . push_back (consensus [i]);

   return;
  }



void  Gapped_Multi_Alignment_t :: Haplo_Sep
    (const vector <char *> & sl, vector <Distinguishing_Column_t> & dc,
     vector <char *> * tg)

//  Identify distinguishing columns in this multialignment
//  and use them to partition the elements into separate
//  haplotypes as much as possible.   sl  holds the sequences
//  of the elements and  tg  holds the id tags of the elements.
//  Set  dc  to hold the SNPs between the haplotypes.

  {
   vector <char>  side;
   vector <int>  segment;
   int  i, m, n;

   Get_Distinguishing_Columns (dc, sl);

   Set_Phase (dc);

   n = sl . size ();
   vector <int>  pos (n, 0), neg (n, 0);

   Classify_Reads (dc, n, side, segment);

   m = dc . size ();
   for  (i = 0;  i < m;  i ++)
     {
      int  j, k;

      if  (dc [i] . phase_ch == '+')
          {
           k = dc [i] . hapa_sub . size ();
           for  (j = 0;  j < k;  j ++)
             pos [dc [i] . hapa_sub [j]] ++;

           k = dc [i] . hapb_sub . size ();
           for  (j = 0;  j < k;  j ++)
             neg [dc [i] . hapb_sub [j]] ++;
          }
      else if  (dc [i] . phase_ch == '-')
          {
           k = dc [i] . hapa_sub . size ();
           for  (j = 0;  j < k;  j ++)
             neg [dc [i] . hapa_sub [j]] ++;

           k = dc [i] . hapb_sub . size ();
           for  (j = 0;  j < k;  j ++)
             pos [dc [i] . hapb_sub [j]] ++;
          }
     }

   int  p_ct = 0, n_ct = 0, u_ct = 0;

   printf ("String Assignments:\n");
   for  (i = 0;  i < n;  i ++)
     {
      if  (tg != NULL)
          printf ("%3d %8s: %7d %7d  %c %4d\n", i, (* tg) [i], pos [i], neg [i],
               side [i], segment [i]);
        else
          printf ("%3d: %7d %7d  %c %4d\n", i, pos [i], neg [i], side [i],
               segment [i]);
      if  (pos [i] > neg [i])
          p_ct ++;
      else if  (pos [i] < neg [i])
          n_ct ++;
        else
          u_ct ++;
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Make_From_CCO_Msg
    (const Celera_Message_t & msg, const vector <int> & slen)

//  Set this multi-alignment to the one in the Celera CCO
//  message  msg .   slen  is the lengths of the reads in  msg .

  {
   vector <Celera_MPS_Sub_Msg_t>  mps_list;
   vector <Celera_UPS_Sub_Msg_t>  ups_list;
   Ordered_Range_t  position;
   Gapped_Alignment_t  ga;
   vector <int>  del;
   int  i, n, slen_sub;

   consensus = msg . getConsensus ();
   n = consensus . length ();
   for  (i = 0;  i < n;  i ++)
     consensus [i] = tolower (consensus [i]);
   con_qual = msg . getQuality ();

   align . clear ();

   mps_list = msg . getMPSList ();
   ups_list = msg . getUPSList ();

   n = mps_list . size ();
   for  (i = 0;  i < n;  i ++)
     {
      int  j, k, x, y;

      ga . a_lo = 0;
      ga . a_hi = slen [i];
      ga . a_len = slen [i];
      position = mps_list [i] . getPosition ();
      x = position . getBegin ();
      y = position . getEnd ();
      if  (x < y)
          {
           ga . b_lo = x;
           ga . b_hi = y;
           ga . flipped = 0;
          }
        else
          {
           ga . b_lo = y;
           ga . b_hi = x;
           ga . flipped = 1;
          }
      ga . errors = 0;    // Temporary--needs to be set correctly later
      del = mps_list [i] . getDel ();
      k = del . size ();
      for  (j = 0;  j < k;  j ++)
        del [j] += ga . b_lo + j;
      ga . setSkip (del);
      align . push_back (ga);
     }

   slen_sub = n;

   // Only extract alignment for surrogate (i.e., stone) unitigs
   n = ups_list . size ();
   for  (i = 0;  i < n;  i ++)
     {
      int  j, k, x, y;

      if  (ups_list [i] . getUnitigType () == STONE_UNI_T)
          {
           ga . a_lo = 0;
           ga . a_hi = slen [slen_sub];
           ga . a_len = slen [slen_sub];
           slen_sub ++;
           position = ups_list [i] . getPosition ();
           x = position . getBegin ();
           y = position . getEnd ();
           if  (x < y)
               {
                ga . b_lo = x;
                ga . b_hi = y;
                ga . flipped = 0;
               }
             else
               {
                ga . b_lo = y;
                ga . b_hi = x;
                ga . flipped = 1;
               }
           ga . errors = 0;    // Temporary--needs to be set correctly later
           del = ups_list [i] . getDel ();
           k = del . size ();
           for  (j = 0;  j < k;  j ++)
             del [j] += ga . b_lo + j;
           ga . setSkip (del);
           align . push_back (ga);
          }
     }

   if  (slen_sub != int (slen . size ()))
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  In Make_From_CCO_Msg used %d of %d lengths\n"
            "  Should have been equal", slen_sub, int (slen.size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   return;
  }



void  Gapped_Multi_Alignment_t :: Make_Gapped_Ungapped_Lookup
    (vector <int> & lookup)

//  Set  lookup  to hold the corresponding ungapped position
//  for every position in this multialignment's consensus.

  {
   int  i, n;

   n = consensus . length ();
   lookup . resize (n + 1);

   lookup [0] = 0;
   for  (i = 0;  i < n;  i ++)
     if  (consensus [i] == '-')
         lookup [i + 1] = lookup [i];
       else
         lookup [i + 1] = 1 + lookup [i];

   return;
  }



void  Gapped_Multi_Alignment_t :: Merge
    (const Gapped_Multi_Alignment_t & m)

//  Merge the multialignment  m  into this one

  {
   // Not done yet.  Need to add extra parameters specifying
   // where the merge happens (including orientation)

   return;
  }



void  Gapped_Multi_Alignment_t :: Output_Read_Positions
    (FILE * fp, int id, const vector <char *> & tag, int orig_tag_ct,
     int len, int offset)

//  Output to  fp  a header line for this multialignment
//  containing  id  and the (ungapped) length of the consensus
//  followed by a list of reads and their implied positions
//  in this multialignment.  Include an indication of whether
//  the read is an original read (if it's subscript is  < orig_tag_ct )
//  or an added one (otherwise).   len  is the length of the
//  ungapped consensus.   offset  is added to the b positions
//  of all the reads

  {
   vector <int>  lookup;
   vector <Base_Alignment_t>  extreme;
        // holds extreme positions of all component reads
   Base_Alignment_t  * ep;
   int  bad_ct;
   int  i, j, num_aligns, num_tags;

   num_tags = tag . size ();
        // should be the same as number of distinct strings in alignment

   Make_Gapped_Ungapped_Lookup (lookup);

   extreme . resize (num_tags);
   for  (i = 0;  i < num_tags;  i ++)
     extreme [i] . string_sub = -1;   // mark to indicate not modified

   num_aligns = align . size ();
   for  (i = 0;  i < num_aligns;  i ++)
     {
      j = align [i] . string_sub;
      ep = & (extreme [j]);
      if  (ep -> string_sub == -1)
          {
           extreme [j] = align [i];
           ep -> b_lo = lookup [ep -> b_lo];
           ep -> b_hi = lookup [ep -> b_hi];
          }
      else if  (ep -> flipped != align [i] . flipped)
          ;  // skip it;  something's wrong;  same read is in with
             //   different orientations
        else
          {
           ep -> a_lo = Min (ep -> a_lo, align [i] . a_lo);
           ep -> a_hi = Max (ep -> a_hi, align [i] . a_hi);
           ep -> b_lo = Min (ep -> b_lo, lookup [align [i] . b_lo]);
           ep -> b_hi = Max (ep -> b_hi, lookup [align [i] . b_hi]);
          }
     }

   bad_ct = 0;
   for  (i = 0;  i < num_tags;  i ++)
     if  (extreme [i] . string_sub < 0)
         bad_ct ++;
   if  (bad_ct > 0)
       {
        fprintf (stderr, "YIKES:  %d unused alignments for contig %d\n",
             bad_ct, id);
       }

   fprintf (fp, ">%d  %d  %d\n", id, len, num_tags - bad_ct);
   
   for  (i = 0;  i < num_tags;  i ++)
     {
      int  actual_start, actual_stop;
      int  implied_start, implied_stop;

      ep = & (extreme [i]);

      if  (ep -> string_sub < 0)
          continue;

      if  (ep -> flipped)
          {
           actual_start = ep -> b_hi;
           actual_stop = ep -> b_lo;
           implied_start = actual_start + (ep -> a_len - ep -> a_hi);
           implied_stop = actual_stop - ep -> a_lo;
          }
        else
          {
           actual_start = ep -> b_lo;
           actual_stop = ep -> b_hi;
           implied_start = actual_start - ep -> a_lo;
           implied_stop = actual_stop + (ep -> a_len - ep -> a_hi);
          }
      actual_start += offset;
      actual_stop += offset;
      implied_start += offset;
      implied_stop += offset;

      fprintf (fp, "  %c  %c %10s  %7d %7d  %7d %7d\n",
           tag [i] [0], (i < orig_tag_ct ? 'a' : 'b'),
           tag [i] + 1, actual_start, actual_stop,
           implied_start, implied_stop);
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Partial_Merge
    (const Gapped_Multi_Alignment_t & m, int a_lo, int a_hi,
     int b_lo, int b_hi, vector <char *> & sl1,
     const vector <char *> & sl2, vector <int> & sl2_place,
     vector <char *> * tg1, vector <char *> * tg2)

//  Merge the portion of the alignment in  m  between  a_lo  and
//  a_hi  into this alignment between  b_lo  and  b_hi .
//   sl1  and  sl2  are the sequences of the reads for this
//  multialignment and multialignment  m , respectively.
//  Must have  a_lo <= a_hi  and  b_lo <= b_hi .
//   sl2_place  indicates whether and where  sl2  strings are already
//  in  sl1 .  Any new strings are added
//  from  sl2  to end of  sl1  and  sl2_place  is updated.
//  If neither  tg1  nor  tg2  is  NULL , then also add relevant
//  sequences from  tg2  to the end of  tg1 , mirroring  sl1
//  and  sl2 .

  {
   Alignment_t  ali;
   Align_Score_Entry_t  last_entry;
   string  x, y;
   int  a_gapped_lo, a_gapped_hi, b_gapped_lo, b_gapped_hi;
   int  x_len, y_len;

   // First align the consensus sequences (also aligning the gap
   //   positions based on the characters that are there)

   // Pull out the respective consensus regions
   // Need to add some indication of what characters aligned with the
   //   gap position
   m . Extract_Gapped_Region (a_lo, a_hi, x, a_gapped_lo, a_gapped_hi);
   x_len = x . length ();

   Extract_Gapped_Region (b_lo, b_hi, y, b_gapped_lo, b_gapped_hi);
   y_len = y . length ();

#if  0   
   printf (">a%d..%d\n", a_lo, a_hi);
   Fasta_Print (stdout, x . c_str (), NULL);

   printf (">b%d..%d\n", b_lo, b_hi);
   Fasta_Print (stdout, y . c_str (), NULL);
#endif

   // Align them
   // Need to add something to take into account characters associated
   //   with gap positions
#if  0
   Global_Align (x . c_str (), x_len, y . c_str (), 0, y_len, 2, -3, -2, -1, ali);
#else
   Complete_Align (x . c_str (), 0, x_len, y . c_str (), 0, 1, y_len,
        2, -3, -2, -1, NULL, last_entry, ali);
#endif

   if  (Verbose > 2)
       ali . Print (stdout, x . c_str (), y . c_str ());

   // Then align the individual reads to the result.

   if  (Verbose > 2)
       {
        printf ("ali:\n");
        ali . Dump (stdout);
        printf ("a_gapped_lo/hi = %d/%d  b_gapped_lo/hi = %d/%d\n",
             a_gapped_lo, a_gapped_hi, b_gapped_lo, b_gapped_hi);
       }

   ali . a_lo += a_gapped_lo;
   ali . a_hi += a_gapped_lo;
   ali . b_lo += b_gapped_lo;
   ali . b_hi += b_gapped_lo;
   Expand_Consensus (ali);

   if  (Verbose > 2)
       {
        cout << endl << endl << "Contig #1 after Expand_Consensus" << endl;
        cout << "sl1 . size () = " << sl1 . size () << endl;
        cout << "sl2 . size () = " << sl2 . size () << endl;
        Print (stdout, sl1, 60);
       }

   Add_Aligned_Seqs (m, ali, a_gapped_lo, a_gapped_hi, sl1, sl2, sl2_place,
        tg1, tg2);

   if  (Verbose > 2)
       {
        int  i, n;

        n = align . size ();
        cout << endl << "align after partial merge  n = " << n << endl;
        cout << "sl1 . size () = " << sl1 . size () << endl;
        cout << "sl2 . size () = " << sl2 . size () << endl;
        for  (i = 0;  i < n;  i ++)
          {
           printf ("align [%d]:\n", i);
           align [i] . Dump (stdout);
          }
        
        n = sl2_place . size ();
        cout << endl << "sl2_place  n = " << n << endl;
        for  (i = 0;  i < n;  i ++)
          cout << sl2_place [i] << endl;
       }

   // Eventually re-call the consensus here

   return;
  }



void  Gapped_Multi_Alignment_t :: Print
    (FILE * fp, const vector <char *> & s, int width,
     vector <char *> * tag)

//  Display this multialignment to file  fp  using
//   width  characters per line.   s  holds the strings
//  the alignment references.  If  tag  is not NULL, it
//  holds tag strings corresponding to the sequences  s
//  to be used to label the sub-alignment lines.  If  use_string_sub
//  is true, then use the  string_sub  field in each alignment to
//  determine which string in  s  to use.  If
//   with_diffs  is true also print a line under the consensus showing
//  where there are any differences with the aligned sequences.

  {
   char  * buff, * diff;
   bool  use_string_sub, with_diffs, use_gapped_coords;
   int  lo, hi, len, print_lo, print_hi, ungapped_lo, ungapped_hi;
   int  i, j, n;

   use_string_sub = (print_flags & PRINT_USING_STRING_SUB);
   with_diffs = (print_flags & PRINT_WITH_DIFFS);
   use_gapped_coords = (print_flags & PRINT_CONSENSUS_GAP_COORDS);

   if  (! use_string_sub && s . size () != align . size ())
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align . Print called with %d strings and %d alignments",
            int (s . size ()), int (align . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   n = align . size ();
   len = consensus . length ();
   buff = (char *) Safe_malloc (5 + len, __FILE__, __LINE__);
   if  (with_diffs)
       diff = (char *) Safe_malloc (5 + width, __FILE__, __LINE__);

   // assume for now the alignments are sorted in ascending order by
   // b_lo value

   ungapped_lo = 0;
   for  (lo = 0;  lo < len;  lo += width)
     {
      int  a_lo, a_hi;

      hi = Min (lo + width, len);
      if  (with_diffs)
          {
           memset (diff, ' ', hi - lo);
           diff [hi - lo] = '\0';
          }

      for (i = 0; i < n; i ++)
        if (! align [i] . Is_Empty ()
            && Range_Intersect (align [i] . b_lo, align [i] . b_hi, lo, hi))
          {
            char  tag_buff [100];
            int  sub;

            if  (use_string_sub)
              sub = align [i] . string_sub;
            else
              sub = i;
            align [i] . Print_Subalignment_Line
              (buff, lo, hi, s [sub], a_lo, a_hi);
             
            if  (tag == NULL)
              sprintf (tag_buff, "%4d:", sub);
            else
              sprintf (tag_buff, "%10.10s:", (* tag) [sub]);
            if  (align [i] . flipped)
              {
                print_lo = align [i] . a_len - a_lo;
                print_hi = align [i] . a_len - a_hi;
              }
            else
              {
                print_lo = a_lo;
                print_hi = a_hi;
              }
            fprintf (fp, "%s  %s  (%d-%d)\n", tag_buff, buff, print_lo,
                     print_hi);

            if  (with_diffs)
              {
                int  j, k;

                k = 0;
                for  (j = lo;  j < hi;  j ++, k ++)
                  if  (buff [k] != ' ' && buff [k] != consensus [j])
                    diff [k] = '^';
              }
          }

      Print_Consensus (buff, lo, hi);

      ungapped_hi = ungapped_lo;
      for  (j = 0;  buff [j] != '\0';  j ++)
        if  (buff [j] != '-')
            ungapped_hi ++;

      if  (use_gapped_coords)
          {
           print_lo = lo;
           print_hi = hi;
          }
        else
          {
           print_lo = ungapped_lo;
           print_hi = ungapped_hi;
          }

      if  (tag == NULL)
          {
           fprintf (fp, "%4s:  %s  (%d-%d)\n", "cons", buff, print_lo, print_hi);
           if  (with_diffs)
               fprintf (fp, "%5s  %s\n", "", diff);
          }
        else
          {
           fprintf (fp, "%10.10s:  %s  (%d-%d)\n", "consensus", buff, print_lo,
                print_hi);
           if  (with_diffs)
               fprintf (fp, "%11s  %s\n", "", diff);
          }

      ungapped_lo = ungapped_hi;

      if  (hi < len)
          fprintf (fp, "\n");
     }
   
   free (buff);
   if  (with_diffs)
       free (diff);

   return;
  }



void  Gapped_Multi_Alignment_t :: Print_Consensus
    (char * buff, int b1, int b2)

//  Write into  buff  the portion of this consensus string
//  that lies within the subrange  b1 .. b2 .

  {
   int  i, ct;

   ct = 0;
   for  (i = b1;  i < b2;  i ++)
     buff [ct ++] = consensus [i];

   buff [ct] = '\0';

   return;
  }



int  Gapped_Multi_Alignment_t :: Print_Empty_Aligns
  (FILE * fp, const vector <char *> & tag, const char * contig_id)

// Print to  fp  strings in  tag  corresponding to alignments in this
// multialignment that are empty.  For each, also print  contig_id

  {
   int  i, n, ct = 0;

   n = align . size ();
   for (i = 0; i < n; i ++)
     if (align [i] . Is_Empty ())
       {
         fprintf (fp, "%-10s  %s\n", tag [i], contig_id);
         ct ++;
       }

   return ct;
  }



void  Gapped_Multi_Alignment_t :: Print_Ungapped_Consensus
    (FILE * fp, char * hdr, int width)

//  Print to  fp  the non-gap entries in this multialignment's
//  consensus sequence in fasta-style.  Print  hdr  on the
//  fasta-header line.  Print  width  characters per line.

  {
   int  i, j, n;

   fprintf (fp, ">%s\n", hdr);

   n = consensus . length ();
   for  (i = j = 0;  i < n;  i ++)
     if  (consensus [i] != '-')
         {
          fputc (consensus [i], fp);
          j ++;
          if  (j == width)
              {
               fputc ('\n', fp);
               j = 0;
              }
         }

   if  (j > 0)
       fputc ('\n', fp);

   return;
  }



void  Gapped_Multi_Alignment_t :: Reverse_Complement
    (vector <char *> & s)

//  Reverse-complement the consensus of this multialignment
//  and all the entries in  s , and adjust the alignments
//  of all the component strings (which are in  s ).

  {
   int  cons_len;
   int  i, n;

   :: Reverse_Complement (consensus);
   Reverse_String (con_qual);

   cons_len = consensus . length ();

   if  (align . size () != s . size ())
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  GMA reverse_complement called with %d strings and %d alignments",
            int (s . size ()), int (align . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
        
       }
   n = align . size ();
   for  (i = 0;  i < n;  i ++)
     {
      int  read_len = strlen (s [i]);

      :: Reverse_Complement (s [i]);
      align [i] . Flip (read_len, cons_len);
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Set_Consensus_And_Qual
    (const vector <char *> & s, const vector <char *> & q)

//  Reset the consensus base values using the libSlice package
//  and assign the corresponding quality for each one in the
//  con_qual string. All ambiguous read bases or consensus base
//  are handled as an N. Strings in alignment are in  s  and
// their quality values are in  q .

  {

   unsigned int  qvsum [5];
   unsigned int  cns_qv;
   char  cns_ch;
   vector <Gapped_MA_Bead_t>  active_bead;
   string  seq_column, qual_column;
   int  col_len;
   int  len, next;
   int  i, j, n;

   len = consensus . length ();
   con_qual . resize (len);
   n = align . size ();

   if  (Verbose > 3)
       {
        printf ("consensus . length () = %d   n = %d\n", len, n);
        for  (i = 0;  i < n;  i ++)
          {
           printf ("> Sequence %d\n", i);
           Fasta_Print (stdout, s [i], NULL);
           Fasta_Print (stdout, q [i], NULL);
          }
       }

   // Take a slice at each sequence position and find its consensus
   next = 0;
   for  (i = 0;  i < len;  i ++)
     {

      Gapped_MA_Bead_t  b;
      vector <Gapped_MA_Bead_t> :: iterator  bp;

      seq_column . erase ();
      qual_column . erase ();

      for  (bp = active_bead . begin ();  bp != active_bead . end (); )
        {
         bp -> Advance ();
         if  (! bp -> active)
             bp = active_bead . erase (bp);
           else
             {
              seq_column . push_back (bp -> seq_ch);
              qual_column . push_back (bp -> qual_ch - MIN_QUALITY);
              bp ++;
             }
        }

      while  (next < n && align [next] . b_lo == i)
        {
         b . seq = s [next];
         b . qual = q [next];
         b . ga = & (align [next]);
         b . a_pos = b . ga -> a_lo;
         b . b_pos = i;
         b . skip_i = 0;
         b . skip_pos = b . ga -> Get_Skip (b . skip_i);
         b . seq_ch = s [next] [b . a_pos];
         b . qual_ch = q [next] [b . a_pos];
         b . active = true;
         active_bead . push_back (b);
         seq_column . push_back (b . seq_ch);
         qual_column . push_back (b . qual_ch - MIN_QUALITY);
         next ++;
        }

      // Array of sum of quality scores in the slice for A,C,G,T,- resp.
      for (j = 0; j < 6; j ++)
        qvsum [j] = 0;

      int nof_ambiguities = 0;
      col_len = seq_column . length ();
      for (j = 0; j < col_len; j ++)
        switch (tolower (seq_column [j]))
          {
          case 'a' :
            qvsum [0] += qual_column [j];
            break;
          case 'c' :
            qvsum [1] += qual_column [j];
            break;
          case 'g' :
            qvsum [2] += qual_column [j];
            break;
          case 't' :
            qvsum [3] += qual_column [j];
            break;
          case '-' :
            qvsum [4] += qual_column [j];
            break;
          default:
            // All other bases are considered an 'N'
            nof_ambiguities += 1;
            break;
          }

      libSlice_ConsensusLight (qvsum, nof_ambiguities, & cns_ch, & cns_qv);

      consensus [i] = cns_ch;
      con_qual [i] = MIN_QUALITY + Min (cns_qv, unsigned (MAX_QUALITY_CHAR));

      if  (Verbose > 3)
          {
           printf ("%6d:  ", i);
           cout << seq_column << "  col_len = " << col_len << endl;
           printf ("         ");
           for  (j = 0;  j < col_len;  j ++)
             putchar (qual_column [j] + MIN_QUALITY);
           putchar ('\n');
           printf ("         ");
           for  (j = 0;  j < col_len;  j ++)
             printf (" %02d", (signed) qual_column [j]);
           putchar ('\n');

           cout << "         cons = " << consensus [i] << "  qv = "
                << con_qual [i] << endl;
          }
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Set_Flipped
    (const vector <AMOS :: Range_t> & clr)

//  Set the flipped bit true for each alignment in this
//  alignment iff the corresponding  clr  begin  value
//  is greater than the  end  value.

  {
   int  i, n;

   n = align . size ();
   if  (int (clr . size ()) < n)
       {
        sprintf (Clean_Exit_Msg_Line,
             "ERROR:  Number of clear ranges %d < number of aligns %d\n",
             int (clr . size ()), n);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   for  (i = 0;  i < n;  i ++)
     if  (clr [i] . end < clr [i] . begin)
         align [i] . flipped = 1;

   return;
  }



void  Gapped_Multi_Alignment_t :: Set_Phase
    (vector <Distinguishing_Column_t> & dc)

//  Determine the phase of each column based on common reads with
//  consistent haplotype patterns

  {
   vector <Phase_Entry_t>  edge_list;
   vector <int>  tree_edge;
   Phase_Entry_t  phase;
   vector <Distinguishing_Column_t> :: iterator  p;
   int  min;
   int  i, j, e, n, t;

   n = dc . size ();
   for  (i = 0;  i < n;  i ++)
     dc [i] . phase_ch = ' ';

   // Eliminate dubious  dc  entries by marking the ones that
   // are bounded by much better ones
   for  (i = 0;  i < n - 1;  i ++)
     {
      int  k, hi_j = i;

      if  (dc [i] . phase_ch == 'D')
          continue;

      for  (j = i + 1;  j < n && dc [i] . Intersects (dc [j], phase, min);  j ++)
        {
         if  (abs (phase . same_ct - phase . opposite_ct) >= 4
                && min >= 4)
             hi_j = j;
        }

      for  (k = i + 1;  k < hi_j;  k ++)
        if  (dc [k] . hapb_sub . size () <= 2)
            dc [k] . phase_ch = 'D';
     }

   if  (Verbose > 2)
       {
        printf ("Dubious Columns:\n");
        for  (i = 0;  i < n;  i ++)
          if  (dc [i] . phase_ch == 'D')
              dc [i] . Print (stdout);
       }

   // Eliminate dubious columns
   p = dc . begin ();
   while  (p != dc . end ())
     if  (p -> phase_ch == 'D')
         dc . erase (p);
       else
         p ++;

   // Create edges between remaining columns
   n = dc . size ();

   for  (i = 0;  i < n - 1;  i ++)
     {
      for  (j = i + 1;  j < n && dc [i] . Intersects (dc [j], phase, min);  j ++)
        {
         phase . from = i;
         phase . to = j;
         phase . weight = abs (phase . same_ct - phase . opposite_ct);
         if  (phase . weight > 0)
             edge_list . push_back (phase);
        }
     }

   sort (edge_list . begin (), edge_list . end (), greater<Phase_Entry_t>());

   e = edge_list . size ();

   if  (Verbose > 2)
       {
        printf ("Edges:\n");
        for  (i = 0;  i < e;  i ++)
          printf ("%4d %4d  %3d %3d  %3d\n", edge_list [i] . from,
               edge_list [i] . to, edge_list [i] . same_ct,
               edge_list [i] . opposite_ct, edge_list [i] . weight);
       }


   vector < vector <Phase_Entry_t> >  tree (n);
     // allocate n empty adjacency lists

   Best_Spanning_Tree (n, edge_list, tree_edge);

   printf ("Vertices = %d\n", n);
   t = tree_edge . size ();
   printf ("Tree edges = %d\n", t);
   for  (i = 0;  i < t;  i ++)
     {
      tree [edge_list [tree_edge [i]] . from]
           . push_back (edge_list [tree_edge [i]]);
      Swap (edge_list [tree_edge [i]] . from,
              edge_list [tree_edge [i]] . to);
      tree [edge_list [tree_edge [i]] . from]
           . push_back (edge_list [tree_edge [i]]);
     }

   if  (Verbose > 2)
       {
        printf ("Tree:\n");
        for  (i = 0;  i < n;  i ++)
          {
           int  j, m;

           printf ("%3d:\n", i);
           m = tree [i] . size ();
           for  (j = 0;  j < m;  j ++)
             tree [i] [j] . Print (stdout);
          }
       }

   for  (i = 0;  i < n;  i ++)
     if  (dc [i] . phase_ch == ' ')
         {
          int  sum = 0;

          Traverse (i, -1, tree, dc, '+', sum);
          if  (sum < 0)
              Traverse (i, -1, tree, dc, '-', sum);
         }

   printf ("DC after Set_Phase\n");
   for  (i = 0;  i < n;  i ++)
     {
      printf ("i = %d:\n", i);
      dc [i] . Print (stdout);
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Set_String_Subs
    (void)

//  Set the  string_sub  field in each component alignment
//  to the same subscript as the component alignemnt.

  {
   int  i, n;

   n = align . size ();
   for  (i = 0;  i < n;  i ++)
     align [i] . string_sub = i;

   return;
  }



void  Gapped_Multi_Alignment_t :: Shift_B_Right
    (int offset)

//  Shift all the  b  values in this alignment to the right
//  by  offset .

  {
   int  i, n;

   n = align . size ();
   for  (i = 0;  i < n;  i ++)
     {
      align [i] . b_lo += offset;
      align [i] . b_hi += offset;
      align [i] . Shift_Skip (offset);
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: Show_Skips
    (FILE * fp)

//  Print the skip values in the alignment to file  fp .

  {
   vector <Gapped_Alignment_t> :: iterator  p;

   for  (p = align . begin ();  p != align . end ();  p ++)
     p -> Dump_Skip (fp);

   return;
  }



void  Gapped_Multi_Alignment_t :: Sort
    (vector <char *> & s, vector <int> * ref, vector <char *> * tag_list)

//  Sort the align entries in this multialignment according to their
//   b_lo  values.  Also sort the strings  s  along with them.
//  If  ref  isn't  NULL , then move its entries in parallel
//  with those of  s  and  align .  Likewise, for  tag_list .

  {
   int  i, j, n;

   n = align . size ();
   if  (n != int (s . size ()))
       {
        sprintf (Clean_Exit_Msg_Line,
             "ERROR:  Sorting %d strings with %d alignments\n",
             int (s . size ()), n);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   for  (i = 1;  i < n;  i ++)
     {
      char  * s_save, * t_save;
      Gapped_Alignment_t  a_save;
      int  r_save;

      if  (align [i - 1] . b_lo <= align [i] . b_lo)
          continue;

      s_save = s [i];
      a_save = align [i];
      if  (ref != NULL)
          r_save = (* ref) [i];
      if  (tag_list != NULL)
          t_save = (* tag_list) [i];

      for  (j = i;  j > 0 && align [j - 1] . b_lo > a_save . b_lo;  j --)
        {
         align [j] = align [j - 1];
         s [j] = s [j - 1];
         if  (ref != NULL)
             (* ref) [j] = (* ref) [j - 1];
         if  (tag_list != NULL)
             (* tag_list) [j] = (* tag_list) [j - 1];
        }

      s [j] = s_save;
      align [j] = a_save;
      if  (ref != NULL)
          (* ref) [j] = r_save;
      if  (tag_list != NULL)
          (* tag_list) [j] = t_save;
     }

   return;
  }



void  Gapped_Multi_Alignment_t :: TA_Print
    (FILE * fp, const vector <char *> & s, const vector <AMOS :: Range_t> & clr_list,
     int width, const vector <char *> * tag, const string & id)

//  Display this multialignment to file  fp  in TIGR Assembler
//  .contig format.  Use  width  characters per line of sequences.
//    s  holds the strings the alignment references and  clr_list  holds
//  the clear ranges of the original reads (in reverse order if the
//  original read has been reversed complemented).  If  tag  is
//  not NULL, it holds IID tags corresponding to the sequences  s
//  to be used to label the sub-alignment lines.   id  is the
//  id number of this contig.

  {
   vector <int>  lookup;
   char  * buff;
   bool  use_string_sub;
   int  clr_begin = 0, clr_end = 0;
   int  len;
   int  i, n, sub;

   use_string_sub = (print_flags & PRINT_USING_STRING_SUB);

   if  (! use_string_sub && s . size () != align . size ())
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align . Print called with %d strings and %d alignments",
            int (s . size ()), int (align . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   n = align . size ();
   len = consensus . length ();
   buff = (char *) Safe_malloc (5 + len, __FILE__, __LINE__);

   Make_Gapped_Ungapped_Lookup (lookup);

   // first print the header information and gapped consensus.
   fprintf (fp, "##%s %d %d bases, 00000000 checksum.\n",
        id . c_str (), n, len);
   Fasta_Print (stdout, getConsensusString (), NULL);

   // Then the gapped reads
   for  (i = 0;  i < n;  i ++)
     {
      bool  flipped;
      int  a_lo, a_hi;

     if  (use_string_sub)
         sub = align [i] . string_sub;
       else
         sub = i;

      if  (tag == NULL)
          sprintf (buff, "%d", i);
        else
          sprintf (buff, "%s", (* tag) [sub]);

      clr_begin = clr_list [sub] . begin;
      clr_end = clr_list [sub] . end;
      if  (clr_begin < clr_end)
          {
           flipped = false;
           clr_begin ++;
          }
        else
          {
           flipped = true;
           clr_end ++;
          }

      fprintf (fp, "#%s(%d) [%s] %d bases, 00000000 checksum. {%d %d} <%d %d>\n",
           buff, align [i] . b_lo,
           flipped ? "RC" : "", align [i] . b_hi - align [i] . b_lo,
           clr_begin, clr_end, 1 + lookup [align [i] . b_lo],
           lookup [align [i] . b_hi]);

      align [i] . Print_Subalignment_Line
           (buff, align [i] . b_lo, align [i] . b_hi, s [sub],
            a_lo, a_hi);

      Uppercase (buff);
      Fasta_Print (stdout, buff, NULL);
     }

   return;
  }


void Gapped_Multi_Alignment_t :: Make_AMOS_Contig 
     (const vector <AMOS :: Range_t> & clr_list, const vector <char *> & tag,
      AMOS :: Contig_t & out) const
  // returns a Contig_t representation of the multi-alignment
{
  out . setSequence (consensus, con_qual);
  out . setIID (strtol (id . c_str (), NULL, 10));

  int n = align . size ();

  vector <Tile_t> tiles;
  for (int i = 0; i < n; i ++)
    {
      AMOS :: Tile_t tile;

      if (align [i] . Is_Empty ())
        continue;
      // skip empty alignments which indicate expelled strings

      tile . source = strtol(tag [i], NULL, 10);
      if (tile.source == 0){
        cerr << "Source of tag " << tag[i] << " is null\n";
      }
      tile . range = clr_list [i];
      tile . offset = align [i] . b_lo;
    
      int skip = align [i] . Get_Skip (0);
      int s = 0;
      while (skip != INT_MAX) {
        tile . gaps . push_back (skip - tile . offset - s); // convert to CA standard
        skip = align [i] . Get_Skip(++ s);
    }

    tiles . push_back(tile);
  }
  out . setReadTiling(tiles);
}



int  Gapped_Multi_Alignment_t :: Ungapped_Consensus_Len
    (void)  const

//  Return the number of non-'-' characters in this
//  multialignment's consensus sequence.

  {
   int  i, n, ct;

   n = consensus . length ();
   ct = 0;
   for  (i = 0;  i < n;  i ++)
     if  (consensus [i] != '-')
         ct ++;

   return  ct;
  }




// ###  Independent Functions  ###


void  Align_Row_Update
    (vector <Augmented_Score_Entry_t> & align_row, char ch, const char * s,
     int s_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, int first_score)

//  Update  align_row  to be the values in the next row of an alignment
//  matrix where the preceding row is in  align_row  now.   ch  is
//  the character for the next row and  s  (whose length is  s_len )
//  is the characters across the top of the alignment matrix.
//   match_score ,  mismatch_score ,  indel_score  and  gap_score
//  are the scores for matching characters, mismatching characters,
//  insertion/deletion and initiating a gap, respectively.
//   first_score  is the value to add to the first entry's  top_score  field.

  {
   Augmented_Score_Entry_t  prev, curr;
   int  mxs;
   unsigned int  mxf;
   int  i;

   if  (s_len == 0)
       return;

   assert (int (align_row . size ()) == s_len + 1);

   prev = align_row [0];
   align_row [0] . top_score += first_score;
   align_row [0] . diag_score = align_row [0] . left_score = NEG_INFTY_SCORE;

   for  (i = 1;  i <= s_len;  i ++)
     {
      Augmented_Score_Entry_t  * p = & (align_row [i]);

      curr = align_row [i];
      curr . Get_Max_Top (mxs, mxf, gap_score);
      
      p -> top_score = mxs + indel_score;
      p -> top_from = mxf;
      p -> top_ref = curr . Get_Ref (mxf);

      prev . Get_Max (mxs, mxf);
      p -> diag_score = mxs
           + ((ch == s [i - 1]) ? match_score : mismatch_score);
      p -> diag_from = mxf;
      p -> diag_ref = prev . Get_Ref (mxf);

      align_row [i - 1] .  Get_Max_Left (mxs, mxf, gap_score);
      p -> left_score = mxs + indel_score;
      p -> left_from = mxf;
      p -> left_ref = align_row [i - 1] . Get_Ref (mxf);

      prev = curr;
     }

   return;
  }



void  Align_Row_Update_With_Errors
    (vector <Errors_Score_Entry_t> & align_row, char ch, const char * s,
     int s_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, int first_score, int first_error)

//  Update  align_row  to be the values in the next row of an alignment
//  matrix where the preceding row is in  align_row  now.   ch  is
//  the character for the next row and  s  (whose length is  s_len )
//  is the characters across the top of the alignment matrix.
//   match_score ,  mismatch_score ,  indel_score  and  gap_score
//  are the scores for matching characters, mismatching characters,
//  insertion/deletion and initiating a gap, respectively.
//   first_score  is the value to add to the first entry's  top_score  field.
//   first_error  is the value to add to the first entry's  top_errors  field.

  {
   Errors_Score_Entry_t  prev, curr;
   int  mxs;
   unsigned int  mxf;
   int  i;

   if  (s_len == 0)
       return;

   assert (int (align_row . size ()) == s_len + 1);

   prev = align_row [0];
   align_row [0] . top_score += first_score;
   align_row [0] . diag_score = align_row [0] . left_score = NEG_INFTY_SCORE;
   align_row [0] . top_errors += first_error;
   align_row [0] . diag_errors = align_row [0] . left_errors = 0;

   for  (i = 1;  i <= s_len;  i ++)
     {
      Errors_Score_Entry_t  * p = & (align_row [i]);

      curr = align_row [i];
      curr . Get_Max_Top (mxs, mxf, gap_score);
      
      p -> top_score = mxs + indel_score;
      p -> top_from = mxf;
      p -> top_ref = curr . Get_Ref (mxf);
      p -> top_errors = curr . Get_Errors (mxf) + 1;

      prev . Get_Max (mxs, mxf);
      p -> diag_score = mxs
           + ((ch == s [i - 1]) ? match_score : mismatch_score);
      p -> diag_from = mxf;
      p -> diag_ref = prev . Get_Ref (mxf);
      p -> diag_errors = prev . Get_Errors (mxf);
      if  (ch != s [i - 1])
          p -> diag_errors ++;

      align_row [i - 1] .  Get_Max_Left (mxs, mxf, gap_score);
      p -> left_score = mxs + indel_score;
      p -> left_from = mxf;
      p -> left_ref = align_row [i - 1] . Get_Ref (mxf);
      p -> left_errors = align_row [i - 1] . Get_Errors (mxf) + 1;

      prev = curr;
     }

   return;
  }



void  Banded_Overlap
    (const char * s, int s_len, const char * t, int t_len,
     int  lo_offset, int hi_offset, Simple_Overlap_t & olap,
     int match_score,
     int mismatch_score,
     int indel_score,
     int gap_score)

//  Find the highest-scoring overlap alignment between string  s  and
//  string  t , restricted to a band between  lo_offset  and
//  hi_offset , inclusive.  Offsets are measured by the position in
//   t  minus the position in  s  of a corresponding base.
//  The length of  s  is  s_len  and the length of  t  is  t_len .
//  The resulting overlap is stored in  olap .
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).

  {
   vector <Errors_Score_Entry_t> band;
     //  row of alignment array
   Errors_Score_Entry_t  entry;
   int  max_row, max_col, max_score, mxs, max_ref, max_errors;
   unsigned int  max_from, mxf;
   int  bandwidth;
   int  left_col, right_col;
     // bound region represented by band
   int  r, c;

   if  (s_len + lo_offset < 0)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "lo_offset=%d longer than s_len=%d in  Banded_Overlap",
                 lo_offset, s_len);
        throw AlignmentException_t (Clean_Exit_Msg_Line, __LINE__, __FILE__,
                                    -1, -1);
       }
   if  (t_len < hi_offset)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "hi_offset=%d longer than t_len=%d in  Banded_Overlap",
                 hi_offset, t_len);
        throw AlignmentException_t (Clean_Exit_Msg_Line, __LINE__, __FILE__,
                                    -1, -1);
       }

   bandwidth = 1 + hi_offset - lo_offset;
   max_score = NEG_INFTY_SCORE;

   // Create the first row of the alignment matrix
   entry . diag_score = entry . top_score = entry . left_score = 0;
   entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
   entry . diag_errors = entry . top_errors = entry . left_errors = 0;

   if  (lo_offset <= 0)
       {
        r = 0;
        right_col = - lo_offset;
       }
     else
       {
        r = lo_offset;
        right_col = 0;
       }
   left_col = 1 + right_col - bandwidth;

   // Create band
   for  (c = left_col;  c <= right_col;  c ++)
     {
      entry . top_ref = entry . diag_ref = entry . left_ref = c;
      band . push_back (entry);
     }

   if  (right_col == s_len || r == t_len)
       {
        entry . Get_Max (max_score, max_from);
        max_row = 0;
        max_col = s_len;
        max_ref = s_len;
        max_errors = entry . Get_Errors (max_from);
       }

   // Do remaining rows
   while  (left_col < s_len && r < t_len)
     {
      if  (left_col <= 0)
          band [- left_col] . top_ref = band [- left_col] . diag_ref
           = band [- left_col] . left_ref = - r;
              // is value of current row, not one being built

      Update_Banded_Row (band, t [r], s, s_len, left_col, right_col,
           match_score, mismatch_score, indel_score, gap_score, 0, 0);
      r ++;
      left_col ++;
      right_col ++;

      // Check last column entry for max
      if  (left_col <= s_len && s_len <= right_col)
          {
           band [s_len - left_col] . Get_Max (mxs, mxf);
           if  (mxs > max_score)
               {
                max_score = mxs;
                max_from = mxf;
                max_row = r;
                max_col = s_len;
                max_ref = band [s_len - left_col] . Get_Ref (mxf);
                max_errors = band [s_len - left_col] . Get_Errors (mxf);
               }
          }
     }

   // Check entries in last row for max
   if  (r == t_len)
       {
        int  lo, hi;

        lo = Max (0, - left_col);
        hi = Min (bandwidth - 1, s_len - left_col);
        for  (c = lo;  c <= hi;  c ++)
          {
           band [c] . Get_Max (mxs, mxf);
           if  (mxs > max_score)
               {
                max_score = mxs;
                max_from = mxf;
                max_row = t_len;
                max_col = left_col + c;
                max_ref = band [c] . Get_Ref (mxf);
                max_errors = band [c] . Get_Errors (mxf);
               }
          }
       }

   olap . score = max_score;
   olap . errors = max_errors;
   olap . a_hang = max_ref;
   if  (max_col < s_len)
       olap . b_hang = max_col - s_len;
     else
       olap . b_hang = t_len - max_row;
   if  (max_ref >= 0)
       {
        olap . b_olap_len = max_row;
        olap . a_olap_len = max_col - max_ref;
       }
     else
       {
        olap . b_olap_len = max_row + max_ref;
        olap . a_olap_len = max_col;
       }

   return;
  }



void  Best_Spanning_Tree
    (int n, const vector <Phase_Entry_t> & edge_list,
     vector <int> & tree_edge)

//  Find the best spanning tree of graph of  n  vertices,  0 .. (n-1)
//  using edges in  edge_list  in order.  Put subscripts of edges
//  used in  tree_edge .  Use  Union-Find  algorithm.

  {
   vector <int>  uf (n, -1);
   int  i, e;

   tree_edge . clear ();

   e = edge_list . size ();
   for  (i = 0;  i < e;  i ++)
     {
      int  j, k;

edge_list [i] . Print (stdout);
      j = UF_Find (edge_list [i] . from, uf);
      k = UF_Find (edge_list [i] . to, uf);
printf ("  j = %d  k = %d\n", j, k);
      if  (j != k)
          {
           tree_edge . push_back (i);
           UF_Union (j, k, uf);
          }
     }

   return;
  }


void  Classify_Reads
    (const vector <Distinguishing_Column_t> & dc, int n,
     vector <char> & side, vector <int> & segment)

//  Assign haplotypes to reads  0 .. (n-1)  based on the
//  entries in  dc .

  {
   vector < vector <Phase_Entry_t> >  A (n);
   vector <Phase_Entry_t>  edge_list;
   Phase_Entry_t  phase;
   vector <int>  uf (n, -1);
   int  len;
   int  e, i, j, k, m;

   m = dc . size ();
   for  (i = 0;  i < m;  i ++)
     {
      int  a_len, b_len;
      int  from, to;

      a_len = dc [i] . hapa_sub . size ();
      b_len = dc [i] . hapb_sub . size ();

      for  (j = 0;  j < a_len;  j ++)
        for  (k = 0;  k < b_len;  k ++)
          {
           from = dc [i] . hapa_sub [j];
           to = dc [i] . hapb_sub [k];
           if  (from < to)
               Incr_Opposite (A [from], from, to);
             else
               Incr_Opposite (A [to], to, from);
          }
     }

   printf ("Opposite Counts  n = %d:\n", n);
   for  (i = 0;  i < n;  i ++)
     {
      len = A [i] . size ();
      printf ("%3d (%2d): ", i, len);
      for  (j = 0;  j < len;  j ++)
        if  (A [i] [j] . opposite_ct > 0)
            printf (" %3d/%-3d", A [i] [j] . to, A [i] [j] . opposite_ct);
      putchar ('\n');
     }

   // Make a list of edges
   for  (i = 0;  i < n;  i ++)
     {
      len = A [i] . size ();
      for  (j = 0;  j < len;  j ++)
        {
         phase . from = i;
         phase . to = A [i] [j] . to;
         phase . weight = A [i] [j] . opposite_ct;

         edge_list . push_back (phase);
        }
     }

   // Sort into descending order by weight
   sort (edge_list . begin (), edge_list . end (), greater<Phase_Entry_t>());

   // Greedily find approximate best bipartite subgraph
   e = edge_list . size ();
   for  (i = 0;  i < e;  i ++)
     {
      int  j_par, k_par;
      int  from, to;

      from = edge_list [i] . from;
      to = edge_list [i] . to;

      j = UF_Find_With_Parity (from, uf, j_par);
      k = UF_Find_With_Parity (to, uf, k_par);

      if  (j != k)
          {
           if  (j_par == k_par)
               UF_Union (j, k, uf);
             else
               {  // break usual UF rules to ensure that  from  and
                  //  to  get opposite sides of the bipartite graph
                if  (j == from)
                    {
                     uf [k] += uf [j];
                     uf [j] = to;
                    }
                else if  (k == to)
                    {
                     uf [j] += uf [k];
                     uf [k] = from;
                    }
                else if  (uf [j] <= uf [k])
                    {  // k is smaller since values are negative sizes
                     uf [j] += uf [k];
                     uf [k] = from;
                    }
               }
          }
     }

   side . resize (n);
   segment . resize (n);

   printf ("Bipartite Segments:\n");
   for  (i = 0;  i < n;  i ++)
     {
      char  ch;
      int  par;

      j = UF_Find_With_Parity (i, uf, par);
      if  (par == 0)
          ch = 'a';
        else
          ch = 'b';
      side [i] = ch;
      segment [i] = j;

      printf ("%4d %4d  %c\n", i, j, ch);
     }

   return;
  }



void  Complete_Align
    (const char * s, int s_lo, int s_hi, const char * t, int t_lo, int t_slip,
     int t_hi, int match_score, int mismatch_score, int indel_score,
     int gap_score, Align_Score_Entry_t * first_entry,
     Align_Score_Entry_t & last_entry, Alignment_t & align)

//  Find the best alignment of the entire string  s [s_lo .. (s_hi - 1)]
//  to the entire string  t [t_lo .. (t_hi - 1)]  but the alignment in
//   t  may start between  t_lo  and  t_slip .
//  If  first_entry  is not  NULL , use it for the first entry in
//  the alignment matrix; otherwise, use a zero value.
//  Set  last_entry  to the last entry in the alignment matrix.
//  The resulting alignment is stored in  align ,
//  where  s  is the  a  string and  t  is the  b  string.
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).

  {
   int  matrix_size;

   if  (Verbose > 3)
       fprintf (stderr,
            "Complete_Align:  s_lo/hi = %d/%d  t_lo/slip/hi = %d/%d/%d\n",
            s_lo, s_hi, t_lo, t_slip, t_hi);

   assert (t_lo < t_slip);
   assert (t_slip <= t_hi);
   assert (s_lo <= s_hi );

   matrix_size = (s_hi - s_lo) * (t_hi - t_lo);
   if  (t_hi - t_lo <= 10 || matrix_size <= MATRIX_SIZE_LIMIT)
       Complete_Align_Full_Matrix (s, s_lo, s_hi, t, t_lo, t_slip, t_hi,
            match_score, mismatch_score, indel_score, gap_score,
            first_entry, last_entry, align);
     else
       Complete_Align_Save_Space (s, s_lo, s_hi, t, t_lo, t_slip, t_hi,
            match_score, mismatch_score, indel_score, gap_score,
            first_entry, last_entry, align);

   return;
  }



void  Complete_Align_Full_Matrix
    (const char * s, int s_lo, int s_hi, const char * t, int t_lo, int t_slip,
     int t_hi, int match_score, int mismatch_score, int indel_score,
     int gap_score, Align_Score_Entry_t * first_entry,
     Align_Score_Entry_t & last_entry, Alignment_t & align)

//  Find the best alignment of the entire string  s [s_lo .. (s_hi - 1)]
//  to the entire string  t [t_lo .. (t_hi - 1)]  but the alignment in
//   t  may start between  t_lo  and  t_slip .
//  If  first_entry  is not  NULL , use it for the first entry in
//  the alignment matrix; otherwise, use a zero value.
//  Set  last_entry  to the last entry in the alignment matrix.
//  The resulting alignment is stored in  align ,
//  where  s  is the  a  string and  t  is the  b  string.
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).
//  Use a full matrix for the computation.

  {
   vector < vector <Align_Score_Entry_t> > a;  // the alignment array
   vector <Align_Score_Entry_t>  empty_vector;
   vector <int>  delta;
   Align_Score_Entry_t  entry;
   unsigned int  mxf;
   int  mxs, s_len, t_len, error_ct, row_limit;
   int  r, c;    // row and column
   int  i, j, n;  // position in string  t

   if  (Verbose > 3)
       {
        fprintf (stderr, "Start Complete_Align_Full_Matrix\n");
        fprintf (stderr, "s_lo = %d  s_hi = %d  t_lo = %d  t_slip = %d  t_hi = %d\n",
             s_lo, s_hi, t_lo, t_slip, t_hi);
        fprintf (stderr, "Scores:  match/mismatch/indel/gap = %d/%d/%d/%d\n",
             match_score, mismatch_score, indel_score, gap_score);
       }

   s_len = s_hi - s_lo;
   t_len = t_hi - t_lo;

   // Do first row
   a . push_back (empty_vector);
   if  (first_entry != NULL)
       entry = * first_entry;
     else
       {
        entry . diag_score = entry . top_score = entry . left_score = 0;
        entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
       }
   r = c = 0;
   a [r] . push_back (entry);

   entry . top_score = NEG_INFTY_SCORE;
   entry . top_from = FROM_NOWHERE;
   entry . diag_score = NEG_INFTY_SCORE;
   entry . diag_from = FROM_NOWHERE;
   for  (c = 1;  c <= s_len;  c ++)
     {
      entry . left_score = a [r] [c - 1] . left_score + indel_score;
      if  (c == 1)
          entry . left_score += gap_score;
      entry . left_from = FROM_LEFT;
      a [r] . push_back (entry);
     }
     
   // Do remaining rows
   for  (i = t_lo;  i < t_hi;  i ++)
     {
      r ++;

      // First column in row
      a . push_back (empty_vector);
      entry . diag_score = entry . left_score = NEG_INFTY_SCORE;
      if  (i < t_slip - 1)
          {
           entry . top_score = 0;
           entry . top_from = FROM_NOWHERE;
          }
        else
          {
           entry . top_score = a [r - 1] [0] . top_score + indel_score;
           if  (i == t_slip - 1)
               entry . top_score += gap_score;
           entry . top_from = FROM_TOP;
          }
      entry . diag_from = entry . left_from = FROM_NOWHERE;
      a [r] . push_back (entry);

      // Remaining columns in row
      j = s_lo;
      for  (c = 1;  c <= s_len;  c ++, j ++)
        {
         Align_Score_Entry_t  * p;

         p = & (a [r - 1] [c]);
         p -> Get_Max_Top (mxs, mxf, gap_score);
         entry . top_score = mxs + indel_score;
         entry . top_from = mxf;

         p = & (a [r - 1] [c - 1]);
         p -> Get_Max (mxs, mxf);
         entry . diag_score = mxs;
         entry . diag_from = mxf;
         entry . diag_score += ((t [i] == s [j]) ? match_score : mismatch_score);

         p = & (a [r] [c - 1]);
         p -> Get_Max_Left (mxs, mxf, gap_score);
         entry . left_score = mxs + indel_score;
         entry . left_from = mxf;
         a [r] . push_back (entry);
        }
     }

   last_entry = a [t_len] [s_len];
   last_entry . Get_Max (mxs, mxf);
   if  (Verbose > 1)
       printf ("Complete_Align_Full_Matrix:  final score = %d\n", mxs);

   // Trace back
   row_limit = t_slip - t_lo - 1;
   Trace_Back_Align_Path (a, r, s_len, row_limit, 0, delta,
        error_ct, s + s_lo, t + t_lo);

   align . a_lo = s_lo;
   align . a_hi = s_hi;
   align . b_hi = t_hi;
   align . b_lo = r + t_lo;
   align . errors = error_ct;

   if  (Verbose > 2)
       {
        n = delta . size ();
        for  (i = 0;  i < n;  i ++)
          printf ("delta [%d] = %d\n", i, delta [i]);
       }

   align . setDelta (delta);

   return;
  }



void  Complete_Align_Save_Space
    (const char * s, int s_lo, int s_hi, const char * t, int t_lo, int t_slip,
     int t_hi, int match_score, int mismatch_score, int indel_score,
     int gap_score, Align_Score_Entry_t * first_entry,
     Align_Score_Entry_t & last_entry, Alignment_t & align)

//  Find the best alignment of the entire string  s [s_lo .. (s_hi - 1)]
//  to the entire string  t [t_lo .. (t_hi - 1)]  but the alignment in
//   t  may start between  t_lo  and  t_slip .
//  If  first_entry  is not  NULL , use it for the first entry in
//  the alignment matrix; otherwise, use a zero value.
//  Set  last_entry  to the last entry in the alignment matrix.
//  The resulting alignment is stored in  align ,
//  where  s  is the  a  string and  t  is the  b  string.
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).
//  Do recursively keeping just a single row.

  {
   Alignment_t  sub_ali_1, sub_ali_2;
   vector <Augmented_Score_Entry_t> align_row;
        //  row of alignment array
   Augmented_Score_Entry_t  entry;
   int  s_len, t_len, t_half, tmp_slip;
   int  mxs, max_ref;
   unsigned int  mxf;
   int  r, c;
   
   s_len = s_hi - s_lo;
   t_len = t_hi - t_lo;
   assert (t_len > 1);

   // Build first row
   if  (first_entry == NULL)
       {
        entry . diag_score = entry . top_score = entry . left_score = 0;
        entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
       }
     else
       {
        entry . diag_score = first_entry -> diag_score;
        entry . top_score = first_entry -> top_score;
        entry . left_score = first_entry -> left_score;
        entry . diag_from = first_entry -> diag_from;
        entry . top_from = first_entry -> top_from;
        entry . left_from = first_entry -> left_from;
       }
   entry . top_ref = entry . diag_ref = entry . left_ref = 0;
   align_row . push_back (entry);

   entry . top_score = NEG_INFTY_SCORE;
   entry . top_from = FROM_NOWHERE;
   entry . diag_score = NEG_INFTY_SCORE;
   entry . diag_from = FROM_NOWHERE;
   entry . left_from = FROM_LEFT;
   entry . left_ref = 0;
   entry . top_ref = entry . diag_ref = -1;

   for  (c = 1;  c <= s_len;  c ++)
     {
      entry . left_score = align_row [c - 1] . left_score + indel_score;
      if  (c == 1)
          entry . left_score += gap_score;
      align_row . push_back (entry);
     }

   // Do remaining rows
   t_half = (t_lo + t_hi) / 2;
   for  (r = t_lo + 1;  r <= t_hi;  r ++)
     {
      int  first_score;

      if  (r < t_slip)
          first_score = 0;
      else if  (r == t_slip)
          first_score = indel_score + gap_score;
        else
          first_score = indel_score;

      Align_Row_Update (align_row, t [r - 1], s + s_lo, s_len, match_score,
           mismatch_score, indel_score, gap_score, first_score);

      if  (r == t_half)
          {  // set the ref entries to reference this row
           align_row [0] . top_ref = 0;
           for  (c = 1;  c <= s_len;  c ++)
             {
              align_row [c] . top_ref = align_row [c] . diag_ref = c;
              align_row [c - 1] . Get_Max_Left (mxs, mxf, gap_score);
              align_row [c] . left_ref = align_row [c - 1] . Get_Ref (mxf);
             }
          }
     }

   // Get max from last entry
   align_row [s_len] . Get_Max (mxs, mxf);
   max_ref = align_row [s_len] . Get_Ref (mxf);

   tmp_slip = Min (t_slip, t_half);
   if  (Verbose > 3)
       fprintf (stderr, "max_ref = %d  t_lo = %d  t_slip = %d  t_half = %d  t_hi = %d\n",
            max_ref, t_lo, t_slip, t_half, t_hi);
   Complete_Align_Full_Matrix (s, s_lo, s_lo + max_ref, t, t_lo, tmp_slip, t_half,
        match_score, mismatch_score, indel_score, gap_score, first_entry,
        entry, sub_ali_1);

   if  (max_ref == 0)
       tmp_slip = Max (t_slip, t_half + 1);
     else
       tmp_slip = t_half + 1;
   Complete_Align_Full_Matrix (s, s_lo + max_ref, s_hi, t, t_half, tmp_slip,
        t_hi, match_score, mismatch_score, indel_score, gap_score,
        & entry, last_entry, sub_ali_2);

   align . Combine (sub_ali_1, sub_ali_2);

   return;
  }



int  DNA_Char_To_Sub
    (char ch)

//  Return the subscript  0 .. 4  corresponding to  ch
//  acgt-, respectively.  Return  -1  if  ch  is not
//  one of these characters

  {
   switch (tolower (ch))
     {
      case  'a' :
        return  0;
      case  'c' :
        return  1;
      case  'g' :
        return  2;
      case  't' :
        return  3;
      case  '-' :
        return  4;
      default :
        return  -1;
     }
  }



int  Exact_Prefix_Match
    (const char * s, const char * t, int max_len)

//  Return the length of the longest exact prefix match of
//  strings  s  and  t , not to exceed  max_len .

  {
   int  i;

   if  (Verbose > 5)
       {
        fprintf (stderr, "In Exact_Prefix_Match  max_len = %d\n", max_len);
        fprintf (stderr, "s :\n");
        Fasta_Print (stderr, s, NULL);
        fprintf (stderr, "t :\n");
        Fasta_Print (stderr, t, NULL);
       }

   for  (i = 0;  i < max_len && s [i] == t [i];  i ++)
//**ALD  Allow N's to match anything
//   for  (i = 0;  i < max_len
//             && (s [i] == t [i] || tolower (s [i]) == 'n'
//                  || tolower (t [i]) == 'n');  i ++)
     ;

   if  (Verbose > 3)
       {
        fprintf (stderr, "Exact_Prefix_Match  return value = %d\n", i);
       }

   return  i;
  }



int  Gapped_Equivalent
    (int pos, const string & s)

//  Return the gapped position in  s  that is equivalent to
//  pos  in an ungapped sequence.  The return position is
//  the largest possible value, i.e., we slide over as many
//  '-'s as possible.

  {
   int  i, j, n;

   n = s . length ();
   for  (i = j = 0;  i < n && j <= pos;  i ++)
     if  (s [i] != '-')
         {
          if  (j == pos)
              return  i;
          j ++;
         }

   return  i;
  }



void  Global_Align
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align)

//  Find the best global alignment of the entire string  s  to a
//  substring of  t  between positions  t_lo  and  t_hi .
//  The length of  s  is  s_len  and the length of  t  is at least
//   t_hi .  The resulting alignment is stored in  align ,
//  where  s  is the  a  string and  t  is the  b  string.
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).

  {
   vector < vector <Align_Score_Entry_t> > a;  // the alignment array
   vector <Align_Score_Entry_t>  empty_vector;
   vector <int>  delta;
   Align_Score_Entry_t  entry;
   int  r, c;    // row and column
   int  max_row, max_score;
   unsigned int  max_from;
   int  mxs;
   unsigned int  mxf;
   int  ct, sign;
   int  i, n;  // position in string  t

   assert (t_lo <= t_hi);
   assert (0 <= s_len );

   // Do first row
   a . push_back (empty_vector);
   entry . diag_score = entry . top_score = entry . left_score = 0;
   entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
   r = 0;
   a [r] . push_back (entry);
   for  (c = 1;  c <= s_len;  c ++)
     {
      entry . top_score = NEG_INFTY_SCORE;
      entry . top_from = FROM_NOWHERE;
      entry . diag_score = NEG_INFTY_SCORE;
      entry . diag_from = FROM_NOWHERE;
      entry . left_score = a [r] [c - 1] . left_score + indel_score;
      if  (c == 1)
          entry . left_score += gap_score;
      entry . left_from = FROM_LEFT;
      a [r] . push_back (entry);
     }
   entry . Get_Max (max_score, max_from);
   max_row = 0;
     
   // Do remaining rows
   for  (i = t_lo;  i < t_hi;  i ++)
     {
      r ++;

      // First column in row
      a . push_back (empty_vector);
      entry . diag_score = entry . left_score = NEG_INFTY_SCORE;
      entry . top_score = 0;
      entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
      a [r] . push_back (entry);

      // Remaining columns in row
      for  (c = 1;  c <= s_len;  c ++)
        {
         Align_Score_Entry_t  * p;

         p = & (a [r - 1] [c]);
         p -> Get_Max (mxs, mxf);
         if  (mxf != FROM_TOP)
             mxs += gap_score;
         mxs += indel_score;
         entry . top_score = mxs;
         entry . top_from = mxf;

         p = & (a [r - 1] [c - 1]);
         if  (p -> left_score < p -> diag_score)
             {
              entry . diag_score = p -> diag_score;
              entry . diag_from = FROM_DIAG;
             }
           else
             {
              entry . diag_score = p -> left_score;
              entry . diag_from = FROM_LEFT;
             }
         if  (entry . diag_score < p -> top_score)
             {
              entry . diag_score = p -> top_score;
              entry . diag_from = FROM_TOP;
             }
         entry . diag_score += ((t [i] == s [c - 1]) ? match_score : mismatch_score);

         p = & (a [r] [c - 1]);
         if  (p -> left_score < p -> diag_score)
             {
              entry . left_score = p -> diag_score + gap_score;
              entry . left_from = FROM_DIAG;
             }
           else
             {
              entry . left_score = p -> left_score;   // Don't add gap_score here
              entry . left_from = FROM_LEFT;
             }
         if  (entry . left_score < p -> top_score)
             {
              entry . left_score = p -> top_score + gap_score;
              entry . left_from = FROM_TOP;
             }
         entry . left_score += indel_score;
         a [r] . push_back (entry);
#if  0
entry . Get_Max (mxs, mxf);
printf ("r = %d  c = %d  mxs = %d  mxf = %u  max_score = %d  max_row = %d\n",
     r, c, mxs, mxf, max_score, max_row);
printf ("L = %d:%u  D = %d:%u  T = %d:%u\n", entry . left_score, entry . left_from,
     entry . diag_score, entry . diag_from, entry . top_score, entry . top_from);
#endif
        }

      entry . Get_Max (mxs, mxf);
      if  (mxs > max_score)
          {
           max_score = mxs;
           max_from = mxf;
           max_row = r;
          }
     }

   r = max_row;
   c = s_len;
   ct = 0;
   sign = 0;
   while  (c > 0)
     {
#if  0
{
 int  mxs;
 unsigned int  mxf;

 a [r] [c] . Get_Max (mxs, mxf);

 printf ("r = %d  c = %d  mxs = %d  mxf = %u\n", r, c, mxs, mxf);
 printf ("sign = %d  ct = %d\n", sign, ct);
}
#endif
      switch  (max_from)
        {
         case  FROM_LEFT :
           if  (sign != 0)
               {
                if  (Verbose > 2)
                    printf ("del = %d\n", sign * ct);
                delta . push_back (sign * ct);
               }
           ct = 1;
           sign = -1;
           max_from = a [r] [c] . left_from;
           c --;
           break;
         case  FROM_DIAG :
           ct ++;
           max_from = a [r] [c] . diag_from;
           r --;
           c --;
           break;
         case  FROM_TOP :
           if  (sign != 0)
               {
                if  (Verbose > 2)
                    printf ("del = %d\n", sign * ct);
                delta . push_back (sign * ct);
               }
           ct = 1;
           sign = 1;
           max_from = a [r] [c] . top_from;
           r --;
           break;
         default :
           sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad from = %u in Global_Align",
                max_from);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
        }
     }

   if  (sign != 0)
       {
        if  (Verbose > 2)
            printf ("del = %d\n", sign * ct);
        delta . push_back (sign * ct);
       }

   align . a_lo = 0;
   align . a_hi = s_len;
   align . b_hi = max_row + t_lo;
   align . b_lo = r + t_lo;

   reverse (delta . begin (), delta . end ());
   n = delta . size ();
   if  (Verbose > 2)
        for  (i = 0;  i < n;  i ++)
          printf ("delta [%d] = %d\n", i, delta [i]);

   align . setDelta (delta);

   return;
  }



void  Incr_Opposite
    (vector <Phase_Entry_t> & v, int from, int to)

//  Increment  opposite_ct  in  v [i]  that matches  from  and
//   to .  If none is found, then add a new one and set its
//   oppostie_ct  to  1  and its  same_ct  to  0 .

  {
   Phase_Entry_t  p;
   int  i, n;

   n = v . size ();
   for  (i = 0;  i < n;  i ++)
     if  (v [i] . to == to)
         {
          assert (v [i] . from = from);
          v [i] . opposite_ct ++;
          return;
         }

   p . from = from;
   p . to = to;
   p . opposite_ct = 1;
   p . same_ct = 0;
   p . weight = 0;

   v . push_back (p);

   return;
  }



void  Incr_Same
    (vector <Phase_Entry_t> & v, int from, int to)

//  Increment  same_ct  in  v [i]  that matches  from  and
//   to .  If none is found, then add a new one and set its
//   same_ct  to  1  and its  opposite_ct  to  0 .

  {
   Phase_Entry_t  p;
   int  i, n;

   n = v . size ();
   for  (i = 0;  i < n;  i ++)
     if  (v [i] . to == to)
         {
          assert (v [i] . from = from);
          v [i] . same_ct ++;
          return;
         }

   p . from = from;
   p . to = to;
   p . same_ct = 1;
   p . opposite_ct = 0;
   p . weight = 0;

   v . push_back (p);

   return;
  }



bool  Is_Distinguishing
    (unsigned char ct [5], char & ch1, int & ch1_ct,
     char & ch2, int & ch2_ct)

//  Check if values in  ct  indicate a polymorphism.  If so set
//   ch1  and  ch2  to the most frequenct characters and
//   ch1_ct  and  ch2_ct  to the number of occurrences of each.

  {
   int  sub1, sub2;
   int  i;

   ch1_ct = ct [0];
   sub1 = 0;
   ch2_ct = 0;

   for  (i = 1;  i < 5;  i ++)
     if  (ch2_ct < ct [i])
         {
          if  (ch1_ct < ct [i])
              {
               ch2_ct = ch1_ct;
               sub2 = sub1;
               ch1_ct = ct [i];
               sub1 = i;
              }
            else
              {
               ch2_ct = ct [i];
               sub2 = i;
              }
         }

   // Simple version that makes a column distinguishing if it has
   // >= 3 occurrences of 3 or more characters
   // Should probably make alternative version that uses quality values
   if  (ch1_ct >= 3 && ch2_ct >= 3)
       {
        ch1 = Sub_To_DNA_Char (sub1);
        ch2 = Sub_To_DNA_Char (sub2);
        return  true;
       }

   return  false;
  }



int  Match_Count
    (const vector <int> & a, const vector <int> & b)

//  Return the number of entries in  a  that are also in  b
//  Assume each list is in ascending sorted order without
//  duplicates.

  {
   int  i, j, m, n, ct;

   m = a . size ();
   n = b . size ();

   i = j = ct = 0;
   while  (i < m && j < n)
     {
      if  (a [i] < b [j])
          i ++;
      else if  (b [j] < a [i])
          j ++;
        else
          {
           ct ++;
           i ++;
           j ++;
          }
     }

   return  ct;
  }



void  Multi_Align
    (const string & id, vector <char *> & s, vector <int> & offset,
     int offset_delta, double error_rate, int min_overlap,
     Gapped_Multi_Alignment_t & gma, vector <int> * ref,
     vector <char *> * tag_list, bool allow_expels)

//  id  is the id of the contig being multi-aligned.
// Create multialignment in  gma  of strings  s  each of which has
// a nominal offset from its predecessor of  offset .   offset_delta  is
// the number of positions by which the offset is allowed to vary in
// either direction.   error_rate  is the maximum expected error rate
// in alignments between strings.  It should be twice the expected error
// rate to the real reference string to allow for independent errors
// in separate strings.  Strings must overlap by at least  min_overlap
// bases (unless forced).  The value of  offset [0]  must be zero.
// If  ref  isn't  NULL  then make its values be the subscripts of
// the original locations of the entries in  s  in case they are
// shifted.   If  tag_list  isn't  NULL , then use its values to identify
// the strings in  s  and shift them along with the entries in  s .
// If  allow_expels  is true, then reads can be left out of the multialignment
// if doing so does not separate the multialignment into disjoint segments

  {
   Multi_Alignment_t  ma;
   vector <Vote_t>  vote;
   vector <int>  s_len;
   bool  changed;
   int  i, n, ct;

   n = s . size ();
   if  (n == 0)
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align for contig %s called with no strings",
            id . c_str ());
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }
   if  (n != int (offset . size ()))
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align for contig %s called with %d strings and %d offsets",
            id . c_str (), n, int (offset . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }
   if  (offset [0] != 0)
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align for contig %s called with non-zero  offset [0] = %d",
            id . c_str (), offset [0]);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   for (i = 0; i < n; i ++)
     s_len . push_back (strlen (s [i]));

   // Sort the sequence strings in s and sequence length in s_len by increasing
   // sequence offset
   Sort_Strings_And_Offsets (s, s_len, offset, ref, tag_list);

   ma . setID (id);
   ma . Set_Initial_Consensus (s, offset, offset_delta, error_rate, min_overlap,
        vote, tag_list, allow_expels);

   ct = 0;
   do
     {
      ma . Reset_From_Votes (s, offset_delta, error_rate, vote, changed);
      ct ++;
     }  while  (ct < MAX_REFINEMENTS && changed);

   if  (Verbose > 3)
       ma . Print_Alignments_To_Consensus (stderr, s);

   gma . Convert_From (ma);

   gma . Sort (s, ref, tag_list);

   return;
  }



void  Overlap_Align
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int t_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align)

//  Find the best alignment of a prefix of string  s  to a
//  substring of  t  starting between positions  t_lo  and  t_hi ,
//  where the alignment must extend to the end of one of the strings.
//  The length of  s  is  s_len  and the length of  t  is  t_len .
//  The resulting alignment is stored in  align ,
//  where  s  is the  a  string and  t  is the  b  string.
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).

  {
   double  matrix_size;

   if  (Verbose > 3)
       fprintf (stderr, "Overlap_Align:  s_len = %d  t_lo/hi/len = %d/%d/%d\n",
            s_len, t_lo, t_hi, t_len);

   assert (t_lo <= t_hi);
   assert (t_hi <= t_len);
   assert (0 <= s_len );

   matrix_size = double (s_len) * (t_len - t_lo);

   if  (t_len - t_lo <= 10 || matrix_size <= MATRIX_SIZE_LIMIT)
       Overlap_Align_Full_Matrix (s, s_len, t, t_lo, t_hi, t_len,
            match_score, mismatch_score, indel_score, gap_score, align);
     else
       Overlap_Align_Save_Space (s, s_len, t, t_lo, t_hi, t_len,
            match_score, mismatch_score, indel_score, gap_score, align);

   return;
  }



void  Overlap_Align_Full_Matrix
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int t_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align)

//  Find the best alignment of a prefix of string  s  to a
//  substring of  t  starting between postions  t_lo  and  t_hi ,
//  where the alignment must extend to the end of one of the strings.
//  The length of  s  is  s_len  and the length of  t  is  t_len .
//  The resulting alignment is stored in  align ,
//  where  s  is the  a  string and  t  is the  b  string.
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).
//  Use a full matrix for the computation.

  {
   vector < vector <Align_Score_Entry_t> > a;  // the alignment array
   vector <Align_Score_Entry_t>  empty_vector;
   vector <int>  delta;
   Align_Score_Entry_t  entry;
   int  r, c;    // row and column
   unsigned int  max_from, mxf;
   int  max_row, max_col, max_score, mxs;
   int  error_ct, row_limit;
   int  i, n;  // position in string  t

   // Do first row

   if  (Verbose > 0)
       fprintf (stderr, "Overlap_Align_Full_Matrix:\n");

   a . push_back (empty_vector);
   entry . diag_score = entry . top_score = entry . left_score = 0;
   entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
   r = 0;
   a [r] . push_back (entry);
   for  (c = 1;  c <= s_len;  c ++)
     {
      entry . top_score = NEG_INFTY_SCORE;
      entry . top_from = FROM_NOWHERE;
      entry . diag_score = NEG_INFTY_SCORE;
      entry . diag_from = FROM_NOWHERE;
      entry . left_score = a [r] [c - 1] . left_score + indel_score;
      if  (c == 1)
          entry . left_score += gap_score;
      entry . left_from = FROM_LEFT;
      a [r] . push_back (entry);
     }
   entry . Get_Max (max_score, max_from);
   max_row = 0;
     
   // Do remaining rows
   for  (i = t_lo;  i < t_len;  i ++)
     {
      r ++;

      // First column in row
      a . push_back (empty_vector);
      entry . diag_score = entry . left_score = NEG_INFTY_SCORE;
      if  (i < t_hi - 1)
          {
           entry . top_score = 0;
           entry . top_from = FROM_NOWHERE;
          }
        else
          {
           entry . top_score = a [r - 1] [0] . top_score + indel_score;
           if  (i == t_hi - 1)
               entry . top_score += gap_score;
           entry . top_from = FROM_TOP;
          }
      entry . diag_from = entry . left_from = FROM_NOWHERE;
      a [r] . push_back (entry);

      // Remaining columns in row
      for  (c = 1;  c <= s_len;  c ++)
        {
         Align_Score_Entry_t  * p;

         p = & (a [r - 1] [c]);
         p -> Get_Max_Top (mxs, mxf, gap_score);
         entry . top_score = mxs + indel_score;
         entry . top_from = mxf;

         p = & (a [r - 1] [c - 1]);
         p -> Get_Max (mxs, mxf);
         entry . diag_score = mxs;
         entry . diag_from = mxf;
         entry . diag_score += ((t [i] == s [c - 1]) ? match_score : mismatch_score);

         p = & (a [r] [c - 1]);
         p -> Get_Max_Left (mxs, mxf, gap_score);
         entry . left_score = mxs + indel_score;
         entry . left_from = mxf;
         a [r] . push_back (entry);
        }

      // check last entry in row to find max
      entry . Get_Max (mxs, mxf);
      if  (mxs > max_score)
          {
           max_score = mxs;
           max_from = mxf;
           max_row = r;
           max_col = s_len;
          }
     }

   // See if get better max in the last row
   for  (c = 0;  c <= s_len;  c ++)
     {
      a [r] [c] . Get_Max (mxs, mxf);
      if  (mxs > max_score)
          {
           max_score = mxs;
           max_from = mxf;
           max_row = r;
           max_col = c;
          }
     }

   // Trace back
   r = max_row;
   c = max_col;
   row_limit = t_hi - t_lo - 1;
   Trace_Back_Align_Path (a, r, c, row_limit, 0, delta,
        error_ct, s, t + t_lo);

   align . a_lo = 0;
   align . a_hi = max_col;
   align . b_hi = max_row + t_lo;
   align . b_lo = r + t_lo;
   align . errors = error_ct;

   if  (Verbose > 2)
       {
        n = delta . size ();
        for  (i = 0;  i < n;  i ++)
          printf ("delta [%d] = %d\n", i, delta [i]);
       }

   align . setDelta (delta);

   return;
  }



void  Overlap_Align_Save_Space
    (const char * s, int s_len, const char * t, int t_lo, int t_hi,
     int t_len, int match_score, int mismatch_score, int indel_score,
     int gap_score, Alignment_t & align)

//  Find the best alignment of a prefix of string  s  to a
//  substring of  t  starting between postions  t_lo  and  t_hi ,
//  where the alignment must extend to the end of one of the strings.
//  The length of  s  is  s_len  and the length of  t  is  t_len .
//  The resulting alignment is stored in  align ,
//  where  s  is the  a  string and  t  is the  b  string.
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).
//  Do recursively keeping just a finite number of rows

  {
   Alignment_t  sub_ali_1, sub_ali_2;
   vector <Augmented_Score_Entry_t> align_row;
        //  row of alignment array
   Augmented_Score_Entry_t  entry, last_entry;
   int  t_half, t_slip;
   int  max_row, max_col, max_score, mxs, max_ref;
   unsigned int  max_from, mxf;
   int  r, c;
   
   if  (Verbose > 3)
       fprintf (stderr, "Overlap_Align_Save_Space:\n");

   // Build first row

   entry . diag_score = entry . top_score = entry . left_score = 0;
   entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
   entry . top_ref = entry . diag_ref = entry . left_ref = 0;
   align_row . push_back (entry);
   for  (c = 1;  c <= s_len;  c ++)
     {
      entry . top_score = NEG_INFTY_SCORE;
      entry . top_from = FROM_NOWHERE;
      entry . diag_score = NEG_INFTY_SCORE;
      entry . diag_from = FROM_NOWHERE;
      entry . left_score = align_row [c - 1] . left_score + indel_score;
      if  (c == 1)
          entry . left_score += gap_score;
      entry . left_from = FROM_LEFT;
      entry . left_ref = 0;
      entry . top_ref = entry . diag_ref = -1;
      align_row . push_back (entry);
     }
   entry . Get_Max (max_score, max_from);
   max_row = t_lo;
   max_col = s_len;
   max_ref = 0;

   // Do remaining rows

   t_half = (t_lo + t_len) / 2;
   for  (r = t_lo + 1;  r <= t_len;  r ++)
     {
      int  first_score;

      if  (r < t_hi)
          first_score = 0;
      else if  (r == t_hi)
          first_score = indel_score + gap_score;
        else
          first_score = indel_score;

      Align_Row_Update (align_row, t [r - 1], s, s_len, match_score,
           mismatch_score, indel_score, gap_score, first_score);

      if  (r == t_half)
          {  // set the ref entries to reference this row
           align_row [0] . top_ref = 0;
           for  (c = 1;  c <= s_len;  c ++)
             {
              align_row [c] . top_ref = align_row [c] . diag_ref = c;
              align_row [c - 1] . Get_Max_Left (mxs, mxf, gap_score);
              align_row [c] . left_ref = align_row [c - 1] . Get_Ref (mxf);
             }
          }

      // Check last column entry for max
      align_row [s_len] . Get_Max (mxs, mxf);
      if  (mxs > max_score)
          {
           max_score = mxs;
           max_from = mxf;
           max_row = r;
           max_col = s_len;
           max_ref = align_row [s_len] . Get_Ref (mxf);
          }
     }

   // Check entries in last row for max
   for  (c = 0;  c <= s_len;  c ++)
     {
      align_row [c] . Get_Max (mxs, mxf);
      if  (mxs > max_score)
          {
           max_score = mxs;
           max_from = mxf;
           max_row = t_len;
           max_col = c;
           max_ref = align_row [c] . Get_Ref (mxf);
          }
     }

   t_slip = Min (t_hi, max_row);
   if  (Verbose > 3)
       fprintf (stderr,
            "max_row/col = %d/%d  t_lo = %d  t_slip = %d  t_half = %d  t_hi = %d\n",
            max_row, max_col, t_lo, t_slip, t_half, t_hi);
   if  (max_row <= t_half)
       {
        // only need one recursive call on the first half of the alignment
        Complete_Align (s, 0, s_len, t, t_lo, t_slip, max_row,
             match_score, mismatch_score, indel_score, gap_score, NULL,
             entry, align);

        return;
       }

   if  (max_col == 0 && t_half < t_hi)
       {
        // only need one recursive call on the second half of the alignment
        // because the best start for the alignment is after t_half
        Complete_Align (s, 0, s_len, t, t_half, t_hi, max_row,
             match_score, mismatch_score, indel_score, gap_score, NULL,
             entry, align);

        return;
       }

   t_slip = Min (t_hi, t_half);
   if  (Verbose > 3)
       fprintf (stderr,
            "max_ref = %d  t_lo = %d  t_slip = %d  t_half = %d  t_hi = %d\n",
            max_ref, t_lo, t_slip, t_half, t_hi);
   Complete_Align (s, 0, max_ref, t, t_lo, t_slip, t_half,
        match_score, mismatch_score, indel_score, gap_score,
        NULL, entry, sub_ali_1);

   if  (max_ref == 0)
       t_slip = Max (t_hi, t_half + 1);
     else
       t_slip = t_half + 1;
   Complete_Align (s, max_ref, max_col, t, t_half, t_slip,
        max_row, match_score, mismatch_score, indel_score, gap_score,
        & entry, last_entry, sub_ali_2);

   align . Combine (sub_ali_1, sub_ali_2);

   return;
  }



bool  Overlap_Match_VS
    (const char * s, int s_len, const char * t, int t_len,
     int lo_off, int hi_off, int min_len, int max_errors,
     Alignment_t & align)

//  Return whether there is an overlap between string  s  and string
//   t  starting at a position in the range  lo_off .. hi_off  wrt
//  string  t .  These values must be non-negative.  The overlap region
//  can have at most  max_errors  errors and must be at least  min_len
//  long in both  s  and  t  (length can differ because of indels).
//  The length of  s  is  s_len  and the length of  t  is  t_len .
//  If successful, the alignment is stored in  align .
//  Uses  Vishkin_Schieber (e * n)  algorithm.  Errors are inserts,
//  deletes or substitutions.  Returns the best match found, i.e.,
//  the match with the lowest errors/overlap_len ratio, where
//  overlap_len is the average of the lengths of the two strings'
//  overlap regions.

  {
   static Match_Extent_Entry_t  * space = NULL;
   static int  space_size = 0;
   static Match_Extent_Entry_t  * * tab = NULL;
   static int  tab_size = 0;
   int  space_needed;
   bool  found;
   int  complete_match, possible_len;
   int  e, i, best_i;

   if  (Verbose > 3)
       {
        fprintf (stderr, "In Overlap_Match_VS  lo_off = %d  hi_off = %d\n",
                 lo_off, hi_off);
       }

   lo_off = Max (0, lo_off);
   hi_off = Min (hi_off, t_len - min_len);
   
   if  (hi_off < lo_off || s_len < min_len)
       return  false;     // No match possible

   if  (max_errors < 0)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  max_errors = %d < 0", max_errors);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   // allocate more memory if necessary
   space_needed = (1 + max_errors) * (max_errors + 1 + hi_off - lo_off);
   if  (space_needed > space_size)
       {
        space_size = space_needed;
        space = (Match_Extent_Entry_t *) Safe_realloc
                    (space, space_size * sizeof (Match_Extent_Entry_t),
                     __FILE__, __LINE__);
       }
   if  (1 + max_errors > tab_size)
       {
        tab_size = 1 + max_errors;
        tab = (Match_Extent_Entry_t * *) Safe_realloc
                    (tab, tab_size * sizeof (Match_Extent_Entry_t *),
                     __FILE__, __LINE__);
       }

   // tab  points to the logical start of each row of
   // (truncated) pyramidal array with entries for  (lo_off .. hi_off)
   // in the zeroth row and extending one to the left and one to the
   // right every row after that

   tab [0] = space - lo_off;

   // find how far can match at each spot with zero errors
   // Could do better maybe using KMP here
   for  (i = lo_off;  i <= hi_off;  i ++)
     {
      possible_len = Min (s_len, t_len - i);
      tab [0] [i] . len = Exact_Prefix_Match (s, t + i, possible_len);
      tab [0] [i] . from = MATCH_FROM_TOP;
      if  (tab [0] [i] . len == possible_len)
          {   // match found
           align . Set_Exact (0, i, possible_len);
           return  true;
          }
     }

   // now progressively try more errors
   found = false;
   best_i = lo_off - max_errors - 2;

   for  (e = 1;  e <= max_errors && ! found;  e ++)
     {
      int  prev_match;

      tab [e] = tab [e - 1] + hi_off - lo_off + 2 * e;

      i = lo_off - e;
      complete_match = Min (s_len, t_len - i);
      prev_match = tab [e - 1] [i + 1] . len;
      possible_len = Min (s_len - prev_match - 1,
                                     t_len - i - prev_match - 1);
      tab [e] [i] . len = prev_match + 1
          + Exact_Prefix_Match (s + prev_match + 1, t + i + prev_match + 1,
                                possible_len);
      tab [e] [i] . from = MATCH_FROM_NE;
      if  (tab [e] [i] . len == complete_match)
          {
           found = true;
           best_i = i;
          }
      if  (Verbose > 3)
          fprintf (stderr, "e = %d  i = %d  NE  len = %d  poss_len = %d\n",
                   e, i, tab [e] [i] . len, possible_len);

      for  (i = lo_off - e + 1;  i <= hi_off + e - 1;  i ++)
        {
         int  next;

         complete_match = Min (s_len, t_len - i);

         prev_match = tab [e - 1] [i] . len;
         possible_len = Min (s_len - prev_match - 1,
                             t_len - i - prev_match - 1);
         tab [e] [i] . len = prev_match + 1
             + Exact_Prefix_Match (s + prev_match + 1, t + i + prev_match + 1,
                                   possible_len);
         tab [e] [i] . from = MATCH_FROM_TOP;
         if  (Verbose > 3)
             fprintf (stderr, "e = %d  i = %d  TOP  len = %d  poss_len = %d\n",
                      e, i, tab [e] [i] . len, possible_len);
         
         if  (i > lo_off - e + 1)
             {
              prev_match = tab [e - 1] [i - 1] . len;
              possible_len = Min (s_len - prev_match,
                                    t_len - i - prev_match);
              next = prev_match
                        + Exact_Prefix_Match
                              (s + prev_match, t + i + prev_match,
                               possible_len);
              if  (Verbose > 3)
                  fprintf (stderr, "e = %d  i = %d  NW  len = %d  poss_len = %d\n",
                           e, i, next, possible_len);
              if  (next > tab [e] [i] . len)
                  {
                   tab [e] [i] . len = next;
                   tab [e] [i] . from = MATCH_FROM_NW;
                  }
             }

         if  (i < hi_off + e - 1)
             {
              prev_match = tab [e - 1] [i + 1] . len;
              possible_len = Min (s_len - prev_match - 1,
                                    t_len - i - prev_match - 1);
              next = prev_match + 1
                        + Exact_Prefix_Match
                              (s + prev_match + 1, t + i + prev_match + 1,
                               possible_len);
              if  (Verbose > 3)
                  fprintf (stderr, "e = %d  i = %d  NE  len = %d  poss_len = %d\n",
                           e, i, next, possible_len);
              if  (next > tab [e] [i] . len)
                  {
                   tab [e] [i] . len = next;
                   tab [e] [i] . from = MATCH_FROM_NE;
                  }
             }

         // Prefer MATCH_FROM_TOP if there are consecutive matches
         if  (tab [e] [i] . len == complete_match)
             {
              if  (tab [e] [i] . from == MATCH_FROM_TOP)
                  {
                   best_i = i;
                   found = true;
                   break;
                  }
              else if  (! found)
                  {
                   found = true;
                   best_i = i;
                  }
             }
         else if  (found)
            break;
        }

      if  (! found)
          {
           i = hi_off + e;
           complete_match = Min (s_len, t_len - i);
           prev_match = tab [e - 1] [i - 1] . len;
           possible_len = Min (s_len - prev_match,
                                          t_len - i - prev_match);
           tab [e] [i] . len = prev_match
               + Exact_Prefix_Match (s + prev_match, t + i + prev_match,
                                     possible_len);
           tab [e] [i] . from = MATCH_FROM_NW;
           if  (tab [e] [i] . len == complete_match)
               {
                best_i = i;
                found = true;
               }
          }
     }

   if  (Verbose > 1)
       Print_VS_Table (stderr, tab, lo_off, hi_off + 1, e, best_i, found);

   if  (found)
       {
        align . Set_From_VS_Matrix (tab, e - 1, best_i);
        return  true;
       }

   return  false;
  }



void  Print_Align_Lines_Pair
    (FILE * fp, const string & s, const string & t, int len,
     const char * s_label, const char * t_label,
     int s_lo, int s_hi, int t_lo, int t_hi)

//  Print to  fp  alignment lines  s  and  t , each of length  len .
//  Print  s_label  at the beginning of the  s  line  and  t_label
//  at the beginning of the  t  line, and print the ranges
//   s_lo .. s_hi  and  t_lo .. t_hi  at the right of the lines.
//  Also print a line of carets underneath the  s  and  t  lines
//  with a caret beneath every position where the  s  and  t
//  entries differ.

  {
   int  s_label_len, t_label_len;
   int  i;

   // In case  s_label  and  t_label  aren't the same length
   // we'll use the longer of the two and right align them

   s_label_len = strlen (s_label);
   t_label_len = strlen (t_label);
   if  (s_label_len < t_label_len)
       s_label_len = t_label_len;

   fprintf (fp, "%*s  ", s_label_len, s_label);
   for  (i = 0;  i < len;  i ++)
     fputc (s [i], fp);
   fprintf (fp, "  (%d-%d)\n", s_lo, s_hi);

   fprintf (fp, "%*s  ", s_label_len, t_label);
   for  (i = 0;  i < len;  i ++)
     fputc (t [i], fp);
   fprintf (fp, "  (%d-%d)\n", t_lo, t_hi);

   fprintf (fp, "%*s  ", s_label_len, "");
   for  (i = 0;  i < len;  i ++)
     fputc (s [i] == t [i] ? ' ' : '^', fp);
   fputc ('\n', fp);

   return;
  }



void  Print_VS_Table
    (FILE * fp, Match_Extent_Entry_t * * tab, int t_lo, int t_hi, int e,
     int best_i, bool found)

//  Print to  fp  the entries in  tab  for columns t_lo - e .. t_hi + e
//  and rows  0 .. e - 1 .  End the last row at column  best_i .
//  Also print the value of  found .

  {
   int  i, j;

   fprintf (stderr, "e = %d  best_i = %d  found = %c\n",
            e, best_i, found ? 'T' : 'F');
   fprintf (stderr, "%3s ", "e");
   for  (j = t_lo - e + 1;  j < t_hi + e - 1;  j ++)
     fprintf (stderr, " %3d ", j);
   fputc ('\n', stderr);

   if  (! found)
       best_i = t_hi + e - 1;

   for  (i = 0;  i < e;  i ++)
     {
      fprintf (stderr, "%2d: ", i);

      for  (j = t_lo - e + 1;  (i < e - 1 && j < t_hi + e - 1)
                            || (i == e - 1 && j <= best_i);  j ++)
        if  (j < t_lo - i || j >= t_hi + i)
            fprintf (stderr, " %3s ", "-");
          else
            {
             char  ch;
             switch  (tab [i] [j] . from)
               {
                case  -1 :
                  ch = 'r';
                  break;
                case  0 :
                  ch = 't';
                  break;
                case  1 :
                  ch = 'l';
                  break;
                default :
                  sprintf (Clean_Exit_Msg_Line, "ERROR:  from = %d\n",
                           tab [i] [j] . from);
                  Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
               }
             fprintf (stderr, " %3d%c", tab [i] [j] . len, ch);
            }
      fputc ('\n', stderr);
     }
  }



bool  Range_Intersect
    (int a_lo, int a_hi, int b_lo, int b_hi)

//  Return true iff the substring range  a_lo .. a_hi  overlaps the
//  substring range  b_lo .. b_hi .

  {
   if  (a_lo == a_hi)
       return  false;    // a is empty

   if  (a_hi <= b_lo || a_lo >= b_hi)
       return  false;

   return  true;
  }



void  Simple_Overlap
    (const char * s, int s_len, const char * t, int t_len,
     Simple_Overlap_t & olap, int match_score,
     int mismatch_score,
     int indel_score,
     int gap_score)

//  Find the highest-scoring overlap alignment between string  s  and
//  string  t , i.e., an alignment the extends to the end of either
//  of the two strings on each end.
//  The length of  s  is  s_len  and the length of  t  is  t_len .
//  The resulting overlap is stored in  olap .
//   match_score  is the score for matching characters (positive);
//   mismatch_score  the score for aligning different characters (negative);
//   indel_score  the score for insertions/deletions (negative);
//  and  gap_score  the extra penalty for starting a gap (negative).

  {
   vector <Errors_Score_Entry_t> align_row;
        //  row of alignment array
   Errors_Score_Entry_t  entry;
   int  max_row, max_col, max_score, mxs, max_ref, max_errors;
   unsigned int  max_from, mxf;
   int  r, c;

   // Create the first row of the alignment matrix
   entry . diag_score = entry . top_score = entry . left_score = 0;
   entry . diag_from = entry . top_from = entry . left_from = FROM_NOWHERE;
   entry . diag_errors = entry . top_errors = entry . left_errors = 0;

   for  (c = 0;  c <= s_len;  c ++)
     {
      entry . top_ref = entry . diag_ref = entry . left_ref = c;
      align_row . push_back (entry);
     }

   entry . Get_Max (max_score, max_from);
   max_row = 0;
   max_col = s_len;
   max_ref = s_len;

   // Do remaining rows
   for  (r = 1;  r <= t_len;  r ++)
     {
      align_row [0] . top_ref = align_row [0] . diag_ref
           = align_row [0] . left_ref = 1 - r;
              // is value of current row, not one being built
      Align_Row_Update_With_Errors (align_row, t [r - 1], s, s_len, match_score,
           mismatch_score, indel_score, gap_score, 0, 0);

      // Check last column entry for max
      align_row [s_len] . Get_Max (mxs, mxf);
      if  (mxs > max_score)
          {
           max_score = mxs;
           max_from = mxf;
           max_row = r;
           max_col = s_len;
           max_ref = align_row [s_len] . Get_Ref (mxf);
           max_errors = align_row [s_len] . Get_Errors (mxf);
          }
     }

   // Check entries in last row for max
   for  (c = 0;  c <= s_len;  c ++)
     {
      align_row [c] . Get_Max (mxs, mxf);
      if  (mxs > max_score)
          {
           max_score = mxs;
           max_from = mxf;
           max_row = t_len;
           max_col = c;
           max_ref = align_row [c] . Get_Ref (mxf);
           max_errors = align_row [c] . Get_Errors (mxf);
          }
     }

   if  (Verbose > 2)
       printf ("row = %d  col = %d  ref = %d  score = %d  s_len = %d  t_len = %d\n",
           max_row, max_col, max_ref, max_score, s_len, t_len);

   olap . score = max_score;
   olap . errors = max_errors;
   olap . a_hang = max_ref;
   if  (max_col < s_len)
       olap . b_hang = max_col - s_len;
     else
       olap . b_hang = t_len - max_row;
   if  (max_ref >= 0)
       {
        olap . b_olap_len = max_row;
        olap . a_olap_len = max_col - max_ref;
       }
     else
       {
        olap . b_olap_len = max_row + max_ref;
        olap . a_olap_len = max_col;
       }

   return;
  }



void  Sort_Strings_And_Offsets
    (vector <char *> & s, vector <int> & s_len, vector <int> & offset,
     vector <int> * ref, vector <char *> * tag_list)

//  Sort the strings in  s  (together with their lengths in  s_len )
//  into order so that all their offsets
//  are non-negative.  Adjust the values in  offset  accordingly.
//  Use lengths to resolve tied offsets preferring to put
//  longer strings first.
//  Use insertion sort since most offsets should be positive.
//  If  ref  isn't  NULL, then set it to the subscripts of the
//  positions of the entries in  s  and  offset  before they were
//  changed.  If  tag_list  isn't  NULL , then also sort its entries.
//  Note:  These offsets are relative to the preceding string, so
//  only non-positive values need to be moved.

  {
   int  i, j, n;

   n = offset . size ();
   if (n != int (s . size ()))
     {
      sprintf (Clean_Exit_Msg_Line,
           "ERROR:  Sorting %d strings with %d offsets\n",
           int (s . size ()), n);
      Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
     }
   if (ref != NULL)
     {
      ref -> resize (n);
      for  (i = 0;  i < n;  i ++)
        (* ref) [i] = i;
     }

   for  (i = 1;  i < n;  i ++)
     {
      char  * s_save, * t_save;
      int  o_save, r_save, l_save;

      if (0 < offset [i])
        continue;

      s_save = s [i];
      l_save = s_len [i];
      o_save = offset [i];
      if (tag_list != NULL)
        t_save = (* tag_list) [i];
      if (ref != NULL)
        r_save = (* ref) [i];
      if (i < n - 1)
        offset [i + 1] += o_save;

      for (j = i;  j > 0 && (o_save < 0 || (o_save == 0 && s_len [j - 1] < l_save));
           j --)
        {
         o_save += offset [j - 1];
         offset [j] = offset [j - 1];
         s [j] = s [j - 1];
         s_len [j] = s_len [j - 1];
         if  (tag_list != NULL)
             (* tag_list) [j] = (* tag_list) [j - 1];
         if  (ref != NULL)
             (* ref) [j] = (* ref) [j - 1];
        }

      s [j] = s_save;
      s_len [j] = l_save;
      offset [j] = o_save;
      offset [j + 1] -= o_save;
      if  (tag_list != NULL)
          (* tag_list) [j] = t_save;
      if  (ref != NULL)
          (* ref) [j] = r_save;
     }

   return;
  }



bool  Substring_Match_VS
    (const char * s, int s_len, const char * t, int t_len,
     int t_lo, int t_hi, int max_errors, Alignment_t & align)

//  Return whether string  s  is contained as a substring within
//  string  t  with at most  max_errors  errors.  The length of
//   s  is  s_len  and the length of  t  is  t_len .  The match
//  must start within  t  between positions  t_lo  and  t_hi  (gap
//  coordinates).  If successful, the alignment is stored in  align .
//  Uses  Vishkin_Schieber (e * n)  algorithm.  Errors are inserts,
//  deletes or substitutions.  Returns first match found, i.e., the
//  one beginning closest to  t_lo  (in case there is more than one).

  {
   static Match_Extent_Entry_t  * space = NULL;
   static int  space_size = 0;
   static Match_Extent_Entry_t  * * tab = NULL;
   static int  tab_size = 0;
   int  space_needed;
   bool  found;
   int  e, i, best_i;

   if  (Verbose > 3)
       {
        fprintf (stderr, "In Substring_Match_VS  t_lo = %d  t_hi = %d\n",
                 t_lo, t_hi);
       }

   if  (t_hi <= t_lo)
       return  false;     // No place for match to start

   if  (max_errors < 0)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  max_errors = %d < 0", max_errors);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   // allocate more memory if necessary
   space_needed = (1 + max_errors) * (max_errors + t_hi - t_lo);
   if  (space_needed > space_size)
       {
        space_size = space_needed;
        space = (Match_Extent_Entry_t *) Safe_realloc
                    (space, space_size * sizeof (Match_Extent_Entry_t),
                     __FILE__, __LINE__);
       }
   if  (1 + max_errors > tab_size)
       {
        tab_size = 1 + max_errors;
        tab = (Match_Extent_Entry_t * *) Safe_realloc
                    (tab, tab_size * sizeof (Match_Extent_Entry_t *),
                     __FILE__, __LINE__);
       }

   // tab  points to the logical start of each row of
   // (truncated) pyramidal array with entries for  (t_lo .. t_hi - 1)
   // in the zeroth row and extending one to the left and one to the
   // right every row after that

   tab [0] = space - t_lo;

   // find how far can match at each spot with zero errors
   // Could do better maybe using KMP here
   for  (i = t_lo;  i < t_hi;  i ++)
     {
      tab [0] [i] . len = Exact_Prefix_Match (s, t + i, Min (s_len, t_len - i));
      tab [0] [i] . from = MATCH_FROM_TOP;
      if  (tab [0] [i] . len == s_len)
          {   // match found
           align . Set_Exact (0, i, s_len);
           return  true;
          }
     }

   // now progressively try more errors
   found = false;
   best_i = t_lo - max_errors - 2;

   for  (e = 1;  e <= max_errors && ! found;  e ++)
     {
      int  prev_match;

      tab [e] = tab [e - 1] + t_hi - t_lo + 2 * e - 1;

      i = t_lo - e;
      prev_match = tab [e - 1] [i + 1] . len;
      tab [e] [i] . len = prev_match + 1
          + Exact_Prefix_Match (s + prev_match + 1, t + i + prev_match + 1,
                                Min (s_len - prev_match - 1,
                                     t_len - i - prev_match - 1));
      tab [e] [i] . from = MATCH_FROM_NE;
      if  (tab [e] [i] . len == s_len)
          {
           found = true;
           best_i = i;
          }
      if  (Verbose > 3)
          fprintf (stderr, "e = %d  i = %d  NE  len = %d\n",
                   e, i, tab [e] [i] . len);

      for  (i = t_lo - e + 1;  i < t_hi + e - 1;  i ++)
        {
         int  next;

         prev_match = tab [e - 1] [i] . len;
         tab [e] [i] . len = prev_match + 1
             + Exact_Prefix_Match (s + prev_match + 1, t + i + prev_match + 1,
                                   Min (s_len - prev_match - 1,
                                        t_len - i - prev_match - 1));
         tab [e] [i] . from = MATCH_FROM_TOP;
         if  (Verbose > 3)
             fprintf (stderr, "e = %d  i = %d  TOP  len = %d\n",
                      e, i, tab [e] [i] . len);
         
         if  (i > t_lo - e + 1)
             {
              prev_match = tab [e - 1] [i - 1] . len;
              next = prev_match
                        + Exact_Prefix_Match
                              (s + prev_match, t + i + prev_match,
                               Min (s_len - prev_match,
                                    t_len - i - prev_match));
              if  (Verbose > 3)
                  fprintf (stderr, "e = %d  i = %d  NW  len = %d\n",
                           e, i, next);
              if  (next > tab [e] [i] . len)
                  {
                   tab [e] [i] . len = next;
                   tab [e] [i] . from = MATCH_FROM_NW;
                  }
             }

         if  (i < t_hi + e - 2)
             {
              prev_match = tab [e - 1] [i + 1] . len;
              next = prev_match + 1
                        + Exact_Prefix_Match
                              (s + prev_match + 1, t + i + prev_match + 1,
                               Min (s_len - prev_match - 1,
                                    t_len - i - prev_match - 1));
              if  (Verbose > 3)
                  fprintf (stderr, "e = %d  i = %d  NE  len = %d\n",
                           e, i, next);
              if  (next > tab [e] [i] . len)
                  {
                   tab [e] [i] . len = next;
                   tab [e] [i] . from = MATCH_FROM_NE;
                  }
             }

         // Prefer MATCH_FROM_TOP if there are consecutive matches
         if  (tab [e] [i] . len == s_len)
             {
              if  (tab [e] [i] . from == MATCH_FROM_TOP)
                  {
                   best_i = i;
                   found = true;
                   break;
                  }
              else if  (! found)
                  {
                   found = true;
                   best_i = i;
                  }
             }
         else if  (found)
            break;
        }

      if  (! found)
          {
           i = t_hi + e - 1;
           prev_match = tab [e - 1] [i - 1] . len;
           tab [e] [i] . len = prev_match
               + Exact_Prefix_Match (s + prev_match, t + i + prev_match,
                                     Min (s_len - prev_match,
                                          t_len - i - prev_match));
           tab [e] [i] . from = MATCH_FROM_NW;
           if  (tab [e] [i] . len == s_len)
               {
                best_i = i;
                found = true;
               }
          }
     }

   if  (Verbose > 1)
       Print_VS_Table (stderr, tab, t_lo, t_hi, e, best_i, found);

   if  (found)
       {
        align . Set_From_VS_Matrix (tab, e - 1, best_i);
        return  true;
       }

   return  false;
  }



char  Sub_To_DNA_Char
    (int i)

//  Return the DNA character equivalent of subscript  i .

  {
   static char  convert [] = "acgt-";

   if  (i >= 5)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  subscript %d >= 5",
             i);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   return  convert [i];
  }



void  Trace_Back_Align_Path
    (const vector < vector <Align_Score_Entry_t> > & a, int & r, int & c,
     int r_start, int c_start, vector <int> & delta, int & errors,
     const char * s, const char * t)

//  Trace back the alignment in  a  from row/column  r / c  back
//  to an entry with  r <= r_start  and  c <= c_start .
//  Set  delta  to the delta-encoding for the alignment and set
//   errors  to the number of errors in the alignment.  Strings
//  s  and  t  are the strings referred to by the matrix,  s
//  for columns and  t  for rows.

  {
   int  ct, sign, mxs;
   unsigned int  mxf;

   a [r] [c] . Get_Max (mxs, mxf);
   ct = sign = errors = 0;
   delta . clear ();
   
   while  (r > r_start || c > c_start)
     {
      switch  (mxf)
        {
         case  FROM_LEFT :
           if  (sign != 0)
               {
                if  (Verbose > 2)
                    printf ("del = %d\n", sign * ct);
                delta . push_back (sign * ct);
               }
           ct = 1;
           sign = -1;
           mxf = a [r] [c] . left_from;
           c --;
           errors ++;
           break;
         case  FROM_DIAG :
           ct ++;
           mxf = a [r] [c] . diag_from;
           r --;
           c --;
           if  (s [c] != t [r])
               errors ++;
           break;
         case  FROM_TOP :
           if  (sign != 0)
               {
                if  (Verbose > 2)
                    printf ("del = %d\n", sign * ct);
                delta . push_back (sign * ct);
               }
           ct = 1;
           sign = 1;
           mxf = a [r] [c] . top_from;
           r --;
           errors ++;
           break;
         default :
           sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad from = %u in Global_Align",
                mxf);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
        }
     }

   if  (sign != 0)
       {
        if  (Verbose > 2)
            printf ("del = %d\n", sign * ct);
        delta . push_back (sign * ct);
       }

   reverse (delta . begin (), delta . end ());

   return;
  }



void  Traverse
    (int v, int par, vector < vector <Phase_Entry_t> > & tree,
     vector <Distinguishing_Column_t> & dc, char ch, int & sum)

//  Set  dc [v] . phase_char  to  ch  and continue a depth-first
//  traversal of the tree in  tree  using the phase of the tree
//  edges to determine the next phase character.  Increment
//   sum  by  ch * (dc [v] . (hapa_ct - hapb_ct)) .
//   par  is the subscript of the parent of  v  in the tree.
//  Note:  there must be *NO* cycles in tree except between
//  parent and child.

  {
   int  i, n;

   dc [v] . phase_ch = ch;
   if  (ch == '+')
       sum += int (dc [v] . hapa_sub . size ())
                - int (dc [v] . hapb_sub . size ());
     else
       sum += int (dc [v] . hapb_sub . size ())
                - int (dc [v] . hapa_sub . size ());

   n = tree [v] . size ();
   for  (i = 0;  i < n;  i ++)
     if  (tree [v] [i] . to != par)
         {
          if  (tree [v] [i] . same_ct >= tree [v] [i] . opposite_ct)
              Traverse (tree [v] [i] . to, v, tree, dc, ch, sum);
            else
              {
               char  flip_ch;

               if  (ch == '+')
                   flip_ch = '-';
                 else
                   flip_ch = '+';
               Traverse (tree [v] [i] . to, v, tree, dc, flip_ch, sum);
              }
         }

   return;
  }



int  UF_Find
    (int i, vector <int> & uf)

//  Return the subscript of the set leader of  i  in  Union-Find
//  array  uf .

  {
   int  j;

   for  (j = i;  uf [j] >= 0;  j = uf [j])
     ;

   if  (j == i)
       return  j;

   while  (uf [i] != j)
     {
      int  k = uf [i];

      uf [i] = j;
      i = k;
     }

   return  j;
  }



int  UF_Find_With_Parity
    (int i, vector <int> & uf, int & parity)

//  Return the subscript of the set leader of  i  in  Union-Find
//  array  uf  and set  parity to  0  if it's an even number of
//  edges from the leader in the UF tree or  1  if it's  odd.
//  Maintain the parity of any other nodes when compressing the path.

  {
   int  j, par, save;

   parity = 0;
   for  (j = i;  uf [j] >= 0;  j = uf [j])
     {
      save = j;
      parity = 1 - parity;
     }

   //  save  should now be the child of  j  (which is the root of
   // the tree), on the path from  i  to the root.

   if  (j == i)
       return  j;

   par = parity;
   while  (uf [i] != j)
     {
      int  k = uf [i];

      if  (par == 0)
          uf [i] = save;
        else
          uf [i] = j;
      i = k;
      par = 1 - par;
     }

   return  j;
  }



void  UF_Union
    (int i, int j, vector <int> & uf)

//  Union the sets represented by  i  and  j  in Union-Find
//  array  uf .  

  {
   assert (uf [i] < 0 && uf [j] < 0);

   if  (uf [i] <= uf [j])
       {  // i is more negative, hence bigger
        uf [i] += uf [j];
        uf [j] = i;
       }
     else
       {  // j is more negative, hence bigger
        uf [j] += uf [i];
        uf [i] = j;
       }

   return;
  }



int  Ungapped_Positions
    (const string & s, int lo, int hi)

//  Return the number of non-'-' positions in string  s
//  in the range  lo .. hi , which are in gapped coordinates.

  {
   int  i, n, ct;

   n = s . length ();
   if  (hi > n)
       {
        sprintf (Clean_Exit_Msg_Line,
             "ERROR:  hi = %d > s . length () = %d in Ungapped_Positions\n",
             hi, n);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   ct = 0;
   for  (i = lo;  i < hi;  i ++)
     if  (s [i] != '-')
         ct ++;

   return  ct;
  }



void  Update_Banded_Row
    (vector <Errors_Score_Entry_t> & band, char ch, const char * s,
     int s_len, int left_col, int right_col, int match_score,
     int mismatch_score, int indel_score, int gap_score, int first_score,
     int first_error)

//  Update  band  to be the values in the next row of an alignment
//  matrix where the preceding row is in  band  now.
//  The new band is shifted one to the right of the current band.
//   ch  is the character for the next row and  s  (whose length is  s_len )
//  is the characters across the top of the alignment matrix.
//  The current band represents columns  left_col .. right_col  in
//  the alignment matrix.
//   match_score ,  mismatch_score ,  indel_score  and  gap_score
//  are the scores for matching characters, mismatching characters,
//  insertion/deletion and initiating a gap, respectively.
//   first_score  is the value to add to the first entry's  top_score  field.
//   first_error  is the value to add to the first entry's  top_errors  field.

  {
   Errors_Score_Entry_t  save, * p, * q;
   int  mxs;
   unsigned int  mxf;
   int  c, i, first, last;

   assert (int (band . size ()) == 1 + right_col - left_col);

   // Fill in the first cell
   if  (left_col < 0)
       {
        first = -1 - left_col;
        c = 0;
        p = & (band [first]);
        q = & (band [first + 1]);

        p -> top_score = q -> top_score + first_score;
        p -> diag_score = p -> left_score = NEG_INFTY_SCORE;
        p -> top_errors = q -> top_errors + first_error;
        p -> diag_errors = p -> left_errors = 0;
       }
     else
       {
        first = 0;
        c = left_col + 1;
        save = band [first];
        p = & (band [first]);
        q = & (band [first + 1]);

        q -> Get_Max_Top (mxs, mxf, gap_score);
        p -> top_score = mxs + indel_score;
        p -> top_from = mxf;
        p -> top_ref = q -> Get_Ref (mxf);
        p -> top_errors = q -> Get_Errors (mxf) + 1;

        save . Get_Max (mxs, mxf);
        p -> diag_score = mxs
             + ((ch == s [left_col]) ? match_score : mismatch_score);
        p -> diag_from = mxf;
        p -> diag_ref = save . Get_Ref (mxf);
        p -> diag_errors = save . Get_Errors (mxf);
        if  (ch != s [left_col])
            p -> diag_errors ++;

        p -> left_score = NEG_INFTY_SCORE;
        p -> left_errors = 0;
        p -> left_ref = 0;
       }

   // Do the remaining cells
   if  (right_col >= s_len)
       last = s_len - left_col - 1;
     else
       last = right_col - left_col;
   for  (i = first + 1;  i <= last;  i ++, c++)
     {
      save = band [i];
      p = & (band [i]);

      if  (i == last)
          {
           p -> top_score = NEG_INFTY_SCORE;
           p -> top_ref = p -> top_errors = 0;
          }
        else
          {
           q = & (band [i + 1]);
           q -> Get_Max_Top (mxs, mxf, gap_score);
           p -> top_score = mxs + indel_score;
           p -> top_from = mxf;
           p -> top_ref = q -> Get_Ref (mxf);
           p -> top_errors = q -> Get_Errors (mxf) + 1;
          }

      save . Get_Max (mxs, mxf);
      p -> diag_score = mxs
           + ((ch == s [c]) ? match_score : mismatch_score);
      p -> diag_from = mxf;
      p -> diag_ref = save . Get_Ref (mxf);
      p -> diag_errors = save . Get_Errors (mxf);
      if  (ch != s [c])
          p -> diag_errors ++;

      band [i - 1] .  Get_Max_Left (mxs, mxf, gap_score);
      p -> left_score = mxs + indel_score;
      p -> left_from = mxf;
      p -> left_ref = band [i - 1] . Get_Ref (mxf);
      p -> left_errors = band [i - 1] . Get_Errors (mxf) + 1;
     }

   return;
  }
