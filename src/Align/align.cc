//
//  File:  align.cc
//
//  Last Modified:  25 November 2002
//
//  Routines to do string alignments


#include  "delcher.h"
#include  "align.h"
#include  "fasta.h"
#include  <stack>



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

      if  (Verbose > 1)
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



int  Exact_Prefix_Match
    (const char * s, const char * t, int max_len)

//  Return the length of the longest exact prefix match of
//  strings  s  and  t , not to exceed  max_len .

  {
   int  i;

   if  (Verbose > 3)
       {
        fprintf (stderr, "In Exact_Prefix_Match  max_len = %d\n", max_len);
        printf ("s :\n");
        Fasta_Print (stderr, s, NULL);
        printf ("t :\n");
        Fasta_Print (stderr, t, NULL);
       }

   for  (i = 0;  i < max_len && s [i] == t [i];  i ++)
     ;

   if  (Verbose > 3)
       {
        fprintf (stderr, "Exact_Prefix_Match  return value = %d\n", i);
       }

   return  i;
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