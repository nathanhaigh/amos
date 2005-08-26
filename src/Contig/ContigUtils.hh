#ifndef CONTIGUTILS_HH_
#define CONTIGUTILS_HH_ 1

#include "foundation_AMOS.hh"

void leftTrimContig(AMOS::Contig_t & contig, int lefttrim);
void rightTrimContig(AMOS::Contig_t & contig, int righttrim);


void shiftContigRight(AMOS::Contig_t & contig, int contigShiftDistance);
void widenContig(AMOS::Contig_t & contig, int contigGrowDistance);

#endif
