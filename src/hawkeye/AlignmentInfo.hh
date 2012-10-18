#ifndef ALIGNMENTINFO_HH_
#define ALIGNMENTINFO_HH_ 1

#include <vector>

class AlignmentInfo
{
public:
  AlignmentInfo(): m_startshift(0), m_endshift(0) {}
  ~AlignmentInfo() {}

  // global numbering -> contig gindex
  int getContigPos(int gindex)
  {
    int retval = gindex - m_startshift;
    for (unsigned int i = 0; i < m_gaps.size(); i++)
    {
      if (m_gaps[i] < gindex-m_startshift) { retval--; }
    }
    return retval;
  }

  // contig gindex -> global numbering
  inline int getGlobalPos(int gindex)
  {
    return gindex;
    int retval = gindex;
    for (unsigned int i = 0; i < m_gaps.size(); i++)
    {
      if (m_gaps[i] <= gindex) { retval++; }
    }

    return retval + m_startshift;
  }

  int m_startshift;
  int m_endshift;
  std::vector<int> m_gaps;

};

#endif

