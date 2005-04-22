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


using namespace AMOS;
using namespace std;
typedef std::map<ID_t, Tile_t *> SeqTileMap_t;

class CoverageLevel
{
public:
  CoverageLevel(QPointArray & coverage):
     m_coverage(coverage),
     m_maxdepth(0),
     m_curpos(0)
  { }

  void addEndpoints(int curloffset, int curroffset)
  {
    multiset<int>::iterator vi, vi2;

    // find end points that have already passed
    vi = m_endpoints.begin();
    while (vi != m_endpoints.end())
    {
      if (*vi <= curloffset) 
      { 
        m_coverage[m_curpos++]=QPoint(*vi, m_endpoints.size());
        m_coverage[m_curpos++]=QPoint(*vi, m_endpoints.size()-1);
        vi2 = vi; vi2++; m_endpoints.erase(vi); vi = vi2;
      }
      else
      { 
        break; 
      }
    }

    // Add this insert's beginning and end
    m_coverage[m_curpos++]=QPoint(curloffset, m_endpoints.size());
    m_coverage[m_curpos++]=QPoint(curloffset, m_endpoints.size()+1);
    m_endpoints.insert(curroffset);

    if (m_endpoints.size() > (unsigned int) m_maxdepth) 
    { 
      m_maxdepth = m_endpoints.size(); 
    }
  }

  void finalize()
  {
    multiset<int>::iterator vi, vi2;

    // Handle remaining end points
    vi = m_endpoints.begin();
    while (vi != m_endpoints.end())
    {
      m_coverage[m_curpos++]=QPoint(*vi, m_endpoints.size());
      m_coverage[m_curpos++]=QPoint(*vi, m_endpoints.size()-1);
      vi2 = vi; vi2++; m_endpoints.erase(vi); vi = vi2;
    }
  }

  void normalize(float hscale, int hoffset, int voffset)
  {
    int size = m_coverage.size();

    // Adjust coordinates for painting
    for (int i = 0; i < size; i++)
    {
      m_coverage[i].setX((int)((m_coverage[i].x()+hoffset) * hscale));
      m_coverage[i].setY(voffset-m_coverage[i].y());
    }
  }

  QPointArray & m_coverage;
  int m_maxdepth;
  int m_curpos;

  multiset<int> m_endpoints;
};

