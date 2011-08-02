#include "TilingFrame.hh"
#include "TilingField.hh"
#include <set>

#include <q3scrollview.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qcursor.h>
//Added by qt3to4:
#include <Q3BoxLayout>
#include <QPaintEvent>
#include <Q3Frame>
#include <Q3VBoxLayout>
#include "ConsensusField.hh"
#include "RenderSeq.hh"
#include "UIElements.hh"
#include "DataStore.hh"
#include "AlignmentInfo.hh"
#include <qregexp.h>

using namespace std;
using namespace AMOS;

int min (int a, int b)
{
  return a < b ? a : b;
}

struct SeqnameOrderCmp
{
  bool operator() (const RenderSeq_t & a, const RenderSeq_t & b)
  {
    return a.m_read.getEID() < b.m_read.getEID();
  }
};


struct SNPTilingOrderCmp
{
  static int snpposition;
  static int contiglen;
  static int reverse;

  bool operator() (const RenderSeq_t & a, const RenderSeq_t & b)
  {
    char abase = a.base(snpposition, false, contiglen);
    char bbase = b.base(snpposition, false, contiglen);

    if (reverse)
    {
      return toupper(abase) > toupper(bbase);
    }

    return toupper(abase) < toupper(bbase);
  }
};

int SNPTilingOrderCmp::snpposition(0);
int SNPTilingOrderCmp::contiglen(0);
int SNPTilingOrderCmp::reverse(0);


TilingFrame::TilingFrame(DataStore * datastore, 
                         AlignmentInfo * ai,
                         QWidget * parent, 
                         const char * name, 
                         Qt::WFlags f)
  :Q3Frame(parent, name, f),
   m_datastore(datastore),
   m_alignment(ai)
{
  m_gindex = 0;
  m_fontsize = 10;
  m_displayAllChromo = true;

  m_loadedStart = m_loadedEnd = -1;

  toggleDisplayAllChromo(false);
  m_nextDiscrepancyBuffer = 10;

  m_sv = new Q3ScrollView(this, "tilingscroll");
  m_sv->setHScrollBarMode(Q3ScrollView::AlwaysOff);
  
  m_tilingfield = new TilingField(datastore,
                                  m_renderedSeqs,
                                  m_consensus,
                                  m_cstatus,
                                  m_alignment,
                                  m_gindex,
                                  m_fontsize,
                                  m_sv->viewport(),
                                  "tiling" );
  m_sv->addChild(m_tilingfield);
  m_sv->setPaletteBackgroundColor(UIElements::color_tiling);

  m_consfield = new ConsensusField(m_consensus, 
                                   m_cstatus, 
                                   m_consqual, 
                                   m_ugpos,
                                   m_alignment,
                                   m_gindex, 
                                   m_contigId,
                                   m_contigName,
                                   this, 
                                   "cons");

  Q3BoxLayout * layout = new Q3VBoxLayout(this);
  layout->addWidget(m_consfield);
  layout->addWidget(m_sv, 10);


  connect(m_tilingfield, SIGNAL(highlightRead(int)),
          this,          SIGNAL(highlightRead(int)));

  connect(this,          SIGNAL(fontSizeChanged(int)),
          m_consfield,   SLOT(setFontSize(int)));

  connect(m_tilingfield, SIGNAL(setTilingVisibleRange(int, int, int)),
          this,          SIGNAL(setTilingVisibleRange(int, int, int)));

  connect(m_consfield,   SIGNAL(sortColumns(int)),
          this,          SLOT(sortColumns(int)));

  connect(this,          SIGNAL(togglePackReads(bool)),
          m_tilingfield, SLOT(togglePackReads(bool)));

  connect(this,          SIGNAL(togglePackReads(bool)),
          m_consfield,   SLOT(togglePackReads(bool)));

  connect(this,          SIGNAL(toggleShowUngapped(bool)),
          m_consfield,   SLOT(toggleShowUngapped(bool)));

  connect(this,          SIGNAL(toggleDisplayQV(bool)),
          m_tilingfield, SLOT(toggleDisplayQV(bool)));

  connect(this,          SIGNAL(togglePolymorphismView(bool)),
          m_tilingfield, SLOT(togglePolymorphismView(bool)));

  connect(this,          SIGNAL(toggleQVColoring(bool)),
          m_tilingfield, SLOT(toggleQVColoring(bool)));

  connect(this,        SIGNAL(toggleLowQualityLowerCase(bool)),
          m_tilingfield, SLOT(toggleLowQualityLowerCase(bool)));

  connect(this,        SIGNAL(toggleShowFullRange(bool)),
          m_tilingfield, SLOT(toggleShowFullRange(bool)));

  connect(this,        SIGNAL(toggleHighlightDiscrepancy(bool)),
          m_tilingfield, SLOT(toggleHighlightDiscrepancy(bool)));

  connect(this,        SIGNAL(toggleShowNumbers(bool)),
          m_consfield,   SLOT(toggleShowNumbers(bool)));

  connect(this,        SIGNAL(toggleBaseColors(bool)),
          m_consfield,   SLOT(toggleBaseColors(bool)));

  connect(this,        SIGNAL(toggleShowConsQV(bool)),
          m_consfield,   SLOT(toggleShowConsQV(bool)));

  connect(this,        SIGNAL(toggleShowIndicator(bool)),
          m_consfield,   SLOT(toggleShowIndicator(bool)));

  connect(this,        SIGNAL(toggleSNPColoring(bool)),
          m_tilingfield, SLOT(toggleSNPColoring(bool)));

  connect(this,        SIGNAL(toggleBaseColors(bool)),
          m_tilingfield, SLOT(toggleBaseColors(bool)));
}

