#include "amp.hh"

const char * Date
     ( )

     // Returns a date and time string

{
  time_t tp;
  time (&tp);
  return asctime (localtime (&tp));
}
