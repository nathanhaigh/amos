#ifndef ALIGN_UTILS_HH_
#define ALIGN_UTILS_HH_

#include <string>

#include "align.hh"

namespace Output 
{

   static const char  SCAFFOLD_GAP_CHAR = 'N';
   static const int   MIN_OVERLAP = 15;
   static const int   OVL_WIGGLE  = 50;
   static const float ERROR_RATE  = 0.25;

   bool hasValidOverlap(
           const std::string sequenceA, 
           const std::string sequenceB, 
           uint32_t offset, 
           uint32_t startA, 
           uint32_t endA, 
           uint32_t startB, 
           uint32_t endB, 
           bool     &contained,
           Alignment_t &ali);
}
#endif
