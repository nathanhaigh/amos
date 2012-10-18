//  A. L. Delcher
//
//  File:  simple-overlap.cc
//
//  Last Modified:  Thu May 27 08:24:56 EDT 2004
//
//  Compute overlaps among an input set of sequences by
//  direct all-pairs alignment.


#include  "simple-overlap.hh"


static string  Bank_Name;
  // Name of read bank from which reads are obtained
static double  Error_Rate = DEFAULT_ERROR_RATE;
  // Fraction of errors allowed in overlap alignment
static bool  Fasta_Input = false;
  // If true, then input comes from the multifasta file named
  // on the command line
static int  Lo_ID = 0, Hi_ID = INT_MAX;
  // Range of indices for which to compute overlaps
static int  Min_Overlap_Len = DEFAULT_MIN_OVERLAP_LEN;
  // Minimum number of bases by which two sequences must overlap
static bool  Show_Alignment = false;
  // If true, then also output the overlap alignments


int  main
    (int argc, char * argv [])

  {
   BankStream_t  read_bank (Read_t::NCODE);
   Simple_Overlap_t  olap;
   vector <char *>  string_list, qual_list;
   vector <char *>  tag_list;
   vector <ID_t> id_list;
   vector <Range_t>  clr_list;
   Alignment_t  ali;
   time_t  now;
   iostream :: fmtflags  status;
   int  i, j, n;


   try
     {
      now = time (NULL);
      cerr << "Starting on " << ctime (& now) << endl;

      Verbose = 0;

      Parse_Command_Line (argc, argv);

      if  (Fasta_Input)
          cerr << "Fasta input file is " << Bank_Name << endl;
        else
          cerr << "Read bank is " << Bank_Name << endl;

      status = cerr . setf (ios :: fixed);
      cerr << "Alignment error rate is " << setprecision (2)
           << Error_Rate << endl;
      cerr . setf (status);
      cerr << "Minimum overlap bases is " << Min_Overlap_Len << endl;

      if  (Fasta_Input)
          Read_Fasta_Strings (string_list, id_list, tag_list, Bank_Name);
        else
          {
           read_bank . open (Bank_Name, B_READ);
           Get_Strings_From_Bank (string_list, qual_list, clr_list,
				  id_list, tag_list, read_bank);
          }

      n = string_list . size ();
      for  (i = 0;  i < n - 1;  i ++)
        {
         double  erate;
         int  lo, hi;

         for  (j = i + 1;  j < n;  j ++)
           {
            Simple_Overlap (string_list [i], strlen (string_list [i]),
                 string_list [j], strlen (string_list [j]), olap);
            if  (olap . a_olap_len < Min_Overlap_Len
                    || olap . b_olap_len < Min_Overlap_Len)
                continue;
            erate = (2.0 * olap . errors)
                / (olap . a_olap_len + olap . b_olap_len);
            if  (erate <= Error_Rate)
                {
                 olap . a_id = id_list [i];
                 olap . b_id = id_list [j];
                 olap . flipped = false;
                 Output (stdout, olap);
                 if  (Show_Alignment)
                     {
                      if  (olap . a_hang <= 0)
                          {
                           lo = Max (- olap . a_hang - 5, 0);
                           hi = - olap . a_hang + 5;
                           Overlap_Align (string_list [i], strlen (string_list [i]),
                                string_list [j], lo, hi, strlen (string_list [j]),
                                1, -3, -2, -2, ali);
                           printf (
                           "\nOverlap  a: %d .. %d of %d (%s)  b: %d .. %d of %d (%s)\n",
                                ali . a_lo, ali . a_hi, id_list [i], tag_list [i],
                                ali . b_lo, ali . b_hi, id_list [j], tag_list [j]);
                           ali . Print (stdout, string_list [i], string_list [j]);
                          }
                        else
                          {
                           lo = Max (olap . a_hang - 5, 0);
                           hi = olap . a_hang + 5;
                           Overlap_Align (string_list [j], strlen (string_list [j]),
                                string_list [i], lo, hi, strlen (string_list [i]),
                                1, -3, -2, -2, ali);
                           printf (
                           "\nOverlap  a: %d .. %d of %d (%s)  b: %d .. %d of %d (%s)\n",
                                ali . a_lo, ali . a_hi, id_list [j], tag_list [j],
                                ali . b_lo, ali . b_hi, id_list [i], tag_list [i]);
                           ali . Print (stdout, string_list [j], string_list [i]);
                          }
                     }
                }
           }

         Reverse_Complement (string_list [i]);

         for  (j = i + 1;  j < n;  j ++)
           {
            Simple_Overlap (string_list [i], strlen (string_list [i]),
                 string_list [j], strlen (string_list [j]), olap);
            if  (olap . a_olap_len < Min_Overlap_Len
                    || olap . b_olap_len < Min_Overlap_Len)
                continue;
            erate = (2.0 * olap . errors)
                / (olap . a_olap_len + olap . b_olap_len);
            if  (erate <= Error_Rate)
                {
                 int  save;

                 if  (Show_Alignment)
                     {
                      if  (olap . a_hang <= 0)
                          {
                           lo = Max (- olap . a_hang - 5, 0);
                           hi = - olap . a_hang + 5;
                           Overlap_Align (string_list [i], strlen (string_list [i]),
                                string_list [j], lo, hi, strlen (string_list [j]),
                                1, -3, -2, -2, ali);
                           printf (
   "\nOverlap  a: %d .. %d of complement %d (%s)  b: %d .. %d of %d (%s)\n",
                                ali . a_lo, ali . a_hi, id_list [i], tag_list [i],
                                ali . b_lo, ali . b_hi, id_list [j], tag_list [j]);
                           ali . Print (stdout, string_list [i], string_list [j]);
                          }
                        else
                          {
                           lo = Max (olap . a_hang - 5, 0);
                           hi = olap . a_hang + 5;
                           Overlap_Align (string_list [j], strlen (string_list [j]),
                                string_list [i], lo, hi, strlen (string_list [i]),
                                1, -3, -2, -2, ali);
                           printf (
   "\nOverlap  a: %d .. %d of %d (%s)  b: %d .. %d of complement %d (%s)\n",
                                ali . a_lo, ali . a_hi, id_list [j], tag_list [j],
                                ali . b_lo, ali . b_hi, id_list [i], tag_list [i]);
                           ali . Print (stdout, string_list [j], string_list [i]);
                          }
                     }

                 // Re-orient with a forward and b reversed
                 save = olap . a_hang;
                 olap . a_hang = - olap . b_hang;
                 olap . b_hang = - save;

                 olap . a_id = id_list [i];
                 olap . b_id = id_list [j];
                 olap . flipped = true;
                 Output (stdout, olap);
                }
           }
        }

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



static void  Get_Strings_From_Bank
    (vector <char *> & s, vector <char *> & q,
     vector <Range_t> & clr_list, vector <ID_t> & id_list,
     vector <char * > & tag_list, BankStream_t & read_bank)

//  Populate  s  and  q  with sequences and quality values, resp.,
//  from  read_bank .  Put the clear-ranges for the sequences in  clr_list .
//   read_bank  must already be opened.  Put the identifying tags for the
//  sequences in  tag_list .

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
   id_list . clear();
   clr_list . clear ();

   char  * tmp, tag_buff [100];
   string  seq;
   string  qual;
   Range_t  clear;
   int  this_offset;
   int  a, b, j, len, qlen;

   while ( read_bank >> read )
     {
      id_list . push_back (read . getIID());
      tag_list . push_back (strdup (read . getEID() . c_str()));
      
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



static void  Output
    (FILE * fp, const Simple_Overlap_t & olap)

//  Print the contents of  olap  to  fp .

  {
   fprintf (fp, "%5d %5d  %c %5d %5d  %5d %5d  %5d  %3d  %4.2f\n",
        olap . a_id, olap . b_id, olap . flipped ? 'I' : 'N',
        olap . a_hang, olap . b_hang,
        olap . a_olap_len, olap . b_olap_len, olap . score,
        olap . errors,
        200.0 * olap . errors
             / (olap . a_olap_len + olap . b_olap_len));

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

   while (!errflg && ((ch = getopt (argc, argv, "aE:Fho:v:")) != EOF))
     switch  (ch)
       {
        case  'a' :
          Show_Alignment = true;
          break;

        case  'E' :
          Error_Rate = strtod (optarg, NULL);
          break;

        case  'F' :
          Fasta_Input = true;
          break;

        case  'h' :
          errflg = true;
          break;

        case  'o' :
          Min_Overlap_Len = strtol (optarg, NULL, 10);
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



static void  Read_Fasta_Strings
     (vector <char *> & s, vector <ID_t> & id_list,
      vector <char *> & tag_list, const std :: string & fn)

//  Open file named  fn  and read FASTA-format sequences from it
//  into  s  and their tags into  tag_list .

  {
   FILE  * fp;
   std :: string  seq, hdr;

   fp = File_Open (fn . c_str (), "r", __FILE__, __LINE__);
   s . clear ();
   id_list . clear();
   tag_list . clear ();

   char  tag [MAX_LINE];
   char  * tmp;
   int  j, len;
   int cnt = 0;

   while  (Fasta_Read (fp, seq, hdr))
     {
       if ( cnt >= Lo_ID  &&  cnt < Hi_ID )
	 {
	   tmp = strdup (seq . c_str ());
	   len = seq . length ();
	   for  (j = 0;  j < len;  j ++)
	     tmp [j] = tolower (tmp [j]);
	   s . push_back (tmp);
	   
	   sscanf (hdr . c_str (), "%s", tag);
	   tag_list . push_back (strdup (tag));
	   id_list . push_back (cnt);
	 }
       ++ cnt;
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
           "USAGE:  %s  <bank-name>\n"
           "\n"
           "Compute pairwise overlaps among a set of sequences by\n"
           "brute-force all-pairs alignment.  Sequences are obtained\n"
           "from <bank-name>\n"
           "\n"
           "Options:\n"
           "  -a       Also show alignments of overlaps \n"
           "  -E <x>   Maximum error rate for overlaps is <x>\n"
           "           e.g., -E 0.06 for 6% error rate\n"
           "  -F       Input is a fasta file\n"
           "  -h       Print this usage message\n"
           "  -o <n>   Set minimum overlap length to <n>\n"
           "  -v <n>   Set verbose level to <n>.  Higher produces more output.\n"
           "\n",
           command);

   return;
  }



