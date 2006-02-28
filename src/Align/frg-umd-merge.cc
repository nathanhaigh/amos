//  A. L. Delcher
//
//  File:  frg-umd-merge.cc
//
//  Last Modified:  17 July 2003
//
//  Read a Celera-format .frg file and corresponding sequence and
//  quality files from the UMd overlap and output a Celera-format
//  .frg file with the sequence and quality set to the UMd values.


#include  "CelMsg.hh"
#include  "delcher.hh"
#include  "fasta.hh"
#include  <vector>
#include  <string>
#include  <algorithm>


using namespace std;
using namespace AMOS;


const int  MAX_LINE = 1000;
const int  NEW_SIZE = 1000;


static char  * Frg_File_Name;
  // Name of input .frg file
static char  * Qual_File_Name;
  // Name of input UMd quality file
static char  * Seq_File_Name;
  // Name of input UMd sequence file
static int  Seqname_Match = 0;
  // Determines which field on fasta header line must
  // match seqname in src field of frg message


static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   Celera_Message_t  msg;
   FILE  * frg_fp, * seq_fp, * qual_fp;
   string  new_seq, new_qual;
   string  seq_hdr, qual_hdr;
   string  frg_comment;
   char  seq_id [MAX_LINE], qual_id [MAX_LINE];
   char  * p, * p1, * q1, * p2, * q2;
   time_t  now;

   try
     {
      now = time (NULL);
      cerr << "Starting on " << ctime (& now) << endl;

      Verbose = 1;

      Parse_Command_Line (argc, argv);

      frg_fp = File_Open (Frg_File_Name, "r");
      seq_fp = File_Open (Seq_File_Name, "r");
      qual_fp = File_Open (Qual_File_Name, "r");

      while  (msg . read (frg_fp))
        {
         if  (msg . getMsgType () != FRG_MSG)
             msg . print (stdout);
           else
             {
              int  s_len, q_len;
              int  i;
              bool  success;

              frg_comment = msg . getSource ();

              success = Fasta_Read (seq_fp, new_seq, seq_hdr);
              if  (! success)
                  {
                   sprintf (Clean_Exit_Msg_Line,
                        "Could not read UMd sequence for frg %s",
                        msg . getAccession () . c_str ());
                   throw IOException_t (Clean_Exit_Msg_Line);
                  }
              Fasta_Qual_Read (qual_fp, new_qual, qual_hdr);
              if  (! success)
                  {
                   sprintf (Clean_Exit_Msg_Line,
                        "Could not read UMd quality for frg %s",
                        msg . getAccession () . c_str ());
                   throw IOException_t (Clean_Exit_Msg_Line);
                  }
              strcpy (seq_id, seq_hdr . c_str ());
              strcpy (qual_id, qual_hdr . c_str ());
              p1 = strtok (seq_id, " \t\n");
              p2 = strtok (NULL, " \t\n");
              q1 = strtok (qual_id, " \t\n");
              q2 = strtok (NULL, " \t\n");
              switch (Seqname_Match)
                {
                 case  1 :
                   p = p1;
                   break;
                 case  2 :
                   p = p2;
                   break;
                 default :
                   p = "XXX";
                }
              if  (strcmp (p1, q1) != 0
                     || (Seqname_Match > 0 && int (frg_comment . find (p)) < 0))
                  {
                   sprintf (Clean_Exit_Msg_Line,
                        "Tag mismatch:  UMseq = %s  UMqual = %s  p = %s  frg = %s",
                        p1, q1, p, frg_comment . c_str ());
                   throw IOException_t (Clean_Exit_Msg_Line);
                  }

              s_len = new_seq . length ();
              q_len = new_qual . length ();
              if  (s_len != q_len)
                  {
                   sprintf (Clean_Exit_Msg_Line,
                        "Seq len = %d != Qual len = %d for %s",
                        s_len, q_len, p);
                   throw IOException_t (Clean_Exit_Msg_Line);
                  }
              for  (i = 0;  i < q_len;  i ++)
                new_qual [i] = Min (new_qual [i], MAX_CELERA_QUALITY_CHAR);
              msg . setSequence (new_seq);
              msg . setQuality (new_qual);
              msg . setClearRange (0, s_len);

              msg . print (stdout);
             }
        }
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



static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false, helpflg = false;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "012h")) != EOF))
     switch  (ch)
       {
        case  '0' :
          Seqname_Match = 0;
          break;
        case  '1' :
          Seqname_Match = 1;
          break;
        case  '2' :
          Seqname_Match = 2;
          break;
        case  'h' :
          helpflg = true;
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

   if  (helpflg)
       {
        Usage (argv [0]);
        exit (EXIT_SUCCESS);
       }

   Frg_File_Name = argv [optind ++];
   Seq_File_Name = argv [optind ++];
   Qual_File_Name = argv [optind ++];

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  [options] <frg-file> <umd-seq-file> <umd-qual-file>\n"
           "\n"
           "Read <frg-file> and replace sequences and qualities with\n"
           "corresponding entries in <umd-seq-file> and <umd-qual-file>\n"
           "Output revised .frg-file to  stdout \n"
           "\n"
           "Options:\n"
           "  -0\n"
           "  -1\n"
           "  -2\n"
           "     Determines which field on fasta header line must\n"
           "     match the seqname in the src field of the frg file\n"
           "     -0  no match required (default)\n"
           "     -1  first field must match\n"
           "     -2  second field must match\n"
           "  -h\n"
           "     Print this usage message\n"
           "\n",
           command);

   return;
  }



