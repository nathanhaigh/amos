#include "OverviewWidget.hh"
#include "OverviewField.hh"
#include "ContigCanvasItem.hh"
#include "FeatureCanvasItem.hh"
#include <qcanvas.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qwmatrix.h>

#include "DataStore.hh"


using namespace std;
using namespace AMOS;


struct TileSizeCmp
{
  bool operator() (const Tile_t & a, const Tile_t & b)
  {
    return (a.range.getLength() > b.range.getLength());
  }
};


OverviewWidget::OverviewWidget(DataStore * datastore,
                               QWidget * parent, 
                               const char * name)
  :QWidget(parent, name)
{
  QBoxLayout * hbox = new QHBoxLayout(this);

  m_datastore = datastore;

  m_ocanvas = new QCanvas(this, "ocanvas");
  m_ocanvas->setBackgroundColor(Qt::black);

  m_ofield = new OverviewField(datastore, m_ocanvas, this, "ofield");

  hbox->addWidget(m_ofield);
  hbox->activate();

  initializeTiling();
  m_ofield->show();
}


void OverviewWidget::initializeTiling()
{
  m_tilingwidth = 0;
  m_features.clear();
  m_contigs.clear();

  if (m_datastore->contig_bank.isOpen())
    {
      Contig_t contig;
      m_datastore->contig_bank.seekg(1);

      while (m_datastore->contig_bank >> contig)
        {
          Tile_t currentContig;
          currentContig.source = contig.getIID();
          currentContig.offset = 0;
          currentContig.range = Range_t(0, contig.getLength());

          m_contigs.push_back (currentContig);

          if ( currentContig.range.getLength() > m_tilingwidth )
            m_tilingwidth = currentContig.range.getLength();
        }
    }

  if (m_datastore->feat_bank.isOpen())
    {
      Feature_t feat;
      m_datastore->feat_bank.seekg(1);
      
      while (m_datastore->feat_bank >> feat)
        {
          if (feat.getSource().second == Contig_t::NCODE)
            m_features.push_back(feat);
        }
    }

  sort (m_contigs.begin(), m_contigs.end(), TileSizeCmp());

  paintCanvas();
}


void OverviewWidget::paintCanvas()
{
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  int seqheight = 4;
  int gutter = 15;
  int voffset = gutter;
  int lineheight = seqheight + gutter;

  vector<Tile_t>::const_iterator ci;
  for (ci = m_contigs.begin(); ci != m_contigs.end(); ++ci)
    {
      int offset = m_tilingwidth / 2 - ci->range.getLength() / 2;
      int vpos = voffset;

      ContigCanvasItem * contig = new ContigCanvasItem
        (offset, vpos, ci->range.getLength(), seqheight, *ci, m_ocanvas);
      contig->show();


      vector<AMOS::Feature_t>::iterator fi;
      for (fi = m_features.begin(); fi != m_features.end(); fi++)
        {
          if ( fi->getSource().first != ci->source ||
               fi->getType() == Feature_t::COVERAGE )
            continue;
          int foffset = offset + fi->getRange().getLo();
          int flength = fi->getRange().getLength();
          FeatureCanvasItem * feature = new FeatureCanvasItem
            (foffset, vpos+seqheight, flength, seqheight, *fi, m_ocanvas);
          feature->show();
        }


      voffset += lineheight;
    }


  m_ocanvas->resize(m_tilingwidth, voffset);
  m_ocanvas->update();

  QApplication::restoreOverrideCursor();
}


void OverviewWidget::resizeEvent (QResizeEvent*)
{
  double xf = (double)m_ofield->viewport()->width() / m_ocanvas->width();
  QWMatrix matrix (xf, 0, 0, 1, 0, 0);
  m_ofield->setWorldMatrix (matrix);
  m_ofield->update();
}
