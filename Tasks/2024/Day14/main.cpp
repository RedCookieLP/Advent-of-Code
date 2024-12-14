#include <AoC-Module.h>
#include <Vector2.hpp>
#include <vector>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <array>
#ifndef PART_1
#include <thread>
#include <chrono>
#endif

using Vec2 = Vector2i32;

struct Robot
{
	Robot(Vec2 pos, Vec2 vel)
		: position{pos}, velocity{vel}
	{}
	Vec2 position{};
	Vec2 velocity{};
};

#ifdef PART_1
constexpr static uint64_t DURATION_S = 100ull;
#else
constexpr static uint64_t MAX_ITERATIONS = 10000ull;
#endif
const static Vec2 AREA{ 101, 103 }; 
static std::vector<Robot> s_robots;

static inline void printArea(const std::unordered_map<Vec2, uint32_t>& robotLocations)
{
	for (int y = 0 ; y < AREA.y() ; y++)
	{
		for (int x = 0 ; x < AREA.x() ; x++)
		{
			auto iter = robotLocations.find(Vec2{x,y});
			std::cout << (iter != robotLocations.end() ? std::to_string(iter->second) : ".");
		}
		std::cout << std::endl;
	}
}

void initialize(uint64_t lineCount)
{
	s_robots.reserve(lineCount-1);
}

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	static const std::regex LINE_REGEX{ R"(^p=(-?\d+),(-?\d+)\s+?v=(-?\d+),(-?\d+)$)" };
	
	std::smatch strMatch;
	if (!std::regex_match(line, strMatch, LINE_REGEX))
	{
		std::cerr << "Line \"" << line << "\" doesn't match regex!" << std::endl;
		return false;
	}

	int64_t posX, posY, velX, velY;
	{std::stringstream{} << strMatch[1] >> posX;}
	{std::stringstream{} << strMatch[2] >> posY;}
	{std::stringstream{} << strMatch[3] >> velX;}
	{std::stringstream{} << strMatch[4] >> velY;}
	s_robots.emplace_back(Vec2{posX, posY}, Vec2{velX, velY});

	return true;
}

void finalize()
{
#ifndef PART_1
	for (uint32_t iteration = 0u ; iteration < MAX_ITERATIONS ; iteration++)
	{
#endif
	std::unordered_map<Vec2, uint32_t> robotsPerTile{};
	// First, simulate each robot
	for (Robot& robot : s_robots)
	{
#ifdef PART_1
		Vec2 newPos = robot.position + (robot.velocity * DURATION_S);
#else
		Vec2 newPos = robot.position + robot.velocity;
#endif

		newPos.x() %= AREA.x();
		if (newPos.x() < 0)
			newPos.x() += AREA.x();
		
		newPos.y() %= AREA.y();
		if (newPos.y() < 0)
			newPos.y() += AREA.y();
		
		robot.position = newPos;
		robotsPerTile[newPos]++;
	}

#ifndef PART_1
	// Try to detect a line of ten consecutive robots, which might be the christmas tree we're looking for
	bool foundLine{false};
	for (const auto& [position, count] : robotsPerTile)
	{
		uint8_t consecutiveRobotCount = 1u;
		static const Vec2 EAST{1u,0u};
		for (Vec2 pos = position + EAST ; consecutiveRobotCount < 10 ; pos += EAST, consecutiveRobotCount++)
		{
			if (robotsPerTile.find(pos) == robotsPerTile.end())
				break;
		}
		if (consecutiveRobotCount == 10u)
		{
			foundLine = true;
			break;	
		}
	}

	// If we didn't find a line, drop the candidate
	if (!foundLine)
		continue;

	// Otherwise print it out for manual inspection
	std::cout << "Possible candidate found after " << (iteration+1u) << " seconds:" << std::endl;
	printArea(robotsPerTile);

	}
#else
	std::cout << "After " << DURATION_S << " seconds, the robots are scattered like this:" << std::endl;
	printArea(robotsPerTile);

	static const std::array<Vec2,4u> QUADRANT_CORNERS
	{
		Vec2{(AREA.x())/2, (AREA.y())/2},
		Vec2{AREA.x(), (AREA.y())/2},
		Vec2{(AREA.x())/2, AREA.y()},
		Vec2{AREA.x(), AREA.y()}
	};

	// Time to determine all quadrant's robots
	std::array<uint64_t,4u> quadrantRobotCounts{};
	for (auto iter = robotsPerTile.cbegin() ; iter != robotsPerTile.cend() ; )
	{
		const auto& [robotPos, count] = *iter;
		// Check if they're on the border of all quadrants. If so, skip them
		// (Check against the first quadrant corner since it's right in the middle of the area...)
		if (robotPos.x() == QUADRANT_CORNERS[0].x() || robotPos.y() == QUADRANT_CORNERS[0].y())
		{
			iter = robotsPerTile.erase(iter);
			continue;
		}
		for (size_t i = 0u ; i < 4u ; i++)
		{
			if (QUADRANT_CORNERS[i].contains(robotPos))
			{
				quadrantRobotCounts[i] += count;
				iter = robotsPerTile.erase(iter);
				break;
			}
		}
	}

	uint64_t safetyFactor{1ull};
	for (size_t i = 0u ; i < 4u ; i++)
	{
		std::cout << "Number of robots in quadrant " << (i+1) << ": " << quadrantRobotCounts[i] << std::endl;
		safetyFactor *= quadrantRobotCounts[i];
	}

	std::cout << "The safety factor for the entire area is " << safetyFactor << std::endl;
#endif
}
