#ifndef CONSENSUSFIELD_HH_
#define CONSENSUSFIELD_HH_ 1

#include <string>
#include <qframe.h>

class ConsensusField : public QFrame
{
  Q_OBJECT

public:
    ConsensusField(string & cons,
                   int & gindex,
                   QWidget *parent=0,
                   const char *name=0);
    void setWidth(int width) { resize(width, height()); }

public slots:
    void setFontSize(int fontsize);

protected:
    void paintEvent(QPaintEvent *);

private:
    string & m_consensus;
    int & m_gindex;
    int m_fontsize;


};

#endif
