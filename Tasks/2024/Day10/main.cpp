#include <AoC-Module.h>
#include <array>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <Vector2.hpp>

using Position = Vector2u32;

// Stores all positions of a specific height
static std::array<std::unordered_set<Position>, 10u> s_heightMap;
static Position s_area;
const static Position DIRECTIONS[4]
{
	{0u,-1u},	// North
	{1u,0u},	// East
	{0u,1u},	// South
	{-1u,0u}	// West
};

// Returns how many *unique* peaks are reachable from this starting position
#ifdef PART_1
static uint32_t estimateTrailheadScore(const Position& startingPos);
#else
static uint32_t estimateTrailheadRating(const Position& startingPos);
#endif

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	if (s_area.x() == 0u)
		s_area.x() = line.length();
	
	static uint32_t yPos{0u};
	
	for (uint32_t xPos = 0u ; xPos < line.length() ; xPos++)
	{
		// We'll just assert that all the characters are numbers from 0 to 9...
		s_heightMap[line[xPos] - '0'].emplace(xPos, yPos);
	}

	yPos++;
	s_area.y() = yPos;
	return true;
}

void finalize()
{
	uint32_t trailheadScoreSum = 0u;
	for (const Position& headPos : s_heightMap[0])
	{
#ifdef PART_1
		trailheadScoreSum += estimateTrailheadScore(headPos);
#else
		trailheadScoreSum += estimateTrailheadRating(headPos);
#endif
	}
	std::cout << "The sum of all trailheads is " << trailheadScoreSum << std::endl;
}

#ifdef PART_1
static void hikeToPeak(const Position& pos, uint8_t height, std::unordered_set<Position>& peakHistory)
{
	// First check if we're even in a valid spot (pos- and height-wise).
	// If not, we're done
	if (s_heightMap[height].find(pos) == s_heightMap[height].end())
		return;
	
	// If we're at the peak, add our position to the history (if not already present)
	if (height == 9)
	{
		peakHistory.insert(pos);
		return;
	}

	// Otherwise, check all the other four directions (north, east, south and west)
	for (uint8_t dir = 0u ; dir < 4u ; dir++)
	{
		hikeToPeak(pos+DIRECTIONS[dir], height+1u, peakHistory);
	}
}
static uint32_t estimateTrailheadScore(const Position& trailHeadPos)
{
	std::unordered_set<Position> reachablePeaks{};
	hikeToPeak(trailHeadPos, 0u, reachablePeaks);
	return reachablePeaks.size();
}
#else
static void traceToPeak(const Position& pos, uint8_t height, std::unordered_set<std::string>& trailHistory, std::array<Position, 10u>& trail)
{
	// First check if we're even in a valid spot (pos- and height-wise).
	// If not, we're done
	if (s_heightMap[height].find(pos) == s_heightMap[height].end())
		return;
	
	// Then add ourselves to the trailHistory (I was here!)
	trail[height] = pos;
	
	// If we're at the peak, build a literal "hash string" of
	// all Positions of our trail and add it to the trailHistory
	if (height == 9u)
	{
		std::hash<Position> posHasher{};
		std::stringstream ss;
		for (uint8_t i = 0u ; i < trail.size() ; i++)
		{
			ss << std::to_string(posHasher(trail[i]));
		}
		trailHistory.insert(ss.str());
		return;
	}

	// Otherwise, check all the other four directions (north, east, south and west)
	for (uint8_t dir = 0u ; dir < 4u ; dir++)
	{
		traceToPeak(pos+DIRECTIONS[dir], height+1u, trailHistory, trail);
	}
}
static uint32_t estimateTrailheadRating(const Position& startingPos)
{
	// We do something a bit cursed here: We walk every possible trail,
	// but once we reach the top, we concatenate all `Position`-hashes in a string
	// and then store *it* in the unordered_set.
	std::unordered_set<std::string> trailHashes{};
	std::array<Position, 10u> trail;
	traceToPeak(startingPos, 0u, trailHashes, trail);
	return trailHashes.size();
}
#endif
