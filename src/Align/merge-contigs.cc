//  A. L. Delcher
//
//  File:  merge-contigs.cc
//
//  Last Modified:  15 August 2003
//
//  Read Celera-format contigs and merge their multialignments


#include  "delcher.hh"
#include  "WGA_databank.hh"
#include  "WGA_datatypes.hh"
#include  "CelMsgWGA.hh"
#include  "align.hh"
#include  "prob.hh"
#include  "fasta.hh"
#include  <vector>
#include  <string>
#include  <algorithm>


using namespace std;


const int  MAX_LINE = 1000;
const int  NEW_SIZE = 1000;


enum  Input_Format_t
     {SIMPLE_CONTIG_FORMAT, CELERA_MSG_FORMAT};

struct  ID_Pair_t
  {
   int  uid, iid;
  };

struct  Unitig_Ref_t
  {
   int  uid;
   char  * seq;
  };



static string  Bank_Name;
  // Name of read bank from which reads are obtained
static string  Map_File_Name;
  // Name of file containing uid-iid map
static bool  Output_Alignments = false;
  // If set true (by -a option) then multialignments will be output
  // instead of just messages with consensus sequences
static bool  Output_FASTA = false;
  // If set true (by -f option) then multialignments will be output
  // instead of just messages with consensus sequences
static string  Tig_File_Name;
  // Name of file containing input contig/unitig messages
static string  Unitig_File_Name;
  // Name of fasta file surrogate unitig sequences


static bool  Range_Precedes
    (const Ordered_Range_t & a, const Ordered_Range_t & b)
  {
   int  a_start, a_end, b_start, b_end;

   a_start = a . getStart ();
   a_end = a . getEnd ();
   b_start = b . getStart ();
   b_end = b . getEnd ();

   if  (a_end < a_start)
       Swap (a_start, a_end);
   if  (b_end < b_start)
       Swap (b_start, b_end);

   if  (a_start < b_start)
       return  true;
   if  (a_start == b_start && a_end < b_end)
       return  true;

   return  false;
  }


static void  Copy
    (vector <char *> & S, const vector <char *> & T);
static int  Find_Unitig
    (int uid, const vector <Unitig_Ref_t> & uni_list);
static void  Get_Strings_And_Lengths
    (vector <char *> & s, vector <char *> & q, vector <int> & str_len,
     vector <char *> & tag, const Celera_Message_t & msg,
     ReadBank_t & read_bank, const vector <ID_Pair_t> & map,
     const vector <Unitig_Ref_t> & uni_list);
static void  Load_ID_Map
    (vector <ID_Pair_t> & map, const char * fn);
static void  Load_Unitigs
    (vector <Unitig_Ref_t> & uni_list, const char * fn);
static int  Lookup
    (int uid, const vector <ID_Pair_t> & map);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   ReadBank_t  read_bank;
   Celera_Message_t  msg;
   Read_t  read;
   FILE  * input_fp;
   vector <char *>  string_list, qual_list, tag;
   vector <char *>  sl1, sl2;
   vector <char *>  tg1, tg2;
   vector <int>  offset;
   vector <int>  ref, str_len;
   vector <Range_t>  pos;
   vector < vector <int> >  del_list;
   Gapped_Multi_Alignment_t  gma, gma1, gma2;
   vector <ID_Pair_t>  map;
   vector <Unitig_Ref_t>  uni_list;
   time_t  now;
   int  contig_ct, cons_len;

   now = time (NULL);
   cerr << "Starting at " << ctime (& now) << endl;

   Verbose = 1;

   Parse_Command_Line (argc, argv);

   cerr << "Read bank is " << Bank_Name << endl;
   cerr << "Processing messages from file " << Tig_File_Name << endl;
   cerr << "Map file is " << Map_File_Name << endl;

   Load_ID_Map (map, Map_File_Name . c_str ());
   Load_Unitigs (uni_list, Unitig_File_Name . c_str ());

   input_fp = File_Open (Tig_File_Name . c_str (), "r");
   read_bank . openStore (Bank_Name);

   contig_ct = 0;
   while  (msg . read (input_fp))
     {
      if  (msg . getMsgType () == CCO_MSG)
          {
           cerr << "Process contig " << msg . getAccession () << endl;

           Get_Strings_And_Lengths
               (string_list, qual_list, str_len, tag, msg, read_bank, map,
                uni_list);

           gma . Make_From_CCO_Msg (msg, str_len);

           contig_ct ++;

           if  (contig_ct == 1)
               {
                gma1 = gma;
                Copy (sl1, string_list);
                Copy (tg1, tag);
               }
           else if  (contig_ct == 2)
               {
                gma2 = gma;

                Copy (sl2, string_list);
                Copy (tg2, tag);
               }
          }
     }

   fclose (input_fp);
   read_bank . closeStore ();

   cout << endl << endl << "Contig #1" << endl;
   gma1 . Print (stdout, sl1, true, 60, & tg1);

