#include "InsertWidget.hh"

#include <qlayout.h>
#include <qlabel.h>
#include <qwmatrix.h>
#include <qmessagebox.h>
#include "RenderSeq.hh"
#include "InsertCanvasItem.hh"
#include "CoverageCanvasItem.hh"
#include "FeatureCanvasItem.hh"
#include <set>


using namespace AMOS;
using namespace std;
typedef std::map<ID_t, Tile_t *> SeqTileMap_t;

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

  m_iposition = new InsertPosition(this, "insertposition");
  m_icanvas = new QCanvas(this, "icanvas");
  m_icanvas->setBackgroundColor(Qt::black);

  m_paddle = NULL;
  m_ball = NULL;
  m_timer = new QTimer(this, "timer");
  connect(m_timer, SIGNAL(timeout()),
          this, SLOT(timeout()));

  m_hoffset = 0;
  m_connectMates = 1;
  m_partitionTypes = 1;
  m_coveragePlot = 1;
  m_showFeatures = 1;

  refreshCanvas();

  m_ifield = new InsertField(datastore, m_hoffset, m_icanvas, this, "qcv");
  m_ifield->show();

  connect(this, SIGNAL(highlightIID(const QString &)),
          m_ifield, SLOT(highlightIID(const QString &)));

  connect(this, SIGNAL(highlightEID(const QString &)),
          m_ifield, SLOT(highlightEID(const QString &)));

  vbox->addWidget(m_iposition);
  vbox->addWidget(m_ifield, 10);
  vbox->activate();

  connect(m_ifield, SIGNAL(visibleRange(int, double)),
          m_iposition, SLOT(setVisibleRange(int,double)));

  connect(m_ifield, SIGNAL(setStatus(const QString &)),
          this,     SIGNAL(setStatus(const QString &)));

  connect(m_ifield, SIGNAL(setGindex(int)),
          this,     SIGNAL(setGindex(int)));
}

void InsertWidget::initializeVisibleRectangle()
{
  m_tilingVisible = new QCanvasRectangle(m_icanvas);
  m_tilingVisible->setZ(-1);
  m_tilingVisible->setBrush(QColor(100,100,100));
  m_tilingVisible->setPen(QColor(100,100,100));
}

