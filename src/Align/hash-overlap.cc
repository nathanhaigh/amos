//  A. L. Delcher
//
//  File:  hash-overlap.cc
//
//  Last Modified:  Tue May 25 09:42:14 EDT 2004
//
//  Compute overlaps among an input set of sequences by
//  creating a hash table of minimizers to use as seed
//  matches and then do a full alignment for pairs with
//  common seeds.


#include  "hash-overlap.hh"


#define  USE_SIMPLE_OVERLAP  0


static bool  AMOS_Output = false;
  // Determines whether output is AMOS messages or simple
  // text lines
static string  Bank_Name;
  // Name of read bank from which reads are obtained
static double  Error_Rate = DEFAULT_ERROR_RATE;
  // Fraction of errors allowed in overlap alignment
static int  Lo_IID = 1, Hi_IID = INT_MAX;
  // Range of read (internal) IDs in bank for which to compute
  // overlaps
static int  Minimizer_Window_Len = 20;
  // Length of window from which a minimizer is extracted
static int  Min_Overlap_Len = DEFAULT_MIN_OVERLAP_LEN;
  // Minimum number of bases by which two sequences must overlap
static bool  Use_SeqNames = false;
  // If set true, will use comment field in the readbank to
  // extract sequence tags



int  main
    (int argc, char * argv [])

  {
   Bank_t  read_bank (Read_t :: NCode ());
   Read_t  read;
   Simple_Overlap_t  olap;
   vector <char *>  string_list, qual_list;
   vector <char *>  tag_list;
   vector <Range_t>  clr_list;
   hash_map <unsigned int, Hash_Entry_t> hash_table;
   time_t  now;
   iostream :: fmtflags  status;
   int  i, j, n;

   try
     {
      now = time (NULL);
      cerr << "Starting on " << ctime (& now) << endl;

      Verbose = 0;

      Parse_Command_Line (argc, argv);

      cerr << "Read bank is " << Bank_Name << endl;
      status = cerr . setf (ios :: fixed);
      cerr << "Alignment error rate is " << setprecision (2)
           << Error_Rate << endl;
      cerr . setf (status);
      cerr << "Minimum overlap bases is " << Min_Overlap_Len << endl;

      read_bank . open (Bank_Name);

      Hi_IID = Min (Hi_IID, int (read_bank . getLastIID ()));
      Check_IIDs ();

      Get_Strings_From_Bank (Lo_IID, Hi_IID, string_list, qual_list, clr_list,
           tag_list, read_bank);

      Map_Minimizers (string_list, hash_table);

      Find_Fwd_Overlaps (string_list, hash_table, Lo_IID);

      Find_Rev_Overlaps (string_list, hash_table, Lo_IID);

      read_bank . close ();
     }
   catch (Exception_t & e)
     {
      cerr << "** AMOS Exception **" << endl;
      cerr << e << endl;
      exit (EXIT_FAILURE);
     }
   catch (std :: exception & e)
     {
      cerr << "** Standard Exception **" << endl;
      cerr << e << endl;
      exit (EXIT_FAILURE);
     }

   return  0;
  }



static unsigned int  Bit_Pattern
    (char ch)

//  Return the bit pattern equivalent of character  ch .

  {
   switch  (tolower (ch))
     {
      case  'a' :
        return  0x0;
      case  'c' :
        return  0x1;
      case  'g' :
        return  0x2;
      case  't' :
        return  0x3;
      default :       // Anything else is converted to an A
        return  0x0;
     }   
  }



static int  By_String_Num_Then_Pos
    (const void * a, const void * b)

//  Return the order relationship between  a  and  b  regarded
//  as  Reference_t 's based first on the  string_num  field,
//  and second on the  pos  field.  Used by  qsort  function.

  {
   Reference_t  * x, * y;

   x = (Reference_t *) a;
   y = (Reference_t *) b;

   if  (x -> string_num < y -> string_num)
       return  -1;
   else if  (x -> string_num > y -> string_num)
       return  1;
   else if  (x -> pos < y -> pos)
       return  -1;
   else if  (x -> pos > y -> pos)
       return  1;
     else  return  0;
  }



static void  Check_IIDs
    (void)

