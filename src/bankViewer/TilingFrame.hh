#ifndef TILINGFRAME_HH_
#define TILINGFRAME_HH_ 1

#include <qframe.h>
#include <string>
#include "RenderSeq.hh"
#include "TilingField.hh"
#include "ConsensusField.hh"
#include <qscrollview.h>

#include "DataStore.hh"



class TilingFrame: public QFrame
{
  Q_OBJECT

public:
  TilingFrame::TilingFrame(DataStore * datastore,
                           QWidget * parent = 0,
                           const char * name = 0,  
                           WFlags f = 0);
public slots:
    void setGindex(int gindex);
    void setContigId(int contigId);
    void setFontSize(int);
    void advanceNextDiscrepancy();
    void advancePrevDiscrepancy();
    void highlightRead(int);
    void sortColumns(int);
    void toggleDisplayAllChromo(bool);
    void searchString(const QString &, bool forward);

signals:
    void gindexChanged( int );
    void setGindexRange( int, int );
    void contigLoaded(int);
    void setStatus(const QString &);
    void fontSizeChanged(int);
    void setShowNumbers(bool);
    void setTilingVisibleRange(int, int);

    void toggleShowFullRange(bool);
    void toggleShowNumbers(bool);
    void toggleShowIndicator(bool);
    void toggleDisplayQV(bool);
    void toggleLowQualityLowerCase(bool);
    void toggleBaseColors(bool);
    void toggleSNPColoring(bool);
    void toggleHighlightDiscrepancy(bool);

protected:
    void paintEvent(QPaintEvent *);

private:

    void loadContigRange(int gindex);

    int m_fontsize;
    int m_gindex;
    int m_displaywidth;

    bool m_displayAllChromo;

    int m_loadedStart;
    int m_loadedEnd;
    int m_loadedWidth;
    int m_nextDiscrepancyBuffer;

    QScrollView * m_sv;

    std::vector<AMOS::Tile_t> m_tiling;
    std::string m_consensus;
    std::string m_cstatus;

    DataStore * m_datastore;
    std::vector<RenderSeq_t> m_renderedSeqs;

    TilingField * m_tilingfield;
    ConsensusField * m_consfield;
};


#endif