if  (0)
{
 int  i, n;

 n = tg1 . size ();
 for  (i = 0;  i < n;  i ++)
   tg1 [i] [0] = 'a';

 n = tg2 . size ();
 for  (i = 0;  i < n;  i ++)
   tg2 [i] [0] = 'b';
}

#if  0
   cons_len = gma2 . Ungapped_Consensus_Len ();

//   gma1 . Partial_Merge (gma2, cons_len - 3731, cons_len - 2452, 1322, 2638,
//        sl1, sl2, & tg1, & tg2);

   gma2 . Reverse_Complement (sl2);
   cout << endl << endl << "Contig #2 after reverse" << endl;
   gma2 . Print (stdout, sl2, true, 60);

   gma1 . Partial_Merge (gma2, cons_len - 3731, cons_len - 2566, 1322, 2487,
        sl1, sl2, & tg1, & tg2);
   gma1 . Partial_Merge (gma2, cons_len - 2474, cons_len - 236, 3080, 5324,
        sl1, sl2, & tg1, & tg2);

   cout << endl << endl << "Contig #1 after merge" << endl;
   gma1 . Print (stdout, sl1, true, 60, & tg1);
#endif

#if  0
   {
    char  * s = "gacgttc";
    char  * t = "ggactcc";
    Alignment_t  ali;

    Global_Align (s, 7, t, 0, 7, 2, -3, -2, -1, ali);
    ali . Print (stdout, s, t);
   }
#endif

   return  0;
  }



static void  Copy
    (vector <char *> & S, const vector <char *> & T)

//  Copy contents of  T  into  S  allocating new memory
//  for each string

  {
   int  i, n;

   n = S . size ();
   for  (i = 0;  i < n;  i ++)
     free (S [i]);
   S . clear ();

   n = T . size ();
   for  (i = 0;  i < n;  i ++)
     S . push_back (strdup (T [i]));

   return;
  }



static int  Find_Unitig
    (int uid, const vector <Unitig_Ref_t> & uni_list)

//  Search for the entry in  uni_list  whose  uid  field matches
//   uid  and return the subscript of that entry.  Return  -1
//  if none is found.   uni_list  is assumed to be in sorted order by
//   uid  so a binary search is done.

  {
   int  lo, hi, mid;

   lo = 0;
   hi = uni_list . size () - 1;
   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (uid == uni_list [mid] . uid)
          return  mid;
      else if  (uid < uni_list [mid] . uid)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   return  -1;
  }



static void  Get_Strings_And_Lengths
    (vector <char *> & s, vector <char *> & q, vector <int> & str_len,
     vector <char *> & tag, const Celera_Message_t & msg,
     ReadBank_t & read_bank, const vector <ID_Pair_t> & map,
     const vector <Unitig_Ref_t> & uni_list)

