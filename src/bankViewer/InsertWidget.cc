#include "InsertWidget.hh"

#include <set>
#include <qlayout.h>
#include <qlabel.h>
#include <qwmatrix.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qaccel.h>
#include "RenderSeq.hh"
#include <qapplication.h>
#include "InsertCanvasItem.hh"
#include "CoverageCanvasItem.hh"
#include "FeatureCanvasItem.hh"
#include "ContigCanvasItem.hh"
#include "UIElements.hh"
#include "CoverageStats.hh"
#include "InsertField.hh"
#include "InsertPosition.hh"
#include "DataStore.hh"



using namespace AMOS;
using namespace std;
typedef HASHMAP::hash_map<ID_t, Tile_t *> SeqTileMap_t;


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

  QBoxLayout * vbox = new QVBoxLayout(this);

  m_hscale = .06250;

  m_iposition = new InsertPosition(m_datastore, this, "insertposition");
  m_icanvas = new QCanvas(this, "icanvas");
  m_icanvas->setBackgroundColor(Qt::black);

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
  m_cestats        = 0;
  m_showFeatures   = 1;
  m_paintScaffold  = 1;
  m_colorByLibrary = 0;
  m_colorByMate    = 0;
  m_tintHappiness  = 0;
  m_tintFeatures   = 0;
  m_showscaffold   = 1;
  m_kmercoverageplot = 1;

  m_kmerstats = NULL;

  m_persistant = false;
  m_error = 0;


  m_currentScaffold = AMOS::NULL_ID;

  m_seqheight = 4;
  m_tilingwidth = 0;

  m_ifield = new InsertField(datastore, m_hoffset, m_icanvas, this, "qcv");

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

  vbox->addWidget(m_iposition);
  vbox->addWidget(m_ifield, 10);
  vbox->activate();

  connect(m_ifield, SIGNAL(visibleRange(int, double)),
          m_iposition, SLOT(setVisibleRange(int,double)));

  connect(this,    SIGNAL(currentScaffoldCoordinate(int)),
          m_iposition, SLOT(setScaffoldCoordinate(int)));

  connect(m_ifield, SIGNAL(setStatus(const QString &)),
          this,     SIGNAL(setStatus(const QString &)));

  connect(m_ifield, SIGNAL(setGindex(int)),
          this,     SLOT(computePos(int)));

  QAccel *a = new QAccel( this );
  a->connectItem(a->insertItem(CTRL+SHIFT+Key_S), this, SLOT(start()) );
  a->connectItem(a->insertItem(Key_Left),         this, SLOT(left()) );
  a->connectItem(a->insertItem(Key_Right),        this, SLOT(right()) );
  a->connectItem(a->insertItem(Key_Escape),       this, SLOT(stopbreak()));
  a->connectItem(a->insertItem(CTRL+SHIFT+Key_A), this, SLOT(autoplay()) );

  initializeTiling();
  m_ifield->show();
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
  m_tilingVisible = new QCanvasRectangle(m_icanvas);
  m_tilingVisible->setZ(-1);

  m_tilingVisible->setBrush(QColor(200,200,100));
  m_tilingVisible->setPen(QColor(200,200,100));
}

InsertWidget::~InsertWidget()
{
  flushInserts();
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
  QRect rc = QRect(m_ifield->contentsX(), m_ifield->contentsY(),
                   m_ifield->visibleWidth(), m_ifield->visibleHeight());
  QRect canvasRect = m_ifield->inverseWorldMatrix().mapRect(rc);

  m_tilingVisible->setSize((int)(m_hscale*(gend - gstart)) +1, m_icanvas->height());
  m_tilingVisible->move((int)(m_hscale*(gstart+m_hoffset)), 0);
  m_tilingVisible->show();

  // ensure visible
  int mapx, mapy;
  m_ifield->worldMatrix().map((int)(m_hscale*((gstart + gend)/2+m_hoffset)), 
                              (int)(canvasRect.y() + canvasRect.height()/2),
                              &mapx, &mapy);
  m_ifield->ensureVisible(mapx, mapy);

  m_icanvas->update();

  emit currentScaffoldCoordinate(gstart);
}

