#include "InsertWidget.hh"

#include <set>

#include <qlayout.h>
#include <qlabel.h>
#include <qmatrix.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <q3accel.h>
#include <qapplication.h>
#include <q3textedit.h>
#include <q3hbox.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3BoxLayout>
#include <Q3VBoxLayout>
#include <Q3PointArray>
#include <QtGui>

#include "RenderSeq.hh"
#include "InsertCanvasItem.hh"
#include "CoverageCanvasItem.hh"
#include "FeatureCanvasItem.hh"
#include "ContigCanvasItem.hh"
#include "UIElements.hh"
#include "CoverageStats.hh"
#include "InsertField.hh"
#include "InsertPosition.hh"
#include "DataStore.hh"
#include "CoverageRectCanvasItem.hh"
#include "OverviewField.hh"

#include "RangeScrollBar.hh"



using namespace AMOS;
using namespace std;
typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;

int MAXMERCOUNT = 250;


int extractSNPScore (const AMOS::Feature_t & fea)
{
  QString com (fea.getComment().c_str());
  com.replace ("HIGH_SNP ", "");
  com.replace (QRegExp (" .+"), "");
  return com.toInt();
}

int extractUnitigScore (const AMOS::Feature_t & fea)
{
  return fea.getRange().getLength();
}

int extractQCScore (const AMOS::Feature_t & fea)
{
  return fea.getRange().getLength();
}

int extractBreakScore (const AMOS::Feature_t & fea)
{
  QString com (fea.getComment().c_str());
  com.replace (QRegExp (" .+"), "");
  return com.toInt();
}


struct FeatOrderCmp
{
  bool operator() (const AMOS::Feature_t & a, const AMOS::Feature_t & b)
  {
    int aoffset = a.getRange().getLo();
    int boffset = b.getRange().getLo();

    int offdiff = boffset - aoffset;

    if (offdiff)
    {
      if (offdiff < 0) { return false; }
      return true;
    }

    int lendiff = (b.getRange().getLength()) -
                  (a.getRange().getLength());

    if (lendiff)
    {
      if (lendiff < 0) { return false; }
      return true;
    }

    return true;
  }
};



InsertWidget::InsertWidget(DataStore * datastore,
                           map<char, pair<int, bool> > & types,
                           QWidget * parent, const char * name)
 : QWidget(parent, name),
   m_types(types)
{
  m_datastore = datastore;
  m_tilingVisible = NULL;

  //setWindowFlags(Qt::WRepaintNoErase | Qt::WResizeNoErase);

  m_hscale = .06250;
  m_contigid = 0;
  m_gstart = 0;
  m_gend = 0;
  m_width = 0;
  m_scaffoldId = 0;

  m_paddle = NULL;
  m_ball = NULL;
  m_timer = new QTimer(this, "timer");
  m_autoplay = 0;
  connect(m_timer, SIGNAL(timeout()),
          this, SLOT(timeout()));

  m_hoffset        = 0;
  m_connectMates   = 1;
  m_partitionTypes = 1;
  m_coveragePlot   = 1;
  m_cestats        = 1;
  m_paintScaffold  = 1;
  m_colorByLibrary = 0;
  m_colorByMate    = 0;
  m_colorByStretchiness = 0;

  m_tintHappiness  = 0;
  m_showscaffold   = 1;
  m_kmercoverageplot = 1;
  m_insertCovFeatures = 1;
  m_readCovFeatures = 1;
//   m_snpFeatures = 1;
//   m_unitigFeatures = 1;
//   m_qcFeatures = 1;
//   m_breakFeatures = 1;
  m_misasmFeatures = 1;
  m_otherFeatures = 1;

  m_syncWithTiling = 1;

  m_kmerstats = NULL;
  m_insertCL = NULL;
  m_readCL = NULL;

  m_overviewtop = 0;
  m_overviewbottom = 1;

  m_updatingScrollBars = false;

  m_currentScaffold = AMOS::NULL_ID;
  m_currentContig   = AMOS::NULL_ID;

  m_seqheight = 4;
  m_tilingwidth = 0;

  m_iposition = new InsertPosition(m_datastore, m_scaffoldId, m_scaffoldName,
                                   this, "insertposition");
  m_icanvas = new Q3Canvas(this, "icanvas");
  m_icanvas->setBackgroundColor(QColor(0,0,0));
  m_icanvas->retune(128);

  Q3HBox * hbox = new Q3HBox(this);
  m_ifield = new InsertField(datastore, m_hoffset, m_icanvas, hbox, "qcv");

  vrange = new RangeScrollBar_t(Qt::Vertical, hbox);

  m_overview = new OverviewField(m_icanvas, this, "overview");
  m_overview->setMaximumHeight(100);
  m_overview->setMinimumHeight(100);
  m_overview->setVScrollBarMode(Q3ScrollView::AlwaysOff);
  m_overview->setHScrollBarMode(Q3ScrollView::AlwaysOff);
  connect(m_overview, SIGNAL(centerView(int)), this, SLOT(centerView(int)));

  hrange = new RangeScrollBar_t(Qt::Horizontal, this);

  m_ifield->setScrollBars(hrange, vrange);

  Q3BoxLayout * vbox = new Q3VBoxLayout(this);
  vbox->addWidget(m_iposition);
  vbox->addWidget(hbox, 10);
  vbox->addWidget(m_overview);
  vbox->addWidget(hrange);
  vbox->activate();

  connect(this, SIGNAL(setZoomInTool()),  m_ifield, SLOT(setZoomInTool()));
  connect(this, SIGNAL(setZoomOutTool()), m_ifield, SLOT(setZoomOutTool()));
  connect(this, SIGNAL(setSelectTool()),  m_ifield, SLOT(setSelectTool()));

  connect(m_ifield, SIGNAL(updateVisibleRange()),
          this,     SLOT(updateVisibleRange()));


  connect(this,     SIGNAL(search(const QString &)),
          m_ifield,   SLOT(search(const QString &)));

  connect(this,     SIGNAL(highlightIID(const QString &)),
          m_ifield,   SLOT(highlightIID(const QString &)));

  connect(this,     SIGNAL(canvasCleared()),
          m_ifield,   SLOT(canvasCleared()));

  connect(this,     SIGNAL(highlightRead(int)),
          m_ifield,   SLOT(highlightRead(int)));

  connect(this,     SIGNAL(highlightEID(const QString &)),
          m_ifield,   SLOT(highlightEID(const QString &)));

  connect(m_ifield, SIGNAL(readEIDHighlighted(const QString &)),
          this,     SIGNAL(readEIDHighlighted(const QString &)));

  connect(m_ifield, SIGNAL(readIIDHighlighted(const QString &)),
          this,     SIGNAL(readIIDHighlighted(const QString &)));

  connect(m_ifield, SIGNAL(jumpToRead(int)),
          this,     SIGNAL(jumpToRead(int)));


  connect(hrange, SIGNAL(rangeChanged(int, int)),
          this,   SLOT(setVisibleHRange(int, int)));

  connect(hrange, SIGNAL(valueChanged(int)),
          this,   SLOT(setHPos(int)));

  connect(vrange, SIGNAL(rangeChanged(int, int)),
          this,   SLOT(setVisibleVRange(int, int)));

  connect(vrange, SIGNAL(valueChanged(int)),
          this,   SLOT(setVPos(int)));

  connect(m_ifield, SIGNAL(visibleRange(int, double)),
          m_iposition, SLOT(setVisibleRange(int,double)));

  connect(this,    SIGNAL(currentScaffoldCoordinate(int)),
          m_iposition, SLOT(setScaffoldCoordinate(int)));

  connect(m_ifield, SIGNAL(setDetails(const QString &)),
          this,     SIGNAL(setDetails(const QString &)));

  connect(m_ifield, SIGNAL(setGindex(int)),
          this,     SLOT(computePos(int)));

  connect(m_ifield,   SIGNAL(fieldResized()),    this, SLOT(resizeField()));
  connect(m_overview, SIGNAL(overviewResized()), this, SLOT(resizeOverview()));

  Q3Accel *a = new Q3Accel( this );
  a->connectItem(a->insertItem(Qt::CTRL+Qt::SHIFT+Qt::Key_S), this, SLOT(start()) );
  a->connectItem(a->insertItem(Qt::Key_Left),         this, SLOT(left()) );
  a->connectItem(a->insertItem(Qt::Key_Right),        this, SLOT(right()) );
  a->connectItem(a->insertItem(Qt::Key_Escape),       this, SLOT(stopbreak()));
  a->connectItem(a->insertItem(Qt::CTRL+Qt::SHIFT+Qt::Key_A), this, SLOT(autoplay()) );

  a->connectItem(a->insertItem(Qt::CTRL+Qt::Key_Q),       qApp, SLOT(quit()));

  m_ifield->show();
}

