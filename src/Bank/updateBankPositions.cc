#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"


using namespace AMOS;
using namespace std;



int main(int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: updateBankPositions bankname posfile" << endl;
    return EXIT_FAILURE;
  }

  Bank_t read_bank(Read_t::NCODE);
  string bank_name = argv[1];
  string pos_name = argv[2];

  int verbose = 0;

  vector<string> badSeqs;

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    read_bank.open(bank_name);
    Read_t read;

    FILE * fpos = fopen(pos_name.c_str(), "r");

    if (!fpos)
    {
      cerr << "Can't open posfile: " << pos_name << endl;
      return EXIT_FAILURE;
    }

    char c;
    char hex[5];
    hex[4] = '\0';

    int MAX_SEQNAME=100;
    char seqname[MAX_SEQNAME];
    int version;
    int x;
    int count = 0;

    while(fscanf(fpos, "%s\t%d\t", seqname, &version) > 0)
    {
      if (!strcmp(seqname, "SequenceName"))
      {
        c = ' ';
        while (c != '\n')
        {
          c = fgetc(fpos);
        }
      }
      else
      {
        if (verbose) { cerr << "Loading " << seqname; }
        vector<int16_t> positions;

        c = fgetc(fpos);
        ungetc(c, fpos);

        while (c != '\n' && fgets (hex, 5, fpos))
        {
          sscanf(hex, "%04x", &x);
          positions.push_back(x);

          c = fgetc(fpos);
          ungetc(c, fpos);
        }

        if (verbose) {cerr << " " << positions.size(); }

        if (read_bank.existsEID(seqname))
        {
          if (verbose) {  cerr << "."; }
          read_bank.fetch(seqname, read);

          if (read.getLength() != positions.size())
          {
            badSeqs.push_back(seqname);

            if (0)
            {
              throw Exception_t("Length mismatch detected!",
                                __LINE__, __FILE__);
            }
          }

          read.setBasePositions(positions);
          read_bank.replace(read.getIID(), read);
          count++;
        }
        
        if (verbose) { cerr << endl; }
      }
    }

    cerr << "Loaded " << count << " records." << endl;

    cerr << "Cleaning Bank" << endl;
    read_bank.clean();

    cerr << badSeqs.size() << " mismatches detected" << endl;
    vector<string>::iterator si;
    for (si =  badSeqs.begin();
         si != badSeqs.end();
         si++)
    {
      cerr << *si;
      read_bank.fetch(si->c_str(), read);

      cerr << " slen:" << read.getLength() 
           << " plen:" << read.getBasePositions().size() 
           << endl;
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;


  return EXIT_SUCCESS;
}








