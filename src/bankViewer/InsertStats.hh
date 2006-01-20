#ifndef INSERTSTATS_HH_
#define INSERTSTATS_HH_ 1

#include <vector>
#include <string>

class InsertStats
{
public:
  InsertStats(const std::string & label);
  std::string m_label;

  void   addSize(int size);

  int    count() const;
  double mean()  const;
  double stdev() const;
  int    withinSD(double numsd) const;

  std::vector<int> m_sizes;

  void   histogram(int buckets, bool fitzero=true);
  std::vector<int>    m_buckets;
  std::vector<double> m_bucketlow;
  double m_bucketsize;
  int m_low;
  int m_high;
  int m_maxcount;
};


#endif