void InsertWidget::centerView(int xpos)
{
  xpos = (int)(xpos*m_ifield->worldMatrix().m11());
  m_ifield->center(xpos, (int)(m_ifield->contentsY() + m_ifield->visibleHeight()/2));
  updateVisibleRange();
}

void InsertWidget::computePos(int gindex)
{
  // figure out which contig tiles this position

  if (m_ctiling.size() == 1)
  {
    emit setGindex(gindex);
  }
  else
  {
    vector<Tile_t>::const_iterator ci;

    for (ci = m_ctiling.begin(); ci != m_ctiling.end(); ci++)
    {
      if ((ci->offset <= gindex) && (gindex <= (ci->offset + ci->range.getLength())))
      {
        AMOS::ID_t bid = m_datastore->contig_bank.getIDMap().lookupBID(ci->source);

        gindex -= ci->offset;

        if (ci->range.isReverse())
        {
          gindex = ci->range.getLength() - gindex;
        }

        emit setContigId(bid);
        emit setGindex(gindex);
        break;
      }
    }
  }
}

void InsertWidget::initializeVisibleRectangle()
{
  m_tilingVisible = new Q3CanvasRectangle(m_icanvas);
  m_tilingVisible->setZ(-1);

  m_tilingVisible->setBrush(QColor(200,200,100));
  m_tilingVisible->setPen(QColor(200,200,100));
}

InsertWidget::~InsertWidget()
{
  flushInserts();

  if (m_kmerstats) { delete m_kmerstats; }
  if (m_insertCL)  { delete m_insertCL;  }
  if (m_readCL)    { delete m_readCL;    }
}


void InsertWidget::setTilingVisibleRange(int contigid, int gstart, int gend)
{
  if (contigid == AMOS::NULL_ID) { return; }

  m_contigid = contigid;
  m_gstart = gstart;
  m_gend = gend;

  if (m_paintScaffold && m_datastore->m_scaffoldId != AMOS::NULL_ID)
  {
    vector<Tile_t>::const_iterator ci;
    for (ci = m_ctiling.begin(); ci != m_ctiling.end(); ci++)
    {
      AMOS::ID_t bid = m_datastore->contig_bank.getIDMap().lookupBID(ci->source);

      if (bid == (ID_t) contigid)
      {
        if (ci->range.isReverse())
        {
          int t = gstart;
          gstart = ci->offset + (ci->range.getLength() - gend);
          gend   = ci->offset + (ci->range.getLength() - t);
        }
        else
        {
          gstart += ci->offset;
          gend += ci->offset;
        }

        break;
      }
    }
  }


  // resize and place rectangle
  m_tilingVisible->setSize((int)(m_hscale*(gend - gstart)) +1, m_icanvas->height());
  m_tilingVisible->move((int)(m_hscale*(gstart+m_hoffset)), 0);
  m_tilingVisible->show();

  m_icanvas->setChanged(m_tilingVisible->boundingRect());
  m_icanvas->update();

  if (m_syncWithTiling)
  {
    double xpos = m_hscale*((gstart + gend)/2+m_hoffset);
    xpos *= m_ifield->worldMatrix().m11();
    xpos -= (m_ifield->visibleWidth())/2;
    m_ifield->setContentsPos((int) xpos, m_ifield->contentsY());
    updateVisibleRange();
  }

  emit currentScaffoldCoordinate(gstart);
}

void InsertWidget::setSyncWithTiling(bool sync)
{
  m_syncWithTiling = sync;
}


void InsertWidget::setHPos(int xpos)
{
  m_ifield->setContentsPos((int)(xpos*m_ifield->worldMatrix().m11()), m_ifield->contentsY());
}

void InsertWidget::setVPos(int vpos)
{
  m_ifield->setContentsPos(m_ifield->contentsX(), (int)(vpos*m_ifield->worldMatrix().m22()));
}

void InsertWidget::setVisibleHRange(int left, int right)
{
  if (!m_updatingScrollBars)
  {
    right = max(left+2, right);
    double xf = ((double)m_ifield->width()-4) / ((right - left + 1));

    QMatrix m = m_ifield->worldMatrix();
    QMatrix newzoom(xf, m.m12(), m.m21(), m.m22(), m.dx(), m.dy());
    m_ifield->setWorldMatrix(newzoom); // visiblehrange

    setHPos(left);
  }
}

void InsertWidget::setVisibleVRange(int top, int bottom)
{
  if (!m_updatingScrollBars)
  {
    bottom = max(top+2, bottom);
    double yf = ((double)(m_ifield->height()-4)) / (bottom - top + 1);

    QMatrix m(m_ifield->worldMatrix());
    QMatrix newzoom(m.m11(), m.m12(), m.m21(), yf, m.dx(), m.dy());
    m_ifield->setWorldMatrix(newzoom); //visiblevrange

    setVPos(top);
  }
}

void InsertWidget::updateVisibleRange()
{
  m_updatingScrollBars = true;
  QRect rc = QRect(m_ifield->contentsX(),    m_ifield->contentsY(),
                   m_ifield->visibleWidth(), m_ifield->visibleHeight() );
  QRect real = m_ifield->inverseWorldMatrix().mapRect(rc);

  hrange->setRange((int)(real.x()), (int)(real.x()+real.width()));
  vrange->setRange((int)(real.y()), (int)(real.y()+real.height()));
  m_updatingScrollBars = false;
}


void InsertWidget::flushInserts()
{
  vector<Insert *>::iterator i;
  
  for (i =  m_inserts.begin();
       i != m_inserts.end();
       i++)
  {
    delete (*i);
  }

  m_inserts.clear();
}

void InsertWidget::computeInsertHappiness()
{
  flushInserts();
  m_datastore->calculateInserts(m_tiling, m_inserts, 1, 1);
  computeCoverage();

  if (!m_connectMates)
  {
    disconnectMates();
  }
}

