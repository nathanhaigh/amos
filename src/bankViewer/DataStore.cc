#include "DataStore.hh"
#include "Insert.hh"
#include <sys/types.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "amp.hh"
#include "delcher.hh"
#include "fasta.hh"


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
  m_tracepaths.push_back("chromat_dir");
  m_tracepaths.push_back("../chromat_dir");

  m_tracecache        = ".tracecache";
  m_tracecachecreated = 0;
  m_tracecmd          = "curl \"http://www.ncbi.nlm.nih.gov/Traces/trace.fcgi?cmd=java&j=scf&val=%EID%&ti=%EID%\" -s -o %TRACECACHE%/%EID%";
  m_tracecmdpath      = "%TRACECACHE%/%EID%";
  m_tracecmdenabled   = 0;
  Kmer_Len = 0;
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
  int retval = 1;

  try
  {
    cerr << "Opening " << bankname << endl;
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
    retval = 0;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in openBank():\n" << e;
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


typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;


void DataStore::calculateInserts(vector<Tile_t> & tiling,
                                 vector<Insert *> & inserts,
                                 bool connectMates,
                                 int verbose)
{
  if (verbose) { cerr << "Loading mates" << endl; }

  SeqTileMap_t seqtileLookup;

  int mated = 0;
  int unmated = 0;
  int matelisted = 0;
  
  int happycount = 0;
  int unhappycount = 0;

  vector<Tile_t>::iterator ti;

  // map iid -> tile * (within this contig)
  for (ti =  tiling.begin();
       ti != tiling.end();
       ti++)
  {
    seqtileLookup[ti->source] = &(*ti);
  }


  Insert * insert;
  MateLookupMap::iterator mi;

  ProgressDots_t dots(seqtileLookup.size(), 50);
  int count = 0;

  // For each read in the contig
  SeqTileMap_t::iterator ai;
  for (ai =  seqtileLookup.begin();
       ai != seqtileLookup.end();
       ai++)
  {
    count++;
    if (verbose) { dots.update(count); }

    if (ai->second == NULL)
    {
      //cerr << "Skipping already seen read" << endl;
      continue;
    }

    ID_t aid = ai->first;
    ID_t acontig = lookupContigId(aid);
    Tile_t * atile = ai->second;

    AMOS::ID_t libid = getLibrary(aid);
    AMOS::Distribution_t dist = getLibrarySize(libid);

    // Does it have a mate
    mi = m_readmatelookup.find(aid);
    if (mi == m_readmatelookup.end())
    {
      unmated++;
      insert = new Insert(aid, acontig, atile,
                          AMOS::NULL_ID, AMOS::NULL_ID, NULL,
                          libid, dist,
                          AMOS::Fragment_t::NULL_FRAGMENT);
    }
    else
    {
      matelisted++;

      ID_t bid = mi->second.first;
      ID_t bcontig = AMOS::NULL_ID;
      Tile_t * btile = NULL;
      bcontig = lookupContigId(bid);

      SeqTileMap_t::iterator bi = seqtileLookup.find(bid);

      if (bi != seqtileLookup.end())
      {
        mated++;

        btile = bi->second;
        bi->second = NULL;
      }

      insert = new Insert(aid, acontig, atile, 
                          bid, bcontig, btile, 
                          libid, dist, 
                          mi->second.second);

      if (insert->m_state == 'H')
      {
        happycount++;
      }
      else
      {
        unhappycount++;
      }


      if (connectMates && insert->reasonablyConnected())
      {
        // A and B are within this contig, and should be drawn together
        insert->m_active = 2;
      }
      else if (btile)
      {
        // A and B are within this contig, but not reasonably connected
        //Insert * j = new Insert(*insert);

        Insert * j = new Insert(aid, acontig, atile,
                                bid, bcontig, btile,
                                libid, dist,
                                mi->second.second);
        j->setActive(1, insert, connectMates);
        inserts.push_back(j);

        insert->setActive(0, j, connectMates);
      }
      else 
      { 
        // Just A is valid
        insert->setActive(0, NULL, connectMates); 
      }
    }

    inserts.push_back(insert);

    // Mark that this read has been taken care of already
    ai->second = NULL;
  }

  sort(inserts.begin(), inserts.end(), Insert::TilingOrderCmp());

  if (verbose)
  {
    cerr << endl;

    cerr << "mated: "   << mated 
         << " matelisted: " << matelisted
         << " unmated: " << unmated
         << " happy: " << happycount
         << " unhappy: " << unhappycount << endl;
  }
}


static unsigned  Char_To_Binary (char ch)
//  Return the binary equivalent of  ch .
  {
   switch  (tolower (ch))
     {
      case  'a' :
      case  'n' :
        return  0;
      case  'c' :
        return  1;
      case  'g' :
        return  2;
      case  't' :
        return  3;
      default :
        return 0;
     }
   return  0;
  }




char BinaryToAscii(char b)
{
  switch(b)
  {
    case 0: return 'A';
    case 1: return 'C';
    case 2: return 'G';
    case 3: return 'T';
  }

  return '*';
}

void DataStore::MerToAscii(Mer_t mer, string & s)
{
  s.erase();

  for (int i = 0; i < Kmer_Len; i++)
  {
    char a = BinaryToAscii(mer & 0x3);
    mer >>= 2;

    s.append(1, a);
  }

  reverse(s.begin(), s.end());
}


void DataStore::Forward_Add_Ch (DataStore::Mer_t & mer, char ch)

//  Add  ch  to  mer  on the right, sliding one character
//  off the left end of  mer .

  {
   mer &= Forward_Mask;
   mer <<= 2;
   mer |= Char_To_Binary (ch);

   return;
  }

void  DataStore::Reverse_Add_Ch (DataStore::Mer_t & mer, char ch)

//  Add the Watson-Crick complement of  ch  to  mer  on the left,
//  sliding one character off the right end of  mer .

  {
   mer >>= 2;
   mer |= ((long long unsigned) (3 ^ Char_To_Binary (ch)) << (2 * Kmer_Len - 2));

   return;
  }

void DataStore::Fasta_To_Binary (const string & s, Mer_t & mer)

//  Convert string  s  to its binary equivalent in  mer .

  {
   int  i, n;

   n = s . length ();
   mer = 0;
   for  (i = 0;  i < n;  i ++)
     {
      mer <<= 2;
      mer |= Char_To_Binary (s [i]);
     }

   return;
  }



void  DataStore::Read_Mers (const char * fname)

//  Read kmers from file name  fname  and save them
//  in binary form in  mer_list .  Input format is
//  a multi-fasta file.  Mers are assumed to contain only
//  ACGT's

{
  FILE  * fp;
  string  s, tag;
  Mer_t  mer;

  cerr << "Loading mers... ";

  fp = File_Open (fname, "r", __FILE__, __LINE__);

  mer_table . clear ();

  while  (Fasta_Read (fp, s, tag))
  {
    unsigned short mercount = atoi(tag.c_str());

    if  (Kmer_Len == 0)
    {
      Kmer_Len = s . length ();
    }
    else if  (Kmer_Len != int (s . length ()))
    {
      cerr << "New kmer " << s << " has length " << s.length() << " instead of " << Kmer_Len << endl;
      throw "Error!";
    }
    Fasta_To_Binary (s, mer);

 //  MerToAscii(mer, tag);
 //   fprintf(stderr, "orig: %s mer: %032llx asc: %s\n", s.c_str(), mer, tag.c_str());

    mer_table.insert(make_pair(mer, mercount));
   }

   cerr << mer_table.size() << " mers loaded." << endl;

   Forward_Mask = ((long long unsigned) 1 << (2 * Kmer_Len - 2)) - 1;

   return;
}

unsigned int DataStore::getMerCoverage(Mer_t fwd_mer, Mer_t rev_mer)
{
  unsigned int fcount = 0;
  unsigned int rcount = 0;

  MerTable_t::const_iterator fi = mer_table.find(fwd_mer);
  MerTable_t::const_iterator ri = mer_table.find(rev_mer);

  if (fi != mer_table.end()) { fcount = fi->second; }
  if (ri != mer_table.end()) { rcount = ri->second; }

  unsigned int mcount = (fcount > rcount) ? fcount : rcount;


  return mcount;
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


