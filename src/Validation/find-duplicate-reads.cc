#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"


using namespace AMOS;
using namespace std;



long hashstring(const string & s)
{
  long retval = 0;
  int len = s.length();

  for (int i = 0; i < len; i++)
  {
    int j;
    switch (toupper(s[i]))
    {
      case 'C': j = 1; break;
      case 'T': j = 2; break;
      case 'G': j = 3; break;
      default: j = 0; break;
    };

    retval += j << ((i * 2) % 63);
  }

  return retval;
}



int main(int argc, char ** argv)
{
  if (argc != 2)
  {
    cerr << "Usage: find-duplicate-reads bankname" << endl;
    return EXIT_FAILURE;
  }

  Bank_t red_bank(Read_t::NCODE);
  string bank_name = argv[1];

  int verbose = 0;

  cerr << "Processing " << bank_name << " at " << Date() << endl;


  try
  {
    red_bank.open(bank_name, B_READ);

    multimap<long, ID_t> hash2read;

    const IDMap_t & readmap = red_bank.getIDMap();
    IDMap_t::const_iterator r;

    int count = 0;
    for (r = readmap.begin(); r!= readmap.end(); r++)
    {
      Read_t red;
      red_bank.fetch(r->iid, red);

      string seq = red.getSeqString();
      long hash = hashstring(seq);

      hash2read.insert(make_pair(hash, r->iid));
      count++;
    }

    cerr << "Loaded " << count << " reads." << endl;

    count = 0;


    multimap<long, ID_t>::const_iterator h1;
    multimap<long, ID_t>::const_iterator h2;

    for (h1 = hash2read.begin(); h1 != hash2read.end(); h1++)
    {
      Read_t r1;
      red_bank.fetch(h1->second, r1);
      string seq1 = r1.getSeqString();

      int l = seq1.length();

      int dups = 0;

      h2 = h1;
      h2++;
      for (; ((h2 != hash2read.end()) && (h1->first == h2->first)); h2++)
      {
        Read_t r2;
        red_bank.fetch(h2->second, r2);

        string seq2 = r2.getSeqString();
        count++;

        if (l == seq2.length())
        {

          bool match = true;
          for (int i = 0; i < l; i++)
          {
            char s1 = toupper(seq1[i]);
            char s2 = toupper(seq2[i]);

            if (s1 == 'N') { s1 = 'A'; }
            if (s2 == 'N') { s2 = 'A'; }

            if (s1 != s2)
            {
              match = false;
              break;
            }
          }

          if (match)
          {
            dups++;

            if (dups == 1) { cout << r1.getEID(); }
            cout << "\t" << r2.getEID();
          }
        }
      }

      if (dups) { cout << endl; }
    }

    cerr << "Made " << count << " comparisons" << endl;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;

  return EXIT_SUCCESS;
}
