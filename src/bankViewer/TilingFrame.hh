#ifndef TILINGFRAME_HH_
#define TILINGFRAME_HH_ 1

#include <qframe.h>
#include <string>
#include "RenderSeq.hh"
#include "TilingField.hh"
#include "ConsensusField.hh"
#include <qscrollview.h>



class TilingFrame: public QFrame
{
  Q_OBJECT

public:
  TilingFrame::TilingFrame(QWidget * parent = 0,
                           const char * name = 0,  
                           WFlags f = 0);
public slots:
    void setGindex(int gindex);
    void setContigId(int contigID);
    void setBankname(string bankname);
    void setFontSize(int);
    void setDB(const QString & db);
    void trackGindex(int);
    void trackGindexDone();
    void toggleStable(bool);
    void toggleShowNumbers(bool);

signals:
    void gindexChanged( int );
    void setGindexRange( int, int );
    void contigRange(int, int);
    void contigLoaded(int);
    void setStatus(const QString &);
    void fontSizeChanged(int);
    void setShowNumbers(bool);

protected:
    void paintEvent(QPaintEvent *);

private:

    int m_fontsize;
    int m_gindex;
    int m_displaywidth;
    bool m_loaded;

    QScrollView * m_sv;

    string m_bankname;
    std::vector<AMOS::Tile_t> m_tiling;
    string m_consensus;
    AMOS::Bank_t read_bank;
    AMOS::Bank_t contig_bank;
    std::vector<RenderSeq_t> m_renderedSeqs;

    string m_db;

    TilingField * m_tilingfield;
    ConsensusField * m_consfield;
};


#endif
