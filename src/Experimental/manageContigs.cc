
#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "AMOS_Foundation.hh"
#include "ContigUtils.hh"

using namespace AMOS;
using namespace std;


int main (int argc, char ** argv)
{

  int retval = 0;
  AMOS_Foundation * tf = NULL;

  try
  {
    string version =  "Version 1.0";
    string dependencies = "";
    string helptext = 
"Manage contigs by deleting contigs, deleting reads from contigs, and\n"
"duplicating contigs\n"
"\n"
"   Usage: manageContigs [options] contig.bnk\n"
"   -C <iid>      IID of contig to copy\n"
"   -c <eid>      EID of contig to copy\n"
"   -D <iid>      IID of contig to delete\n"
"   -d <eid>      EID of contig to delete\n"
"   -e <eidfile>  EID of reads to delete\n"
"   -i <iidfile>  IID of reads to delete\n"
"\n";

    int copyiid = 0;
    int deleteiid = 0;
    string readeids;
    string readiids;

    string copyeid;
    string deleteeid;

    // Instantiate a new TIGR_Foundation object
    tf = new AMOS_Foundation (version, helptext, dependencies, argc, argv);
    tf->disableOptionHelp();
    tf->getOptions()->addOptionResult("C=i", &copyiid,   "Copy iid");
    tf->getOptions()->addOptionResult("D=i", &deleteiid, "Delete iid");

    tf->getOptions()->addOptionResult("c=s", &copyeid,   "Copy eid");
    tf->getOptions()->addOptionResult("d=s", &deleteeid, "Delete eid");

    tf->getOptions()->addOptionResult("e=s", &readeids,  "Delete eid");
    tf->getOptions()->addOptionResult("i=s", &readiids,  "Delete iid");

    tf->handleStandardOptions();

    list<string> argvv = tf->getOptions()->getAllOtherData();

    if (argvv.size() != 1)
    {
      cerr << "Usage: manageContigs [options] contig.bnk" << endl;
      return EXIT_FAILURE;
    }

    string bankname = argvv.front(); argvv.pop_front();

    Bank_t ctg_bank(Contig_t::NCODE);
    Bank_t red_bank(Read_t::NCODE);

    ctg_bank.open(bankname, B_READ | B_WRITE);
    red_bank.open(bankname, B_READ);

    if (!copyeid.empty())   { copyiid = ctg_bank.lookupIID(copyeid); }
    if (!deleteeid.empty()) { deleteiid = ctg_bank.lookupIID(deleteeid); }

    if (copyiid != 0)
    {
      Contig_t contig;
      ctg_bank.fetch(copyiid, contig);

      string eid(contig.getEID());
      eid += "_copy";

      contig.setIID(ctg_bank.getMaxIID()+1);
      contig.setEID(eid);

      ctg_bank.append(contig);
    }
    else if (deleteiid != 0)
    {
      ctg_bank.remove(deleteiid);

      vector<ID_t> bidstodelete;

      BankStream_t feat_bank(Feature_t::NCODE);
      feat_bank.open(bankname, B_READ|B_WRITE);
      ID_t bid = feat_bank.tellg();

      Feature_t feat;
      while (feat_bank >> feat)
      {
        if ((feat.getSource().first == deleteiid) &&
            (feat.getSource().second == Contig_t::NCODE))
        {
          bidstodelete.push_back(bid);
        }
        
        bid = feat_bank.tellg();
      }

      cerr << "Removing Associated Features: ";

      for (int i = 0; i < bidstodelete.size(); i++)
      {
        cerr << ".";
        feat_bank.removeByBID(bidstodelete[i]);
      }

      cerr << endl;

      feat_bank.close();

    }
    else if (!readeids.empty() || !readiids.empty())
    {
      set<ID_t> iidstoremove;

      if (!readeids.empty())
      {
        ifstream eidfile;
        eidfile.open(readeids.c_str());

        if (!eidfile)
        {
          throw Exception_t("Couldn't open EID File", __LINE__, __FILE__);
        }

        string eid;
        while (eidfile >> eid)
        {
          ID_t iid = red_bank.lookupIID(eid.c_str());
          if (iid != AMOS::NULL_ID)
          {
            iidstoremove.insert(iid);
          }
          else
          {
            cerr << "ERROR: READ EID: \"" << eid << "\" not found in bank, skipping" << endl;
          }
        }
      }
      else
      {
        ifstream iidfile;
        iidfile.open(readiids.c_str());

        if (!iidfile)
        {
          throw Exception_t("Couldn't open IID File", __LINE__, __FILE__);
        }

        ID_t iid;
        while (iidfile >> iid)
        {
          iidstoremove.insert(iid);
        }
      }

      Contig_t contig;

      AMOS::IDMap_t::const_iterator ci;
      for (ci = ctg_bank.getIDMap().begin();
           ci;
           ci++)
      {
        ctg_bank.fetch(ci->iid, contig);
        vector<Tile_t> & tiling = contig.getReadTiling();

        sort(tiling.begin(), tiling.end(), TileOrderCmp());

        int tilesize = tiling.size();
        int tilewritepos = 0;
        int readsdeleted = 0;

        for (int i = 0; i < tilesize; i++)
        {
          if (iidstoremove.find(tiling[i].source) == iidstoremove.end())
          {
            if (i != tilewritepos)
            {
              tiling[tilewritepos] = tiling[i];
            }

            tilewritepos++;
          }
          else
          {
            readsdeleted++;
          }
        }

        if (readsdeleted)
        {
          cout << "Deleted " << readsdeleted << " reads from contig e" << contig.getEID() << endl;
          tiling.resize(tilewritepos);
          ctg_bank.replace(contig.getIID(), contig);
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
