//  A. L. Delcher
//
//  File:  show-ma-asm.cc
//
//  Last Modified:  20 May 2003
//
//  Read Celera-format .asm and .frg files and display the
//  multialignment for a specified contig


#include  "delcher.hh"
#include  "fasta.hh"
#include  <vector>
#include  <string>
#include  <algorithm>

using namespace std;


static const int  MAX_LINE = 1000;
  // maximum length input line


struct  Read_Info_t
  {
   int  id;
   int  start, end;
   vector <int>  del;
  };


static int  Contig_ID;
static char  * ASM_File_Name;
static char  * FRG_File_Name;


static void  Parse_Command_Line
    (int argc, char * argv []);
static void  Usage
    (const char * command);



int  main
    (int argc, char * argv [])

  {
   FILE  * fp;
   char  line [MAX_LINE];
   vector <Read_Info_t>  read_list;
   Read_Info_t  read;
   char  * consensus;
   bool  in_cco = false, in_consensus = false, in_mps = false, in_del = false;
   bool  found_it = false;
                     int  ct, dln, level;

   cerr << "Starting on " << __DATE__ << " at " << __TIME__ << endl;

   Verbose = 1;

   Parse_Command_Line (argc, argv);

   printf ("Multialignment for contig %d\n", Contig_ID);
   printf ("Using .asm file %s and .frg file %s\n",
        ASM_File_Name, FRG_File_Name);

   // Find and extract the CCO message for the specified contig
   fp = File_Open (ASM_File_Name, "r");
   level = 0;
   while  (fgets (line, MAX_LINE, fp) != NULL)
     {
      if  (line [0] == '{')
          level ++;
      else if  (line [0] == '}')
          level --;
      if  (strncmp (line, "{CCO", 4) == 0)
          in_cco = true;
      if  (in_cco && level == 0)
          in_cco = false;
      if  (in_cco && strncmp (line, "acc:", 4) == 0)
          {
           int  cid;

           cid = strtol (line + 5, NULL, 10);
           if  (cid == Contig_ID)
               found_it = true;
          }
      if  (found_it && level == 0)
          break;
      if  (found_it && strncmp (line, "len:", 4) == 0)
          {
           int  len;

           len = strtol (line + 4, NULL, 10);
cerr << "len = " << len << endl;
           consensus = (char *) Safe_malloc (len + 1);
           consensus [0] = '\n';
           ct = 0;     // length of consensus
          }
      if  (found_it && strncmp (line, "cns:", 4) == 0)
          in_consensus = true;
      else if  (in_consensus)
          {
           char  * s;

           s = strtok (line, " \t\n");
           if  (strcmp (s, ".") == 0)
               in_consensus = false;
             else
               {
                strcpy (consensus + ct, s);
                ct += strlen (s);
               }
          }
      if  (found_it && strncmp (line, "{MPS", 4) == 0)
          in_mps = true;
      else if  (in_mps && line [0] == '}')
          {
           in_mps = in_del = false;
           read_list . push_back (read);
cerr << "push read  list len = " << read_list . size () << "  level = " <<
     level << endl;
           read . del . clear ();
          }
      if  (in_mps && strncmp (line, "mid:", 4) == 0)
          read . id = strtol (line + 4, NULL, 10);
      if  (in_mps && strncmp (line, "pos:", 4) == 0)
          sscanf ("%d,%d", line + 4, & read . start, & read . end);
      if  (in_mps && strncmp (line, "dln:", 4) == 0)
          dln = strtol (line + 4, NULL, 10);
      if  (in_mps && strncmp (line, "del:", 4) == 0)
          in_del = true;
      else if  (in_del)
          {
           char  * p;

           for  (p = strtok (line, " \t\n");  p != NULL;  p = strtok (NULL, " \t\n"))
             read . del . push_back (strtol (p, NULL, 10));
          }
     }

   fclose (fp);

   fprintf (stderr, "Num read = %d\n", int (read_list . size ()));
   Fasta_Print (stderr, consensus, NULL);
   

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

   while  (! errflg && ((ch = getopt (argc, argv, "h")) != EOF))
     switch  (ch)
       {
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

   Contig_ID = strtol (argv [optind ++], NULL, 10);
   ASM_File_Name = argv [optind ++];
   FRG_File_Name = argv [optind ++];

   return;
  }



static void  Usage
    (const char * command)

//  Print to stderr description of options and command line for
//  this program.   command  is the command that was used to
//  invoke it.

  {
   fprintf (stderr,
           "USAGE:  %s  <contig-id> <asm-file> <frg-file>\n"
           "\n"
           "Read Celera-format .asm file and .frg file and\n"
           "print the multialignment for the specified contig\n"
           "\n"
           "Options:\n"
           "  -h    Print this usage message\n"
           "\n",
           command);

   return;
  }



