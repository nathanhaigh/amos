#ifndef CONTIG_ITERATOR_HH
#define CONTIG_ITERATOR_HH 1

#include <foundation_AMOS.hh>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <functional>


class TiledRead_t 
{
public:

  TiledRead_t(AMOS::Tile_t tile, AMOS::Read_t red, int readidx);

  inline char base(AMOS::Pos_t gindex) const
  {
    if (gindex < m_loffset || gindex > m_roffset) { return ' '; }
    return m_seq[gindex-m_loffset];
  }
  
  inline int qv(AMOS::Pos_t gindex) const
  {
    if (gindex < m_loffset || gindex > m_roffset) { return 0; }
    return m_qual[gindex-m_loffset] - AMOS::MIN_QUALITY;
  }

  int m_readidx;

  AMOS::Pos_t m_roffset;
  AMOS::Pos_t m_loffset;

  char m_isRC;

  AMOS::ID_t  m_fragid;
  AMOS::ID_t  m_iid;
  std::string m_eid;

private:
  std::string m_seq;
  std::string m_qual;
};

typedef std::list<TiledRead_t> ReadList_t;
typedef std::vector<AMOS::Tile_t> TileVector_t;

struct cmpTile 
{
  bool operator () (const AMOS::Tile_t & a, const AMOS::Tile_t & b)
  {
    int offdiff = b.offset - a.offset;

    if (offdiff)
    {
      if (offdiff < 0) { return false; }
      return true;
    }

    int lendiff = (b.getGappedLength()) - (a.getGappedLength());

    if (lendiff)
    {
      if (lendiff < 0) { return false; }
      return true;
    }

    return (a.source < b.source);
  }
};


struct cmpPosIt 
{
  bool operator () (const ReadList_t::iterator & a, const ReadList_t::iterator & b)
  {
    return (a->m_roffset > b->m_roffset);
  }
};

typedef std::vector<ReadList_t::iterator> BaseGroup_t;

class BaseStats_t
{
public:
  BaseStats_t(char base);
  void addRead(ReadList_t::iterator & tile, AMOS::Pos_t gindex);

  char m_base;
  int  m_count;
  int  m_cumqv;
  int  m_maxqv;

  BaseGroup_t m_reads;
};


typedef std::map<char, BaseStats_t> StatsMap;
typedef std::vector<BaseStats_t *> StatsList;

struct BaseStatsFreqCmp
{
  bool operator() (const BaseStats_t * a, const BaseStats_t * b)
  {
    return a->m_count > b->m_count;
  }
};

struct BaseStatsQVCmp
{
  bool operator() (const BaseStats_t * a, const BaseStats_t * b)
  {
    return a->m_cumqv > b->m_cumqv;
  }
};




class Column_t 
{
public:
  Column_t ();

  int  m_depth;
  char m_consensus;
  int  m_consqual;

  StatsMap  m_baseinfo;

  StatsList m_basefrequencies;
  StatsList m_baseqv;
};





class ContigIterator_t 
{
public:
  ContigIterator_t(AMOS::Contig_t & ctg, AMOS::Bank_t * rdbank);

  bool seek(AMOS::Pos_t gindex);
  bool advanceNext();

  bool hasSNP() const;

  inline char cons()   const { return m_consensus[m_gindex]; }
  inline int  cqv()    const { return m_consqual[m_gindex];  }

         AMOS::Pos_t  uindex() const;
  inline AMOS::Pos_t  gindex() const { return m_gindex; }

  Column_t getColumn();

  inline const ReadList_t &     getTilingReads() const { return m_tiled;  }
  inline const AMOS::Contig_t & getContig()      const { return m_contig; }


private:
  void renderTile(AMOS::Tile_t & tile, int tilingIndex);

  AMOS::Bank_t * m_readBank;
  std::priority_queue<ReadList_t::iterator, std::vector<ReadList_t::iterator>, cmpPosIt > m_ends;

  std::string    m_consensus;
  std::string    m_consqual;
  AMOS::Pos_t    m_gindex;
  AMOS::Pos_t    m_uindex;

  int            m_currenttile;

  int            m_tilingreads;
  ReadList_t     m_tiled;
  AMOS::Contig_t m_contig;

  static int     s_tilingreadsoffset;
};


#endif
