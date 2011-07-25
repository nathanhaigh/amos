#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include <map>

using namespace AMOS;
using namespace std;

int s_verbose(0);
int MIN_CONTIG_SIZE(0);
int DISPLAY_ALL(0);


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Builds a bank of multiple assemblies\n"
"\n"
"   Usage: build-persistant-bank [options] bank1 ... bankn\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -v|--verbose  Produce a verbose output\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("v|verbose",           &s_verbose,       "Be verbose when reporting");
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() <= 2)
    {
      cerr << "Usage: build-persistant-bank [options] newbank bank1 ... bankn" << endl;
      return EXIT_FAILURE;
    }

    bool first = true;

    typedef map<ID_t, ID_t> ReadLookup;
    ReadLookup read2contig;
    string lastbank;

    string newbankname = argvv.front(); argvv.pop_front();

    Bank_t new_read_bank(Read_t::NCODE);
    Bank_t new_frg_bank(Fragment_t::NCODE);
    Bank_t new_contig_bank(Contig_t::NCODE);
    Bank_t new_scaffold_bank(Scaffold_t::NCODE);
    Bank_t new_lib_bank(Library_t::NCODE);
    Bank_t new_feat_bank(Feature_t::NCODE);

    new_read_bank.create(newbankname);
    new_frg_bank.create(newbankname);
    new_contig_bank.create(newbankname);
    new_scaffold_bank.create(newbankname);
    new_lib_bank.create(newbankname);
    new_feat_bank.create(newbankname);

    while (!argvv.empty())
    {
      string bankname = argvv.front(); argvv.pop_front();
      cerr << "Processing " << bankname << " at " << Date() << endl;

      BankStream_t read_bank(Read_t::NCODE);
      read_bank.open(bankname, B_READ);

      BankStream_t frg_bank(Fragment_t::NCODE);
      frg_bank.open(bankname, B_READ);

      BankStream_t contig_bank(Contig_t::NCODE);
      contig_bank.open(bankname, B_READ);

      BankStream_t scaffold_bank(Scaffold_t::NCODE);
      scaffold_bank.open(bankname, B_READ);

      BankStream_t feat_bank(Feature_t::NCODE);
      feat_bank.open(bankname, B_READ);


      if (first)
      {
        cerr << "L";

        BankStream_t lib_bank(Library_t::NCODE);
        lib_bank.open(bankname, B_READ);

        Library_t lib;
        while (lib_bank >> lib)
        {
          new_lib_bank.append(lib);
        }

        first = false;
      }

      bankname = bankname.substr(0, bankname.find_first_of("/ "));


      HASHMAP::hash_map<ID_t, ID_t> frgtranslate;
      HASHMAP::hash_map<ID_t, ID_t> readtranslate;
      HASHMAP::hash_map<ID_t, ID_t> contigtranslate;
      ID_t newiid;


      cerr << "F";
      const IDMap_t & frgmap = frg_bank.getIDMap();
      IDMap_t::const_iterator c;

      for (c = frgmap.begin(), newiid = new_frg_bank.getMaxIID()+1;
           c != frgmap.end(); 
           c++, newiid++)
      {
        frgtranslate.insert(make_pair(c->iid, newiid));
      }


      cerr << "R";

      Read_t read;
      newiid = new_read_bank.getMaxIID()+1;

      while (read_bank >> read)
      {
        readtranslate.insert(make_pair(read.getIID(), newiid));

        read.setEID(read.getEID()+"_"+bankname);
        read.setIID(newiid);
        read.setFragment(frgtranslate[read.getFragment()]);

        new_read_bank.append(read);
        newiid++;
      }



      cerr << "F2";
      Fragment_t frg;

      while (frg_bank >> frg)
      {
        frg.setEID(frg.getEID()+"_"+bankname);
        frg.setIID(frgtranslate[frg.getIID()]);

        pair<ID_t, ID_t> mates(frg.getMatePair());
        mates.first = readtranslate[mates.first];
        mates.second = readtranslate[mates.second];
        frg.setReads(mates);

        new_frg_bank.append(frg);
      }



      cerr << "C";

      Contig_t contig;
      newiid = new_contig_bank.getMaxIID()+1;

      while (contig_bank >> contig)
      {
        contigtranslate.insert(make_pair(contig.getIID(), newiid));

        contig.setEID(contig.getEID()+"_"+bankname);
        contig.setIID(newiid);

        vector<Tile_t> & tiling = contig.getReadTiling();
        vector<Tile_t>::iterator ti;
        for (ti = tiling.begin(); ti != tiling.end(); ti++)
        {
          ti->source = readtranslate[ti->source];
        }

        new_contig_bank.append(contig);
        newiid++;
      }

      cerr << "S";

      Scaffold_t scaffold;
      newiid = new_scaffold_bank.getMaxIID()+1;

      while (scaffold_bank >> scaffold)
      {
        scaffold.setEID(scaffold.getEID()+"_"+bankname);
        scaffold.setIID(newiid);

        vector<Tile_t> & tiling = scaffold.getContigTiling();
        vector<Tile_t>::iterator si;
        for (si = tiling.begin(); si != tiling.end(); si++)
        {
          si->source = contigtranslate[si->source];
        }

        new_scaffold_bank.append(scaffold);
        newiid++;
      }

      cerr << "F";
      Feature_t feat;
      while (feat_bank >> feat)
      {
        pair<ID_t, NCode_t> src = feat.getSource();

        if (src.second == Contig_t::NCODE)
        {
          src.first = contigtranslate[src.first];
        }
        else if (src.second == Read_t::NCODE)
        {
          src.first = readtranslate[src.first];
        }
        else
        {
          cerr << "Warning: Unsupported feat type " << src.second << endl;
        }

        feat.setSource(src);

        new_feat_bank.append(feat);
      }

      cerr << endl;
    }
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

  try
  {
    if (tf) delete tf;
  }
  catch (const amosException & e)
  {
    cerr << "amosException while deleting tf: " << e << endl;
    retval = 105;
  }

  cerr << "End: " << Date() << endl;
  return retval;
}
