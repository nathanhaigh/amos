#ifndef CHROMOFIELD_HH_
#define CHROMOFIELD_HH_

#include <qwidget.h>
#include <qpixmap.h>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C"
{
#endif

#include <Read.h>

#ifdef __cplusplus
}
#endif

class RenderSeq_t;

class ChromoField: public QWidget
{
  Q_OBJECT
public:
  ChromoField(RenderSeq_t * read, 
              const std::string & db,
              const std::string & cons,
              const std::string & cstatus,
              QWidget *parent=0, 
              const char *name=0);

  int getWindowPos(int gindex);

protected:
  void paintEvent( QPaintEvent * );

private:
  Read * m_trace;
  RenderSeq_t * m_render;
  QPixmap * m_pix;
  std::vector<int> m_pos;
  double m_hscale;
  int m_hoffset;
  bool m_contigView;
};



#endif
