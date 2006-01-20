#include "InsertStats.hh"
#include <cmath>


InsertStats::InsertStats() {}

void InsertStats::addSize(int size)
{
  m_sizes.push_back(size); 
}


int InsertStats::count() const
{
  return m_sizes.size(); 
}


double InsertStats::mean() const 
{ 
  long sum = 0; 
  for (int i = 0; i < m_sizes.size(); i++)
    { sum += m_sizes[i]; }

  return ((double)sum) / m_sizes.size();
}


double InsertStats::stdev() const
{
  double mea = mean();

  double sumdiff = 0;

  for (int i = 0; i < m_sizes.size(); i++)
  {
    double diff = m_sizes[i] - mea;
    sumdiff += (diff *diff);
  }

  sumdiff /= m_sizes.size() -1;

  return sqrt(sumdiff);
}
