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
    virtual void mouseReleaseEvent(QMouseEvent * event);

public slots:
    void setGindex(int gindex);
    void setContigId(int contigID);
    void setBankname(string bankname);
    void setFontSize(int);
    void setDB(const QString & db);

signals:
    void gindexChanged( int );
    void setGindexRange( int, int );
    void contigRange(int, int);
    void contigLoaded(int);
    void setStatus(const QString &);

protected:
    void paintEvent( QPaintEvent * );

private:
    int m_fontsize;
    int m_gindex;
    int m_displaywidth;
    bool m_loaded;

    string m_bankname;
    std::vector<AMOS::Tile_t> m_tiling;
    string m_consensus;
    AMOS::Bank_t read_bank;
    AMOS::Bank_t contig_bank;
    std::vector<RenderSeq_t> m_renderedSeqs;

    vector<RenderSeq_t *> m_currentReads;
    int m_desiredheight;

    string m_db;
};


#endif
