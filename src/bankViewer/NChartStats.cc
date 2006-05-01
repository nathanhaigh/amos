#include "NChartStats.hh"
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;


NChartStats::NChartStats(const string & label)
 : m_label(label), m_maxsize(0), m_sum(0), m_maxscore(0)
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

struct StatScoreCmp
{
  bool operator () (const StatValue & a, const StatValue & b)
  {
    return a.m_score > b.m_score;
  }
};


void NChartStats::nchart(bool sortByScore)
{
  int l = m_sizes.size();

  if (sortByScore)
  {
    sort(m_sizes.begin(), m_sizes.end(), StatScoreCmp());
  }
  else
  {
    sort(m_sizes.begin(), m_sizes.end(), StatLengthCmp());
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

void NChartStats::addScore(int id, double val)
{
  int l = m_sizes.size();

  for (int i = 0; i < l; i++)
  {
    if (m_sizes[i].m_id == id)
    {
      m_sizes[i].m_score+=val;
      if (m_sizes[i].m_score > m_maxscore) { m_maxscore = m_sizes[i].m_score; }

      break;
    }
  }
}
