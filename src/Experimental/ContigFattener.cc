#include "ContigFattener.hh"
#include "ContigUtils.hh"
#include "AMOS_Foundation.hh"

using namespace std;
using namespace AMOS;


ContigFattener::ContigFattener(Contig_t & contig,
                               Bank_t & read_bank,
                               bool verbose)
  : m_contig(contig),
    m_read_bank(read_bank),
    m_verbose(verbose)
{
  // recall to be non-ambiguous consensus for fattening
  recallCons();
}

ContigFattener::~ContigFattener()
{

}

ContigSequence * ContigFattener::getContigSequence(const string & eid)
{
  vector<Tile_t> & tiling = m_contig.getReadTiling();
  vector<Tile_t>::iterator ti;
  for (ti = tiling.begin();
       ti != tiling.end();
       ti++)
  {
    if (m_read_bank.lookupEID(ti->source) == eid)
    {
      Read_t read;
      m_read_bank.fetch(ti->source, read);
      return new ContigSequence(ti, read);
    }
  }

  return NULL;
}

void ContigFattener::extend3Read(ContigSequence * read, int distance)
{
  int distanceExtended = 0;
  int trimLength = read->get3TrimLength();
  int origUContigWidth = m_contig.getUngappedLength();

  int i = read->getReadID();

  if (m_verbose)
  {
    cerr << "Extending r" << read->getReadID() 
         << " by " << distance 
         << " of " << trimLength << endl;
  }

  if (trimLength)
  {
    if (read->isReverseCompliment())
    {
      distanceExtended = extend3Left(read, distance);
    }
    else
    {
      distanceExtended = extend3Right(read, distance);
    }
  }

  recallCons();
  
  recordExtension(read, distanceExtended, trimLength, 
                  0.0, 0.0, origUContigWidth);
}

// Public API
void ContigFattener::extend3Read(const string & seqname, int distance)
{
  ContigSequence * read = getContigSequence(seqname);
  if (read)
  {
    extend3Read(read, distance);
  }
  else
  {
    throw amosException("Specified read (" + seqname + ") not found", 
                        "ContigFattener::extend3Read");
  }
}

ContigSequenceSlice ContigFattener::getTilingSlice(int gindex)
{
  ContigSequenceSlice retval;

  vector<Tile_t> & tiling = m_contig.getReadTiling();
  sort(tiling.begin(), tiling.end(), TileOrderCmp());

  int count;
  vector<Tile_t>::iterator ti;
  for (count = 0, ti = tiling.begin(); ti != tiling.end(); ti++, count++)
  {
    if (!(ti->offset > gindex || ti->getRightOffset() < gindex))
    {
      Read_t rr;
      m_read_bank.fetch(ti->source, rr);

      ContigSequence cs(ti, rr, count);
      retval.push_back(cs);
    }
  }

  return retval;
}

// Public API
void ContigFattener::growContigLeft(bool requireConfirmation)
{
  // First try to fatten the left side
  fattenContig(0, 500, 1, 0);
  int confirmedDistance = 0;

  // Find the read that now extends the furthest
  int bestExtension = 0;
  ContigSequence * best = NULL;
  ContigSequenceSlice::iterator t;
  ContigSequenceSlice tilingSlice = getTilingSlice(0);

  for (t =  tilingSlice.begin();
       t != tilingSlice.end();
       t++)
  {
    if (t->isReverseCompliment())
    {
      int currentExtension = t->get3TrimLength();
      if (currentExtension > bestExtension)
      {
        bestExtension = currentExtension;
        best = &(*t);
      }
    }
  }

  // Stretch the contig with the best candidate
  if (best)
  {
    ID_t bestiid = best->getIID();

    if (m_verbose)
    {
      cerr << "best read to left extend is r" << best->getReadID()
           << " by " << bestExtension << "bp" << endl;
    }

    // best->offset == 0, so this extended by exactly bestExtension bp
    extend3Read(best, -1);

    // Now fatten the contig to the new beginning
    int origSize = m_contig.getLength();
    fattenContig(0, bestExtension+500, 1, 0);
    bestExtension += (m_contig.getLength() - origSize); // add extra gaps


    int confirmedOffset = m_contig.getLength();
    int confirmedRead = -1;

    vector<Tile_t> & tiling = m_contig.getReadTiling();
    vector<Tile_t>::iterator ti;

    for (ti = tiling.begin(); ti != tiling.end(); ti++)
    {
      if (ti->source != bestiid && 
          ti->offset < confirmedOffset)
      {
        confirmedOffset = ti->offset;
        confirmedRead = ti->source;
      }
    }

    confirmedDistance = (bestExtension - confirmedOffset);

    if (requireConfirmation)
    {
      cerr << "@TrimContigLeft=" << confirmedOffset << endl;
      leftTrimContig(m_contig, confirmedOffset);

      int count;
      tiling = m_contig.getReadTiling();
      
      for (count=0, ti = tiling.begin(); ti != tiling.end(); ti++, count++)
      {
        if (ti->source == bestiid)
        {
          Read_t read;
          m_read_bank.fetch(ti->source, read);
          best = new ContigSequence(ti, read, count);
          dumpRange(0, bestExtension+10, best);
          delete best;
          break;
        }
      }
    }
  }

  cerr << "@ConfirmLeftContigGrowDistance=" << confirmedDistance << endl;
}

