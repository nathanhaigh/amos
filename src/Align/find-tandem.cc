#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <sys/time.h>
using namespace std;

const int OFFSET_TABLE_SIZE = 100;

string FASTA_FILE;

int MIN_REPORT_LEN = 8;
int MIN_REPORT_UNITS = 3;
int MIN_UNIT_LEN = 1;
int MAX_UNIT_LEN = 4;
int FLANK = 10;

// Record length of event in seconds
class EventTime_t
{
public:

  EventTime_t()
  {
    start();
    memset(&m_end, 0, sizeof(struct timeval));
  }

  void start()
  {
    gettimeofday(&m_start, NULL);
  }

  void end()
  {
    gettimeofday(&m_end, NULL);
  }

  double length()
  {
    if ((m_end.tv_sec == 0) && (m_end.tv_usec == 0)) { end(); }
    return ((m_end.tv_sec - m_start.tv_sec)*1000000.0 + (m_end.tv_usec - m_start.tv_usec)) / 1e6;
  }

  string str(bool format, int precision)
  {
    double r = length();

    char buffer[1024];
    sprintf(buffer, "%0.*f", precision, r);

    if (format)
    {
      string s("[");
      s += buffer;
      s += "s]";
      return s;
    }

    return buffer;
  }


private:
  struct timeval m_start;
  struct timeval m_end;
};



bool  Fasta_Read (FILE * fp, string & s, string & hdr)

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


void findTandems(const string & seq, const string & tag)
{
  cout << ">" << tag << " len=" << seq.length() << endl;

  int offsets[OFFSET_TABLE_SIZE][OFFSET_TABLE_SIZE];

  // initialize the offsets
  for (int merlen = 1; merlen <= MAX_UNIT_LEN; merlen++)
  {
    for (int phase = 0; phase < merlen; phase++)
    {
      offsets[merlen][phase] = phase;
    }
  }

  // now scan the sequence, considering mers starting at position i
  for (int i = 0; i < seq.length(); i++)
  {
    
    // consider all possible merlens from 1 to max
    for (int merlen = 1; merlen <= MAX_UNIT_LEN; merlen++)
    {
      int phase = i % merlen;
      int offset = offsets[merlen][phase];

      // compare [i..i+merlen) to [offset..offset+merlen)
      int j = 0;
      while ((j < merlen) && 
             (i+j < seq.length()) && 
             (seq[i+j] == seq[offset+j])) 
      { j++; }

      // is the end of the tandem?
      if (j != merlen || (i+j+1 == seq.length()))
      {
        // am i the leftmost version of this tandem?
        if (seq[offset-1] != seq[offset+merlen-1])
        {
          // is it long enough to report?
          if (((i-offset)/merlen >= MIN_REPORT_UNITS) && (i - offset >= MIN_REPORT_LEN))
          {
            // is it primitive?
            int ml = 1;

            while (ml < merlen)
            {
              int units = (i-offset+j) / ml;

              int allmatch = 1;
              for (int index = 1; allmatch && (index < units); index++)
              {
                // compare the bases of the current unit to those of unit0
                for (int m = 0; m < ml; m++)
                {
                  if (seq[offset+m] != seq[offset+index*ml+m])
                  {
                    allmatch = 0;
                    break;
                  }
                }
              }

              if (!allmatch) { ml++; }
              else           { break; }
            }

            // everything checks, now report it

            if (ml == merlen)
            {
              // start end length
              cout << offset+1 << "\t" << i+j << "\t" << i+j-offset << "\t";

              // tandem seq
              for (int z = 0; z < merlen; z++) { cout << seq[offset+z]; }

              // complete units + remainder
              cout << "\t" << (i - offset) / merlen << "+" << j << "\t";

              // left flank - tandem - right flank
              for (int z = offset-FLANK; z < offset;    z++) { if (z >= 0) { cout << (char) tolower(seq[z]); } }
              for (int z = offset;       z < i+j;       z++) { cout << seq[z]; }
              for (int z = i+j;          z < i+j+FLANK; z++) { if (z < seq.length()) { cout << (char) tolower(seq[z]); } }

              cout << "\t" << phase;

              cout << endl;
            }
          }
        }

        offsets[merlen][phase] = i;
      }
    }
  }
}


int main (int argc, char * argv [])
{

stringstream helptext;
helptext <<
"Find tandem repeats\n"
"\n"
" Usage: find-tandems -f fasta\n"
"\n"
"   -f <fasta> multifasta file to scan\n"
"   -u <units> minimum number of units to report (default: " << MIN_REPORT_UNITS << ")\n"
"   -l <bp>    minimum length of tandem in bp (default: " << MIN_REPORT_LEN << ")\n"
"   -x <len>   max unit length (default: " << MAX_UNIT_LEN << ")\n"
"   -m <len>   min unit length (default: " << MIN_UNIT_LEN << ")\n"
"   -k <bp>    flanking bp to report (default: " << FLANK << ")\n"
"\n"
" Output format:\n"
">fasta header\n"
"start end total_len tandem_unit complete_units+partial left_flank+repeat+right_flank\n"
"\n";

  bool errflg = false;
  int ch;

  optarg = NULL;

  while (!errflg && ((ch = getopt (argc, argv, "f:u:l:x:m:k:h")) != EOF))
  {
    switch (ch)
    {

      case 'f': FASTA_FILE = optarg;             break; 
      case 'u': MIN_REPORT_UNITS = atoi(optarg); break;
      case 'l': MIN_REPORT_LEN   = atoi(optarg); break;
      case 'x': MAX_UNIT_LEN     = atoi(optarg); break;
      case 'm': MIN_UNIT_LEN     = atoi(optarg); break;
      case 'k': FLANK            = atoi(optarg); break;

      case '?':
        fprintf (stderr, "Unrecognized option -%c\n", optopt);

      case 'h': 

      default:
        errflg = true;
    }

    if (errflg)
    {
      cout << helptext.str();
      exit (EXIT_FAILURE);
    }
  }

  if (FASTA_FILE.empty())
  {
    cerr << "You must specify a fasta file" << endl;
    return 1;
  }

  if (MAX_UNIT_LEN >= OFFSET_TABLE_SIZE)
  {
    cerr << "Max unit size must be less than " << OFFSET_TABLE_SIZE << endl;
    return 1;
  }

  cerr << "Processing sequences in " << FASTA_FILE << "..." << endl;

  FILE * fp = fopen(FASTA_FILE.c_str(), "r");

  if (!fp)
  {
    cerr << "Couldn't open " << FASTA_FILE << endl;
    exit(1);
  }

  EventTime_t timer;

  string s, tag;

  while  (Fasta_Read (fp, s, tag))
  {
    findTandems(s, tag);
  }

  cerr << "finished in " << timer.length() << "s" << endl;

  return 0;
}



