/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

/*! @file
 *  @brief getConsQC and getConsQCRange Implementation
 *  \n
 *
 *  C code adaptation of Martin Shumway's consensus class script from cutAsm.
 *  Return values range from 0 to 23. A return value of 0 indicates an error 
 *  has occured calculating the quality class. Quality classes 1-23
 *  correspond to the quality classes used in TIGR Assembler.
 */

#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Slice.h"


// Bit Flags
// Quality in both directions
#define QUALITY_OTHER_STRAND     0x1  

// No conflicts with existing consensus
#define NO_CONFLICT              0x2  

// Quality in at least one direction
#define QUALITY_STRAND           0x4  

// Low quality read in other direction
#define NON_QUALITY_OTHER_STRAND 0x8  

// More than one in quality direction
#define QUALITY_SAME_STRAND      0x10 

// Non quality read in high or 2 low reads
#define NON_QUALITY_SAME_STRAND  0x20 

// Conflict with low quality read
#define NON_QUALITY_CONFLICT     0x40 

// Only one read
#define SINGLE_COVERAGE          0x80 

// Low quality read in same direction
#define NON_QUALITY_STRAND       0x100

// High quality conflict
#define QUALITY_CONFLICT         0x200

// Ambigious existing consensus
#define AMBIGUITY                0x400

// for (NON_QUALITY_SAME_STRAND || QUALITY_SAME_STRAND)
#define ANY_SAME_STRAND          0x800

// Consensus is a gap
#define GAP_CONSENSUS            0x1000

// Depth of Coverage == 0
#define DEPTHZERO                0x2000

#define FORWARD                  0
#define REVERSE                  1

// Quality classes
#define QUALITY_CLASS_0     0  
#define QUALITY_CLASS_1     1  
#define QUALITY_CLASS_2     2 
#define QUALITY_CLASS_3     3
#define QUALITY_CLASS_4     4
#define QUALITY_CLASS_5     5
#define QUALITY_CLASS_6     6
#define QUALITY_CLASS_7     7
#define QUALITY_CLASS_8     8
#define QUALITY_CLASS_9     9
#define QUALITY_CLASS_10    10
#define QUALITY_CLASS_11    11
#define QUALITY_CLASS_12    12
#define QUALITY_CLASS_13    13
#define QUALITY_CLASS_14    14
#define QUALITY_CLASS_15    15
#define QUALITY_CLASS_16    16
#define QUALITY_CLASS_17    17
#define QUALITY_CLASS_18    18
#define QUALITY_CLASS_19    19
#define QUALITY_CLASS_20    20
#define QUALITY_CLASS_21    21
#define QUALITY_CLASS_22    22
#define QUALITY_CLASS_23    23

#define QUALITY_CLASS_ERROR 0


/*! @internal
 *  Convert a bit field coded integer into associated quality class
 */
