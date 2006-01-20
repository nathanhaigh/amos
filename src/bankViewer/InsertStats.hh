#ifndef INSERTSTATS_HH_
#define INSERTSTATS_HH_ 1

#include <vector>

class InsertStats
{
public:
  InsertStats();

  void   addSize(int size);

  int    count() const;
  double mean()  const;
  double stdev() const;
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
