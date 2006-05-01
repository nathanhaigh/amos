#ifndef NCHARTSTATS_HH_
#define NCHARTSTATS_HH_ 1

#include <vector>
#include <string>

class StatValue
{
public:
  StatValue(int id, double size) : m_id(id), m_size(size), m_perc(0), m_feat(0) {}
  int m_id;
  int m_feat;
  double m_size;
  double m_perc;
};

class NChartStats
{
public:
  NChartStats(const std::string & label);
  std::string m_label;

  int       count() const;
  void      addSize(int id, double size);
  void      addFeat(int id);

  StatValue nvalue(double perc);
  void      nchart(int ordering);

  std::vector<StatValue> m_sizes;
  double m_maxsize;
  double m_sum;
  int m_maxfeat;
};


#endif
