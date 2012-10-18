#ifndef __AMP_HH
#define __AMP_HH

#include <sys/time.h>
#include <string>

// Return a character string representing the current date
const char * Date ( );

// Reverse complement a 1 based coordinate
inline long long int RevComp1 (long long int c, long long int len)
{ return len - c + 1; }

// Reverse complement a 0 based coordinate
inline long long int RevComp0 (long long int c, long long int len)
{ return len - c - 1; }

// Shows progress dots across the screen
class ProgressDots_t
{
private:

  long long total_m; // number of dots currently printed
  long long count_m; // total number of dots to print
  long long end_m;   // end of job progress (progress measured between 0 - end)

public:

  ProgressDots_t (long long end, long long count = 100)
    : total_m (0), count_m (count), end_m (end)
  { }

  void update (long long progress);  // update job progress with respect to end

  void end ( );  // job complete, flush remaining dots and newline
};



// Record length of event in seconds
class EventTime_t
{
public:
  EventTime_t();
  void   start();
  void   end();
  double length();
  std::string str(bool format = true, int precision=2);

private:
  struct timeval m_start;
  struct timeval m_end;
};

#endif // _AMP_HH
