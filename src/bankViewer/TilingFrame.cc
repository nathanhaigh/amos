#include "TilingFrame.hh"
#include "TilingField.hh"
#include <qscrollview.h>
#include <qlayout.h>
#include "ConsensusField.hh"

int min (int a, int b)
{
  return a < b ? a : b;
}

TilingFrame::TilingFrame(QWidget * parent, const char * name, WFlags f = 0)
  :QFrame(parent, name, f),
   read_bank(Read_t::NCODE),
   contig_bank(Contig_t::NCODE)
{
  m_gindex = 0;
  m_fontsize = 12;
  resize(500, 500);
  m_db = "DMG";
  m_sv = new QScrollView(this, "tilingscroll");
  m_sv->setHScrollBarMode(QScrollView::AlwaysOff);
  m_sv->setVScrollBarMode(QScrollView::AlwaysOn);

  m_tilingfield = new TilingField(m_renderedSeqs,
                                  m_consensus,
                                  m_db,
                                  m_gindex,
                                  m_fontsize,
                                  m_sv->viewport(),
                                  "tiling" );

  m_sv->addChild(m_tilingfield);
  m_consfield = new ConsensusField(m_consensus, m_gindex, this, "cons");
  m_consfield->setFrameStyle(m_sv->frameStyle());
  m_consfield->setLineWidth(m_sv->lineWidth());
  m_consfield->setMidLineWidth(m_sv->midLineWidth());
  m_consfield->setBackgroundMode(m_sv->backgroundMode());

  QGridLayout * layout = new QGridLayout(this, 2, 1);
  layout->addWidget(m_consfield, 0,0);
  layout->addWidget(m_sv,1,0);
  layout->setRowStretch(1,10);

  connect(this,        SIGNAL(fontSizeChanged(int)),
          m_consfield, SLOT(setFontSize(int)));
}

void TilingFrame::paintEvent(QPaintEvent * event)
{
  QFrame::paintEvent(event);
  m_consfield->repaint();

  m_tilingfield->setSize(m_sv->visibleWidth(), m_sv->visibleHeight());
  m_tilingfield->repaint();
}

void TilingFrame::setContigId(int contigId)
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

      sort(m_tiling.begin(), m_tiling.end(), RenderSeq_t::TilingOrderCmp());

      // Render the aligned sequences
      int vectorpos;
      vector<Tile_t>::iterator vi;

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


void TilingFrame::setBankname(string bankname)
{
  if (bankname != "")
  {
    try
    {
      m_bankname = bankname;
      contig_bank.open(m_bankname);
      emit contigRange(1, contig_bank.getSize());

      setContigId(1);
    }
    catch (Exception_t & e)
    {
      cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    }
  }
}

void TilingFrame::setFontSize(int fontsize )
{
  if (fontsize == m_fontsize) { return; }
  m_fontsize = fontsize;

  int width = this->width();
  m_displaywidth = width/m_fontsize;

  repaint();
  emit fontSizeChanged(m_fontsize);
}

void TilingFrame::setGindex( int gindex )
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

void TilingFrame::setDB(const QString & db)
{
  m_db = db.ascii();
}
