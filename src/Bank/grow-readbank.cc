//  A. L. Delcher
//
//  File:  grow-readbank.cc
//
//  Last Modified:  20 February 2003
//
//  Add reads to a readbank, creating a new one if specfied.


#include  "delcher.hh"
#include  "WGA_databank.hh"
#include  "WGA_datatypes.hh"
#include  "CelMsgWGA.hh"
#include  <vector>
#include  <string>

using namespace std;


const int  NEW_SIZE = 1000;


static string  Bank_Name;
  // Name of read bank to be added to.
static bool  Create_New_Bank = false;
  // If true, then start a new read bank
static bool  Force_New_Bank = false;
  // If true, then force a new read bank by deleting existing one first



static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   ReadBank_t  read_bank;
   ID_t  iid;
   Read_t  read;
   Celera_Message_t  msg;
   int  ct;

   fprintf (stderr, "Starting on  %s  at  %s\n", __DATE__, __TIME__);

   Verbose = 1;

   Parse_Command_Line (argc, argv);

   cout << "Bank is " << Bank_Name << endl;

   if  (Force_New_Bank)
       {
        read_bank . openStore (Bank_Name);
        read_bank . closeStore ();
        read_bank . destroyStore ();
       }

   if  (Create_New_Bank)
       read_bank . createStore (Bank_Name, "silly comment");

   ct = 0;
   while  (msg . read (stdin))
     if  (msg . getMsgType () == FRG_MSG)
         {
          string  s;
          
          ct ++;
          read . setClearRange (msg . getClearRange ());
          read . setComment (msg . getSource ());
          read . setEID (msg . getAccession ());
          read . setSequence (msg . getSequence (), msg . getQuality ());
          iid = read_bank . append (read);
         }
   printf ("%d FRG messages\n", ct);

   read_bank . commitStore ();
   read_bank . closeStore ();

#if  0    // test random access reads from store
{
 int  i, j;

   read_bank . openStore (Bank_Name);
   for  (i = 0;  i < 10;  i ++)
     {
      j = 1 + lrand48 () % ct;
      read = read_bank . fetch (j);
      read . Print (cout);
     }
   read_bank . closeStore ();
}
#endif   

   return  0;
  }



static void  Parse_Command_Line
    (int argc, char * argv [])

//  Get options and parameters from command line with  argc
//  arguments in  argv [0 .. (argc - 1)] .

  {
   bool  errflg = false;
   int  ch;

   optarg = NULL;

   while  (! errflg && ((ch = getopt (argc, argv, "cfh")) != EOF))
     switch  (ch)
       {
        case  'c' :
          Create_New_Bank = true;
          break;

        case  'f' :
          Force_New_Bank = true;
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

   if  (optind > argc - 1)
       {
        fprintf (stderr, "ERROR:  bank name not specified\n");
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
           "Add reads (in Celera frg-file format) from stdin to the readbank\n"
           "<bank-name>\n"
           "\n"
           "Options:\n"
           "  -c    Create a new read bank\n"
           "  -f    Force new read bank by deleting existing one first\n"
           "  -h    Print this usage message\n"
           "\n",
           command);

   return;
  }
