#include <AoC-Module.h>
#include <Vector2.hpp>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <array>

using Position = Vector2u32;

static Position s_area{};
static std::vector<Position> s_corruptedBytes{};

static uint64_t dijkstra(Position startPos, Position endPos, const std::unordered_set<Position>& corruptedBytes);

void initialize(uint64_t lineCount)
{
	s_corruptedBytes.reserve(lineCount-1u);
}

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	Position newPos;
	size_t commaPos{line.find(',')};
	{std::stringstream{} << line.substr(0,commaPos) >> newPos.x();}
	{std::stringstream{} << line.substr(commaPos+1u) >> newPos.y();}

	// Use "common sense" to check if we're working with the large input data or the small sample data
	// to determine the size of the area that we're in (7x7 for the sample, 71x71 for the input).
	if (s_area.x() == 0u && (newPos.x() > 6u || newPos.y() > 6u || s_corruptedBytes.size() >= (7u*7u)))
		s_area = Position{71u,71u};

	s_corruptedBytes.emplace_back(std::move(newPos));
	return true;
}

void finalize()
{
	// If the area's size was unset by now, then we must be dealing
	// with the small input area, so set its size to 7x7
	if (s_area.x() == 0u)
		s_area = Position{7u,7u};
	
	// Take the first 1024 (or 12 for the sample) bytes
	std::unordered_set<Position> part{};
	size_t partSize = 1024;
	if (s_area.x() == 7u)
		partSize = 12u;
	
	// Insert them to a set
	part.insert(s_corruptedBytes.begin(), s_corruptedBytes.begin() + std::min(partSize, s_corruptedBytes.size()));
	Position endPos = s_area - Position{1u,1u};
#ifdef PART_1
	// And run dijkstra with the newly created set of corrupted bytes
	uint64_t lowestSteps = dijkstra(Position{}, endPos, part);
	std::cout << "We need a minimum of " << lowestSteps << " steps to reach the end!" << std::endl;
#else
	// For part 2, continue where we last left off.
	// And to quote some memes: "My resolution? Air strikes. BOMB THEM! BOMB THEM! KEEP BOMBING THEM! BOMB THEM AGAIN! AND AGAIN!" (with bytes)
	for (auto iter = s_corruptedBytes.begin() + part.size() ; iter != s_corruptedBytes.end() ; iter++)
	{
		part.insert(*iter);
		if (dijkstra(Position{}, endPos, part) == 0u)
		{
			std::cout << "The corrupted byte that separated the area in half landed at " << *iter << '!' << std::endl;
			break;
		}
	}
#endif
}

const static std::array<Position, 4u> DIRECTIONS
{
	Position{1u,0u},
	Position{0u,1u},
	Position{-1u,0u},
	Position{0u,-1u}
};

using Direction = uint8_t;
struct QueueEntry
{
	Position pos;
	uint64_t steps;
};

static uint64_t dijkstra(Position startPos, Position endPos, const std::unordered_set<Position>& corruptedBytes)
{
	// Just plain ol' Dijkstra (says the dude that hadn't worked [successfully] with Dijkstra anytime besides two days ago for day 16)
	std::queue<QueueEntry> priorityQueue{};
	priorityQueue.push(QueueEntry{startPos, 0ull});

	// Just use a simple set to keep track of the fields we already visited
	std::unordered_set<Position> visited{};
	while (!priorityQueue.empty())
	{
		QueueEntry entry = priorityQueue.front();
		priorityQueue.pop();
		
		// If we've found the end, return the number of steps to get here
		if (entry.pos == endPos)
		{
			return entry.steps;
		}

		// If we were already at the current position, drop it, this path would defininetly be slower
		if (visited.find(entry.pos) != visited.end())
			continue;
		// Otherwise add it to the set
		visited.insert(entry.pos);

		// Now check all four cardinal directions and adding them to the queue...
		for (uint8_t i = 0u ; i < 4 ; i++)
		{
			Position newPos = entry.pos + DIRECTIONS[i];
			// ... as long as there isn't an obstacle there and the position is in-bounds
			if (!s_area.contains(newPos) || corruptedBytes.find(newPos) != corruptedBytes.end())
				continue;
			priorityQueue.push(QueueEntry{newPos, entry.steps+1u});
		}
	}

	// If we get here, no way had been found...
	return 0;
}
