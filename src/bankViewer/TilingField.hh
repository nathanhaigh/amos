#ifndef TILING_H
#define TILING_H

#include <qwidget.h>
#include "RenderSeq.hh"

class TilingField : public QWidget
{
    Q_OBJECT
public:
    TilingField( QWidget *parent=0, const char *name=0 );

    int gindex() const { return m_gindex; }
    QSizePolicy sizePolicy() const;

public slots:
    void setGindex( int gindex );
    void loadContig(string bankname, int contigID);

signals:
    void gindexChanged( int );
    void setRange( int, int );

protected:
    void paintEvent( QPaintEvent * );

private:
    int m_gindex;
    int m_displaywidth;
    bool m_loaded;
    std::vector<AMOS::Tile_t> m_tiling;
    string m_consensus;
    AMOS::Bank_t read_bank;
    AMOS::Bank_t contig_bank;
    std::vector<RenderSeq_t> m_renderedSeqs;
};


#endif
