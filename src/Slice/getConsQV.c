/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$

/*! @file
 *  @brief Implementation of functions which calculate consensus and consensus
 *         quality values.
 */

#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Slice.h"

//! Quality assigned to a gap when it it input as having 0 quality
#define GAP_QUALITY_VALUE 20

//! Quality value to assign to gap in an empty slice
#define GAP_QUALITY_VALUE_EMPTY_SLICE 1

//! The default distribution of bases for distributing qv probabilities
libSlice_BaseDistribution standardDistribution = {.20, .20, .20, .20, .20};

#define CONIC_SIMPLE_SCORING_COUNT 10
#define CONIC_SIMPLE_SCORING -2
#define CONIC_ALL_AMBIGUITY -1
#define CONIC_NO_ADJUSTMENT 0

#define MAX_SCORING_COUNT 5



/*   
 *   scoringMatrix[0]-LQ
 *   0...secondQV
 *   .
 *   .
 *   .
 *   maxQV
 *
 *   scoringMatrix[0]-HQ
 *   0...secondQV
 *   .
 *   .
 *   .
 *   maxQV
 *
 *   scoringMatrix[1]-LQ
 *   0...secondQV
 *   .
 *   .
 *   .
 *   maxQV
 *
 *   .
 *   .
 *   .
 */

int scoringMatrix[MAX_SCORING_MODEL][2][MAX_SCORING_COUNT][MAX_SCORING_COUNT] = 
{
  {
    // 0-LQ 
    {
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
    },

    // 0-HQ
    {
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
      {0,  0,  0,  0,  0},
    },
  },

  {
    // 1-LQ
    {
      {0,  0,  0,  0,  0},
      {0, -1, -1, -1, -1},
      {0, -1, -1, -1, -1},
      {0, -1, -1, -1, -1},
      {0, -1, -1, -1, -1},
    },

    // 1-HQ
    {
      {0,  0,  0,  0,  0},
      {0, -1, -1, -1, -1},
      {0, -1, -1, -1, -1},
      {0, -1, -1, -1, -1},
      {0, -1, -1, -1, -1},
    },
  },


  {
    // 2-LQ
    {
      {0,  0,  0,  0,  0},
      {0, -2, -2, -2, -2},
      {0, -2, -2, -2, -2},
      {0, -2, -2, -2, -2},
      {0, -2, -2, -2, -2},
    },

    // 2-HQ
    {
      {0,  0,  0,  0,  0},
      {0, -2, -2, -2, -2},
      {0, -2, -2, -2, -2},
      {0, -2, -2, -2, -2},
      {0, -2, -2, -2, -2},
    },
  },

  {
    // 3-LQ
    {
      {0,  0,  0,  0,  0},
      {0, 16, 12,  8,  4},
      {0, 15, 11,  7,  3},
      {0, 14, 10,  6,  2},
      {0, 13,  9,  5,  1},
    },

    // 3-HQ
    {
      {0,  0,  0,  0,  0},
      {0, 16, 12,  8,  4},
      {0, 15, 11,  7,  3},
      {0, 14, 10,  6,  2},
      {0, 13,  9,  5,  1},
    },
  },
};



/*! @internal 
 *  Convert probabilities into the equivalent quality value.
 *
 *  Note: If pr is exactly 0 or <0, returns MAX_QUALITY_VALUE.
 */
inline int prToQV(long double pr)
{
  if (pr == 0.0L || pr < 0)
  {
    //fprintf (stderr, "floating point underflow in prToQV\n");
    return MAX_QUALITY_VALUE;
  }

  return floor(0.5 - 10.0 * log10(pr));
}

/*! @internal
 *  Assigns a base a positive probability and divides the remaining probabilty.
 *  The remaining probability is distributed according to their frequency.
 */
inline void distributeQV(int qv, 
                         long double * base,
                         long double * error1, double frequency1,
                         long double * error2, double frequency2,
                         long double * error3, double frequency3,
                         long double * error4, double frequency4)
{
  if (qv)
  {
    long double ep = pow(0.1, (double) qv/10.0);
    long double sum = frequency1 + frequency2 + frequency3 + frequency4;

    // error probability 1-Pr(x_ij=A | s_i=A)
    *base = 1-ep;

    // Divide the remaining probability among the other possibilities
    // based on their global frequency

    *error1 = ep * frequency1/sum;
    *error2 = ep * frequency2/sum;
    *error3 = ep * frequency3/sum;
    *error4 = ep * frequency4/sum;
  }
}

/*! @name Ambiguity Code Calculations
 *  Functions for calculating ambiguity codes
 */

//@{

