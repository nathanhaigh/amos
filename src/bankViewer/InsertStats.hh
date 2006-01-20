#ifndef INSERTSTATS_HH_
#define INSERTSTATS_HH_ 1

#include <cmath>
#include <vector>


class InsertStats
{
public:
  InsertStats();

    void addSize(int size);
     int count() const;
  double mean()  const;
  double stdev() const;

  std::vector<int> m_sizes;
};


#endif