// Check that global IID range bounds  Lo_IID  and  Hi_IID
// are valid.  Throw an exception if they're not.

  {
   if  (Lo_IID < 1)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "Low IID = %d is less than 1", Lo_IID);
        AMOS_THROW (Clean_Exit_Msg_Line);
       }
   if  (Hi_IID < Lo_IID)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "Low IID = %d and High IID = %d are out of order", Lo_IID, Hi_IID);
        AMOS_THROW (Clean_Exit_Msg_Line);
       }
   if  (Lo_IID == Hi_IID)
       {
        sprintf (Clean_Exit_Msg_Line,
                 "Low IID = %d = High IID  No overlaps are possible", Lo_IID);
        AMOS_THROW (Clean_Exit_Msg_Line);
       }

   return;
  }



static void  Find_Fwd_Overlaps
    (const vector <char *> & string_list,
     hash_map <unsigned int, Hash_Entry_t> & ht, int lo_iid)

//  Find all overlaps between pairs of strings in  string_list
//  where both are in the forward orientation and share
//  a minimizer in  ht .  Add  lo_iid  to the subscript
//  in  string_list  when outputting the string id number.

  {
   vector <Offset_Entry_t>  offset_list;
   hash_map <unsigned int, Hash_Entry_t> :: iterator  iter;
   Simple_Overlap_t  olap;
   double  erate;
   int  i, j, n;

   n = string_list . size ();
   offset_list . resize (n);

   for  (i = 0;  i < n;  i ++)
     offset_list [i] . ct = 0;

   // Store the offsets
   for  (iter = ht . begin ();  iter != ht . end ();  iter ++)
     {
      if  (iter -> second . ct < 2)
          continue;

      for  (i = 0;  i < iter -> second . ct - 1;  i ++)
        {
         int  a = iter -> second . ref [i] . string_num;
         int  a_pos = iter -> second . ref [i] . pos;

         for  (j = i + 1;  j < iter -> second . ct;  j ++)
           {
            int  b = iter -> second . ref [j] . string_num;
            int  b_pos = iter -> second . ref [j] . pos;

            if  (a == b)
                continue;

            offset_list [a] . Add_Offset (b, b_pos - a_pos);
           }
        }
     }

   for  (i = 0;  i < n;  i ++)
     for  (j = 0;  j < offset_list [i] . ct;  j ++)
       {
        int  b = offset_list [i] . off [j] . string_num;
        int  lo, hi;

#if  USE_SIMPLE_OVERLAP
        Simple_Overlap (string_list [i], strlen (string_list [i]),
             string_list [b], strlen (string_list [b]), olap);
#else
        lo = Max (offset_list [i] . off [j] . lo_offset - 5,
                    - int (strlen (string_list [i])));
        hi = Min (offset_list [i] . off [j] . hi_offset + 5,
                    int (strlen (string_list [b])));
        Banded_Overlap (string_list [i], strlen (string_list [i]),
             string_list [b], strlen (string_list [b]), lo, hi, olap);
#endif
        if  (olap . a_olap_len < Min_Overlap_Len
                || olap . b_olap_len < Min_Overlap_Len)
            continue;
        erate = (2.0 * olap . errors)
            / (olap . a_olap_len + olap . b_olap_len);
        if  (erate <= Error_Rate)
            {
             olap . a_id = i + lo_iid;
             olap . b_id = b + lo_iid;
             olap . flipped = false;
             Output (cout, olap);
            }
       }

   for  (i = 0;  i < n;  i ++)
     if  (offset_list [i] . ct > 0)
         free (offset_list [i] . off);

   return;
  }



static void  Find_Rev_Overlaps
    (vector <char *> & string_list,
     hash_map <unsigned int, Hash_Entry_t> & ht, int lo_iid)