//! Calculates ambiguity flags from Churchill and Waterman procedure
/*! The procedure has been modified to take as a parameter the maximum number
 *  of bases that can be represented by the ambiguity code. This way if only
 *  A's and C's were seen in the read a baseCount of 2 can be pased and
 *  the ambiguity code will only be one of A,C, or (A || C) = M even if the 
 *  QV of M does not reach the quality threshold. Set this value to 5
 *  to be compatible with the Churchill and Waterman procedure.
 *
 *  Returns a character of which the 5 lowest bits are used to flag the
 *  inclusion of A,C,G,T,-.
 * 
 *  @param cpA Error probability of 'A'
 *  @param cpC Error probability of 'C'
 *  @param cpG Error probability of 'G'
 *  @param cpT Error probability of 'T'
 *  @param cpGap Error probability of '-'
 *  @param highQualityThreshold QV Value considered high quality
 *  @param baseCount Maximum number of bases represented by ambiguity code
 *  
 *  @retval Ambiguity flags packed into a single character
 */
char libSlice_calculateAmbiguityFlags(long double cpA,
                                      long double cpC,
                                      long double cpG,
                                      long double cpT,
                                      long double cpGap,
                                      int highQualityThreshold,
                                      int baseCount)
{
  int codeSize = 0;
  char ambiguityFlags = 0;
  long double probSum = 0;
  long double baseProb [5] = {cpA, cpC, cpG, cpT, cpGap};

  long double cpAmbiguity;

  if (highQualityThreshold == 0)
  {
    highQualityThreshold = HIGH_QUALITY_THRESHOLD;
  }

  if (baseCount < 1 || baseCount > 5) baseCount = 5;

  /*
  fprintf(stdout,
          "cpA=%Le, cpC=%Le, cpG=%Le, cpT=%Le, cpGap=%Le, hq=%d\n",
          cpA, cpC, cpG, cpT, cpGap, highQualityThreshold);
  */

  // Find the minimum set of bases such that the sum of the probabilities
  // exceeds the threshold and the size of the code is at most baseCount bases
  do
  {
    int i;
    int min = 0; // Assume position 0 is best

    // Find the base with the lowest error probability
    for (i = 1; i < 5; i++)
    {
      if (baseProb[i] < baseProb[min]) min = i;
    }

    // Add this base to the running sum of probability of success
    probSum += (1.0 - baseProb[min]);
    baseProb[min] = 2.0; // Guaranteed not to be included in next pass
    codeSize++;

    switch (min)
    {
      case 0: ambiguityFlags |= AMBIGUITY_FLAGBIT_A;   break;
      case 1: ambiguityFlags |= AMBIGUITY_FLAGBIT_C;   break;
      case 2: ambiguityFlags |= AMBIGUITY_FLAGBIT_G;   break;
      case 3: ambiguityFlags |= AMBIGUITY_FLAGBIT_T;   break;
      case 4: ambiguityFlags |= AMBIGUITY_FLAGBIT_GAP; break;
    };

    if (probSum == 1.0)
    {
      // A QV as low as 200 can be problematic because 1-baseProb[min] converts
      // a probability close to 0 to one close to 1, and it takes more bits to
      // represent a number close to 1.
      //
      // Fortunately, at that level, the consensus should not be ambigious
      
      //fprintf(stdout, "floating point underflow detected in calcAmbFlags\n");
      break;
    }

    cpAmbiguity = 1.0-probSum;
  } 
  while ((prToQV(cpAmbiguity) < highQualityThreshold) && 
         (codeSize < baseCount));

  return ambiguityFlags;
}

//! Calculates extended ambiguity code bases from ambiguity flags
/*! Use the five ambiguity flags to find an ambiguity code. Extended
 *  ambiguity codes are encoded in lowercase, ie. "A*" = 'a'.
 *
 *  @param ambiguityFlags Output from calculateAmbiguityFlags
 *  @retval Extended Ambiguity Code
 */