void InsertWidget::clearCanvas()
{
  // clear and flush
  Q3CanvasItemList list = m_icanvas->allItems();
  Q3CanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it) {
      if ( *it )
          delete *it;
  }

  initializeVisibleRectangle();
  m_icanvas->update();
  emit canvasCleared();
}

void InsertWidget::initializeTiling()
{
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  if (m_kmerstats)
  {
    delete m_kmerstats;
    m_kmerstats = NULL;
  }

  m_features.clear();
  m_ctiling.clear();

  bool newScaffold = 
          (m_currentScaffold == AMOS::NULL_ID && (m_currentContig   != m_datastore->m_contigId)) ||
          (m_currentScaffold != AMOS::NULL_ID && (m_currentScaffold != m_datastore->m_scaffoldId));

  m_currentScaffold = m_datastore->m_scaffoldId;
  m_currentContig   = m_datastore->m_contigId;
  m_tiling.clear();

  if (m_paintScaffold && (m_currentScaffold != AMOS::NULL_ID))
  {
    Scaffold_t scaffold;
    m_datastore->fetchScaffoldBID(m_datastore->m_scaffoldId, scaffold);

    m_scaffoldId = scaffold.getIID();
    m_scaffoldName = scaffold.getEID().c_str();

    m_tilingwidth = scaffold.getSpan();
    
    m_ctiling = scaffold.getContigTiling();
    sort(m_ctiling.begin(), m_ctiling.end(), TileOrderCmp());

    if (m_kmercoverageplot && !m_datastore->mer_table.empty())
    {
      m_kmerstats = new CoverageStats(scaffold.getSpan(), 0, Distribution_t());
    }

    EventTime_t ctime;
    cerr << "Mapping read tiling for " << m_ctiling.size() << " contigs... ";

    int lendiff = 0;

    typedef map<ID_t, vector<Tile_t>::iterator > ContigTileMap;
    ContigTileMap contigTiles;

    vector<Tile_t>::iterator ci;
    for (ci = m_ctiling.begin(); ci != m_ctiling.end(); ci++)
    {
      Contig_t contig;
      m_datastore->fetchContigIID(ci->source, contig);

      contigTiles.insert(make_pair(contig.getIID(), ci));

      ci->offset += lendiff; // shift the start of the contig by the cummulative length difference

      int clen = contig.getLength();

      Range_t scaffrange = ci->range;

      // Ensure contig coordinates are gapped
      if (scaffrange.isReverse())
      {
        scaffrange.begin = scaffrange.end+clen;
        lendiff += scaffrange.begin - ci->range.begin;
      }
      else
      {
        scaffrange.end = scaffrange.begin+clen; 
        lendiff += scaffrange.end - ci->range.end;
      }

      ci->range = scaffrange;

      // Flip the contig if necessary
      if (scaffrange.isReverse())
      {
        contig.reverseComplement();
        scaffrange.swap();
      }

      if (m_kmerstats)
      {
        string cons = contig.getSeqString();

        // current mer between i..j
        DataStore::Mer_t fwd_mer = 0, rev_mer = 0;
        int merlen = 0;
        int j = 0; // 1 past where the mer ends

        for (int i = 0; i < clen; i++)
        {
          while(merlen < m_datastore->Kmer_Len && j < clen)
          {
            if (cons[j] != '-')
            {
              m_datastore->Forward_Add_Ch(fwd_mer, cons[j]);
              m_datastore->Reverse_Add_Ch(rev_mer, cons[j]);
              merlen++;
            }
            j++;
          }

          if (j >= clen) { break; }

          int mc = m_datastore->getMerCoverage(fwd_mer, rev_mer);
          if (mc > MAXMERCOUNT) { mc = MAXMERCOUNT; }
          m_kmerstats->addPoint(i+ci->offset, mc);

          if (cons[i] != '-') { merlen--; }
        }
      }

      vector<Tile_t> & rtiling = contig.getReadTiling();
      vector<Tile_t>::const_iterator ri;

      for (ri = rtiling.begin(); ri != rtiling.end(); ri++)
      {
        Tile_t mappedTile;
        mappedTile.source = ri->source;
        mappedTile.gaps   = ri->gaps;
        mappedTile.range  = ri->range;
        mappedTile.offset = ci->offset + ri->offset;

        m_tiling.push_back(mappedTile);
      }
    }

    cerr << "done. " << ctime.str() << endl;

    if (m_datastore->feat_bank.isOpen()) 
    {
      EventTime_t timer;
      cerr << "Loading Features... ";

      Feature_t feat;
      m_datastore->feat_bank.seekg(1);

      while (m_datastore->feat_bank >> feat)
      {
        if (feat.getSource().second == Scaffold_t::NCODE &&
            feat.getSource().first == scaffold.getIID())
        {
          m_features.push_back(feat);
        }
        else if (feat.getSource().second == Contig_t::NCODE)
        {
          ContigTileMap::iterator ctm = contigTiles.find(feat.getSource().first);
          
          if (ctm != contigTiles.end())
          {
            Range_t rng = feat.getRange( );

            if (ctm->second->range.isReverse())
            {
              rng.swap();
              rng.begin = (ctm->second->range.getLength() - feat.getRange().begin);
              rng.end = (ctm->second->range.getLength() - feat.getRange().end);
            }

            rng.begin += ctm->second->offset;
            rng.end   += ctm->second->offset;

            feat.setRange(rng);
            m_features.push_back(feat);
          }
        }
      }

      cerr << timer.str() << " " << m_features.size() << " features" << endl;
    }
  }
  else
  {
    if (m_kmercoverageplot && !m_datastore->mer_table.empty())
    {
      cerr << "Computing kmer coverage" << endl;
      string cons = m_datastore->m_contig.getSeqString();
      int clen = cons.size();

      m_kmerstats = new CoverageStats(clen, 0, Distribution_t());

      for (int i = 0; i < clen; i++)
      {
        DataStore::Mer_t fwd_mer = 0, rev_mer = 0;
        int merlen = 0;
        int j = i;

        while(merlen < m_datastore->Kmer_Len && j < clen)
        {
          if (cons[j] != '-')
          {
            m_datastore->Forward_Add_Ch(fwd_mer, cons[j]);
            m_datastore->Reverse_Add_Ch(rev_mer, cons[j]);
            merlen++;
          }
          j++;
        }

        if (j >= clen) { break; }

        int mc = m_datastore->getMerCoverage(fwd_mer, rev_mer);
        if (mc > MAXMERCOUNT) { mc = MAXMERCOUNT; }
        m_kmerstats->addPoint(i, mc);
      }
    }

    Tile_t currentContig;
    currentContig.source = m_datastore->m_contig.getIID();
    currentContig.offset = 0;
    currentContig.range = Range_t(0, m_datastore->m_contig.getLength());

    m_ctiling.push_back(currentContig);

    m_tilingwidth = m_datastore->m_contig.getLength();
    m_tiling      = m_datastore->m_contig.getReadTiling();

    if (m_datastore->feat_bank.isOpen())
    {
      Feature_t feat;
      m_datastore->feat_bank.seekg(1);
      
      while (m_datastore->feat_bank >> feat)
      {
        if (feat.getSource().second == Contig_t::NCODE &&
            feat.getSource().first == m_datastore->m_contig.getIID())
        {
          m_features.push_back(feat);
        }
      }
    }
  }

  sort(m_tiling.begin(), m_tiling.end(), TileOrderCmp());
  sort(m_features.begin(), m_features.end(), FeatOrderCmp());

  computeInsertHappiness();
  paintCanvas();

  if (newScaffold) { showAll(); }

  QApplication::restoreOverrideCursor();
}

