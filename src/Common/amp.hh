#ifndef __AMP_HH
#define __AMP_HH

#include <ctime>
#include <cstring>

// Return a character string representing the current date
const char * Date ( );

// Reverse complement a 1 based coordinate
inline long long int RevComp1 (long long int c, long long int len)
{ return len - c + 1; }

// Reverse complement a 0 based coordinate
inline long long int RevComp0 (long long int c, long long int len)
{ return len - c - 1; }

#endif // _AMP_HH