//  Populate  s  with reads for the contig in  msg  with reads coming
//  from  read_bank , which must already be opened.  Put the corresponding
//  quality-value strings for the reads into  q , put the lengths of
//  the reads into  str_len , and put an identifying tag for
//  each string in  tag .  Use  map  to convert uids
//  (which is what's in the CCO messages) to iids (which is what's in
//  the read bank).  Get surrogate unitig sequences from  uni_list .

  {
   const vector <Celera_MPS_Sub_Msg_t> &  frgs = msg . getMPSList ();
   const vector <Celera_UPS_Sub_Msg_t> &  unis = msg . getUPSList ();
   vector <int>  id_list;
   Read_t  read;
   Ordered_Range_t  position;
   char  tag_buff [MAX_LINE];
   char  * tmp;
   int  len;
   int  a, b, i, j, n;

   n = s . size ();
   for  (i = 0;  i < n;  i ++)
     free (s [i]);
   s . clear ();

   n = q . size ();
   for  (i = 0;  i < n;  i ++)
     free (q [i]);
   q . clear ();

   n = tag . size ();
   for  (i = 0;  i < n;  i ++)
     free (tag [i]);
   tag . clear ();

   str_len . clear ();

   n = msg . getNumFrags ();
   for  (i = 0;  i < n;  i ++)
     {
      string  seq;
      string  qual;
      Range_t  clear;
      int  iid, uid;
      int  qlen;

      position = frgs [i] . getPosition ();
      a = position . getStart ();
      b = position . getEnd ();
      
      uid = frgs [i] . getId ();
      iid = Lookup (uid, map);
      read = read_bank . fetch (iid);
      clear = read . getClearRange ();
      if  (Verbose > 2)
          read . Print (cerr);
      seq = read . getSeqString (clear);
      qual = read . getQualString (clear);
      if  (b < a)
          {
           Reverse_Complement (seq);
           reverse (qual . begin (), qual . end ());
          }

      len = seq . length ();
      tmp = strdup (seq . c_str ());
      for  (j = 0;  j < len;  j ++)
        tmp [j] = tolower (tmp [j]);
      s . push_back (tmp);

      qlen = qual . length ();
      if  (len != qlen)
          {
           sprintf (Clean_Exit_Msg_Line,
                "ERROR:  Sequence length (%d) != quality length (%d) for read %d",
                len, qlen, uid);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      tmp = strdup (qual . c_str ());
      q . push_back (tmp);

      str_len . push_back (len);

      sprintf (tag_buff, "r%d", uid);
      tag . push_back (strdup (tag_buff));
     }

   n = msg . getNumUnitigs ();
   printf ("Num unitigs = %d\n", n);
   for  (i = 0;  i < n;  i ++)
     {
      Celera_Unitig_Type_t  u_type;
      int  uid;

      position = unis [i] . getPosition ();
      a = position . getStart ();
      b = position . getEnd ();
      
      uid = unis [i] . getId ();
      u_type = unis [i] . getUnitigType ();
      printf ("Unitig %8d  %c\n", uid, Convert_Char (u_type));
      if  (u_type == STONE_UNI_T)
          {
           j = Find_Unitig (uid, uni_list);
           if  (j < 0)
               {
                sprintf (Clean_Exit_Msg_Line,
                     "ERROR:  Could not find unitig %d in unitig list",
                         uid);
                Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
               }
           
           len = strlen (uni_list [j] . seq);
           tmp = strdup (uni_list [j] . seq);
           for  (j = 0;  j < len;  j ++)
             tmp [j] = tolower (tmp [j]);
           if  (b < a)
               Reverse_Complement (tmp);
           s . push_back (tmp);

           // Make artificial quality string of all 10's
           tmp = (char *) Safe_malloc (len + 1);
           memset (tmp, QUALITY_OFFSET + 10, len);
           tmp [len] = '\0';
           q . push_back (tmp);

           str_len . push_back (len);

           sprintf (tag_buff, "u%d", uid);
           tag . push_back (strdup (tag_buff));
          }
     }

   return;
  }



static void  Load_ID_Map
    (vector <ID_Pair_t> & map, const char * fn)

//  Load the  iid-uid  map from the file name  fn  into
//   map .

  {
   FILE  * fp;
   char  tag [MAX_LINE];
   ID_Pair_t  pair;

   map . clear ();
   fp = File_Open (fn, "r");
   while  (fscanf (fp, "%d %d %s", & pair . uid, & pair . iid, tag) == 3)
     if  (strcmp (tag, "AFG") == 0)
         map . push_back (pair);

   fclose (fp);

   return;
  }



static void  Load_Unitigs
    (vector <Unitig_Ref_t> & uni_list, const char * fn)

//  Load unitigs from the file named  fn  and store them in  uni .

  {
   FILE  * fp;
   string  s, tag;
   Unitig_Ref_t  u;

   uni_list . clear ();
   fp = File_Open (fn, "r");
   while  (Fasta_Read (fp, s, tag))
     {
      u . uid = int (strtol (tag . c_str (), NULL, 10));
      if  (u . uid == 0)
          {
           sprintf (Clean_Exit_Msg_Line,
                "ERROR:  Bad unitig uid \"%s\"",
                    tag . c_str ());
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      u . seq = strdup (s . c_str ());
      uni_list . push_back (u);
     }

   fclose (fp);

   return;
  }



static int  Lookup
    (int uid, const vector <ID_Pair_t> & map)

//  Find  uid  in sorted list  map  and return the corresponding
//  iid .  Fail if not found.

  {
   int  lo, hi, mid;

   lo = 0;  hi = map . size () - 1;
   while  (lo <= hi)
     {
      mid = (lo + hi) / 2;
      if  (map [mid] . uid == uid)
          return  map [mid] . iid;
      else if  (uid < map [mid] . uid)
          hi = mid - 1;
        else
          lo = mid + 1;
     }

   cerr << "Lookup of uid " << uid << " failed" << endl;
   exit (EXIT_FAILURE);
  }



static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "afh")) != EOF))
     switch  (ch)
       {
        case  'a' :
          Output_Alignments = true;
          break;

        case  'f' :
          Output_FASTA = true;
          break;

        case  'h' :
          errflg = true;
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

   if  (optind > argc - 4)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Tig_File_Name = argv [optind ++];
   Bank_Name = argv [optind ++];
   Map_File_Name = argv [optind ++];
   Unitig_File_Name = argv [optind ++];

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <tig-file> <bank-name> <map-file> <unitig-file>\n"
           "\n"
           "Read unitigs/contigs from <tig-file> and create\n"
           "multialignments and/or consensus sequences for them.  Reads are\n"
           "obtained from <bank-name>.  <map-file>  gives the uid-iid\n"
           "mapping for reads.  <unitig-file> is the fasta sequence of\n"
           "surrogate unitigs\n"
           "\n"
           "Options:\n"
           "  -a    Output alignments instead of consensus messages\n"
           "  -f    Output consensus only in FASTA format\n"
           "  -h    Print this usage message\n"
           "\n",
           command);

   return;
  }



