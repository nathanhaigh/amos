#include "TilingField.hh"
#include <qpainter.h>
#include <qpixmap.h>
#include "ReadInfo.hh"
#include <qstring.h>

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
  m_db = "DMG";
  setPalette( QPalette( QColor( 250, 250, 250) ) );
  m_loaded = 0;
  m_displaywidth=0;
  m_desiredheight = 0;
  m_fontsize = 12;
  resize(this->width(), 500);
}


void TilingField::mouseReleaseEvent( QMouseEvent *e )
{
  int lineheight  = m_fontsize+5;
  int tilevoffset = lineheight*4;

  int top = tilevoffset - lineheight;

  int dcov = e->y();
  if (dcov < top) { return; }
  dcov -= top;
  dcov /= lineheight;

  if (dcov >= (int) m_currentReads.size()) { return; }
  ReadInfo * readinfo = new ReadInfo(m_currentReads[dcov], m_db, this, "readinfo");
  readinfo->show();
}

void TilingField::setContigId(int contigId)
{
  if (m_bankname != "")
  {
    try
    {
      read_bank.open(m_bankname);
      contig_bank.open(m_bankname);

      Contig_t contig;
      contig_bank.fetch(contigId, contig);

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
        RenderSeq_t rendered(vectorpos);
        m_renderedSeqs.push_back(rendered);
        m_renderedSeqs[vectorpos].load(read_bank, vi);
      }

      emit setGindexRange(0, (int)m_consensus.size()-1);
      emit contigLoaded(contigId);
      m_loaded = 1;

      QString s = "Viewing ";
      s += m_bankname.c_str();
      s += " with ";
      s += QString::number(contig_bank.getSize());
      s += " contigs";

      s += " Contig Id:";
      s += QString::number(contigId);
      s += " Size: ";
      s += QString::number(m_consensus.size());
      s += " Reads: ";
      s += QString::number(m_tiling.size());

      emit setStatus(s);

      repaint();
    }
    catch (Exception_t & e)
    {
      cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    }
  }
}

int min (int a, int b)
{
  return a < b ? a : b;
}

int max (int a, int b)
{
  return a > b ? a : b;
}

void TilingField::setBankname(string bankname)
{
  if (bankname != "")
  {
    try
    {
      m_bankname = bankname;
      contig_bank.open(m_bankname);
      emit contigRange(1, contig_bank.getSize());

      QString s = "Viewing ";
      s += m_bankname.c_str();
      s += " with ";
      s += QString::number(contig_bank.getSize());
      s += " contigs";

      emit setStatus(s);

      setContigId(1);
    }
    catch (Exception_t & e)
    {
      cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    }
  }
}

void TilingField::setFontSize(int fontsize )
{
  if (fontsize == m_fontsize) { return; }
  m_fontsize = fontsize;

  int width = this->width();
  m_displaywidth = width/m_fontsize;

  repaint();
  emit setFontSize(m_fontsize);
}

void TilingField::setGindex( int gindex )
{
  if (!m_loaded) { return; }

  int width = this->width();
  m_displaywidth = width/m_fontsize;

  gindex = min(gindex, m_consensus.size()-m_displaywidth);

  if ( m_gindex == gindex ) {return;}

  m_gindex = gindex;
  repaint();
  emit gindexChanged( m_gindex );
}


void TilingField::paintEvent( QPaintEvent * )
{
  if (!m_loaded) { return; }

  int height = 1000;

  QPixmap pix(this->width(), height);
  pix.fill(this, 0,0);

  QPainter p( &pix );

  int lineheight  = m_fontsize+5;
  int tilevoffset = lineheight*4;
  int tilehoffset = m_fontsize*10;
  int seqnamehoffset = 10;

  int width = this->width();
  m_displaywidth = width/(m_fontsize-4);


  p.setFont(QFont("Courier", m_fontsize));

  vector<int> reads;
  reads.reserve(m_tiling.size());

  Pos_t grangeStart = m_gindex;
  Pos_t grangeEnd = min(m_gindex + m_displaywidth, m_consensus.size()-1);

  // Figure out which reads tile this range
  vector<RenderSeq_t>::iterator ri;
  int dcov = 0;
  
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

  m_currentReads.clear();

  for (ri =  m_renderedSeqs.begin();
       ri != m_renderedSeqs.end(); 
       ri++)
  {
    if (hasOverlap(grangeStart, grangeEnd, 
                   ri->m_offset, ri->m_nucs.size(), 
                   clen))
    {
//      QString s = QString::number(ri->m_vectorpos);
      QString s = ri->m_read.getEID().c_str();
      if (ri->m_rc)
      {
        s += "[RC]"; 
      }

      p.drawText(seqnamehoffset, lineheight*dcov + tilevoffset, s);

      s = "";
      for (int j = grangeStart; j <= grangeEnd; j++)
      {
        s += ri->base(j);
      }

      p.drawText (tilehoffset, lineheight*dcov + tilevoffset, s);
      dcov++;

      m_currentReads.push_back(ri);
    }
  }

  height = lineheight*dcov + tilevoffset;
  m_desiredheight = height;

  p.end();
  p.begin(this);
  pix.resize(this->width(), height);

  p.drawPixmap(0, 0, pix);
}


QSizePolicy TilingField::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void TilingField::setDB(const QString & db)
{
  m_db = db.ascii();
}





