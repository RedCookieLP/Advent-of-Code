#include <AoC-Module.h>
#include <Vector2.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>

using Position = Vector2u32;

struct Region
{
public:
	Region() = default;
	~Region() = default;
public:
	bool isNeighbouring(const Position& plotPos) const noexcept;
	void addPlot(const Position& plotPos) noexcept;
public:
	inline uint64_t getPerimeter() const noexcept
	{
		return m_perimeter;
	}
	inline uint64_t getArea() const noexcept
	{
		return m_plots.size();
	}
	bool merge(const Region& other) noexcept;
private:
	uint8_t getNeighbourArrangement(const Position& plotPos) const noexcept;
	inline uint32_t getNumberOfNeighbours(const Position& plotPos) const noexcept
	{
		uint8_t arrangement = getNeighbourArrangement(plotPos);
		uint32_t count = 0u;
		for (uint8_t bit = 0x01u ; bit < 0x10u ; bit <<= 1u)
		{
			count += ((arrangement & bit) != 0u);
		}
		return count;
	}
private:
	std::unordered_set<Position> m_plots{};
	uint64_t m_perimeter{0ull};
};

static std::unordered_map<char, std::vector<Position>> s_plots;
static const std::array<Position, 4u> DIRECTIONS =
{
	Position{0u,-1u},	// North
	Position{1u,0u},	// East
	Position{0u,1u},	// South
	Position{-1u,0u}	// West
};

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	static uint32_t yPos{0u};
	for (uint32_t xPos = 0u ; xPos < line.length() ; xPos++)
	{
		s_plots[line[xPos]].emplace_back(xPos, yPos);
	}
	yPos++;
	return true;
}

void finalize()
{
	// First build the areas from the top down
	std::unordered_map<char, std::vector<Region>> regions;
	for (const auto& [type, plotPosVec] : s_plots)
	{
		std::vector<Region>& regionVec = regions[type];
		for (const auto& plotPos : plotPosVec)
		{
			if (!regionVec.empty())
			{
				// Ho-ho-holy moly...
				bool foundRegion = false;
				for (Region& region : regionVec)
				{
					if (region.isNeighbouring(plotPos))
					{
						foundRegion = true;
						region.addPlot(plotPos);
						break;
					}
				}
				if (foundRegion)
				{
					continue;
				}
			}
			regionVec.emplace_back().addPlot(plotPos);
		}
	}

	// Then merge all neighbouring regions of the same type
	for (auto& [type, regionVec] : regions)
	{
		for (auto regIter = regionVec.begin() ; regIter != regionVec.end() ; regIter++)
		{
			for (auto nextIter = regIter+1u ; nextIter != regionVec.end() ; )
			{
				if (regIter->merge(*nextIter))
				{
					nextIter = regionVec.erase(nextIter);
				}
				else
				{
					nextIter++;
				}
			}
		}
	}

	uint64_t cost = 0u;
	for (const auto& [type, regionVec] : regions)
	{
		size_t regions = regionVec.size();
		std::cout << "Found " << regions << " regions of type " << type << std::endl;
		for (const Region& reg : regionVec)
		{
			uint64_t price = reg.getArea() * reg.getPerimeter();
			std::cout << "\t> Aera: " << reg.getArea() << " | Perimeter: " << reg.getPerimeter() << " >>> Price: " << price << std::endl;
			cost += price;
		}
	}
	std::cout << "The cost for all fences is " << cost << std::endl;
}



uint8_t Region::getNeighbourArrangement(const Position& plotPos) const noexcept
{
	// The arrangement indicates if a plot has a neighbour in a direction like so:
	//  > arrangement: 0 0 0 0 W S E N
	// Where each bit denotes if there's a neighbour in that direction
	uint8_t arrangement = 0u;
	for (uint8_t i = 0u ; i < 4u ; i++)
	{
		if (m_plots.find(plotPos + DIRECTIONS[i]) != m_plots.end())
			arrangement |= (1u << i);
	}
	return arrangement;
}

bool Region::isNeighbouring(const Position& plotPos) const noexcept
{
	// Plots that are already contained, are not neighbouring...
	if (m_plots.find(plotPos) != m_plots.end())
		return false;
	
	// Now check if we've got at least one neighbour
	return (getNumberOfNeighbours(plotPos) > 0u);
}

void Region::addPlot(const Position& plotPos) noexcept
{
	m_plots.insert(plotPos);
	// If this is the first plot, set the perimeter to 4u
	if (m_plots.size() == 1u)
	{
		m_perimeter = 4u;
		return;
	}

	// Get the number of neighbours to this tile
	uint32_t neighbours = getNeighbourArrangement(plotPos);

	// Now manipulate the perimeter in a *special way*
	switch (neighbours)
	{
	case 0b1000:
	case 0b0100:
	case 0b0010:
	case 0b0001:
		m_perimeter += 2u;
		break;
	case 0b1110:
	case 0b1101:
	case 0b1011:
	case 0b0111:
	case 0b1010:
	case 0b0101:
		m_perimeter -= 2u;
		break;
	case 0b1111:
		m_perimeter -= 4u;
		break;
	case 0b0000:
		m_perimeter += 4u; // Just in case...
	default: // There's nothing to do for the rest
		break;
	}
}

bool Region::merge(const Region& other) noexcept
{
#ifndef SMART
	bool mergeable = false;

	const Region *greater, *smaller;
	if (m_plots.size() < other.m_plots.size())
	{
		greater = &other;
		smaller = this;
	}
	else
	{
		greater = this;
		smaller = &other;
	}

	for (const Position& otherPos : smaller->m_plots)
	{
		if (greater->isNeighbouring(otherPos))
		{
			mergeable = true;
			break;
		}
	}

	if (!mergeable)
		return false;
	
	for (const Position& otherPos : other.m_plots)
		addPlot(otherPos);
	return true;
#else
#endif
}