char libSlice_convertAmbiguityFlags(char ambiguityFlags)
{
  int addA   = (ambiguityFlags & AMBIGUITY_FLAGBIT_A);
  int addC   = (ambiguityFlags & AMBIGUITY_FLAGBIT_C);
  int addG   = (ambiguityFlags & AMBIGUITY_FLAGBIT_G);
  int addT   = (ambiguityFlags & AMBIGUITY_FLAGBIT_T);
  int addGap = (ambiguityFlags & AMBIGUITY_FLAGBIT_GAP);
  
  char consensus = '?';  

  // Pick IUPAC code 
 
  if      (addA && addC && addG && addT) consensus = 'N';
  else if (addA && addC && addG)         consensus = 'V';
  else if (addA && addC && addT)         consensus = 'H';
  else if (addA && addG && addT)         consensus = 'D';
  else if (addC && addG && addT)         consensus = 'B';
  else if (addA && addT)                 consensus = 'W';
  else if (addA && addC)                 consensus = 'M';
  else if (addA && addG)                 consensus = 'R';
  else if (addC && addG)                 consensus = 'S';
  else if (addC && addT)                 consensus = 'Y';
  else if (addG && addT)                 consensus = 'K';
  else if (addA)                         consensus = 'A';
  else if (addC)                         consensus = 'C';
  else if (addG)                         consensus = 'G';
  else if (addT)                         consensus = 'T';
  else if (addGap)                       consensus = '-';

  // Encode extended ambiguity codes as well
  if ((consensus != '-') && addGap)
  {
    consensus = tolower(consensus);
  }

  return consensus;
}


//! Return IUPAC Ambiguity Code in one operation
/*! Uses calculateAmbiguityFlags and convertAmbiguityFlags to generate
 *  an IUPAC ambiguity code from 5 error probabilities.
 *
 *  @param cpA Error probability of 'A'
 *  @param cpC Error probability of 'C'
 *  @param cpG Error probability of 'G'
 *  @param cpT Error probability of 'T'
 *  @param cpGap Error probability of '-'
 *  @param highQualityThreshold QV Value considered high quality
 *  @param baseCount Maximum number of bases represented by ambiguity code
 *  
 *  @retval IUPAC Ambiguity Code
 *
 *  @see calculateAmbiguityFlags
 *  @see convertAmbiguityFlags
 */
char libSlice_getAmbiguityCode(long double cpA,
                               long double cpC,
                               long double cpG,
                               long double cpT,
                               long double cpGap,
                               int highQualityThreshold,
                               int baseCount)
{
  char ambiguityFlags = libSlice_calculateAmbiguityFlags(cpA, cpC, cpG, cpT, 
                                                         cpGap, 
                                                         highQualityThreshold, 
                                                         baseCount);
  return libSlice_convertAmbiguityFlags(ambiguityFlags);
}

//@}

/*! @name Slice Consensus Calculations 
 *  Functions for calculating the consensus and qv of a single slice
 */
//@{

// Not thread safe, but protects the API
int m_recallEmpty = 0;

//! Toggles if empty slices should be recalled as gap
/*! If this is set to true, slices will be recalled as gap. Otherwise,
 *  empty slices will be left alone. The probabilities and cqv will be
 *  incorrect in either case, but it's pretty meaningless on an empty slice.
 */
void libSlice_setRecallEmpty(int recallEmpty)
{
  m_recallEmpty = recallEmpty;
}

//! Calculates the consensus quality value for a single slice
/*! Calculates the consensus and consensus quality values for a slice using
 *  Bayes formula and the procedure from Churchill, G.A. and Waterman, M.S. 
 *  "The accuracy of DNA sequences: Estimating sequence quality." 
 *  Genomics 14, pp. 89-98 (1992). 
 *
 *  Note: If dist is NULL, assumes a normal distribution of {.2,.2,.2,.2,.2} for
 *        {A,C,G,T,-}.
 *
 *  Note: It is the callers responsibility to free the result.
 *
 *  @param s Pointer to a slice.
 *  @param result Cargo pointer for storing results.
 *  @param dist Table of distribution of the bases.
 *  @param highQualityThreshold Threshold for ambiguity codes.
 *  @param doAmbiguity Flag to calculte ambiguity codes.
 *  @return errorCode 0 on sucess.
 */
