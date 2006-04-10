#ifndef OVERVIEW_WIDGET_HH__
#define OVERVIEW_WIDGET_HH__ 1

#include <qwidget.h>
#include <qcanvas.h>
#include <vector>
#include "foundation_AMOS.hh"


using std::string;

class DataStore;
class OverviewField;

class OverviewWidget : public QWidget
{
  Q_OBJECT

public:

  OverviewWidget(DataStore * datastore,
                 QWidget * parent = 0,
                 const char * name = 0);

public slots:

void initializeTiling();
void paintCanvas();


protected:
  void resizeEvent (QResizeEvent*);

private:

  DataStore * m_datastore;
  OverviewField * m_ofield;
  QCanvas * m_ocanvas;
  int m_tilingwidth;

  std::vector<AMOS::Feature_t> m_features;
  std::vector<AMOS::Tile_t> m_contigs;
};

#endif
