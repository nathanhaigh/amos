#ifndef READINFO_HH_
#define READINFO_HH_ 1

#include <qdialog.h>
#include "RenderSeq.hh"
#include <string>

class ReadInfo: public QDialog
{
public:
  ReadInfo(RenderSeq_t * read, 
           string & db, 
           string & cons,
           QWidget * parent=0, 
           const char *name=0);



};



#endif