void InsertWidget::setZoom(int zoom)
{
  double xfactor = 16.00/(zoom);

  if (zoom > 16)
  {
    xfactor = pow(xfactor, zoom/16);
  }

  QWMatrix matrix = m_ifield->worldMatrix();
  QWMatrix imatrix = m_ifield->inverseWorldMatrix();

  // reset to identity
  m_ifield->setWorldMatrix(imatrix);

  // set zoom
  QWMatrix newzoom(xfactor, matrix.m12(), matrix.m21(), matrix.m22(),
                   matrix.dx(), matrix.dy());
  m_ifield->setWorldMatrix(newzoom);

  setTilingVisibleRange(m_contigid, m_gstart, m_gend);
}

void InsertWidget::setVZoom(int vzoom)
{
  double yfactor = 16.0/vzoom;

  QWMatrix matrix = m_ifield->worldMatrix();
  QWMatrix imatrix = m_ifield->inverseWorldMatrix();

  // reset to identity
  m_ifield->setWorldMatrix(imatrix);

  // set zoom
  QWMatrix newzoom(matrix.m11(), matrix.m12(), matrix.m21(), yfactor,
                   matrix.dx(), matrix.dy());
  m_ifield->setWorldMatrix(newzoom);
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
  m_datastore->calculateInserts(m_tiling, m_inserts, m_connectMates, 1);
}

void InsertWidget::clearCanvas()
{
  // clear and flush
  QCanvasItemList list = m_icanvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
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
  m_currentScaffold = m_datastore->m_scaffoldId;
  m_tiling.clear();

  if (m_paintScaffold && m_currentScaffold != AMOS::NULL_ID)
  {
    Scaffold_t scaffold;
    m_datastore->fetchScaffold(m_datastore->m_scaffoldId, scaffold);

    m_tilingwidth = scaffold.getSpan();
    
    if (m_datastore->feat_bank.isOpen())
    {
      Feature_t feat;
      m_datastore->feat_bank.seekg(1);

      while (m_datastore->feat_bank >> feat)
      {
        if (feat.getSource().second == Scaffold_t::NCODE &&
            feat.getSource().first == scaffold.getIID())
        {
          m_features.push_back(feat);
        }
      }
    }

    m_ctiling = scaffold.getContigTiling();
    sort(m_ctiling.begin(), m_ctiling.end(), TileOrderCmp());

    if (m_kmercoverageplot && !m_datastore->mer_table.empty())
    {
      m_kmerstats = new CoverageStats(scaffold.getSpan(), 0, Distribution_t());
    }

    cerr << "Mapping read tiling for " << m_ctiling.size() << " contigs... ";

    int lendiff = 0;

    vector<Tile_t>::iterator ci;
    for (ci = m_ctiling.begin(); ci != m_ctiling.end(); ci++)
    {
      Contig_t contig;
      m_datastore->fetchContig(ci->source, contig);

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
          if (mc > 200) { mc = 200; }
          m_kmerstats->addPoint(i+ci->offset, mc);
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

      if (m_datastore->feat_bank.isOpen()) 
      {
        Feature_t feat;
        m_datastore->feat_bank.seekg(1);

        while (m_datastore->feat_bank >> feat)
        {
          if (feat.getSource().second == Contig_t::NCODE &&
              feat.getSource().first == contig.getIID())
          {
            Range_t rng = feat.getRange( );

            if (ci->range.isReverse())
            {
              rng.swap();
              rng.begin = (ci->range.getLength() - feat.getRange().begin);
              rng.end = (ci->range.getLength() - feat.getRange().end);
            }

            rng.begin += ci->offset;
            rng.end   += ci->offset;

            feat.setRange(rng);
            m_features.push_back(feat);
          }
        }
      }
    }

    cerr << "done." << endl;
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
        if (mc > 200) { mc = 200; }
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

    //cerr << "new color: " << iid << " s: " << s << "i: " << i << " j: " << j << endl;

    QColor color = UIElements::getInsertColor((Insert::MateState)Insert::allstates[i]);
    if (j == 1) { color = color.dark(200);  }

    cci = contigColorMap.insert(make_pair(iid, color)).first;
  }

  return cci->second;
}


