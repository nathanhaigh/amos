//  Daniel Sommer
//
//  - FasqOffset[qualType] File:  fastq.cc
//
//  Last Modified:  April 2011
//
//  Routines to manipulate FASTQ format files

#include  "exceptions_AMOS.hh"
#include  "inttypes_AMOS.hh"
#include  "fastq.hh"
using namespace std;

static const char FastqOffset[] = {'@', '!'};

bool  Fastq_Read(FILE * fp, string & s, string & hdr, string & q, string & qualHdr, FastqQualType qualType)
//  Read next fastq-format string from file  fp  (which must
//  already be open) into string  s .  Put the fasta header line into
//  string  hdr .  Return  true  if a string is successfully,
//  read; false, otherwise.

  {
   int  ch, val;

   s.erase();
   hdr.erase();
   q.erase();
   qualHdr.erase();

   // skip till next '@' if necessary
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
   while(( ch = fgetc(fp) ) != EOF && ch != '\n')
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

   // put all numbers up till newline into  q
   while((ch = fgetc(fp)) != EOF && ch != '\n')
     {
       // check of errors
       int qlVal = (int)ch;
       qlVal -= (int) FastqOffset[qualType];
       int maxQlt = (int)AMOS::MAX_QUALITY - (int)AMOS::MIN_QUALITY;
       if (qlVal < 0) {
          sprintf (Clean_Exit_Msg_Line,
                     "Failed: invalid quality value too low %c %d. Please check quality type\n", ch, qlVal);
          throw AMOS::Exception_t(Clean_Exit_Msg_Line, __LINE__, __FILE__);
       } else if (qlVal > maxQlt) {
          ch = FastqOffset[qualType] + AMOS::MAX_QUALITY;
       }
       q.push_back(char (ch - FastqOffset[qualType] + AMOS::MIN_QUALITY));
     }


   return  true;
  }