// Public API
void ContigFattener::growContigRight(bool requireConfirmation)
{
  int confirmedDistance = 0;

  // First try to grow the right side
  fattenContig(m_contig.getLength()-500, m_contig.getLength(), 0, 1);

  // Find the read that now extends the furthest
  int bestExtension = 0;
  ContigSequence * best = NULL;
  ContigSequenceSlice::iterator t;
  ContigSequenceSlice tilingSlice = getTilingSlice(m_contig.getLength()-1);

  for (t =  tilingSlice.begin();
       t != tilingSlice.end();
       t++)
  {
    if (!t->isReverseCompliment())
    {
      int currentExtension = t->get3TrimLength();
      if (currentExtension > bestExtension)
      {
        bestExtension = currentExtension;
        best = &(*t);
      }
    }
  }

  // Stretch the contig with the best candidate
  if (best)
  {
    int bestiid = best->getIID();
    int oldSize = m_contig.getLength();
    if (m_verbose)
    {
      cerr << "best read to right extend is r" << best->getReadID()
           << " by " << bestExtension << "bp" << endl;
    }


    extend3Read(best, -1);
    
    // Now fatten the contig to match out to the new end
    fattenContig(oldSize-1, m_contig.getLength(), 0, 1);

    int confirmedOffset = oldSize;
    int confirmedRead = -1;
    
    vector<Tile_t> & tiling = m_contig.getReadTiling();
    vector<Tile_t>::iterator ti;

    for (ti = tiling.begin(); ti != tiling.end(); ti++)
    {
      if (ti->source != bestiid &&
          ti->getRightOffset() > confirmedOffset)
      {
        confirmedOffset = ti->getRightOffset();
        confirmedRead = ti->source;
      }
    }

    confirmedDistance = confirmedOffset - oldSize;

    if (requireConfirmation)
    {
      int trimdistance = m_contig.getLength() - confirmedOffset;
      cerr << "@TrimContigRight=" << trimdistance << endl;
      rightTrimContig(m_contig, trimdistance);

      int count;
      tiling = m_contig.getReadTiling();

      for (count = 0, ti = tiling.begin(); ti != tiling.end(); ti++, count++)
      {
        if (ti->source == bestiid)
        {
          Read_t read;
          m_read_bank.fetch(ti->source, read);
          best = new ContigSequence(ti, read, count);

          dumpRange(ti->offset-10, m_contig.getLength(), best);

          delete best;
          break;
        }
      }
    }
  }

  cerr << "@ConfirmRightContigGrowDistance=" << confirmedDistance << endl;
}

#include <algorithm>

// Public API
void ContigFattener::fattenContig(int fattenRangeStart, int fattenRangeEnd,
                                  int doLeftExtension, int doRightExtension)
{
  int contigSize = m_contig.getLength() - 1;
  int origUContigWidth = m_contig.getUngappedLength();

  if (fattenRangeEnd > contigSize)
  {
    fattenRangeEnd = contigSize;
  }

  if (m_verbose)
  {
    if (doLeftExtension)
    {
      cerr << "Left ";
      if (doRightExtension) { cerr << "and "; }
    }

    if (doRightExtension) { cerr << "Right "; }

    cerr << "Fattening in [" << fattenRangeStart
         << "," << fattenRangeEnd
         << "]" << endl;
  }

  vector<Tile_t> & tiling = m_contig.getReadTiling();

  sort(tiling.begin(), tiling.end(), TileOrderCmp());

  int count;
  vector<Tile_t>::iterator i;
  for (i =  tiling.begin(), count=0;
       i != tiling.end();
       i++, count++)
  {
    Read_t rr;
    m_read_bank.fetch(i->source, rr);
    ContigSequence * read = new ContigSequence(i, rr, count);

    int fattenRead = 0;
    int distanceExtended = 0;
    int trimLength = read->get3TrimLength();

    int count = 10;
    float origAvg = 0.0;
    float origTrimAvg = read->getAvgQV3Trim(count);

    if (read->isReverseCompliment())
    {
      origAvg = read->getAvgQV(read->getOffset(), count);

      fattenRead = doLeftExtension;
      if (fattenRead && fattenRangeStart != -1)
      {
        int offset = read->getOffset();
        fattenRead = fattenRangeStart <= offset && offset <= fattenRangeEnd;
      }

      if (fattenRead && trimLength)
      {
        distanceExtended = fatten3Left(read, -1);
      }
    }
    else
    {
      origAvg = read->getAvgQV(read->getRightEnd(), count);

      fattenRead = doRightExtension;
      if (fattenRead && fattenRangeStart != -1)
      {
        int right = read->getRightEnd();
        fattenRead = fattenRangeStart <= right && right <= fattenRangeEnd;
      }

      if (fattenRead && trimLength)
      {
        distanceExtended = fatten3Right(read, -1);
      }
    }

    if (fattenRead)
    {
      // TODO: Fix the range when gaps are inserted outside of the fatten range. 
      // Fortunately this will only falsely increase the range to fatten
      
      // Shift the end of the range to reflect the gaps inserted in the fatten
      // range
      if (fattenRangeEnd != -1)
      {
        int distanceToEnd = contigSize - fattenRangeEnd;
        contigSize = m_contig.getLength()-1;
        fattenRangeEnd = contigSize - distanceToEnd;
      }

      recordExtension(read, distanceExtended, trimLength, 
                      origAvg, origTrimAvg, origUContigWidth);
      origUContigWidth = m_contig.getUngappedLength();
    }

    delete read;
  }
}





