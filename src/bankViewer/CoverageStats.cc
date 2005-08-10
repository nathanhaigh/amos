#include "CoverageStats.hh"

using namespace AMOS;
using namespace std;

CoverageStats::CoverageStats(int numpoints, ID_t libid, Distribution_t dist) :
   m_coverage(numpoints),
   m_cestat(numpoints),
   m_libid(libid),
   m_dist(dist),
   m_maxdepth(0),
   m_curpos(0),
   m_sum(0)
{ }


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
    double numerator = m_cestat[i].y() - m_dist.mean;
    double denominator = m_dist.sd * sqrt((double)m_coverage[i].y());
    double val = (denominator) ? (numerator)/(denominator) :  numerator / 0.0001;

    if (m_coverage[i].y() == 0) { val = 0; }

    val *= 100;
    val /= 2;

    if (val > half)       { val = half; }
    else if (val < -half) { val = -half; }

    val += half;

    m_cestat[i].setY((int)val);
    //cerr << "lib: " << m_libid << " x: " << m_cestat[i].x() << " ce: " << m_cestat[i].y() 
    //     << " val: " << val << " = " << numerator << " / " << denominator << endl;
  }
}

void CoverageStats::normalize(float hscale, int hoffset, int voffset, bool adjustCE)
{
  // Adjust coordinates for painting
  for (int i = 0; i < m_curpos; i++)
  {
    m_coverage[i].setX((int)((m_coverage[i].x()+hoffset) * hscale));
    m_coverage[i].setY(voffset-m_coverage[i].y());

    if (adjustCE)
    {
      m_cestat[i].setX((int)((m_cestat[i].x()+hoffset) * hscale));
      m_cestat[i].setY(voffset-m_cestat[i].y());
    }
  }
}


int CoverageStats::handlePoint(int pos, int eps, int sumdelta)
{
  m_coverage[m_curpos] = QPoint(pos, eps); 
  m_cestat[m_curpos]   = QPoint(pos, eps ? m_sum/eps : 0); 
  m_curpos++; 

  if (sumdelta < 0) { eps--; }
  else              { eps++; }

  m_sum += sumdelta;

  m_coverage[m_curpos] = QPoint(pos, eps); 
  m_cestat[m_curpos]   = QPoint(pos, eps ? m_sum/eps : 0); 
  m_curpos++; 

  return eps;
}

