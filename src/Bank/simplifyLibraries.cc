#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"


using namespace AMOS;
using namespace std;

// Collapse libraries with identical mean and sd into a single library
// Also fixes fragments to point to new libraries


int main(int argc, char ** argv)
{
  if (argc != 2)
  {
    cerr << "Usage: simplifyLibraries bankname" << endl;
    return EXIT_FAILURE;
  }

  Bank_t lib_bank(Library_t::NCODE);
  Bank_t frg_bank(Fragment_t::NCODE);
  string bank_name = argv[1];

  int verbose = 0;

  cerr << "Processing " << bank_name << " at " << Date() << endl;


  try
  {
    lib_bank.open(bank_name, B_READ | B_WRITE);
    frg_bank.open(bank_name, B_READ|B_WRITE);

    map<ID_t, ID_t> newlibraries;
    map<ID_t, ID_t>::iterator mi;

    vector<Library_t> keeplibraries;
    vector<Library_t>::const_iterator vi;

    const IDMap_t & map =  lib_bank.getIDMap();
    IDMap_t::const_iterator li;
    for (li = map.begin(); li != map.end(); li++)
    {
      Library_t lib;
      lib_bank.fetch(li->iid, lib);

      Distribution_t dist = lib.getDistribution();

      bool duplicate = false;
      for (vi = keeplibraries.begin(); vi != keeplibraries.end(); vi++)
      {
        if ((dist.mean == vi->getDistribution().mean) &&
            (dist.sd   == vi->getDistribution().sd))
        {
          duplicate = true;
          newlibraries.insert(make_pair(lib.getIID(), vi->getIID()));
          break;
        }
      }

      if (!duplicate)
      {
        newlibraries.insert(make_pair(lib.getIID(), lib.getIID()));
        keeplibraries.push_back(lib);
      }
    }

    int removecount = 0;
    int keepcount = 0;

    for (mi = newlibraries.begin(); mi != newlibraries.end(); mi++)
    {
      if (mi->first != mi->second)
      {
        lib_bank.remove(mi->first);
        removecount++;
      }
      else
      {
        keepcount++;
      }
    }

    cerr << "Kept " << keepcount << " libraries, deleted " << removecount << endl;
    lib_bank.close();


    const IDMap_t & frgmap = frg_bank.getIDMap();
    int update = 0;
    int all = 0;

    for (li = frgmap.begin(); li != frgmap.end(); li++)
    {
      Fragment_t frg;
      frg_bank.fetch(li->iid, frg);

      all++;

      ID_t oldlib = frg.getLibrary();
      ID_t newlib = newlibraries[oldlib];

      if (oldlib != newlib)
      {
        update++;
        frg.setLibrary(newlib);
        frg_bank.replace(li->iid, frg);
      }
    }
    
    cerr << "Updated " << update << " of " << all << " fragments" << endl;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;

  return EXIT_SUCCESS;
}
