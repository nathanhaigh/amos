#ifndef CONSENSUSFIELD_HH_
#define CONSENSUSFIELD_HH_ 1

#include <string>
#include <qframe.h>

class ConsensusField : public QFrame
{
  Q_OBJECT

public:
    ConsensusField(const string & consensus,
                   const string & cstatsus,
                   int & gindex,
                   QWidget *parent=0,
                   const char *name=0);
    void setWidth(int width) { resize(width, height()); }

public slots:
    void setFontSize(int fontsize);
    void setShowNumbers(bool doShow);
    void toggleHighlightDiscrepancy(bool doShow);

protected:
    void paintEvent(QPaintEvent *);

private:
    const string & m_consensus;
    const string & m_cstatus;
    int & m_gindex;
    int m_fontsize;
    bool m_shownumbers;
    bool m_highlightdiscrepancy;

    int m_lineheight;
    int m_tilehoffset;
    int m_seqnamehoffset;
    int m_basewidth;
    int m_basespace;
    int m_diam;
    int m_lineoffset;
    int m_posoffset;
    int m_discoffset;
    int m_consoffset;
};

#endif
