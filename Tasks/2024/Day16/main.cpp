#include <AoC-Module.h>
#include <vector>
#include <Vector2.hpp>
#include <unordered_map>
#include <unordered_set>
#include <array>

using Position = Vector2u32;

struct Intersection
{
	std::array<Position, 4u> neighbours;
	Position pos;
};

static Position s_startPos{};
static Position s_endPos{};
static std::unordered_map<Position, Intersection> s_intersections{};

static std::vector<std::pair<bool, Position>> turnHeavyDijkstra(const Position& startPos, const Position& endPos);

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	// How would I optimally parse this to get a map of all paths?...
}

void finalize()
{
	std::vector<std::pair<bool, Position>> bestPath = turnHeavyDijkstra(s_startPos, s_endPos);
	uint64_t deerScore = 0ull;
	Position lastPos = s_startPos;
	for (const auto& [mustTurn, newPos] : bestPath)
	{
		deerScore += (mustTurn ? 1000ull : 0ull) + (newPos - lastPos).len<uint64_t>() + 1u;
	}
	std::cout << "The best score the deer could achieve is " << deerScore << std::endl;
}

static std::vector<std::pair<bool, Position>> turnHeavyDijkstra(const Position& startPos, const Position& endPos)
{
	// TODO: the rest
}