typedef map <ID_t, QColor> ColorMap;
QColor getContigColor(ColorMap & contigColorMap, ID_t iid)
{
  ColorMap::iterator cci = contigColorMap.find(iid);

  if (cci == contigColorMap.end())
  {
    int s = contigColorMap.size();
    int i = s % strlen(Insert::allstates);
    int j = (s / strlen(Insert::allstates)) % 2;

    QColor color = UIElements::getInsertColor((Insert::MateState)Insert::allstates[i]);
    if (j == 1) { color = color.dark(200);  }

    cci = contigColorMap.insert(make_pair(iid, color)).first;
  }

  return cci->second;
}

void InsertWidget::computeCoverage()
{
  int leftmost = 0;
  if (!m_inserts.empty())
  {
    leftmost = min(0, (*m_inserts.begin())->m_loffset);
  }
  m_hoffset = -leftmost;

  //cerr << "m_hoffset: " << m_hoffset << endl;

  if (m_insertCL)  { delete m_insertCL;  }
  if (m_readCL)    { delete m_readCL;  }

  // coverage will change at each endpoint of each (happy) insert
  m_insertCL = new CoverageStats((2+m_inserts.size())*4, 0, Distribution_t());
  m_insertCL->addEndpoints(leftmost, leftmost);

  m_libStats.clear();
  LibStats::iterator li;

  int curloffset = 0, curroffset = 0;
  int totalinsertlen = 0;
  int rightmost = 0;

  int cestatsheight = 100;

  vector<Insert *>::iterator ii;

  for (ii = m_inserts.begin(); ii != m_inserts.end(); ii++)
  {
    curloffset = (*ii)->m_loffset;
    curroffset = (*ii)->m_roffset;

    if (curroffset > rightmost) { rightmost = curroffset; }

    // Only count happy mates towards insert coverage
    if ((*ii)->m_state == Insert::Happy)
    {
      totalinsertlen += (curroffset - curloffset + 1);
      m_insertCL->addEndpoints(curloffset, curroffset);
    }

    // CE connected inserts count towards ce val
    if ((*ii)->ceConnected())
    {
      li = m_libStats.find((*ii)->m_libid);

      if (li == m_libStats.end())
      {
        li = m_libStats.insert(make_pair((*ii)->m_libid, CoverageStats(m_inserts.size()*4, (*ii)->m_libid, (*ii)->m_dist))).first;
      }

      li->second.addEndpoints(curloffset, curroffset);
    }
  }

  m_insertCL->addEndpoints(rightmost,rightmost);
  m_insertCL->finalize();

  m_width = rightmost - leftmost + 1; 

  for (li = m_libStats.begin(); li != m_libStats.end(); li++)
  {
    li->second.finalize();
    li->second.finalizeCE(cestatsheight);
  }

  m_readCL = new CoverageStats(m_tiling.size()*4, 0, Distribution_t());

  int totalbases = 0;
  int readspan = 0;

  vector<Tile_t>::iterator ti;
  for (ti = m_tiling.begin(); ti != m_tiling.end(); ti++)
  {
    int len = ti->getGappedLength();

    curloffset = ti->offset;
    curroffset = ti->offset + len - 1;

    if (curroffset > readspan) { readspan = curroffset; }

    totalbases += len;

    m_readCL->addEndpoints(curloffset, curroffset);
  }

  m_readCL->finalize();

  int insertspan = (int)(m_insertCL->m_coverage[m_insertCL->m_curpos-1].x() - m_insertCL->m_coverage[0].x());
  m_meaninsertcoverage = (insertspan) ? ((double)totalinsertlen)/insertspan : 0;
  m_meanreadcoverage = ((double)totalbases) / readspan;
}


void InsertWidget::disconnectMates()
{
  int last = m_inserts.size();

  for (int i = 0; i < last; i++)
  {
    Insert * ins = m_inserts[i];
    if (ins->m_active == 2)
    {
      // disconnect the mates
      Insert * j = new Insert(ins->m_aid, ins->m_acontig, ins->m_atile,
                              ins->m_bid, ins->m_bcontig, ins->m_btile,
                              ins->m_libid, ins->m_dist, ins->m_matetype);

      j->setActive(1, ins, 0);
      m_inserts.push_back(j);

      ins->setActive(0, j, 0);
    }
    else
    {
      // disconnect the library
      ins->setActive(ins->m_active, ins->m_other, 0);
    }
  }

  sort(m_inserts.begin(), m_inserts.end(), Insert::TilingOrderCmp());
}


// You can't paint the entire coverage plot in one go because
// of the silly 16 bit limitation in qpainter/x11, so break the coverage
// into small pieces, and draw each separately.
    
void InsertWidget::paintCoverage(const PointArray_t & arr, 
                                 const vector<double> & rawvalues,
                                 bool copyRaw,
                                 int arrLen, 
                                 int voffset, int vheight,
                                 int libid,
                                 double baseLevel,
                                 QColor color,
                                 bool compressed)
{
  if (arr.empty()) { return; }

  QPen pen(color, 1.0);

  int i = 0;
  while (1)
  {
    int size = min(1000, (arrLen - i));
    Q3PointArray window(size);
    vector<double> windowraw(size);

    for (int j = 0; j < size; j++)
    {
      window[j].setX((int)((arr[i+j].x()+m_hoffset) * m_hscale));

      if (compressed) { window[j].setY(arr[i+j].y()); }
      else            { window[j].setY(voffset+vheight-arr[i+j].y()); }

      if (copyRaw) { windowraw[j] = rawvalues[i+j]; }
    }

    int width = window[size-1].x()-window[0].x()+1;

    if ( compressed )
    {
      new CoverageRectCanvasItem(window[0].x(), voffset,
                                 width, vheight, 
                                 libid, baseLevel,
                                 window, windowraw, copyRaw,
                                 m_icanvas, color);
    }
    else
    {
      CoverageCanvasItem * cci = new CoverageCanvasItem(window[0].x(), voffset,
                                                        width, vheight, 
                                                        libid, baseLevel,
                                                        window, windowraw, copyRaw,
                                                        m_icanvas, color);
      cci->setPen(pen);
    }

    i+= size;

    if (i >= arrLen) { break; }
    i-=10;
  }
}