int ContigFattener::extend3Left(ContigSequence * read, int distance)
{
  int retval = 0;
  string leftTrim = read->get3Trim();
  if (distance == -1) { distance = leftTrim.size(); }

  if (distance > (int)leftTrim.size())
  {
    cerr << "distance(" << distance 
         << ") > leftTrim(" << leftTrim.size() << endl;

    throw amosException("distance > leftTrim.size()", "extend3Left");
  }

  int origoffset = read->getOffset();
  if (origoffset != 0)
  {
    // We need to fatten this read to the left end of the contig
    int distanceAligned = fatten3Left(read, distance);

    retval += distanceAligned;

    distance -= distanceAligned;
    leftTrim.resize(leftTrim.size()-distanceAligned);

    stripSequenceEdgeGaps();
  }

  if (distance)
  {
    int newoffset = read->getOffset();

    if (newoffset != 0)
    {
      if (m_verbose)
      {
        cerr << "fattening failed to reach the end of the consensus" << endl;
      }
    }

    if (newoffset - distance < 0)
    {
      // need to shift the entire contig right
      int contigShiftDistance = distance - newoffset;

      cerr << "@ContigShiftDistance=" << contigShiftDistance << endl;
      shiftContigRight(m_contig, contigShiftDistance);
    }

    newoffset = read->getOffset(); // shifted as necessary

    // Trim off the left side (3' side) that isn't wanted
    long int eraseDistance = leftTrim.size() - distance;
    if (eraseDistance)
    {
      leftTrim.erase(0, eraseDistance);
    }

    // Finally, extend the read
    retval += read->extend3Left(leftTrim);

    dumpRange(read->getOffset()-10, newoffset+10, read);
  }

  return retval;
}

int ContigFattener::extend3Right(ContigSequence * read, int distance)
{
  int retval = 0;
  string rightTrim = read->get3Trim();
  if (distance == -1) { distance = rightTrim.size(); }

  if (distance > (int)rightTrim.size())
  {
    cerr << "distance(" << distance 
         << ") > rightTrim(" << rightTrim.size() << endl;

    throw amosException("distance > rightTrim.size()", "extend3Right");
  }


  int origend = read->getRightEnd();
  if (origend != m_contig.getLength()-1)
  {
    // We need to fatten this read to the right end of the contig
    int distanceAligned = fatten3Right(read, distance);
    retval += distanceAligned;

    distance -= distanceAligned;
    rightTrim.erase(0, distanceAligned);

    stripSequenceEdgeGaps();
  }

  if (distance)
  {
    int newend = read->getRightEnd();

    if (newend != m_contig.getLength()-1)
    {
      cerr << "fattening failed to reach the end of the consensus" << endl;
    }

    if (newend + distance >= m_contig.getLength())
    {
      // open space in the consensus and tiling
      // need to grow contig
      int contigGrowDistance =  newend + distance + 1 - m_contig.getLength();

      cerr << "@ContigGrowDistance=" << contigGrowDistance << endl;
      widenContig(m_contig, contigGrowDistance);
    }

    // Finally, extend the read
    rightTrim.resize(distance);
    retval += read->extend3Right(rightTrim);

    dumpRange(origend-10, read->getRightEnd()+10, read);
  }

  return retval;
}

