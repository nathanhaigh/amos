#include <algorithm>

#include "foundation_AMOS.hh"
#include "amp.hh"
#include "fasta.hh"
#include "Slice.h"

using namespace AMOS;
using namespace std;

bool USE_EID = 1;

bool hasOverlap(Pos_t rangeStart, // 0-based exact offset of range
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

Pos_t getUngappedPos(const string & str, Pos_t offset)
{
  Pos_t retval = 1;

  for (Pos_t gindex = 0; gindex < offset; gindex++)
  {
    if (str[gindex] != '-')
    {
      retval++;
    }
  }

  return retval;
}


class Render_t
{
public:
  Render_t();
  ~Render_t();
  void load(Bank_t & read_bank, vector<Tile_t>::const_iterator tile);

  Read_t read;
  string m_nucs;
  string m_qual;
  bool m_rc;
  Pos_t m_offset;

private:
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



int main (int argc, char ** argv)
{
  if (argc != 2)
  {
    cerr << "Usage: bank2coverage bankname" << endl;
    return EXIT_FAILURE;
  }

  BankStream_t contig_bank(Contig_t::NCODE);
  Bank_t read_bank(Read_t::NCODE);
  string bank_name = argv[1];

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
      const std::vector<Tile_t> & tiling = contig.getReadTiling();
      const string & consensus = contig.getSeqString();
      const Pos_t clen = consensus.size();

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
        renderedSeqs.push_back(rendered);
      }

      Pos_t gindex, index;
      for (gindex = 0, index = 1; gindex < clen; gindex++)
      {
        // Figure out which reads tile this position
        
        vector<int> reads;
        vector<char> quals;
        vector<char> rcs;
        string bases;

        vector<Render_t>::const_iterator ri;

        for (ri =  renderedSeqs.begin(), vectorpos = 0; 
             ri != renderedSeqs.end(); 
             ri++, vectorpos++)
        {
          if (hasOverlap(gindex, gindex, 
                         ri->m_offset, ri->m_nucs.size(), 
                         clen))
          {
            bases.push_back(ri->m_nucs[gindex - ri->m_offset]);
            quals.push_back(ri->m_qual[gindex - ri->m_offset]-AMOS::MIN_QUALITY);
            rcs.push_back(ri->m_rc);
            reads.push_back(vectorpos+vectoroffset);
          }
        }

        // Compute consensus qv

        libSlice_Slice s;
        libSlice_Consensus * consensusResults;
        s.c = consensus[gindex];
        s.dcov = reads.size();
        s.bc = (char *) bases.c_str();

        s.qv = (char *) malloc(s.dcov);
        s.rc = (char *) malloc(s.dcov);

        int j;
        for (j = 0; j < s.dcov; j++)
        {
          s.qv[j] = quals[j];
          s.rc[j] = rcs[j];
        }

        libSlice_getConsensusParam(&s, &consensusResults, NULL, 0, 0);
        int cqv = consensusResults->qvConsensus;

        free(s.qv);
        free(s.rc);
        free(consensusResults);

        int coordinate = index;
        if (consensus[gindex] == '-' && coordinate > 1) { coordinate--; }

        // Print Slice

        cout << contig_count << " "
             << coordinate << " " 
             << consensus[gindex] << " "
             << cqv;

        if (consensus[gindex] != '-') { index++; }

        if (!reads.empty())
        {
          cout << " " << bases << " ";
          
          vector<char>::const_iterator q;
          for (q = quals.begin(); q != quals.end(); q++)
          {
            if (q != quals.begin()) { cout << ":"; }

            cout << (int) *q;
          }

          cout << " ";

          vector<int>::const_iterator r;
          for (r = reads.begin(); r != reads.end(); r++)
          {
            if (r != reads.begin()) { cout << ":"; }

            cout << *r;
          }
        }

        cout << endl;
      }

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
