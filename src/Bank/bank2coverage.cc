#include <algorithm>

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "Slice.h"

using namespace AMOS;
using namespace std;

const int MIN_QUAL = 4;


bool USE_EID = 1;

inline bool hasOverlap(Pos_t rangeStart, // 0-based exact offset of range
                       Pos_t rangeEnd,   // 0-based exact end of range
                       Pos_t seqOffset,  // 0-bases exact offset of seq
                       Pos_t seqLen,     // count of bases of seq (seqend+1)
                       Pos_t contigLen)  // count of bases in contig (contigend+1)
{
  int retval = 1;

  if (seqOffset >= 0)
  {
    if ((seqOffset > rangeEnd)                  // sequence right flanks
        || (seqOffset + seqLen-1 < rangeStart)) // sequence left flanks
    {
      retval = 0;
    }
  }
  else
  {
    // Negative Offset, test left and right separately
    retval = hasOverlap(rangeStart, rangeEnd, 
                        0, seqLen+seqOffset, contigLen) ||
             hasOverlap(rangeStart, rangeEnd, 
                        contigLen+seqOffset, -seqOffset, contigLen);
  }

  return retval;
}

class Render_t
{
public:
  Render_t();
  ~Render_t();
  void load(Bank_t & read_bank, vector<Tile_t>::const_iterator tile);

  string m_nucs;
  string m_qual;
  bool m_rc;
  Pos_t m_offset;

  Read_t m_read;
};

Render_t::Render_t()
{ }

Render_t::~Render_t()
{ }

void Render_t::load(Bank_t & read_bank, vector<Tile_t>::const_iterator tile)
{
  read_bank.fetch(tile->source, m_read);
  Range_t range = tile->range;

  m_offset = tile->offset;
  m_rc = 0;
  if (tile->range.begin > tile->range.end) { m_rc = 1; range.swap();} 

  m_nucs = m_read.getSeqString(range);
  m_qual = m_read.getQualString(range);

  if (m_rc) 
  { 
    Reverse_Complement(m_nucs); 
    reverse (m_qual.begin(), m_qual.end());
  }

  // Insert gaps
  Pos_t gapcount;
  vector<Pos_t>::const_iterator g;
  for (g =  tile->gaps.begin(), gapcount = 0; 
       g != tile->gaps.end(); 
       g++, gapcount++)
  {
    m_nucs.insert(*g+gapcount, 1, '-');

    // qv of a gap is the min of the flanking values
    char lqv = (*g+gapcount > 0) ? m_qual[*g+gapcount-1] : -1;
    char rqv = (*g+gapcount < m_qual.size()) ? m_qual[*g+gapcount] : -1;
    char gapqv = (lqv < rqv) 
                 ? (lqv != -1) ? lqv : rqv 
                 : (rqv != -1) ? rqv : lqv;

    m_qual.insert(*g+gapcount, 1, gapqv);
  }
}

struct TilingOrderCmp
{
  bool operator() (const Tile_t & a, const Tile_t & b)
  {
    int offdiff = b.offset - a.offset;

    if (offdiff)
    {
      if (offdiff < 0) { return false; }
      return true;
    }

    int lendiff = (b.range.getLength() + b.gaps.size()) -
                  (a.range.getLength() + a.gaps.size());

    if (lendiff)
    {
      if (lendiff < 0) { return false; }
      return true;
    }

    return (a.source < b.source);
  }
};