void ContigFattener::recordExtension(const ContigSequence * read, 
                                     int distanceExtended,
                                     int totalTrim,
                                     float origAvg,
                                     float origTrimAvg,
                                     int origUContigWidth)
{
  bool rc = read->isReverseCompliment();
  int offset = read->getOffset();
  int rightEnd = read->getRightEnd();
  int count = 10;

  float avg = read->getAvgQV((rc) ? offset : rightEnd, count);
  float trimavg = read->getAvgQV3Trim(count);

  int distanceToEnd = (rc) ? offset : m_contig.getLength() - rightEnd-1;
  int deltaUContig = m_contig.getUngappedLength() - origUContigWidth;

  fprintf(stderr, "%s=%d %d %d %d %0.1f %0.1f %0.1f %0.1f\n",
          read->getSeqname().c_str(),
          totalTrim,
          distanceExtended,
          deltaUContig,
          distanceToEnd,
          origAvg,
          origTrimAvg,
          avg,
          trimavg);
}




long int ContigFattener::getBasesToSkip(DeltaVector & delta,
                                        DeltaVector::const_iterator & deltaiter,
                                        bool & consGap,
                                        unsigned int cpos,
                                        unsigned int rpos,
                                        const string & trimmed,
                                        const string & consensus,
                                        bool autoPromoteConsGaps)
{
  long int basesToSkip = 0;

  if (deltaiter != delta.end())
  {
    basesToSkip = *deltaiter;
    consGap = true;

    if (basesToSkip < 0)
    {
      basesToSkip = -basesToSkip;
      consGap = false;
    }

    if (!autoPromoteConsGaps) { consGap = !consGap; }

    if (m_verbose)
    {
      if (cpos < (unsigned int) m_contig.getLength() && 
          rpos < trimmed.size())
      {
        cerr << "basesToSkip: " << basesToSkip 
             << " cpos: " << cpos << " [" << consensus[cpos] << "]"
             << " rpos: " << rpos << " [" << trimmed[rpos]     << "]"
             << " consGap: " << consGap
             << endl;
      }
    }
  }

  return basesToSkip;
}



// Merge the bases in trimmedSeq with the consensus beteen cpos and lastConsPos
// (inclusively) inserting gaps as necessary. Throw an error if the alignment
// does go as expected. Optionally autoPromote gaps from the consensus as
// necessary. lastConsPos must point at an actual base (not gap).