InsertWidget::~InsertWidget()
{
  flushInserts();
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

void InsertWidget::setTilingVisibleRange(int gstart, int gend)
{
  // resize and place rectangle
  QRect rc = QRect(m_ifield->contentsX(), m_ifield->contentsY(),
                   m_ifield->visibleWidth(), m_ifield->visibleHeight());
  QRect canvasRect = m_ifield->inverseWorldMatrix().mapRect(rc);

  m_tilingVisible->setSize(gend - gstart +1, m_icanvas->height());
  m_tilingVisible->move(gstart+m_hoffset, 0);
  m_tilingVisible->show();

  // ensure visible
  int mapx, mapy;
  m_ifield->worldMatrix().map((gstart + gend)/2+m_hoffset, 
                              canvasRect.y() + canvasRect.height()/2,
                              &mapx, &mapy);
  m_ifield->ensureVisible(mapx, mapy);

  m_icanvas->update();
}

void InsertWidget::setZoom(int zoom)
{
  double xfactor = 1.00/zoom;

  QWMatrix matrix = m_ifield->worldMatrix();
  QWMatrix imatrix = m_ifield->inverseWorldMatrix();

  // reset to identity
  m_ifield->setWorldMatrix(imatrix);

  // set zoom
  QWMatrix newzoom(xfactor, matrix.m12(), matrix.m21(), matrix.m22(),
                   matrix.dx(), matrix.dy());
  m_ifield->setWorldMatrix(newzoom);
}

void InsertWidget::refreshCanvas()
{
  // clear and flush
  QCanvasItemList list = m_icanvas->allItems();
  QCanvasItemList::Iterator it = list.begin();
  for (; it != list.end(); ++it) {
      if ( *it )
          delete *it;
  }

  flushInserts();
  initializeVisibleRectangle();
  m_icanvas->update();

  // now draw
  if (!m_datastore->m_loaded) { return; }

  m_seqheight = 3;
  m_hoffset = 0; 

  int posoffset = 5;
  int gutter = 5;
  int tileoffset = posoffset+2*gutter;
  int lineheight = m_seqheight+gutter;

  int layoutgutter = 50;

  int clen = m_datastore->m_contig.getSeqString().size();
  SeqTileMap_t seqtileLookup;

  int mated = 0;
  int unmated = 0;
  int matelisted = 0;

  m_tiling = m_datastore->m_contig.getReadTiling();
  sort(m_tiling.begin(), m_tiling.end(), RenderSeq_t::TilingOrderCmp());

  try
  {
    // map iid -> tile * (within this contig)
    vector<Tile_t>::iterator vi;
    for (vi =  m_tiling.begin();
         vi != m_tiling.end();
         vi++)
    {
      seqtileLookup[vi->source] = &(*vi);
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
      ID_t acontig = m_datastore->m_contigId;
      Tile_t * atile = ai->second;

      AMOS::Distribution_t dist = m_datastore->getLibrarySize(aid);

      // Does it have a mate
      mi = m_datastore->m_readmatelookup.find(aid);
      if (mi == m_datastore->m_readmatelookup.end())
      {
        unmated++;
        insert = new Insert(aid, acontig, atile,
                            AMOS::NULL_ID, AMOS::NULL_ID, NULL,
                            dist, clen, AMOS::Matepair_t::NULL_MATE);
      }
      else
      {
        matelisted++;

        ID_t bid = mi->second.first;
        ID_t bcontig = AMOS::NULL_ID;
        Tile_t * btile = NULL;

        SeqTileMap_t::iterator bi = seqtileLookup.find(bid);

        if (bi == seqtileLookup.end())
        {
          bcontig = m_datastore->lookupContigId(bid);
        }
        else
        {
          mated++;

          btile = bi->second;
          bi->second = NULL;
          bcontig = m_datastore->m_contigId;
        }

        insert = new Insert(aid, acontig, atile, 
                            bid, bcontig, btile, 
                            dist, clen, 
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

    cerr << "mated: "   << mated 
         << " matelisted: " << matelisted
         << " unmated: " << unmated << endl;
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  int leftmost = 0;
  int rightmost = clen;

  vector<Insert *>::iterator ii;

  if (!m_inserts.empty())
  {
    sort(m_inserts.begin(), m_inserts.end(), Insert::TilingOrderCmp());

    leftmost = (*m_inserts.begin())->m_loffset;
    if (leftmost > 0) { leftmost = 0; }
  }


  m_hoffset = -leftmost;

  // Draw the consensus
  QCanvasLine * line = new QCanvasLine(m_icanvas);
  line->setPoints(m_hoffset,        posoffset, 
                  m_hoffset + clen, posoffset);
  line->setPen(QPen(Qt::white, 2));
  line->show();

  // Draw ticks
  for (int i = 0; i < clen; i+=1000)
  {
    line = new QCanvasLine(m_icanvas);

    line->setPoints(m_hoffset + i, posoffset-2,
                    m_hoffset + i, posoffset+2);
    line->setPen(Qt::white);
    line->show();

    if ((clen - i < 1000) && (i != clen-1))
    {
      // draw a tick at the end of the contig in the next round
      i = clen - 1 - 1000;
    }
  }

  cerr << "Paint:";

  if (m_coveragePlot)
  {
    cerr << " coverage";
    int maxdepth = 0;
    int maxroffset = 0;

    multiset<int> endpoints;
    multiset<int>::iterator vi, vi2;

    // coverage will change at each endpoint of each insert
    QPointArray coveragelevel(m_inserts.size()*4); 
    int curcpos = 0;

    for (ii = m_inserts.begin(); ii != m_inserts.end(); ii++)
    {
      int curloffset = (*ii)->m_loffset;
      int curroffset = (*ii)->m_roffset;

      // find end points that have already passed
      vi = endpoints.begin();
      while (vi != endpoints.end())
      {
        if (*vi <= curloffset) 
        { 
          coveragelevel[curcpos++]=QPoint(*vi, endpoints.size());
          coveragelevel[curcpos++]=QPoint(*vi, endpoints.size()-1);
          vi2 = vi; vi2++; endpoints.erase(vi); vi = vi2;
        }
        else
        { 
          break; 
        }
      }

      // Add this insert's beginning and end
      coveragelevel[curcpos++]=QPoint(curloffset, endpoints.size());
      coveragelevel[curcpos++]=QPoint(curloffset, endpoints.size()+1);
      endpoints.insert(curroffset);

      if (endpoints.size() > maxdepth) 
      { 
        maxdepth = endpoints.size(); 
      }

      if (curroffset > maxroffset)
      {
        maxroffset = curroffset;
      }
    }

    // Handle remaining end points
    vi = endpoints.begin();
    while (vi != endpoints.end())
    {
      coveragelevel[curcpos++]=QPoint(*vi, endpoints.size());
      coveragelevel[curcpos++]=QPoint(*vi, endpoints.size()-1);
      vi2 = vi; vi2++; endpoints.erase(vi); vi = vi2;
    }

    // Adjust coordinates for painting
    for (int i = 0; i < curcpos; i++)
    {
      coveragelevel[i].setX(coveragelevel[i].x()+m_hoffset);
      coveragelevel[i].setY((maxdepth-coveragelevel[i].y()) + tileoffset);
    }

    QCanvasRectangle * bg = new QCanvasRectangle(0, tileoffset, maxroffset + m_hoffset + 1, maxdepth, m_icanvas);
    bg->setBrush(QColor(60,60,60));
    bg->setZ(-2);
    bg->show();

    QCanvasLine * base = new QCanvasLine(m_icanvas);
    base->setPoints(0, tileoffset+maxdepth, maxroffset+m_hoffset+1, tileoffset+maxdepth);
    base->setPen(Qt::white);
    base->show();

    CoverageCanvasItem * citem = new CoverageCanvasItem(0, tileoffset,
                                                        maxroffset + m_hoffset + 1, maxdepth,
                                                        coveragelevel, m_icanvas);
    citem->show();

    tileoffset += maxdepth + gutter;
  }

  if (m_showFeatures)
  {
    cerr << " features";
    vector<AMOS::Feature_t> & feats = m_datastore->m_contig.getFeatures();
    sort(feats.begin(), feats.end(), FeatOrderCmp());

    int layoutpos;
    vector<int> layout;
    vector<int>::iterator li;

    vector<AMOS::Feature_t>::iterator fi;
    for (fi = feats.begin(); fi != feats.end(); fi++)
    {
      int offset = fi->range.isReverse() ? fi->range.end : fi->range.begin;

      // First fit into the layout
      for (li =  layout.begin(), layoutpos = 0;
           li != layout.end();
           li++, layoutpos++)
      {
        if (*li < offset)
        {
          break;
        }
      }

      if (li == layout.end()) { layout.push_back(0); }
      layout[layoutpos] = offset + fi->range.getLength() + layoutgutter;

      int vpos = tileoffset + layoutpos * lineheight;

      FeatureCanvasItem * fitem = new FeatureCanvasItem(m_hoffset+offset, vpos,
                                                        fi->range.getLength(), m_seqheight,
                                                        *fi, m_icanvas);
      fitem->show();
    }

    if (!layout.empty())
    {
      tileoffset += (layout.size() + 1) * lineheight;
    }
  }

  // bubblesort the types by the order they appear in the popup menu
  vector<char> types;
  map<char, pair<int, bool> >::iterator ti;
  map<char, bool> drawType;

  for (ti = m_types.begin(); ti != m_types.end(); ti++)
  {
    types.push_back(ti->first);
    drawType[ti->first] = ti->second.second;
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


  cerr << " inserts" << endl;
  int layoutoffset = 0;

  vector<int>::iterator li;
  int layoutpos;

  // For all types, or when !m_connectMates, do exactly 1 pass
  for (unsigned int type = 0; type < types.size(); type++)
  {
    if (m_partitionTypes && !m_types[types[type]].second) { continue; }

    vector<int> layout;

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

      // Find a position
      for (li =  layout.begin(), layoutpos = 0;
           li != layout.end();
           li++, layoutpos++)
      {
        if (*li < (*ii)->m_loffset)
        {
          break;
        }
      }

      if (li == layout.end()) { layout.push_back(0); }

      if ((*ii)->m_roffset > rightmost) { rightmost = (*ii)->m_roffset; }

      int vpos = tileoffset+(layoutpos + layoutoffset)*lineheight;
      layout[layoutpos] = (*ii)->m_roffset + layoutgutter;

      int inserthpos = m_hoffset + (*ii)->m_loffset; 
      int insertlength = (*ii)->m_length;

      InsertCanvasItem * iitem = new InsertCanvasItem(inserthpos, vpos,
                                                      insertlength, m_seqheight,
                                                      *ii, m_icanvas);
      iitem->show();
    }

    if (!layout.empty()) 
    { 
      layoutoffset += layout.size() + 1;
    }

    if (!m_partitionTypes)
    {
      break;
    }
  }

  int newheight = tileoffset+layoutoffset*lineheight;

  if (m_showFeatures)
  {
    vector<AMOS::Feature_t> & feats = m_datastore->m_contig.getFeatures();
    vector<AMOS::Feature_t>::iterator fi;
    for (fi = feats.begin(); fi != feats.end(); fi++)
    {
      int offset = fi->range.isReverse() ? fi->range.end : fi->range.begin;

      QCanvasRectangle * rect = new QCanvasRectangle(m_hoffset + offset, 0, fi->range.getLength(), newheight, m_icanvas);
      rect->setBrush(QColor(139,119,101));
      rect->setPen(QColor(169,149,131));
      rect->setZ(-2);
      rect->show();
    }
  }

  m_icanvas->resize(rightmost - leftmost + 1000, newheight);
  m_icanvas->update();
}

void InsertWidget::setConnectMates(bool b)
{
  m_connectMates = b;
  refreshCanvas();
}

void InsertWidget::setPartitionTypes(bool b)
{
  m_partitionTypes = b;
  refreshCanvas();
}

void InsertWidget::setCoveragePlot(bool b)
{
  m_coveragePlot = b;
  refreshCanvas();
}

void InsertWidget::setFeatures(bool b)
{
  m_showFeatures = b;
  refreshCanvas();
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

  m_ball = new QCanvasEllipse(100, 5, m_icanvas);
  m_ball->setX(rect.x() + rect.width()/2);
  m_ball->setY(basey - 25);
  m_ball->setPen(Qt::white);
  m_ball->setBrush(Qt::white);
  m_ball->show();

  m_paddle = new Paddle(rect.x() + rect.width()/2, basey-10, 1000, 5, m_icanvas);
  m_paddle->setPen(Qt::white);
  m_paddle->setBrush(Qt::white);
  m_paddle->show();

  m_xvel = rand() % 250 - 125;
  m_yvel = 1;

  m_icanvas->update();

  m_timer->start(20);
}

void InsertWidget::left()
{
  if (m_paddle)
  {
    m_paddle->moveBy(-250, 0);
    m_icanvas->update();
  }
}

void InsertWidget::right()
{
  if (m_paddle)
  {
    m_paddle->moveBy(250, 0);
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

        for (QCanvasItemList::Iterator li=l.begin(); li != l.end(); li++)
        {
          if ((*li)->rtti() == InsertCanvasItem::RTTI)
          {
            (*li)->hide();
            m_yvel = -m_yvel;
          }
          else if ((*li)->rtti() == Paddle::RTTI)
          {
            m_yvel = -m_yvel;
            m_xvel += rand()%100-50;
          }
        }
      }

      if (abs(m_xvel) > 250) { m_xvel = m_xvel/2; }

      m_ball->moveBy(m_xvel, m_yvel);

      m_icanvas->update();
    }
  }
}