struct FeatOrderCmp
{
  bool operator() (const AMOS::Feature_t & a, const AMOS::Feature_t & b)
  {
    int aoffset = a.range.isReverse() ? a.range.end : a.range.begin;
    int boffset = b.range.isReverse() ? b.range.end : b.range.begin;

    int offdiff = boffset - aoffset;

    if (offdiff)
    {
      if (offdiff < 0) { return false; }
      return true;
    }

    int lendiff = (b.range.getLength()) -
                  (a.range.getLength());

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

  m_iposition = new InsertPosition(this, "insertposition");
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
  m_showFeatures   = 1;
  m_paintScaffold  = 1;
  m_colorByLibrary = 0;

  m_currentScaffold = AMOS::NULL_ID;

  m_seqheight = 3;
  m_tilingwidth = 0;

  m_ifield = new InsertField(datastore, m_hoffset, m_icanvas, this, "qcv");

  connect(this, SIGNAL(highlightIID(const QString &)),
          m_ifield, SLOT(highlightIID(const QString &)));

  connect(this, SIGNAL(highlightEID(const QString &)),
          m_ifield, SLOT(highlightEID(const QString &)));

  connect(m_ifield, SIGNAL(readEIDHighlighted(const QString &)),
          this,     SIGNAL(readEIDHighlighted(const QString &)));

  connect(m_ifield, SIGNAL(readIIDHighlighted(const QString &)),
          this,     SIGNAL(readIIDHighlighted(const QString &)));

  vbox->addWidget(m_iposition);
  vbox->addWidget(m_ifield, 10);
  vbox->activate();

  connect(m_ifield, SIGNAL(visibleRange(int, double)),
          m_iposition, SLOT(setVisibleRange(int,double)));

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
  SeqTileMap_t seqtileLookup;

  int mated = 0;
  int unmated = 0;
  int matelisted = 0;

  vector<Tile_t>::iterator ti;

  // map iid -> tile * (within this contig)
  for (ti =  m_tiling.begin();
       ti != m_tiling.end();
       ti++)
  {
    seqtileLookup[ti->source] = &(*ti);
  }

  cerr << "Loading mates" << endl;
  Insert * insert;
  DataStore::MateLookupMap::iterator mi;

  ProgressDots_t dots(seqtileLookup.size(), 50);
  int count = 0;

  // For each read in the contig
  SeqTileMap_t::iterator ai;
  for (ai =  seqtileLookup.begin();
       ai != seqtileLookup.end();
       ai++)
  {
    count++;
    dots.update(count);

    if (ai->second == NULL)
    {
      //cerr << "Skipping already seen read" << endl;
      continue;
    }

    ID_t aid = ai->first;
    ID_t acontig = m_datastore->lookupContigId(aid);
    Tile_t * atile = ai->second;

    AMOS::ID_t libid = m_datastore->getLibrary(aid);
    AMOS::Distribution_t dist = m_datastore->getLibrarySize(libid);

    // Does it have a mate
    mi = m_datastore->m_readmatelookup.find(aid);
    if (mi == m_datastore->m_readmatelookup.end())
    {
      unmated++;
      insert = new Insert(aid, acontig, atile,
                          AMOS::NULL_ID, AMOS::NULL_ID, NULL,
                          libid, dist, m_tilingwidth, 
                          AMOS::Matepair_t::NULL_MATE);
    }
    else
    {
      matelisted++;

      ID_t bid = mi->second.first;
      ID_t bcontig = AMOS::NULL_ID;
      Tile_t * btile = NULL;
      bcontig = m_datastore->lookupContigId(bid);

      SeqTileMap_t::iterator bi = seqtileLookup.find(bid);

      if (bi != seqtileLookup.end())
      {
        mated++;

        btile = bi->second;
        bi->second = NULL;
      }

      insert = new Insert(aid, acontig, atile, 
                          bid, bcontig, btile, 
                          libid, dist, m_tilingwidth, 
                          mi->second.second);


      if (m_connectMates && insert->reasonablyConnected())
      {
        // A and B are within this contig, and should be drawn together
        insert->m_active = 2;
      }
      else if (btile)
      {
        // A and B are within this contig, but not reasonably connected
        Insert * j = new Insert(*insert);
        j->setActive(1, insert, m_connectMates);
        m_inserts.push_back(j);

        insert->setActive(0, j, m_connectMates);
      }
      else 
      { 
        // Just A is valid
        insert->setActive(0, NULL, m_connectMates); 
      }
    }

    m_inserts.push_back(insert);

    // Mark that this read has been taken care of already
    ai->second = NULL;
  }

  dots.end();

  sort(m_inserts.begin(), m_inserts.end(), Insert::TilingOrderCmp());

  cerr << "mated: "   << mated 
       << " matelisted: " << matelisted
       << " unmated: " << unmated << endl;
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
}

void InsertWidget::initializeTiling()
{
  m_features.clear();
  m_ctiling.clear();
  m_currentScaffold = m_datastore->m_scaffoldId;
  m_tiling.clear();

  if (m_paintScaffold && m_datastore->m_scaffoldId != AMOS::NULL_ID)
  {
    Scaffold_t scaffold;
    m_datastore->fetchScaffold(m_datastore->m_scaffoldId, scaffold);

    m_tilingwidth = scaffold.getSpan();

    m_ctiling = scaffold.getContigTiling();
    sort(m_ctiling.begin(), m_ctiling.end(), RenderSeq_t::TilingOrderCmp());

    cerr << "Mapping read tiling for " << m_ctiling.size() << " contigs... ";

    vector<Tile_t>::const_iterator ci;
    for (ci = m_ctiling.begin(); ci != m_ctiling.end(); ci++)
    {
      Contig_t contig;
      m_datastore->fetchContig(ci->source, contig);

      int clen = contig.getLength();

      vector<Tile_t> & rtiling = contig.getReadTiling();
      vector<Tile_t>::const_iterator ri;

      for (ri = rtiling.begin(); ri != rtiling.end(); ri++)
      {
        Tile_t mappedTile;
        mappedTile.source = ri->source;
        mappedTile.gaps   = ri->gaps;
        mappedTile.range  = ri->range;

        int offset = ri->offset;
        if (ci->range.isReverse())
        {
          mappedTile.range.swap();
          offset = clen - (offset + ri->range.getLength() + ri->gaps.size());
        }

        mappedTile.offset = ci->offset + offset;

        m_tiling.push_back(mappedTile);
      }

      const vector<Feature_t> & cfeats = contig.getFeatures();
      vector<Feature_t>::const_iterator fi;
      for (fi = cfeats.begin(); fi != cfeats.end(); fi++)
      {
        Feature_t feat(*fi);

        if (ci->range.isReverse())
        {
          feat.range.swap();

          feat.range.begin = (ci->range.getLength() - feat.range.begin);
          feat.range.end   = (ci->range.getLength() - feat.range.end);
        }

        feat.range.begin += ci->offset;
        feat.range.end   += ci->offset;

        m_features.push_back(feat);
      }
    }

    cerr << "done." << endl;
  }
  else
  {
    Tile_t currentContig;
    currentContig.source = m_datastore->m_contig.getIID();
    currentContig.offset = 0;
    currentContig.range = Range_t(0, m_datastore->m_contig.getLength());

    m_ctiling.push_back(currentContig);

    m_tilingwidth = m_datastore->m_contig.getLength();
    m_tiling      = m_datastore->m_contig.getReadTiling();

    m_features = m_datastore->m_contig.getFeatures();
  }

  sort(m_tiling.begin(), m_tiling.end(), RenderSeq_t::TilingOrderCmp());
  sort(m_features.begin(), m_features.end(), FeatOrderCmp());

  computeInsertHappiness();
  paintCanvas();
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

  if (m_coveragePlot)
  {
    cerr << " coverage";

    // coverage will change at each endpoint of each insert
    QPointArray insertCoverage(m_inserts.size()*4); 
    CoverageLevel insertCL(insertCoverage);

    int curloffset = 0, curroffset = 0;

    for (ii = m_inserts.begin(); ii != m_inserts.end(); ii++)
    {
      curloffset = (*ii)->m_loffset;
      curroffset = (*ii)->m_roffset;

      insertCL.addEndpoints(curloffset, curroffset);
    }

    insertCL.finalize();
    int covwidth = (int)((insertCoverage[insertCoverage.count()-1].x() + m_hoffset) * m_hscale);

    QPointArray readCoverage(m_tiling.size()*4); 
    CoverageLevel readCL(readCoverage);

    vector<Tile_t>::iterator ti;
    for (ti = m_tiling.begin(); ti != m_tiling.end(); ti++)
    {
      curloffset = ti->offset;
      curroffset = ti->offset + ti->range.getLength() + ti->gaps.size() - 1;

      readCL.addEndpoints(curloffset, curroffset);
    }

    readCL.finalize();

    insertCL.normalize(m_hscale, m_hoffset, voffset + insertCL.m_maxdepth);
    readCL.normalize(m_hscale, m_hoffset, voffset + insertCL.m_maxdepth);


    // You can't paint the entire coverage plot in one go because
    // of the silly 16 bit limitation in qpainter/x11, so break the coverage
    // into small pieces, and draw each separately.

    int i = 0;
    while (1)
    {
      int size = min(1000, (int)(insertCoverage.count() - i));
      QPointArray window(size);
      for (int j = 0; j < size; j++)
      {
        window[j] = insertCoverage[i+j];
      }
      new CoverageCanvasItem(window[0].x(), voffset,
                             window[size-1].x()-window[0].x()+1, insertCL.m_maxdepth, 
                             true, window, m_icanvas);
      i+= size;

      if (i == insertCoverage.count()) { break; }
      i--;
    }

    i = 0;
    while (1)
    {
      int size = min(1000, (int)(readCoverage.count() - i));
      QPointArray window(size);
      for (int j = 0; j < size; j++)
      {
        window[j] = readCoverage[i+j];
      }

      int width = window[size-1].x()-window[0].x()+1;

      new CoverageCanvasItem(window[0].x(), voffset,
                             width, insertCL.m_maxdepth, 
                             false, window, m_icanvas);
      i+= size;

      if (i == readCoverage.count()) { break; }
      i--;
    }

    i = 0;
    while (1)
    {
      int size = min(1000, covwidth - i);
      QCanvasRectangle * covbg = new QCanvasRectangle(i, voffset, 
                                                      size+1, insertCL.m_maxdepth, m_icanvas);
      covbg->setBrush(QColor(60,60,60));
      covbg->setPen(QColor(60,60,60));
      covbg->setZ(-2);
      covbg->show();

      QCanvasLine * base = new QCanvasLine(m_icanvas);
      base->setPoints(i, voffset+insertCL.m_maxdepth, 
                      i+size, voffset+insertCL.m_maxdepth);
      base->setPen(Qt::white);
      base->setZ(1);
      base->show();
      i+= size;

      if (i >= covwidth) { break; }
      i-=100;
    }

    voffset += insertCL.m_maxdepth + 2*gutter;
  }

  if (1)
  {
    cerr << " contigs";
    layout.clear();

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

      int vpos = voffset + layoutpos * lineheight;

      ContigCanvasItem * contig = new ContigCanvasItem((int)(m_hscale*(offset + m_hoffset)),
                                                       vpos,
                                                       (int) (ci->range.getLength() * m_hscale),
                                                       m_seqheight,
                                                       *ci, m_icanvas);
      contig->show();
    }

    voffset += (layout.size() + 1) * lineheight;
  }