void ContigFattener::mergeExtension(unsigned int & cpos, 
                                    unsigned int & lastConsPos,
                                    unsigned int & rpos, 
                                    string & trimmedSeq, 
                                    string & consensus,
                                    ContigSequence * read,
                                    DeltaVector & delta,
                                    bool autoPromoteConsGaps,
                                    int consMovementDelta)
{
  int startcpos = cpos;

  if (m_verbose)
  {
    cerr << "Delta:"; 
    for (unsigned int j = 0; j < delta.size(); j++) { cerr << " " << delta[j]; }
    cerr << endl;
  }

  bool consGap = true;
  DeltaVector::const_iterator deltaiter = delta.begin();
  long int basesToSkip = getBasesToSkip(delta, deltaiter, consGap,
                                        cpos, rpos, trimmedSeq, consensus,
                                        autoPromoteConsGaps);

  // AutoPromote Consensus Gaps into the trimmedSeq, and
  // merge the delta values from the alignment
  while(cpos >= 0 && cpos < (unsigned int) m_contig.getLength() &&
        cpos <= lastConsPos && rpos < trimmedSeq.size())
  {

    // cerr <<  "cons[" << cpos << "]=" << consensus[cpos]
    //      << " read[" << rpos << "]=" << trimmedSeq[rpos] << endl;

    if (basesToSkip == 1)
    {
      if (consGap)
      {
        if (autoPromoteConsGaps && consensus[cpos] == '-')
        {
          // next position is a gap anyways
          if (m_verbose) { cerr << "Gap already present at " << cpos << endl; }
        }
        else
        {
          // insert a gap
          insertGapSlice(cpos, read);
          consensus = m_contig.getSeqString();
          lastConsPos++;
        }
      }
      else
      {
        if (m_verbose) { cerr << "inserting gap into r at " << cpos  << endl; }
        trimmedSeq.insert(rpos, "-");

        while (autoPromoteConsGaps && consensus[cpos] == '-')
        {
          if (m_verbose) { cerr << "promoting adjacent consensus gap at " << cpos << endl; }
          trimmedSeq.insert(rpos, "-");
          rpos++; cpos += consMovementDelta;
        }
      }

      deltaiter++;
      basesToSkip = getBasesToSkip(delta, deltaiter, consGap, 
                                   cpos+consMovementDelta, 
                                   rpos+1, trimmedSeq, consensus,
                                   autoPromoteConsGaps);
    }
    else
    {
      if (autoPromoteConsGaps && consensus[cpos] == '-')
      {
        if (m_verbose) { cerr << "promoting consensus gap at " << cpos << endl; }
        trimmedSeq.insert(rpos, "-");
      }
      else if (basesToSkip)
      {
        // Don't count gaps
        basesToSkip--;
      }
    }
    
    // Step to next position
    cpos += consMovementDelta; 
    rpos++;
  }

  // handle special case where gaps need to be inserted at end of alignment, 
  // after the read or consensus has "ended".
  if (basesToSkip)
  {
    if (m_verbose) { cerr << "Extra Bases to skip: " << basesToSkip << endl; }

    while (basesToSkip == 1)
    {
      if (consGap)
      {
        // Don't use gaps that are already present since we hit the end of allow range
        insertGapSlice(cpos, read);
        consensus = m_contig.getSeqString();
        lastConsPos++;
      }
      else
      {
        while (autoPromoteConsGaps && consensus[cpos] == '-')
        {
          if (m_verbose) { cerr << "promoting consensus gap at " << cpos << endl; }
          trimmedSeq.insert(rpos, "-");

          rpos++;
          cpos += consMovementDelta;
        }

        if (m_verbose) { cerr << "Appending extra gap to fix alignment!" << endl; }
        trimmedSeq.insert(rpos, "-");
      }

      deltaiter++;
      basesToSkip = getBasesToSkip(delta, deltaiter, consGap, 
                                   cpos+consMovementDelta, 
                                   rpos+1, trimmedSeq, consensus,
                                   autoPromoteConsGaps);
      rpos++;
      cpos += consMovementDelta;
    }
  }

  if (m_verbose)
  {
    cerr << "final:"
         << " cons[" << cpos-1 << "]=" << consensus[cpos-1]
         << " read[" << rpos-1 << "]=" << trimmedSeq[rpos-1]
         << " read[" << trimmedSeq.length()-1 << "]=" << trimmedSeq[trimmedSeq.length()-1]
         << endl;
  }

  if (basesToSkip)
  {
    cerr << "Internal Error!!! More delta values that sequence to align!" << endl;
    throw amosException("More delta values than sequence to align!",
                        "Contig::mergeExtension");
  }

  if (rpos != trimmedSeq.length() || cpos != lastConsPos+1)
  {
    char buffer[1024];
    sprintf(buffer, "Attempt to merge alignment failed: rpos=%d rlen=%d cpos=%d lastcpos=%d",
            rpos, trimmedSeq.length(), cpos, lastConsPos);
    throw amosException(buffer, "Contig::mergeExtension");
  }
}

//#define _DEBUG_VERBOSE 1
//#define _DEBUG_ASSERT 1

#include "sw_align.hh"

int ContigFattener::fatten3Left(ContigSequence * read, int distance)
{
  DeltaVector delta;
  long int gindex = read->getOffset()-1;
  int i;
  unsigned int alignmentType = OPTIMAL_BACKWARD_SEARCH;
  int retval = 0;

  if (gindex < 0)
  {
    // No consensus to match
    if (m_verbose) { cerr << "No consensus to match for r" << read->getReadID() << endl; }
    return 0;
  }

  string leftTrim = "r" + read->get3Trim();

  if (distance != -1)
  {
    if (m_verbose) { cerr << "Trimming Left Trim to " << distance+1 << endl; }
    reverse(leftTrim.begin(), leftTrim.end());
    leftTrim.resize(distance+1);
    reverse(leftTrim.begin(), leftTrim.end());
    alignmentType = OPTIMAL_BACKWARD_SEARCH;
  }

  long int leftend = leftTrim.size()-1;

  if (leftend == 0)
  {
    // No data to extend with
    if (m_verbose) { cerr << "No left trim for r" << read->getReadID() << endl; }
    return 0;
  }

  if (m_verbose)
  {
    cerr << "attempting left extension of r" << read->getReadID() 
         << " at " << gindex << " with " << leftend << "bp" << endl;

    long int leftpos = gindex-leftend;
    int shift = (leftpos > 10) ? 10 : leftpos;
    dumpRange(leftpos-shift, gindex+10, read);

    cerr << "trim:\t";
    if (leftpos > 0)
    {
      for (i = 0; i < shift; i++) cerr << "*";
      cerr << leftTrim << endl;;
    }
    else 
    {
      cerr << leftTrim.substr(-leftpos, leftTrim.length()+leftpos) << endl;
    }
  }

  string cons = "c" + m_contig.getUngappedSeqString(Range_t(0, gindex+1));
  long int consend = cons.size()-1;
  if (consend == 0)
  {
    // No consensus to match
    if (m_verbose) { cerr << "No consensus to match for r" << read->getReadID() << endl; }
    return 0;
  }

  long int leftstart = 1;
  long int consstart = consend - leftend - 150; // allow for 150 gaps max
  if (consstart < 1) consstart = 1;

  // Compute leftend, consend
  alignSearch(leftTrim.c_str(), leftend, leftstart,
              cons.c_str(), consend, consstart,
              alignmentType);

  if ((leftend > 0 && consend > 0) &&
      (alignTarget(leftTrim.c_str(), leftstart, leftend, 
                   cons.c_str(), consstart, consend,
                   delta, FORCED_FORWARD_ALIGN)))

  {
    // alignment was sucessful!
    if (m_verbose) { cerr << "leftend=" << leftend << " consend=" << consend << endl; }

    // Trim the part that did not align (and leftTrim[0])
    leftTrim.erase(0, leftstart);

    string consensus = m_contig.getSeqString();

    unsigned int rpos = 0;
    unsigned int cpos = m_contig.ungap2gap(consstart-1);
    while (consensus[gindex+1] == '-')
    {
      gindex++;
    }


    string rightEdgeGaps;

    // Starting at an gapped position is trouble
    // Promote the edge gaps before attempting to merge the extension
    while (gindex >= 0 && consensus[gindex] == '-')
    {
      if (m_verbose) { cerr << "Promoting right edge consensus gap at " << cpos << endl; }
      rightEdgeGaps += '-';
      gindex--;
    }

    // switch to unsigned after subtracting!
    unsigned int lastConsPos = gindex;

    mergeExtension(cpos, lastConsPos, rpos, leftTrim, consensus,
                   read, delta, true, +1);

    cpos--; // Return to original gindex position
    retval = read->extend3Left(leftTrim+rightEdgeGaps);
    dumpRange(read->getOffset()-10, cpos+10, read);
  }
  else 
  {
    if (m_verbose) { cerr << "no alignment consend: " << consend << " leftend: " << leftend << endl; }
  }

  return retval;
}

