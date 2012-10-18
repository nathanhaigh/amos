#include "DataStore.hh"
#include "Insert.hh"
#include <sys/types.h>
#include <sys/time.h>
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

  Kmer_Len = 0;
}

DataStore::~DataStore()
{

}

int DataStore::openBank(const string & bankname)
{
  int retval = 1;

  try
  {
    cerr << "Opening " << bankname << "... ";

    EventTime_t timer;

    read_bank.open(bankname,   B_SPY);
    contig_bank.open(bankname, B_SPY);

    cerr << timer.str() << endl;

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
    m_scaffoldId = AMOS::NULL_ID;
    retval = 0;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in openBank():\n" << e;
  }

  if (!retval)
  {
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

      int BIGMEM = 1;

      if (BIGMEM)
      {
        indexFrags();
        indexReads();
      }
    }
    catch (Exception_t & e)
    {
      cerr << "Mates not available:" << e << endl;
    }

    if (0)
    {
      try
      {
        edge_bank.open(bankname, B_SPY);
        link_bank.open(bankname, B_SPY);
      }
      catch (Exception_t & e)
      {
        cerr << "Contig Graph not available\n";
      }
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
  cerr << "Indexing Reads     ";
  ProgressDots_t dots(read_bank.getSize(), 10);
  int count = 0;

  EventTime_t timer;

  m_readfraglookup.clear();
  m_readfraglookup.resize(read_bank.getMaxBID()+1);

  Read_t red;

  read_bank.seekg(1);
  read_bank.setFixedStoreOnly(true);

  while (read_bank >> red)
  {
    count++;
    dots.update(count);

    m_readfraglookup[read_bank.lookupBID(red.getIID())] = red.getFragment();
  }

  read_bank.setFixedStoreOnly(false);

  cerr << " " << timer.str() << " "
       << m_readfraglookup.size() << " reads" << endl;
}

void DataStore::indexFrags()
{
  cerr << "Indexing Mates     ";
  ProgressDots_t dots(frag_bank.getSize(), 10);

  EventTime_t timer;

  m_fragliblookup.clear();
  m_fragliblookup.resize(frag_bank.getMaxBID()+1);

  m_readmatelookup.clear();
  m_readmatelookup.resize(read_bank.getMaxBID()+1);


  Fragment_t frg;
  frag_bank.seekg(1);
  int fragments = 0;
  int reads = 0;

  frag_bank.setFixedStoreOnly(true);

  while (frag_bank >> frg)
  {
    fragments++;
    dots.update(fragments);

    m_fragliblookup[frag_bank.lookupBID(frg.getIID())] = frg.getLibrary();

    std::pair<ID_t, ID_t> mates = frg.getMatePair();
    if (mates.first)
    {
      m_readmatelookup[read_bank.lookupBID(mates.first)]  = make_pair(mates.second, frg.getType());
      reads++;
    }

    if (mates.second)
    {
      m_readmatelookup[read_bank.lookupBID(mates.second)] = make_pair(mates.first,  frg.getType());
      reads++;
    }
  }

  frag_bank.setFixedStoreOnly(false);

  cerr << " " << timer.str() << " "
       << reads << " mated reads in "
       << fragments << " fragments" << endl;
}

DataStore::MateInfo_t DataStore::getMatePair(ID_t readid)
{
  if (!m_readmatelookup.empty())
  {
    return m_readmatelookup[read_bank.lookupBID(readid)];
  }
  else if (frag_bank.isOpen())
  {
    Read_t read;
    read_bank.fetchFix(readid, read);

    if (read.getFragment() != 0);
    {
      Fragment_t frag;
      frag_bank.fetchFix(read.getFragment(), frag);

      if (frag.getMatePair().first == readid)
      {
        return make_pair(frag.getMatePair().second, frag.getType());
      }
      else if (frag.getMatePair().second == readid)
      {
        return make_pair(frag.getMatePair().first,  frag.getType());
      }
    }
  }

  return make_pair(0,0);
}

void DataStore::indexLibraries()
{
  cerr << "Indexing Libraries ";
  ProgressDots_t dots(lib_bank.getSize(), 10);

  EventTime_t timer;

  m_libdistributionlookup.clear();
  m_libdistributionlookup.resize(lib_bank.getSize());

  Library_t lib;
  lib_bank.seekg(1);

  int count = 0;
  while (lib_bank >> lib)
  {
    m_libdistributionlookup.insert(make_pair(lib.getIID(), lib.getDistribution()));

    count++;
    dots.update(count);
  }

  cerr << " " << timer.str() << " "
       << m_libdistributionlookup.size() << " libraries" << endl;
}

void DataStore::indexContigs()
{
  cerr << "Indexing Contigs   ";
  ProgressDots_t dots(contig_bank.getSize(), 10);

  EventTime_t timer;

  m_readcontiglookup.clear();
  m_readcontiglookup.resize(read_bank.getMaxBID()+1, 0);

  int contigs = 0;
  int reads = 0;
  contig_bank.seekg(1);
  Contig_t contig;
  while (contig_bank >> contig)
  {
    int contigid = contig_bank.tellg() - 1;
    contigs++;
    dots.update(contigs);

    vector<Tile_t> & tiling = contig.getReadTiling();
    vector<Tile_t>::const_iterator ti;
    for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    {
      m_readcontiglookup[read_bank.lookupBID(ti->source)] = contigid;
      reads++;
    }
  }

  cerr << " " << timer.str() << " " 
       << reads << " reads in " 
       << contigs << " contigs" << endl;
}

void DataStore::indexScaffolds()
{
  cerr << "Indexing Scaffolds ";

  ProgressDots_t dots(scaffold_bank.getSize(), 10);

  EventTime_t timer;

  m_contigscafflookup.clear();
  m_contigscafflookup.resize(contig_bank.getMaxBID()+1);

  int scaffolds = 0;
  int contigs = 0;

  scaffold_bank.seekg(1);
  Scaffold_t scaffold;
  while (scaffold_bank >> scaffold)
  {
    int scaffid = scaffold_bank.tellg() - 1;
    scaffolds++;
    dots.update(scaffolds);

    vector<Tile_t> & tiling = scaffold.getContigTiling();
    vector<Tile_t>::const_iterator ti;

    for (ti =  tiling.begin();
         ti != tiling.end();
         ti++)
    {
      contigs++;
      m_contigscafflookup[contig_bank.lookupBID(ti->source)] = scaffid;
    }
  }

  cerr << " " << timer.str() << " "
       << contigs << " contigs in " 
       << scaffolds << " scaffolds" << endl;
}

int DataStore::setContigId(int bid)
{
  int retval = 0;

  try
  {
    if (bid != 0)
    {
      EventTime_t timer;
      cerr << "Loading Contig " << bid << "...";

      fetchContigBID(bid, m_contig);
      m_scaffoldId = lookupScaffoldId(bid);
      m_contigId = bid;
      m_loaded = true;

      cerr << " " << timer.str() << " "
           << m_contig.getReadTiling().size() << " reads" << endl;
    }
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in setContigId()\n" << e;
    retval = 1;
  }

  return retval;
}

void DataStore::fetchReadIID(ID_t readid, Read_t & read)
{
  try
  {
    read_bank.fetch(readid, read);
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: Can't fetch read iid:" << readid << "\n" << e;
  }
}

void DataStore::fetchContigBID(ID_t contigid, Contig_t & contig)
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

void DataStore::fetchContigIID(ID_t contigid, Contig_t & contig)
{
  try
  {
    contig_bank.fetch(contigid, contig);
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in fetchContig()\n" << e;
  }
}

void DataStore::fetchContigEID(const string & eid, Contig_t & contig)
{
  contig_bank.fetch(eid, contig);
}

void DataStore::fetchScaffoldBID(ID_t scaffid, Scaffold_t & scaff)
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


void DataStore::fetchScaffoldIID(ID_t scaffid, Scaffold_t & scaff)
{
  try
  {
    scaffold_bank.fetch(scaffid, scaff);
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in fetchScaffold()\n" << e;
  }
}

void DataStore::fetchFragIID(ID_t fragid, Fragment_t & frag)
{
  try
  {
    frag_bank.fetch(fragid, frag);
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR in fetchFrag()\n" << e;
  }
}

ID_t DataStore::lookupFragId(ID_t readid)
{
  if (!m_readfraglookup.empty())
  {
    return m_readfraglookup[read_bank.lookupBID(readid)];
  }
  else
  {
    Read_t read;
    read_bank.fetchFix(readid, read);
    return read.getFragment();
  }
}

AMOS::ID_t DataStore::getLibrary(ID_t readid)
{
  if (!m_libdistributionlookup.empty())
  {
    try
    {
      ID_t fragid, libid;

      fragid = lookupFragId(readid);
      if (fragid != AMOS::NULL_ID)
      {
        if (!m_fragliblookup.empty())
        {
          libid = m_fragliblookup[frag_bank.lookupBID(fragid)];
        }
        else
        {
          Fragment_t frag;
          frag_bank.fetchFix(fragid, frag);
          libid = frag.getLibrary();
        }

        return libid;
      }
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
  return m_readcontiglookup[read_bank.lookupBID(readid)];
}

ID_t DataStore::lookupScaffoldId(ID_t contigid)
{
  if (m_contigscafflookup.empty()){return 0;}
  return m_contigscafflookup[contig_bank.lookupBID(contigid)];
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


void DataStore::mapReadsToScaffold(Scaffold_t & scaff, 
                                   vector<Tile_t> & tiling,
                                   int verbose)
{
  Contig_t contig;
  vector<Tile_t> & ctiling = scaff.getContigTiling();
  vector<Tile_t>::const_iterator ci;

  if (verbose) { cerr << "Mapping reads to scaffold " << scaff.getEID() << "... "; }

  for (ci = ctiling.begin(); ci != ctiling.end(); ci++)
  {
    contig_bank.fetch(ci->source, contig);

    int clen = contig.getLength();

    vector<Tile_t> & crtiling = contig.getReadTiling();
    vector<Tile_t>::const_iterator ri;
    for (ri = crtiling.begin(); ri != crtiling.end(); ri++)
    {
      Tile_t mappedTile;
      mappedTile.source = ri->source;
      mappedTile.gaps   = ri->gaps;
      mappedTile.range  = ri->range;

      int offset = ri->offset;
      if (ci->range.isReverse())
      {
        mappedTile.range.swap();
        offset = clen - (offset + ri->range.getLength() + ri->gaps.size());
      }

      mappedTile.offset = ci->offset + offset;

      tiling.push_back(mappedTile);
    }
  }

  if (verbose) { cerr << tiling.size() << " reads mapped" << endl; }
}


void DataStore::calculateInserts(vector<Tile_t> & tiling,
                                 vector<Insert *> & inserts,
                                 bool connectMates,
                                 int verbose)
{
  if (verbose) { cerr << "Loading mates "; }

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

    DataStore::MateInfo_t mates = getMatePair(aid);

    // Does it have a mate
    if (mates.first == 0)
    {
      // no mate
      unmated++;
      insert = new Insert(aid, acontig, atile,
                          AMOS::NULL_ID, AMOS::NULL_ID, NULL,
                          libid, dist,
                          AMOS::Fragment_t::NULL_FRAGMENT);
    }
    else
    {
      matelisted++;

      ID_t bid = mates.first;
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
                          mates.second);

      if (insert->m_state == Insert::Happy) { happycount++; }
      else                                  { unhappycount++; }


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
                                mates.second);
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

    cerr << "inserts: " << inserts.size()
         << " mated: "   << mated 
         << " matelisted: " << matelisted
         << " unmated: " << unmated
         << " happy: " << happycount
         << " unhappy: " << unhappycount << endl;
  }
}


map<ID_t, CoverageStats> DataStore::computeCEStats(vector<Insert *> & inserts)
{
  typedef map<ID_t, CoverageStats> LibStats;
  LibStats libStats;
  LibStats::iterator li;

  int curloffset = 0, curroffset = 0;
  int totalinsertlen = 0;

  vector<Insert *>::iterator ii;
  for (ii = inserts.begin(); ii != inserts.end(); ii++)
  {
    curloffset = (*ii)->m_loffset;
    curroffset = (*ii)->m_roffset;

    totalinsertlen += (curroffset - curloffset + 1);

    if ((*ii)->ceConnected())
    {
      li = libStats.find((*ii)->m_libid);

      if (li == libStats.end())
      {
        li = libStats.insert(make_pair((*ii)->m_libid, CoverageStats(inserts.size()*4, (*ii)->m_libid, (*ii)->m_dist))).first;
      }

      li->second.addEndpoints(curloffset, curroffset);
    }
  }

  int cestatsheight = 100;

  for (li = libStats.begin(); li != libStats.end(); li++)
  {
    li->second.finalize();
    li->second.finalizeCE(cestatsheight);
  }

  return libStats;
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

