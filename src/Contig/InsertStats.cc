#include "InsertStats.hh"
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;


InsertStats::InsertStats(const string & label)
 : m_label(label)
{
}

void InsertStats::addSize(double size)
{
  m_sizes.push_back(size); 
}


int InsertStats::count() const
{
  return m_sizes.size(); 
}


double InsertStats::mean() const 
{ 
  if (m_sizes.size() == 0) { return 0; }

  double sum = 0; 
  for (int i = 0; i < m_sizes.size(); i++)
    { sum += m_sizes[i]; }

  return (sum) / m_sizes.size();
}


double InsertStats::stdev() const
{
  if (m_sizes.size() == 0) { return 0; }

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

void InsertStats::histogram(int buckets, bool fitzero)
{
  if (m_sizes.size() == 0) { return; }

  m_buckets.resize(buckets);
  m_bucketlow.resize(buckets);

  m_low = m_sizes[0];
  m_high = m_sizes[0];

  vector<double>::iterator vi;
  for (vi = m_sizes.begin(); vi != m_sizes.end(); vi++)
  {
    if (*vi < m_low)  { m_low  = *vi; }
    if (*vi > m_high) { m_high = *vi; }
  }

  double base = m_low;
  if (fitzero) { base = 0.0; }

  m_bucketsize = (m_high - base + 1) / (buckets);

  if (m_high <= 1.0)
  {
    m_bucketsize = 1.0L / buckets;
  }

  for (int i = 0; i < buckets; i++)
  {
    m_buckets[i] = 0;
    m_bucketlow[i] = base + (i*m_bucketsize);
  }

  for (vi = m_sizes.begin(); vi != m_sizes.end(); vi++)
  {
    int bucket = (int)floor(((*vi) - base)/m_bucketsize);
    m_buckets[bucket]++;
  }

  m_maxcount = m_buckets[0];
  for (int i = 0; i < buckets; i++)
  {
    if (m_buckets[i] > m_maxcount) { m_maxcount = m_buckets[i]; }
  }
}

int InsertStats::withinSD(double numsd) const
{
  double mea = mean();
  double sd = stdev();

  double lo = mea - numsd * sd;
  double hi = mea + numsd * sd;

  int count = 0;

  vector<double>::const_iterator vi;
  for (vi = m_sizes.begin(); vi != m_sizes.end(); vi++)
  {
    count += (lo <= *vi && *vi <= hi);
  }

  return count;
}
