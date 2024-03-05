#include <AoC-Module.h>
#include <vector>
#include <algorithm>
#include <unordered_map>

struct Position
{
	int32_t x;
	int32_t y;
	Position operator+(const Position& other) const { return { x + other.x, y + other.y }; }
	Position operator-(const Position& other) const { return { x - other.x, y - other.y }; }
	Position operator*(const Position& other) const { return { x * other.x, y * other.y}; }
	bool operator==(const Position& other) const { return (x == other.x && y == other.y); }
	bool operator!=(const Position& other) const { return !(*this == other); }
};

using state_type = std::vector<Position>;

template <>
struct std::hash<Position>
{
	size_t operator()(const Position& pos) const
	{
		return (int64_t(std::hash<int32_t>{}(pos.x)) << sizeof(int32_t)) | int64_t(std::hash<int32_t>{}(pos.y));
	}
};
template <>
struct std::hash<state_type>
{
	size_t operator()(const state_type& val) const
	{
		std::size_t seed = val.size();
		for(auto x : val) {
			x.x = ((x.x >> 16) ^ x.x) * 0x45d9f3b;
			x.x = ((x.y >> 16) ^ x.y) * 0x45d9f3b;
			x.y = (x.x >> 16) ^ x.y;
			seed ^= x.x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};

static void rollSphere(Position& sphere, Position rollDirection);

constexpr static const Position NORTH{0, -1};
constexpr static const Position WEST{-1, 0};
constexpr static const Position SOUTH{0, 1};
constexpr static const Position EAST{1, 0};

static std::vector<Position> s_cubes;
static std::vector<Position> s_spheres;
static std::vector<Position*> s_allObjects;

// mini-state-map
// prevents multiple caluclations of the same transitions, in case a cycle repeats 1-to-1
static std::unordered_map<size_t, state_type> s_cycleStates;

static size_t s_platformDepth = 0;

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	for (size_t i = 0 ; i < line.length() ; i++)
	{
		char c = line[i];
		if (c == '#' || c == 'O')
		{
			Position newPos = Position{ static_cast<int32_t>(i), static_cast<int32_t>(s_platformDepth) };
			if (c == '#')
				s_cubes.push_back(newPos);
			else
				s_spheres.push_back(newPos);
		}
	}

	s_platformDepth++;
	return true;
}

#ifndef DO_PART_1
#	define CYCLES 1'000'000'000
#	define CYCLE_PRINT_INTERVAL 1'000'000
#	define CYCLE_SUB_INTERVAL 500'000
#endif // ~DO_PART_1

void finalize()
{
	for (Position& pos : s_cubes)
		s_allObjects.push_back(&pos);
	for (Position& pos : s_spheres)
		s_allObjects.push_back(&pos);

	size_t totalLoad = 0;
#ifndef DO_PART_1
	for (size_t cycle = 0 ; cycle < CYCLES ; cycle++)
	{
	if (((cycle+1) % CYCLE_PRINT_INTERVAL) == 0)
	{
		std::cout << "Current interval: " << cycle+1 << std::endl;
	}
	auto stateKey = std::hash<state_type>{}(s_spheres);
	auto iter = s_cycleStates.find(stateKey);
	if (iter != s_cycleStates.end())
	{
		if (((cycle+1) % CYCLE_SUB_INTERVAL) == 0)
			std::cout << "HIT!" << std::endl;
		s_spheres = iter->second;
		continue;
	}
	state_type spheresBackup = s_spheres;
#endif // ~DO_PART_1
	for (Position& pos : s_spheres)
	{
		//	Position oldPos = pos;
		rollSphere(pos, NORTH);
#ifdef DO_PART_1
		totalLoad += (s_platformDepth - pos.y);
#else // DO_PART_1
		rollSphere(pos, WEST);
		rollSphere(pos, SOUTH);
		rollSphere(pos, EAST);
#endif // ~DO_PART_1
		//	std::cout << "Sphere from [" << oldPos.x << ", " << oldPos.y << "] rolls to [" << pos.x << ", " << pos.y << ']' << std::endl;
	}
#ifndef DO_PART_1
	s_cycleStates[std::hash<state_type>{}(spheresBackup)] = s_spheres;
	}
	for (const Position& pos : s_spheres)
		totalLoad += (s_platformDepth - pos.y);
#endif // ~DO_PART_1
	std::cout << "The total load on the north side (after every sphere rolled north) is " << totalLoad << '!' << std::endl;
}

static bool positionYLessThan(const Position& left, const Position& right) { return left.y < right.y; }
static bool positionXLessThan(const Position& left, const Position& right) { return left.x < right.x; }
static bool positionYGreaterThan(const Position& left, const Position& right) { return left.y > right.y; }
static bool positionXGreaterThan(const Position& left, const Position& right) { return left.x > right.x; }

static bool positionXEqual(const Position& left, const Position& right) { return left.x == right.x; }
static bool positionYEqual(const Position& left, const Position& right) { return left.y == right.y; }

static void rollSphere(Position& sphere, Position rollDirection)
{
	bool(*filterFunc)(const Position&, const Position&);
	if (rollDirection == NORTH || rollDirection == SOUTH)
		filterFunc = positionXEqual;
	else
		filterFunc = positionYEqual;
	
	std::vector<Position> obstacles;
	for (const Position* pos : s_allObjects)
	{
		if (filterFunc(*pos, sphere))
			obstacles.push_back(*pos);
	}
	
	bool(*compFunc)(const Position&, const Position&) = nullptr;
	if (rollDirection == NORTH)
		compFunc = positionYLessThan;
	else if (rollDirection == WEST)
		compFunc = positionXLessThan;
	else if (rollDirection == SOUTH)
		compFunc = positionYGreaterThan;
	else if (rollDirection == EAST)
		compFunc = positionXGreaterThan;
	
	if (!compFunc)
		return;
	
	std::sort(obstacles.begin(), obstacles.end(), compFunc);
	
	auto iter = obstacles.cbegin();
	for ( ; compFunc(*iter, sphere) && iter != obstacles.cend() ; iter++);

	if (iter == obstacles.cbegin())
	{
		sphere = sphere + (sphere * rollDirection);
	}
	else
	{
		Position finPos = *(--iter);
		sphere = std::min(sphere, finPos - rollDirection, compFunc);
	}
}
