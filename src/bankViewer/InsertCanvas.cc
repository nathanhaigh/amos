#include "InsertCanvas.hh"
#include <qpainter.h>
#include <qfont.h>
#include <qtooltip.h>

#include "RenderSeq.hh"
#include "Insert.hh"
#include <algorithm>
#include "UIElements.hh"
#include "InsertCanvasItem.hh"

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

  int posoffset = 5;
  int gutter = 5;
  int tileoffset = posoffset+2*gutter;
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
  }
  catch (Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }

  sort(m_tiling.begin(), m_tiling.end(), RenderSeq_t::TilingOrderCmp());

  unsigned int clen = m_consensus.size();

  cerr << "Creating iid -> tile_t * map" << endl;
  SeqTileMap_t seqtileLookup;

  vector<Tile_t>::iterator vi;
  for (vi =  m_tiling.begin();
       vi != m_tiling.end();
       vi++)
  {
    seqtileLookup[vi->source] = &(*vi);
  }


  int mated = 0;
  int unmated = 0;
  int allmates = 0;

  cerr << "Loading mates ";
  Matepair_t mates;

  while (mate_bank >> mates)
  {
    allmates++;
    ID_t aid = mates.getReads().first;
    ID_t bid = mates.getReads().second;
    
    SeqTileMap_t::iterator ai = seqtileLookup.find(aid);
    SeqTileMap_t::iterator bi = seqtileLookup.find(bid);

    Tile_t * a = NULL;
    Tile_t * b = NULL;

    if (ai != seqtileLookup.end())
    {
      a = ai->second;
      seqtileLookup.erase(ai);
    }

    if (bi != seqtileLookup.end())
    {
      b = bi->second;
      seqtileLookup.erase(bi);
    }
    
    if (a || b)
    {
      mated++;
      Insert * i = new Insert(a, m_contigId, b, m_contigId, getLibrarySize(aid), clen);

      if (i->m_state == Insert::Happy)
      {
        m_inserts.push_back(i);
      }
      else
      {
        if (a && b)
        {
          Insert * j = new Insert(*i);
          i->setActive(0, j);
          j->setActive(1, i);
          m_inserts.push_back(j);
        }
        else if (a) { i->setActive(0, NULL); }
        else if (b) { i->setActive(1, NULL); }

        m_inserts.push_back(i);
      }
    }
  }


  SeqTileMap_t::iterator si;
  for (si =  seqtileLookup.begin();
       si != seqtileLookup.end();
       si++)
  {
    if (si->second)
    {
      Insert * i = new Insert(si->second, m_contigId, NULL, AMOS::NULL_ID, getLibrarySize(si->second->source), clen);
      m_inserts.push_back(i);
      unmated++;
    }
  }

  cerr << "allmates: " << allmates 
       << " mated: "   << mated 
       << " unmated: " << unmated << endl;

  sort(m_inserts.begin(), m_inserts.end(), Insert::TilingOrderCmp());

  QCanvasLine * line = new QCanvasLine(this);
  line->setPoints(m_hoffset,        posoffset, 
                  m_hoffset + clen, posoffset);
  line->setPen(QPen(Qt::white, 2));
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
  int numtypes = strlen(types);

  int layoutoffset = 0;

  // For all types
  for (int type = 0; type < numtypes; type++)
  {
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

      if (li == layout.end())
      {
        layout.push_back(0);
      }

      int vpos = tileoffset+(layoutpos + layoutoffset)*lineheight;
      layout[layoutpos] = (*ii)->m_roffset + layoutgutter;

      int inserthpos = m_hoffset + (*ii)->m_loffset; 
      int insertlength = (*ii)->m_length;

      InsertCanvasItem * iitem = new InsertCanvasItem(inserthpos, vpos,
                                                      insertlength, m_seqheight,
                                                      *ii, this);
      iitem->show();
    }

    if (!layout.empty()) 
    { 
      layoutoffset += layout.size() + 1;
    }
  }

  cerr << endl;

  resize(clen+1000, tileoffset+layoutoffset*lineheight);
}

InsertCanvas::~InsertCanvas()
{
  vector<Insert *>::iterator i;
  
  for (i =  m_inserts.begin();
       i != m_inserts.end();
       i++)
  {
    delete (*i);
  }
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
