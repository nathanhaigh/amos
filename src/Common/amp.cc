#include "amp.hh"
#include "alloc.hh"
#include <new>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
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

const char * TempFile (const char * prefix)
{
  static char name [MAX_FILE_NAME];
  snprintf (name, MAX_FILE_NAME, "%s%ld%d", prefix, time(NULL), rand( ));
  int fd = creat (name, 0644);
  if ( fd == -1 )
    throw std::exception();
  close (fd);
  return name;
}

const char * TempDir (const char * prefix)
{
  static char name [MAX_FILE_NAME];
  snprintf (name, MAX_FILE_NAME, "%s%ld%d", prefix, time(NULL), rand( ));
  if ( mkdir (name, 0755) == -1 )
    throw std::exception();
  return name;
}
