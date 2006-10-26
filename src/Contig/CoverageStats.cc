#include "CoverageStats.hh"
#include <math.h>

using namespace AMOS;
using namespace std;

CoverageStats::CoverageStats() :
  m_coverage(0),
  m_libid(0),
  m_maxdepth(0),
  m_curpos(0),
  m_sum(0)
{}

CoverageStats::CoverageStats(int numpoints, ID_t libid, Distribution_t dist) :
   m_coverage(numpoints),
   m_libid(libid),
   m_dist(dist),
   m_maxdepth(0),
   m_curpos(0),
   m_sum(0)
{ 
  //cerr << "Allocated to hold " << numpoints << " values" << endl;
  m_cestat.resize(numpoints);
}

CoverageStats::CoverageStats(const CoverageStats & other)
{
  *this = other;
}

CoverageStats & CoverageStats::operator=(const CoverageStats & other)
{
  if ( this != &other )
    {
      m_coverage = other.m_coverage;
      m_cestat = other.m_cestat;
      m_libid = other.m_libid;
      m_dist = other.m_dist;
      m_maxdepth = other.m_maxdepth;
      m_curpos = other.m_curpos;
      m_sum = other.m_sum;
      m_endpoints = other.m_endpoints;
    }

  return *this;
}

void CoverageStats::addEndpoints(int curloffset, int curroffset)
{
  EndPoints::iterator vi, vi2;
  int eps = m_endpoints.size();

  // find end points that have already passed
  vi = m_endpoints.begin();
  while (vi != m_endpoints.end())
  {
    if (vi->first <= curloffset) 
    { 
      eps = handlePoint(vi->first, eps, -vi->second);
      vi2 = vi; vi2++; m_endpoints.erase(vi); vi = vi2;
    }
    else
    { 
      break; 
    }
  }

  int len = curroffset - curloffset + 1;

  // Add this insert's beginning and end
  eps = handlePoint(curloffset, eps, +len);
  m_endpoints.insert(make_pair(curroffset, len));

  if (eps > m_maxdepth) { m_maxdepth = eps; }

  if (eps != m_endpoints.size())
  {
    cerr << "eps: " << eps << " m_endpoints.size():" << m_endpoints.size() << endl;
  }
}

void CoverageStats::finalize()
{
  EndPoints::iterator vi, vi2;

  int eps = m_endpoints.size();

  // Handle remaining end points
  
  for (EndPoints::iterator vi = m_endpoints.begin(); vi != m_endpoints.end(); vi++)
  {
    eps = handlePoint(vi->first, eps, -vi->second);
  }

  m_endpoints.clear();

  if (eps != 0 || m_endpoints.size() != 0)
  {
    cerr << "not zero eps: " << eps << " m_endpoints.size():" << m_endpoints.size() << endl;
  }

  if (m_sum != 0)
  {
    cerr << "not zero sum: " << m_sum << endl;
  }
}

void CoverageStats::finalizeCE(int vheight)
{
  int half = vheight / 2;

  for (int i = 0; i < m_curpos; i++)
  {
    double numerator = (m_cestat[i] - m_dist.mean) * sqrt((double)m_coverage[i].y());    
    double denominator = m_dist.sd;
    double val = (denominator) ? (numerator)/(denominator) :  numerator / 0.0001;

    // cestat gets the raw ce value
    m_cestat[i] = val;

    int plotval = (int)(val*8);

    if (plotval > half)       { plotval = half; }
    else if (plotval < -half) { plotval = -half; }

    plotval += half;

    // make sure plotval is within the vertical range
    plotval = max(0,plotval);
    plotval = min(vheight, plotval);

    // coverage gets the value to plot
    m_coverage[i].setY(plotval);

    //cerr << "lib: " << m_libid << " x: " << m_coverage[i].x() << " plotted: " << m_coverage[i].y() 
    //     << " ce: " << val << " = " << numerator << " / " << denominator << endl;
  }
}

void CoverageStats::normalize(float hscale, int hoffset, int voffset)
{
  // Adjust coordinates for painting
  for (int i = 0; i < m_curpos; i++)
  {
    m_coverage[i].setX((int)((m_coverage[i].x()+hoffset) * hscale));
    m_coverage[i].setY(voffset-m_coverage[i].y());
  }
}


int CoverageStats::handlePoint(int pos, int eps, int sumdelta)
{
  if (m_curpos+1 >= m_coverage.size())
  {
    cerr << "WARNING: resizing coverage array" << endl;
    m_coverage.resize(m_coverage.size()*2+2);
    m_cestat.resize(m_cestat.size()*2+2);
  }

  m_coverage[m_curpos] = Point_t(pos, eps); 
  m_cestat[m_curpos]   = eps ? m_sum/eps : 0; 
  m_curpos++; 

  if (sumdelta < 0) { eps--; }
  else              { eps++; }

  m_sum += sumdelta;

  m_coverage[m_curpos] = Point_t(pos, eps); 
  m_cestat[m_curpos]   = eps ? m_sum/eps : 0; 
  m_curpos++; 

  return eps;
}


void CoverageStats::addPoint(int x, int y)
{
  m_coverage[m_curpos] = Point_t(x,y);
  m_curpos++;

  if (y > m_maxdepth) { m_maxdepth = y; }
}
