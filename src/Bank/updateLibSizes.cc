#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"


using namespace AMOS;
using namespace std;



int main(int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: updateLibSizes bankname [libiid.mea.std | -dump]" << endl;
    return EXIT_FAILURE;
  }

  BankStream_t lib_bank(Library_t::NCODE);
  string bank_name = argv[1];
  string sizes_name = argv[2];

  int verbose = 0;

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    lib_bank.open(bank_name, B_READ | B_WRITE);

    if (sizes_name == "-dump")
    {
      Library_t lib;

      while (lib_bank >> lib)
      {
        Distribution_t dist = lib.getDistribution();
        ID_t libid = lib.getIID();

        cout << libid << " " << dist.mean << " " << dist.sd << endl;
      }
    }
    else
    {
      FILE * fclr = fopen(sizes_name.c_str(), "r");

      if (!fclr)
      {
        cerr << "Can't open sizes: " << sizes_name << endl;
        return EXIT_FAILURE;
      }

      int libid,a,b;
      int count = 0;

      while(fscanf(fclr, "%d %d %d", &libid, &a, &b) > 0)
      {
        count++;
        Distribution_t d;
        d.mean=a;
        d.sd = b;
        Library_t lib;

        lib_bank.fetch(libid, lib);
        lib.setDistribution(d);
        lib_bank.replace(libid, lib);

      }
      cerr << "Loaded " << count << " records." << endl;
    }

    lib_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;

  return EXIT_SUCCESS;
}
