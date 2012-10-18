#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits>
#include <string>

#include <assert.h>
#include <iostream>

#include "Position.hh"
#include "Align_Utils.hh"
#include "align.hh"

Position::Position() {
	name = sequence = std::string();
	start = end = oldEnd = 0;
	numGaps = 0;
}

Position::Position(std::string rangeName, uint32_t rangeStart, uint32_t rangeEnd, std::string seq) {
	name = rangeName;
	start = rangeStart;
	end = rangeEnd;
        oldEnd = rangeEnd;
	sequence = seq;

        if (end != start + sequence.length()) {
           end = start + sequence.length();
        }
        numGaps = 0;
        for ( uint32_t i = 0; i < sequence.length(); i ++ ) {
        	if ( !isalpha (sequence[i]) || sequence[i] == Output::SCAFFOLD_GAP_CHAR) {
			numGaps++;
		}
	}
}

Position::Position(const Position &other) {
	name = other.name;
	start = other.start;
	end = other.end;
        oldEnd = other.oldEnd;
	sequence = other.sequence;
	editType = other.editType;
	numGaps = other.numGaps;
}

Position Position::merge(Position &p, std::vector<Position>& edits) {
	Position result;
	if (sequence.length() == 0) {
		result = p;
	} else if (p.sequence.length() == 0) {
		result = (*this);
	} else {
                bool merge = true; 
		result.name = name + "_" + p.name;
		result.start = (p.start < start ? p.start : start);
                result.oldEnd = (p.oldEnd > oldEnd ? p.oldEnd : oldEnd);

                // adjust position based on where we were supposed to be (preserving originally computed distance)
                // this happens if one of the sequences we are merging has a larger overlap than we expected
                // then it will shrink in size and we need to adjust
                int32_t adjustSecond = 0;
                if (p.start < start) {
                   // when p starts earlier than the current sequence and has changed in size, we need to update current
                   // the current is only updated when it is not contained
                   if (p.end != p.oldEnd && oldEnd > p.oldEnd) {
                      adjustSecond = (int32_t)p.end - (int32_t)p.oldEnd;
                      if ((int) start + adjustSecond < 0) { adjustSecond -= ((int)start+adjustSecond); }
                      start += adjustSecond;
                      end += adjustSecond;
                   }
                } else {
                   // when the current starts earlier than p and has changed in size, update p
                   // p is only updated when it is not contained
                   if (end != oldEnd && p.oldEnd > oldEnd) {
                      adjustSecond = (int32_t)end - (int32_t)oldEnd;
                      if ((int) p.start + adjustSecond < 0) { adjustSecond -= ((int)p.start+adjustSecond); }
                      p.start += adjustSecond;
                      p.end += adjustSecond;
                   }
                }
                result.end = (p.end > end ? p.end : end);
		uint32_t ovlStart = (p.start > start ? p.start : start);
		uint32_t ovlEnd = (p.end < end ? p.end : end);

                // when we have an overlap or are close to one, use sequence alignment to find it and adjust positions
                Alignment_t ali;
                bool contained = false;
                bool aligns = false;
                if ((ovlEnd > ovlStart && (ovlEnd-ovlStart) > 0) || (ovlEnd <=ovlStart && ((int32_t)(ovlEnd-ovlStart)+2*Output::OVL_WIGGLE) > 0)) {
                   aligns = Output::hasValidOverlap(p.sequence, sequence, result.start, p.start, p.end, start, end, contained, ali);
                   if (aligns == false && ovlEnd > ovlStart) { // not an overlap
                      // when it is contained and the nodes do not overlap, skip merging the sequence in and report it separately
                      if (contained) {
                          merge = false;
                      // again we though they overlap but they don't, make them abut (with a small gap) instead
                      } else {
                         result.end += (ovlEnd - ovlStart)+Output::MIN_OVERLAP;
                         if (p.start == ovlStart) {
                            p.start += (ovlEnd-ovlStart)+Output::MIN_OVERLAP;
                            p.end   += (ovlEnd-ovlStart)+Output::MIN_OVERLAP;
                            ovlStart = ovlEnd = p.start;
                         } else if (start == ovlStart) {
                            start += (ovlEnd - ovlStart)+Output::MIN_OVERLAP;
                            end   += (ovlEnd - ovlStart)+Output::MIN_OVERLAP;
                            ovlStart = ovlEnd = start;
                         }
                      } 
                   } else if (aligns == true) {
                      // adjust positions based on overlap
                      if (p.start < start) {
                         start = (p.start + ali.b_hi - (ali.b_hi - ali.b_lo));
                         end   = (start + (sequence.length()));
                         ovlStart = start;
                         ovlEnd = ovlStart + (ali.b_hi - ali.a_lo);
                      } else {
                         p.start = (start + ali.b_hi - (ali.b_hi - ali.b_lo));
                         p.end   = (p.start + (p.sequence.length()));
                         ovlStart = p.start;
                         ovlEnd = ovlStart + (ali.b_hi - ali.b_lo);
                      }
                      result.end = (p.end > end ? p.end : end);
                   }
                }
                // now build the result string
                std::string fasta;
                std::string overlap;
                uint32_t gapAdjust = 0;
                if (contained && !merge) {
                   Position edit;
                   if (p.sequence.length() > sequence.length()) {
                      result = p;
                      edit = Position(name, start, end, sequence);
                   } else {
                      result = (*this);
                      edit = Position(p.name, p.start, p.end, p.sequence);
                   }
                   edit.editType = Position::REPLACE;
                   edits.push_back(edit);
                } else {
                   fasta = std::string(result.end - result.start, 'N');
                   result.numGaps = fasta.length();

                   fasta.replace(p.start - result.start, p.end - p.start, p.sequence);
                   result.numGaps = result.numGaps - p.sequence.length() + p.numGaps;

                   fasta.replace(start - result.start, Min(ovlStart-result.start, end - start), sequence, 0, Min(ovlStart-result.start, end-start));
                   gapAdjust = sequence.length();
                   assert(end-start == sequence.length());
                   assert(p.end-p.start == p.sequence.length());

                   if (aligns) {
                      if (p.start >= start) {
                         overlap = sequence.substr(ali.b_lo, sequence.length());
                      } else if (p.start <= start && p.end >= end) { 
                         overlap = sequence.substr(ali.a_lo, sequence.length());
                      } else {
                         cerr << "Unexpected overlap between sequences " << name << " and " << p.name << " which overlap from " << ali.a_lo << " to " << ali.b_lo << endl;
                         assert(0);
                         overlap = p.sequence.substr(ali.a_lo, sequence.length());
                      }
                   }

                if (overlap.length() > 0) {
			// get the real overlapping position (those not involving Ns)
			//std::string overlap = fasta.substr(ovlStart - result.start, ovlEnd - ovlStart);
			size_t offset = 0;

		    // Skip delimiters at beginning.
		    std::string::size_type lastPos = overlap.find_first_not_of("N", 0);
		    // Find first "non-delimiter".
		    std::string::size_type pos     = overlap.find_first_of("N", lastPos);
		    while (std::string::npos != pos || std::string::npos != lastPos) {
		    	std::string token = overlap.substr(lastPos, pos-lastPos);
		    	gapAdjust -= token.length();
			if (token.length() > 0) {
					uint32_t start = (pos == std::string::npos ? 0 : pos) + ovlStart;
					uint32_t end = start + token.length();
					std::string seq = overlap.substr(start-ovlStart, end-start);
					Position edit(p.name, start, end, seq);
					edit.editType = Position::REPLACE;
					edits.push_back(edit);
		    	}

		    	// move to the next token in the list
		    	lastPos = overlap.find_first_not_of("N", pos);
		    	pos = overlap.find_first_of("N", lastPos);
		    }
		}
                result.numGaps = result.numGaps - gapAdjust + numGaps;
		result.sequence = fasta;
           }
	}
        assert(result.end-result.start == result.sequence.length());

	return result;
}

