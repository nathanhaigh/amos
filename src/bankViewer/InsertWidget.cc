#include "InsertWidget.hh"

#include <qlayout.h>
#include <qlabel.h>
#include <qwmatrix.h>
#include "RenderSeq.hh"
#include "InsertCanvasItem.hh"

using namespace AMOS;
using namespace std;
typedef std::map<ID_t, Tile_t *> SeqTileMap_t;


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

  m_hoffset = 0;

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

  unsigned int clen = 0;
  SeqTileMap_t seqtileLookup;

  int mated = 0;
  int unmated = 0;
  int matelisted = 0;

  try
  {
    m_tiling = m_datastore->m_contig.getReadTiling();
    clen = m_datastore->m_contig.getSeqString().size();

    sort(m_tiling.begin(), m_tiling.end(), RenderSeq_t::TilingOrderCmp());

    cerr << "Creating iid -> tile_t * map" << endl;
    vector<Tile_t>::iterator vi;
    for (vi =  m_tiling.begin();
         vi != m_tiling.end();
         vi++)
    {
      seqtileLookup[vi->source] = &(*vi);
    }


    cerr << "Loading mates" << endl;
    Matepair_t mates;
    map<ID_t, ID_t>::iterator mi;

    SeqTileMap_t::iterator si;
    for (si =  seqtileLookup.begin();
         si != seqtileLookup.end();
         si++)
    {
      if (si->second == NULL)
      {
        //cerr << "Skipping already seen read" << endl;
        continue;
      }

      mi = m_datastore->m_readmatelookup.find(si->first);

      if (mi == m_datastore->m_readmatelookup.end())
      {
        // unmated
        Insert * i = new Insert(si->second, si->first, m_datastore->m_contigId, 
                                NULL, AMOS::NULL_ID, AMOS::NULL_ID,
                                m_datastore->getLibrarySize(si->first), clen);
        m_inserts.push_back(i);
        unmated++;
      }
      else
      {
        matelisted++;

        ID_t aid = mi->first;
        ID_t bid = mi->second;
        ID_t good = aid;

        //cerr << "aid:" << aid << " bid: " << bid << endl;

        ID_t acontig = AMOS::NULL_ID;
        ID_t bcontig = AMOS::NULL_ID;
        
        SeqTileMap_t::iterator ai = si;
        SeqTileMap_t::iterator bi = seqtileLookup.find(bid);

        Tile_t * a = ai->second;
        Tile_t * b = NULL;

        ai->second = NULL;
        acontig = m_datastore->m_contigId;

        if (bi != seqtileLookup.end())
        {
          b = bi->second;
          bi->second = NULL;
          bcontig = m_datastore->m_contigId;

          good = bid;
          cerr << ".";
        }
        else
        {
          bcontig = m_datastore->lookupContigId(bid);
          cerr << "+";
        }
        

        mated++;
        Insert * i = new Insert(a, aid, acontig, b, bid, bcontig, 
                                m_datastore->getLibrarySize(good), clen);

        if (i->m_state == Insert::Happy)
        {
          m_inserts.push_back(i);
        }
        else
        {
          if (a && b)
          {
            Insert * j = new Insert(*i);
            j->setActive(1, i);
            m_inserts.push_back(j);

            i->setActive(0, j);
          }
          else if (a) { i->setActive(0, NULL); }
          else if (b) { i->setActive(1, NULL); }

          m_inserts.push_back(i);
        }
      }
    }

    cerr << " mated: "   << mated 
         << " matelisted: " << matelisted
         << " unmated: " << unmated << endl;

  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  int leftmost = 0;
  int rightmost = clen;

  if (!m_inserts.empty())
  {
    sort(m_inserts.begin(), m_inserts.end(), Insert::TilingOrderCmp());

    leftmost = (*m_inserts.begin())->m_loffset;
    if (leftmost > 0) { leftmost = 0; }
  }

  cerr << "leftmost: " << leftmost << endl;
  m_hoffset = -leftmost;

  QCanvasLine * line = new QCanvasLine(m_icanvas);
  line->setPoints(m_hoffset,        posoffset, 
                  m_hoffset + clen, posoffset);
  line->setPen(QPen(Qt::white, 2));
  line->show();

  for (unsigned int i = 0; i < clen; i ++)
  {
    if (((i % 1000 == 0) && (clen - i > 1000)) || (i == clen-1))
    {
      line = new QCanvasLine(m_icanvas);

      line->setPoints(m_hoffset + i, posoffset-2,
                      m_hoffset + i, posoffset+2);
      line->setPen(Qt::white);
      line->show();
    }
  }



  // bubblesort the types by the order they appear in the popup menu
  vector<char> types;
  map<char, pair<int, bool> >::iterator ti;

  for (ti = m_types.begin(); ti != m_types.end(); ti++)
  {
    types.push_back(ti->first);
  }

  for (int i = 0; i < types.size(); i++)
  {
    for (int j = i+1; j < types.size(); j++)
    {
      if (m_types[types[j]].first > m_types[types[i]].first)
      {
        char swap = types[i];
        types[i] = types[j];
        types[j] = swap;
      }
    }
  }


  cerr << "paint inserts: ";
  int layoutoffset = 0;

  // For all types
  for (int type = 0; type < types.size(); type++)
  {
    cerr << (char)(types[type] - 'A' + 'a');

    if (!m_types[types[type]].second) { continue; }

    vector<Insert *>::iterator ii;

    vector<int> layout;
    vector<int>::iterator li;
    int layoutpos;
    
    cerr << types[type];

    // For all inserts
    for (ii = m_inserts.begin(); ii != m_inserts.end(); ii++)
    {
      if ((*ii)->m_state != types[type]) { continue; }

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
  }

  cerr << endl;

  m_icanvas->resize(rightmost - leftmost + 1000, tileoffset+layoutoffset*lineheight);
  m_icanvas->update();
}


