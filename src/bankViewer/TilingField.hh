#ifndef TILING_H
#define TILING_H

#include <qwidget.h>
#include "RenderSeq.hh"

class TilingField : public QWidget
{
    Q_OBJECT
public:
    TilingField(vector<RenderSeq_t> & renderedSeqs,
                const string & consensus,
                const string & cstatus,
                const string & db,
                int & gindex,
                int & fontsize,
                QWidget *parent=0, 
                const char *name=0 );

    QSizePolicy sizePolicy() const;
    void mouseDoubleClickEvent(QMouseEvent * event);
    void setSize(int width, int height);

public slots:
    void toggleStable(bool);
    void toggleHighlightDiscrepancy(bool);

protected:
    void paintEvent( QPaintEvent * );

private:
    int & m_fontsize;
    int & m_gindex;
    
    int m_height;
    int m_width;
    int m_stabletiling;

    const string & m_db;
    const string & m_consensus;
    const string & m_cstatus;

    bool m_highlightdiscrepancy;

    vector<RenderSeq_t> & m_renderedSeqs;
    vector<RenderSeq_t *> m_currentReads;
};


#endif
