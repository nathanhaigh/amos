#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>

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

  if (seqOffset >= (Pos_t) 0)
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



TilingField::TilingField( QWidget *parent, const char *name )
        : QWidget( parent, name ),
          read_bank(Read_t::NCODE),
          contig_bank(Contig_t::NCODE)
{
  m_gindex = 0;
  setPalette( QPalette( QColor( 250, 250, 200) ) );
  m_displaywidth=40;
  m_loaded = 0;
}

void TilingField::loadContig(string bank_name, int contigId)
{
  cerr << "Loading " << bank_name << ":" << contigId 
       << " at " << Date() << endl;

  read_bank.open(bank_name);
  contig_bank.open(bank_name);

  Contig_t contig;
  contig_bank.fetch(contigId, contig);
  {
    m_tiling = contig.getReadTiling();
    m_consensus = contig.getSeqString();

    sort(m_tiling.begin(), m_tiling.end(), TilingOrderCmp());

    // Render the aligned sequences
    int vectorpos;
    vector<Tile_t>::const_iterator vi;

    for (vi =  m_tiling.begin(), vectorpos = 0;
         vi != m_tiling.end();
         vi++, vectorpos++)
    {
      RenderSeq_t rendered;
      m_renderedSeqs.push_back(rendered);
      m_renderedSeqs[vectorpos].load(read_bank, vi);
    }
  }

  emit setRange(0, (int)m_consensus.size()-m_displaywidth);
  m_loaded = 1;
}


void TilingField::setGindex( int gindex )
{
  if (!m_loaded) { return; }

  int width = this->width();
  m_displaywidth = width/8;

  if (gindex > m_consensus.size()-m_displaywidth)
  {
    gindex = m_consensus.size()-m_displaywidth;
  }

  if ( m_gindex == gindex ) {return;}

  m_gindex = gindex;
  repaint();
  emit gindexChanged( m_gindex );
}

int min (int a, int b)
{
  return a < b ? a : b;

}


void TilingField::paintEvent( QPaintEvent * )
{
  QRect cr(0,0, this->width(), this->height());

  QPixmap pix(this->width(), this->height());
  pix.fill(this, cr.topLeft());

  QPainter p( &pix );

  int fontsize = 12;
  int lineheight  = fontsize+5;
  int tilevoffset = lineheight*4;
  int tilehoffset = 50;
  int seqnamehoffset = 10;


  p.setFont(QFont("Courier", fontsize));

  vector<int> reads;
  reads.reserve(m_tiling.size());

  Pos_t grangeStart = m_gindex;
  Pos_t grangeEnd = min(m_gindex + m_displaywidth, m_consensus.size()-1);

  // Figure out which reads tile this range
  vector<RenderSeq_t>::const_iterator ri;
  int dcov = 0;
  int vectorpos = 0;
  
  int clen = m_consensus.size();

  QString pos;
  QString cons;
  for (int j = grangeStart; j <= grangeEnd; j++)
  {
    pos += QString::number(j%10);
    cons += m_consensus[j];
  }

  p.drawText(tilehoffset, lineheight, pos);

  p.drawText(tilehoffset, lineheight*2, cons);
  cons = "cons";
  p.drawText(seqnamehoffset, lineheight*2, cons);


  for (ri =  m_renderedSeqs.begin(), vectorpos = 0; 
       ri != m_renderedSeqs.end(); 
       ri++, vectorpos++)
  {
    if (hasOverlap(grangeStart, grangeEnd, 
                   ri->m_offset, ri->m_nucs.size(), 
                   clen))
    {
      QString s = QString::number(vectorpos);
      p.drawText(seqnamehoffset, lineheight*dcov + tilevoffset, s);

      s = "";
      for (int j = grangeStart; j <= grangeEnd; j++)
      {
        s += ri->base(j);
      }

      p.drawText (tilehoffset, lineheight*dcov + tilevoffset, s);
      dcov++;
    }
  }

  p.end();
  p.begin(this);
  p.drawPixmap(cr.topLeft(), pix);
}


QSizePolicy TilingField::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}





