#ifndef TILING_H
#define TILING_H

#include <qwidget.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>
#include "RenderSeq.hh"

class DataStore;
class AlignmentInfo;

class TilingField : public QWidget
{
    Q_OBJECT
public:
    TilingField(DataStore * datastore,
                std::vector<RenderSeq_t> & renderedSeqs,
                const std::string & consensus,
                const std::string & cstatus,
                AlignmentInfo * ai,
                int & gindex,
                int & fontsize,
                QWidget *parent=0, 
                const char *name=0 );

    QSizePolicy sizePolicy() const;
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void setSize(int width, int height);

public slots:
    void toggleHighlightDiscrepancy(bool);
    void toggleDisplayQV(bool);
    void togglePolymorphismView(bool);
    void toggleLowQualityLowerCase(bool);
    void toggleBaseColors(bool);
    void toggleShowFullRange(bool);
    void toggleSNPColoring(bool);
    void toggleQVColoring(bool);
    void togglePackReads(bool);
    void singleClick();
    void setReadnameWidth(int);
    int  getReadnameWidth();

signals:
    void setTilingVisibleRange(int, int, int);
    void highlightRead(int);

protected:
    void paintEvent( QPaintEvent * );

private:
    int getReadCov(int y);

    const std::string & m_consensus;
    const std::string & m_cstatus;
    AlignmentInfo * m_alignment;

    std::vector<RenderSeq_t> & m_renderedSeqs;
    DataStore * m_datastore;

    int m_clickstate;
    QTimer * m_clickTimer;
    int m_yclick;

    int & m_fontsize;
    int & m_gindex;
    
    int m_height;
    int m_width;
    int m_traceheight;
    int m_tracespace;
    int m_readnamewidth;

    bool m_displayqv;
    bool m_stabletiling;
    bool m_snpcoloring;
    bool m_polymorphismView;
    bool m_qvcoloring;
    bool m_highlightdiscrepancy;
    bool m_lowquallower;
    bool m_basecolors;
    bool m_fullseq;
    bool m_packreads;

    bool m_doubleclick;
};


#endif
