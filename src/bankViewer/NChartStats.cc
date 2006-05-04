#include "NChartStats.hh"
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;


NChartStats::NChartStats(const string & label)
 : m_label(label), m_sum(0), m_maxsize(0), m_maxscore(0)
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
    if (a.m_score == b.m_score) 
    {
      return a.m_size > b.m_size;
    }
    return (a.m_score > b.m_score);
  }
};

struct StatIdCmp
{
  bool operator () (const StatValue & a, const StatValue & b)
  {
    return a.m_id > b.m_id;
  }
};


void NChartStats::nchart(int sortMethod)
{
  int l = m_sizes.size();

  if (sortMethod == 0)
  {
    sort(m_sizes.begin(), m_sizes.end(), StatLengthCmp());
  }
  else if (sortMethod == 1)
  {
    sort(m_sizes.begin(), m_sizes.end(), StatScoreCmp());
  }
  else if (sortMethod == 2)
  {
    sort(m_sizes.begin(), m_sizes.end(), StatScoreCmp());
  }
}

StatValue NChartStats::nvalue(double perc)
{
  int l = m_sizes.size();

  double cursize = 0;

  for (int i = 0; i < l; i++)
  {
    cursize += m_sizes[i].m_size;
    if ((((double)cursize*100.0) / m_sum) >= perc)
    {
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
