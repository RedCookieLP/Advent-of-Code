#include <AoC-Module.h>
#include <Vector2.hpp>
#include <unordered_map>
#include <unordered_set>
#include <HashHelper.hpp>
#include <queue>
#include <array>
#include <sstream>
#include <cassert>

using Position = Vector2u32;

struct Cheat
{
	Cheat(const Position& startPos, const Position& endPos)
		: startPos{startPos}, endPos{endPos}
	{}
	inline bool operator==(const Cheat& other) const noexcept
	{
		return (startPos == other.startPos && endPos == other.endPos);
	}
	Position startPos{};
	Position endPos{};
};
template <>
struct std::hash<Cheat>
{
	inline size_t operator()(const Cheat& cheat) const noexcept
	{
		return Hash::combine(cheat.startPos, cheat.endPos);
	}
};

/*
Today's algorithm is going to be interesting.

To solve the puzzle for part 1 (and hopefully part 2 as well), I'll do the following steps:
	1. Flood-fill the maze beginning from the end to get the distances from every field to the end
	2. Store the distance from the start to the end (since the description states that
	   "there's only one path to the end", this check is trivially easy), which will be done during the flood-fill
	3. Follow the entire path from the start:
	   - If any chance arises for a cheat (aka. passing through a single wall), simulate the cheat
	   - on the other side of the wall: compare the simulated distance which we have now (distance on trivial path - 2) with
	     the previously determined distance from the new path to the end.
		 If the distance is now less, then this is a valid cheat
*/

static void floodFillMaze(std::unordered_map<Position, uint64_t>& mazeTracks, const Position& floodStartPos);
static void printFloodMap(const std::unordered_map<Position, uint64_t>& mazeTracks, const Position& startPos, const Position& endPos, const Position& area);
static void findPossibleCheats(const std::unordered_map<Position, uint64_t>& mazeTracks, const Position& startPos, uint64_t minimumTimeSave, std::unordered_map<Cheat, uint64_t>& possibleCheats);

static Position s_area;
static Position s_startPos;
static Position s_endPos;
static std::unordered_map<Position, uint64_t> s_mazeTracks;
static const std::array<Position, 4u> DIRECTIONS
{
	Position{1u,0u},
	Position{0u,1u},
	Position{-1u,0u},
	Position{0u,-1u}
};

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	if (s_area.x() == 0u)
		s_area.x() = line.length();

	static uint32_t yPos = 0u;
	for (size_t xPos = line.find_first_of(".SE") ; xPos != line.npos ; xPos = line.find_first_of(".SE", xPos+1u))
	{
		Position newPos{static_cast<uint32_t>(xPos), yPos};
		if (line[xPos] == 'E')
			s_endPos = newPos;
		else if (line[xPos] == 'S')
			s_startPos = newPos;
		s_mazeTracks.emplace(std::move(newPos), UINT64_MAX);
	}
	yPos++;
	s_area.y() = yPos;
	return true;
}

void finalize()
{
	// First, flood-fill the maze to get all distances
	floodFillMaze(s_mazeTracks, s_endPos);

	// Now check if we've got a path from start to end.
	// According to the puzzle's description, there should *always be exactly one*
	uint64_t maxDistance = s_mazeTracks.at(s_startPos);
	if (maxDistance == UINT64_MAX)
	{
		std::cerr << "Something doesn't add up: During the flood-fill, no path to the start had been found!" << std::endl;
		return;
	}

	// Now that we've successfully flooded the maze, we can trace our trivial path from the start to the end.
	// This will later be used to determine a) possible cheats and b) viable cheats
	
	// Debugging (and pretty at that...)
	//	printFloodMap(s_mazeTracks, s_startPos, s_endPos, s_area);
	
	std::unordered_map<Cheat, uint64_t> possibleCheats{};
	findPossibleCheats(s_mazeTracks, s_startPos, 100ull, possibleCheats);
	std::cout << "The number of cheats which would save at least 100 picoseconds is " << possibleCheats.size() << std::endl;
}

struct QueueElement
{
	QueueElement(const Position& pos, uint64_t distance)
		: pos{pos}, distance{distance}
	{}
	Position pos;
	uint64_t distance;
};

static void floodFillMaze(std::unordered_map<Position, uint64_t>& mazeTracks, const Position& floodStartPos)
{
	std::queue<QueueElement> floodQueue{};
	floodQueue.emplace(floodStartPos, 0u);
	while (!floodQueue.empty())
	{
		QueueElement floodElement = floodQueue.front();
		floodQueue.pop();

		auto iter = mazeTracks.find(floodElement.pos);
		if (iter == mazeTracks.end() || iter->second < floodElement.distance)
			continue;
		iter->second = floodElement.distance;

		for (const Position& dir : DIRECTIONS)
		{
			Position newPos{floodElement.pos + dir};
			if (mazeTracks.find(newPos) == mazeTracks.end())
				continue;
			floodQueue.emplace(std::move(newPos), floodElement.distance+1u);
		}
	}
}

