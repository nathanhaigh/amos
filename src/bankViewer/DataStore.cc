#include "DataStore.hh"
#include <sys/types.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>


using namespace AMOS;
using namespace std;

DataStore::DataStore()
  : contig_bank(Contig_t::NCODE),
    read_bank(Read_t::NCODE),
    frag_bank(Fragment_t::NCODE),
    lib_bank(Library_t::NCODE),
    scaffold_bank(Scaffold_t::NCODE),
    edge_bank(ContigEdge_t::NCODE),
    link_bank(ContigLink_t::NCODE),
    feat_bank(Feature_t::NCODE)
{
  m_loaded = false;
  m_contigId = AMOS::NULL_ID;
  m_scaffoldId = AMOS::NULL_ID;

  m_tracepaths.push_back("/local/chromo/Chromatograms/%TRACEDB%/ABISSed/%EID3%/%EID4%/%EID5%/");
  m_tracepaths.push_back("/local/chromo2/Chromatograms/%TRACEDB%/ABISSed/%EID3%/%EID4%/%EID5%/");
  m_tracepaths.push_back("/local/chromo3/Chromatograms/%TRACEDB%/ABISSed/%EID3%/%EID4%/%EID5%/");

  m_tracepaths.push_back("/fs/szasmg/Chromatograms/%TRACEDB%/");

  m_tracecache        = ".tracecache";
  m_tracecachecreated = 0;
  m_tracecmd          = "curl \"http://www.ncbi.nlm.nih.gov/Traces/trace.fcgi?cmd=java&j=scf&val=%EID%&ti=%EID%\" -s -o %TRACECACHE%/%EID%";
  m_tracecmdpath      = "%TRACECACHE%/%EID%";
  m_tracecmdenabled   = 0;
}

DataStore::~DataStore()
{
  if (m_tracecachecreated)
  {
    cerr << "Cleaning tracecache directory: " << m_tracecache << endl;
    string cmd = "/bin/rm -rf " + m_tracecache;
    system(cmd.c_str());
  }
}

int DataStore::openBank(const string & bankname)
{
  int retval = 0;

  try
  {
    read_bank.open(bankname,   B_SPY);
    contig_bank.open(bankname, B_SPY);

    m_bankname = bankname;
    m_contigId = AMOS::NULL_ID;
    m_scaffoldId = AMOS::NULL_ID;

    m_readmatelookup.clear();
    m_readcontiglookup.clear();
    m_fragliblookup.clear();
    m_readfraglookup.clear();
    m_contigscafflookup.clear();
    m_libdistributionlookup.clear();

    indexContigs();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in openBank():\n" << e;
    retval = 1;
  }

  if (!retval)
  {
    m_scaffoldId = AMOS::NULL_ID;

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
      frag_bank.open(bankname, B_SPY);
      lib_bank.open(bankname,  B_SPY);

      indexLibraries();
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

    try
    {
      feat_bank.open(bankname, B_SPY);
    }
    catch (const Exception_t & e)
    {
      cerr << "Features not available" << endl;
    }
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

  m_readmatelookup.clear();
  m_readmatelookup.resize(frag_bank.getSize() * 2);

  cerr << "Indexing frags... ";

  Fragment_t frg;
  frag_bank.seekg(1);

  while (frag_bank >> frg)
  {
    m_fragliblookup.insert(make_pair(frg.getIID(), frg.getLibrary()));

    std::pair<ID_t, ID_t> mates = frg.getMatePair();
    m_readmatelookup.insert(make_pair(mates.first,  pair<AMOS::ID_t, AMOS::FragmentType_t> (mates.second, frg.getType())));
    m_readmatelookup.insert(make_pair(mates.second, pair<AMOS::ID_t, AMOS::FragmentType_t> (mates.first,  frg.getType())));
  }

  cerr << m_fragliblookup.size() << " fragments ["
       << m_readmatelookup.size() << " mated reads]" << endl;
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
    cerr << "ERROR in setContigId()\n" << e;
    retval = 1;
  }

  return retval;
}

