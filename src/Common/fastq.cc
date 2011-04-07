//  Daniel Sommer
//
//  File:  fastq.cc
//
//  Last Modified:  April 2011
//
//  Routines to manipulate FASTQ format files


#include  "inttypes_AMOS.hh"
#include  "fastq.hh"
using namespace std;

bool  Fastq_Read(FILE * fp, string & s, string & hdr, string & q, string & qualHdr)
//  Read next fastq-format string from file  fp  (which must
//  already be open) into string  s .  Put the faster
//  header line into
//  string  hdr .  Return  true  if a string is successfully,
//  read; false, otherwise.

  {
   bool  have_value;
   int  ch, val;

   s.erase();
   hdr.erase();
   q.erase();
   qualHdr.erase();

   // skip till next '>' if necessary
   while  ((ch = fgetc (fp)) != EOF && ch != '@')
     ;

   if  (ch == EOF)
       return  false;

   // skip spaces if any
   while  ((ch = fgetc (fp)) != EOF && ch == ' ')
     ;
   if  (ch == EOF)
       return  false;
   ungetc(ch, fp);

   // put rest of line into  hdr
   while  ((ch = fgetc (fp)) != EOF && ch != '\n')
     hdr.push_back (char (ch));

   // put everything up till '+' into  s
   while(( ch = fgetc(fp) ) != EOF && ch != '+')
     {
      if  (! isspace(ch))
          s.push_back(char (ch));
     }

   if  (ch == EOF)
       return  false;

   // skip spaces if any
   while  ((ch = fgetc (fp)) != EOF && ch == ' ')
     ;

   ungetc (ch, fp);

   // put rest of line into  hdr
   while  ((ch = fgetc (fp)) != EOF && ch != '\n')
     qualHdr.push_back (char (ch));

   // put all numbers up till next '@' into  q
   have_value = false;
   val = 0;
   while((ch = fgetc(fp)) != EOF && ch != '@')
     {
      if(isspace (ch))
          {
           if(have_value)
               q.push_back(char (val + AMOS::MIN_QUALITY));
           have_value = false;
           val = 0;
          }
      else if  (isdigit (ch))
          {
           have_value = true;
           val = 10 * val + ch - '0';
          }
     }

   if  (ch == '@')
       ungetc (ch, fp);

   return  true;
  }