uint32_t Position::getUngappedLength() const {
	return getLength() - numGaps;
}

uint32_t Position::getLength() const {
	return end-start;
}

std::string Position::getName() const {
	return name;
}

uint32_t Position::getStart() const {
	return start;
}

uint32_t Position::getEnd() const {
	return end;
}

uint32_t Position::getOldEnd() const {
        return oldEnd;
}

std::string Position::getSequence() const {
	return sequence;
}

std::string Position::getUngappedSequence() const {
  std::string retval;
  retval.reserve(sequence.length());

  for ( uint32_t i = 0; i < sequence.length(); i ++ )
    {
      if ( isalpha (sequence[i]) )
        retval . push_back (sequence[i]);
    }

    return retval;
}

Position::PositionEditType Position::getEditType() const {
	return editType;
}

void Position::setEditType(Position::PositionEditType edit) {
	editType = edit;
}

Position& Position::operator= (const Position & source) {
	if ( this != &source ) {
		editType = source.editType;
		start = source.start;
		end = source.end;
                oldEnd = source.oldEnd;
		name = source.name;
		sequence = source.sequence;
		numGaps = source.numGaps;
	}
	return *this;
}

bool operator== (const Position & a, const Position & b) {
	if (a.getEditType() == b.getEditType()
			&& a.getStart() == b.getStart()
			&& a.getEnd() == b.getEnd()
			&& a.getName() == b.getName()
			&& a.getSequence() == b.getSequence())
		return true;
	else
		return false;
}

bool operator!= (const Position & a, const Position & b) {
	return !(a == b);
}