int libSlice_getConsensusParam(const libSlice_Slice *s, 
                               libSlice_Consensus ** result, 
                               const libSlice_BaseDistribution * dist,
                               int highQualityThreshold,
                               int doAmbiguity) 
{
  int retval = 0;
    
  // quality value multiplicities
  unsigned int qvMultA   = 0; 
  unsigned int qvMultC   = 0;
  unsigned int qvMultG   = 0;
  unsigned int qvMultT   = 0;
  unsigned int qvMultGap = 0;
  
  // consensus quality value parameters
  // current consensus base: s_i, the column of base calls above it: x_ij
  long double pr_A_A     = 1.0; // Pr(x_ij=A | s_i=A)
  long double pr_C_A     = 1.0; // Pr(x_ij=C | s_i=A)
  long double pr_G_A     = 1.0; // Pr(x_ij=G | s_i=A)
  long double pr_T_A     = 1.0; // Pr(x_ij=T | s_i=A)
  long double pr_GAP_A   = 1.0;
  
  long double pr_A_C     = 1.0;
  long double pr_C_C     = 1.0;
  long double pr_G_C     = 1.0;
  long double pr_T_C     = 1.0;
  long double pr_GAP_C   = 1.0;
  
  long double pr_A_G     = 1.0;
  long double pr_C_G     = 1.0;
  long double pr_G_G     = 1.0;
  long double pr_T_G     = 1.0;
  long double pr_GAP_G   = 1.0;

  long double pr_A_T     = 1.0;
  long double pr_C_T     = 1.0;
  long double pr_G_T     = 1.0;
  long double pr_T_T     = 1.0;
  long double pr_GAP_T   = 1.0;

  long double pr_A_GAP   = 1.0;
  long double pr_C_GAP   = 1.0;
  long double pr_G_GAP   = 1.0;
  long double pr_T_GAP   = 1.0;
  long double pr_GAP_GAP = 1.0;

  long double   pA,  pC,  pG,  pT,  pGap, sum;
  long double  cpA, cpC, cpG, cpT, cpGap, cpConsensus;
  unsigned int qvA, qvC, qvG, qvT, qvGap;

  char consensus;        // calculated consensus
  int baseCount = 0;     // number of bases seen in read
  unsigned int cqv = 0;  // calculated consensus quality value

  if (!result || !s)
  {
    // Result can't be NULL
    return -1;
  }

  *result = (libSlice_Consensus *) malloc (sizeof(libSlice_Consensus));

  if (!*result)
  {
    return -2;
  }

  if (highQualityThreshold < 0)
  {
    doAmbiguity = 0;
  }

  if (!s->dcov && !m_recallEmpty)
  {
    (*result)->qvA   = 0;
    (*result)->qvC   = 0;
    (*result)->qvG   = 0;
    (*result)->qvT   = 0;
    (*result)->qvGap = 0;

    (*result)->cpA   = 0.2;
    (*result)->cpC   = 0.2;
    (*result)->cpG   = 0.2;
    (*result)->cpT   = 0.2;
    (*result)->cpGap = 0.2;

    (*result)->qvConsensus = 0;
    (*result)->ambiguityFlags = 0;

    // Just set the consensensus to be the old consensus
    (*result)->consensus = s->c;
  }
  else
  {
    if (s->dcov)
    {
      // Loop through the bases and sum the qv for each component
      
      int j;
      for(j = 0; j < s->dcov; j++)
      {
        char a = toupper(s->bc[j]);
        switch(a)
        {
          case 'A': qvMultA += s->qv[j]; break;
          case 'C': qvMultC += s->qv[j]; break;
          case 'G': qvMultG += s->qv[j]; break;
          case 'T': qvMultT += s->qv[j]; break;

          case '-': 
              // If the gap has a qv use that, otherwise use the default
              qvMultGap += s->qv[j] ? s->qv[j] : GAP_QUALITY_VALUE;
              break;

          default:
              // fprintf(stderr, "Bad input=%c\n", a);
              // Ambiguity codes cannot be processed
	    break;
        };
      }
    }
    else
    {
      // An empty slice is a gap (by definition)

      qvMultGap = GAP_QUALITY_VALUE_EMPTY_SLICE;
    }

    if (qvMultA)   baseCount++;
    if (qvMultC)   baseCount++;
    if (qvMultG)   baseCount++;
    if (qvMultT)   baseCount++;
    if (qvMultGap) baseCount++;

    if (!dist) dist = &standardDistribution;

    distributeQV(qvMultA, 
                 &pr_A_A,
                 &pr_A_C,   dist->freqC, 
                 &pr_A_G,   dist->freqG,
                 &pr_A_T,   dist->freqT, 
                 &pr_A_GAP, dist->freqGap);

    distributeQV(qvMultC, 
                 &pr_C_C,
                 &pr_C_A,   dist->freqA, 
                 &pr_C_G,   dist->freqG,
                 &pr_C_T,   dist->freqT, 
                 &pr_C_GAP, dist->freqGap);

    distributeQV(qvMultG, 
                 &pr_G_G,
                 &pr_G_A,   dist->freqA, 
                 &pr_G_C,   dist->freqC,
                 &pr_G_T,   dist->freqT, 
                 &pr_G_GAP, dist->freqGap);

    distributeQV(qvMultT, 
                 &pr_T_T,
                 &pr_T_A,   dist->freqA, 
                 &pr_T_C,   dist->freqC,
                 &pr_T_G,   dist->freqG, 
                 &pr_T_GAP, dist->freqGap);

    distributeQV(qvMultGap, 
                 &pr_GAP_GAP,
                 &pr_GAP_A, dist->freqA, 
                 &pr_GAP_C, dist->freqC,
                 &pr_GAP_G, dist->freqG, 
                 &pr_GAP_T, dist->freqT);

    // Calculate raw values
    pA   = pr_A_A   * pr_C_A   * pr_G_A   * pr_T_A   * pr_GAP_A;
    pC   = pr_A_C   * pr_C_C   * pr_G_C   * pr_T_C   * pr_GAP_C;
    pG   = pr_A_G   * pr_C_G   * pr_G_G   * pr_T_G   * pr_GAP_G;
    pT   = pr_A_T   * pr_C_T   * pr_G_T   * pr_T_T   * pr_GAP_T;
    pGap = pr_A_GAP * pr_C_GAP * pr_G_GAP * pr_T_GAP * pr_GAP_GAP;

    sum = pA + pC + pG + pT + pGap;

    
    /*
    fprintf(stderr,
            "sum=%Lg, pA=%Lg, pC=%Lg, pG=%Lg, pT=%Lg, pGap=%Lg\n",
             sum,     pA,     pC,     pG,     pT,     pGap);
    */
    

    // Normalize values and computer error probabilities
    // Note: Error probability is the sum of the probability of the other bases
    
    if (sum != 0.0)
    {
      cpA   = (0.0 + pC  + pG  + pT  + pGap) / sum;
      cpC   = (pA  + 0.0 + pG  + pT  + pGap) / sum;
      cpG   = (pA  + pC  + 0.0 + pT  + pGap) / sum;
      cpT   = (pA  + pC  + pG  + 0.0 + pGap) / sum;
      cpGap = (pA  + pC  + pG  + pT  + 0.0)  / sum;
    }
    else
    {
      // Sum can go to zero by floating point underflow
      cpA = cpC = cpG = cpT = cpGap = .20;
    }

    // Note: cpA + cpC + cpG + cpG + cpT + cpGap == 1

    
    /*
    fprintf(stderr,
            "sum=%Lg, cpA=%Lg, cpC=%Lg, cpG=%Lg, cpL=%Lg, cpGap=%Lg\n",
             sum,     cpA,     cpC,     cpG,     cpT,     cpGap);
    */
    

    // Convert error probabilities into quality values
    qvA   = prToQV(cpA);
    qvC   = prToQV(cpC);
    qvG   = prToQV(cpG);
    qvT   = prToQV(cpT);
    qvGap = prToQV(cpGap);

    // The consensus quality value is the maximum of the quality 
    // values for each base. The base that has the lowest probability of
    // error is the unambiguous consensus of the slice.
    
    // Find the absolute winner
    consensus = 'A'; cqv = qvA; cpConsensus = cpA;

    // Allow a small margin to handle floating point weirdness
    if ((cpC - cpConsensus) < -0.000000001) 
    {
      consensus = 'C'; cqv = qvC; cpConsensus = cpC;
    }
    
    if ((cpG - cpConsensus) < -0.000000001) 
    {
      consensus = 'G'; cqv = qvG; cpConsensus = cpG;
    }
    
    if ((cpT - cpConsensus) < -0.000000001) 
    {
      consensus = 'T'; cqv = qvT; cpConsensus = cpT;
    }
    
    if ((cpGap-cpConsensus) < -0.000000001) 
    {
      consensus = '-'; cqv=qvGap; cpConsensus = cpGap;
    }

    /*
    fprintf(stderr, "cp: A %s -\n", (cpA==cpGap)?"==":"!=");
    fprintf(stderr, "qv: A %s -\n", (qvA==qvGap)?"==":"!=");

    fprintf(stderr, "%Lf\n", cpGap - cpA);
    */

    (*result)->qvA   = qvA;
    (*result)->qvC   = qvC;
    (*result)->qvG   = qvG;
    (*result)->qvT   = qvT;
    (*result)->qvGap = qvGap;

    (*result)->cpA   = cpA;
    (*result)->cpC   = cpC;
    (*result)->cpG   = cpG;
    (*result)->cpT   = cpT;
    (*result)->cpGap = cpGap;

    (*result)->qvConsensus = cqv;

    // This finds all ambiguity flags
    (*result)->ambiguityFlags = 
            libSlice_calculateAmbiguityFlags(cpA, cpC, cpG, cpT, cpGap,
                                             highQualityThreshold,
                                             baseCount);
    if (doAmbiguity)
    {
      consensus = libSlice_convertAmbiguityFlags((*result)->ambiguityFlags);
    }

    (*result)->consensus = consensus;
  }

  return retval;
}

