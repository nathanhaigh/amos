#ifndef CHROMO_STORE_HH_
#define CHROMO_STORE_HH_ 1

#include "foundation_AMOS.hh"
#include <string>
#include <map>
#include <vector>


class ChromoStore 
{
public:
  static ChromoStore * Instance();
  ~ChromoStore();

  char * fetchTrace(const AMOS::Read_t & read, std::vector<int16_t> & positions);


  std::vector <std::string> m_tracepaths;
  std::string m_tracecache;
  std::string m_tracecmd;
  std::string m_tracecmdpath;
  std::string m_tracedb;
  bool m_tracecmdenabled;




protected:
  ChromoStore();
  ChromoStore(const ChromoStore&);
  ChromoStore& operator= (const ChromoStore&);


private:
  std::string replaceTraceTokens(const std::string & str,
                                 const std::string & eid,
                                 const std::string & iid);

  bool m_tracecachecreated;

  
};

#endif
