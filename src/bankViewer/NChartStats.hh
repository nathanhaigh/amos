#ifndef NCHARTSTATS_HH_
#define NCHARTSTATS_HH_ 1

#include <vector>
#include <string>

class StatValue
{
public:
  StatValue(int id, double size) : m_id(id), m_size(size), m_perc(0), m_score(0) {}
  int m_id;
  double m_score;
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
  void      addScore(int id, double val);

  StatValue nvalue(double perc);
  void      nchart(bool sortByScore);

  std::vector<StatValue> m_sizes;
  double m_maxsize;
  double m_sum;
  double m_maxscore;
};


#endif