  if (m_showFeatures)
  {
    cerr << " features";
    layout.clear();

    vector<AMOS::Feature_t>::iterator fi;
    for (fi = m_features.begin(); fi != m_features.end(); fi++)
    {
      int offset = fi->range.isReverse() ? fi->range.end : fi->range.begin;

      // First fit into the layout
      for (li =  layout.begin(), layoutpos = 0;
           li != layout.end();
           li++, layoutpos++)
      {
        if (*li < offset) { break; }
      }

      if (li == layout.end()) { layout.push_back(0); }
      layout[layoutpos] = offset + fi->range.getLength() + layoutgutter;

      int vpos = voffset + layoutpos * lineheight;

      FeatureCanvasItem * fitem = new FeatureCanvasItem((int)(m_hscale * (offset+m_hoffset)), vpos,
                                                        (int)(m_hscale*fi->range.getLength()), m_seqheight,
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

  typedef map <ID_t, QColor> LibColorMap;
  LibColorMap libColorMap;

  if (m_colorByLibrary)
  {
    unsigned int type = 0;

    map<ID_t, Distribution_t>::const_iterator li;
    for (li =  m_datastore->m_libdistributionlookup.begin();
         li != m_datastore->m_libdistributionlookup.end();
         li++)
    {
      libColorMap[li->first] = UIElements::getInsertColor((Insert::MateState)types[type]);
      type++;

      if (type >= types.size())
      {
        type = 0;
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
          if ((*ii)->m_state != types[type]) { continue; }
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
        int insertlength = (int)(m_hscale * (*ii)->m_length);


        InsertCanvasItem * iitem = new InsertCanvasItem(inserthpos, vpos,
                                                        insertlength, m_seqheight,
                                                        *ii, m_icanvas);
        if (m_colorByLibrary)
        {
          QColor insertcolor(Qt::cyan);

          LibColorMap::const_iterator lci = libColorMap.find((*ii)->m_libid);

          if (lci != libColorMap.end())
          {
            insertcolor = lci->second;
          }

          iitem->setPen(insertcolor);
          iitem->setBrush(insertcolor);
        }
        else
        {
          iitem->setPen(UIElements::getInsertColor((*ii)->m_state));
          iitem->setBrush(UIElements::getInsertColor((*ii)->m_state));
        }

        iitem->show();
      }

      if (!layout.empty()) 
      { 
        voffset += (layout.size() + 1) * lineheight;
      }

      if (!m_partitionTypes)
      {
        break;
      }
    }
  }

  if (m_showFeatures && 0)
  {
    vector<AMOS::Feature_t>::iterator fi;
    for (fi = m_features.begin(); fi != m_features.end(); fi++)
    {
      int offset = fi->range.isReverse() ? fi->range.end : fi->range.begin;
      QCanvasRectangle * rect = new QCanvasRectangle((int)((m_hoffset+offset) * m_hscale), 0, 
                                                     (int)(fi->range.getLength() * m_hscale), voffset, 
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

void InsertWidget::setColorByLibrary(bool b)
{
  m_colorByLibrary = b;
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



