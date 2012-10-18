#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"

#include "ContigFattener.hh"

using namespace AMOS;
using namespace std;

int s_leftExtension(0);
int s_rightExtension(0);
int s_growContig(0);
int s_fatten(0);

int s_requireConfirmation(1);

int s_fattenerRangeStart(-1);
int s_fattenerRangeEnd(-1);
int s_ungapped(0);

int s_extensionDistance(0);
string s_readToExtend("");

string s_contigeid("");
int    s_contigiid(0);

int s_verbose(0);



void fattenContig(Contig_t & contig, Bank_t & read_bank)
{
  cerr << "Fattening eid: " << contig.getEID()
       << " iid: " << contig.getIID()
       << endl;

  ContigFattener fattener(contig, read_bank, s_verbose);

  if (s_extensionDistance)
  {
    fattener.extend3Read(s_readToExtend, s_extensionDistance);
  }

  if (s_growContig)
  {
    if (s_rightExtension) { fattener.growContigRight(s_requireConfirmation); }
    if (s_leftExtension)  { fattener.growContigLeft(s_requireConfirmation); }
  }

  if (s_fatten)
  {
    fattener.fattenContig(s_fattenerRangeStart, 
                          s_fattenerRangeEnd,
                          s_leftExtension,
                          s_rightExtension);
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
"Fattens a contig by extending reads outside of their 3' clear range.\n"
"By default, just fattens the contig, but can also be used to grow the contig,\n"
"or extend a specific read, or some combination of all three.\n"
"\n"
"  Order of operations:\n"
"  1) Extend Specified Read\n"
"  2) Grow contig\n"
"  3) Fatten Contig\n"
"\n"
"   Usage: fattenContig [options] bank\n"
"\n"
"     bank AMOS bank containing contigs to fatten\n"
"\n"
"   Options\n"
"   -------------------\n"
"   -f|--fatten   Fatten contig in optional region [DEFAULT]\n"
"   -g|--grow     Grow the contig (confirmed contig extension)\n"
"   --unconfirmed Maximal extend contig without confirming read\n"
"\n"
"   -L            Grow or fatten to the left only (Default: L & R)\n"
"   -R            Grow or fatten to the right only\n"
"\n"
"   -x            [Optional] Start of range to fatten\n"
"   -y            [Optional] End of range to fatten\n"
"   -U|--ungapped Coordinates are ungapped\n"
"\n"
"   -E eid        Modify just contig named eid\n"
"   -I iid        Modify just contig named iid\n"
"   -r <seqname>  Extend Specified Read\n"
"   -d <distance> Distance to extend read (-1 = all the way to end of read)\n"
"\n"
"   -v|--verbose  Produce a verbose output\n"
"\n";


    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();

    int unconfirmed = !s_requireConfirmation;

    tf->getOptions()->addOptionResult("x=i",         &s_fattenerRangeStart, "Specify start of range to fatten");
    tf->getOptions()->addOptionResult("y=i",         &s_fattenerRangeEnd,   "Specify start of range to fatten");
    tf->getOptions()->addOptionResult("d=i",         &s_extensionDistance,  "Specify distance to extend");
    tf->getOptions()->addOptionResult("r=s",         &s_readToExtend,       "Specify read to extend");
    tf->getOptions()->addOptionResult("E=s",         &s_contigeid,          "Specify Contig eid");
    tf->getOptions()->addOptionResult("I=i",         &s_contigiid,          "Specify Contig iid");
    tf->getOptions()->addOptionResult("R",           &s_rightExtension,     "Perform Right Extension while fattening or growing");
    tf->getOptions()->addOptionResult("L",           &s_leftExtension,      "Perform Left Extension while fattening or growing");
    tf->getOptions()->addOptionResult("g|grow",      &s_growContig,         "Grow the contig");
    tf->getOptions()->addOptionResult("f|fatten",    &s_fatten,             "Fatten the contig in the optional range");
    tf->getOptions()->addOptionResult("v|verbose",   &s_verbose,            "Be verbose when reporting");
    tf->getOptions()->addOptionResult("U|ungapped",  &s_ungapped,           "Ranges are ungapped coordinates");
    tf->getOptions()->addOptionResult("unconfirmed", &unconfirmed,          "Do unconfirmed contig growing");


    tf->handleStandardOptions();

    s_requireConfirmation = !unconfirmed;

    // Sanity check that a read and a distance are specified together
    if ((!s_readToExtend.empty() && !s_extensionDistance) ||
        ( s_readToExtend.empty() &&  s_extensionDistance))
    {
      throw amosException("You must specify a readname and a distance to extend together",
                          "fattenContig");
    }

    // Enable both by default
    if (!s_leftExtension && !s_rightExtension)
    {
      s_leftExtension = s_rightExtension = 1;
    }

    // By default, just fatten
    if (!s_fatten && !s_growContig && !s_extensionDistance)
    {
      s_fatten = 1;
    }


    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 1)
    {
      cerr << "Usage: fattenContig [options] bankname" << endl;
      return EXIT_FAILURE;
    }

    string bankname = argvv.front(); argvv.pop_front();

    Bank_t contig_bank(Contig_t::NCODE);
    Bank_t read_bank(Read_t::NCODE);

    cerr << "Processing " << bankname << " at " << Date() << endl;

    contig_bank.open(bankname, B_READ|B_SPY);
    read_bank.open(bankname, B_READ|B_SPY);
    Contig_t contig;

    if (!s_contigeid.empty())
    {
      contig_bank.fetch(s_contigeid, contig);
      fattenContig(contig, read_bank);
      //contig_bank.replace(contig.getEID(), contig);
    }
    else if (s_contigiid != AMOS::NULL_ID)
    {
      contig_bank.fetch(s_contigiid, contig);
      fattenContig(contig, read_bank);
      //contig_bank.replace(contig.getIID(), contig);
    }
    else
    {
      AMOS::IDMap_t::const_iterator ci;
      for (ci = contig_bank.getIDMap().begin();
           ci;
           ci++)
      {
        contig_bank.fetch(ci->iid, contig);
        fattenContig(contig, read_bank);
        //contig_bank.replace(contig.getEID(), contig);
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
