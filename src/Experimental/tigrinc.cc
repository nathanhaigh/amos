#include "tigrinc.hh"


FILE *  File_Open  (const char * Filename, const char * Mode)

/* Open  Filename  in  Mode  and return a pointer to its control
*  block.  If fail, print a message and exit. */

  {
   FILE  *  fp;

   fp = fopen (Filename, Mode);
   if  (fp == NULL)
       {
        fprintf (stderr, "ERROR:  Could not open file  %s \n", Filename);
        exit (EXIT_FAILURE);
       }

   return  fp;
  }



#if 0
void *  Safe_calloc  (size_t N, size_t Len)

/* Allocate and return a pointer to an array of  N  elements of
*   Len  bytes each.  All set to 0.  Exit if fail. */

  {
   void  * P;

   P = calloc (N, Len);
   if  (P == NULL)
       {
        fprintf (stderr,
		 "ERROR:  calloc failed, there is not enough memory\n");
        exit (EXIT_FAILURE);
       }

   return  P;
  }
#endif



void *  Safe_malloc  (size_t Len)

/* Allocate and return a pointer to  Len  bytes of memory.
*  Exit if fail. */

  {
   void  * P;

   P = malloc (Len);
   if  (P == NULL)
       {
        fprintf (stderr,
		 "ERROR:  malloc failed, there is not enough memory\n");
        exit (EXIT_FAILURE);
       }

   return  P;
  }



void *  Safe_realloc  (void * Q, size_t Len)

/* Reallocate memory for  Q  to  Len  bytes and return a
*  pointer to the new memory.  Exit if fail. */

  {
   void  * P;

   P = realloc (Q, Len);
   if  (P == NULL)
       {
        fprintf (stderr,
		 "ERROR:  realloc failed, there is not enough memory\n");
        exit (EXIT_FAILURE);
       }

   return  P;
  }


#if 0

char  Complement  (char Ch)

/* Returns the DNA complement of  Ch . */

  {
   switch  (tolower (Ch))
     {
      case  'a' :
        return  't';
      case  'c' :
        return  'g';
      case  'g' :
        return  'c';
      case  't' :
        return  'a';
      case  'r' :          // a or g
        return  'y';
      case  'y' :          // c or t
        return  'r';
      case  's' :          // c or g
        return  's';
      case  'w' :          // a or t
        return  'w';
      case  'm' :          // a or c
        return  'k';
      case  'k' :          // g or t
        return  'm';
      case  'b' :          // c, g or t
        return  'v';
      case  'd' :          // a, g or t
        return  'h';
      case  'h' :          // a, c or t
        return  'd';
      case  'v' :          // a, c or g
        return  'b';
      default :            // anything
        return  tolower (Ch);
     }
  }

#endif


int Read_String  (FILE * fp, char * & T, long int & Size, char Name [],
		  int Partial)

/* Read next string from  fp  (assuming FASTA format) into  T [1 ..]
*  which has  Size  characters.  Allocate extra memory if needed
*  and adjust  Size  accordingly.  Return  TRUE  if successful,  FALSE
*  otherwise (e.g., EOF).  Partial indicates if first line has
*  numbers indicating a subrange of characters to read.
*/

  {
   char  * P, Line [MAX_LINE];
   long int  Len, Lo, Hi;
   int  Ch, Ct;

   while  ((Ch = fgetc (fp)) != EOF && Ch != '>')
     ;

   if  (Ch == EOF)
       return  FALSE;

   fgets (Line, MAX_LINE, fp);
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
           Size += INCR_SIZE;
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


#if 0

void  Reverse_Complement
    (char S [], long int Lo, long int Hi)

//  Convert substring  S [Lo .. Hi]  to its Watson-Crick reverse
//  complement

  {
   char  Ch;

   while  (Lo <= Hi)
     {
      Ch = S [Hi];
      S [Hi] = Complement (S [Lo]);
      S [Lo] = Complement (Ch);
      Lo ++;
      Hi --;
     }

   return;
  }


#endif
