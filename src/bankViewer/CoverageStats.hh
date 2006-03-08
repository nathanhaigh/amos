#ifndef COVERAGE_STATS_HH_
#define COVERAGE_STATS_HH_ 1

#include <foundation_AMOS.hh>
#include <qpointarray.h>
#include <map>
#include <vector>

class CoverageStats
{
public:
  CoverageStats(int numpoints, AMOS::ID_t libid, AMOS::Distribution_t dist);

  typedef std::multimap<int, int> EndPoints;

  QPointArray  m_coverage;
  std::vector<double> m_cestat;

  AMOS::ID_t m_libid;
  AMOS::Distribution_t m_dist;

  int m_maxdepth;
  int m_curpos;

  void addEndpoints(int curloffset, int curroffset);
  void finalize();
  void finalizeCE(int vheight);
  void normalize(float hscale, int hoffset, int voffset);

  void addPoint(int x, int y);

private:
  int handlePoint(int pos, int eps, int sumdelta);
  int m_sum;
  EndPoints m_endpoints;
};



#endif
