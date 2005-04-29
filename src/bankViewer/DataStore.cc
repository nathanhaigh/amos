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
    scaffold_bank(Scaffold_t::NCODE),
    edge_bank(ContigEdge_t::NCODE),
    link_bank(ContigLink_t::NCODE)
{
  m_contigId = 0;
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

    indexContigs();

    m_contigId = 0;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    retval = 1;
  }

  try
  {
    scaffold_bank.open(bankname, B_SPY);
    indexScaffolds();
  }
  catch (Exception_t & e)
  {
    cerr << "Scaffold information not available" << endl;

  }

  try
  {
    mate_bank.open(bankname, B_SPY);
    frag_bank.open(bankname, B_SPY);
    lib_bank.open(bankname,  B_SPY);

    indexLibraries();
    indexMates();

    indexFrags();
    indexReads();
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

void DataStore::indexReads()
{
  m_readfraglookup.clear();
  m_readfraglookup.resize(read_bank.getSize());

  cerr << "Indexing reads... ";

  Read_t red;
  read_bank.seekg(1);

  while (read_bank >> red)
  {
    m_readfraglookup.insert(make_pair(red.getIID(), red.getFragment()));
  }

  cerr << m_readfraglookup.size() << " reads" << endl;
}

void DataStore::indexFrags()
{
  m_fragliblookup.clear();
  m_fragliblookup.resize(frag_bank.getSize());

  cerr << "Indexing frags... ";

  Fragment_t frg;
  frag_bank.seekg(1);

  while (frag_bank >> frg)
  {
    m_fragliblookup.insert(make_pair(frg.getIID(), frg.getLibrary()));
  }

  cerr << m_fragliblookup.size() << " fragments" << endl;
}

void DataStore::indexLibraries()
{
  m_libdistributionlookup.clear();
  m_libdistributionlookup.resize(lib_bank.getSize());

  cerr << "Indexing libraries... ";

  Library_t lib;
  lib_bank.seekg(1);

  while (lib_bank >> lib)
  {
    m_libdistributionlookup.insert(make_pair(lib.getIID(), lib.getDistribution()));
  }

  cerr << m_libdistributionlookup.size() << " libraries" << endl;
}

void DataStore::indexMates()
{
  m_readmatelookup.clear();
  m_readmatelookup.resize(mate_bank.getSize() * 2);

  cerr << "Indexing mates... ";

  mate_bank.seekg(1);
  Matepair_t mates;
  while (mate_bank >> mates)
  {
    m_readmatelookup.insert(make_pair(mates.getReads().first, pair<AMOS::ID_t, AMOS::MateType_t> (mates.getReads().second, mates.getType())));
    m_readmatelookup.insert(make_pair(mates.getReads().second, pair<AMOS::ID_t, AMOS::MateType_t> (mates.getReads().first,  mates.getType())));
  }

  cerr << m_readmatelookup.size() << " mated reads" << endl;
}

void DataStore::indexContigs()
{
  cerr << "Indexing contigs... ";
  m_readcontiglookup.clear();
  m_readcontiglookup.resize(read_bank.getSize());

  int contigid = 1;
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
      m_readcontiglookup.insert(make_pair(ti->source, contigid));
    }

    contigid = contig_bank.tellg();
  }

  cerr <<  m_readcontiglookup.size() << " reads in " << contigid-1 << " contigs" << endl;
}

void DataStore::indexScaffolds()
{
  cerr << "Indexing scaffolds... ";
  m_contigscafflookup.clear();
  m_contigscafflookup.resize(contig_bank.getSize());

  scaffold_bank.seekg(1);
  int scaffid = 1;

  Scaffold_t scaffold;
  while (scaffold_bank >> scaffold)
  {
    vector<Tile_t> & tiling = scaffold.getContigTiling();
    vector<Tile_t>::const_iterator ti;

    for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    {
      m_contigscafflookup.insert(make_pair(ti->source, scaffid));
    }

    scaffid = scaffold_bank.tellg();
  }

  cerr <<  m_contigscafflookup.size() << " contigs in " << scaffid-1 << " scaffolds" << endl;
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
      fetchContig(bankid, m_contig);
      m_scaffoldId = lookupScaffoldId(id);
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

void DataStore::fetchRead(ID_t readid, Read_t & read)
{
  read_bank.seekg(read_bank.getIDMap().lookupBID(readid));
  read_bank >> read;
}

void DataStore::fetchContig(ID_t contigid, Contig_t & contig)
{
  contig_bank.seekg(contigid);
  contig_bank >> contig;
}

void DataStore::fetchScaffold(ID_t scaffid, Scaffold_t & scaff)
{
  scaffold_bank.seekg(scaffid);
  scaffold_bank >> scaff;
}

void DataStore::fetchFrag(ID_t fragid, Fragment_t & frag)
{
  frag_bank.seekg(frag_bank.getIDMap().lookupBID(fragid));
  frag_bank >> frag;
}

AMOS::ID_t DataStore::getLibrary(ID_t readid)
{
  if (!m_libdistributionlookup.empty())
  {
    try
    {
      ID_t fragid, libid;

      if (!m_readfraglookup.empty())
      {
        fragid = m_readfraglookup[readid];
      }
      else
      {
        Read_t read;
        fetchRead(readid, read);
        fragid = read.getFragment();
      }

      if (!m_fragliblookup.empty())
      {
        libid = m_fragliblookup[fragid];
      }
      else
      {
        Fragment_t frag;
        fetchFrag(fragid, frag);
        libid = frag.getLibrary();
      }

      return libid;
    }
    catch (Exception_t & e)
    {
    }
  }

  return AMOS::NULL_ID;
}

Distribution_t DataStore::getLibrarySize(ID_t libid)
{
  if (libid)
  {
    return m_libdistributionlookup[libid];
  }

  return Distribution_t();
}

ID_t DataStore::lookupContigId(ID_t readid)
{
  IdLookup_t::iterator i = m_readcontiglookup.find(readid);

  if (i == m_readcontiglookup.end())
  {
    return AMOS::NULL_ID;
  }
  else
  {
    return i->second;
  }
}

ID_t DataStore::lookupScaffoldId(ID_t contigid)
{
  IdLookup_t::iterator i = m_contigscafflookup.find(contigid);

  if (i == m_contigscafflookup.end())
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
