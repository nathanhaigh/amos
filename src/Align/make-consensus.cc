//  A. L. Delcher
//
//  File:  make-consensus.cc
//
//  Last Modified:  10 March 2003
//
//  Read Celera-format unitigs/contigs and create multialignments and/or
//  consensus sequences for them.


#include  "delcher.hh"
#include  "WGA_databank.hh"
#include  "WGA_datatypes.hh"
#include  "CelMsgWGA.hh"
#include  "align.hh"
#include  "prob.hh"
#include  <vector>
#include  <string>
#include  <algorithm>

using namespace std;


const int  NEW_SIZE = 1000;


static string  Bank_Name;
  // Name of read bank from which reads are obtained
static bool  Do_Contig_Messages = false;
  // If set true (by -c option) then contig messages in the
  // input will be processed
static bool  Do_Unitig_Messages = false;
  // If set true (by -u option) then unitig messages in the
  // input will be processed
static bool  Output_Alignments = false;
  // If set true (by -a option) then multialignments will be output
  // instead of just messages with consensus sequences
static string  Tig_File_Name;
  // Name of file containing input contig/unitig messages


static void  Get_Strings_And_Offsets
    (vector <char *> & s, vector <char *> & q, vector <int> & offset,
     const Celera_Message_t & msg, ReadBank_t & read_bank);
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
   Gapped_Multi_Alignment_t  gma;
   int  contig_ct, unitig_ct;

   cerr << "Starting on " << __DATE__ << " at " << __TIME__ << endl;

   Verbose = 1;

   Parse_Command_Line (argc, argv);

   cerr << "Read bank is " << Bank_Name << endl;
   cerr << "Processing ";
   if  (Do_Contig_Messages)
       cerr << "contig";
     else
       cerr << "unitig";
   cerr << " messages from file " << Tig_File_Name << endl;

   input_fp = File_Open (Tig_File_Name . c_str (), "r");
   read_bank . openStore (Bank_Name);

   unitig_ct = contig_ct = 0;
   while  (msg . read (input_fp))
     {
      if  (msg . getMsgType () == IUM_MSG && Do_Unitig_Messages)
          {
           cerr << "Process unitig " << msg . getAccession () << endl;
           Get_Strings_And_Offsets
               (string_list, qual_list, offset, msg, read_bank);

           Multi_Align (string_list, offset, 5, 0.04, gma);

#if 0
           cout << endl << endl << "Unitig #" << msg . getAccession () << endl;
           gma . Print (stdout, string_list, 60);
#else
           gma . Set_Consensus_And_Qual (string_list, qual_list);
           msg . setSequence (gma . getConsensusString ());
           msg . setQuality (gma . getQualityString ());
           msg . print (stdout);
#endif

           unitig_ct ++;
          }
      if  (msg . getMsgType () == ICM_MSG && Do_Contig_Messages)
          {
           contig_ct ++;
          }
     }
   if  (Do_Unitig_Messages)
       cerr << unitig_ct << " IUM messages processed" << endl;
   if  (Do_Contig_Messages)
       cerr << contig_ct << " ICM messages processed" << endl;

   fclose (input_fp);
   read_bank . closeStore ();

   return  0;
  }



static void  Get_Strings_And_Offsets
    (vector <char *> & s, vector <char *> & q, vector <int> & offset,
     const Celera_Message_t & msg, ReadBank_t & read_bank)

//  Populate  s  and  offset  with reads and their unitig positions
//  for the unitig in  msg  with reads coming from  read_bank.
//   read_bank  must already be opened.  Put the corresponding quality-value
//  strings for the reads into  q .

  {
   const vector <Celera_IMP_Sub_Msg_t> &  frgs = msg . getFragList ();
   Read_t  read;
   Ordered_Range_t  position;
   int  id, prev_offset;
   int  i, n;

   n = s . size ();
   for  (i = 0;  i < n;  i ++)
     free (s [i]);
   s . clear ();
   q . clear ();
   offset . clear ();

   prev_offset = 0;
   n = msg . getNumFrags ();
   for  (i = 0;  i < n;  i ++)
     {
      char  * tmp;
      string  seq;
      string  qual;
      Range_t  clear;
      int  this_offset;
      int  a, b, j, len, qlen;

      id = frgs [i] . getId ();
      position = frgs [i] . getPosition ();
      a = position . getStart ();
      b = position . getEnd ();

      read = read_bank . fetch (id);
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
                    len, qlen, id);
           Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
          }
      tmp = strdup (qual . c_str ());
      q . push_back (tmp);

      this_offset = Min (a, b);
      offset . push_back (this_offset - prev_offset);
      prev_offset = this_offset;
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

   while  (! errflg && ((ch = getopt (argc, argv, "achu")) != EOF))
     switch  (ch)
       {
        case  'a' :
          Output_Alignments = true;
          break;

        case  'c' :
          Do_Contig_Messages = true;
          Do_Unitig_Messages = false;
          break;

        case  'h' :
          errflg = true;
          break;

        case  'u' :
          Do_Unitig_Messages = true;
          Do_Contig_Messages = false;
          break;

        case  '?' :
          fprintf (stderr, "Unrecognized option -%c\n", optopt);

        default :
          errflg = true;
       }

   if  (! Do_Unitig_Messages && ! Do_Contig_Messages)
       {
        fprintf (stderr, "\nERROR:  Must specify either -u or -c\n\n");
        errflg = true;
       }

   if  (errflg)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   if  (optind > argc - 2)
       {
        Usage (argv [0]);
        exit (EXIT_FAILURE);
       }

   Tig_File_Name = argv [optind ++];
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
           "USAGE:  %s  <tig-file> <bank-name>\n"
           "\n"
           "Read Celera-format unitigs/contigs from <tig-file> and create\n"
           "multialignments and/or consensus sequences for them.  Reads are\n"
           "obtained from <bank-name>\n"
           "\n"
           "Options:\n"
           "  -a    Output alignments instead of consensus messages\n"
           "  -c    Process contig messages\n"
           "  -h    Print this usage message\n"
           "  -u    Process unitig messages\n"
           "\n",
           command);

   return;
  }
