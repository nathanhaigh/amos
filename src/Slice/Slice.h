/*
 * Copyright (c) 2003, The Institute for Genomic Research (TIGR).  All
 * rights reserved.
 */

// $Id$ 

/*! @file
 *  @brief Defines interface for libSlice.
 *
 *  Library of functions for determining quality values and classes on slices.
 *  Include this file to use all publicly available methods and data 
 *  structures.
 */

#ifndef SLICE_H
#define SLICE_H 1

/*! @defgroup cinterface C Interface
 *  The C interface contains the algorithms for the calculations on raw slices.
 *
 *  @{
 */

//! What is considered high quality by ABI
/*! Default threshold used when highQualityThreshold is 0.
 */
#define HIGH_QUALITY_THRESHOLD         30   

//! Quality Value to return when floating point underflows occur.
/*! Effectively, the maximum quality value that can ever occur. Quality Values
 *  upto ~3000 can occur, so this has been set at 4000 to indicate that it is
 *  higher than all the rest, but there is no way calculate the value exactly
 *  without higher precision floats.
 */
#define MAX_QUALITY_VALUE      4000

#ifdef __cplusplus
extern "C"
{
#endif

//! Structure for specifing slice information
struct libSlice_Slice
{
  //! Null terminated list of bases in {'A', 'C', 'T', 'G', ...}
  char * bc;  

  //! Null terminated list of quality values in [0..99] 
  char * qv;  

  //! Null terminated list of reverse complements in {0,1} 
  char * rc;  

  //! Existing consensus
  char c;     

  //! Depth of coverage
  /*! Note: dcov == strlen(bc) == strlen (qv) == strlen(rc) */
  unsigned short dcov; 
};
typedef struct libSlice_Slice libSlice_Slice;

//! Bit flag for 'A' in ambiguity code
#define AMBIGUITY_FLAGBIT_A   0x1

//! Bit flag for 'C' in ambiguity code
#define AMBIGUITY_FLAGBIT_C   0x2

//! Bit flag for 'G' in ambiguity code
#define AMBIGUITY_FLAGBIT_G   0x4

//! Bit flag for 'T' in ambiguity code
#define AMBIGUITY_FLAGBIT_T   0x8

//! Bit flag for '-' in ambiguity code
#define AMBIGUITY_FLAGBIT_GAP 0x10

//! Holds the quality value of the onsensus and each of the 5 components
struct libSlice_Consensus
{
  //! Quality Value that 'A' is the consensus
  unsigned int qvA;
  //! Quality Value that 'C' is the consensus
  unsigned int qvC;
  //! Quality Value that 'G' is the consensus
  unsigned int qvG;
  //! Quality Value that 'T' is the consensus
  unsigned int qvT;
  //! Quality Value that '-' (gap) is the consensus
  unsigned int qvGap;

  //! Quality Value of the consensus
  unsigned int qvConsensus;
  //! Consensus value
  char consensus;

  //! Flags indicating which of the 5 bases should be included
  char ambiguityFlags;

  long double cpA, cpC, cpG, cpT, cpGap;
};

typedef struct libSlice_Consensus libSlice_Consensus;


//! Holds a table of the distribution of bases
struct libSlice_BaseDistribution
{
  //! Global frequency of 'A' as consensus
  double freqA;
  //! Global frequency of 'C' as consensus
  double freqC;
  //! Global frequency of 'G' as consensus
  double freqG;
  //! Global frequency of 'T' as consensus
  double freqT;
  //! Global frequency of '-' as consensus
  double freqGap;
};
typedef struct libSlice_BaseDistribution libSlice_BaseDistribution;

#define MAX_SCORING_MODEL 4


/*! @name Slice Consensus Calculations 
 *  Functions for calculating the consensus and qv of a single slice
 */
//@{

// Set if empty slices should be recalled (to gap) or not
void libSlice_setRecallEmpty(int recallEmpty);

// Calculate consensus base and quality values faster
int libSlice_ConsensusLight (const unsigned qvSum [5],
                             int nof_ambiguities,                           
                             char * consensus,
                             unsigned * cns_qv);

// Internal function: Calculates the consensus and consensus quality 
//                    values for a single slice
int libSlice_getConsensusParam(const libSlice_Slice * s, 
                               libSlice_Consensus * result, 
                               const libSlice_BaseDistribution * dist,
                               int highQualityThreshold,
                               int doAmbiguity); 

// Updates the ambiguity flags using the conic model and scoring matrix
int libSlice_updateAmbiguityConic(const libSlice_Slice * s, 
                                  libSlice_Consensus * consensus,
                                  int highQaulity,
                                  int scoringModel);


// Update the ambiguity flags with all high quality reads
int libSlice_updateAmbiguity(const libSlice_Slice * s,
                             libSlice_Consensus * consensus,
                             int highQualityThreshold);

// Wrapper to calculate the consensus and consensus quality values 
// for a range of slices without using ambiguity codes
int libSlice_getConsensus(const libSlice_Slice * s, 
                          libSlice_Consensus * results, 
                          const libSlice_BaseDistribution * dist,
                          int highQualityThreshold);
         
// Wrapper to calculate the consensus and consensus quality values 
// for a range of slices with using ambiguity codes
int libSlice_getConsensusWithAmbiguity(const libSlice_Slice * s, 
                                       libSlice_Consensus * results, 
                                       const libSlice_BaseDistribution * dist,
                                       int highQualityThreshold);
//@}

/*! @name SliceRange Consensus Calculations 
 *  Functions for calculating the consensus and qv of a range of slices
 */
//@{

// Internal function: Calculates the consensus and consensus quality 
//                    values for an array of slices
int libSlice_getConsensusRangeParam(const libSlice_Slice s[],
                                    libSlice_Consensus results[], 
                                    int len,  
                                    const libSlice_BaseDistribution * dist,
                                    int highQualityThreshold,
                                    int doAmbiguity);

// Calculates the consensus of an array of slice with no ambiguity codes
int libSlice_getConsensusRange(const libSlice_Slice s[],
                               libSlice_Consensus results[], 
                               int len,  
                               const libSlice_BaseDistribution * dist,
                               int highQualityThreshold);

// Calculates the consensus of an array of slice with ambiguity codes
int libSlice_getConsensusRangeWithAmbiguity(const libSlice_Slice s[],
                                            libSlice_Consensus results[], 
                                            int len,  
                                            const libSlice_BaseDistribution * dist,
                                            int highQualityThreshold);

//@}

/*! @name Quality Class Calculations 
 *  Functions for calculating the quality class of slices
 */
//@{

// Calculates the consensus quality class for a single slice
char libSlice_getConsQC(const libSlice_Slice * s, int highQualityThreshold);

// Calculates consensus quality classes for a range of slices
char * libSlice_getConsQCRange(const libSlice_Slice s[], 
                               int len, 
                               int highQualityThreshold);

//@}

/*! @name Ambiguity Code Calculations
 *  Functions for calculating ambiguity Codes
 */
//@{

// Calculate IUPAC ambiguity code based on error probabilities
char libSlice_getAmbiguityCode(long double cpA,
                               long double cpC,
                               long double cpG,
                               long double cpT,
                               long double cpGap,
                               int highQualityThreshold,
                               int baseCount);

// Calculate error probabilities with a threshold into ambiguity flags
char libSlice_calculateAmbiguityFlags(long double cpA,
                                      long double cpC,
                                      long double cpG,
                                      long double cpT,
                                      long double cpGap, 
                                      int highQualityThreshold,
                                      int baseCount);


// Converts a character with 5 bit flags into IUPAC Ambiguity Code
char libSlice_convertAmbiguityFlags(char ambiguityFlags);

//@}


/*! @name Utility Functions
 *  Other useful functions
 */
//@{

// Get the complement of a base or ambiguity code
char libSlice_getCompliment(char base);

// Get a version string
const char * libSlice_getVersion();



//! Error Checking malloc
/*! Returns char * to a new block of memory, or prints an error if it couldnt
 *  malloc the memory.\n\n
 *
 *  A slight modification of Paul Hardy's code from alnseq
 *  @param number Number of blocks to allocate
 *  @param size Size of each block to allocate
 *  @return Block of new memory number*size bytes big
 *  @exception Exits(-1) If it can't malloc memory
 */
void * libSlice_newmem(int number, int size);

//@}

//! @}

#ifdef __cplusplus
}
#endif

#endif

