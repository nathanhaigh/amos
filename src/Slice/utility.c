/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

/*! @file
 *  @brief Utility Functions on slices
 */

#include <ctype.h>
#include "Slice.h"
#include "version.h"
#include "string.h" // for strncpy

//! Get the compliment to a base or ambiguity code
/*! 
 *  @param base Ambiguity Code/Base to get compliment
 *  @return Compliment of the base (case is preserved)
 */
char libSlice_getCompliment(char base)
{
  char retval = base;
  int islow = islower(base);

  switch (toupper(base))
  {
    case 'A': retval = 'T'; break;
    case 'T': retval = 'A'; break;
    case 'C': retval = 'G'; break;
    case 'G': retval = 'C'; break;

    case 'M': retval = 'K'; break;
    case 'R': retval = 'Y'; break;
    case 'W': retval = 'W'; break;
    case 'S': retval = 'S'; break;
    case 'Y': retval = 'R'; break;
    case 'K': retval = 'M'; break;

    case 'V': retval = 'B'; break;
    case 'H': retval = 'D'; break;
    case 'D': retval = 'H'; break;
    case 'B': retval = 'V'; break;

    case 'N': retval = 'N'; break;
  };

  if (islow)
  {
    retval = tolower(retval);
  }

  return retval;
}

#include <stdio.h>

//! Gets a string representing the version of the library
/*! Creates a string like: "libSlice Version 1.01 (Build: 1.11)"
 *
 *  Note: The value is a pointer to a static buffer, so the value 
 *  should not be freed.
 */
const char * libSlice_getVersion()
{
  static char buffer[256];
  memset(buffer, 0, sizeof(buffer));

  strcpy(buffer, gVersionStr);
  strcat(buffer, " (Build: ");
  strcat(buffer, gRevisionStr+11);
  buffer[strlen(buffer)-2] = ')';
  buffer[strlen(buffer)-1] = 0;

  return buffer;
}