void TilingFrame::toggleDisplayAllChromo(bool display)
{
  m_displayAllChromo = display;

  if (m_displayAllChromo)
  {
    m_loadedWidth = 500;

    // load only the reads in this region
    setGindex(m_gindex);

    // force loading of chromatograms
    vector<RenderSeq_t>::iterator ri;
    for (ri =  m_renderedSeqs.begin();
         ri != m_renderedSeqs.end();
         ri++)
    {
      ri->loadTrace();
      ri->m_displayTrace = true;
    }

    update();
  }
  else
  {
    m_loadedWidth = 5000;
  }
}


void TilingFrame::paintEvent(QPaintEvent * event)
{
  Q3Frame::paintEvent(event);
  m_consfield->update();

  m_tilingfield->setSize(width(), m_sv->visibleHeight());
  m_tilingfield->update();
}

void TilingFrame::setContigId(int contigId)
{
  if (m_datastore->m_loaded)
  {
    try
    {
      m_contigId = contigId;
      m_contigName = m_datastore->contig_bank.lookupEID(contigId).c_str();
      m_tiling = m_datastore->m_contig.getReadTiling();
      m_consensus = m_datastore->m_contig.getSeqString();
      m_consqual = m_datastore->m_contig.getQualString();

      int l = m_consensus.size();

      m_cstatus.erase();
      m_cstatus.resize(l, ' ');
      m_renderedSeqs.clear();

      int i = 0;
      m_ugpos.resize(l);
      while (i < l && m_consensus[i] == '-')
      {
        m_ugpos[i] = 0;
        i++;
      }

      if (i < l)
      {
        m_ugpos[i] = 1;

        for (i++; i < l; i++)
        {
          int last = m_ugpos[i-1];
          if (m_consensus[i] == '-') { m_ugpos[i] = last; }
          else                       { m_ugpos[i] = last+1; }
        }
      }

      for (unsigned int i = 0; i < m_alignment->m_gaps.size(); i++)
      {
        m_consensus.insert(i+m_alignment->m_gaps[i], 1, '*');
        m_consqual.insert (i+m_alignment->m_gaps[i], 1, '*');
        m_cstatus.insert  (i+m_alignment->m_gaps[i], 1, '*');
        //m_ugpos.insert    (i+m_alignment->m_gaps[i], 1, -1); TODO
      }

      m_consensus.insert((unsigned) 0,(unsigned) m_alignment->m_startshift, '*');
      m_cstatus.insert  ((unsigned) 0,(unsigned) m_alignment->m_startshift, '*');
      m_consqual.insert ((unsigned) 0,(unsigned) m_alignment->m_startshift, '*');
      // m_ugpos.insert    ((unsigned) 0,(unsigned) m_alignment->m_startshift, -1); TODO

      m_consensus.append(m_alignment->m_endshift, '*');
      m_cstatus.append  (m_alignment->m_endshift, '*');
      m_consqual.append (m_alignment->m_endshift, '*');
      //m_ugpos.append    (m_alignment->m_endshift, -1); // TODO

      sort(m_tiling.begin(), m_tiling.end(), TileOrderCmp());

      m_loadedStart = m_loadedEnd = -1;

      m_consfield->setHighlightRange(-1,-1);
      setGindex(0);
      emit setGindexRange(0, l-1);
    }
    catch (Exception_t & e)
    {
      cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
    }
  }
}

