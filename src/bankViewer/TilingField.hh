#ifndef TILING_H
#define TILING_H

#include <qwidget.h>
#include "RenderSeq.hh"

class TilingField : public QWidget
{
    Q_OBJECT
public:
    TilingField(vector<RenderSeq_t> & renderedSeqs,
                string & consensus,
                string & db,
                int & gindex,
                int & fontsize,
                QWidget *parent=0, 
                const char *name=0 );

    QSizePolicy sizePolicy() const;
    void mouseDoubleClickEvent(QMouseEvent * event);
    void setWidth(int width) { resize(width, height()); }

protected:
    void paintEvent( QPaintEvent * );

private:
    int & m_fontsize;
    int & m_gindex;

    string & m_db;
    string & m_consensus;

    vector<RenderSeq_t> & m_renderedSeqs;

    vector<RenderSeq_t *> m_currentReads;
};


#endif
