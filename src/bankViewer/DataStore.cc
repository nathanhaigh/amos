#include "DataStore.hh"
#include <sys/types.h>
#include <dirent.h>

extern "C"
{
  #include "Read.h"
}


using namespace AMOS;
using namespace std;

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

  m_chromodbs.push_back("/local/chromo/Chromatograms/");
  m_chromodbs.push_back("/local/chromo2/Chromatograms/");
  m_chromodbs.push_back("/local/chromo3/Chromatograms/");
  m_chromodbs.push_back("/local/asmg/scratch/mschatz/Chromatograms/");

//  m_chromopaths.push_back("/home/mschatz/build/sample/32774/chromo");
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
    cerr << "Mates not available\n";
  }

  try
  {
    edge_bank.open(bankname, B_SPY);
    link_bank.open(bankname, B_SPY);
  }
  catch (Exception_t & e)
  {
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

  cerr << "Loading mates... ";

  mate_bank.seekg(1);
  Matepair_t mates;
  while (mate_bank >> mates)
  {
    m_readmatelookup[mates.getReads().first]  = pair<AMOS::ID_t, AMOS::MateType_t> (mates.getReads().second, mates.getType());
    m_readmatelookup[mates.getReads().second] = pair<AMOS::ID_t, AMOS::MateType_t> (mates.getReads().first,  mates.getType());
  }

  cerr << m_readmatelookup.size() << " mated reads" << endl;
}

void DataStore::loadContigs()
{
  cerr << "Indexing contigs... ";
  m_readcontiglookup.clear();

  int contigid = 1;
  contig_bank.seekg(contigid);

  Contig_t contig;
  while (contig_bank >> contig)
  {
    vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::const_iterator ti;

    for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    {
      m_readcontiglookup[ti->source] = contigid;
    }

    contigid++;
  }

  cerr <<  m_readcontiglookup.size() << " reads in " << contigid-1 << " contigs" << endl;
}

int DataStore::setContigId(int id)
{
  int retval = 0;

  try
  {
    ID_t bankid = id;
    cerr << "Setting contig to bid " << bankid << endl;

    if (bankid != 0)
    {
      contig_bank.seekg(bankid);
      contig_bank >> m_contig;
      m_contigId = id;
      m_loaded = true;
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }

  return retval;
}

AMOS::ID_t DataStore::getLibrary(ID_t readid)
{
  if (!m_libdistributionlookup.empty())
  {
    try
    {
      Read_t read;
      read_bank.fetch(readid, read);

      Fragment_t frag;
      frag_bank.fetch(read.getFragment(), frag);

      return frag.getLibrary();
    }
    catch (Exception_t & e)
    {
    }
  }

  return AMOS::NULL_ID;
}

Distribution_t DataStore::getLibrarySize(ID_t readid)
{
  if (!m_libdistributionlookup.empty())
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
    }
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

static string chromodbpath(const string & base,
                    const string & db,
                    const string & readname)
{
  string path = base + db;
  path += (string)"/ABISSed/" +
          readname[0]+readname[1]+readname[2] + "/" +
          readname[0]+readname[1]+readname[2]+readname[3] + "/" +
          readname[0]+readname[1]+readname[2]+readname[3]+readname[4]+ "/";

  return path;        
}


void * DataStore::fetchTrace(const AMOS::Read_t & read)
{
  string readname = read.getEID();

  Read * trace = NULL;

  string path;

  vector <string>::iterator ci;
  for (ci =  m_chromopaths.begin();
       ci != m_chromopaths.end() && !trace;
       ci++)
  {
    path = *ci + "/";
    path += readname;
    trace = read_reading((char *)path.c_str(), TT_ANY);
  }


  for (ci =  m_chromodbs.begin();
       ci != m_chromodbs.end() && !trace;
       ci++)
  {
    path = chromodbpath(*ci, m_db, readname);
    if (DIR * dir = opendir(path.c_str()))
    {
      closedir(dir);
      path += readname;
      trace = read_reading((char *)path.c_str(), TT_ANY);
    }
  }

  return (void *) trace;
}
