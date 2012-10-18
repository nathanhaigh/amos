////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Michael Schatz
//! \date 08/15/2005
//!
//! \brief Header for ContigIterator and related classes
//!
////////////////////////////////////////////////////////////////////////////////



#ifndef CONTIG_ITERATOR_HH
#define CONTIG_ITERATOR_HH 1

#include "Contig_AMOS.hh"
#include "Read_AMOS.hh"

#include <map>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <functional>

namespace AMOS 
{

/*! @brief A read within a contig "rendered" with gaps inserted
 *  
 *  An enhanced version of a read object which with the sequence and 
 *  quality values rendered with the gaps inserted.
 *  
 */

class TiledRead_t 
{
public:

  //! The tile information is used to render the sequence from the read. readidx is an index to assign
  TiledRead_t(Tile_t tile, Read_t red, int readidx);

  //! Returns the base at a given gapped consensus position or ' ' if outside the tiled range
  char base(Pos_t gindex) const
  {
    if (gindex < m_loffset || gindex > m_roffset) { return ' '; }
    return m_seq[gindex-m_loffset];
  }
  
  //! Returns the qv at a given gapped consensus position or -1 if outside the tiled range
  int qv(Pos_t gindex) const
  {
    if (gindex < m_loffset || gindex > m_roffset) { return -1; }
    return m_qual[gindex-m_loffset] - MIN_QUALITY;
  }

  //! Callee assigned index for read
  int32_t m_readidx;

  //! Left offset of the read in the contig
  Pos_t m_loffset;

  //! Right offset of the read in the contig
  Pos_t m_roffset;

  //! Boolean indicating if the read has been reversed
  bool m_isRC;

  //! iid of the read's fragment
  ID_t m_fragid;

  //! iid of the read
  ID_t m_iid;

  //! eid of the read
  std::string m_eid;

private:
  //! rendered sequence
  std::string m_seq;
  
  //! rendered qual
  std::string m_qual;
};

typedef std::list<TiledRead_t> TiledReadList_t;

/*! @brief A binary function for determining the canonical sort order for reads
 *  
 *  Sorts by offset, then read length, and finally by iid
 *
 */

struct TileOrderCmp
{
  bool operator () (const Tile_t & a, const Tile_t & b)
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




/*! @brief Maintains statistics the Column_t about reads which share the same base at the current position
 *
 *  Reads that agree at the current contig position are stored in the same
 *  BaseStats_t object. The object maintains a list of reads that agree, and
 *  some general statistics on the bases that tile that position.
 *  
 */

class BaseStats_t
{
public:
  //! The base this BaseStats_t will represent
  BaseStats_t(char base);

  //! The base this BaseStats_t represents
  char m_base;

  //! The cumulative quality value of the reads that tile the current position that are m_base
  int  m_cumqv;

  //! The maximum quality value of the reads that tile the current position that are m_base
  int  m_maxqv;

  //! A vector of reads that tile the current position that are m_base
  std::vector<TiledReadList_t::const_iterator> m_reads;

  //! Used to add a read to the BaseStats_t
  void addRead(TiledReadList_t::const_iterator tile, Pos_t gindex);
};



//! Functor for sorting BaseStats_t by count of reads of each base, then cumqv, then maxqv
struct BaseStatsCmp
{
  bool operator() (const BaseStats_t * a, const BaseStats_t * b)
  {
    if (a->m_reads.size() == b->m_reads.size())
    {
      if (a->m_cumqv == b->m_cumqv)
      {
        return a->m_maxqv > b->m_maxqv;
      }

      return a->m_cumqv > b->m_cumqv;
    }
    
    return a->m_reads.size() > b->m_reads.size();
  }
};



class ContigIterator_t;

/*! @brief Maintains data on reads at a given contig position
 *
 *  Organizes the reads at the current position in the contig
 *  by the base each read has at that position. The reads
 *  are collected into the m_baseinfo map which maps from
 *  base to BaseStats_t. The sortBaseInfo() method can be used
 *  to iterate through each base present at the current position.
 *
 *  Warning: The column data becomes invalid if the contig iterator
 *           position is changed!
 *  
 */

class Column_t 
{
public:
  //! Object can only be used for a single position!
  Column_t (ContigIterator_t & ci);

