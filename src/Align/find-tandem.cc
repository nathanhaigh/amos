#include "foundation_AMOS.hh"
#include  "delcher.hh"
#include  "fasta.hh"
#include  "amp.hh"
#include "AMOS_Foundation.hh"

#include  <string>
#include  <vector>
using namespace std;
using namespace HASHMAP;
using namespace AMOS;

const int OFFSET_TABLE_SIZE = 100;

int MIN_REPORT_LEN = 8;
int MIN_REPORT_UNITS = 3;
int MIN_UNIT_LEN = 1;
int MAX_UNIT_LEN = 4;
int FLANK = 10;

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
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version = "Version 1.0";
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

    string fastafile;

    tf = new AMOS_Foundation(version, helptext.str(), "", argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("f=s", &fastafile);
    tf->getOptions()->addOptionResult("u=i", &MIN_REPORT_UNITS);
    tf->getOptions()->addOptionResult("l=i", &MIN_REPORT_LEN);
    tf->getOptions()->addOptionResult("x=i", &MAX_UNIT_LEN);
    tf->getOptions()->addOptionResult("m=i", &MIN_UNIT_LEN);
    tf->getOptions()->addOptionResult("k=i", &FLANK);

    tf->handleStandardOptions();

    if (fastafile.empty())
    {
      cerr << "You must specify a fasta file" << endl;
      return 1;
    }

    if (MAX_UNIT_LEN >= OFFSET_TABLE_SIZE)
    {
      cerr << "Max unit size must be less than " << OFFSET_TABLE_SIZE << endl;
      return 1;
    }

    cerr << "Processing sequences in " << fastafile << "..." << endl;

    FILE * fp = fopen(fastafile.c_str(), "r");

    if (!fp)
    {
      cerr << "Couldn't open " << fastafile << endl;
      exit(1);
    }

    EventTime_t timer;

    string s, tag;

    while  (Fasta_Read (fp, s, tag))
    {
      findTandems(s, tag);
    }

    cerr << "finished in " << timer.length() << "s" << endl;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }
  catch (const ExitProgramNormally & e)
  {
    retval = 0;
  }
  catch (const amosException & e)
  {
    cerr << e << endl;
    retval = 100;
  }
  catch (...)
    {
      cerr << "uncaught exception\n"; 
    }

  try
  {
    if (tf) delete tf;
  }
  catch (const amosException & e)
  {
    cerr << "amosException while deleting tf: " << e << endl;
    retval = 105;
  }

  return retval;
}



