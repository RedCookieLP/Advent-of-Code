#include <AoC-Module.h>
#include <unordered_set>
#include <Vector2.hpp>

using Position = Vector2u32;

const static Position NORTH_WEST{ -1u, -1u };
const static Position WEST{ -1u, 0u };
const static Position SOUTH_WEST{ -1u, 1u };
const static Position SOUTH{ 0u, 1u };
const static Position SOUTH_EAST{ 1u, 1u };
const static Position EAST{ 1u, 0u };
const static Position NORTH_EAST{ 1u, -1u };
const static Position NORTH{ 0u, -1u };

#ifdef PART_1
static std::unordered_set<Position> s_XPositions{};
#endif
static std::unordered_set<Position> s_MPositions{};
static std::unordered_set<Position> s_APositions{};
static std::unordered_set<Position> s_SPositions{};

inline static bool containsPos(const std::unordered_set<Position>& set, const Position& pos)
{
	return (set.find(pos) != set.end());
}
#ifdef PART_1
inline static bool hasXMASAtPositionInDirection(Position startPos, Position direction);
#else
inline static bool hasXMASAtPosition(Position startPos);
#endif

void initialize([[maybe_unused]] uint64_t lineCount)
{
	// Empty, for now
}

// First store each relevant letters' position in different sets
bool handleLine(const std::string& line)
{
	//	std::cout << "Line: " << line << std::endl;
	static size_t s_lineCount = 0ull;
	for (size_t charPos = line.find_first_of("XMAS") ; charPos != std::string::npos ; charPos = line.find_first_of("XMAS", charPos+1u))
	{
		char c = line[charPos];
#ifdef PART_1
		if (c == 'X')
			s_XPositions.emplace(charPos, s_lineCount);
		else if (c == 'M')
			s_MPositions.emplace(charPos, s_lineCount);
#else // To make it not *that* cursed...
		if (c == 'M')
			s_MPositions.emplace(charPos, s_lineCount);
#endif
		else if (c == 'A')
			s_APositions.emplace(charPos, s_lineCount);
		else if (c == 'S')
			s_SPositions.emplace(charPos, s_lineCount);
	}
	s_lineCount++;
	return true;
}

// Then do the actual checks for all "XMAS"-strings by walking through each
// 'X'-position and trying to find the other letters in a specific direction
void finalize()
{
	size_t numberOfXMAS = 0ull;
#ifdef PART_1
	for (const Position& pos : s_XPositions)
	{
		//	std::cout << "Checking at 'X'-position [" << pos.x << ',' << pos.y << ']' << std::endl;
		// coordinate-system is like this:
		//     0---> X
		//     |
		//     v
		//     Y
		//
		numberOfXMAS +=
			  hasXMASAtPositionInDirection(pos, NORTH_WEST)
			+ hasXMASAtPositionInDirection(pos, WEST)
			+ hasXMASAtPositionInDirection(pos, SOUTH_WEST)
			+ hasXMASAtPositionInDirection(pos, SOUTH)
			+ hasXMASAtPositionInDirection(pos, SOUTH_EAST)
			+ hasXMASAtPositionInDirection(pos, EAST)
			+ hasXMASAtPositionInDirection(pos, NORTH_EAST)
			+ hasXMASAtPositionInDirection(pos, NORTH);
	}
#else
	for (const Position& pos : s_APositions)
	{
		numberOfXMAS += hasXMASAtPosition(pos);
	}
#endif
	std::cout << "The number of \"XMAS\"-strings found is " << numberOfXMAS << std::endl;
}

#ifdef PART_1
inline static bool hasXMASAtPositionInDirection(Position startPos, Position direction)
{
	// Make sure we don't underflow for negative-component directions
	if ((direction.x() == -1u && startPos.x() < 3u) ||
		(direction.y() == -1u && startPos.y() < 3u)
	)
	{
		return false;
	}

	Position xPos = startPos;
	Position mPos = xPos + direction;
	Position aPos = mPos + direction;
	Position sPos = aPos + direction;

	return (
		containsPos(s_XPositions, xPos) &&
		containsPos(s_MPositions, mPos) &&
		containsPos(s_APositions, aPos) &&
		containsPos(s_SPositions, sPos)
	);
}
#else
inline static bool hasXMASAtPosition(Position startPos)
{
	// Can't get this stuff at the left and top edge with my struct... wouldn't be possible anyway
	if (startPos.x() == 0u || startPos.y() == 0u || !containsPos(s_APositions, startPos))
		return false;
	
	Position posNW = startPos + NORTH_WEST;
	Position posNE = startPos + NORTH_EAST;
	Position posSW = startPos + SOUTH_WEST;
	Position posSE = startPos + SOUTH_EAST;
	
	bool hasFirstMAS = (
		(containsPos(s_MPositions, posNW) && containsPos(s_SPositions, posSE)) ||
		(containsPos(s_MPositions, posSE) && containsPos(s_SPositions, posNW))
	);
	bool hasSecondMAS = (
		(containsPos(s_MPositions, posNE) && containsPos(s_SPositions, posSW)) ||
		(containsPos(s_MPositions, posSW) && containsPos(s_SPositions, posNE))
	);

	return hasFirstMAS && hasSecondMAS;
}
#endif
