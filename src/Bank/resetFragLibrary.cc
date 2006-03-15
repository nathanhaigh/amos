#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"


using namespace AMOS;
using namespace std;



int main(int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: updateClrRanges bankname fragid.newlibid" << endl;
    return EXIT_FAILURE;
  }

  Bank_t lib_bank(Library_t::NCODE);
  Bank_t frag_bank(Fragment_t::NCODE);
  string bank_name = argv[1];
  string file_name = argv[2];

  int verbose = 0;

  cerr << "Processing " << bank_name << " at " << Date() << endl;


  try
  {
    frag_bank.open(bank_name, B_READ | B_WRITE);
    lib_bank.open(bank_name, B_READ);

    FILE * file = fopen(file_name.c_str(), "r");

    if (!file)
    {
      cerr << "Can't open file: " << file_name << endl;
      return EXIT_FAILURE;
    }

    int fragid, libid;
    int count = 0;

    Fragment_t frag;

    while(fscanf(file, "%d %d", &fragid, &libid) > 0)
    {
      count++;

      frag_bank.fetch(fragid, frag);
      frag.setLibrary(libid);
      frag_bank.replace(fragid, frag);
    }

    cerr << "Updated " << count << " records." << endl;

    lib_bank.close();
    frag_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;

  return EXIT_SUCCESS;
}
