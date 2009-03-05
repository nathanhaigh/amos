#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "ChromoStore.hh"


using namespace std;

ChromoStore * ChromoStore::Instance () 
{
  static ChromoStore inst;
  return &inst;
}

ChromoStore::ChromoStore() 
{ 
  m_tracepaths.push_back("/local/chromo/Chromatograms/%TRACEDB%/ABISSed/%EID3%/%EID4%/%EID5%/");
  m_tracepaths.push_back("/local/chromo2/Chromatograms/%TRACEDB%/ABISSed/%EID3%/%EID4%/%EID5%/");
  m_tracepaths.push_back("/local/chromo3/Chromatograms/%TRACEDB%/ABISSed/%EID3%/%EID4%/%EID5%/");

  m_tracepaths.push_back("/fs/szasmg/Chromatograms/%TRACEDB%/");
  m_tracepaths.push_back("chromat_dir");
  m_tracepaths.push_back("../chromat_dir");

  m_tracecache        = ".tracecache";
  m_tracecachecreated = 0;
  m_tracecmd          = "curl \"http://www.ncbi.nlm.nih.gov/Traces/trace.fcgi?cmd=java&val=%EID%\" -s -o %TRACECACHE%/%EID%";
  m_tracecmdpath      = "%TRACECACHE%/%EID%";
  m_tracecmdenabled   = 0;
}

ChromoStore::~ChromoStore()
{
  if (m_tracecachecreated)
  {
    cerr << "Cleaning tracecache directory: " << m_tracecache << endl;
    string cmd = "/bin/rm -rf " + m_tracecache;
    system(cmd.c_str());
  }
}


static void replaceAll(string & str,
                       const string & token,
                       const string & value)
{
  int startpos;
  int endpos;

  string searchtoken = "%" + token;

  while ((startpos = str.find(searchtoken)) != str.npos)
  {
    string replaceval;
    endpos = startpos+searchtoken.length();

    if (str[endpos] == '%')
    {
      replaceval = value;
    }
    else
    {
      // Figure out how long of a prefix of value to replace
      int len = atoi(str.c_str()+endpos);
      if (len > value.length()) { len = value.length(); }

      // grab the first len characters of value
      replaceval = value.substr(0, len);

      // find the end token
      while (str[endpos] != '%' && (endpos < str.length()))
      {
        endpos++;
      }
    }

    str.replace(startpos, endpos-startpos+1, replaceval);
  }
}

string ChromoStore::replaceTraceTokens(const string & str,
                                const string & eid,
                                const string & iid)
{
  string result(str);

  replaceAll(result, "TRACECACHE", m_tracecache);
  replaceAll(result, "TRACEDB",    m_tracedb);
  replaceAll(result, "EID",  eid);
  replaceAll(result, "IID",  iid);

  return result;
}



extern "C"
{
  #include "Read.h"
}

char * ChromoStore::fetchTrace(const AMOS::Read_t & read, 
                             std::vector<int16_t> & positions )
{
  string eid = read.getEID();

  char iidarr[16];
  sprintf(iidarr, "%d", read.getIID());
  string iid(iidarr);

  Read * trace = NULL;

  string path;

  vector <string>::iterator ci;
  for (ci =  m_tracepaths.begin();
       ci != m_tracepaths.end() && !trace;
       ci++)
  {
    path = replaceTraceTokens(*ci, eid, iid);

    DIR * dir = opendir(path.c_str());
    if (dir)
    {
      closedir(dir);
      path += "/" + eid;
      trace = read_reading((char *)path.c_str(), TT_ANY);
    }
  }

  if (!trace && m_tracecmdenabled)
  {
    if (!m_tracecachecreated)
    {
      int retval = mkdir(m_tracecache.c_str(), 
                         S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH );
      m_tracecachecreated = !retval;
    }

    string cmd = replaceTraceTokens(m_tracecmd, eid, iid);
    path = replaceTraceTokens(m_tracecmdpath, eid, iid);

    cerr << "**** Executing: \"" << cmd << "\"" << endl;

    int retval = system(cmd.c_str());

    if (!retval)
    {
      trace = read_reading((char *)path.c_str(), TT_ANY);
    }
  }

  // Load positions out of trace
  if (trace && positions.empty() && trace->basePos)
  {
    for (int i = 0; i < trace->NBases; i++)
    {
      positions.push_back(trace->basePos[i]);
    }
  }

  return (char *) trace;
}


