#ifndef READINFO_HH_
#define READINFO_HH_ 1

#include <qdialog.h>
#include "RenderSeq.hh"
#include <string>
#include <qscrollview.h>
#include "ChromoField.hh"

class ReadInfo: public QDialog
{
  Q_OBJECT

public:
  ReadInfo(RenderSeq_t * read, 
           const std::string & db, 
           const std::string & cons,
           const std::string & cstatus,
           QWidget * parent=0, 
           const char *name=0);

public slots:
  void setTilingVisibleRange(int contigid, int grangeStart, int grangeEnd);

private:
  QScrollView * m_sv;
  ChromoField * m_chromo;
  RenderSeq_t m_read;
};

#endif