void InsertWidget::paintCanvas()
{
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  clearCanvas();
  if (!m_datastore->m_loaded) { return; }


  int posoffset = 5;
  int gutter = 5;
  int voffset = posoffset+2*gutter;
  int lineheight = m_seqheight+gutter;

  int layoutgutter = 64;

  int layoutpos;
  vector<int> layout;
  vector<int>::iterator li;

  vector<Insert *>::iterator ii;
  vector<Tile_t>::const_iterator ci;

  cerr << "Paint:";

  ColorMap libColorMap;
  ColorMap contigColorMap;

  if (m_colorByLibrary || m_cestats)
  {
    unsigned int type = 0;

    DataStore::LibLookup_t::iterator li;
    for (li =  m_datastore->m_libdistributionlookup.begin();
         li != m_datastore->m_libdistributionlookup.end();
         li++)
    {
      libColorMap[li->first] = UIElements::getInsertColor((Insert::MateState)Insert::allstates[type]);
      type++;

      if (type >= strlen(Insert::allstates))
      {
        type = 0;
      }
    }
  }

  bool doPaintCoverage = true;
  bool doPaintContigs = true;
  bool doPaintFeatures = true;
  bool m_drawInserts = true;
  
  if (doPaintCoverage)
  {
    cerr << " coverage";

    int inswidth = (m_insertCL->m_curpos) ?  (int)((m_insertCL->m_coverage[m_insertCL->m_curpos-1].x() + m_hoffset) * m_hscale) : 0;
    int redwidth = (m_readCL->m_curpos) ? (int)((m_readCL->m_coverage[m_readCL->m_curpos-1].x() + m_hoffset) * m_hscale) : 0;

    int covwidth = max(inswidth, redwidth);

    int cestatsheight = 100;
    int covheight = max(m_insertCL->m_maxdepth, m_readCL->m_maxdepth);
    if (m_kmerstats) { covheight = max(covheight, m_kmerstats->m_maxdepth); }

    int cestatsoffset = voffset;

    if (m_coveragePlot)
    {
      cestatsoffset += covheight + 2*gutter;

      paintCoverage(m_insertCL->m_coverage, m_insertCL->m_cestat, false, m_insertCL->m_curpos,
                    voffset, covheight, -1, m_meaninsertcoverage, UIElements::color_insertcoverage);

      paintCoverage(m_readCL->m_coverage, m_readCL->m_cestat, false, m_readCL->m_curpos,
                    voffset, covheight, -2, m_meanreadcoverage, UIElements::color_readcoverage);

      if (m_kmerstats)
      {
        paintCoverage(m_kmerstats->m_coverage, m_kmerstats->m_cestat, false, m_kmerstats->m_curpos,
                      voffset, covheight, -3, 0.0, Qt::yellow);
      }
    }

    if (m_cestats)
    {
      LibStats::iterator li;
      for (li = m_libStats.begin(); li != m_libStats.end(); li++)
      {
        paintCoverage(li->second.m_coverage, li->second.m_cestat, true, li->second.m_curpos,
                      cestatsoffset, cestatsheight, (int)li->second.m_libid, (int)(cestatsheight/2),
                      (libColorMap[li->second.m_libid]));
      }
    }

    int i = 0;
    while (1)
    {
      int size = min(1000, covwidth - i);

      if (m_coveragePlot)
      {
        Q3CanvasRectangle * covbg = new Q3CanvasRectangle(i, voffset, 
                                                        size+1, covheight, m_icanvas);
        covbg->setBrush(QColor(60,60,60));
        covbg->setPen(QColor(60,60,60));
        covbg->setZ(-3);
        covbg->show();

        Q3CanvasLine * base = new Q3CanvasLine(m_icanvas);
        base->setPoints(i, voffset+covheight,
                        i+size, voffset+covheight);
        base->setPen(QPen(Qt::white));
        base->setZ(1);
        base->show();
      }

      if (m_cestats)
      {
        Q3CanvasRectangle * covbg = new Q3CanvasRectangle(i, cestatsoffset, 
                                                        size+1, cestatsheight, m_icanvas);
        covbg->setBrush(QColor(60,60,60));
        covbg->setPen(QColor(60,60,60));
        covbg->setZ(-4);
        covbg->show();

        covbg = new Q3CanvasRectangle(i, cestatsoffset + cestatsheight/2 - 3*8 , 
                                     size+1, 6*8, m_icanvas);
        covbg->setBrush(QColor(75,75,75));
        covbg->setPen(QColor(75,75,75));
        covbg->setZ(-3);
        covbg->show();
      }

      i+= size;

      if (i >= covwidth) { break; }
      i-=100;
    }

    if (m_coveragePlot) { voffset += covheight     + 2*gutter; }
    if (m_cestats)      { voffset += cestatsheight + 2*gutter; }
  }

  m_overviewtop = voffset - lineheight;

  if (doPaintContigs)
  {
    cerr << " contigs";
    layout.clear();

    if (m_showscaffold) { voffset += lineheight; }
    int rightmost = 0;
    int lastcontig = -1;

    for (ci = m_ctiling.begin(); ci != m_ctiling.end(); ci++)
    {
      int offset = ci->offset;

      // First fit into the layout
      for (li = layout.begin(), layoutpos = 0; 
           li != layout.end(); 
           li++, layoutpos++)
      {
        // force adjacent contigs to different lines
        if (layoutpos == lastcontig) { continue; } 

        // Otherwise, put into first row that fits
        if (*li < offset) { break; }
      }

      if (li == layout.end()) { layout.push_back(0); }
      layout[layoutpos] = offset + ci->range.getLength() + layoutgutter;
      lastcontig = layoutpos;

      if (offset+ci->range.getLength() > rightmost)
      {
        rightmost = offset+ci->range.getLength();
      }

      int vpos = voffset + layoutpos * lineheight;

      ContigCanvasItem * contig = new ContigCanvasItem((int)(m_hscale*(offset + m_hoffset)),
                                                       vpos,
                                                       (int) (ci->range.getLength() * m_hscale),
                                                       m_seqheight,
                                                       *ci, m_icanvas);
      contig->show();

      if (m_showscaffold)
      {
        Q3CanvasRectangle * rect = new Q3CanvasRectangle((int)(contig->x()+(contig->boundingRect().width())/2-3),
                                                       (int)(voffset-lineheight), 
                                                       6, vpos-voffset+lineheight+m_seqheight, m_icanvas);
        rect->setPen(contig->pen());
        rect->setBrush(contig->brush());
        rect->show();
      }
    }

    if (m_showscaffold)
    {
      int i = 0;
      while (i < rightmost)
      {
        int len = min(100000, rightmost-i);
        Q3CanvasRectangle * scaff = new Q3CanvasRectangle((int)(m_hscale*(m_hoffset+i)), voffset-lineheight,
                                                        (int)(len * m_hscale),
                                                         1, m_icanvas);
        scaff->setPen(UIElements::color_Scaffold);
        scaff->setBrush(UIElements::color_Scaffold);
        scaff->show();

        i += len;
      }
    }

    if (rightmost > m_width) { m_width = rightmost; }

    voffset += (layout.size() + 1) * lineheight;
  }


  if ( m_insertCovFeatures )
  {
    cerr << " insetcovfeat";

    if (m_insertCL->m_curpos)
    {
      paintCoverage(m_insertCL->m_coverage, m_insertCL->m_cestat, false, m_insertCL->m_curpos,
                    voffset, m_seqheight*2, -1, m_meaninsertcoverage, 
                    UIElements::color_insertcoverage, true);

      voffset += m_seqheight*2+gutter;

      int diff = (int) (m_insertCL->m_maxdepth - m_meaninsertcoverage);

      //cerr << "max: " << m_insertCL->m_maxdepth << endl;
      //cerr << "mean: " << m_meaninsertcoverage << endl;
      //cerr << "diff: " << diff << endl;

      emit newMaxInsertCovTol(((int)((diff > m_meaninsertcoverage) ? diff : m_meaninsertcoverage)));
    }
  }

  if ( m_readCovFeatures )
  {
    cerr << " readcovfeat";
    if (m_readCL->m_curpos)
    {
      paintCoverage(m_readCL->m_coverage, m_readCL->m_cestat, false, m_readCL->m_curpos,
                    voffset, m_seqheight*2, -2, m_meanreadcoverage,
                    UIElements::color_readcoverage, true);

      voffset += m_seqheight*2+gutter;
      emit newMaxReadCovTol((int)(m_readCL->m_maxdepth - m_meanreadcoverage >
                                  m_meanreadcoverage ?
                                  m_readCL->m_maxdepth - m_meanreadcoverage :
                                  m_meanreadcoverage));
    }
  }


  if (doPaintFeatures)
  {
    cerr << " features";
    layout.clear();

    int score = 0;
    vector<AMOS::Feature_t>::iterator fi;
    for (fi = m_features.begin(); fi != m_features.end(); fi++)
    {
      switch ( fi->getType() )
        {
        case AMOS::Feature_t::COVERAGE:
        case AMOS::Feature_t::POLYMORPHISM:
        case AMOS::Feature_t::BREAKPOINT:
        case AMOS::Feature_t::KMER:
        case AMOS::Feature_t::MATEPAIR:
        case AMOS::Feature_t::MISASSEMBLY:
          if ( !m_misasmFeatures ) continue;
          break;

        default:
          if ( !m_otherFeatures ) continue;
          break;
        }

      int offset = fi->getRange().getLo();
      int length = fi->getRange().getLength();

      if (fi->getType() == 'B')
      {
        length = 500;
        offset -= length/2;
      }

      // First fit into the layout
      for (li =  layout.begin(), layoutpos = 0;
           li != layout.end();
           li++, layoutpos++)
      {
        if (*li < offset) { break; }
      }

      if (li == layout.end()) { layout.push_back(0); }
      layout[layoutpos] = offset + length + layoutgutter;

      int vpos = voffset + layoutpos * lineheight;

      FeatureCanvasItem * fitem =
        new FeatureCanvasItem((int)(m_hscale * (offset+m_hoffset)), vpos,
                              (int)(m_hscale*length), m_seqheight,
                              *fi, m_icanvas);
      fitem->show();
    }

    if (!layout.empty())
    {
      voffset += (layout.size() + 1) * lineheight;
    }
  }

  m_overviewbottom = voffset;

  // bubblesort the types by the order they appear in the popup menu
  vector<char> types;
  map<char, pair<int, bool> >::iterator type;
  map<char, bool> drawType;

  for (type = m_types.begin(); type != m_types.end(); type++)
  {
    types.push_back(type->first);
    drawType[type->first] = type->second.second;
  }

  for (unsigned int i = 0; i < types.size(); i++)
  {
    for (unsigned int j = i+1; j < types.size(); j++)
    {
      if (m_types[types[j]].first > m_types[types[i]].first)
      {
        char swap = types[i];
        types[i] = types[j];
        types[j] = swap;
      }
    }
  }

  if (m_drawInserts)
  {
    cerr << " inserts";

    // For all types, or when !m_connectMates, do exactly 1 pass
    for (unsigned int type = 0; type < types.size(); type++)
    {
      if (m_partitionTypes && !m_types[types[type]].second) { continue; }

      layout.clear();

      // For all inserts of this type (or if type enabled)
      for (ii = m_inserts.begin(); ii != m_inserts.end(); ii++)
      {
        Insert::MateState istate = (*ii)->m_state;

        if (m_partitionTypes)
        {
          if (m_colorByStretchiness)
          {
            if (types[type] == (char)Insert::Happy)
            {
              if ((istate == Insert::Happy) || 
                  (istate == Insert::CompressedMate) || 
                  (istate == Insert::ExpandedMate)) { }
              else { continue; }
            }
            else if (istate == Insert::CompressedMate || istate == Insert::ExpandedMate) { continue; }
            else if ((char)istate != types[type]) { continue; }
          }
          else
          {
            if ((char)istate != types[type]) { continue; }
          }
        }
        else
        {
          if (!drawType[istate]) { continue; }
        }

        int offset = (*ii)->m_loffset;

        // Find a position
        for (li =  layout.begin(), layoutpos = 0;
             li != layout.end();
             li++, layoutpos++)
        {
          if (*li < offset) { break; }
        }

        if (li == layout.end()) { layout.push_back(0); }

        int vpos = voffset + layoutpos*lineheight;
        layout[layoutpos] = (*ii)->m_roffset + layoutgutter;

        int inserthpos =   (int)(m_hscale * ((*ii)->m_loffset + m_hoffset)); 
        int insertlength = (int)(m_hscale * ((*ii)->m_length-1));

        if (((*ii)->m_loffset + (*ii)->m_length - 1) != (*ii)->m_roffset)
        {
          cerr << "loffset: " << (*ii)->m_loffset
               << " length: " << (*ii)->m_length
               << " l+l: " << (*ii)->m_loffset + (*ii)->m_length - 1
               << " roffset: " << (*ii)->m_roffset << endl;
        }

        InsertCanvasItem * iitem = new InsertCanvasItem(inserthpos, vpos,
                                                        insertlength, m_seqheight,
                                                        *ii, m_icanvas);

        QColor insertcolor(UIElements::getInsertColor((*ii)->m_state));


        if (m_colorByLibrary)
        {
          insertcolor = Qt::cyan;

          ColorMap::const_iterator lci = libColorMap.find((*ii)->m_libid);

          if (lci != libColorMap.end())
          {
            insertcolor = lci->second;
          }
        }
        else if (m_colorByMate && ((*ii)->m_state == Insert::LinkingMate))
        {
          insertcolor = getContigColor(contigColorMap, (*ii)->m_bcontig);
        }
        else if (m_colorByStretchiness && 
                  (((*ii)->m_state == Insert::Happy) ||
                   ((*ii)->m_state == Insert::CompressedMate) ||
                   ((*ii)->m_state == Insert::ExpandedMate)))
        {
          double disttomean = (*ii)->m_actual - (*ii)->m_dist.mean;

          if (disttomean < 0)
          {
            insertcolor = UIElements::color_CompressedMate;
            disttomean = -disttomean;
          }
          else
          {
            insertcolor = UIElements::color_ExpandedMate;
          }

          disttomean /= (*ii)->m_dist.sd;

          disttomean += 1.5;

          disttomean *= disttomean;
          disttomean *= disttomean;

          if ( disttomean > 255 ) disttomean = 255;
          int ns = disttomean < 60 ? 60 : (int)disttomean;
          int nv = disttomean < 60 ? 60 : (int)disttomean;

          int h, s, v;
          insertcolor.hsv(&h,&s,&v);
          insertcolor.setHsv(h,ns,nv);
        }

        iitem->setPen(insertcolor);
        iitem->setBrush(insertcolor);

        iitem->show();
      }

      if (!layout.empty()) 
      { 
        if (m_tintHappiness && m_partitionTypes)
        {
          Q3CanvasRectangle * covbg = new Q3CanvasRectangle(0, voffset-3, 
                                                          (int)(m_hscale*(m_width)), 
                                                          (layout.size() * lineheight)+6,
                                                          m_icanvas);
          QColor color = UIElements::getInsertColor((Insert::MateState)types[type]);

          int h, s, v;
          color.hsv(&h, &s, &v);

          s /= 2; v /= 2;
          color.setHsv(h, s, v);

          covbg->setPen(color);

          s /= 2; v /= 2;
          color.setHsv(h, s, v);

          covbg->setBrush(color);
          covbg->setBrush(QBrush(color, Qt::Dense5Pattern));
          covbg->setZ(-2);
          covbg->show();
        }
        
        voffset += (layout.size() + 2) * lineheight;
      }

      if (!m_partitionTypes)
      {
        break;
      }
    }
  }

  int scaledwidth = (int)((m_width) * m_hscale);

  cerr << endl 
       << "width: " << m_width
       << " swidth: " << scaledwidth
       << " height: " << voffset;

  m_icanvas->resize(scaledwidth, voffset);
  cerr << ".";
  m_icanvas->update();
  cerr << "." << endl;


  setInsertCanvasSize(scaledwidth, voffset);

  bool old = m_syncWithTiling;
  m_syncWithTiling = false;
  setTilingVisibleRange(m_contigid, m_gstart, m_gend);
  m_syncWithTiling = old;

  resizeOverview();

  QApplication::restoreOverrideCursor();
}


