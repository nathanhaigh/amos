#include "InsertField.hh"
#include <qpainter.h>
#include <qfont.h>
#include <qtooltip.h>

#include "RenderSeq.hh"
#include "Insert.hh"
#include <algorithm>
#include "UIElements.hh"

using namespace AMOS;
typedef map<ID_t, Tile_t *> SeqTileMap_t;

InsertField::InsertField(const string & bankname,
                         int contigId,
                         QWidget * parent,
                         const char * name)
  : QWidget(parent, name),
    read_bank(AMOS::Read_t::NCODE),
    contig_bank(AMOS::Contig_t::NCODE),
    frag_bank(AMOS::Fragment_t::NCODE),
    lib_bank(AMOS::Library_t::NCODE),
    mate_bank(AMOS::Matepair_t::NCODE)
{
  setPalette(QPalette(QColor(200, 200, 200)));
  m_scale = .10;
  m_seqheight = 8;

  m_bankname = bankname;
  m_contigId = contigId;


  try
  {
    read_bank.open(m_bankname);
    contig_bank.open(m_bankname);
    mate_bank.open(m_bankname);
    frag_bank.open(m_bankname);
    lib_bank.open(m_bankname);

    Contig_t contig;
    contig_bank.fetch(m_contigId, contig);

    m_consensus = contig.getSeqString();
    m_tiling = contig.getReadTiling();
    sort(m_tiling.begin(), m_tiling.end(), RenderSeq_t::TilingOrderCmp());
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  resize((int)(m_consensus.size()*m_scale)+60, 10000);

  cerr << "Creating iid -> tile_t * map" << endl;
  SeqTileMap_t seqtileLookup;

  vector<Tile_t>::iterator vi;
  for (vi =  m_tiling.begin();
       vi != m_tiling.end();
       vi++)
  {
    seqtileLookup[vi->source] = vi;
  }


  cerr << "Loading mates" << endl;
  Matepair_t mates;
  vector<Insert> inserts;

  while (mate_bank >> mates)
  {
    ID_t aid = mates.getReads().first;
    ID_t bid = mates.getReads().second;

    Tile_t * a = seqtileLookup[aid];
    Tile_t * b = seqtileLookup[bid];

    if (a || b)
    {
      Insert i(a, m_contigId, b, m_contigId, getLibrarySize(aid));
      inserts.push_back(i);
    }
  }

  sort(inserts.begin(), inserts.end(), Insert::TilingOrderCmp());


  int posoffset = 15;
  int gutter = 10;

  int hoffset = (int)(20/m_scale);
  int tileoffset = posoffset+gutter;
  int seqdelta = 6;
  int lineheight = m_seqheight+seqdelta+gutter;
  int layoutgutter = (int)(10/m_scale);

  m_pix = new QPixmap(width(), height());

  QPainter p(m_pix);
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);
  p.setBrush(Qt::NoBrush);
  m_pix->fill(this,0,0);


  // Numberline and tics
  p.drawLine((int)(hoffset*m_scale), posoffset, 
             (int)((hoffset + m_consensus.size())*m_scale), posoffset);

  p.setFont(QFont("Helvetica", 8));
  for (unsigned int i = 0; i < m_consensus.size(); i ++)
  {
    if ((i % 500 == 0) || (i == m_consensus.size()-1))
    {
      p.drawLine((int)((hoffset + i)*m_scale), posoffset-2,
                 (int)((hoffset + i)*m_scale), posoffset+2);
      p.drawText((int)((hoffset + i)*m_scale - 20), posoffset-10, 
                 40, 10, Qt::AlignHCenter | Qt::AlignBottom, QString::number(i));
    }
  }


  cerr << "paint inserts" << endl;
  p.scale(m_scale, 1.0);

  vector<int> layout;
  vector<int>::iterator li;
  int layoutpos;

  vector<Insert>::iterator ii;
  for (ii = inserts.begin(); ii != inserts.end(); ii++)
  {
    for (li = layout.begin(), layoutpos = 0;
         li != layout.end();
         li++, layoutpos++)
    {
      if (*li < ii->m_loffset)
      {
        break;
      }
    }

    if (li == layout.end())
    {
      layout.push_back(0);
    }

    int vpos = tileoffset+layoutpos*lineheight;
    layout[layoutpos] = ii->m_roffset + layoutgutter;

    int inserthpos = hoffset + ii->m_loffset; 
    int insertlength = ii->m_roffset - ii->m_loffset;

    QBrush b(UIElements::getInsertColor(ii->m_state));
    b.setStyle(Qt::Dense7Pattern);
    p.setPen(NoPen);
    p.setBrush(b);

    int insertheight = m_seqheight + seqdelta + 2;

    p.drawRect(inserthpos, vpos, insertlength, insertheight);
    QString s = "Actual: " + QString::number(insertlength) +
                "\nExpected: " + QString::number(ii->m_dist.mean-3*ii->m_dist.sd) +
                " - "          + QString::number(ii->m_dist.mean+3*ii->m_dist.sd);
    QToolTip::add(this, QRect((int)(inserthpos*m_scale), vpos,
                              (int)(insertlength*m_scale), insertheight), s);



    b.setStyle(Qt::SolidPattern);
    p.setBrush(b);
    p.setPen(Qt::black);

    drawTile(ii->m_atile, p, hoffset, vpos);
    drawTile(ii->m_btile, p, hoffset, vpos + seqdelta);
  }

  p.end();
}

void InsertField::paintEvent(QPaintEvent * e)
{
  if (m_pix)
  {
    QPainter painter(this);
    painter.drawPixmap(0, 0, *m_pix);
  }
}

void InsertField::drawTile(Tile_t * tile, QPainter & p, int hoffset, int vpos)
{
  if (!tile) { return; }

  int hpos = hoffset + tile->offset;
  int readLength = tile->range.getLength() + tile->gaps.size();
  int arrowLen = (int)(10/m_scale);

  QPointArray seqarrow(5);

  if (tile->range.end < tile->range.begin)
  {
    // rc
    seqarrow[0]=QPoint(hpos,            vpos+m_seqheight/2);
    seqarrow[1]=QPoint(hpos+arrowLen,   vpos);
    seqarrow[2]=QPoint(hpos+readLength, vpos);
    seqarrow[3]=QPoint(hpos+readLength, vpos+m_seqheight);
    seqarrow[4]=QPoint(hpos+arrowLen,   vpos+m_seqheight);
  }
  else
  {
    // forward
    seqarrow[0]=QPoint(hpos,                     vpos);
    seqarrow[1]=QPoint(hpos+readLength-arrowLen, vpos);
    seqarrow[2]=QPoint(hpos+readLength,          vpos+m_seqheight/2);
    seqarrow[3]=QPoint(hpos+readLength-arrowLen, vpos+m_seqheight);
    seqarrow[4]=QPoint(hpos,                     vpos+m_seqheight);
  }

  p.drawPolygon(seqarrow);

  const char * seqname = read_bank.lookupEID(tile->source);
  QToolTip::add(this, QRect((int)(hpos*m_scale), vpos, 
                            (int)(readLength*m_scale), m_seqheight), seqname);
}


AMOS::Distribution_t InsertField::getLibrarySize(ID_t readid)
{
  Read_t read;
  read_bank.fetch(readid, read);

  Fragment_t frag;
  frag_bank.fetch(read.getFragment(), frag);

  Library_t lib;
  lib_bank.fetch(frag.getLibrary(), lib);

  return lib.getDistribution();
}