char QUALITY_CLASS(int x)
{
  // Set the AND_SAME_STRAND bit if either of the same values are true
  if ((x & NON_QUALITY_SAME_STRAND) || (x & QUALITY_SAME_STRAND))
  {
    x += ANY_SAME_STRAND;
  }

  if (x & DEPTHZERO)
  {
    return QUALITY_CLASS_0;
  }
  else if ((x & GAP_CONSENSUS) &&  
           (x & NO_CONFLICT))
  {
    return QUALITY_CLASS_21;
  }
  else if ((x & NO_CONFLICT) && 
           (x & QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_1;
  }
  else if ((x & NO_CONFLICT) && 
           (x & QUALITY_STRAND) && 
           (x & NON_QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_2;
  }
  else if ((x & NO_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & ANY_SAME_STRAND))
  {
    return QUALITY_CLASS_3;
  }
  else if ((x & NO_CONFLICT) &&
           (x & NON_QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_4;
  }
  else if ((x & NO_CONFLICT) &&
           (x & NON_QUALITY_SAME_STRAND))
  {
    return QUALITY_CLASS_5;
  }
  else if ((x & NON_QUALITY_CONFLICT) &&
           (x & QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_6;
  }
  else if ((x & NON_QUALITY_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & NON_QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_7;
  }
  else if ((x & NON_QUALITY_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & ANY_SAME_STRAND))
  {
    return QUALITY_CLASS_8;
  }
  else if ((x & NO_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & SINGLE_COVERAGE))
  {
    return QUALITY_CLASS_9;
  }
  else if ((x & NON_QUALITY_CONFLICT) &&
           (x & NON_QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_10;
  }
  else if ((x & NO_CONFLICT) &&
           (x & NON_QUALITY_STRAND) &&
           (x & SINGLE_COVERAGE))
  {
    return QUALITY_CLASS_11;
  }
  else if ((x & NON_QUALITY_CONFLICT) &&
           (x & NON_QUALITY_SAME_STRAND))
  {
    return QUALITY_CLASS_12;
  }
  else if ((x & NON_QUALITY_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & SINGLE_COVERAGE))
  {
    return QUALITY_CLASS_13;
  }
  else if ((x & NON_QUALITY_CONFLICT) &&
           (x & NON_QUALITY_STRAND) &&
           (x & SINGLE_COVERAGE))
  {
    return QUALITY_CLASS_14;
  }
  else if ((x & QUALITY_CONFLICT) &&
           (x & QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_15;
  }
  else if ((x & QUALITY_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & NON_QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_16;
  }
  else if ((x & QUALITY_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & ANY_SAME_STRAND))
  {
    return QUALITY_CLASS_17;
  }
  else if ((x & QUALITY_CONFLICT) &&
           (x & NON_QUALITY_OTHER_STRAND))
  {
    return QUALITY_CLASS_18;
  }
  else if ((x & QUALITY_CONFLICT) &&
           (x & NON_QUALITY_SAME_STRAND))
  {
    return QUALITY_CLASS_19;
  }
  else if ((x & QUALITY_CONFLICT) &&
           (x & QUALITY_STRAND) &&
           (x & SINGLE_COVERAGE))
  {
    return QUALITY_CLASS_20;
  }
  else if ((x & AMBIGUITY) &&
           (x & SINGLE_COVERAGE))
  {
    return QUALITY_CLASS_21;
  }
  else if (x & AMBIGUITY)
  {
    return QUALITY_CLASS_22;
  }
  else if ((x & QUALITY_CONFLICT) &&
           (x & NON_QUALITY_STRAND) &&
           (x & SINGLE_COVERAGE))
  {
    return QUALITY_CLASS_23;
  }
  else if ((x & (QUALITY_STRAND | NON_QUALITY_STRAND)) == 0)
  {
    // No reads in agreement with the consensus
    return QUALITY_CLASS_23;
  }

  return QUALITY_CLASS_ERROR;
}


//! Calculates the quality class for a single slice
/*! Quality classes 1-23 are the same as the quality class definitions
 *  from TIGR Assembler. A return value of 0 indicates that an error has occured
 *  in calculating the quality class.
 *
 *  @param s Slice to evaluate
 *  @param highQualityThreshold Threshold for defining highQuality
 *  @return Quality class of slice
 */
char libSlice_getConsQC(const libSlice_Slice *s, int highQualityThreshold)
{
  // Initialize Counters index 0 = Forward strand, 1 = Reverse strand
  int HQStrand[2]              = {0,0};  
  int LQStrand[2]              = {0,0};
  int HQStrandDisagree[2]      = {0,0};
  int LQStrandDisagree[2]      = {0,0};

  // Counters
  int nonHomogCount = 0; // number of bases in conflict with consensus
  int homogCount    = 0; // number of bases in agreement

  char existing_consensus  = s->c;
  int class = 0;

  // Initialize Flags
  int gap_consensus            = 0;
  int single_coverage          = 0;
  int no_conflict              = 1; 

  int quality_strand           = 0; 
  int quality_same_strand      = 0; 
  int quality_other_strand     = 0; 
  int quality_conflict         = 0; 
  int non_quality_strand       = 0; 
  int non_quality_same_strand  = 0; 
  int non_quality_other_strand = 0; 
  int non_quality_conflict     = 0; 
  int ambiguity                = 0;
  int depthzero                = 0;
  int i;

  if (highQualityThreshold == 0)
  {
    highQualityThreshold = HIGH_QUALITY_THRESHOLD;
  }

  depthzero = (s->dcov == 0);
  
  // Loop through each base call to compute counts of high/low quality
  for(i = 0; i < s->dcov; i++)
  {
    int rc        = s->rc[i];
    int qual      = s->qv[i];          // base call quality value
    char basecall = toupper(s->bc[i]); // base call value

    if (basecall == existing_consensus)
    {
      homogCount++;
      if (qual >= highQualityThreshold)
      { 
        HQStrand[rc]++;
      }
      else
      {
        LQStrand[rc]++;
      }
    }
    else
    {
      nonHomogCount++;
      no_conflict = 0;

      if (qual >= highQualityThreshold)
      { 
        HQStrandDisagree[rc]++;
      }
      else
      {
        LQStrandDisagree[rc]++;
      }
    }
  }

  if (homogCount == 1)
  {
    single_coverage = 1;
  }

  // Compute additional properties for quality classes.
  if (existing_consensus == 'A' || existing_consensus == 'T' || 
      existing_consensus == 'G' || existing_consensus == 'C' ||
      existing_consensus == '-')
  {
    gap_consensus = (existing_consensus == '-');

    quality_strand       = HQStrand[FORWARD] || HQStrand[REVERSE];
    quality_other_strand = HQStrand[FORWARD] && HQStrand[REVERSE];

    quality_same_strand  = HQStrand[FORWARD] > 1 || HQStrand[REVERSE] > 1;

    quality_conflict     = HQStrandDisagree[FORWARD]||HQStrandDisagree[REVERSE];

    non_quality_conflict = LQStrandDisagree[FORWARD]||LQStrandDisagree[REVERSE];

    if (quality_strand)
    {
      int strand               = HQStrand[FORWARD] < HQStrand[REVERSE];
      int other_strand         = !strand;

      non_quality_strand       = LQStrand[strand] > 0;
      non_quality_same_strand  = LQStrand[strand] > 0;
      non_quality_other_strand = LQStrand[other_strand] > 0;
    }
    else
    {
      non_quality_strand       = LQStrand[FORWARD] || LQStrand[REVERSE];
      non_quality_other_strand = LQStrand[FORWARD] && LQStrand[REVERSE];

      non_quality_same_strand  = LQStrand[FORWARD] > 1 || LQStrand[REVERSE] > 1;
    }
  }
  else
  {
    ambiguity = 1;
  }

  if (quality_conflict)
  {
    // This ensures that the two are mutually exclusive, with preference
    // towards quality_conflict.
    non_quality_conflict = 0;
  }


  // Compute the quality class
  class += (no_conflict              * NO_CONFLICT);
  class += (quality_strand           * QUALITY_STRAND);
  class += (quality_same_strand      * QUALITY_SAME_STRAND);
  class += (quality_other_strand     * QUALITY_OTHER_STRAND);
  class += (quality_conflict         * QUALITY_CONFLICT);
  class += (non_quality_strand       * NON_QUALITY_STRAND);
  class += (non_quality_same_strand  * NON_QUALITY_SAME_STRAND);
  class += (non_quality_other_strand * NON_QUALITY_OTHER_STRAND);
  class += (non_quality_conflict     * NON_QUALITY_CONFLICT);
  class += (single_coverage          * SINGLE_COVERAGE);
  class += (ambiguity                * AMBIGUITY);
  class += (gap_consensus            * GAP_CONSENSUS);
  class += (depthzero                * DEPTHZERO);
  
  return QUALITY_CLASS(class);
}


//! Calculates the quality classes for a range of slices
/*! 
 *  @param s Array of slices to evaluate quality classes
 *  @param len Length of array
 *  @param highQualityThreshold Threshold to use for highQuality
 *  @return Pointer to Null terminated list of quality classes 
 *          (one for each slice)
 *
 *  @see getConsQC
 */
char * libSlice_getConsQCRange(const libSlice_Slice s[], 
                               int len, 
                               int highQualityThreshold)
{
  int i;
  char *cqc = libSlice_newmem(len+1,sizeof(char));
  cqc[len] = '\0';

  for(i = 0; i < len; i++)
  {
    cqc[i] = libSlice_getConsQC(&s[i], highQualityThreshold);
  }

  return cqc;
}
