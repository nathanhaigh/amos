#include "InsertCanvas.hh"
#include <qpainter.h>
#include <qfont.h>
#include <qtooltip.h>

#include "RenderSeq.hh"
#include "Insert.hh"
#include <algorithm>
#include "UIElements.hh"

using namespace AMOS;
using namespace std;
typedef std::map<ID_t, Tile_t *> SeqTileMap_t;

InsertCanvas::InsertCanvas(const string & bankname,
                           int contigId,
                           QWidget * parent,
                           const char * name)
  : QCanvas(parent, name),
    read_bank(AMOS::Read_t::NCODE),
    contig_bank(AMOS::Contig_t::NCODE),
    frag_bank(AMOS::Fragment_t::NCODE),
    lib_bank(AMOS::Library_t::NCODE),
    mate_bank(AMOS::Matepair_t::NCODE)
{
  setBackgroundColor(Qt::black);
  m_bankname = bankname;
  m_contigId = contigId;

  m_seqheight = 3;
  m_hoffset = 0; // 20

  int posoffset = 15;
  int gutter = 5;

  setBackgroundColor(Qt::black);
  int tileoffset = posoffset+3*gutter;
  int lineheight = m_seqheight+gutter;

  int layoutgutter = 50;

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
  resize(clen+1000, 1000);

  cerr << "Creating iid -> tile_t * map" << endl;
  SeqTileMap_t seqtileLookup;

  vector<Tile_t>::iterator vi;
  for (vi =  m_tiling.begin();
       vi != m_tiling.end();
       vi++)
  {
    seqtileLookup[vi->source] = &(*vi);
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

  QCanvasLine * line = new QCanvasLine(this);
  line->setPoints(m_hoffset,        posoffset, 
                  m_hoffset + clen, posoffset);
  line->setPen(Qt::white);
  line->show();

  for (unsigned int i = 0; i < clen; i ++)
  {
    if (((i % 1000 == 0) && (clen - i > 1000)) || (i == clen-1))
    {
      line = new QCanvasLine(this);

      line->setPoints(m_hoffset + i, posoffset-2,
                      m_hoffset + i, posoffset+2);
      line->setPen(Qt::white);
      line->show();
    }
  }


  cerr << "paint inserts: ";
  char types [] = "HSMNOLU";

  int layoutoffset = 0;
  int layoutpos = 0;

  for (int type = 0; type < strlen(types); type++)
  {
    vector<Insert>::iterator ii;
    vector<int> layout;
    vector<int>::iterator li;
    
    cerr << types[type];

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

      int inserthpos = m_hoffset + ii->m_loffset; 
      int insertlength = ii->m_length;
      int actuallength = ii->m_actual;

      //cerr << (char) ii->m_state << " " << inserthpos << "," << insertlength;

      line = new QCanvasLine(this);

      line->setPoints(inserthpos,              vpos+m_seqheight, 
                      inserthpos+insertlength, vpos+m_seqheight);
      line->setPen(UIElements::getInsertColor(ii->m_state));
      line->show();

      QString s = "Actual: " + QString::number(actuallength) +
                  "\nExpected: " + QString::number(ii->m_dist.mean-3*ii->m_dist.sd) +
                  " - "          + QString::number(ii->m_dist.mean+3*ii->m_dist.sd);
//      QToolTip::add(this, QRect(inserthpos, vpos,
//                                insertlength, m_seqheight), s);


      if (ii->m_state == Insert::Happy)
      {
        drawTile(ii->m_atile, this, m_hoffset, vpos, ii->m_state);
        drawTile(ii->m_btile, this, m_hoffset, vpos, ii->m_state);
      }
      else if (ii->m_active == 0)
      {
        drawTile(ii->m_atile, this, m_hoffset, vpos, ii->m_state);
      }
      else
      {
        drawTile(ii->m_btile, this, m_hoffset, vpos, ii->m_state);
      }

      //cerr << endl;
    }

    if (!layout.empty()) 
    { 
      layoutoffset += 2; 
      layoutoffset += layout.size();
    }
  }

  cerr << endl;

  resize(width(), tileoffset+(layoutpos + layoutoffset)*lineheight);

}

void InsertCanvas::drawTile(Tile_t * tile, QCanvas * p, int hoffset, int vpos, Insert::MateState state)
{
  if (!tile) { return; }

  int hpos = hoffset + tile->offset;
  int readLength = tile->range.getLength() + tile->gaps.size();

  //cerr << " " << hpos << " " << readLength;

  QCanvasRectangle * rect = new QCanvasRectangle(hpos, vpos, 
                                                 readLength, m_seqheight, p);
  rect->setBrush(UIElements::getInsertColor(state));
  rect->setPen(UIElements::getInsertColor(state));
  rect->show();

  const char * seqname = read_bank.lookupEID(tile->source);
  QString tip = seqname;

  tip += " [" + QString::number(tile->offset) 
       +  "," + QString::number(tile->offset + tile->range.getLength() + tile->gaps.size() -1)
       + "]";

  if (tile->range.end < tile->range.begin)
  {
    tip += " [RC]";
  }

 // QToolTip::add(this, QRect(hpos, vpos, 
 //               readLength, m_seqheight), tip);
}

AMOS::Distribution_t InsertCanvas::getLibrarySize(ID_t readid)
{
  Read_t read;
  read_bank.fetch(readid, read);

  Fragment_t frag;
  frag_bank.fetch(read.getFragment(), frag);

  Library_t lib;
  lib_bank.fetch(frag.getLibrary(), lib);

  return lib.getDistribution();
}
