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
"Builds a barcode graph of multiple assemblies\n"
"\n"
"   Usage: barcode-graph [options] bank1 ... bankn\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -v|--verbose  Produce a verbose output\n"
"   -m|--min_contig_size <size> Minimum number of reads\n"
"   -d|--display_all_links  Display full graph, not just max\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("v|verbose",           &s_verbose,       "Be verbose when reporting");
    tf->getOptions()->addOptionResult("m|min_contig_size=i", &MIN_CONTIG_SIZE, "Minimum Number of reads");
    tf->getOptions()->addOptionResult("d|display_all_links", &DISPLAY_ALL,     "Display all graph transitions");
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() <= 1)
    {
      cerr << "Usage: barcode-graph [options] bank1 ... bankn" << endl;
      return EXIT_FAILURE;
    }

    bool first = true;

    typedef map<ID_t, ID_t> ReadLookup;
    ReadLookup read2contig;
    string lastbank;

    if (s_verbose)
    {
      cerr << "MinContigSize: " << MIN_CONTIG_SIZE << endl;
    }

    cout << "digraph G" << endl
         << "{" << endl
         << "rankdir=LR" << endl;

    while (!argvv.empty())
    {
      string bankname = argvv.front(); argvv.pop_front();
      cerr << "Processing " << bankname << " at " << Date() << endl;

      Bank_t contig_bank(Contig_t::NCODE);
      contig_bank.open(bankname, B_READ);

      //Bank_t read_bank(Read_t::NCODE);
      //read_bank.open(bankname, B_READ);

      bankname = bankname.substr(0, bankname.find_first_of("/ "));

      const IDMap_t & contigmap = contig_bank.getIDMap();
      IDMap_t::const_iterator c;

      // Just print a node for each contig
      cout << "{ rank=same;" << endl;
      cout << bankname << endl;
      for (c = contigmap.begin(); c!= contigmap.end(); c++)
      {
        Contig_t contig;
        contig_bank.fetch(c->iid, contig);

        int tilingsize = contig.getReadTiling().size();

        if (tilingsize >= MIN_CONTIG_SIZE)
        {
          cout << "\"" << bankname << "." << c->iid << "\" " 
               << "[label=\"" << c->iid << "\\n[" << tilingsize << "r " << contig.getLength() << "bp]\"]" << endl;
        }
      }
      cout << "}" << endl;

      if (!first)
      {
        cout << lastbank << " -> " << bankname << endl;
        // Draw an edge
        for (c = contigmap.begin(); c!= contigmap.end(); c++)
        {
          Contig_t contig;
          contig_bank.fetch(c->iid, contig);

          vector<Tile_t>::const_iterator ti;

          typedef map<ID_t, int> ContigStatus;
          ContigStatus oldcontigcount;
          ContigStatus::iterator cs;

          int tilingsize = contig.getReadTiling().size();

          if (tilingsize >= MIN_CONTIG_SIZE)
          {
            // Count where each read was in the old contig
            for (ti = contig.getReadTiling().begin(); ti != contig.getReadTiling().end(); ti++)
            {
              ReadLookup::iterator r2c = read2contig.find(ti->source);

              ID_t oldcontig;
              if (r2c == read2contig.end()) { oldcontig = 0; }
              else                          { oldcontig = r2c->second; }

              cs = oldcontigcount.find(oldcontig);
              if (cs == oldcontigcount.end())
              {
                oldcontigcount.insert(make_pair(oldcontig, 1));
              }
              else
              {
                cs->second++;
              }
            }

            // Find the most similiar contig
            ID_t oldmax = oldcontigcount.begin()->first;
            int oldmaxcount = oldcontigcount.begin()->second;

            if (DISPLAY_ALL)
            {
              for (cs = oldcontigcount.begin(); cs != oldcontigcount.end(); cs++)
              {
                oldmax = cs->first;
                oldmaxcount = cs->second;

                if (oldmaxcount >= MIN_CONTIG_SIZE)
                {
                  // Print the pair
                  cout << "\"" << lastbank << "." << oldmax << "\" -> " 
                       << "\"" << bankname << "." << c->iid << "\""
                       << " [label=\"" << oldmaxcount << "\"]" << endl;
                }
              }
            }
            else
            {
              for (cs = oldcontigcount.begin(); cs != oldcontigcount.end(); cs++)
              {
                if (cs->second > oldmaxcount)
                {
                  oldmax = cs->first;
                  oldmaxcount = cs->second;
                }
              }

              if (oldmaxcount >= MIN_CONTIG_SIZE)
              {
                // Print the pair
                cout << "\"" << lastbank << "." << oldmax << "\" -> " 
                     << "\"" << bankname << "." << c->iid << "\""
                     << " [label=\"" << oldmaxcount << "/" << tilingsize << "\"]" << endl;
              }
            }
          }
        }
      }

      cout << endl << endl;

      first = false;
      lastbank = bankname;
      read2contig.clear();

      for (c = contigmap.begin(); c!= contigmap.end(); c++)
      {
        Contig_t contig;
        contig_bank.fetch(c->iid, contig);

        vector<Tile_t>::const_iterator ti;

        for (ti = contig.getReadTiling().begin(); ti != contig.getReadTiling().end(); ti++)
        {
          read2contig.insert(make_pair(ti->source, c->iid));
        }
      }
    }

    cout << "}" << endl;
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
