#ifndef TILING_H
#define TILING_H

#include <qwidget.h>
#include <qtimer.h>
#include "RenderSeq.hh"

class TilingField : public QWidget
{
    Q_OBJECT
public:
    TilingField(std::vector<RenderSeq_t> & renderedSeqs,
                const std::string & consensus,
                const std::string & cstatus,
                const std::string & db,
                int & gindex,
                int & fontsize,
                QWidget *parent=0, 
                const char *name=0 );

    QSizePolicy sizePolicy() const;
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void setSize(int width, int height);

public slots:
    void toggleStable(bool);
    void toggleHighlightDiscrepancy(bool);
    void toggleDisplayQV(bool);
    void singleClick();

signals:
    void setTilingVisibleRange(int, int);

protected:
    void paintEvent( QPaintEvent * );

private:
    int getReadCov(int y);

    int m_clickstate;
    QTimer * m_clickTimer;
    int m_yclick;

    int & m_fontsize;
    int & m_gindex;
    
    int m_displayqv;
    int m_height;
    int m_width;
    int m_stabletiling;
    int m_traceheight;

    const std::string & m_db;
    const std::string & m_consensus;
    const std::string & m_cstatus;

    bool m_highlightdiscrepancy;

    std::vector<RenderSeq_t> & m_renderedSeqs;
    std::vector<RenderSeq_t *> m_currentReads;
};


#endif
