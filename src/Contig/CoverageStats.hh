#ifndef COVERAGE_STATS_HH_
#define COVERAGE_STATS_HH_ 1

#include <foundation_AMOS.hh>
#include <map>
#include <vector>

struct Point_t
{
  Point_t() : m_x(0.0), m_y(0.0) {}
  Point_t(double x, double y) : m_x(x), m_y(y) {}

  double m_x;
  double m_y;

  double x() const { return m_x; }
  double y() const { return m_y; }

  void setX(double x) { m_x = x; }
  void setY(double y) { m_y = y; }
};

typedef std::vector<Point_t> PointArray_t;

class CoverageStats
{
public:
  CoverageStats();
  CoverageStats(int numpoints, AMOS::ID_t libid, AMOS::Distribution_t dist);
  CoverageStats(const CoverageStats &);
  CoverageStats & operator=(const CoverageStats &);

  typedef std::multimap<int, int> EndPoints;

  PointArray_t  m_coverage;
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
