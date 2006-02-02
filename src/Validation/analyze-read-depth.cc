#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

using namespace AMOS;
using namespace std;

int s_verbose(0);
int clusterdist(0);
float thresholdx = 3.0;


int main (int argc, char ** argv)
{
  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Analyze the depth of coverage along contigs and flag regions\n"
"that are significantly deeper than the average depth of coverage\n"
"By default, flag regions that are more than 3-times the average\n"
"depth. Optionally, cluster regions that are within <dist> bp\n"
"\n"
"Output is suitable for loadFeatures:\n"
"contigeid D end5 end3 maxdepth\n"
"\n"
"   Usage: analyze-read-depth [options] bank\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -v|--verbose Produce a verbose output\n"
"   -x <val>     Flag regions val x times the global average depth\n"
"   -c <dist>    Cluster regions within <dist> bp\n"
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("v|verbose", &s_verbose,   "Be verbose when reporting");
    tf->getOptions()->addOptionResult("x=f",       &thresholdx,  "Be verbose when reporting");
    tf->getOptions()->addOptionResult("c=i",       &clusterdist, "Be verbose when reporting");
    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 1)
    {
      cerr << "Usage: analyze-read-depth [options] bank" << endl;
      return EXIT_FAILURE;
    }

    bool first = true;

    while (!argvv.empty())
    {
      string bankname = argvv.front(); argvv.pop_front();
      cerr << "Processing " << bankname << " at " << Date() << endl;

      Bank_t contig_bank(Contig_t::NCODE);
      contig_bank.open(bankname, B_READ);

      bankname = bankname.substr(0, bankname.find_first_of("/ "));

      const IDMap_t & contigmap = contig_bank.getIDMap();
      IDMap_t::const_iterator c;

      // compute global average
      int conslen=0;
      int readlen=0;
      int contigs=0;

      for (c = contigmap.begin(); c!= contigmap.end(); c++)
      {
        Contig_t contig;
        contig_bank.fetch(c->iid, contig);

        contigs++;
        conslen += contig.getLength();

        vector<Tile_t> & tiling = contig.getReadTiling();
        vector<Tile_t>::const_iterator ti;
        for (ti = tiling.begin(); ti != tiling.end(); ti++)
        {
          readlen += ti->getGappedLength();
        }
      }

      double avgdepth = ((double) readlen) / conslen;
      double threshdepth = avgdepth * thresholdx;

      cerr << "Global average contig depth: " << avgdepth << endl;
      cerr << "Flagging regions above: " << threshdepth << endl;

      for (c = contigmap.begin(); c!=contigmap.end(); c++)
      {
        Contig_t contig;
        contig_bank.fetch(c->iid, contig);

        vector<Tile_t> & tiling = contig.getReadTiling();
        vector<Tile_t>::const_iterator ti;
        sort(tiling.begin(), tiling.end(), TileOrderCmp());

        multiset<int> endpoints;
        multiset<int>::iterator ep, ep2;

        bool cluster = false;
        int start;
        int end = -clusterdist - 10;
        int maxdepth = 0;
        int conslen = contig.getLength();

        for (ti = tiling.begin(); ti != tiling.end(); ti++)
        {
          int offset = ti->offset;
          int roffset = offset + ti->getGappedLength() - 1;

          ep = endpoints.begin();
          while ((ep != endpoints.end()) && (*ep < offset))
          {
            ep2 = ep;
            ep++;
            endpoints.erase(ep2);
          }

          endpoints.insert(roffset);
          int dcov = endpoints.size();

          if (dcov > threshdepth)
          {
            if (!cluster)
            {
              start = offset;
              cluster = true;
            }

            end = roffset;

            if (dcov > maxdepth) { maxdepth = dcov; }
          }
          else
          {
            if (cluster && (offset > end + clusterdist))
            {
              cout << c->eid << "\tD\t" << start << "\t" << end << "\tHIGH_READ_COVERAGE " << maxdepth << endl;
              maxdepth = 0;

              cluster = false;
            }
          }
        }

        if (cluster)
        {
          cout << c->eid << "\tD\t" << start << "\t" << conslen << "\tHIGH_READ_COVERAGE " << maxdepth << endl;
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
