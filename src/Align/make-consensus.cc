//  A. L. Delcher
//
//  File:  make-consensus.cc
//
//  Last Modified:  10 March 2003
//
//  Read Celera-format unitigs/contigs and create multialignments and/or
//  consensus sequences for them.


#include  "universals_AMOS.hh"
#include  "delcher.hh"
#include  "CelMsg.hh"
#include  "align.hh"
#include  "prob.hh"
#include  "fasta.hh"
#include  <vector>
#include  <string>
#include  <algorithm>


using namespace std;
using namespace AMOS;

const int  MAX_LINE = 1000;
const int  NEW_SIZE = 1000;
const int  ALIGN_WIGGLE = 10;
   // Number of positions left and right of specified location
   // to look for alignments.


enum  Input_Format_t
     {PARTIAL_READ_INPUT, SIMPLE_CONTIG_INPUT, CELERA_MSG_INPUT};
enum  Output_Format_t
     {MULTI_ALIGN_OUTPUT, ONLY_FASTA_OUTPUT, CELERA_MSG_OUTPUT,
      TIGR_CONTIG_OUTPUT};


static string  Bank_Name;
  // Name of read bank from which reads are obtained
static bool  Do_Contig_Messages = false;
  // If set true (by -c option) then contig messages in the
  // input will be processed
static bool  Do_Unitig_Messages = false;
  // If set true (by -u option) then unitig messages in the
  // input will be processed
static Input_Format_t  Input_Format = SIMPLE_CONTIG_INPUT;
  // Type of input
static Output_Format_t  Output_Format = CELERA_MSG_OUTPUT;
  // Type of output to produce
static string  Tig_File_Name;
  // Name of file containing input contig/unitig messages


static void  Get_Strings_And_Offsets
    (vector <char *> & s, vector <char *> & q, vector <Range_t> & clr_list,
     vector <char *> & tag_list, vector <int> & offset,
     const Celera_Message_t & msg, Bank_t & read_bank);
static void  Get_Strings_And_Offsets
    (vector <char *> & s, vector <char *> & q, vector <Range_t> & clr_list,
     vector <char * > & tag_list, vector <int> & offset, const vector <int> & fid,
     const vector <Ordered_Range_t> pos, const vector <Ordered_Range_t> seg,
     Bank_t & read_bank);
static void  Output_Unit
    (const string & label, const string & id, int num_reads,
     Gapped_Multi_Alignment_t & gma, Celera_Message_t & msg,
     const vector <char *> & string_list,
     const vector <char *> & qual_list, const vector <Range_t> & clr_list,
     const vector <char *> tag_list);