void InsertWidget::resizeField()
{
  //setInsertCanvasSize(m_icanvas->width(), m_icanvas->height());

  setVisibleVRange(vrange->rangeLow(), vrange->rangeHigh());
  setVisibleHRange(hrange->rangeLow(), hrange->rangeHigh());
}

void InsertWidget::resizeOverview()
{
  double xf = (double)(m_overview->width() - 2*(hrange->controlPix_m + hrange->gripPix_m)) / m_icanvas->width();
  QMatrix matrix(xf, 0, 0, 1, hrange->controlPix_m+hrange->gripPix_m-2, 0);
  m_overview->setWorldMatrix(matrix); // resizeoverview
  m_overview->setContentsPos(0, m_overviewtop);
  m_overview->update();
  m_overview->setMaximumHeight(m_overviewbottom-m_overviewtop);
  m_overview->setMinimumHeight(m_overviewbottom-m_overviewtop);
}

void InsertWidget::setInsertCanvasSize(int cwidth, int cheight)
{
  m_updatingScrollBars = true;
  hrange->setMaxRange(0, cwidth);
  vrange->setMaxRange(0, cheight);
  m_updatingScrollBars = false;
}

void InsertWidget::showAll()
{
  hrange->setRange(hrange->minimum(), hrange->maximum());
  vrange->setRange(vrange->minimum(), vrange->maximum());
}


