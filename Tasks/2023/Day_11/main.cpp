#include <AoC-Module.h>
#include <vector>

struct Position
{
	int64_t x;
	int64_t y;
};

#if defined(DEBUG) && defined(DO_PART_1) // This is to not crash the program, in case it's trying to print at least 1'000'000x larger universes...
static void dumpGalaxies();
#else // (DEBUG && DO_PART_1)
static void dumpGalaxies() {}
#endif // ~(DEBUG && DO_PART_1)
static void expandUniverse();

// This is the multiplier for the expansion-rate, so to get double the amount of empty-space, this multiplier has to be *1*
// This also means that the multiplier has to be *999'999* to get 1'000'000x the empty space...
#ifdef DO_PART_1
#	define EXPANSION_MULTIPLIER 1
#else // DO_PART_1
#	define EXPANSION_MULTIPLIER 999'999
#endif // ~DO_PART_1

static std::vector<Position> s_galaxyPositions;
static size_t s_imageWidth = 0ULL;
static size_t s_imageHeight = 0ULL;
// These "hit-maps" (as I've named them) tell, if any* x- or y-coordinate is used by one or more galaxies
static std::vector<bool> s_xHitMap;
static std::vector<bool> s_yHitMap;
static std::vector<size_t> s_emptyColumns;
static std::vector<size_t> s_emptyRows;

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	if (line.find_first_not_of(".#") != line.npos)
	{
		std::cerr << "Line \"" << line << "\" contains unknown characters!" << std::endl;
		return false;
	}

	if (s_imageWidth == 0)
	{
		s_imageWidth = line.length();
		s_xHitMap.resize(s_imageWidth);
	}

	if (line.find('#') == line.npos)
	{
		s_yHitMap.push_back(false);
	}
	else
	{
		s_yHitMap.push_back(true);

		for (size_t offsetX = offsetX = line.find('#') ; (offsetX != line.npos && offsetX < s_imageWidth) ; offsetX = line.find('#', offsetX+1))
		{
			s_xHitMap[offsetX] = true;
			s_galaxyPositions.emplace_back(Position{static_cast<int64_t>(offsetX), static_cast<int64_t>(s_imageHeight)});
		}
	}
	s_imageHeight++;

	return true;
}

void finalize()
{
	dumpGalaxies();

	std::cout << "Making pairs of galaxies:" << std::endl;
	// Simply make pairs by iteratively assigning each galaxy to the following ones...
	// Meaning: Connect #0 to #1, #2, #3, ... #x
	//                  #1 to #2, #3, #4, ... #x
	//                  #2 to #3, #4, #5, ... #x
	//                  ...
	//                  #x-1 to #x
	// The total pair count for x numbers will be x*(x-1)/2
	size_t galaxyCount = s_galaxyPositions.size();
	std::vector<std::pair<Position*, Position*>> galaxyPairs;
	for (size_t i = 0 ; i < galaxyCount - 1 ; i++)
	{
		for (size_t j = i+1 ; j < galaxyCount ; j++)
		{
			//	std::cout << "  - Pairing galaxy #" << i << " with galaxy #" << j << std::endl;
			galaxyPairs.push_back(std::make_pair(&s_galaxyPositions[i], &s_galaxyPositions[j]));
		}
	}

	std::cout << "The following colums are completely empty and will be expanded soon: ";
	for (size_t column = 0 ; column < s_xHitMap.size() ; column++)
	{
		if (s_xHitMap[column])
			continue;
		s_emptyColumns.push_back(column);
		std::cout << column << ' ';
	}
	std::cout << std::endl << "The following rows   are completely empty and will be expanded soon: ";
	for (size_t row = 0 ; row < s_yHitMap.size() ; row++)
	{
		if (s_yHitMap[row])
			continue;
		s_emptyRows.push_back(row);
		std::cout << row << ' ';
	}
	std::cout << std::endl;

	expandUniverse();
	std::cout << "The universe has been expanded!" << std::endl;

#if defined(DEBUG) && defined(DO_PART_1)
	std::cout << "It now looks like this:" << std::endl;
	dumpGalaxies();
#endif // (DEBUG && DO_PART_1)

	std::cout << "Calculating the sum of all shortest paths between all pairs..." << std::endl;
	size_t sumOfAllShortestPaths = 0;
	for (const auto[galaxy1, galaxy2] : galaxyPairs)
	{
		size_t pathLength = std::abs(galaxy1->x - galaxy2->x) + std::abs(galaxy1->y - galaxy2->y);
		//	std::cout << "  - Distance between [" << galaxy1->x << ", " << galaxy1->y << "] and [" << galaxy2->x << ", " << galaxy2->y << "]: " << pathLength << std::endl;
		sumOfAllShortestPaths += pathLength;
	}
	std::cout << "The sum of all pair's shortest paths is " << sumOfAllShortestPaths << '!' << std::endl;
}

