#include "amp.hh"
#include <new>
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
