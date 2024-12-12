#include <AoC-Module.h>
#include <Vector2.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include <stack>

using Position = Vector2u32;

struct Region
{
	std::unordered_set<Position> plots{};
#ifdef PART_1
	uint64_t perimeter{0ull};
#else
	uint64_t uniqueSides{0ull};
#endif
};

static std::unordered_map<char, std::unordered_set<Position>> s_plots;
static const std::array<Position, 4u> DIRECTIONS =
{
	Position{0u,-1u},	// North = bit 0
	Position{1u,0u},	// East = bit 1
	Position{0u,1u},	// South = bit 2
	Position{-1u,0u}	// West = bit 3
};

static void extractRegion(std::vector<Region>& regions, std::unordered_set<Position> plots) noexcept;
static Region floodFillExtract(const Position& startPos, std::unordered_set<Position>& plots) noexcept;
#ifdef PART_1
static uint64_t calculatePerimeter(const Region& region) noexcept;
#else
static uint64_t calculateUniqueSides(const Region& region) noexcept;
#endif

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	static uint32_t yPos{0u};
	for (uint32_t xPos = 0u ; xPos < line.length() ; xPos++)
	{
		s_plots[line[xPos]].emplace(xPos, yPos);
	}
	yPos++;
	return true;
}

void finalize()
{
	uint64_t cost = 0u;
	// First build the areas from the top down
	for (const auto& [type, plotPosSet] : s_plots)
	{
		std::vector<Region> regionVec{};
		extractRegion(regionVec, plotPosSet);
		for (const Region& region : regionVec)
		{
#ifdef PART_1
			std::cout << "Region for '" << type << "' with perimeter " << region.perimeter << " and area " << region.plots.size() << std::endl;
			cost += (region.perimeter * region.plots.size());
#else
			//	std::cout << "Region for '" << type << "' with unique-sides " << region.uniqueSides << " and area " << region.plots.size() << std::endl;
			cost += (region.uniqueSides * region.plots.size());
#endif
		}
	}
	std::cout << "The total cost for all fences is " << cost << std::endl;
}

static void extractRegion(std::vector<Region>& regions, std::unordered_set<Position> plots) noexcept
{
	// To extract each region, do the following
	// - start with the first position in the set
	// - do a flood-fill until there's nothing else to find
	// - add all the plots flooded by the flood-fill to a new region
	// and repeat until all plots had been flooded

	while (!plots.empty())
	{
		// start with the first position
		Position startPos = *plots.begin();

		// do a flood-fill until there's nothing else to find
		Region newRegion = floodFillExtract(startPos, plots);
#ifdef PART_1
		newRegion.perimeter = calculatePerimeter(newRegion);
#else
		newRegion.uniqueSides = calculateUniqueSides(newRegion);
#endif
		regions.emplace_back(std::move(newRegion));
	}
}

static Region floodFillExtract(const Position& startPos, std::unordered_set<Position>& plots) noexcept
{
	Region newRegion{};
	std::stack<Position> stack{};
	stack.push(startPos);
	while (!stack.empty())
	{
		Position pos = stack.top();
		stack.pop();
		if (plots.find(pos) == plots.end())
			continue;
		plots.erase(pos);
		if (newRegion.plots.find(pos) != newRegion.plots.end())
			continue;
		newRegion.plots.insert(pos);
		for (uint8_t i = 0 ; i < 4 ; i++)
			stack.push(pos + DIRECTIONS[i]);
	}
	return newRegion;
}

