#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <string>
#include <vector>

class Position {
public:
	enum PositionEditType {
		REPLACE
	};

	Position();
	Position(std::string rangeName, uint32_t rangeStart, uint32_t rangeEnd, std::string seq);
        Position(const Position &other);

	Position merge(const Position& p, std::vector<Position>& edits);

	uint32_t getLength() const;
	std::string getName() const;
	uint32_t getStart() const;
	uint32_t getEnd() const;
	std::string getSequence() const;
	std::string getUngappedSequence() const;
        PositionEditType getEditType() const;

	void setEditType(PositionEditType);
	Position& operator= (const Position & source);

private:
	std::string sequence;
	uint32_t start;
	uint32_t end;
	std::string name;
	PositionEditType editType;
};

bool operator== (const Position & a, const Position & b);
bool operator!= (const Position & a, const Position & b);