  //! Returns a vector of m_baseinfo sorted by BaseStatsCmp
  std::vector<BaseStats_t *> getBaseInfo();

  //! 0-based gapped index
  Pos_t m_gindex;
  
  //! 1-based ungapped index
  Pos_t m_uindex;

  //! Current consenus
  char m_cons;
  
  //! Consensus quality value
  int m_cqv;

  //! Total depth of coverage
  int32_t m_depth;

  //! Map for collecting read information about each base
  std::map<char, BaseStats_t> m_baseinfo;
};



/*! @brief Used for iterating through a contig on a per-consensus position basis
 * 
 *  Internally, the ContigIterator maintains a list of reads which tile the current
 *  contig position. This list can be accessed via the getTilingReads() method. In
 *  addition, you can also use the getColumn() method to create a Column_t object
 *  which sorts the reads by the base at the current consensus position rather
 *  than the position of the read in the layout. Use the 
 *  hasSNP() method to determine if the tiling at the current position is uniform
 *  or if it has a SNP.
 *
 *  Note: You must call advanceNext() or seek() to initialize the object
 */

class ContigIterator_t 
{
public:
  //! Constructor for iterator. You must call seek() or advanceNext() before using
  ContigIterator_t(Contig_t & ctg, Bank_t * rdbank);

  //! Seek to random position in O(numreads) time. Returns if the current position is valid
  bool seek(Pos_t gindex);

  //! Advance to next position (gindex+1) in O(maxdepth) time. Returns if the current position is valid
  bool advanceNext();

  //! Returns the current 1-based ungapped contig position
  Pos_t uindex() const;

  //! Returns the current 0-based gapped contig position
  Pos_t gindex() const { return m_gindex; }

  //! Returns if there is a SNP at the current position
  bool hasSNP() const;

  //! Returns depth at current position
  int depth() const { return m_tilingreads.size(); }

  //! Returns the consenus at the current position
  char cons()   const { return m_consensus[m_gindex]; }
  
  //! Returns the consensus quality value at this current position
  int cqv()    const { return m_consqual[m_gindex] - MIN_QUALITY;  }

  //! Returns a Column_t of the current position
  Column_t getColumn();

  //! Get a reference to the contig object
  const Contig_t &        getContig()      const { return m_contig; }


  //! Get list of reads tiling current position
  const TiledReadList_t & getTilingReads() const { return m_tilingreads; }




private:
  //! Renders a particular read by loading sequence and inserting gaps
  void renderTile(Tile_t & tile, int tilingIndex);

  //! Sort operator for maintain end position queue
  struct ReadListItEndCmp
  {
    bool operator () (const TiledReadList_t::iterator & a, const TiledReadList_t::iterator & b)
    {
      return (a->m_roffset > b->m_roffset);
    }
  };

  //! Keeps track of the next read to end
  std::priority_queue<TiledReadList_t::iterator, std::vector<TiledReadList_t::iterator>, ReadListItEndCmp > m_ends;

  //! Pointer to the readbank
  Bank_t * m_readBank;

  //! Consensus
  std::string m_consensus;

  //! Consensus quality
  std::string m_consqual;

  //! Current 0-based gapped contig position
  Pos_t m_gindex;

  //! Current 1-based ungapped contig position
  Pos_t m_uindex;

  //! Next read to consider in the tiling
  int32_t m_currenttile;

  //! Number of reads in the contig
  int32_t m_numreads;

  //! Contig object
  Contig_t m_contig;

  //! How many reads have been seen in previous contigs
  static int32_t s_tilingreadsoffset;
  
  //! Current offset to use for readidx
  int32_t m_tilingreadsoffset;

  //! Get List of reads tiling current position
  TiledReadList_t m_tilingreads;
};


} // namespace AMOS


#endif