//! Updates the ambiguity flags using the conic model and scoring matrix
int libSlice_updateAmbiguityConic(const libSlice_Slice * s, 
                                  libSlice_Consensus * consensus,
                                  int highQuality,
                                  int scoringModel)
{
  int retval = 0;

  if (highQuality == 0) { highQuality = HIGH_QUALITY_THRESHOLD; }

  if (!s || !consensus)
  {
    retval = -1;
  }
  else if (s->dcov == 0)
  {
    if (m_recallEmpty)
    {
      // Zero Coverage, assign gap
      consensus->ambiguityFlags |= AMBIGUITY_FLAGBIT_GAP;
    }
  }
  else
  {
    int i;
    char ambiguityFlags = consensus->ambiguityFlags;

    unsigned int qvArr[5]      = {0,0,0,0,0};
    unsigned int countArr[5]   = {0,0,0,0,0};
    unsigned int hqCountArr[5] = {0,0,0,0,0};

    unsigned int maxQV = 0;
    unsigned int secondQV = -1;

    unsigned int maxCount = 0;
    unsigned int secondCount = -1;

    int scoringThreshold = CONIC_SIMPLE_SCORING;
    int base;

    // Figure out base counts and sum quality values
    for (i = 0; i < s->dcov; i++)
    {
      char a = toupper(s->bc[i]);
      switch(a)
      {
        case 'A': base = 0; break;
        case 'C': base = 1; break;
        case 'G': base = 2; break;
        case 'T': base = 3; break;
        case '-': base = 4; break;

        default:
            // fprintf(stderr, "Bad input=%c\n", a);
            // Ambiguity codes cannot be processed
            base = -1;
      };

      if (base != -1)
      {
        // Use 20 for gap qv by default
        qvArr[base] += ((base==4) && s->qv[i]==0) ? 20 : s->qv[i];

        countArr[base]++;
        if (s->qv[i] >= highQuality) { hqCountArr[base]++; }
      }
    }

    // Find the two bases with the highest qv and top two highest counts
    // Start at 1 by assuming 0 is max
    for (i = 1; i < 5; i++)
    {
      if (qvArr[i] > qvArr[maxQV])
      {
        secondQV = maxQV;
        maxQV = i;
      }
      else if (secondQV == -1 || qvArr[i] > qvArr[secondQV])
      {
        secondQV = i;
      }

      if (countArr[i] > countArr[maxQV])
      {
        secondCount = maxCount;
        maxCount = i;
      }
      else if (secondCount == -1 || countArr[i] > countArr[secondQV])
      {
        secondCount = i;
      }
    }

    // If the counts are within the scoring matrix, look up the threshold
    if ((scoringModel       < MAX_SCORING_MODEL) &&
        (countArr[maxQV]    < MAX_SCORING_COUNT) &&
        (countArr[secondQV] < MAX_SCORING_COUNT))
    {
      scoringThreshold = scoringMatrix[scoringModel][0]
                                      [countArr[maxQV]]
                                      [countArr[secondQV]];
    }

    if (scoringThreshold == CONIC_SIMPLE_SCORING)
    {
      double ambiguityAngle = 36.8698977;
      double effectiveAngle = ((ambiguityAngle / 2) <= 45) 
                              ? ((double)ambiguityAngle)/2 : 45;

      double lowerRadians = (M_PI/(double) 180) * (45.0 - effectiveAngle);
      double upperRadians = (M_PI/(double) 180) * (45.0 + effectiveAngle);

      double lowerlimit = tan(lowerRadians);
      double upperlimit = tan(upperRadians);

      int component = 0;

      ambiguityFlags = 0;

      for (component = 0; component < 5; component++)
      {
        // Initialize this to be effectively infinite in case qvArr[maxQV] == 0
        // but set to be zero if qvArr[component] == 0
        double tangent = (qvArr[maxQV]) ? 
                         ((double) qvArr[component]) / qvArr[maxQV] : 
                         qvArr[component] * 10000000.0;
        
        // Everything is compared to the max, except itself
        if (component == maxQV) { continue; }

        /*
        fprintf(stderr, "CSS: ll = %0.5f, uu=%0.5f, tan=%0.5f\n",
                lowerlimit, upperlimit, tangent);

        */

        if (tangent < lowerlimit)
        {
          // Consensus in non-ambiguous for maxQV
          // Normal case

          ambiguityFlags |= (1 << maxQV);
        }
        else if (tangent < upperlimit)
        {
          // Consensus is ambiguous between maxQV and component
          ambiguityFlags |= (1 << maxQV);
          ambiguityFlags |= (1 << component);
        }
        else
        {
          // Consensus is non-ambiguous for component
          // This should never be the case
          ambiguityFlags |= (1 << component);
        }
      }
    }
    else if (scoringThreshold == CONIC_ALL_AMBIGUITY)
    {
      // Add as ambiguity if there is any qv at all
      if (qvArr[maxQV])    { ambiguityFlags |= (1 << maxQV);    }
      if (qvArr[secondQV]) { ambiguityFlags |= (1 << secondQV); }
      //fprintf(stderr, "CAA: All Ambiguity\n");
    }
    else if (scoringThreshold == CONIC_NO_ADJUSTMENT)
    {
      // Make no adjustment to the ambiguity code
      /*
      fprintf(stderr, "CNA: No adjustment c(max)=%d, c(second)=%d\n",
              countArr[maxQV], countArr[secondQV]);
      */
    }
    else
    {
      // Check if qvArr[secondQV] is within scoringThreshold order's
      // of magnitute from qvArr[maxQV];

      unsigned int difference = qvArr[maxQV] - qvArr[secondQV];
      unsigned int ordersOfMagnitute = difference / 10;

      /*
      fprintf(stderr, "COOM: c=%d, q=%d, c=%d, q=%d oom=%d, threshold=%d\n", 
              countArr[maxQV],    qvArr[maxQV],
              countArr[secondQV], qvArr[secondQV],
              ordersOfMagnitute,  scoringThreshold);
      */

      if (abs(ordersOfMagnitute) <= scoringThreshold)
      {
        // It is ambiguous
        ambiguityFlags |= (1 << maxQV);
        ambiguityFlags |= (1 << secondQV);
      }
      else if (ordersOfMagnitute > 0)
      {
        // maxqv is non-ambiguous winner
        ambiguityFlags |= (1 << maxQV);
      }
      else
      {
        // secondqv is non-ambiguous winner
        ambiguityFlags |= (1 << secondQV);
      }
    }

    // Set the new ambiguity flags
    consensus->ambiguityFlags = ambiguityFlags;
  }

  return retval;
}


