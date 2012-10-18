//  A. L. Delcher
//
//  File:  delcher.hh
//
//  Last Modified:  11 September 2003
//
//  Common generic routines.


#ifndef  __DELCHER_HH
#define  __DELCHER_HH

#include  <cstdio>
#include  <cstdlib>
#include  <iostream>
#include  <iomanip>
#include  <fstream>
#include  <cmath>
#include  <cstring>
#include  <cctype>
#include  <climits>
#include  <cfloat>
#include  <ctime>
#include  <cassert>
#include  <errno.h>
#include  <unistd.h>


#ifndef TRUE
#define  TRUE  1
#endif

#ifndef FALSE
#define  FALSE  0
#endif 

#ifndef  EXIT_FAILURE
  #define  EXIT_FAILURE  -1
#endif
#ifndef  EXIT_SUCCESS
  #define  EXIT_SUCCESS  0
#endif


const int  MAX_ERROR_MSG_LEN = 1000;
  // Length of longest possible error message


extern char  Clean_Exit_Msg_Line [MAX_ERROR_MSG_LEN];
  // String to write error messages to before exiting
extern int  Verbose;
  // Flag to determine level of debugging output


const char *  Commatize
    (long int  n);
void  Clean_Exit
    (const char * msg, const char * src_fname = NULL, size_t line_num = 0);
FILE *  File_Open
    (const char * fname, const char * mode, const char * src_fname = NULL,
     size_t line_num = 0);
double  Percent
    (double a, double b);
double  Pseudo_Normal
    (void);
double  Ratio
    (double a, double b);
void *  Safe_calloc
    (size_t n, size_t len, const char * src_fname = NULL,
     size_t line_num = 0);
void *  Safe_malloc
    (size_t len, const char * src_fname = NULL, size_t line_num = 0);
void *  Safe_realloc
    (void * q, size_t len, const char * src_fname = NULL,
     size_t line_num = 0);
char *  Strip_Trailing
    (char * s, char ch);
void  Uppercase
    (char * s);



template <class DT>
void  Incr_Limited  (DT & A, DT limit)

// Increment  A  by 1, but only if it's less than  limit .

  {
   if  (A < limit)
       A ++;

   return;
  }



template <class DT>
DT  Max  (DT A, DT B)

// Return the larger of  A  and  B .

  {
   if  (A > B)
       return  A;
     else
       return  B;
  }



template <class DT>
DT  Min  (DT A, DT B)

// Return the smaller of  A  and  B .

  {
   if  (A < B)
       return  A;
     else
       return  B;
  }



template <class DT>
void  Swap  (DT & A, DT & B)

// Swap the values in  A  and  B .

  {
   DT  Save;

   Save = A;
   A = B;
   B = Save;

   return;
  }



#endif // #ifndef __DELCHER_HH
