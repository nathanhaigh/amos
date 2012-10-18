#ifndef NCHART_WINDOW_HH__
#define NCHART_WINDOW_HH__ 1


#include "NChartWindowBase.hh"

class NChartStats;

class NChartWindow : public NChartWindowBase
{
  Q_OBJECT

public:
  NChartWindow(NChartStats * stats, QWidget * parent, const char * name);
};

#endif
