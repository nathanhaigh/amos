//  A. L. Delcher
//
//  File:  fasta.cc
//
//  Last Modified:  25 November 2002
//
//  Routines to manipulate FASTA format files


#include  "delcher.h"
#include  "fasta.h"


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