static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   Bank_t  read_bank (Read_t::NCode( ));
   Celera_Message_t  msg;
   Read_t  read;
   FILE  * input_fp;
   string  label = "Contig";
   vector <char *>  string_list, qual_list;
   vector <char *>  tag_list;
   vector <int>  offset;
   vector <int>  ref;
   vector <Range_t>  pos, clr_list;
   vector < vector <int> >  del_list;
   Gapped_Multi_Alignment_t  gma;
   time_t  now;
   int  contig_ct, unitig_ct;

   
   now = time (NULL);
   cerr << "Starting on " << ctime (& now) << endl;

   Verbose = 1;

   Parse_Command_Line (argc, argv);

   cerr << "Read bank is " << Bank_Name << endl;

   gma . setPrintFlag (PRINT_WITH_DIFFS);

   if  (Input_Format == CELERA_MSG_INPUT)
       {
        cerr << "Processing ";
        if  (Do_Contig_Messages)
            cerr << "contig";
          else
            {
             cerr << "unitig";
             label = "Unitig";
            }
        cerr << " messages from file " << Tig_File_Name << endl;

        input_fp = File_Open (Tig_File_Name . c_str (), "r");
        read_bank . open (Bank_Name);

        unitig_ct = contig_ct = 0;
        while  (msg . read (input_fp))
          {
           if  (msg . getMsgType () == IUM_MSG && Do_Unitig_Messages)
               {
                cerr << "Process unitig " << msg . getAccession () << endl;
                Get_Strings_And_Offsets
                    (string_list, qual_list, clr_list, tag_list, offset,
                     msg, read_bank);

                Multi_Align (string_list, offset, ALIGN_WIGGLE, 0.04, gma, & ref);
                Permute (qual_list, ref);
                Permute (clr_list, ref);
                Permute (tag_list, ref);

                gma . Set_Flipped (clr_list);
                gma . Get_Positions (pos);
                gma . Extract_IMP_Dels (del_list);
                msg . Update_IMPs (pos, ref, del_list);

                Output_Unit (label, msg . getAccession (),
                     msg . getNumFrags (), gma, msg, string_list,
                     qual_list, clr_list, tag_list);

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
       }
   else if  (Input_Format == SIMPLE_CONTIG_INPUT
               || Input_Format == PARTIAL_READ_INPUT)
       {
        char  line [MAX_LINE];
        string  cid;
        vector <Ordered_Range_t>  pos_list, seg_list;
        vector <int>  frg_id_list;
        int  fid;

        input_fp = File_Open (Tig_File_Name . c_str (), "r");
        read_bank . open (Bank_Name);

        msg . setType (IUM_MSG);
        msg . setStatus (UNASSIGNED_UNITIG);

        contig_ct = 0;

        while  (fgets (line, MAX_LINE, input_fp) != NULL)
          {
           char  * p;

           p = strtok (line, " \t\n");
           if  (p == NULL)
               continue;

           if  (strcmp (p, "C") == 0)
               {
                if  (frg_id_list . size () > 0)
                    {
                     Get_Strings_And_Offsets (string_list, qual_list, clr_list,
                          tag_list, offset, frg_id_list, pos_list, seg_list,
                          read_bank);

                     msg . setAccession (cid);
                     msg . setIMPs (frg_id_list, pos_list);
                     Multi_Align (string_list, offset, ALIGN_WIGGLE, 0.04,
                          gma, & ref);
                     Permute (qual_list, ref);
                     Permute (clr_list, ref);
                     Permute (tag_list, ref);
                     Permute (frg_id_list, ref);

                     gma . Set_Flipped (clr_list);
                     gma . Get_Positions (pos);
                     gma . Extract_IMP_Dels (del_list);
                     msg . Update_IMPs (pos, ref, del_list);

                     gma . Set_Consensus_And_Qual (string_list, qual_list);
                     msg . setSequence (gma . getConsensusString ());
                     msg . setQuality (gma . getQualityString ());
                     msg . setUniLen (strlen (gma . getConsensusString ()));

                     Output_Unit (label, msg . getAccession (),
                          msg . getNumFrags (), gma, msg, string_list,
                          qual_list, clr_list, tag_list);

                     contig_ct ++;
                    }

                frg_id_list . clear ();
                pos_list . clear ();
                seg_list . clear ();

                p = strtok (NULL, " \t\n");
                cid = p;
               }
             else
               {
                Ordered_Range_t  ps;
                int  a, b;

                fid = strtol (p, NULL, 10);
                p = strtok (NULL, " \t\n");
                a = strtol (p, NULL, 10);
                p = strtok (NULL, " \t\n");
                b = strtol (p, NULL, 10);
                ps . setRange (a, b);
                frg_id_list . push_back (fid);
                pos_list . push_back (ps);
                if  (Input_Format == PARTIAL_READ_INPUT)
                    {
                     p = strtok (NULL, " \t\n");
                     a = strtol (p, NULL, 10);
                     p = strtok (NULL, " \t\n");
                     b = strtol (p, NULL, 10);
                     ps . setRange (a, b);
                     seg_list . push_back (ps);
                    }
               }
          }

        // Process the last contig here
        if  (frg_id_list . size () > 0)
            {
             Get_Strings_And_Offsets (string_list, qual_list, clr_list,
                  tag_list, offset, frg_id_list, pos_list, seg_list, read_bank);

             msg . setAccession (cid);
             msg . setIMPs (frg_id_list, pos_list);
             Multi_Align (string_list, offset, ALIGN_WIGGLE, 0.04, gma, & ref);
             Permute (qual_list, ref);
             Permute (clr_list, ref);
             Permute (tag_list, ref);
             Permute (frg_id_list, ref);

             gma . Set_Flipped (clr_list);
             gma . Get_Positions (pos);
             gma . Extract_IMP_Dels (del_list);
             msg . Update_IMPs (pos, ref, del_list);

             gma . Set_Consensus_And_Qual (string_list, qual_list);
             msg . setSequence (gma . getConsensusString ());
             msg . setQuality (gma . getQualityString ());
             msg . setUniLen (strlen (gma . getConsensusString ()));

             Output_Unit (label, msg . getAccession (),
                  msg . getNumFrags (), gma, msg, string_list,
                  qual_list, clr_list, tag_list);

             contig_ct ++;
            }

        cerr << "Processed " << contig_ct << " contigs" << endl;
       }

   fclose (input_fp);
   read_bank . close ();

   return  0;
  }



static void  Get_Strings_And_Offsets
    (vector <char *> & s, vector <char *> & q, vector <Range_t> & clr_list,
     vector <char * > & tag_list, vector <int> & offset,
     const Celera_Message_t & msg, Bank_t & read_bank)

//  Populate  s  and  offset  with reads and their unitig positions
//  for the unitig in  msg  with reads coming from  read_bank.
//  Put the clear-ranges for the strings in  clr_list  with
//  coordinates swapped if the string is reverse-complemented.
//   read_bank  must already be opened.  Put the corresponding quality-value
//  strings for the reads into  q .  Put the identifying tags for the reads
//  in  tag_list .

  {
   const vector <Celera_IMP_Sub_Msg_t> &  frgs = msg . getIMPList ();
   Read_t  read;
   Ordered_Range_t  position;
   int  prev_offset;
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
   offset . clear ();

   prev_offset = 0;
   n = msg . getNumFrags ();
   for  (i = 0;  i < n;  i ++)
     {
      char  * tmp, tag_buff [100];
      string  seq;
      string  qual;
      Range_t  clear;
      int  this_offset;
      int  a, b, j, len, qlen;

      read . setIID ( frgs [i] . getId () );
      position = frgs [i] . getPosition ();
      a = position . getBegin ();
      b = position . getEnd ();

      read_bank . fetch (read);
      sprintf (tag_buff, "%u", read . getIID ());
      tag_list . push_back (strdup (tag_buff));
      
      clear = read . getClearRange ();
      if  (Verbose > 2)
	cerr << read;
      seq = read . getSeqString (clear);
      qual = read . getQualString (clear);
      if  (b < a)
          {
           Reverse_Complement (seq);
           reverse (qual . begin (), qual . end ());
           clear . Swap ();
          }
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

      this_offset = Min (a, b);
      offset . push_back (this_offset - prev_offset);
      prev_offset = this_offset;
     }
   

   return;
  }



static void  Get_Strings_And_Offsets
    (vector <char *> & s, vector <char *> & q, vector <Range_t> & clr_list,
     vector <char *> & tag_list, vector <int> & offset, const vector <int> & fid,
     const vector <Ordered_Range_t> pos, const vector <Ordered_Range_t> seg,
     Bank_t & read_bank)

//  Populate  s  and  offset  with reads and their contig positions
//  for the contig with read-ids in  fid  and  consensus positions
//  in  pos .  Put the corresponding quality-value strings for the reads
//  into  q .  Put the clear-ranges for the strings in  clr_list  with
//  coordinates swapped if the string is reverse-complemented.
//  Put the identifying tags for the reads
//  in  tag_list .  Get reads and qualities from  read_bank.
//   read_bank  must already be opened.  If  seg  is not empty, used
//  the values in it to determine what segment of each read to use.

  {
   Read_t  read;
   int  prev_offset;
   bool  partial_reads;
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
   offset . clear ();
   partial_reads = (seg . size () > 0);

   prev_offset = 0;
   n = fid . size ();
   for  (i = 0;  i < n;  i ++)
     {
      char  * tmp, tag_buff [100];
      string  seq;
      string  qual;
      Range_t  clear;
      int  this_offset;
      int  a, b, j, len, qlen;

      a = pos [i] . getBegin ();
      b = pos [i] . getEnd ();

      read . setIID ( fid [i] );
      read_bank . fetch (read);
      sprintf (tag_buff, "%u", read . getIID ());
      tag_list . push_back (strdup (tag_buff));
      clear = read . getClearRange ();
      if  (Verbose > 2)
	cerr << read;
      seq = read . getSeqString (clear);
      qual = read . getQualString (clear);
      if  (partial_reads)
          {
           int  lo, hi;

           lo = seg [i] . getBegin ();
           hi = seg [i] . getEnd ();
           seq = seq . substr (lo, hi - lo);
           qual = qual . substr (lo, hi - lo);
          }
      if  (b < a)
          {
           Reverse_Complement (seq);
           reverse (qual . begin (), qual . end ());
           clear . Swap ();
          }

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
                    len, qlen, fid [i]);
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



static void  Output_Unit
    (const string & label, const string & id, int num_reads,
     Gapped_Multi_Alignment_t & gma, Celera_Message_t & msg,
     const vector <char *> & string_list,
     const vector <char *> & qual_list, const vector <Range_t> & clr_list,
     vector <char *> tag_list)

//  Output the consensus/multialignment unit in  msg  and  gma  with
//   id  and  num_reads  in the format specified in global  Output_Format .
//   label  is the separator in  MULTI_ALIGN_OUTPUT .   string_list  and
//   qual_list  hold the sequence and quality values of the component
//  reads.   clr_list  holds the clear ranges of the fragments from
//  which the reads came (in reverse order if the read has been
//  reverse-complemented).   tag_list  holds the IIDs of the reads.

  {
   switch  (Output_Format)
     {
      case  MULTI_ALIGN_OUTPUT :
        cout << endl << endl << label << " #" << msg . getAccession ()
             << "   " << num_reads << " reads" << endl;
        gma . Consensus_To_Lower ();
        gma . Print (stdout, string_list, 60, & tag_list);
        break;
      case  ONLY_FASTA_OUTPUT :
        cout << ">" << msg . getAccession () << endl;
        Fasta_Print_Skip (stdout, gma . getConsensusString (),
             "-", NULL);
        break;
      case  CELERA_MSG_OUTPUT :
        gma . Set_Consensus_And_Qual (string_list, qual_list);
        msg . setSequence (gma . getConsensusString ());
        msg . setQuality (gma . getQualityString ());
        msg . setUniLen (strlen (gma . getConsensusString ()));
        msg . print (stdout);
        break;
      case  TIGR_CONTIG_OUTPUT :
        gma . TA_Print (stdout, string_list, clr_list, 60, & tag_list,
             msg . getAccession ());
        break;
      default :
        sprintf
            (Clean_Exit_Msg_Line,
             "ERROR:  Bad output type = %d\n",
             int (Output_Format));
        Clean_Exit (Clean_Exit_Msg_Line, __FILE__, __LINE__);
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

   while  (! errflg && ((ch = getopt (argc, argv, "acCfhPSTu")) != EOF))
     switch  (ch)
       {
        case  'a' :
          Output_Format = MULTI_ALIGN_OUTPUT;
          break;

        case  'c' :
          Do_Contig_Messages = true;
          Do_Unitig_Messages = false;
          break;

        case  'C' :
          Input_Format = CELERA_MSG_INPUT;
          break;

        case  'f' :
          Output_Format = ONLY_FASTA_OUTPUT;
          break;

        case  'h' :
          errflg = true;
          break;

        case  'P' :
          Input_Format = PARTIAL_READ_INPUT;
          break;

        case  'S' :
          Input_Format = SIMPLE_CONTIG_INPUT;
          break;

        case  'T' :
          Output_Format = TIGR_CONTIG_OUTPUT;
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

   if  (Input_Format == CELERA_MSG_INPUT
          &&! Do_Unitig_Messages && ! Do_Contig_Messages)
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
           "Read unitigs/contigs from <tig-file> and create\n"
           "multialignments and/or consensus sequences for them.  Reads are\n"
           "obtained from <bank-name>\n"
           "\n"
           "Options:\n"
           "  -a    Output alignments instead of consensus messages\n"
           "  -c    Process contig messages\n"
           "  -C    Input is Celera msg format, i.e., a .cgb or .cgw file\n"
           "  -f    Output consensus only in FASTA format\n"
           "  -h    Print this usage message\n"
           "  -P    Input is simple contig format, i.e., UMD format\n"
           "          using partial reads\n"
           "  -S    Input is simple contig format, i.e., UMD format\n"
           "  -T    Output in TIGR Assembler contig format\n"
           "  -u    Process unitig messages\n"
           "\n",
           command);

   return;
  }



