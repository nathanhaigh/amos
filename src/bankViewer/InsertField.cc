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
  setPalette(QPalette(QColor(0, 0, 0)));
  m_bankname = bankname;
  m_contigId = contigId;

  m_scale = .05;
  m_seqheight = 3;
  m_hoffset = (int)(20/m_scale);

  int posoffset = 15;
  int gutter = 5;

  int tileoffset = posoffset+3*gutter;
  int lineheight = m_seqheight+gutter;

  int layoutgutter = (int)(10/m_scale);

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

  unsigned int clen = m_consensus.size();
  resize((int)(clen*m_scale)+60, 10000);

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
    if (a) { seqtileLookup.erase(aid); }

    Tile_t * b = seqtileLookup[bid];
    if (b) { seqtileLookup.erase(bid); }
    
    if (a || b)
    {
      Insert i(a, m_contigId, b, m_contigId, getLibrarySize(aid), clen);

      if (i.m_state == Insert::Happy)
      {
        inserts.push_back(i);
      }
      else
      {
        Insert j = i;

        if (a)
        {
          i.setActive(0);
          inserts.push_back(i);
        }
        
        if (b)
        {
          j.setActive(1);
          inserts.push_back(j);
        }
      }
    }
  }


  int unmated = 0;
  SeqTileMap_t::iterator si;
  for (si =  seqtileLookup.begin();
       si != seqtileLookup.end();
       si++)
  {
    if (si->second)
    {
      Insert i(si->second, m_contigId, NULL, AMOS::NULL_ID, getLibrarySize(si->second->source), clen);
      inserts.push_back(i);
      unmated++;
    }
  }

  cerr << "unmated: " << unmated << endl;


  sort(inserts.begin(), inserts.end(), Insert::TilingOrderCmp());

  m_pix = new QPixmap(width(), height());

  QPainter p(m_pix);
  QPen pen;
  pen.setColor(black);
  p.setPen(pen);
  p.setBrush(Qt::NoBrush);
  m_pix->fill(this,0,0);

  p.setPen(white);

  // Numberline and tics
  p.drawLine((int)(m_hoffset*m_scale), posoffset, 
             (int)((m_hoffset + clen)*m_scale), posoffset);

  p.setFont(QFont("Helvetica", 8));

  for (unsigned int i = 0; i < clen; i ++)
  {
    if (((i % 1000 == 0) && (clen - i > 1000)) || (i == clen-1))
    {
      p.drawLine((int)((m_hoffset + i)*m_scale), posoffset-2,
                 (int)((m_hoffset + i)*m_scale), posoffset+2);
      p.drawText((int)((m_hoffset + i)*m_scale - 20), posoffset-10, 
                 40, 10, Qt::AlignHCenter | Qt::AlignBottom, QString::number(i));
    }
  }

  cerr << "paint inserts" << endl;
  p.scale(m_scale, 1.0);
  p.setBrush(Qt::SolidPattern);

  char types [] = "HSMNOLU";

  int layoutoffset = 0;
  int layoutpos = 0;

  for (int type = 0; type < strlen(types); type++)
  {
    vector<Insert>::iterator ii;
    vector<int> layout;
    vector<int>::iterator li;

    for (ii = inserts.begin(); ii != inserts.end(); ii++)
    {
      if (ii->m_state != types[type]) { continue; }

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

      int vpos = tileoffset+(layoutpos + layoutoffset)*lineheight;
      layout[layoutpos] = ii->m_roffset + layoutgutter;

      p.setPen(UIElements::getInsertColor(ii->m_state));
      p.setBrush(UIElements::getInsertColor(ii->m_state));

      int inserthpos = m_hoffset + ii->m_loffset; 
      int insertlength = ii->m_length;
      int actuallength = ii->m_actual;

      //cerr << (char) ii->m_state << " " << inserthpos << "," << insertlength;

      p.drawLine(inserthpos, vpos+m_seqheight, inserthpos+insertlength, vpos+m_seqheight);
      QString s = "Actual: " + QString::number(actuallength) +
                  "\nExpected: " + QString::number(ii->m_dist.mean-3*ii->m_dist.sd) +
                  " - "          + QString::number(ii->m_dist.mean+3*ii->m_dist.sd);
      QToolTip::add(this, QRect((int)(inserthpos*m_scale), vpos,
                                (int)(insertlength*m_scale), m_seqheight), s);


      if (ii->m_state == Insert::Happy)
      {
        drawTile(ii->m_atile, p, m_hoffset, vpos);
        drawTile(ii->m_btile, p, m_hoffset, vpos);
      }
      else if (ii->m_active == 0)
      {
        drawTile(ii->m_atile, p, m_hoffset, vpos);
      }
      else
      {
        drawTile(ii->m_btile, p, m_hoffset, vpos);
      }

      //cerr << endl;
    }

    if (!layout.empty()) 
    { 
      layoutoffset += 2; 
      layoutoffset += layout.size();
    }
  }

  p.end();

  resize(width(), tileoffset+(layoutpos + layoutoffset)*lineheight);

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

  //cerr << " " << hpos << " " << readLength;

  p.drawRect(hpos, vpos, readLength, m_seqheight);

  const char * seqname = read_bank.lookupEID(tile->source);
  QString tip = seqname;

  tip += " [" + QString::number(tile->offset) 
       +  "," + QString::number(tile->offset + tile->range.getLength() + tile->gaps.size() -1)
       + "]";

  if (tile->range.end < tile->range.begin)
  {
    tip += " [RC]";
  }

  QToolTip::add(this, QRect((int)(hpos*m_scale), vpos, 
                            (int)(readLength*m_scale), m_seqheight), tip);
}

void InsertField::mouseDoubleClickEvent(QMouseEvent * e)
{
  int pos = e->x();
  pos /= m_scale; 
  pos -= m_hoffset;

  emit setGindex(pos);
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
