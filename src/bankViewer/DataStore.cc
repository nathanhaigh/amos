#include "DataStore.hh"

using namespace AMOS;

DataStore::DataStore()
  : contig_bank(Contig_t::NCODE),
    read_bank(Read_t::NCODE),
    frag_bank(Fragment_t::NCODE),
    lib_bank(Library_t::NCODE),
    mate_bank(Matepair_t::NCODE),
    edge_bank(ContigEdge_t::NCODE),
    link_bank(ContigLink_t::NCODE)
{
  m_contigId = -1;
  m_loaded = false;

}

int DataStore::openBank(const string & bankname)
{
  int retval = 0;

  try
  {
    read_bank.open(bankname,   B_SPY);
    contig_bank.open(bankname, B_SPY);

    m_bankname = bankname;

    loadContigs();

    m_contigId = -1;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }

  try
  {
    mate_bank.open(bankname, B_SPY);
    frag_bank.open(bankname, B_SPY);
    lib_bank.open(bankname,  B_SPY);
    loadMates();
    loadLibraries();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    cerr << "Mates not available\n";
  }

  try
  {
    edge_bank.open(bankname, B_SPY);
    link_bank.open(bankname, B_SPY);
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    cerr << "Contig Graph not available\n";
  }

  return retval;
}

void DataStore::loadLibraries()
{
  m_libdistributionlookup.clear();

  Library_t lib;
  while (lib_bank >> lib)
  {
    m_libdistributionlookup[lib.getIID()] = lib.getDistribution();
  }

  cerr << "Loaded " << m_libdistributionlookup.size() << " libraries" << endl;
}

void DataStore::loadMates()
{
  m_readmatelookup.clear();

  mate_bank.seekg(1);
  Matepair_t mates;
  while (mate_bank >> mates)
  {
    m_readmatelookup[mates.getReads().first] = mates.getReads().second;
    m_readmatelookup[mates.getReads().second] = mates.getReads().first;
  }

  cerr << "Loaded mates for " << m_readmatelookup.size() << " reads" << endl;
}

void DataStore::loadContigs()
{
  cerr << "Loading contigs\n";
  m_readcontiglookup.clear();

  contig_bank.seekg(1);

  Contig_t contig;
  while (contig_bank >> contig)
  {
    vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::const_iterator ti;

    for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    {
      m_readcontiglookup[ti->source] = contig.getIID();
    }
  }

  cerr << "Loaded contig id mapping for " << m_readcontiglookup.size() << " reads" << endl;
}

int DataStore::setContigId(int id)
{
  int retval = 0;

  try
  {
    ID_t bankid = contig_bank.getIDMap().lookupBID(id);
    contig_bank.seekg(bankid);
    contig_bank >> m_contig;
    m_contigId = id;
    m_loaded = true;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }

  return retval;
}

Distribution_t DataStore::getLibrarySize(ID_t readid)
{
  try
  {
    Read_t read;
    read_bank.fetch(readid, read);

    Fragment_t frag;
    frag_bank.fetch(read.getFragment(), frag);

    return m_libdistributionlookup[frag.getLibrary()];
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  return Distribution_t();
}

ID_t DataStore::lookupContigId(ID_t readid)
{
  map<ID_t, ID_t>::iterator i = m_readcontiglookup.find(readid);

  if (i == m_readcontiglookup.end())
  {
    return AMOS::NULL_ID;
  }
  else
  {
    return i->second;
  }
}
