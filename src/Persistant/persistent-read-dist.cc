#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

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
"Computes the read distance evolution\n"
"\n"
"   Usage: persistent-read-dist [options] bank1 ... bankn\n"
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

    if (argvv.size() == 0)
    {
      cerr << "Usage: persistent-read-dist [options] bank1 ... bankn" << endl;
      return EXIT_FAILURE;
    }

    bool first = true;

    typedef map<ID_t, pair<ID_t,int> > ReadPosLookup;
    ReadPosLookup read2contigpos;
    string lastbank;

    if (s_verbose)
    {
      cerr << "MinContigSize: " << MIN_CONTIG_SIZE << endl;
    }

    while (!argvv.empty())
    {
      string bankname = argvv.front(); argvv.pop_front();
      cerr << "Processing " << bankname << " at " << Date() << endl;

      Bank_t contig_bank(Contig_t::NCODE);
      contig_bank.open(bankname, B_READ);

      //Bank_t read_bank(Read_t::NCODE);
      //read_bank.open(bankname, B_READ);

      bankname = bankname.substr(0, bankname.find_first_of("/ "));

      cout << ">" << bankname << endl;

      const IDMap_t & contigmap = contig_bank.getIDMap();
      IDMap_t::const_iterator c;

      for (c = contigmap.begin(); c!= contigmap.end(); c++)
      {
        Contig_t contig;
        contig_bank.fetch(c->iid, contig);

        vector<Tile_t>::const_iterator t1;
        vector<Tile_t>::const_iterator t2;

        int tilingsize = contig.getReadTiling().size();

        if (!first)
        {
          for (t1 = contig.getReadTiling().begin(); t1 != contig.getReadTiling().end(); t1++)
          {
            int t1pos = t1->offset;
            if (t1->range.isReverse()) { t1pos = t1pos + t1->getGappedLength(); }

            for (t2 = t1+1; t2 != contig.getReadTiling().end(); t2++)
            {
              int t2pos = t2->offset;
              if (t2->range.isReverse()) { t2pos = t2pos + t2->getGappedLength(); }

              int dist = abs(t2pos - t1pos);

              ReadPosLookup::const_iterator r1 = read2contigpos.find(t1->source);
              ReadPosLookup::const_iterator r2 = read2contigpos.find(t2->source);

              int olddist = 0;
              int delta = 0;

              // Distance is only defined if the two reads existed in the same contig before
              if (r1 != read2contigpos.end() &&
                  r2 != read2contigpos.end() &&
                  r1->second.first == r2->second.first)
              {
                olddist = abs(r1->second.second - r2->second.second);
                delta = dist-olddist;
              }

              cout << c->iid << "\t" 
                   << t1->source << "\t" 
                   << t2->source << "\t" 
                   << dist << "\t"
                   << olddist << "\t"
                   << delta << endl;
            }
          }
        }
      }

      read2contigpos.clear();
      first = false;

      for (c = contigmap.begin(); c!= contigmap.end(); c++)
      {
        Contig_t contig;
        contig_bank.fetch(c->iid, contig);

        vector<Tile_t>::const_iterator ti;

        for (ti = contig.getReadTiling().begin(); ti != contig.getReadTiling().end(); ti++)
        {
          int t1pos = ti->offset;
          if (ti->range.isReverse()) { t1pos = t1pos + ti->getGappedLength(); }

          read2contigpos.insert(make_pair(ti->source, make_pair(c->iid, t1pos)));
        }
      }
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
