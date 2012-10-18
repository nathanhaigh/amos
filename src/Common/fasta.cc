//  A. L. Delcher
//
//  File:  fasta.cc
//
//  Last Modified:  25 November 2002
//
//  Routines to manipulate FASTA format files


#include  "inttypes_AMOS.hh"
#include  "fasta.hh"
using namespace std;

static const char  Complement_Table []
    = "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"
      " nnnnnnnnn*nn-.nnnnnnnnnnnnnnnnn"
      "nTVGHNNCDNNMNKNNNNYSANBSNRNnnnn_"
      "ntvghnncdnnmnknnnnysanbsnrnnnnnn";


char  Complement
    (char ch)

// Returns the DNA complement of  ch

  {
   return  Complement_Table [(int)ch];
  }



void  Fasta_Print
    (FILE * fp, const char * s, const char * hdr, int fasta_width)

//  Print string  s  in fasta format to  fp .  Put string  hdr
//  on header line, unless it's  NULL  in which case do not print
//  a header line at all.  Print at most  fasta_width  characters per
//  line.

  {
   int  ct = 0;

   if  (hdr != NULL)
       fprintf (fp, ">%s\n", hdr);

   while  (* s != '\0')
     {
      if  (ct == fasta_width)
          {
           fputc ('\n', fp);
           ct = 0;
          }
      fputc (* s, fp);
      s ++;
      ct ++;
     }

   fputc ('\n', fp);

   return;
  }



void  Fasta_Print_N
    (FILE * fp, const char * s, int n, const char * hdr, int fasta_width)

//  Print first  n  bytes of  string  s  in fasta format to  fp .
//  Put string  hdr  on header line.  Print at most  fasta_width
//  characters per line.

  {
   int  ct = 0, i;

   if  (hdr != NULL)
       fprintf (fp, ">%s\n", hdr);

   for  (i = 0;  i < n;  i ++)
     {
      if  (ct == fasta_width)
          {
           fputc ('\n', fp);
           ct = 0;
          }
      fputc (s [i], fp);
      ct ++;
     }

   fputc ('\n', fp);

   return;
  }



void  Fasta_Print_Skip
    (FILE * fp, const char * s, const char * skip, const char * hdr,
     int fasta_width)

//  Print string  s  in fasta format to  fp  but omit any characters
//  that occur in string  skip .  Put string  hdr
//  on header line, unless it's  NULL  in which case do not print
//  a header line at all.  Print at most  fasta_width  characters per
//  line.

  {
   int  ct = 0;

   if  (hdr != NULL)
       fprintf (fp, ">%s\n", hdr);

   while  (* s != '\0')
     {
      if  (strchr (skip, * s) == NULL)
          {
           if  (ct == fasta_width)
               {
                fputc ('\n', fp);
                ct = 0;
               }
           fputc (* s, fp);
           ct ++;
          }
      s ++;
     }

   fputc ('\n', fp);

   return;
  }



bool  Fasta_Qual_Read
    (FILE * fp, string & q, string & hdr)

//  Read next fasta-like-format quality value sequence from
//  file  fp  (which must already be open) into string  q 
//  (encoded by adding the quality value to the  MIN_QUALITY  value).
//  Put the faster header line (without the '>' and trailing spaces) into
//  string  hdr .  Return  true  if a string is successfully,
//  read; false, otherwise.

  {
   bool  have_value;
   int  ch, val;

   q . erase ();
   hdr . erase ();

   // skip till next '>' if necessary
   while  ((ch = fgetc (fp)) != EOF && ch != '>')
     ;

   if  (ch == EOF)
       return  false;

   // skip spaces if any
   while  ((ch = fgetc (fp)) != EOF && ch == ' ')
     ;
   if  (ch == EOF)
       return  false;
   ungetc (ch, fp);

   // put rest of line into  hdr
   while  ((ch = fgetc (fp)) != EOF && ch != '\n')
     hdr . push_back (char (ch));

   // put all numbers up till next '>' into  q
   have_value = false;
   val = 0;
   while  ((ch = fgetc (fp)) != EOF && ch != '>')
     {
      if  (isspace (ch))
          {
           if  (have_value)
               q . push_back (char (val + AMOS::MIN_QUALITY));
           have_value = false;
           val = 0;
          }
      else if  (isdigit (ch))
          {
           have_value = true;
           val = 10 * val + ch - '0';
          }
     }

   if  (ch == '>')
       ungetc (ch, fp);

   return  true;
  }



bool  Fasta_Read
    (FILE * fp, string & s, string & hdr)