// You can't paint the entire coverage plot in one go because
// of the silly 16 bit limitation in qpainter/x11, so break the coverage
// into small pieces, and draw each separately.
    
void InsertWidget::paintCoverage(QPointArray & arr, 
                                 vector<double> & rawvalues,
                                 bool copyRaw,
                                 int arrLen, 
                                 int voffset, int vheight,
                                 int libid,
                                 double baseLevel,
                                 QColor color)
{
  if (arr.isEmpty()) { return; }

  int i = 0;
  while (1)
  {
    int size = min(1000, (arrLen - i));
    QPointArray window(size);
    vector<double> windowraw(size);

    for (int j = 0; j < size; j++)
    {
      window[j] = arr[i+j];
      if (copyRaw)
      {
        windowraw[j] = rawvalues[i+j];
      }
    }

    int width = window[size-1].x()-window[0].x()+1;
    new CoverageCanvasItem(window[0].x(), voffset,
                           width, vheight, 
                           libid, baseLevel,
                           window, windowraw, copyRaw,
                           m_icanvas, color);

    i+= size;

    if (i >= arrLen) { break; }
    i--;
  }
}

void InsertWidget::paintCanvas()
{
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  clearCanvas();
  if (!m_datastore->m_loaded) { return; }


  m_hoffset = 0; 

  int posoffset = 5;
  int gutter = 5;
  int voffset = posoffset+2*gutter;
  int lineheight = m_seqheight+gutter;

  int layoutgutter = 64;

  int leftmost = 0;
  int rightmost = m_tilingwidth;

  int layoutpos;
  vector<int> layout;
  vector<int>::iterator li;

  vector<Insert *>::iterator ii;
  vector<Tile_t>::const_iterator ci;

  if (!m_inserts.empty())
  {
    leftmost = (*m_inserts.begin())->m_loffset;
    if (leftmost > 0) { leftmost = 0; }
  }


  m_hoffset = -leftmost;

  cerr << "Paint:";

  if (0)
  {
    cerr << " Ticks";

    // Draw the scaffold/contig line
    QCanvasLine * line = new QCanvasLine(m_icanvas);
    line->setPoints((int)(m_hoffset * m_hscale), posoffset, 
                    (int)((m_tilingwidth + m_hoffset) * m_hscale), posoffset);
    line->setPen(QPen(Qt::white, 2));
    line->show();

    // Draw ticks
    for (int i = 0; i < m_tilingwidth; i+=1000)
    {
      line = new QCanvasLine(m_icanvas);

      line->setPoints((int)(m_hscale * (m_hoffset+i)), posoffset-2,
                      (int)(m_hscale * (m_hoffset+i)), posoffset+2);
      line->setPen(Qt::white);
      line->show();

      if ((m_tilingwidth - i < 1000) && (i != m_tilingwidth-1))
      {
        // draw a tick at the end of the contig in the next round
        i = m_tilingwidth - 1 - 1000;
      }
    }
  }

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
  
  if (m_coveragePlot || m_cestats)
  {
    cerr << " coverage";

    // coverage will change at each endpoint of each insert
    CoverageStats insertCL(m_inserts.size()*4+8, 0, Distribution_t());
    insertCL.addEndpoints(leftmost, leftmost);

    typedef map<ID_t, CoverageStats> LibStats;
    LibStats libStats;
    LibStats::iterator li;

    int curloffset = 0, curroffset = 0;
    int totalinsertlen = 0;
    int rightmost = 0;

    for (ii = m_inserts.begin(); ii != m_inserts.end(); ii++)
    {
      if ((*ii)->m_roffset > rightmost) { rightmost = (*ii)->m_roffset; }

      // Only count reasonablyConnected mates towards insert coverage
      if ((*ii)->reasonablyConnected())
      {
        curloffset = (*ii)->m_loffset;
        curroffset = (*ii)->m_roffset;

        totalinsertlen += (curroffset - curloffset + 1);

        insertCL.addEndpoints(curloffset, curroffset);

        if (m_cestats && (*ii)->ceConnected())
        {
          li = libStats.find((*ii)->m_libid);

          if (li == libStats.end())
          {
            li = libStats.insert(make_pair((*ii)->m_libid, CoverageStats(m_inserts.size()*4, (*ii)->m_libid, (*ii)->m_dist))).first;
          }

          li->second.addEndpoints(curloffset, curroffset);
        }
      }
    }

    insertCL.addEndpoints(rightmost,rightmost);


    insertCL.finalize();

    CoverageStats readCL(m_tiling.size()*4, 0, Distribution_t());

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

      readCL.addEndpoints(curloffset, curroffset);
    }

    readCL.finalize();

    int inswidth = (int)((insertCL.m_coverage[insertCL.m_curpos-1].x() + m_hoffset) * m_hscale);
    int redwidth = (int)((readCL.m_coverage[readCL.m_curpos-1].x() + m_hoffset) * m_hscale);

    int covwidth = max(inswidth, redwidth);

    double meaninsertcoverage = ((double)totalinsertlen) / (insertCL.m_coverage[insertCL.m_curpos-1].x() - insertCL.m_coverage[0].x());
    double meanreadcoverage = ((double)totalbases) / readspan;

    int cestatsheight = 100;
    int covheight = max(insertCL.m_maxdepth, readCL.m_maxdepth);
    int cestatsoffset = voffset;

    if (m_kmerstats && m_kmerstats->m_maxdepth > covheight)
    {
      covheight = max(covheight, m_kmerstats->m_maxdepth);
    }

    if (m_coveragePlot)
    {
      cestatsoffset += covheight + 2*gutter;

      insertCL.normalize(m_hscale, m_hoffset, voffset + covheight);
      readCL.normalize(m_hscale, m_hoffset, voffset + covheight);
      
      paintCoverage(insertCL.m_coverage, insertCL.m_cestat, false,
                    insertCL.m_curpos,
                    voffset, covheight,
                    -1, meaninsertcoverage, 
                    UIElements::color_insertcoverage);

      paintCoverage(readCL.m_coverage, readCL.m_cestat, false,
                    readCL.m_curpos,
                    voffset, covheight, 
                    -2, meanreadcoverage,
                    UIElements::color_readcoverage);

      if (m_kmerstats)
      {
        CoverageStats kmer(*m_kmerstats);

        kmer.m_coverage = m_kmerstats->m_coverage.copy();

        kmer.normalize(m_hscale, m_hoffset, voffset + covheight);
        paintCoverage(kmer.m_coverage, kmer.m_cestat, false,
                      kmer.m_curpos,
                      voffset, covheight, 
                      -3, 0.0,
                      Qt::yellow);
      }
    }

    if (m_cestats)
    {
      for (li = libStats.begin(); li != libStats.end(); li++)
      {
        li->second.finalize();
        li->second.finalizeCE(cestatsheight);
        li->second.normalize(m_hscale, m_hoffset, cestatsoffset + cestatsheight);
        paintCoverage(li->second.m_coverage, li->second.m_cestat, true,
                      li->second.m_curpos,
                      cestatsoffset, cestatsheight,
                      (int)li->second.m_libid, (int)(cestatsheight/2),
                      (libColorMap[li->second.m_libid]));
      }
    }

    int i = 0;
    while (1)
    {
      int size = min(1000, covwidth - i);

      if (m_coveragePlot)
      {
        QCanvasRectangle * covbg = new QCanvasRectangle(i, voffset, 
                                                        size+1, covheight, m_icanvas);
        covbg->setBrush(QColor(60,60,60));
        covbg->setPen(QColor(60,60,60));
        covbg->setZ(-3);
        covbg->show();

        QCanvasLine * base = new QCanvasLine(m_icanvas);
        base->setPoints(i, voffset+covheight,
                        i+size, voffset+covheight);
        base->setPen(Qt::white);
        base->setZ(1);
        base->show();
      }

      if (m_cestats)
      {
        QCanvasRectangle * covbg = new QCanvasRectangle(i, cestatsoffset, 
                                                        size+1, cestatsheight, m_icanvas);
        covbg->setBrush(QColor(60,60,60));
        covbg->setPen(QColor(60,60,60));
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

  if (1)
  {
    cerr << " contigs";
    layout.clear();

    if (m_showscaffold) { voffset += lineheight; }
    int rightmost = 0;

    for (ci = m_ctiling.begin(); ci != m_ctiling.end(); ci++)
    {
      int offset = ci->offset;

      // First fit into the layout
      for (li = layout.begin(), layoutpos = 0; 
           li != layout.end(); 
           li++, layoutpos++)
      {
        if (*li < offset) { break; }
      }

      if (li == layout.end()) { layout.push_back(0); }
      layout[layoutpos] = offset + ci->range.getLength() + layoutgutter;

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
        QCanvasRectangle * rect = new QCanvasRectangle(contig->x()+(contig->boundingRect().width())/2-3, voffset-lineheight, 
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
        QCanvasRectangle * scaff = new QCanvasRectangle((int)(m_hscale*(m_hoffset+i)), voffset-lineheight,
                                                        (int)(len * m_hscale),
                                                         1, m_icanvas);
        scaff->setPen(Qt::blue);
        scaff->setBrush(Qt::blue);
        scaff->show();

        i += len;
      }
    }

    voffset += (layout.size() + 1) * lineheight;
  }

  if (m_showFeatures && !m_features.empty())
  {
    cerr << " features";
    layout.clear();

    vector<AMOS::Feature_t>::iterator fi;
    for (fi = m_features.begin(); fi != m_features.end(); fi++)
    {
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

      FeatureCanvasItem * fitem = new FeatureCanvasItem((int)(m_hscale * (offset+m_hoffset)), vpos,
                                                        (int)(m_hscale*length), m_seqheight,
                                                        *fi, m_icanvas);
      fitem->show();
    }

    if (!layout.empty())
    {
      voffset += (layout.size() + 1) * lineheight;
    }
  }

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


  int m_drawInserts = 1;
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
        if (m_partitionTypes)
        {
          if (((char)(*ii)->m_state) != types[type]) { continue; }
        }
        else
        {
          if (!drawType[(*ii)->m_state]) { continue; }
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

        if ((*ii)->m_roffset > rightmost) { rightmost = (*ii)->m_roffset; }

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

        if (m_persistant)
        {
          iitem->m_contigcolor = true;

          iitem->m_alinkedread = m_datastore->getPersistantRead((*ii)->m_aid, m_error);
          iitem->m_alinked = m_datastore->lookupContigId(iitem->m_alinkedread);
          iitem->m_acolor = getContigColor(contigColorMap, iitem->m_alinked);

          if ((*ii)->m_bid)
          {
            iitem->m_blinkedread = m_datastore->getPersistantRead((*ii)->m_bid, m_error);
            iitem->m_blinked = m_datastore->lookupContigId(iitem->m_blinkedread);
            iitem->m_bcolor = getContigColor(contigColorMap, iitem->m_blinked);
          }
        }
        else if (m_colorByLibrary)
        {
          insertcolor = Qt::cyan;

          ColorMap::const_iterator lci = libColorMap.find((*ii)->m_libid);

          if (lci != libColorMap.end())
          {
            insertcolor = lci->second;
          }
        }
        else if (m_colorByMate && ((*ii)->m_state == Insert::MissingMate))
        {
          insertcolor = getContigColor(contigColorMap, (*ii)->m_bcontig);
        }

        iitem->setPen(insertcolor);
        iitem->setBrush(insertcolor);

        iitem->show();
      }

      if (!layout.empty()) 
      { 
        if (m_tintHappiness && m_partitionTypes)
        {
          QCanvasRectangle * covbg = new QCanvasRectangle(0, voffset-3, 
                                                          (int)(m_hscale*(rightmost-leftmost)), 
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

  if (m_showFeatures && m_tintFeatures)
  {
    vector<AMOS::Feature_t>::iterator fi;
    for (fi = m_features.begin(); fi != m_features.end(); fi++)
    {
      int offset = fi->getRange().getLo();
      QCanvasRectangle * rect = new QCanvasRectangle((int)((m_hoffset+offset) * m_hscale), 0, 
                                                     (int)(fi->getRange().getLength() * m_hscale), voffset, 
                                                     m_icanvas);
      rect->setBrush(QColor(59,49,31));
      rect->setPen(QColor(139,119,111));
      rect->setZ(-2);
      rect->show();
    }
  }

  cerr << endl 
       << "width: " << rightmost-leftmost
       << " swidth: " << (int)((rightmost-leftmost) * m_hscale)
       << " voffset: " << voffset;

  m_icanvas->resize((int)(m_hscale*(rightmost - leftmost)) + 500, voffset);
  cerr << ".";
  m_icanvas->update();
  cerr << "." << endl;

  setTilingVisibleRange(m_contigid, m_gstart, m_gend);

  QApplication::restoreOverrideCursor();
}

void InsertWidget::setHappyDistance(float distance)
{
  Insert::MAXSTDEV=distance;
  computeInsertHappiness();
  paintCanvas();
}

void InsertWidget::setConnectMates(bool b)
{
  m_connectMates = b;
  initializeTiling();
}

void InsertWidget::setPartitionTypes(bool b)
{
  m_partitionTypes = b;
  paintCanvas();
}

void InsertWidget::setCoveragePlot(bool b)
{
  m_coveragePlot = b;
  paintCanvas();
} 

void InsertWidget::setCEStatistic(bool b)
{
  m_cestats = b;
  paintCanvas();
}

void InsertWidget::setTintHappiness(bool b)
{
  m_tintHappiness = b;
  paintCanvas();
}

void InsertWidget::setPaintScaffold(bool b)
{
  m_paintScaffold = b;
  initializeTiling();
}

void InsertWidget::setFeatures(bool b)
{
  m_showFeatures = b;
  paintCanvas();
}

void InsertWidget::setTintFeatures(bool b)
{
  m_tintFeatures = b;
  paintCanvas();
}

void InsertWidget::setColorByLibrary(bool b)
{
  m_colorByLibrary = b;
  paintCanvas();
}

void InsertWidget::setColorByMate(bool b)
{
  m_colorByMate = b;
  paintCanvas();
}

void InsertWidget::setPersistant(bool b)
{
  m_persistant = b;
  paintCanvas();
}

void InsertWidget::setErrorRate(int error)
{
  m_error = error;
  paintCanvas();
}

void InsertWidget::setShowScaffold(bool b)
{
  m_showscaffold = b;
  paintCanvas();
}

class Paddle : public QCanvasRectangle
{
public:
  static const int RTTI = 987654;
  Paddle(int x, int y, int width, int height, QCanvas * canvas)
   : QCanvasRectangle(x,y,width,height,canvas) {}
  int rtti() const {return RTTI; }
};

static int min(int a, int b)
{
  return (a<b) ? a : b;
}

void InsertWidget::start()
{
  QRect rc = QRect(m_ifield->contentsX(),    m_ifield->contentsY(),
                   m_ifield->visibleWidth(), m_ifield->visibleHeight() );
  QRect rect = m_ifield->inverseWorldMatrix().mapRect(rc);

  int basey = min(rect.y()+rect.height(), m_icanvas->height());

  m_ball = new QCanvasEllipse(7, 5, m_icanvas);
  m_ball->setX(rect.x() + rect.width()/2);
  m_ball->setY(basey - 15);
  m_ball->setPen(Qt::white);
  m_ball->setBrush(Qt::white);
  m_ball->show();

  m_paddle = new Paddle((int)(m_ball->x()-16), (int)(m_ball->y()+5), 
                        50, 5, m_icanvas);
  m_paddle->setPen(Qt::white);
  m_paddle->setBrush(Qt::white);
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
  QCanvasItemList list = m_icanvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
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
        QCanvasItemList l = m_ball->collisions(true);
        bool hit = false;

        for (QCanvasItemList::Iterator li=l.begin(); li != l.end(); li++)
        {
          if ((*li)->rtti() == InsertCanvasItem::RTTI)
          {
            (*li)->hide();
            m_yvel = -m_yvel;
            hit = true;
          }
          else if ((*li)->rtti() == Paddle::RTTI)
          {
            m_yvel = -m_yvel;
            m_xvel += rand()%10-5;
            hit = true;
          }
        }

        if (hit)
        {
          m_ball->moveBy(0, 3*m_yvel);
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
