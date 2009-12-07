#include "amp.hh"
#include <new>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <ctime>
using namespace std;

#define MAX_FILE_NAME 1024

const char * Date
     ( )

     // Returns a date and time string

{
  int last;
  char * cp;
  time_t tp;
  time (&tp);
  cp = asctime (localtime (&tp));
  last = strlen(cp) - 1;
  if ( cp [last] == '\n' )
    cp [last] = '\0';
  return cp;
}

void ProgressDots_t::update (long long progress)
{
  int dots = (long long)((double)progress / (double)end_m * (double)count_m);
  while ( total_m < dots  &&  total_m < count_m )
    {
      cerr << '.';
      total_m ++;
    }
}

void ProgressDots_t::end ( )
{
  update (end_m);
  cerr << endl;
}


EventTime_t::EventTime_t()
{
  start();
  memset(&m_end, 0, sizeof(struct timeval));
}

void EventTime_t::start()
{
  gettimeofday(&m_start, NULL);
}

void EventTime_t::end()
{
  gettimeofday(&m_end, NULL);
}

double EventTime_t::length()
{
  if ((m_end.tv_sec == 0) && (m_end.tv_usec == 0)) { end(); }
  return ((m_end.tv_sec - m_start.tv_sec)*1000000.0 + (m_end.tv_usec - m_start.tv_usec)) / 1e6;
}

string EventTime_t::str(bool format, int precision)
{
  double r = length();

  char buffer[1024];
  sprintf(buffer, "%0.*f", precision, r);

  if (format)
  {
    string s("[");
    s += buffer;
    s += "s]";
    return s;
  }

  return buffer;
}