//! Updates the ambiguity flags to include all high quality reads
/*! This is an addition step to support the Annotation UC. Annotation
 *  does not utilize the tiling, and only has the consensus available. By
 *  adding all high quality conflicts, the resultant consensus will represent
 *  all like bases for the slice.
 *
 *  @param s input slice
 *  @param consensus libSlice_Consensus to update (only ambiguityFlags)
 *  @param highQuality Level to consider as high quality
 *  @retval 0 on sucess, consensus->ambiguityFlags updated.
 */
int libSlice_updateAmbiguity(const libSlice_Slice * s, 
                             libSlice_Consensus * consensus,
                             int highQuality)
{
  int retval = 0;

  if (!s || !consensus)
  {
    retval = -1;
  }
  else
  {
    char ambiguityFlags = 0;

    // Get the original flags
    int addA   = ((consensus->ambiguityFlags & AMBIGUITY_FLAGBIT_A) != 0);
    int addC   = ((consensus->ambiguityFlags & AMBIGUITY_FLAGBIT_C) != 0);
    int addG   = ((consensus->ambiguityFlags & AMBIGUITY_FLAGBIT_G) != 0);
    int addT   = ((consensus->ambiguityFlags & AMBIGUITY_FLAGBIT_T) != 0);
    int addGap = ((consensus->ambiguityFlags & AMBIGUITY_FLAGBIT_GAP) != 0);
    
    int j;

    if (highQuality == 0)
    {
      highQuality = HIGH_QUALITY_THRESHOLD;
    }
    
    // Add high quality conflicts
    for(j = 0; j < s->dcov; j++)
    {
      char qv = (s->bc[j] == '-' && s->qv[j] == 0) ? 
                GAP_QUALITY_VALUE : s->qv[j];

      if (qv >= highQuality)
      {
        char a = toupper(s->bc[j]);
        switch(a)
        {
          case 'A': addA = 1;   break;
          case 'C': addC = 1;   break;
          case 'G': addG = 1;   break;
          case 'T': addT = 1;   break;
          case '-': addGap = 1; break;

          default:
              // fprintf(stderr, "Bad input=%c\n", a);
              // Ambiguity codes cannot be processed
	    break;
        };
      }
    }

    if (addA)   ambiguityFlags += AMBIGUITY_FLAGBIT_A;
    if (addC)   ambiguityFlags += AMBIGUITY_FLAGBIT_C;
    if (addG)   ambiguityFlags += AMBIGUITY_FLAGBIT_G;
    if (addT)   ambiguityFlags += AMBIGUITY_FLAGBIT_T;
    if (addGap) ambiguityFlags += AMBIGUITY_FLAGBIT_GAP;

    // Set the new ambiguity flags
    consensus->ambiguityFlags = ambiguityFlags;
  }

  return retval;
}

