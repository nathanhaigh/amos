#ifndef CONSENSUSFIELD_HH_
#define CONSENSUSFIELD_HH_ 1

#include <vector>
#include <string>
#include <qframe.h>

class AlignmentInfo;

class ConsensusField : public QFrame
{
  Q_OBJECT

public:
    ConsensusField(const std::string & consensus,
                   const std::string & cstatsus,
                   const std::string & consqual,
                   const std::vector<int> & ugappos,
                   AlignmentInfo * ai,
                   int & gindex,
                   int & contigid,
                   QWidget *parent=0,
                   const char *name=0);
    void setWidth(int width) { resize(width, height()); }
    void mouseReleaseEvent(QMouseEvent * e);

public slots:
    void setFontSize(int fontsize);
    void toggleShowNumbers(bool doShow);
    void toggleShowConsQV(bool doShow);
    void toggleHighlightDiscrepancy(bool doShow);
    void toggleBaseColors(bool show);
    void toggleShowIndicator(bool show);
    void setHighlightRange(int start, int end);
    void toggleShowUngapped(bool show);

signals:
    void sortColumns(int);

protected:
    void paintEvent(QPaintEvent *);

private:
    const std::string & m_consensus;
    const std::string & m_cstatus;
    const std::string & m_consqual;
    const std::vector<int> & m_ugpos;
    AlignmentInfo * m_alignment;
    int & m_gindex;
    int & m_contigId;

    int m_fontsize;
    bool m_shownumbers;
    bool m_highlightdiscrepancy;
    bool m_basecolors;
    bool m_showIndicator;
    bool m_displayQV;
    bool m_showUngapped;

    int m_lineheight;
    int m_tilehoffset;
    int m_seqnamehoffset;
    double m_basewidth;
    int m_basespace;
    int m_diam;
    int m_lineoffset;
    int m_posoffset;
    int m_discoffset;
    int m_consoffset;

    int m_rangestart;
    int m_rangeend;
};

#endif
