#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"


using namespace AMOS;
using namespace std;

HASHMAP::hash_map<ID_t, ID_t> idmap;

ID_t getNewIID(ID_t oldiid)
{
  HASHMAP::hash_map<ID_t, ID_t>::const_iterator idmapiter;
  idmapiter = idmap.find(oldiid);
  if (idmapiter != idmap.end())
  {
    return idmapiter->second;
  }

  return oldiid;
}



int main(int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: renameReads bankname oldiid.newiid" << endl;
    cerr << "Reassigns the iid to reads, updating RED, CTG, and FRG messages"
         << "and writes back to the bank" << endl;

    return EXIT_FAILURE;
  }

  Bank_t contig_bank(Contig_t::NCODE);
  Bank_t read_bank(Read_t::NCODE);
  Bank_t fragment_bank(Fragment_t::NCODE);

  string bank_name = argv[1];
  string newidsfile = argv[2];

  cerr << "Loading bank " << bank_name << " at " << Date() << endl;

  try
  {
    ifstream file;

    file.open(newidsfile.c_str());
    if (!file)
    {
      throw Exception_t("Couldn't open id map file", __LINE__, __FILE__);
    }


    contig_bank.open(bank_name, B_READ|B_WRITE);
    read_bank.open(bank_name, B_READ|B_WRITE);
    fragment_bank.open(bank_name, B_READ|B_WRITE);

    cerr << "Loading iid translation data... ";
    ID_t oldiid, newiid;
    while (file >> oldiid >> newiid)
    {
      idmap.insert(make_pair(oldiid, newiid));
    }
    cerr << idmap.size() << " translations loaded" << endl;


    cerr << "Processing reads... ";
    AMOS::IDMap_t::const_iterator ri;
    for (ri = read_bank.getIDMap().begin();
         ri;
         ri++)
    {
      read_bank.assignIID(ri->eid, getNewIID(ri->iid));
    }
    cerr << read_bank.getIDMapSize() << " reads processed." << endl;


    cerr << "Processing fragments... ";
    Fragment_t frag;
    AMOS::IDMap_t::const_iterator fi;
    for (fi = fragment_bank.getIDMap().begin();
         fi;
         fi++)
    {
      fragment_bank.fetch(fi->iid, frag);
      pair<ID_t, ID_t> mtp = frag.getMatePair();
      frag.setReads(make_pair(getNewIID(mtp.first), getNewIID(mtp.second)));
      fragment_bank.replace(fi->iid, frag);
    }
    cerr << fragment_bank.getIDMapSize() << " fragments processed." << endl;


    cerr << "Processing contigs... ";
    Contig_t contig;
    AMOS::IDMap_t::const_iterator ci;
    for (ci = contig_bank.getIDMap().begin();
         ci;
         ci++)
    {
      contig_bank.fetch(ci->iid, contig);
      vector<Tile_t> tiling = contig.getReadTiling();
      vector<Tile_t>::iterator ti;
      for (ti = tiling.begin();
           ti != tiling.end();
           ti++)
      {
        ti->source = getNewIID(ti->source);
      }
      contig_bank.replace(contig.getIID(), contig);
    }
    cerr << contig_bank.getIDMapSize() << " contigs." << endl;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}