//! Calculates the consensus of a slice with NO ambiguity codes
/*! 
 *  @param s Pointer to a slice.
 *  @param result Cargo pointer for storing results.
 *  @param dist Table of distribution of the bases.
 *  @param highQualityThreshold Threshold for ambiguity codes.
 *  @return errorCode 0 on sucess.
 *
 *  @see _getConsensus
 */
int libSlice_getConsensus(const libSlice_Slice *s, 
                          libSlice_Consensus ** result, 
                          const libSlice_BaseDistribution * dist,
                          int highQualityThreshold)
{
  return libSlice_getConsensusParam(s, result, dist, highQualityThreshold, 0);
}

//! Calculates the consensus of a slice with ambiguity codes
/*! 
 *  @param s Pointer to a slice.
 *  @param result Cargo pointer for storing results.
 *  @param dist Table of distribution of the bases.
 *  @param highQualityThreshold Threshold for ambiguity codes.
 *  @return errorCode 0 on sucess.
 *
 *  @see _getConsensus
 */
int libSlice_getConsensusWithAmbiguity(const libSlice_Slice *s, 
                                       libSlice_Consensus ** result, 
                                       const libSlice_BaseDistribution * dist,
                                       int highQualityThreshold)
{
  return libSlice_getConsensusParam(s, result, dist, highQualityThreshold, 1);
}

