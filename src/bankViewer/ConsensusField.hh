#ifndef CONSENSUSFIELD_HH_
#define CONSENSUSFIELD_HH_ 1

#include <string>
#include <qwidget.h>

class ConsensusField : public QWidget
{
  Q_OBJECT

public:
    ConsensusField(string & cons,
                   int & gindex,
                   int & fontsize,
                   QWidget *parent=0,
                   const char *name=0);
    void setWidth(int width) { resize(width, height()); }
protected:
    void paintEvent(QPaintEvent *);

private:
    string & m_consensus;
    int & m_gindex;
    int & m_fontsize;


};

#endif