void TilingFrame::setFontSize(int fontsize )
{
  if (fontsize == m_fontsize) { return; }
  m_fontsize = fontsize;

  loadContigRange(m_gindex);

  emit fontSizeChanged(m_fontsize);
  update();
}

void TilingFrame::loadContigRange(int gindex)
{
  if (m_tiling.empty()) { return; }

  int basespace = 5;
  double basewidth = m_fontsize + basespace;
  int tilehoffset = m_fontsize*10;

  if (basewidth <= 0)
  {
    basewidth = 1/((-basewidth+2));
  }

  int minheight = 8;

  if (m_fontsize < minheight)
  {
    tilehoffset = minheight * 10;
  }


  m_displaywidth = (width()-tilehoffset)/basewidth;

  gindex = min(gindex, m_consensus.size()-m_displaywidth+1);
  gindex = max(gindex, 0);
  m_gindex = gindex;

  int grangeStart = m_alignment->getContigPos(m_gindex);
  int grangeEnd   = m_alignment->getContigPos(min(m_gindex + m_displaywidth+1000, (int)m_consensus.length()));

  if (grangeStart < m_loadedStart || grangeEnd > m_loadedEnd)
  {
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    m_loadedStart = max(0, grangeStart-m_loadedWidth/2);
    m_loadedEnd   = min(m_consensus.length(), grangeEnd+m_loadedWidth/2);

    int writepos = 0;
    int vectorpos = 0;
    int orig = m_renderedSeqs.size();
    int erase = 0;

    // remove sequences that are no longer in the view window
    //cerr << "checking to erase: " << orig << endl;

    std::set<ID_t> keepset;

    for (vectorpos = 0; vectorpos < orig; vectorpos++)
    {
      RenderSeq_t & r = m_renderedSeqs[vectorpos];
      if (RenderSeq_t::hasOverlap(m_loadedStart, m_loadedEnd,
                                  r.m_tile->offset, r.gappedLen(),
                                  m_consensus.length()))
      {
        if (writepos != vectorpos)
        {
          m_renderedSeqs[writepos] = r;
        }

        writepos++;
        keepset.insert(r.m_tile->source);
      }
      else
      {
        erase++;
      }
    }

    m_renderedSeqs.resize(writepos);

      
    // Render the aligned sequences
    vector<Tile_t>::iterator vi;
    vector<Tile_t>::iterator viend = m_tiling.end();

    orig = m_renderedSeqs.size();
    vector<RenderSeq_t>::iterator ri;
    int clen = m_consensus.length();

    for (vi = m_tiling.begin(); vi != viend; vi++)
    {
      int hasOverlap = RenderSeq_t::hasOverlap(m_loadedStart, m_loadedEnd,
                                               vi->offset, vi->getGappedLength(),
                                               clen);
      if (hasOverlap)
      {
        // see if this has already been rendered
        bool found = false;

        if (keepset.find(vi->source) != keepset.end())
        {
          found = true;
        }

        if (found) { continue; }


        // hasn't been rendered before
        RenderSeq_t rendered;
        rendered.load(m_datastore, &*vi);
        if (m_displayAllChromo) { rendered.loadTrace(); rendered.m_displayTrace = true; }
        m_renderedSeqs.push_back(rendered);

        for (int gindex = rendered.m_loffset; gindex <= rendered.m_roffset; gindex++)
        {
          int circ = gindex;
          if (circ < 0) { circ += clen; }
          circ = circ % clen;
          int global = m_alignment->getGlobalPos(circ);

          if (m_cstatus[global] != 'X')
          {
            if (m_cstatus[global] == '*')
            {
              cerr << "err" << endl;
            }
            else if (m_cstatus[global] == ' ')                   
            { 
              m_cstatus[global] = rendered.base(gindex, false, clen); 
            }
            else if (toupper(m_cstatus[global]) != toupper(rendered.base(gindex, false, clen))) 
            { 
              m_cstatus[global] = 'X'; 
            }
          }
        }
      }
    }
    //cerr << "Loaded [" << m_loadedStart << "," << m_loadedEnd << "]:" << m_renderedSeqs.size() << " kept:" << orig << " erased: " << erase << endl;

    QApplication::restoreOverrideCursor();
  }
}