int ContigFattener::fatten3Right(ContigSequence * read, int distance)
{
  DeltaVector delta;

  long int gindex = read->getRightEnd()+1;
  unsigned int alignmentType = OPTIMAL_FORWARD_SEARCH;
  int retval = 0;

  if (gindex >= m_contig.getLength())
  {
    // No consensus to match
    if (m_verbose) { cerr << "No consensus to match for r" << read->getReadID() << endl; }
    return 0;
  }

  string rightTrim = "r" + read->get3Trim();
  if (distance != -1)
  {
    if (m_verbose) { cerr << "Trimming Right Trim to " << distance+1 << endl; }
    rightTrim.resize(distance+1);
    alignmentType = OPTIMAL_FORWARD_SEARCH;
  }
  long int rightend = rightTrim.size()-1;

  if (rightend == 0 || gindex >= m_contig.getLength())
  {
    // No data to extend with
    if (m_verbose) { cerr << "No right trim for r" << read->getReadID() << endl; }
    return 0;
  }

  if (m_verbose)
  {
    cerr << "attempting right extension of r" << read->getReadID() 
         << " at " << gindex << " with " << rightend << "bp" << endl;

    long int leftpos = gindex-10;
    if (leftpos < 0) {leftpos = 0;}
    dumpRange(leftpos, gindex+rightend+50, read);

    cerr << "trim:\t";
    for (int i = leftpos; i < gindex-1; i++) { cerr << "?"; }
    cerr << rightTrim << endl;
  }

  // Strip the gaps from the consensus
  string cons = "c" + m_contig.getUngappedSeqString(Range_t(gindex, m_contig.getLength()));
  long int consend = cons.size()-1;
  if (consend > rightend + 150) { consend = rightend + 150; }

  if (consend == 0)
  {
    // No consensus to match
    if (m_verbose) { cerr << "No consensus to match for r" << read->getReadID() << endl; }
    return 0;
  }

  if (read->getReadID() == 357)
  {
    cerr << "gindex: " << gindex;
    cerr << " contiglen: " << m_contig.getLength() << endl;
    cerr << "cons: " << cons << endl;
  }

  // Compute rightend, consend
  alignSearch(rightTrim.c_str(), 1, rightend, 
              cons.c_str(), 1, consend,
              alignmentType);

  if ((rightend > 0 && consend > 0) &&
      (alignTarget(rightTrim.c_str(), 1, rightend, 
                   cons.c_str(), 1, consend,
                   delta, FORCED_FORWARD_ALIGN)))
  {
    // alignment was sucessful!
    if (m_verbose) { cerr << "rightend=" << rightend << " consend=" << consend << endl; }

    // Erase the kludge from rightTrim[0] ('r')
    rightTrim.erase(rightTrim.begin());

    // Trim the part that did not align
    rightTrim.resize(rightend);

    unsigned int cpos = gindex; 
    unsigned int rpos = 0; 

    string consensus = m_contig.getSeqString();

    // compute gapped position of last consensus base which aligns
    unsigned int lastConsPos = gindex;
    int count = 0;

    // shift to first non-gap base (probably just gindex)
    while (consensus[lastConsPos] == '-') { lastConsPos++; }

    // Now find the exact coordinate of the last *base* which aligns
    while (count < consend)
    {
      if (consensus[lastConsPos] != '-') { count++; }
      lastConsPos++;
    }

    lastConsPos--; // Step back to exact coordinate of lastAlignedBase

    mergeExtension(cpos, lastConsPos, rpos, rightTrim, consensus, read, delta, true, +1);

    retval = read->extend3Right(rightTrim);
    dumpRange(gindex-10, read->getRightEnd()+10, read);
  }
  else
  {
    if (m_verbose) { cerr << "no alignment consend: " << consend << " rightend: " << rightend << endl; }
  }

  return retval;
}



