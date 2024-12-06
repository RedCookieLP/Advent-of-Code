#include <AoC-Module.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <thread>
#include <sstream>
#include <unordered_set>

struct Position
{
	Position()
		: x{0}, y{0}
	{}
	Position(uint32_t x, uint32_t y)
		: x{x}, y{y}
	{}
	inline bool operator==(const Position& other) const noexcept
	{
		return (x == other.x && y == other.y);
	}
	inline Position operator+(const Position& other) const noexcept
	{
		return Position{x + other.x, y + other.y};
	}
	inline bool operator>(const Position& other) const noexcept
	{
		// Positions are ordered first by y, then by x
		return ((y > other.y) || (y == other.y && x > other.x));
	}
	inline bool operator<(const Position& other) const noexcept
	{
		return !((*this == other) || (*this > other));
	}
	uint32_t x;
	uint32_t y;
};
template <>
struct std::hash<Position>
{
	inline size_t operator()(const Position& obj) const noexcept
	{
		return std::hash<uint64_t>{}((static_cast<uint64_t>(obj.x) << 32ull) | static_cast<uint64_t>(obj.y));
	}
};

using Direction = uint8_t;
struct GuardTile
{
	GuardTile()
		: pos{}, dir{0u}
	{}
	GuardTile(const Position& pos, Direction dir)
		: pos{pos}, dir{dir}
	{}
	inline bool operator==(const GuardTile& other) const noexcept
	{
		return (dir == other.dir && pos == other.pos);
	}
	Position pos;
	Direction dir : 2u;
};
template <>
struct std::hash<GuardTile>
{
	inline size_t operator()(const GuardTile& obj) const noexcept
	{
		size_t posHash = std::hash<Position>{}(obj.pos);
		return (posHash ^ (std::hash<uint8_t>{}(obj.dir) + 0x9e3779b9ull + (posHash << 6ull) + (posHash >> 2ull)));
	}
};



// Using `unordered_set` for just knowing if we've already got a specific position
static std::unordered_set<Position> s_obstaclePositions;
static GuardTile s_guard;
static Position s_area{};
const static Position DIRECTIONS[4]
{
	{0u,-1u},	// North
	{1u,0u},	// East
	{0u,1u},	// South
	{-1u,0u}	// West
};
/**
 * @brief Let a guard patrol an area with obstacles
 * @param[in] area: Width and height of the area to patrol
 * @param[in] guard: Starting position and orientation of the guard
 * @param[in] obstacles: Positions of all the obstacles in the area
 * @param[out] guardPath: A vector containing the path taken by the guard
 * @return If the guard doesn't loop, the number of unique fields covered by the guard is returned, otherwise -1.
 * @note If the area is 0 in any dimension, 0 is returned. If the area is just 1x1, 1 is returned.
 */
static uint32_t patrolArea(const Position& area, GuardTile guard, const std::unordered_set<Position>& obstacles, std::vector<GuardTile>& guardPath) noexcept;



bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	if (s_area.x == 0u)
		s_area.x = line.length();
	
	static uint32_t lineIndex{0u};
	for (size_t charPos = line.find_first_of("#^") ; charPos != line.npos ; charPos = line.find_first_of("#^", charPos+1u))
	{
		Position pos{static_cast<uint32_t>(charPos), lineIndex};
		char ch = line.at(charPos);
		if (ch == '#')
			s_obstaclePositions.insert(std::move(pos));
		else
			s_guard = GuardTile{pos, 0};
	}
	s_area.y = ++lineIndex;
	return true;
}

void finalize()
{
	std::vector<GuardTile> guardPath{};
#ifdef PART_1
	uint32_t fields = patrolArea(s_area, s_guard, s_obstaclePositions, guardPath);
	std::cout << "The guard currently patrols " << fields << " unique fields in the [" << s_area.x << 'x' << s_area.y << "] area!" << std::endl;
#else
	// First, get the regular path of the guard. We can only intercept him
	// on this path, since he wouldn't run into our crate otherwise
	patrolArea(s_area, s_guard, s_obstaclePositions, guardPath);

	// Now check the entire path from before, but place a crate at every point
	// where the guard will run into another obstacle (for optimization, otherwise he would just run off and be free).
	// 
	// Since we aren't allowed to place a crate right in front of the guard,
	// we must explicitly check against that spot
	std::unordered_set<Position> possiblePositions{};

	for (auto& guard : guardPath)
	{
		// First check if the crate would be at the position of an already existing obstacle.
		// If so, *simulate a crate on the right from the guard*
		Position cratePos = guard.pos + DIRECTIONS[guard.dir];
		if (s_obstaclePositions.find(cratePos) != s_obstaclePositions.end())
		{
			cratePos = guard.pos + DIRECTIONS[(guard.dir+1u)%4u];
		}

		// Then check if the crate is at the guard's position **OR** if it's out of bounds
		if (cratePos == s_guard.pos || cratePos.x > s_area.x || cratePos.y > s_area.y)
		{
			continue;
		}

		// Now check if we already confirmed this crate position.
		// If so, continue on;
		if (possiblePositions.find(cratePos) != possiblePositions.end())
		{
			continue;
		}

		// Otherwise check if this would make him loop.
		// If so, add it to the possibilities
		std::unordered_set<Position> obstacles = s_obstaclePositions;
		obstacles.insert(cratePos);
		std::vector<GuardTile> temp{};
		if (patrolArea(s_area, s_guard, obstacles, temp) == -1u)
		{
			possiblePositions.insert(cratePos);
		}
	}

	std::cout << "There are " << possiblePositions.size() << " possibilities to make the guard loop using one crate" << std::endl;
#endif
}

static uint32_t patrolArea(const Position& area, GuardTile guard, const std::unordered_set<Position>& obstacles, std::vector<GuardTile>& guardPath) noexcept
{
	std::unordered_map<Position, Direction> pathHistory{};
	size_t fieldCount = 0ull;
	for ( ; guard.pos.x < area.x && guard.pos.y < area.y ; guard.pos = (guard.pos + DIRECTIONS[guard.dir]))
	{
		guardPath.push_back(guard);
		// 1. Check if our current position is unique
		//	- If so, increment the fieldCount and add our state to the path
		auto iter = pathHistory.find(guard.pos);
		if (iter == pathHistory.end())
		{
			fieldCount++;
			Direction dir = guard.dir;
			pathHistory.emplace(guard.pos, dir);
		}
		else
		{
			// 2. Check if we're looping (same direction)
			//	- If so, return -1
			if (iter->second == guard.dir)
			{
				return -1u;
			}
		}
		// 3. Check if we're in front of an obstacle
		//	- If so, turn right until we aren't anymore
		for (size_t spinCount = 0u ; true ; spinCount++)
		{
			// Failsafe: we're somehow trapped...
			if (spinCount == 4u)
			{
				std::cerr << "HELP! I'M SOMEHOW TRAPPED!" << std::endl;
				return fieldCount;
			}
			Position newPos = guard.pos + DIRECTIONS[guard.dir];
			if (obstacles.find(newPos) == obstacles.end())
				break;
			guard.dir++;
		}
	}
	return fieldCount;
}