void InsertWidget::setHappyDistance(float distance)
{
  if ( Insert::MAXSTDEV == distance ) return;
  Insert::MAXSTDEV=distance;
  computeInsertHappiness();
  paintCanvas();
}

void InsertWidget::setConnectMates(bool b)
{
  if ( m_connectMates == b ) return;
  m_connectMates = b;
  computeInsertHappiness();
  paintCanvas();
}

void InsertWidget::setPartitionTypes(bool b)
{
  if ( m_partitionTypes == b ) return;
  m_partitionTypes = b;
  paintCanvas();
}

void InsertWidget::setCoveragePlot(bool b)
{
  if ( m_coveragePlot == b ) return;
  m_coveragePlot = b;
  paintCanvas();
} 

void InsertWidget::setCEStatistic(bool b)
{
  if ( m_cestats == b ) return;
  m_cestats = b;
  paintCanvas();
}

void InsertWidget::setTintHappiness(bool b)
{
  if ( m_tintHappiness == b ) return;
  m_tintHappiness = b;
  paintCanvas();
}

void InsertWidget::setPaintScaffold(bool b)
{
  if ( m_paintScaffold == b ) return;
  m_paintScaffold = b;
  initializeTiling();
}

void InsertWidget::setShowScaffold(bool b)
{
  if ( m_showscaffold == b ) return;
  m_showscaffold = b;
  paintCanvas();
}

void InsertWidget::setInsertCovFeatures(bool b)
{
  if ( m_insertCovFeatures == b ) return;
  m_insertCovFeatures = b;
  paintCanvas();
}

void InsertWidget::setReadCovFeatures(bool b)
{
  if ( m_readCovFeatures == b ) return;
  m_readCovFeatures = b;
  paintCanvas();
}

// void InsertWidget::setSNPFeatures(bool b)
// {
//   if (m_snpFeatures == b ) return;
//   m_snpFeatures = b;
//   paintCanvas();
// }

// void InsertWidget::setUnitigFeatures(bool b)
// {
//   if ( m_unitigFeatures == b ) return;
//   m_unitigFeatures = b;
//   paintCanvas();
// }

// void InsertWidget::setQCFeatures(bool b)
// {
//   if ( m_qcFeatures == b ) return;
//   m_qcFeatures = b;
//   paintCanvas();
// }

// void InsertWidget::setBreakFeatures(bool b)
// {
//   if ( m_breakFeatures == b ) return;
//   m_breakFeatures = b;
//   paintCanvas();
// }

void InsertWidget::setMisasmFeatures(bool b)
{
  if ( m_misasmFeatures == b ) return;
  m_misasmFeatures = b;
  paintCanvas();
}

void InsertWidget::setOtherFeatures(bool b)
{
  if ( m_otherFeatures == b ) return;
  m_otherFeatures = b;
  paintCanvas();
}


void InsertWidget::setInsertCovTol(int tol)
{
  Q3CanvasItemList all = m_icanvas->allItems();
  for ( Q3CanvasItemList::Iterator i = all.begin(); i != all.end(); ++ i )
    if ( (*i)->rtti() == CoverageRectCanvasItem::RTTI &&
         ((CoverageRectCanvasItem *)*i)->m_libid == -1 )
      {
        CoverageRectCanvasItem * ci = (CoverageRectCanvasItem *) *i;
        ci->m_low = ci->m_baseLevel - (double)tol;
        ci->m_high = ci->m_baseLevel + (double)tol;
        m_icanvas->setChanged(ci->boundingRect());
      }
  m_icanvas->update();
}

void InsertWidget::setReadCovTol(int tol)
{
  Q3CanvasItemList all = m_icanvas->allItems();
  for ( Q3CanvasItemList::Iterator i = all.begin(); i != all.end(); ++ i )
    if ( (*i)->rtti() == CoverageRectCanvasItem::RTTI &&
         ((CoverageRectCanvasItem *)*i)->m_libid == -2 )
      {
        CoverageRectCanvasItem * ci = (CoverageRectCanvasItem *) *i;
        ci->m_low = ci->m_baseLevel - (double)tol;
        ci->m_high = ci->m_baseLevel + (double)tol;
        m_icanvas->setChanged(ci->boundingRect());
      }
  m_icanvas->update();
}

// void InsertWidget::setSNPTol(int tol)
// {
//   QCanvasItemList all = m_icanvas->allItems();
//   for ( QCanvasItemList::Iterator i = all.begin(); i != all.end(); ++ i )
//     if ( (*i)->rtti() == FeatureCanvasItem::RTTI &&
//          ((FeatureCanvasItem *)*i)->m_feat.getType() ==
//          AMOS::Feature_t::POLYMORPHISM )
//       {
//         FeatureCanvasItem * fi = (FeatureCanvasItem *) *i;
//         if ( extractSNPScore (fi->m_feat) >= tol )
//           fi->show();
//         else
//           fi->hide();
//         m_icanvas->setChanged(fi->boundingRect());
//       }
//   m_icanvas->update();
// }