void TilingFrame::setGindex(int gindex)
{
  if (m_loadedStart != -1 && gindex == m_gindex) { return; }

  if      (gindex < 0)                          { gindex = 0; }
  else if (gindex > (int) m_consensus.length()) { gindex = m_consensus.length(); }

  loadContigRange(gindex);
  emit gindexChanged(gindex);
  update();
}

void TilingFrame::advanceNextDiscrepancy()
{

  int gindex = m_gindex+m_nextDiscrepancyBuffer+1;

  while (gindex < (int)m_consensus.length())
  {
    if (gindex > m_loadedEnd)
    {
      loadContigRange(gindex);
    }

    while (gindex <= m_loadedEnd)
    {
      if (m_cstatus[gindex] == 'X')
      {
        setGindex(gindex-m_nextDiscrepancyBuffer);
        return;
      }
      else
      {
        gindex++;
      }
    }
  }

  setGindex(gindex);
} 

void TilingFrame::sortColumns(int gindex)
{
  vector<RenderSeq_t>::iterator vi;

  if (gindex == -1)
  {
    stable_sort(m_renderedSeqs.begin(), m_renderedSeqs.end(), SeqnameOrderCmp());
  }
  else
  {
    if (SNPTilingOrderCmp::snpposition == gindex)
    {
      SNPTilingOrderCmp::reverse = !SNPTilingOrderCmp::reverse;
      stable_sort(m_renderedSeqs.begin(), m_renderedSeqs.end(), SNPTilingOrderCmp());
    }
    else
    {
      SNPTilingOrderCmp::snpposition = gindex;
      SNPTilingOrderCmp::contiglen = m_consensus.length();
      SNPTilingOrderCmp::reverse = 0;
      stable_sort(m_renderedSeqs.begin(), m_renderedSeqs.end(), SNPTilingOrderCmp());
    }

    for (vi =  m_renderedSeqs.begin();
         vi != m_renderedSeqs.end();
         vi++)
    {
      vi->bgcolor = vi->base(gindex, false, m_consensus.length());
    }
  }
  update();
}

void TilingFrame::advancePrevDiscrepancy()
{
  int gindex = m_gindex+m_nextDiscrepancyBuffer-1;

  while (gindex >=0 )
  {
    if (gindex < m_loadedStart)
    {
      loadContigRange(gindex);
    }

    while (gindex >= m_loadedStart)
    {
      if (m_cstatus[gindex] == 'X')
      {
        setGindex(gindex-m_nextDiscrepancyBuffer);
        return;
      }

      gindex--;
    }
  }

  setGindex(gindex);
}



void TilingFrame::searchString(const QString & str, bool forward)
{
  if (!str.isEmpty())
  {
    int pos;
    QRegExp regex(str);
    QString qcons(m_consensus.c_str());

    int gindex = m_gindex + m_nextDiscrepancyBuffer;

    if (forward)
    {
      gindex +=  1;
      pos = regex.search(qcons, gindex);
    }
    else
    {
      gindex -= 1;
      pos = regex.searchRev(qcons, gindex - m_consensus.length());
    }

    if (pos != -1)
    {
      m_consfield->setHighlightRange(pos, pos+regex.matchedLength() - 1);
      setGindex(pos-m_nextDiscrepancyBuffer);
    }
  }
}

void TilingFrame::setReadnameWidth(int width)
{
  m_tilingfield->setReadnameWidth(width);
  m_consfield->setReadnameWidth(width);
}

int TilingFrame::getReadnameWidth()
{
  return m_tilingfield->getReadnameWidth();
}