int main (int argc, char ** argv)
{
  if (argc != 3)
  {
    cerr << "Usage: bank2coverage bankname outprefix" << endl;
    return EXIT_FAILURE;
  }

  BankStream_t contig_bank(Contig_t::NCODE);
  Bank_t read_bank(Read_t::NCODE);
  string bank_name = argv[1];
  string outprefix = argv[2];

  string tcovfile  = outprefix + ".tcov";
  string idtblfile = outprefix + ".idTbl";

  ofstream tstream(tcovfile.c_str());
  if (!tstream.is_open()) { AMOS_THROW_IO("Could not open " + tcovfile); }

  ofstream itbl(idtblfile.c_str());
  if (!itbl.is_open()) { AMOS_THROW_IO("Could not open " + idtblfile); }

  cerr << "Processing " << bank_name << " at " << Date() << endl;

  try
  {
    read_bank.open(bank_name);
    contig_bank.open(bank_name);

    Contig_t contig;
    int contig_count = 1;
    int vectoroffset = 0;
    
    while (contig_bank >> contig)
    {
      string eid = contig.getEID();
      int iid    = contig.getIID();
      std::vector<Tile_t> & tiling = contig.getReadTiling();
      const string & consensus = contig.getSeqString();
      const Pos_t clen = consensus.size();

      sort(tiling.begin(), tiling.end(), TilingOrderCmp());

      Pos_t grangeStart = 0;
      Pos_t grangeEnd = clen-1;

      // Render the aligned sequences
      int vectorpos;
      vector<Tile_t>::const_iterator vi;

      vector<Render_t> renderedSeqs;
      for (vi =  tiling.begin(), vectorpos = 0;
           vi != tiling.end();
           vi++, vectorpos++)
      {
        Render_t rendered;
        rendered.load(read_bank, vi);

        itbl << vectorpos + vectoroffset 
             << " " << rendered.m_read.getEID() 
             << " " << (rendered.m_rc ? '1' : '0') 
             << endl;

        renderedSeqs.push_back(rendered);
      }

      vector<int> reads;
      reads.reserve(tiling.size());

      libSlice_Slice s;
      int dcov;
      dcov = tiling.size(); // trivially, this is the greatest dcov in the contig
      s.bc = new char [dcov+1];
      s.qv = new char [dcov];
      s.rc = new char [dcov];

      Pos_t gindex, index;
      for (gindex = grangeStart, index = contig.gap2ungap(grangeStart); 
           gindex <= grangeEnd; 
           gindex++)
      {
        // Figure out which reads tile this position
        vector<Render_t>::const_iterator ri;
        dcov = 0;

        for (ri =  renderedSeqs.begin(), vectorpos = 0; 
             ri != renderedSeqs.end(); 
             ri++, vectorpos++)
        {
          if (hasOverlap(gindex, gindex, 
                         ri->m_offset, ri->m_nucs.size(), 
                         clen))
          {
            s.rc[dcov]  = ri->m_rc;
            s.bc[dcov]  = ri->m_nucs[gindex - ri->m_offset];

            s.qv[dcov]  = ri->m_qual[gindex - ri->m_offset]-AMOS::MIN_QUALITY;
            if (s.qv[dcov] < MIN_QUAL) { s.qv[dcov] = MIN_QUAL; }

            reads[dcov] = vectorpos+vectoroffset;

            dcov++;
          }
        }

        // Compute consensus qv

        libSlice_Consensus consensusResults;
        s.c = consensus[gindex];
        s.dcov = dcov;
        s.bc[dcov] = '\0';

        libSlice_getConsensusParam(&s, &consensusResults, NULL, 0, 0);
        int cqv = consensusResults.qvConsensus;

        int coordinate = index;
        if (consensus[gindex] == '-' && coordinate > 1) { coordinate--; }

        // Print Slice

        tstream << iid << " "
                << gindex+1 << " "
                << coordinate << " " 
                << consensus[gindex] << " "
                << cqv;

        if (consensus[gindex] != '-') { index++; }

        if (dcov)
        {
          tstream << " " << s.bc << " ";

          int j;
          for (j = 0; j < dcov; j++)
          {
            if (j) { tstream << ":"; }
            tstream << (int) s.qv[j];
          }

          tstream << " ";

          for (j = 0; j < dcov; j++)
          {
            if (j) { tstream << ":"; }
            tstream << (int) reads[j];
          }
        }

        tstream << endl;
      }

      delete [] s.bc;
      delete [] s.qv;
      delete [] s.rc;

      vectoroffset += tiling.size();
      contig_count++;
    }

    read_bank.close();
    contig_bank.close();
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    return EXIT_FAILURE;
  }

  cerr << "End: " << Date() << endl;
  return EXIT_SUCCESS;
}