//  Read next fasta-format string from file  fp  (which must
//  already be open) into string  s .  Put the faster
//  header line (without the '>' and trailing spaces) into
//  string  hdr .  Return  true  if a string is successfully,
//  read; false, otherwise.

  {
   int  ch;

   s . erase ();
   hdr . erase ();

   // skip till next '>' if necessary
   while  ((ch = fgetc (fp)) != EOF && ch != '>')
     ;

   if  (ch == EOF)
       return  false;

   // skip spaces if any
   while  ((ch = fgetc (fp)) != EOF && ch == ' ')
     ;
   if  (ch == EOF)
       return  false;
   ungetc (ch, fp);

   // put rest of line into  hdr
   while  ((ch = fgetc (fp)) != EOF && ch != '\n')
     hdr . push_back (char (ch));

   // put everything up till next '>' into  s
   while  ((ch = fgetc (fp)) != EOF && ch != '>')
     {
      if  (! isspace (ch))
          s . push_back (char (ch));
     }

   if  (ch == '>')
       ungetc (ch, fp);

   return  true;
  }



void  Reverse_Complement
  (char * s)

//  Set string  s  to its DNA Watson-Crick reverse complement

  {
   int  i, j, n;

   n = strlen (s);
   for  (i = 0, j = n - 1;  i < j;  i ++, j --)
     {
      char  ch;

      ch = s [j];
      s [j] = Complement (s [i]);
      s [i] = Complement (ch);
     }

   if  (i == j)
       s [i] = Complement (s [i]);

   return;
  }



void  Reverse_Complement
  (string & s)

//  Set string  s  to its DNA Watson-Crick reverse complement

  {
   int  i, j, n;

   n = s . length ();
   for  (i = 0, j = n - 1;  i < j;  i ++, j --)
     {
      char  ch;

      ch = s [j];
      s [j] = Complement (s [i]);
      s [i] = Complement (ch);
     }

   if  (i == j)
       s [i] = Complement (s [i]);

   return;
  }



void  Reverse_String
    (char * s)

//  Reverse the order of characters in string  s .

  {
   int  i, j, n;

   n = strlen (s);
   for  (i = 0, j = n - 1;  i < j;  i ++, j --)
     {
      char  ch;

      ch = s [i];
      s [i] = s [j];
      s [j] = ch;
     }

   return;
  }



void  Reverse_String
    (string & s)

//  Reverse the order of characters in string  s .

  {
   int  i, j, n;

   n = s . length ();
   for  (i = 0, j = n - 1;  i < j;  i ++, j --)
     {
      char  ch;

      ch = s [i];
      s [i] = s [j];
      s [j] = ch;
     }

   return;
  }



int Fasta_Read_String  (FILE * fp, char * & T, long int & Size, char Name [],
		  int Partial)

/* Read next string from  fp  (assuming FASTA format) into  T [1 ..]
*  which has  Size  characters.  Allocate extra memory if needed
*  and adjust  Size  accordingly.  Return  TRUE  if successful,  FALSE
*  otherwise (e.g., EOF).  Partial indicates if first line has
*  numbers indicating a subrange of characters to read.
*/

  {
    const int max_line = 1024;
    const int fasta_incr = 10000;

   char  * P, Line [max_line];
   long int  Len, Lo, Hi;
   int  Ch, Ct;

   while  ((Ch = fgetc (fp)) != EOF && Ch != '>')
     ;

   if  (Ch == EOF)
       return  FALSE;

   fgets (Line, max_line, fp);
   Len = strlen (Line);
   assert (Len > 0 && Line [Len - 1] == '\n');
   P = strtok (Line, " \t\n");
   if  (P != NULL)
       strcpy (Name, P);
     else
       Name [0] = '\0';
   Lo = 0;  Hi = LONG_MAX;
   if  (Partial)
       {
        P = strtok (NULL, " \t\n");
        if  (P != NULL)
            {
             Lo = strtol (P, NULL, 10);
             P = strtok (NULL, " \t\n");
             if  (P != NULL)
                 Hi = strtol (P, NULL, 10);
            }
        assert (Lo <= Hi);
       }

   Ct = 0;
   T [0] = '\0';
   Len = 1;
   while  ((Ch = fgetc (fp)) != EOF && Ch != '>')
     {
      if  (isspace (Ch))
          continue;

      Ct ++;
      if  (Ct < Lo || Ct > Hi)
          continue;

      if  (Len >= Size)
          {
           Size += fasta_incr;
           T = (char *) Safe_realloc (T, Size);
          }
      Ch = tolower (Ch);

      if  (! isalpha (Ch) && Ch != '*')
          {
           fprintf (stderr, "Unexpected character `%c\' in string %s\n",
                                 Ch, Name);
           Ch = 'x';
          }

      T [Len ++] = Ch;
     }

   T [Len] = '\0';
   if  (Ch == '>')
       ungetc (Ch, fp);

   return  TRUE;
  }
