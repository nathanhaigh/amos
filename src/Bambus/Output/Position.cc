#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits>
#include <string>

#include "Position.hh"

Position::Position() {
	name = sequence = std::string();
	start = end = 0;
}

Position::Position(std::string rangeName, uint32_t rangeStart, uint32_t rangeEnd, std::string seq) {
	name = rangeName;
	start = rangeStart;
	end = rangeEnd;
	sequence = seq;
}

Position::Position(const Position &other) {
	name = other.name;
	start = other.start;
	end = other.end;
	sequence = other.sequence;
	editType = other.editType;
}

Position Position::merge(const Position &p, std::vector<Position>& edits) {
	Position result;

	if (sequence.length() == 0) {
		result = p;
	} else if (p.sequence.length() == 0) {
		result = (*this);
	} else {
		result.name = name + "_" + p.name;
		result.start = (p.start < start ? p.start : start);
		result.end = (p.end > end ? p.end : end);

		std::string fasta(result.end - result.start, 'N');
		uint32_t ovlStart = (p.start > start ? p.start : start);
		uint32_t ovlEnd = (p.end < end ? p.end : end);

		fasta.replace(p.start - result.start, p.end - p.start, p.sequence);
		fasta.replace(start - result.start, end - start, sequence);
		if (ovlEnd > ovlStart && (ovlEnd-ovlStart) > 0) {
			// get the real overlapping position (those not involving Ns)
			std::string overlap = fasta.substr(ovlStart - result.start, ovlEnd - ovlStart);
			size_t offset = 0;

		    // Skip delimiters at beginning.
		    std::string::size_type lastPos = overlap.find_first_not_of("N", 0);
		    // Find first "non-delimiter".
		    std::string::size_type pos     = overlap.find_first_of("N", lastPos);

		    while (std::string::npos != pos || std::string::npos != lastPos) {
		    	std::string token = overlap.substr(lastPos, pos-lastPos);
		    	if (token.length() > 0) {
					uint32_t start = (pos == std::string::npos ? 0 : pos) + ovlStart;
					uint32_t end = start + token.length();
					std::string seq = p.sequence.substr(start - p.start, end - start);
					Position edit(p.name, start, end, seq);
					edit.editType = Position::REPLACE;
					edits.push_back(edit);
		    	}

		    	// move to the next token in the list
		    	lastPos = overlap.find_first_not_of("N", pos);
		    	pos = overlap.find_first_of("N", lastPos);
		    }
		}
		result.sequence = fasta;
	}
	return result;
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
		name = source.name;
		sequence = source.sequence;
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