//  Find all overlaps between pairs of strings in  string_list
//  where the lower-numbered string is in the reverse orientation
//  the higher-numbered string is in the forward orientation.
//  Overlaps are found only if the sequences share
//  a minimizer in  ht .  Add  lo_iid  to the subscript
//  in  string_list  when outputting the string id number.

  {
   Minimizer_t  mini (Minimizer_Window_Len);
   Offset_Entry_t  offset;
   hash_map <unsigned int, Hash_Entry_t> :: iterator  iter;
   Simple_Overlap_t  olap;
   double  erate;
   int  i, j, n, p;

   n = string_list . size ();
   offset . ct = 0;

   for  (i = 0;  i < n;  i ++)
     {
      char  * s;
      unsigned int  sig;
      int  pos, new_pos;
      int  k, m, p;

      Reverse_Complement (string_list [i]);
      s = string_list [i];
      m = strlen (s);
      if  (m < Minimizer_Window_Len)
          continue;

      mini . Init (s);
      sig = mini . Get_Signature ();
      pos = mini . Get_Window_Offset ();
      if  (ht . find (sig) != ht . end ())
          for  (j = 0;  j < ht [sig] . ct;  j ++)
            offset . Add_Offset (ht [sig] . ref [j] . string_num,
                 ht [sig] . ref [j] . pos - pos);

      k = Minimizer_Window_Len;
      for  (p = 1;  p <= m - Minimizer_Window_Len;  p ++, k++)
        {
         mini . Advance (s [k]);
         new_pos = p + mini . Get_Window_Offset ();
         if  (new_pos > pos)
             {
              sig = mini . Get_Signature ();
              if  (ht . find (sig) != ht . end ())
                  for  (j = 0;  j < ht [sig] . ct;  j ++)
                    offset . Add_Offset (ht [sig] . ref [j] . string_num,
                         ht [sig] . ref [j] . pos - pos);
              pos = new_pos;
             }
        }

      for  (j = 0;  j < offset . ct;  j ++)
        {
         int  b = offset . off [j] . string_num;
         int  lo, hi;

#if  USE_SIMPLE_OVERLAP
         Simple_Overlap (string_list [i], strlen (string_list [i]),
              string_list [b], strlen (string_list [b]), olap);
#else
         lo = Max (offset . off [j] . lo_offset - 5,
                     - int (strlen (string_list [i])));
         hi = Min (offset . off [j] . hi_offset + 5,
                     int (strlen (string_list [b])));
         Banded_Overlap (string_list [i], strlen (string_list [i]),
              string_list [b], strlen (string_list [b]), lo, hi, olap);
#endif
         if  (olap . a_olap_len < Min_Overlap_Len
                 || olap . b_olap_len < Min_Overlap_Len)
             continue;
         erate = (2.0 * olap . errors)
             / (olap . a_olap_len + olap . b_olap_len);
         if  (erate <= Error_Rate)
             {
              int  save;

              // Re-orient with a forward and b reversed
              save = olap . a_hang;
              olap . a_hang = - olap . b_hang;
              olap . b_hang = - save;

              olap . a_id = i + lo_iid;
              olap . b_id = b + lo_iid;
              olap . flipped = true;
              Output (cout, olap);
             }
        }

      if  (offset . ct > 0)
          {
           free (offset . off);
           offset . ct = 0;
          }
     }

   return;
  }



static void  Get_Strings_From_Bank
    (int lo_iid, int hi_iid, vector <char *> & s, vector <char *> & q,
     vector <Range_t> & clr_list, vector <char * > & tag_list,
     Bank_t & read_bank)

