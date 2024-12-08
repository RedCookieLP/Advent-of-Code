#include <AoC-Module.h>
#include <unordered_set>
#include <unordered_map>

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
	inline Position& operator+=(const Position& other) noexcept
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	inline Position operator-(const Position& other) const noexcept
	{
		return Position{x - other.x, y - other.y};
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
	inline Position operator-() const noexcept
	{
		return {-x, -y};
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

static Position s_area;
static std::unordered_map<char, std::vector<Position>> s_antennaPositions;

void initialize(uint64_t lineCount)
{
	s_area.y = lineCount-1u;
}

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	if (s_area.x == 0u)
		s_area.x = line.length();
	
	static uint32_t lineCount = 0ull;
	for (uint64_t charPos = line.find_first_not_of('.') ; charPos != line.npos ; charPos = line.find_first_not_of('.', charPos+1ull))
	{
		s_antennaPositions[line.at(charPos)].emplace_back(static_cast<uint32_t>(charPos), lineCount);
	}
	lineCount++;

	return true;
}

void finalize()
{
	std::unordered_set<Position> antinodePositions{};

	// Iterate through each antenna-type and find all the types and their corresponding antinodes
	for (const auto& [antennaType, positions] : s_antennaPositions)
	{
		for (auto iterA = positions.cbegin() ; iterA != positions.cend() ; iterA++)
		{
			const Position& antennaAPos = *iterA;
			for (auto iterB = iterA+1u ; iterB != positions.cend() ; iterB++)
			{
				const Position& antennaBPos = *iterB;
				// Now our unique antenna pair is [antennaAPos, antennaBPos]
				// First, get the delta between the two antennas
				const Position deltaPos = antennaBPos - antennaAPos;
				
#ifdef PART_1
				// Then find their two antinode points by the following formular:
				// Anti_A = A - Δ(A->B)
				// Anti_B = B + Δ(A->B)
				const Position Anti_A = antennaAPos - deltaPos;
				const Position Anti_B = antennaBPos + deltaPos;

				// Finally, add both positions to the set, as long as they're unique AND fit in the area (out-of-bounds one's don't count!)
				if (Anti_A.x < s_area.x && Anti_A.y < s_area.y)
				{
					antinodePositions.insert(Anti_A);
				}
				if (Anti_B.x < s_area.x && Anti_B.y < s_area.y)
				{
					antinodePositions.insert(Anti_B);
				}
#else
				// Now the fun part begins: Add the two antenna's position to the set,
				// but also *every occurence before and after them, because of harmonic frequencies
				antinodePositions.insert(antennaAPos);
				antinodePositions.insert(antennaBPos);

				// We add the harmonics by first walking from A to B and onwards, until we're out-of-bounds.
				// Then we repeat the same in the other direction
				for (Position harmonicAtB = antennaBPos + deltaPos ; harmonicAtB.x < s_area.x && harmonicAtB.y < s_area.y ; harmonicAtB += deltaPos)
				{
					antinodePositions.insert(harmonicAtB);
				}
				for (Position harmonicAtA = antennaAPos - deltaPos ; harmonicAtA.x < s_area.x && harmonicAtA.y < s_area.y ; harmonicAtA += -deltaPos)
				{
					antinodePositions.insert(harmonicAtA);
				}
#endif
			}
		}
	}

	std::cout << "In the area of size [" << s_area.x << ',' << s_area.y << "], the number of unique antinode-positions is " << antinodePositions.size() << std::endl;
}
