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
   time_t  now;
   iostream :: fmtflags  status;
   int  lo_iid, hi_iid;
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
          {
           FILE  * fp;
           std :: string  seq, hdr;
           Alignment_t  ali;

           fp = File_Open (Bank_Name, "r");
           while  (Fasta_Read (fp, seq, hdr))
             string_list . push_back (strdup (seq . c_str ()));


           char  line [MAX_LINE];

           while  (fgets (line, MAX_LINE, stdin) != NULL)
             {
              char  ori [10];
              int  a, b, hang, lo, hi;

              sscanf (line, "%d %d %s %d", & a, & b, ori, & hang);
              hang *= -1;
              lo = Max (hang - 5, 0);
              hi = hang + 5;

              Overlap_Align (string_list [a - 1], strlen (string_list [a - 1]),
                   string_list [b - 1], lo, hi, strlen (string_list [b - 1]),
                   1, -3, -2, -2, ali);

              printf ("\n>>> %5d %5d %3d\n", a, b, hang);
              ali . Print (stdout, string_list [a - 1], string_list [b - 1]);
             }
           exit (-1);
          }
        else
          {
           read_bank . open (Bank_Name);

           lo_iid = 1;
           hi_iid = Min (50, int (read_bank . getLastIID ()));

           Get_Strings_From_Bank (lo_iid, hi_iid, string_list, qual_list, clr_list,
                tag_list, read_bank);
          }

      n = string_list . size ();
      for  (i = 0;  i < n - 1;  i ++)
        {
         double  erate;

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
                 olap . a_id = lo_iid + i;
                 olap . b_id = lo_iid + j;
                 olap . flipped = false;
                 Output (stdout, olap);
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

                 // Re-orient with a forward and b reversed
                 save = olap . a_hang;
                 olap . a_hang = - olap . b_hang;
                 olap . b_hang = - save;

                 olap . a_id = lo_iid + i;
                 olap . b_id = lo_iid + j;
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

   while (!errflg && ((ch = getopt (argc, argv, "fho:sv:")) != EOF))
     switch  (ch)
       {
        case  'f' :
          Fasta_Input = true;
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
           "  -h       Print this usage message\n"
           "  -o <n>   Set minimum overlap length to <n>\n"
           "  -s       Get id tag from readbank comment string\n"
           "  -v <n>   Set verbose level to <n>.  Higher produces more output.\n"
           "\n",
           command);

   return;
  }



