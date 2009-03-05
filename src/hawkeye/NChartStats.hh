#ifndef NCHARTSTATS_HH_
#define NCHARTSTATS_HH_ 1

#include <vector>
#include <string>
#include <qrect.h>

class StatValue
{
public:
  StatValue(int id, double size) : m_id(id), m_size(size), m_score(0), m_pivotlevel(0) {}
  int m_id;
  int m_pivotlevel;
  double m_score;
  double m_size;

  QRect m_rect;
};

class NChartStats
{
public:
  NChartStats(const std::string & label);
  std::string m_label;

  int       count() const;
  void      addSize(int id, double size);
  void      addScore(int id, double val);

  StatValue nvalue(double perc);
  void      nchart(int sortMethod);

  std::vector<StatValue> m_sizes;
  double m_sum;
  double m_maxscore;
  double m_maxsize;
};


#endif