void ContigFattener::stripSequenceEdgeGaps()
{
  vector<Tile_t> & tiling = m_contig.getReadTiling();
  vector<Tile_t>::iterator ti;

  for (ti = tiling.begin(); ti != tiling.end(); ti++)
  {
    int gapcount;
    gapcount = stripLeftGaps(ti);
    gapcount = stripRightGaps(ti);
  }
}

int ContigFattener::stripLeftGaps(vector<Tile_t>::iterator ti)
{
  int leftGaps = 0;
  vector<Pos_t> newgaps;
  vector<Pos_t>::iterator gi;

  for (gi = ti->gaps.begin(); gi != ti->gaps.end(); gi++)
  {
    if (*gi == 0)
    {
      leftGaps++;
    }
    else
    {
      newgaps.push_back(*gi);
    }
  }

  if (leftGaps)
  {
    cerr << "Stripping " << leftGaps << 
            " leftGaps from r" << ti->source << endl;

    ti->gaps = newgaps;
    ti->offset += leftGaps;
  }

  return leftGaps;
}

int ContigFattener::stripRightGaps(vector<Tile_t>::iterator ti)
{
  int rightGaps = 0;
  vector<Pos_t> newgaps;
  vector<Pos_t>::iterator gi;

  for (gi = ti->gaps.begin(); gi != ti->gaps.end(); gi++)
  {
    if (*gi == ti->range.getHi())
    {
      rightGaps++;
    }
    else
    {
      newgaps.push_back(*gi);
    }
  }

  if (rightGaps)
  {
    cerr << "Stripping " << rightGaps << 
            " rightGaps from r" << ti->source << endl;

    ti->gaps = newgaps;
  }

  return rightGaps;
}



void ContigFattener::stripGapSlices()
{
  vector<Pos_t> allGapPositions;
  ContigIterator_t ci(m_contig, &m_read_bank);

  while (ci.advanceNext())
  {
    const TiledReadList_t & tiling = ci.getTilingReads();

    if (!tiling.empty())
    {
      Pos_t gindex = ci.gindex();
      bool allGaps = true;
      TiledReadList_t::const_iterator ri;
      for (ri = tiling.begin(); ri != tiling.end(); ri++)
      {
        char b = ri->base(gindex);

        if (b != '-')
        {
          allGaps = false;
          break;
        }
      }
      
      if (allGaps)
      {
        allGapPositions.push_back(gindex);
      }
    }
  }

  // Remove from right to left, so coordinates will remain accurate
  for (int i = allGapPositions.size()-1; i >= 0; i--)
  {
    removeGapColumn(allGapPositions[i]);
  }
}

void ContigFattener::removeGapColumn(Pos_t gindex)
{
  string qual(m_contig.getQualString());
  string seq(m_contig.getSeqString());
  
  if (seq[gindex] != '-')
  {
    AMOS_THROW_ARGUMENT("Specified position is not a gap column");
  }

  seq.erase(gindex, 1);
  qual.erase(gindex, 1);

  m_contig.setSequence(seq,qual);

  vector<Tile_t> & tiling = m_contig.getReadTiling();
  vector<Tile_t>::iterator ti;
  for (ti =  tiling.begin();
       ti != tiling.end();
       ti++)
  {
    if (gindex < ti->offset)
    {
      // just shift the read down
      ti->offset--;
    }
    else if (gindex <= ti->getRightOffset())
    {
      // offset <= gindex <= roffset
      // shift the read down, erase gap

      int gseqpos = gindex - ti->offset;

      vector<Pos_t> newgaps;
      vector<Pos_t>::iterator gi;
      bool foundGap = false;
      int gapcount = 0;

      for (gi = ti->gaps.begin(); gi != ti->gaps.end(); gi++, gapcount++)
      {
        if (*gi + gapcount == gseqpos)
        {
          foundGap = true;
        }
        else
        {
          // sequence position remains the same for all gaps
          newgaps.push_back(*gi);
        }
      }

      if (!foundGap)
      {
        AMOS_THROW_ARGUMENT("Specified position is not a gap in read");
      }

      ti->gaps = newgaps;

      // if gindex==offset, remove gap, but don't shift offset
    }
  }
}


