#include "NChartStats.hh"
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;


NChartStats::NChartStats(const string & label)
 : m_label(label), m_maxsize(0), m_sum(0), m_maxfeat(0)
{
}

void NChartStats::addSize(int id, double size)
{
  m_sum += size;
  if (size > m_maxsize) { m_maxsize = size; }
  m_sizes.push_back(StatValue(id, size)); 
}

struct StatLengthCmp
{
  bool operator () (const StatValue & a, const StatValue & b)
  {
    return a.m_size > b.m_size;
  }
};

struct StatFeatCmp
{
  bool operator () (const StatValue & a, const StatValue & b)
  {
    return a.m_feat > b.m_feat;
  }
};


void NChartStats::nchart(int sorttype)
{
  int l = m_sizes.size();

  if (sorttype == 0)
  {
    sort(m_sizes.begin(), m_sizes.end(), StatLengthCmp());
  }
  else if (sorttype == 1)
  {
    sort(m_sizes.begin(), m_sizes.end(), StatFeatCmp());
  }


  double cur = m_sum;

  for (int i = 0; i < l; i++)
  {
    m_sizes[i].m_perc = cur*100/m_sum;
    cur -= m_sizes[i].m_size;
  }
}

StatValue NChartStats::nvalue(double perc)
{
  int l = m_sizes.size();

  for (int i = 0; i < l; i++)
  {
    if (m_sizes[i].m_perc < perc)
    {
      if (i > 0) { return m_sizes[i-1]; }
      return m_sizes[i];
    }
  }

  return StatValue(0,0);
}

int NChartStats::count() const
{
  return m_sizes.size();
}

void NChartStats::addFeat(int id)
{
  int l = m_sizes.size();

  for (int i = 0; i < l; i++)
  {
    if (m_sizes[i].m_id == id)
    {
      m_sizes[i].m_feat++;
      if (m_sizes[i].m_feat > m_maxfeat) { m_maxfeat = m_sizes[i].m_feat; }

      break;
    }
  }
}