#if defined(DEBUG) && defined(DO_PART_1)
static void dumpGalaxies()
{
	size_t tracker = 0;
	auto curPosIter = s_galaxyPositions.cbegin();
	for (size_t y = 0 ; y < s_imageHeight ; y++)
	{
		for (size_t x = 0 ; x < s_imageWidth ; x++)
		{
			size_t galaxyX = static_cast<size_t>(curPosIter->x);
			size_t galaxyY = static_cast<size_t>(curPosIter->y);
			if (curPosIter != s_galaxyPositions.cend() && (galaxyY < y || (galaxyY == y && galaxyX < x)))
			{
				curPosIter++;
				tracker++;
			}

			if (galaxyY == y && galaxyX == x)
				std::cout << '#'; // std::hex << tracker << std::dec;
			else
				std::cout << '.';
		}
		std::cout << std::endl;
	}
}
#endif // (DEBUG && DO_PART_1)

static void expandUniverse()
{
	// If there aren't any empty rows/columns, there's nothing to do...
	if (s_emptyColumns.size() == 0 && s_emptyRows.size() == 0)
		return;

	// To expand the universe, I'll first construct an array in which I'll store by what amount each galaxy at a point will expand.
	// Then I'll just add the amounts for each galaxy, given it's original location.
	//
	// This will be done separately for the x- and y-dimension.
	size_t* expansionAmountX = new size_t[s_imageWidth]{0};
	size_t* expansionAmountY = new size_t[s_imageHeight]{0};
	for (size_t x = 0, columnIdx = 0 ; x < s_imageWidth ; x++)
	{
		size_t column = s_emptyColumns[columnIdx];
		if (columnIdx < s_emptyColumns.size() && column < x)
		{
			columnIdx++;
			column = s_emptyColumns[columnIdx];
		}
		expansionAmountX[x] = columnIdx * EXPANSION_MULTIPLIER;
	}
	for (size_t y = 0, rowIdx = 0 ; y < s_imageHeight ; y++)
	{
		size_t row = s_emptyRows[rowIdx];
		if (rowIdx < s_emptyRows.size() && row < y)
		{
			rowIdx++;
			row = s_emptyRows[rowIdx];
		}
		expansionAmountY[y] = rowIdx * EXPANSION_MULTIPLIER;
	}

	// Now apply the calculated expansion offsets to all galaxy-positions...
	for (Position& pos : s_galaxyPositions)
	{
		pos.x += expansionAmountX[pos.x];
		pos.y += expansionAmountY[pos.y];
	}

	// Don't forget to free the allocated arrays!
	delete[] expansionAmountX;
	delete[] expansionAmountY;
	
	// Since every empty row/column has been doubled/millionthed (kjsdhfbbvkjdf), just add the number of empty rows/columns (multiplied by the expansion-multiplier) to the image's width/height
	s_imageWidth += s_emptyColumns.size() * EXPANSION_MULTIPLIER;
	s_imageHeight += s_emptyRows.size() * EXPANSION_MULTIPLIER;
}