#ifdef PART_1
static uint64_t calculatePerimeter(const Region& region) noexcept
{
	uint64_t perimeter = 0ull;
	for (const Position& pos : region.plots)
	{
		for (uint8_t i = 0u ; i < 4u ; i++)
			perimeter += (region.plots.find(pos + DIRECTIONS[i]) == region.plots.end());
	}
	return perimeter;
}
#else
static uint64_t calculateUniqueSides(const Region& region) noexcept
{
	// SPOILER: This is messy, but fast! (got the solution in 0.037ms on my machine with input.txt)

	// First determine all fence-locations
	// (Also determine the area the region takes up, helps with limiting to the minimum)
	Position cornerTL{-1u,-1u};
	Position cornerBR{0u,0u};

	std::unordered_map<Position, uint8_t/*fence-mask*/> fences;
	for (const Position& pos : region.plots)
	{
		// Home in on the size of the area, for less iterations later on
		if (pos.x() < cornerTL.x())
			cornerTL.x() = pos.x();
		if (pos.y() < cornerTL.y())
			cornerTL.y() = pos.y();
		if (pos.x() > cornerBR.x())
			cornerBR.x() = pos.x();
		if (pos.y() > cornerBR.y())
			cornerBR.y() = pos.y();
		
		// Add all the fence-configurations of this region to the "fences"-map
		// Encode the fence-configuration as a bit-mask with the values set like: 0b0000WSEN
		for (const Position& pos : region.plots)
		{
			for (uint8_t i = 0u ; i < 4u ; i++)
			{
				// A side without a fence is useless to us, so skip it
				if (region.plots.find(pos + DIRECTIONS[i]) != region.plots.end())
					continue;
				fences[pos] |= (1u << i);
			}
		}
	}

	// These are the bits for the fence configuration to tell if a position has a fence in a specific orientation
	constexpr uint8_t NORTH_BIT = 0b0001u;
	constexpr uint8_t EAST_BIT  = 0b0010u;
	constexpr uint8_t SOUTH_BIT = 0b0100u;
	constexpr uint8_t WEST_BIT  = 0b1000u;

	// Now try to find as many segments as possible.
	// We do this by first by iterating left to right and counting how many beningings for a segment we got
	uint64_t sidesCount{0ull};
	for (uint32_t y = cornerTL.y() ; y <= cornerBR.y() ; y++)
	{
		uint64_t northSegments = 0u;
		uint64_t southSegments = 0u;
		bool hadPreviousNorth = false;
		bool hadPreviousSouth = false;
		for (uint32_t x = cornerTL.x() ; x <= cornerBR.x() ; x++)
		{
			Position pos{x,y};
			if (fences.find(pos) == fences.end())
			{
				hadPreviousNorth = false;
				hadPreviousSouth = false;
				continue;
			}
			
			uint8_t fenceMask = fences[pos];
			bool hasNorth = ((fenceMask & NORTH_BIT) != 0u);
			bool hasSouth = ((fenceMask & SOUTH_BIT) != 0u);
			if (!hadPreviousNorth && hasNorth)
				northSegments++;
			if (!hadPreviousSouth && hasSouth)
				southSegments++;
			hadPreviousNorth = hasNorth;
			hadPreviousSouth = hasSouth;
		}
		sidesCount += northSegments + southSegments;
	}

	// Then repeat, but this time from the top downwards
	for (uint32_t x = cornerTL.x() ; x <= cornerBR.x() ; x++)
	{
		uint64_t eastSegments = 0u;
		uint64_t westSegments = 0u;
		bool hadPreviousEast = false;
		bool hadPreviousWest = false;
		for (uint32_t y = cornerTL.y() ; y <= cornerBR.y() ; y++)
		{
			Position pos{x,y};
			if (fences.find(pos) == fences.end())
			{
				hadPreviousEast = false;
				hadPreviousWest = false;
				continue;
			}
			
			uint8_t fenceMask = fences[pos];
			bool hasEast = ((fenceMask & EAST_BIT) != 0u);
			bool hasWest = ((fenceMask & WEST_BIT) != 0u);
			if (!hadPreviousEast && hasEast)
				eastSegments++;
			if (!hadPreviousWest && hasWest)
				westSegments++;
			hadPreviousEast = hasEast;
			hadPreviousWest = hasWest;
		}
		sidesCount += eastSegments + westSegments;
	}

	// Finally, return the amount
	return sidesCount;
}
#endif