void DataStore::fetchRead(ID_t readid, Read_t & read)
{
  ID_t bid = 0;
  try
  {
    bid = read_bank.getIDMap().lookupBID(readid);
    read_bank.seekg(bid);
    read_bank >> read;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: Can't fetch read iid:" << readid << " bid: " << bid << "\n" << e;
  }
}

void DataStore::fetchContig(ID_t contigid, Contig_t & contig)
{
  try
  {
    contig_bank.seekg(contigid);
    contig_bank >> contig;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in fetchContig()\n" << e;
  }
}

void DataStore::fetchScaffold(ID_t scaffid, Scaffold_t & scaff)
{
  try
  {
    scaffold_bank.seekg(scaffid);
    scaffold_bank >> scaff;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in fetchScaffold()\n" << e;
  }
}

void DataStore::fetchFrag(ID_t fragid, Fragment_t & frag)
{
  try
  {
    frag_bank.seekg(frag_bank.getIDMap().lookupBID(fragid));
    frag_bank >> frag;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in fetchFrag()\n" << e;
  }
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


static void replaceAll(string & str,
                       const string & token,
                       const string & value)
{
  int startpos;
  int endpos;

  string searchtoken = "%" + token;

  while ((startpos = str.find(searchtoken)) != str.npos)
  {
    string replaceval;
    endpos = startpos+searchtoken.length();

    if (str[endpos] == '%')
    {
      replaceval = value;
    }
    else
    {
      // Figure out how long of a prefix of value to replace
      int len = atoi(str.c_str()+endpos);
      if (len > value.length()) { len = value.length(); }

      // grab the first len characters of value
      replaceval = value.substr(0, len);

      // find the end token
      while (str[endpos] != '%' && (endpos < str.length()))
      {
        endpos++;
      }
    }

    str.replace(startpos, endpos-startpos+1, replaceval);
  }
}

string DataStore::replaceTraceTokens(const string & str,
                                const string & eid,
                                const string & iid)
{
  string result(str);

  replaceAll(result, "TRACECACHE", m_tracecache);
  replaceAll(result, "TRACEDB",    m_tracedb);
  replaceAll(result, "EID",  eid);
  replaceAll(result, "IID",  iid);

  return result;
}


AMOS::ID_t DataStore::getPersistantRead(AMOS::ID_t readiid, int errorrate)
{
  string eid = read_bank.lookupEID(readiid);
  eid = eid.substr(0, eid.find_first_of("_")+1);

  char buffer[16];
  sprintf(buffer, "%04d", errorrate);
  eid += buffer;

  ID_t newiid = read_bank.lookupIID(eid);
  return newiid;
}




extern "C"
{
  #include "Read.h"
}

char * DataStore::fetchTrace(const AMOS::Read_t & read, 
                             std::vector<int16_t> & positions )
{
  string eid = read.getEID();

  char iidarr[16];
  sprintf(iidarr, "%d", read.getIID());
  string iid(iidarr);

  Read * trace = NULL;

  string path;

  vector <string>::iterator ci;
  for (ci =  m_tracepaths.begin();
       ci != m_tracepaths.end() && !trace;
       ci++)
  {
    path = replaceTraceTokens(*ci, eid, iid);

    DIR * dir = opendir(path.c_str());
    if (dir)
    {
      closedir(dir);
      path += "/" + eid;
      trace = read_reading((char *)path.c_str(), TT_ANY);
    }
  }

  if (!trace && m_tracecmdenabled)
  {
    if (!m_tracecachecreated)
    {
      int retval = mkdir(m_tracecache.c_str(), 
                         S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH );
      m_tracecachecreated = !retval;
    }

    string cmd = replaceTraceTokens(m_tracecmd, eid, iid);
    path = replaceTraceTokens(m_tracecmdpath, eid, iid);

    cerr << "**** Executing: \"" << cmd << "\"" << endl;

    int retval = system(cmd.c_str());

    if (!retval)
    {
      trace = read_reading((char *)path.c_str(), TT_ANY);
    }
  }

  // Load positions out of trace
  if (trace && positions.empty() && trace->basePos)
  {
    for (int i = 0; i < trace->NBases; i++)
    {
      positions.push_back(trace->basePos[i]);
    }
  }

  return (char *) trace;
}