static void printFloodMap(const std::unordered_map<Position, uint64_t>& mazeTracks, const Position& startPos, const Position& endPos, const Position& area)
{
	std::stringstream ss{};
	for (uint32_t yPos = 0u ; yPos < area.y() ; yPos++)
	{
		for (uint32_t xPos = 0u ; xPos < area.x() ; xPos++)
		{
			Position pos{xPos, yPos};
			if (pos == startPos)
				ss << "\x1B[1;92mS";
			else if (pos == endPos)
				ss << "\x1B[1;91mE";
			else if (mazeTracks.find(pos) == mazeTracks.end())
				ss << "\x1B[1;90m#";
			else
				ss << "\x1B[1;97m" << char('0' + (mazeTracks.at(pos) % 10u));
			ss << "\x1B[0m";
		}
		ss << std::endl;
	}
	std::cout << ss.str() << std::endl;
}

static void findLocalCheats(const std::unordered_map<Position, uint64_t>& mazeTracks, const Position& pos, uint64_t minimumTimeSave, std::unordered_map<Cheat, uint64_t>& possibleCheats);
static void findPossibleCheats(const std::unordered_map<Position, uint64_t>& mazeTracks, const Position& startPos, uint64_t minimumTimeSave, std::unordered_map<Cheat, uint64_t>& possibleCheats)
{
	// Walk the path from the given position `startPos` along to the end.
	// On each step:
	//  - Check all four cardinal directions for a possible cheat (meaning first a wall and then a path)
	//  - If a cheat has been found, walk it and check if it would result in a time-save
	//  - If it's a time-save, store it in the set
	possibleCheats.clear();
	for (Position pos = startPos ; mazeTracks.at(pos) > 0u ; )
	{
		// Determine all possible cheats at the current position
		findLocalCheats(mazeTracks, pos, minimumTimeSave, possibleCheats);
		// To find the next path, look for the tile with a distance one less then ours around ourselves
		for (const Position& dir : DIRECTIONS)
		{
			auto mazeIter = mazeTracks.find(pos + dir);
			if (mazeIter == mazeTracks.end() || mazeIter->second != mazeTracks.at(pos)-1u)
				continue;
			pos = mazeIter->first;
			break;
		}
	}
}

static void findLocalCheats(const std::unordered_map<Position, uint64_t>& mazeTracks, const Position& pos, uint64_t minimumTimeSave, std::unordered_map<Cheat, uint64_t>& possibleCheats)
{
#ifdef PART_1
	static constexpr int32_t MAX_DISTANCE = 2;
#else
	static constexpr int32_t MAX_DISTANCE = 20;
#endif

	uint64_t distanceAtPos = mazeTracks.at(pos);
	for (int32_t yPos = -MAX_DISTANCE ; yPos <= MAX_DISTANCE ; yPos++)
	{
		int32_t xDeviation = MAX_DISTANCE - std::abs(yPos);
		for (int32_t xPos = -xDeviation ; xPos <= xDeviation ; xPos++)
		{
			if (yPos == 0 && xPos == 0)
				continue;
			
#ifdef PART_1
			if (yPos != 0 && xPos != 0)
				continue;
#endif
			
			// This should result in a blocky-circle
			Position cheatPos{static_cast<uint32_t>(xPos), static_cast<uint32_t>(yPos)};
			cheatPos += pos;

			// Check if the cheat-position is on a valid path. If not, skip
			auto trackIter = mazeTracks.find(cheatPos);
			if (trackIter == mazeTracks.end())
				continue;
			
			// Get the length of the cheat. This is just the manhattan-distance of out current delta
			uint64_t cheatLength = std::abs(xPos) + std::abs(yPos);

			// If our cheat is longer than our distance to the end, then the cheat would make our path unnecessarily longer, so drop it
			if (distanceAtPos < cheatLength)
				continue;
			
			// Now check if this cheat isn't already known and also if it even results in a time-save.
			// If one of these doesn't fit, then the cheat is useless, so drop it
			Cheat newCheat{pos, cheatPos};
			uint64_t cheatDistance = (distanceAtPos - cheatLength);
			if (trackIter->second >= cheatDistance || possibleCheats.find(newCheat) != possibleCheats.end())
				continue;
			
			// Finally, check if the cheat's time-save is at least as much as our minimum time-save
			uint64_t timeSave = cheatDistance - trackIter->second;
			if (timeSave < minimumTimeSave)
				continue;
			
			// Otherwise add it to our set of possible cheats
			possibleCheats.emplace(std::move(newCheat), timeSave);
		}
	}
}
