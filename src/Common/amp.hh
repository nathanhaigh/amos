#ifndef __AMP_HH
#define __AMP_HH
#include <cstdarg>

// Return a character string representing the current date
const char * Date ( );

// Reverse complement a 1 based coordinate
inline long long int RevComp1 (long long int c, long long int len)
{ return len - c + 1; }

// Reverse complement a 0 based coordinate
inline long long int RevComp0 (long long int c, long long int len)
{ return len - c - 1; }

// concats the return of time(NULL) and rand( ) to the end of prefix,
// creates a file with that path, and returns the name
const char * TempFile (const char * prefix);

// concats the return of time(NULL) and rand( ) to the end of prefix,
// creates a file with that path, and returns the name
const char * TempDir (const char * prefix);

#endif // _AMP_HH
