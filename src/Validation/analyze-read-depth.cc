#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "Feature_AMOS.hh"

using namespace AMOS;
using namespace std;

int s_verbose(0);
int clusterdist(0);
int PRINTIID(0);
int DEPTHONLY(0);
int MINCONTIGLEN(0);
float thresholdx = 3.0;
string CONTIGIIDS;

double uconslen(0);
double ureadlen(0);
int PRINTRAW(0);
int USEUNGAPPED(0);

void handlecontig(Contig_t & contig,
                  int & contigs,
                  double & conslen, 
                  double & readlen,
                  int & readcount)
{
  int clen = contig.getLength();

  if (USEUNGAPPED)
  {
    clen = contig.getUngappedLength();
  }

  if (clen >= MINCONTIGLEN)
  {
    contigs++;

    int clen = contig.getLength();
    int uclen = contig.getUngappedLength();

    conslen += clen;
    uconslen += uclen;

    double rl = 0.0;
    double url = 0.0;
    int rc = 0;

    vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::const_iterator ti;
    for (ti = tiling.begin(); ti != tiling.end(); ti++)
    {
      rl += ti->getGappedLength();
      url += ti->range.getLength();
      rc++;
    }

    readlen += rl;
    ureadlen += url;
    readcount += rc;

    if (PRINTRAW)
    {
      cout << contig.getIID() << "\t" 
           << rc << "\t"
           << clen << "\t" 
           << uclen << "\t"
           << (rl/clen) << "\t"
           << (url/uclen) << endl;
    }
  }
}


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
"In raw mode (-r) output is:\n"
"contigiid numreads glen uglen gcov ugcov\n"
"\n"
"   Usage: analyze-read-depth [options] bank\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -v|--verbose Produce a verbose output\n"
"   -x <val>     Flag regions val x times the global average depth\n"
"   -c <dist>    Cluster regions within <dist> bp\n"
"   -I <file>    Only use list of contigs iids in file for computing average\n"
"   -i           Print contig IIDs instead of EIDs\n"
"   -d           Just compute the depth. This returns a line with 4 values:\n"
"                the minimum contig length considered, number of contigs\n"
"                sum of contig lengths and the average contig depth\n"
"   -l <len>     Only use contigs >= len for computing average\n"
"   -r           Print the depth for each contig\n"
"   -u           Use ungapped length for reports\n";
"\n";

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    tf->getOptions()->addOptionResult("v|verbose", &s_verbose);
    tf->getOptions()->addOptionResult("x=f",       &thresholdx);
    tf->getOptions()->addOptionResult("c=i",       &clusterdist);
    tf->getOptions()->addOptionResult("I=s",       &CONTIGIIDS);
    tf->getOptions()->addOptionResult("i",         &PRINTIID);
    tf->getOptions()->addOptionResult("d",         &DEPTHONLY);
    tf->getOptions()->addOptionResult("l=i",       &MINCONTIGLEN);
    tf->getOptions()->addOptionResult("r",         &PRINTRAW);
    tf->getOptions()->addOptionResult("u",         &USEUNGAPPED);
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
      double conslen=0;
      double readlen=0;
      int contigs=0;
      int readcount=0;

      Contig_t contig;

      if (!CONTIGIIDS.empty())
      {
        int id;
        ifstream file;
        file.open(CONTIGIIDS.c_str());

        while (file >> id)
        {
          contig_bank.fetch(id, contig);
          handlecontig(contig, contigs, conslen, readlen, readcount);
        }
      }
      else
      {
        for (c = contigmap.begin(); c!= contigmap.end(); c++)
        {
          contig_bank.fetch(c->iid, contig);
          handlecontig(contig, contigs, conslen, readlen, readcount);
        }
      }

      double avgdepth = readlen / conslen;
      double uavgdepth = ureadlen / uconslen;
      double threshdepth = avgdepth * thresholdx;

      cerr << "Processed contigs >= " << MINCONTIGLEN << "bp." << endl;
      cerr << "Processed reads: " << readcount << " contigs: " << contigs << endl;
      cerr << "Global average contig depth: " << avgdepth << " [" << readlen << "/" << conslen << "]" << endl;
      cerr << "Global ungapped average contig depth: " << uavgdepth << " [" << ureadlen << "/" << uconslen << "]" << endl;
      
      if (DEPTHONLY)
      {
        if (USEUNGAPPED) { fprintf(stdout, ">%d\t% 6d% 10.0f% 8.1f\n", MINCONTIGLEN, contigs, uconslen, uavgdepth); }
        else             { fprintf(stdout, ">%d\t% 6d% 10.0f% 8.1f\n", MINCONTIGLEN, contigs, conslen,  avgdepth); }
      }
      else
      {
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
            int roffset = ti->getRightOffset();

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
                if (PRINTIID)
                {
                  cout << c->iid << "\t" << Feature_t::COVERAGE << "\t" << start << "\t" << end << "\tHIGH_READ_COVERAGE " << maxdepth << endl;
                }
                else
                {
                  cout << c->eid << "\t" << Feature_t::COVERAGE << "\t" << start << "\t" << end << "\tHIGH_READ_COVERAGE " << maxdepth << endl;
                }
                maxdepth = 0;

                cluster = false;
              }
            }
          }

          if (cluster)
          {
            if (PRINTIID)
            {
              cout << c->iid << "\t" << Feature_t::COVERAGE << "\t" << start << "\t" << conslen << "\tHIGH_READ_COVERAGE " << maxdepth << endl;
            }
            else
            {
              cout << c->eid << "\t" << Feature_t::COVERAGE << "\t" << start << "\t" << conslen << "\tHIGH_READ_COVERAGE " << maxdepth << endl;
            }
          }
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