void ContigFattener::printPos(long gindexstart, long gindexend)
{
  long maxvalue = gindexend;
  long divide = 1;
  int numDigits = 1;

  while (maxvalue > 9)
  {
    divide *= 10;
    maxvalue /= 10;
    numDigits++;
  }

  for (; numDigits > 0; numDigits--)
  {
    cerr << "pos:\t";
    for (long j = gindexstart; j <= gindexend; j++)
    {
      int currentValue = (j/divide) % 10;
      cerr << currentValue;
    }
    cerr << endl;

    divide /= 10;
  }
}



void ContigFattener::dumpRange(long gindexstart, 
                               long gindexend, 
                               ContigSequence * read)
{
  if (!m_verbose) { return; }

  if (gindexstart < 0)                   { gindexstart = 0; }
  if (gindexend >= m_contig.getLength()) { gindexend = m_contig.getLength()-1; }

  printPos(gindexstart, gindexend);

  string cons = m_contig.getSeqString();

  cerr << "cons:\t" 
       << cons.substr(gindexstart, gindexend - gindexstart +1)
       << endl;

  if (read)
  {
    read->dumpRange(gindexstart, gindexend);
  }
  else
  {
    vector<Tile_t> & tiling = m_contig.getReadTiling();
    vector<Tile_t>::iterator ti;

    for (ti = tiling.begin(); ti != tiling.end(); ti++)
    {
      if (!((ti->offset > gindexend) || (ti->getRightOffset() < gindexstart)))
      {
        Read_t rr;
        m_read_bank.fetch(ti->source, rr);

        ContigSequence r(ti, rr);
        r.dumpRange(gindexstart, gindexend);
      }
    }
  }
}

void ContigFattener::dumpAlignment()
{
  if (!m_verbose) { return; }

  printPos(0, m_contig.getLength()-1);

  string cons = m_contig.getSeqString();
  cerr << "cons:\t" <<  cons << endl;

  vector<Tile_t> & tiling = m_contig.getReadTiling();
  vector<Tile_t>::iterator ti;

  for (ti = tiling.begin(); ti != tiling.end(); ti++)
  {
    Read_t rr;
    m_read_bank.fetch(ti->source, rr);

    ContigSequence r(ti, rr);
    r.dump();
  }
}

void ContigFattener::recallCons()
{
  ContigIterator_t ci(m_contig, &m_read_bank);

  cerr << "Recalling consensus...";

  string consensus, consqual;

  char cbases[]  = "ACGT-";

  while (ci.advanceNext())
  {
    char cons = ci.cons();
    char cq = ci.cqv();

    // Store the count of ACGT-
    int scores[5] = {0,0,0,0,0};

    const TiledReadList_t & tiling = ci.getTilingReads();

    if (!tiling.empty())
    {
      TiledReadList_t::const_iterator ri;
      for (ri = tiling.begin(); ri != tiling.end(); ri++)
      {
        char b = ri->base(ci.gindex());

        switch(b)
        {
          case 'A': case 'a': scores[0] += 1; break;
          case 'C': case 'c': scores[1] += 1; break;
          case 'G': case 'g': scores[2] += 1; break;
          case 'T': case 't': scores[3] += 1; break;
          case '-':           scores[4] += 1; break;
        };
      }

      unsigned int i;
      unsigned int max = 0; // Assume A is max
      int maxcount = scores[max];
      for (i = 1; i < 5; i++)
      {
        if (scores[i] > maxcount)
        {
          max = i;
          maxcount = scores[i];
        }
      }

      cons = cbases[max];

      // Get rough cqv by doubling counts, subtracting the conflicting counts, 
      // but prefer gap misalignment to base misalignment
      maxcount *= 2;
      
      for (i = 0; i < 5; i++)
      {
        if (i != max)
        {
          maxcount -= (i == 4) ? scores[i] : 2*scores[i];
        }
      }
      cq = maxcount;
    }

    consensus += cons;
    consqual += 'N';
  }

  cerr << " setting... cl:" << consensus.length() << " ql: " << consqual.length();
  m_contig.setSequence(consensus, consqual);
  cerr << " done" << endl;
}



void ContigFattener::insertGapSlice(long gindex, ContigSequence * read)
{
  if (m_verbose) { cerr << "inserting gap slice at " << gindex << endl; }
  
  m_contig.insertGapColumn(gindex);

  //TODO:: read?
  // read->adjustOffset(gindex, +1);
}

