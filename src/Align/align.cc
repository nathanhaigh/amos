//
//  File:  align.cc
//
//  Last Modified:  25 November 2002
//
//  Routines to do string alignments


#include  "align.hh"
#include  <stack>



void  Vote_t :: Incr_After
    (char ch)

//  Add 1 to the  after  count in this  Vote_t  for  ch .

  {
   char  * p;

   p = strchr (ALPHABET, tolower (ch));
   if  (p == NULL)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad character %c (ASCII %d)\n",
                 ch, ch);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   after [p - ALPHABET] ++;

   return;
  }



void  Vote_t :: Incr_Blank
    (void)

//  Add 1 to the blank entry in the  here  counts of this  Vote_t .

  {
   here [ALPHABET_SIZE] ++;

   return;
  }



void  Vote_t :: Incr_By
    (char ch, bool with_blank)

//  Add 1 to the  here  count in this  Vote_t  for  ch .
//  If  with_blank  is true then also add 1 to the after
//  count for a blank.

  {
   char  * p;

   p = strchr (ALPHABET, tolower (ch));
   if  (p == NULL)
       {
        sprintf (Clean_Exit_Msg_Line, "ERROR:  Bad character %c (ASCII %d)\n",
                 ch, ch);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   here [p - ALPHABET] ++;
   if  (with_blank)
       after [ALPHABET_SIZE] ++;

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
   char  * p;

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



void  Alignment_t :: Incr_Votes
    (vector <Vote_t> & vote, char * a)

//  Use the entries in the alignemnt to increment the entries in  vote
//  that correspond to the  b  string in the alignment.
//  Vote values are from the string  a .

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
         vote [j] . Incr_By (a [i], (k < extent - 1));
         i ++;
         j ++;
        }

      if  (delta [d] < 0)
          {
           if  (extent > 0)
               vote [j - 1] . Incr_After (a [i]);
           i ++;
          }
        else
          {
           vote [j] . Incr_Blank ();
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
      vote [j] . Incr_By (a [i], (j < b_hi - 1));
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

   delta . clear ();

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
   if  (d > 0 && (j == 0 || tr [j] - d <= tr [j - 1]))
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
    (FILE * fp)

//  Print to  fp  the entries of this alignment.

  {
   fprintf (fp, "### a_lo = %d  a_hi = %d  b_lo = %d  b_hi = %d  error = %d\n",
            a_lo, a_hi, b_lo, b_hi, errors);
   fprintf (fp, "Skip:\n");
   Dump_Skip (fp);

   return;
  }



void  Gapped_Alignment_t :: Dump_Skip
    (FILE * fp)

//  Print to  fp  the  skip  entries of this alignment.

  {
   int  i, n;

   n = skip . size ();
   for  (i = 0;  i < n;  i ++)
     fprintf (fp, "  %3d: %5d\n", i, skip [i]);

   return;
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



void  Multi_Alignment_t :: Clear
    (void)

//  Make this multi-alignment empty.

  {
   consensus . assign ("");
   align . clear ();

   return;
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
   int  adj, cons_len;
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
   for  (i = 0;  i < n;  i ++)
     {
      bool  matched;
      int  error_limit, len, off;

      len = strlen (s [i]);
      error_limit = Binomial_Cutoff (len, error_rate, 1e-6);

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

      off = align [i] . b_lo + adjust [align [i] . b_lo];
      matched
          = Substring_Match_VS
                (s [i], len, cons, cons_len,
                 off - offset_delta, off + offset_delta,
                 error_limit, align [i]);
      
      if  (! matched)
          {
           fprintf (stderr, "Failed on string %d in  Reset_From_Votes\n", i);
           exit (EXIT_FAILURE);
          }

      align [i] . Incr_Votes (vote, s [i]);
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
     int offset_delta, double error_rate,
     vector <Vote_t> & vote)

//  Create an initial consensus string in this multialignment from the
//  strings in  s  with nominal relative offsets in  offset .  Offsets
//  are allowed to vary by +/-  offset_delta  and the allowed error rate
//  in alignments is  error_rate .  Set  vote  to the votes of the strings
//  at each position of the consensus.  Create the consensus by greedily
//  tiling the strings in order, appending the extension of any
//  string that aligns past the end of the consensus.  Store the
//  initial alignments in this consensus, too.

  {
   Vote_t  v;
   Alignment_t  ali;
   char  * cons;
   int  cons_len;
   int  num_strings;
   int  prev_off;
   int  i, j;

   Clear ();
   vote . clear ();

   cons = strdup (s [0]);
   cons_len = strlen (cons);
   for  (j = 0;  j < cons_len - 1;  j ++)
     {
      v . Set_To (s [0] [j], true);
      vote . push_back (v);
     }
   v . Set_To (s [0] [cons_len - 1], false);
   vote . push_back (v);

   ali . Set_To_Identity (cons_len);
   align . push_back (ali);

   num_strings = s . size ();
   prev_off = 0;
       // where the last string started in the consensus


   for  (i = 1;  i < num_strings;  i ++)
     {
      bool  matched;
      int  error_limit, len, exp_olap_len;

      len = strlen (s [i]);
      exp_olap_len = Min (cons_len - prev_off - offset [i], len);

      error_limit = Binomial_Cutoff (exp_olap_len, error_rate, 1e-6);

      matched = Overlap_Match_VS
              (s [i], len, cons, cons_len,
               prev_off + offset [i] - offset_delta,
               prev_off + offset [i] + offset_delta, 0, error_limit, ali);

      if  (! matched)
          {
           fprintf (stderr, "Failed on string %d in  Set_Initial_Consensus\n", i);
           exit (EXIT_FAILURE);
          }

      ali . Incr_Votes (vote, s [i]);

      if  (ali . a_hi < len)
          {  // s [i] extends past the end of the current consensus
           int  extra;

           extra = len - ali . a_hi;
           cons = (char *) Safe_realloc (cons, cons_len + extra + 1,
                                         __FILE__, __LINE__);
           strcpy (cons + cons_len, s [i] + ali . a_hi);
           cons_len += extra;

           for  (j = ali . a_hi;  j < len - 1;  j ++)
             {
              v . Set_To (s [i] [j], true);
              vote . push_back (v);
             }
           v . Set_To (s [i] [len - 1], false);
           vote . push_back (v);

           ali . a_hi = len;
           ali . b_hi = cons_len;
          }

      align . push_back (ali);

      prev_off = ali . b_lo;
     }

   if  (Verbose > 1)
       {
        fprintf (stderr, "*** Votes ***\n");
        fprintf (stderr, "%5s: %2s  ------ Here -------  ------ After ------\n",
                 "Pos", "Ch");
        for  (j = 0;  j < cons_len;  j ++)
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



void  Gapped_Multi_Alignment_t :: Get_Positions
    (vector <Range_t> & pos)  const

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



void  Gapped_Multi_Alignment_t :: Print
    (FILE * fp, const vector <char *> & s, int width)

//  Display this multialignment to file  fp  using
//   width  characters per line.   s  holds the strings
//  the alignment references.

  {
   char  * buff;
   int  lo, hi, len;
   int  i, n;

   if  (s . size () != align . size ())
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align . Print called with %d strings and %d alignments",
            int (s . size ()), int (align . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   n = s . size ();
   len = consensus . length ();
   buff = (char *) Safe_malloc (5 + len, __FILE__, __LINE__);

   // assume for now the alignments are sorted in ascending order by
   // b_lo value

   for  (lo = 0;  lo < len;  lo += width)
     {
      int  a_lo, a_hi;

      hi = Min (lo + width, len);

      for  (i = 0;  i < n;  i ++)
        if  (Range_Intersect (align [i] . b_lo, align [i] . b_hi, lo, hi))
            {
             align [i] . Print_Subalignment_Line
                           (buff, lo, hi, s [i], a_lo, a_hi);
             fprintf (fp, "%4d:  %s  (%d-%d)\n", i, buff, a_lo, a_hi);
            }

      Print_Consensus (buff, lo, hi);
      fprintf (fp, "%4s:  %s  (%d-%d)\n", "cons", buff, lo, hi);
      if  (hi < len)
          fprintf (fp, "\n");
     }
   
   free (buff);

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



void  Gapped_Multi_Alignment_t :: Set_Consensus_And_Qual
    (const vector <char *> & s, const vector <char *> & q)

//  Reset the consensus base values using the libSlice package
//  and assign the corresponding quality for each one in the
//  con_qual string.  Strings in alignment are in  s  and their
//  quality values are in  q .

  {
   vector <Gapped_MA_Bead_t>  active_bead;
   string  seq_column, qual_column;
   Consensus  * cns;
   static slice  sl;
   int  col_len;
   static int  max_len = 0;
   int  len, next;
   int  i, j, n;


   len = consensus . length ();
   con_qual . resize (len);
   n = align . size ();

   if  (Verbose > 3)
       {
        for  (i = 0;  i < n;  i ++)
          {
           printf ("> Sequence %d\n", i);
           Fasta_Print (stdout, s [i], NULL);
           Fasta_Print (stdout, q [i], NULL);
          }
       }

   if  (max_len == 0)
       {
        max_len = 200;
        sl . rc = (char *) Safe_malloc (max_len, __FILE__, __LINE__);
        memset (sl . rc, 0, max_len);
       }

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
              qual_column . push_back (bp -> qual_ch - QUALITY_OFFSET);
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
         qual_column . push_back (b . qual_ch - QUALITY_OFFSET);

         next ++;
        }

      sl . bc = (char *) seq_column . c_str ();
      sl . qv = (char *) qual_column . c_str ();
      col_len = seq_column . length ();
      if  (col_len >= max_len)
          {
           max_len = Max (2 * max_len, col_len + 1);
           sl . rc = (char *) Safe_realloc (sl . rc, max_len);
           memset (sl . rc, 0, max_len);
          }
      sl . dcov = col_len;
      getConsensus (& sl, & cns, NULL, 0);

      consensus [i] = cns -> consensus;
      con_qual [i] = QUALITY_OFFSET
          + Min (cns -> qvConsensus, unsigned (MAX_QUALITY_CHAR));

      if  (Verbose > 3)
          {
           printf ("%6d:  ", i);
           cout << seq_column << "  col_len = " << col_len << endl;
           for  (j = 0;  j < col_len;  j ++)
             qual_column [j] += QUALITY_OFFSET;
           cout << "         " << qual_column << endl;
           cout << "         cons = " << cns -> consensus << "  qv = "
                << Min (cns -> qvConsensus, unsigned (MAX_QUALITY_CHAR)) << endl;
          }
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
    (vector <char *> & s, vector <int> * ref)

//  Sort the align entries in this multialignment according to their
//   b_lo  values.  Also sort the strings  s  along with them.
//  If  ref  isn't  NULL , then move its entries in parallel
//  with those of  s  and  align .

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
      char  * s_save;
      Gapped_Alignment_t  a_save;
      int  r_save;

      if  (align [i - 1] . b_lo <= align [i] . b_lo)
          continue;

      s_save = s [i];
      a_save = align [i];
      if  (ref != NULL)
          r_save = (* ref) [i];

      for  (j = i;  j > 0 && align [j - 1] . b_lo > a_save . b_lo;  j --)
        {
         align [j] = align [j - 1];
         s [j] = s [j - 1];
         if  (ref != NULL)
             (* ref) [j] = (* ref) [j - 1];
        }

      s [j] = s_save;
      align [j] = a_save;
      if  (ref != NULL)
          (* ref) [j] = r_save;
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
        fprintf (stderr, "s :\n");
        Fasta_Print (stderr, s, NULL);
        fprintf (stderr, "t :\n");
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



void  Multi_Align
    (vector <char *> & s, vector <int> & offset, int offset_delta,
     double error_rate, Gapped_Multi_Alignment_t & gma,
     vector <int> * ref)

//  Create multialignment in  ma  of strings  s  each of which has
//  a nominal offset from its predecessor of  offset .   offset_delta  is
//  the number of positions by which the offset is allowed to vary in
//  either direction.   error_rate  is the maximum expected error rate
//  in alignments between strings.  It should be twice the expected error
//  rate to the real reference string to allow for independent errors
//  in separate strings.  The value of  offset [0]  must be zero.
//  If  ref  isn't  NULL  then make its values be the subscripts of
//  the original locations of the entries in  s  in case they are
//  shifted

  {
   Multi_Alignment_t  ma;
   vector <Vote_t>  vote;
   bool  changed;
   int  ct, n;

   n = s . size ();
   if  (n == 0)
       Clean_Exit ("ERROR:  Multi_Align called with no strings", __FILE__, __LINE__);
   if  (n != int (offset . size ()))
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align called with %d strings and %d offsets",
            n, int (offset . size ()));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }
   if  (offset [0] != 0)
       {
        sprintf (Clean_Exit_Msg_Line,
            "ERROR:  Multi_Align called with non-zero  offset [0] = %d",
            offset [0]);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }

   Sort_Strings_And_Offsets (s, offset, ref);

   ma . Set_Initial_Consensus (s, offset, offset_delta, error_rate, vote);

   ct = 0;
   do
     {
      ma . Reset_From_Votes (s, 5, error_rate, vote, changed);
      ct ++;
     }  while  (ct < 3 && changed);

   if  (Verbose > 3)
       ma . Print_Alignments_To_Consensus (stderr, s);

   gma . Convert_From (ma);

   gma . Sort (s, ref);

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



void  Sort_Strings_And_Offsets
    (vector <char *> & s, vector <int> & offset, vector <int> * ref)

//  Sort the strings in  s  into order so that all their offsets
//  are non-negative.  Adjust the values in  offset  accordingly.
//  Use insertion sort since most offsets should be positive.
//  If  ref  isn't  NULL, then set it to the subscripts of the
//  positions of the entries in  s  and  offset  before they were
//  changed.

  {
   int  i, j, n;

   n = offset . size ();
   if  (n != int (s . size ()))
       {
        sprintf (Clean_Exit_Msg_Line,
             "ERROR:  Sorting %d strings with %d offsets\n",
             int (s . size ()), n);
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
       }
   if  (ref != NULL)
       {
        ref -> resize (n);
        for  (i = 0;  i < n;  i ++)
          (* ref) [i] = i;
       }

   for  (i = 1;  i < n;  i ++)
     {
      char  * s_save;
      int  o_save, r_save;

      if  (0 <= offset [i])
          continue;

      s_save = s [i];
      o_save = offset [i];
      if  (ref != NULL)
          r_save = (* ref) [i];
      if  (i < n - 1)
          offset [i + 1] += o_save;

      for  (j = i;  j > 0 && o_save < 0;  j --)
        {
         o_save += offset [j - 1];
         offset [j] = offset [j - 1];
         s [j] = s [j - 1];
         if  (ref != NULL)
             (* ref) [j] = (* ref) [j - 1];
        }

      s [j] = s_save;
      offset [j] = o_save;
      offset [j + 1] -= o_save;
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