// void InsertWidget::setUnitigTol(int tol)
// {
//   QCanvasItemList all = m_icanvas->allItems();
//   for ( QCanvasItemList::Iterator i = all.begin(); i != all.end(); ++ i )
//     if ( (*i)->rtti() == FeatureCanvasItem::RTTI &&
//          ((FeatureCanvasItem *)*i)->m_feat.getType() ==
//          AMOS::Feature_t::UNITIG )
//       {
//         FeatureCanvasItem * fi = (FeatureCanvasItem *) *i;
//         if ( extractUnitigScore (fi->m_feat) >= tol )
//           fi->show();
//         else
//           fi->hide();
//         m_icanvas->setChanged(fi->boundingRect());
//       }
//   m_icanvas->update();
// }

// void InsertWidget::setQCTol(int tol)
// {
//   QCanvasItemList all = m_icanvas->allItems();
//   for ( QCanvasItemList::Iterator i = all.begin(); i != all.end(); ++ i )
//     if ( (*i)->rtti() == FeatureCanvasItem::RTTI &&
//          ((FeatureCanvasItem *)*i)->m_feat.getType() ==
//          AMOS::Feature_t::COVERAGE )
//       {
//         FeatureCanvasItem * fi = (FeatureCanvasItem *) *i;
//         if ( extractQCScore (fi->m_feat) >= tol )
//           fi->show();
//         else
//           fi->hide();
//         m_icanvas->setChanged(fi->boundingRect());
//       }
//   m_icanvas->update();
// }

// void InsertWidget::setBreakTol(int tol)
// {
//   QCanvasItemList all = m_icanvas->allItems();
//   for ( QCanvasItemList::Iterator i = all.begin(); i != all.end(); ++ i )
//     if ( (*i)->rtti() == FeatureCanvasItem::RTTI &&
//          ((FeatureCanvasItem *)*i)->m_feat.getType() ==
//          AMOS::Feature_t::BREAKPOINT )
//       {
//         FeatureCanvasItem * fi = (FeatureCanvasItem *) *i;
//         if ( extractBreakScore (fi->m_feat) >= tol )
//           fi->show();
//         else
//           fi->hide();
//         m_icanvas->setChanged(fi->boundingRect());
//       }
//   m_icanvas->update();
// }


class Paddle : public Q3CanvasRectangle
{
public:
  static const int RTTI = 987654;
  Paddle(int x, int y, int width, int height, Q3Canvas * canvas)
   : Q3CanvasRectangle(x,y,width,height,canvas) {}
  int rtti() const {return RTTI; }
};

static int min(int a, int b)
{
  return (a<b) ? a : b;
}

void InsertWidget::start()
{
  stopbreak();

  QRect rc = QRect(m_ifield->contentsX(),    m_ifield->contentsY(),
                   m_ifield->visibleWidth(), m_ifield->visibleHeight() );
  QRect rect = m_ifield->inverseWorldMatrix().mapRect(rc);

  int basey = min(rect.y()+rect.height(), m_icanvas->height());

  m_ball = new Q3CanvasEllipse(7, 5, m_icanvas);
  m_ball->setX(rect.x() + rect.width()/2);
  m_ball->setY(basey - 50);
  m_ball->setPen(QPen(Qt::white));
  m_ball->setBrush(QBrush(Qt::white));
  m_ball->show();

  m_paddle = new Paddle((int)(m_ball->x()-25), (int)(m_ball->y()+25), 
                        50, 5, m_icanvas);
  m_paddle->setPen(QPen(Qt::white));
  m_paddle->setBrush(QBrush(Qt::white));
  m_paddle->show();

  m_xvel = rand() % 25 - 12;
  m_yvel = -1;

  m_icanvas->update();

  m_timer->start(20);
}

void InsertWidget::left()
{
  if (m_paddle)
  {
    m_paddle->moveBy(-10, 0);
    m_icanvas->update();
  }
}

void InsertWidget::right()
{
  if (m_paddle)
  {
    m_paddle->moveBy(10, 0);
    m_icanvas->update();
  }
}


void InsertWidget::stopbreak()
{
  m_timer->stop();
  Q3CanvasItemList list = m_icanvas->allItems();
  Q3CanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it) {
    (*it)->show();
  }

  if (m_paddle) { m_paddle->hide(); delete m_paddle; m_paddle = NULL; }
  if (m_ball)   { m_ball->hide();   delete m_ball; m_ball = NULL;}

  m_icanvas->update();
}

void InsertWidget::autoplay()
{
  m_autoplay = !m_autoplay;
}


void InsertWidget::timeout()
{
  if (m_ball)
  {
    QRect rc = QRect(m_ifield->contentsX(),    m_ifield->contentsY(),
                     m_ifield->visibleWidth(), m_ifield->visibleHeight() );
    QRect rect = m_ifield->inverseWorldMatrix().mapRect(rc);

    if (m_ball->y() >= min(rect.y()+rect.height(), m_icanvas->height()))
    {
      m_timer->stop();
      QMessageBox mb("Sorry!", "Game Over!", 
                     QMessageBox::Critical, QMessageBox::Ok, 
                     QMessageBox::NoButton, QMessageBox::NoButton);
      mb.exec();
      stopbreak();
    }
    else
    {
      if (m_ball->y() <= rect.y())
      {
        m_yvel = -m_yvel;
      }
      else if ((m_ball->x() <= rect.x()) ||
               (m_ball->x() >= min(rect.x()+rect.width(), m_icanvas->width())))
      {
        m_xvel = -m_xvel;
      }
      else
      {
        Q3CanvasItemList l = m_icanvas->collisions(m_ball->boundingRect());// m_ball->collisions(true);
        bool hit = false;

        int left   = (int)(m_ball->x());
        int right  = (int)(m_ball->x() + m_ball->width());
        int top    = (int)(m_ball->y());
        int bottom = (int)(m_ball->y() + m_ball->height());

        for (Q3CanvasItemList::Iterator li=l.begin(); li != l.end(); li++)
        {
          if ((right  < (*li)->x()) ||
              (left   > ((*li)->x() + (*li)->boundingRect().width())) ||
              (bottom < (*li)->y()) ||
              (top    > ((*li)->y() + (*li)->boundingRect().height())))
          {
            continue;
          }

          if ((*li)->rtti() == InsertCanvasItem::RTTI)
          {
            //cerr << "i";
            (*li)->hide();
            hit = true;
          }
          else if ((*li)->rtti() == Paddle::RTTI)
          {
            //cerr << "p";
            m_xvel += rand()%10-5;
            hit = true;
          }

          if (hit)
          {
            m_yvel = -m_yvel;
            m_ball->moveBy(0, 4*m_yvel);
            break;
          }
        }
      }

      if (abs(m_xvel) > 25) { m_xvel = m_xvel/2; }

      m_ball->moveBy(m_xvel, m_yvel);

      if (m_autoplay)
      {
        m_paddle->move(m_ball->x()-m_paddle->width()/2, m_paddle->y());
      }

      m_icanvas->update();
    }
  }
}

void InsertWidget::contigChanged()
{
  if (m_paintScaffold && 
      m_currentScaffold == m_datastore->m_scaffoldId &&
      m_currentScaffold != AMOS::NULL_ID)
  {
    return;
  }

  initializeTiling();
}

void InsertWidget::refreshWidget()
{
  initializeTiling();
}

void InsertWidget::setColorStyle(int style)
{
  m_colorByLibrary = 0; 
  m_colorByMate = 0;
  m_colorByStretchiness = 0; 

  switch (style)
  {
    case 1: m_colorByStretchiness = 1; break;
    case 2: m_colorByMate = 1;         break;
    case 3: m_colorByLibrary = 1;      break;
  };

  paintCanvas();
}
