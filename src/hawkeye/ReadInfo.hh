#ifndef READINFO_HH_
#define READINFO_HH_ 1

#include <qdialog.h>
#include <string>
#include <q3scrollview.h>
#include "RenderSeq.hh"

class ChromoField;

class ReadInfo: public QDialog
{
  Q_OBJECT

public:
  ReadInfo(RenderSeq_t * read, 
           const std::string & cons,
           const std::string & cstatus,
           QWidget * parent=0, 
           const char *name=0);

public slots:
  void setTilingVisibleRange(int contigid, int grangeStart, int grangeEnd);

private:
  Q3ScrollView * m_sv;
  ChromoField * m_chromo;
  RenderSeq_t m_read;
};

#endif
