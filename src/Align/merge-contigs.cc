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


static void  Get_Strings_And_Lengths
    (vector <char *> & s, vector <char *> & q, vector <int> & str_len,
     const Celera_Message_t & msg, ReadBank_t & read_bank,
     const vector <ID_Pair_t> & map);
static void  Load_ID_Map
    (vector <ID_Pair_t> & map, const char * fn);
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
   vector <char *>  string_list, qual_list;
   vector <int>  offset;
   vector <int>  ref, str_len;
   vector <Range_t>  pos;
   vector < vector <int> >  del_list;
   Gapped_Multi_Alignment_t  gma;
   vector <ID_Pair_t>  map;
   time_t  now;
   int  contig_ct;

   now = time (NULL);
   cerr << "Starting at " << ctime (& now) << endl;

   Verbose = 1;

   Parse_Command_Line (argc, argv);

   cerr << "Read bank is " << Bank_Name << endl;
   cerr << "Processing messages from file " << Tig_File_Name << endl;
   cerr << "Map file is " << Map_File_Name << endl;

   Load_ID_Map (map, Map_File_Name . c_str ());

   input_fp = File_Open (Tig_File_Name . c_str (), "r");
   read_bank . openStore (Bank_Name);

   contig_ct = 0;
   while  (msg . read (input_fp))
     {
      if  (msg . getMsgType () == CCO_MSG)
          {
           cerr << "Process contig " << msg . getAccession () << endl;

           Get_Strings_And_Lengths
               (string_list, qual_list, str_len, msg, read_bank, map);

           gma . Make_From_CCO_Msg (msg, str_len);

           cout << endl << endl << "Contig #" << msg . getAccession ()
                << endl;
           gma . Print (stdout, string_list, 60);

           contig_ct ++;
          }
     }

   fclose (input_fp);
   read_bank . closeStore ();

   return  0;
  }



static void  Get_Strings_And_Lengths
    (vector <char *> & s, vector <char *> & q, vector <int> & str_len,
     const Celera_Message_t & msg, ReadBank_t & read_bank,
     const vector <ID_Pair_t> & map)

//  Populate  s  with reads for the contig in  msg  with reads coming
//  from  read_bank , which must already be opened.  Put the corresponding
//  quality-value strings for the reads into  q  and put the lengths of
//  the reads into  str_len .  Use  map  to convert uids
//  (which is what's in the CCO messages) to iids (which is what's in
//  the read bank).

  {
   const vector <Celera_MPS_Sub_Msg_t> &  frgs = msg . getMPSList ();
   vector <int>  id_list;
   Read_t  read;
   Ordered_Range_t  position;
   Range_t  range;
   int  i, j, n;

   n = s . size ();
   for  (i = 0;  i < n;  i ++)
     free (s [i]);
   s . clear ();

   n = q . size ();
   for  (i = 0;  i < n;  i ++)
     free (q [i]);
   q . clear ();

   n = msg . getNumFrags ();
   for  (i = 0;  i < n;  i ++)
     {
      char  * tmp;
      string  seq;
      string  qual;
      Range_t  clear;
      int  iid, uid;
      int  len, qlen;
      int  a, b;

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
           sprintf
               (Clean_Exit_Msg_Line,
                "ERROR:  Sequence length (%d) != quality length (%d) for read %d\n",
                    len, qlen, uid);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      tmp = strdup (qual . c_str ());
      q . push_back (tmp);
      str_len . push_back (len);
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

   if  (optind > argc - 3)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Tig_File_Name = argv [optind ++];
   Bank_Name = argv [optind ++];
   Map_File_Name = argv [optind ++];

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <tig-file> <bank-name> <map-file>\n"
           "\n"
           "Read unitigs/contigs from <tig-file> and create\n"
           "multialignments and/or consensus sequences for them.  Reads are\n"
           "obtained from <bank-name>.  <map-file>  gives the uid-iid\n"
           "mapping for reads\n"
           "\n"
           "Options:\n"
           "  -a    Output alignments instead of consensus messages\n"
           "  -f    Output consensus only in FASTA format\n"
           "  -h    Print this usage message\n"
           "\n",
           command);

   return;
  }