//@}


/*! @name SliceRange Calculations 
 *  Functions for calculating the consensus and qv of a range of slices
 */
//@{

//! Calculates the Consensus Quality Values for a range of slices
/*! 
 *  @param s Array of slices
 *  @param results Cargo pointer to array of results
 *  @param len Length of slices (and results)
 *  @param dist Table of distribution of the bases
 *  @param highQualityThreshold Threshold for ambiguity codes.
 *  @param doAmbiguity Flag to calculate ambiguity codes
 *  @return errorCode, 0 on sucess
 *
 *  @see _getConsensus
 */
int libSlice_getConsensusRangeParam(const libSlice_Slice s [], 
                                    libSlice_Consensus *** results, 
                                    int len,
                                    const libSlice_BaseDistribution * dist,
                                    int highQualityThreshold,
                                    int doAmbiguity)
{
  int retval = 0;
  int i;

  // Create an array of pointers to store results
  *results = (libSlice_Consensus **) 
                 malloc (len * sizeof(libSlice_Consensus *));
  
  // It is the responsibility of getConsQC to allocate each individual result
  // Initialize each pointer to NULL as a check for consistency.
  memset(*results, 0, len * sizeof(libSlice_Consensus *));

  for(i = 0; i < len; i++)
  {
    retval += libSlice_getConsensusParam(&s[i], &((*results)[i]), dist,
                                         highQualityThreshold, doAmbiguity);
  }
  
  return retval;
}

//! Calculates the consensus of an array of slice with no ambiguity codes
/*! 
 *  @param s Array of slices
 *  @param results Cargo pointer to array of results
 *  @param len Length of slices (and results)
 *  @param dist Table of distribution of the bases
 *  @param highQualityThreshold Threshold for ambiguity codes.
 *  @return errorCode, 0 on sucess
 *
 *  @see _getConsensusRange
 */
int libSlice_getConsensusRange(const libSlice_Slice s [], 
                               libSlice_Consensus *** results, 
                               int len,
                               const libSlice_BaseDistribution * dist,
                               int highQualityThreshold)
{
  return libSlice_getConsensusRangeParam(s, results, len, dist, 
                                         highQualityThreshold, 0);
}


//! Calculates the consensus of an array of slice with ambiguity codes
/*! 
 *  @param s Array of slices
 *  @param results Cargo pointer to array of results
 *  @param len Length of slices (and results)
 *  @param dist Table of distribution of the bases
 *  @param highQualityThreshold Threshold for ambiguity codes.
 *  @return errorCode, 0 on sucess
 *
 *  @see _getConsensusRange
 */
int libSlice_getConsensusRangeWithAmbiguity(const libSlice_Slice s [], 
                                         libSlice_Consensus *** results, 
                                         int len,
                                         const libSlice_BaseDistribution * dist,
                                         int highQualityThreshold)
{
  return libSlice_getConsensusRangeParam(s, results, len, dist, 
                                         highQualityThreshold, 1);
}

//! Frees the results of getConsensusRange
/*! 
 *  @param results Array of pointers of results to free
 *  @param len Length of the array
 */
void libSlice_freeConsensusRange(libSlice_Consensus **results, int len)
{
  if (results && len)
  {
    int i;
    for (i = 0; i < len; i++)
    {
      if (results[i]) free(results[i]);
    }

    free(results);
  }
}

//@}