//  Populate  s  and  q  with sequences and quality values, resp.,
//  from  read_bank .  Put the clear-ranges for the sequences in  clr_list .
//   read_bank  must already be opened.  Put the identifying tags for the
//  sequences in  tag_list .  Only get sequences with iids in the range
//   lo_iid .. hi_iid .

  {
   Read_t  read;
   Ordered_Range_t  position;
   int  i, n;

   n = s . size ();
   for  (i = 0;  i < n;  i ++)
     free (s [i]);
   s . clear ();

   n = q . size ();
   for  (i = 0;  i < n;  i ++)
     free (q [i]);
   q . clear ();

   n = tag_list . size ();
   for  (i = 0;  i < n;  i ++)
     free (tag_list [i]);
   tag_list . clear ();

   clr_list . clear ();

   for  (i = lo_iid;  i <= hi_iid;  i ++)
     {
      char  * tmp, tag_buff [100];
      string  seq;
      string  qual;
      Range_t  clear;
      int  this_offset;
      int  a, b, j, len, qlen;

      read . setIID (i);
      read_bank . fetch (read);
      if  (read . isRemoved ())
          {
           s . push_back ("");
           q . push_back ("");
           clear . setRange (0, 0);
           clr_list . push_back (clear);
           continue;
          }
      if  (Use_SeqNames)
          {
           j = read . getComment () . find ('\n');
           if  (unsigned (j) == string :: npos)
               j = read . getComment () . size ();
           read . getComment () . copy (tag_buff, j);
           tag_buff [j] = '\0';
          }
        else
          sprintf (tag_buff, "%u", read . getIID ());
      tag_list . push_back (strdup (tag_buff));
      
      clear = read . getClearRange ();
      if  (Verbose > 2)
	cerr << read;
      seq = read . getSeqString (clear);
      qual = read . getQualString (clear);
      clr_list . push_back (clear);

      len = seq . length ();
      tmp = strdup (seq . c_str ());
      for  (j = 0;  j < len;  j ++)
        tmp [j] = tolower (tmp [j]);
      s . push_back (tmp);

      qlen = qual . length ();
      if  (len != qlen)
          {
           sprintf
               (Clean_Exit_Msg_Line,
                "ERROR:  Sequence length (%d) != quality length (%d) for read %d\n",
                    len, qlen, read . getIID( ));
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      tmp = strdup (qual . c_str ());
      q . push_back (tmp);
     }

   return;
  }



static void  Map_Minimizers
    (const vector <char *> & string_list,
     hash_map <unsigned int, Hash_Entry_t> & ht)

//  Find minimizers for all strings in  string_list
//  and store them in  ht .

  {
   Minimizer_t  mini (Minimizer_Window_Len);
   hash_map <unsigned int, Hash_Entry_t> :: iterator  iter;
   int  c, i, n;

   n = string_list . size ();
   for  (i = 0;  i < n;  i ++)
     {
      char  * s;
      unsigned int  sig;
      int  pos, new_pos;
      int  j, k, m;

      s = string_list [i];
      m = strlen (s);
      if  (m < Minimizer_Window_Len)
          continue;

      mini . Init (s);
      sig = mini . Get_Signature ();
      pos = mini . Get_Window_Offset ();
      if  (ht . find (sig) != ht . end ())
          ht [sig] . ct ++;
        else
          ht [sig] . ct = 1;

      k = Minimizer_Window_Len;
      for  (j = 1;  j <= m - Minimizer_Window_Len;  j ++, k++)
        {
         mini . Advance (s [k]);
         new_pos = j + mini . Get_Window_Offset ();
         if  (new_pos > pos)
             {
              sig = mini . Get_Signature ();
              if  (ht . find (sig) != ht . end ())
                  ht [sig] . ct ++;
                else
                  ht [sig] . ct = 1;
              pos = new_pos;
             }
        }
     }

   for  (iter = ht . begin ();  iter != ht . end ();  iter ++)
     {
      iter -> second . ref = new Reference_t [iter -> second . ct];
      iter -> second . ct = 0;
     }

   for  (i = 0;  i < n;  i ++)
     {
      char  * s;
      unsigned int  sig;
      int  pos, new_pos;
      int  j, k, m;

      s = string_list [i];
      m = strlen (s);
      if  (m < Minimizer_Window_Len)
          continue;

      mini . Init (s);
      sig = mini . Get_Signature ();
      pos = mini . Get_Window_Offset ();
      c = ht [sig] . ct;
      ht [sig] . ref [c] . string_num = i;
      ht [sig] . ref [c] . pos = pos;
      ht [sig] . ct ++;

      k = Minimizer_Window_Len;
      for  (j = 1;  j <= m - Minimizer_Window_Len;  j ++, k++)
        {
         mini . Advance (s [k]);
         new_pos = j + mini . Get_Window_Offset ();
         if  (new_pos > pos)
             {
              pos = new_pos;
              sig = mini . Get_Signature ();
              c = ht [sig] . ct;
              ht [sig] . ref [c] . string_num = i;
              ht [sig] . ref [c] . pos = pos;
              ht [sig] . ct ++;
             }
        }
     }

   // Sort the ref entries by string number
   for  (iter = ht . begin ();  iter != ht . end ();  iter ++)
     if  (iter -> second . ct > 1)
         qsort (iter -> second . ref, iter -> second . ct,
              sizeof (Reference_t), By_String_Num_Then_Pos);

   if  (Verbose > 2)
       for  (iter = ht . begin ();  iter != ht . end ();  iter ++)
         {
          printf ("%08x ", iter -> first);
          for  (i = 0;  i < iter -> second . ct;  i ++)
            printf (" %5d/%-4d", iter -> second . ref [i] . string_num,
                 iter -> second . ref [i] . pos);
          putchar ('\n');
         }

   return;
  }



static void  Output
    (ostream & os, const Simple_Overlap_t & olap)

//  Print the contents of  olap  to  fp .

  {
   if  (AMOS_Output)
       {
        Message_t  msg;
        AMOS :: Overlap_t  ovl;
        std :: pair <ID_t, ID_t>  read_pair;

        if  (olap . flipped)
            ovl . setAdjacency (ovl . INNIE);
          else
            ovl . setAdjacency (ovl . NORMAL);
        read_pair . first = olap . a_id;
        read_pair . second = olap . b_id;
        ovl . setReads (read_pair);
        ovl . setAhang (olap . a_hang);
        ovl . setBhang (olap . b_hang);
        ovl . writeMessage (msg);
        msg . write (cout);
       }
     else
       {
        char  line [MAX_LINE];
        
        sprintf (line, "%5d %5d  %c %5d %5d  %5d %5d  %5d  %3d  %4.2f\n",
             olap . a_id, olap . b_id, olap . flipped ? 'I' : 'N',
             olap . a_hang, olap . b_hang,
             olap . a_olap_len, olap . b_olap_len, olap . score,
             olap . errors,
             200.0 * olap . errors
                  / (olap . a_olap_len + olap . b_olap_len));
        os << line;
       }

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

   while (!errflg && ((ch = getopt (argc, argv, "Ab:e:ho:sv:")) != EOF))
     switch  (ch)
       {
        case  'A' :
          AMOS_Output = true;
          break;

        case  'b' :
          Lo_IID = strtol (optarg, NULL, 10);
          break;

        case  'e' :
          Hi_IID = strtol (optarg, NULL, 10);
          break;

        case  'h' :
          errflg = true;
          break;

        case  'o' :
          Min_Overlap_Len = strtol (optarg, NULL, 10);
          break;

        case 's' :
	  Use_SeqNames = true;
	  break;

        case  'v' :
          Verbose = strtol (optarg, NULL, 10);
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

   if  (optind > argc - 1)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Bank_Name = argv [optind ++];

   return;
  }



static void  Shift_In
    (unsigned int & u, char ch, unsigned int mask)

//  Shift into  u  (on the right side) the bit equivalent of  ch
//  and then AND the result with  mask .

  {
   u <<= 2;
   u |= Bit_Pattern (ch);
   u &= mask;

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <bank-name>\n"
           "\n"
           "Compute pairwise overlaps among a set of sequences by\n"
           "brute-force all-pairs alignment.  Sequences are obtained\n"
           "from <bank-name>\n"
           "\n"
           "Options:\n"
           "  -A       Output AMOS-format messages\n"
           "  -b <n>   Use <n> as lowest read iid\n"
           "  -e <n>   Use <n> as highest read iid\n"
           "  -h       Print this usage message\n"
           "  -o <n>   Set minimum overlap length to <n>\n"
           "  -s       Get id tag from readbank comment string\n"
           "  -v <n>   Set verbose level to <n>.  Higher produces more output.\n"
           "\n",
           command);

   return;
  }



void  Offset_Entry_t :: Add_Offset
    (int s, int offset)

//  Add match to string number  s  with offset  offset  to this
//  entry if its not already present.

  {
   int  i;

   if  (ct == 0)
       {
        ct = 1;
        off = (Offset_Range_t *) Safe_calloc (1, sizeof (Offset_Range_t),
                  __FILE__, __LINE__);
        off [0] . string_num = s;
        off [0] . lo_offset = off [0] . hi_offset = offset;
        return;
       }

   for  (i = 0;  i < ct;  i ++)
     if  (off [i] . string_num == s)
         {
          if  (offset < off [i] . lo_offset)
              off [i] . lo_offset = offset;
          if  (offset > off [i] . hi_offset)
              off [i] . hi_offset = offset;
          return;
         }

   ct ++;
   off = (Offset_Range_t *) Safe_realloc (off, ct * sizeof (Offset_Range_t),
             __FILE__, __LINE__);
   off [ct - 1] . string_num = s;
   off [ct - 1] . lo_offset = off [ct - 1] . hi_offset = offset;

   return;
  }



Minimizer_t :: Minimizer_t
    (int wl)

// Construct an empty minimizer that uses a window of  wl  characters

  {
   int  i, two_bit_ct;

   two_bit_ct = 4 * sizeof (unsigned int);
     // number of 2-bit units in an  unsigned int
   assert (signature_len <= two_bit_ct);

   window_len = wl;
   signature = new (unsigned int) [1 + window_len - signature_len];
   delay = new (int) [1 + window_len - signature_len];

   signature_mask = order_mask = 0;
   for  (i = 0;  i < signature_len;  i ++)
     {
      signature_mask = (signature_mask << 2) | 0x3;
      order_mask <<= 2;
      if  (i % 2 == 1)
          order_mask |= 0x3;
     }

   signature_ct = 0;
     // indicates an empty (i.e., uninitialized) minimizer
  }



Minimizer_t :: ~ Minimizer_t
    ()

// Destroy this minimizer

  {
   delete [] signature;
   delete [] delay;
  }



void  Minimizer_t :: Advance
    (char ch)

//  Shift character  ch  into this minimizer window on the right
//  and shift out the leftmost window character.  Update signature
//  and offset values as needed.

  {
   unsigned int  new_sig;
   int  i;

   Shift_In (buff, ch, signature_mask);

   new_sig = buff ^ order_mask;
   window_offset --;
   if  (window_offset < 0)
       {
        for  (i = 1;  i < signature_ct && signature [i] <= new_sig;  i ++)
          {
           signature [i - 1] = signature [i];
           delay [i - 1] = delay [i];
          }
        signature [i - 1] = new_sig;
        signature_ct = i;
        if  (i == 1)
            window_offset = window_len - signature_len;
          else
            {
             window_offset += delay [0];
             for  (i = 1;  i < signature_ct - 1;  i ++)
               delay [i] -= delay [0];
             delay [signature_ct - 1] = window_len - signature_len - window_offset;
            }
        delay [0] = 0;
       }
     else
       {
        for  (i = signature_ct - 1;  i >= 0 && new_sig < signature [i];  i --)
          ;
        signature [i + 1] = new_sig;
        signature_ct = i + 2;
        if  (signature_ct == 1)
            window_offset = window_len - signature_len;
          else
            delay [signature_ct - 1] = window_len - signature_len - window_offset;
       }

   return;
  }



void  Minimizer_t :: Dump
    (FILE * fp)

// Display the contents of this minimizer to  fp .

  {
   int  i;

   fprintf (fp,
        "window_len/offset = %d/%d  signature/order_mask = %08x/%08x\n",
        window_len, window_offset, signature_mask, order_mask);
   fprintf (fp, "buff = %08x\n", buff);
   fprintf (fp, "signatures & delays:\n");
   for  (i = 0;  i < signature_ct;  i ++)
     fprintf (fp, "%2d:  %08x  %2d\n", i, signature [i], delay [i]);

   return;
  }



void  Minimizer_t :: Init
    (const char * s)

// Fill this minimizer window with the first  window_len  characters
// of  s  and find the minimizer in it.

  {
   int  i;

   buff = 0u;
   for  (i = 0;  i < signature_len;  i ++)
     Shift_In (buff, s [i], signature_mask);

   window_offset = window_len - signature_len;
   signature [0] = buff ^ order_mask;
   delay [0] = 0;
   signature_ct = 1;

   for  (i = signature_len;  i < window_len;  i ++)
     Advance (s [i]);

   return;
  }



