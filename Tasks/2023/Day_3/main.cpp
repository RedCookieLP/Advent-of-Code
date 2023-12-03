#include <AoC-Module.h>
#include <vector>
#ifdef WINDOOF
#include <Windows.h>
#endif

struct Position
{
	uint32_t x;
	uint32_t y;
	bool operator==(const Position& other) const { return (other.x == x && other.y == y); }
};

struct BoundingBox
{
	Position topLeft;
	Position bottomRight;
};

struct PartNumber
{
	uint32_t value;
	uint32_t width;
	Position pos;
	bool isValid;
};

struct Symbol
{
	char value;
	Position pos;
};

// Constant representing "no position" value
#define POS_RESET	UINT32_MAX

static std::vector<PartNumber> s_allPartNumbers;
static std::vector<Symbol> s_allSymbols;
#ifndef DO_PART_1
static std::vector<PartNumber> s_allValidPartNumbers;
static uint64_t s_gearRatioSum;
#endif // ~DO_PART_1

static void getBoundingBox(BoundingBox& rBoundingBox, const PartNumber& partNumber);
static bool isPartNumberValid(const PartNumber& partNumber);
[[maybe_unused]] static void dumpMap(uint32_t width, uint32_t height);
static bool isAABBIntersection(const BoundingBox& boundingBox, const Position& point);

bool handleLine(const std::string& line)
{
	// If the line is empty, there's nothing to do
	if (line.empty())
		return true;
	
	// function-local, static variable representing the index of the current line we're on
	static uint32_t cursorYPos = 0;
	
	// temporary struct for a new part number
	PartNumber newNumber{ 0, 0, { POS_RESET, cursorYPos }, false };

	// Iterate through all chars in a line
	for (uint32_t cursorXPos = 0 ; cursorXPos < line.size() ; cursorXPos++)
	{
		const char& c = line[cursorXPos];

		// If the current char is a number, add it to the current/new part number
		if (isdigit(c))
		{
			// If we aren't already building a new part number, set the x-position for the new part number to the current cursor-x-position
			if (newNumber.pos.x == POS_RESET)
				newNumber.pos.x = cursorXPos;
			// Now add the number's value to the part number and increment the width
			newNumber.value = (newNumber.value * 10u) + static_cast<uint32_t>(c - '0');
			newNumber.width++;
		}
		// If the current char isn't a number...
		else
		{
			// Check if we've got a pending part number.
			// If so, add it to the list and reset the temporary struct
			if (newNumber.pos.x != POS_RESET)
			{
				s_allPartNumbers.push_back(newNumber);
				newNumber.pos.x = POS_RESET;
				newNumber.value = 0;
				newNumber.width = 0;
			}

			// If the current char is a dot, it's useless, so just continue
			if (c == '.')
				continue;
			
			// Otherwise add the current char to the list of all symbols with the current position
			s_allSymbols.emplace_back(Symbol{c, {cursorXPos, cursorYPos}});
		}
	}

	// if we got a pending part number at the end of the line, add it to the list, it's complete
	if (newNumber.pos.x != POS_RESET)
		s_allPartNumbers.push_back(newNumber);
	
	// Finally, increment the cursor's y-position for the next line
	cursorYPos++;
	
	return true;
}

void finalize()
{
#ifdef WINDOOF
	// Windows-stuff, enable ANSI-escape-codes
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif // WINDOOF

#ifdef DO_PART_1
	uint32_t partNumberSum = 0u;
#else // DO_PART_1
	uint32_t UNUSED partNumberSum = 0u;
#endif // ~DO_PART_1

	// Iterate through all found part numbers.
	// If one is valid, add it's value to the part number sum (for part 1),
	// mark it as valid (for debug-map) and add it to the list of all valid part numbers (for part 2)
	for (PartNumber& partNum : s_allPartNumbers)
	{
		if (isPartNumberValid(partNum))
		{
			partNumberSum += partNum.value;
			partNum.isValid = true;
#ifndef DO_PART_1
			s_allValidPartNumbers.push_back(partNum);
#endif
		}
	}

	// If we're in debug-mode, print a debug map with all symbols, valid and invalid part numbers colorized
#ifdef DEBUG
	dumpMap(140, 140);
#endif // DEBUG

	// If we're doing part 1, print the sum of all valid part numbers and finish
#ifdef DO_PART_1
	std::cout << "The sum of all valid PartNumbers is " << partNumberSum << '!' << std::endl;
#else // DO_PART_1
	// Otherwise, iterate through all "gears" ('*'-chars) and check if it has 2 part-number-neighbours.
	// If so, calculate it's ration (by multiplying the numbers) and add it to the global ratio sum.
	for (const Symbol& symbol : s_allSymbols)
	{
		if (symbol.value != '*')
			continue;
		
		// We start with 1, since we'll be appending the values by multiplying them
		uint64_t currentRatio = 1;
		// Counter to keep track of all neighbouring part numbers.
		uint32_t numberCount = 0;
		// Now iterate through all *valid* part numbers.
		// This is an optimization, since all numbers next to a gear *must* be valid
		for (const PartNumber& validNumber : s_allValidPartNumbers)
		{
			// Get the bounding box of the current valid part number.
			// It's 1 unit greater on all sides (except for numbers on the left and top edges)
			BoundingBox boundingBox;
			getBoundingBox(boundingBox, validNumber);
			
			// Check if the gear isn't inside the current number's bounding box.
			// If it isn't, then this number can be ignored
			if (!isAABBIntersection(boundingBox, symbol.pos))
				continue;

			// If we get here, the gear is inside the current number's bounding box, so this number is part of the ratio.
			// However, if the counter for ratio-numbers would become greater than 2, just break the loop, this gear is useless
			if (numberCount > 2)
				break;
			
			// Otherwise, multiply the current number's value to this gear's ratio and increment the number counter
			currentRatio *= validNumber.value;
			numberCount++;
		}

		// After iterating, if the count for neighbouring numbers is not 2,
		// this gear is useless, so continue with the next gear
		if (numberCount != 2)
			continue;
		
		// Otherwise add it's value to the sum of gear ratios
		s_gearRatioSum += currentRatio;
	}

	// Finally, print the sum of all gear ratios. This is the answer for part 2
	std::cout << "The sum of all gear ratios is " << s_gearRatioSum << '!' << std::endl;
#endif // ~DO_PART_1
}

static bool isPartNumberValid(const PartNumber& partNumber)
{
	// Calculate a bounding box for this number.
	// The bounding box is exactly one unit bigger in every direction
	BoundingBox boundingBox;
	getBoundingBox(boundingBox, partNumber);
	
	// The constructed bounding box is like this:
	//           T-----+
	//           |xxxxx|
	//           +-----B
	// Where:
	//   - xxxxx = any number
	//   - T     = bBoxTopLeft
	//   - B     = bBoxBottomRight
	// 
	// Using this bounding box, simply check if any symbol is contained in it using AABB-collision.
	// The coordinate system is like this:
	// 0---X+--->
	// |
	// Y+
	// |
	// v
	// If any symbol is contained in the bounding box, this part number is valid
	for (const Symbol& symbol : s_allSymbols)
	{
		const Position& symbolPos = symbol.pos;
		if (isAABBIntersection(boundingBox, symbolPos))
		{
			//	std::cout << "PartNumber " << partNumber.value << " [" << partNumber.pos.x << '|' << partNumber.pos.y << "] is valid because of a symbol '" << symbol.value << "' at [" << symbolPos.x << '|' << symbolPos.y << "]!" << std::endl;
			return true;
		}
	}

	// If we get here, no symbol is a neighbour, so this number is invalid
	return false;
}

static void getBoundingBox(BoundingBox& rBoundingBox, const PartNumber& partNumber)
{
	// Calculate the bounding box for a part number by expanding it's size on all sides (if possible)
	const Position& partNumPos = partNumber.pos;
	Position bBoxTopLeft { (partNumPos.x > 0 ? partNumPos.x - 1 : 0), (partNumPos.y > 0 ? partNumPos.y - 1 : 0) };
	Position bBoxBottomRight { partNumPos.x + partNumber.width, partNumPos.y + 1 };
	// Return the bounding box in the 'rBoundingBox' parameter
	rBoundingBox = BoundingBox{ bBoxTopLeft, bBoxBottomRight };
}

static bool isAABBIntersection(const BoundingBox& boundingBox, const Position& point)
{
	// Simple AABB-collision.
	// If the point-position is...
	//  - "greater or equal" than the top-left corner and...
	//  - "less or equal" than the bottom-right corner...
	// Then this point is inside the bounding box.
	return
	(
		point.x >= boundingBox.topLeft.x &&
		point.x <= boundingBox.bottomRight.x &&
		point.y >= boundingBox.topLeft.y &&
		point.y <= boundingBox.bottomRight.y
	);
}

constexpr static const char* const RESET = "\x1b[0m";
constexpr static const char* const RED = "\x1b[1;31m";
constexpr static const char* const GREEN = "\x1B[1;32m";
constexpr static const char* const YELLOW = "\x1B[1;33m";

// Function for printing the debug map.
// Since this code is a total mess and I'm not commenting it... have fun reading through and trying to understanding it :)...
[[maybe_unused]] static void dumpMap(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0)
		return;
	
	auto partNumberIter = s_allPartNumbers.cbegin();
	auto symbolIter = s_allSymbols.cbegin();
	for (Position cursorPos{0,0} ; cursorPos.y < height ; cursorPos.y++)
	{
		for (cursorPos.x = 0 ; cursorPos.x < width ; cursorPos.x++)
		{
			bool hadPrinted = false;
			for (; partNumberIter != s_allPartNumbers.cend() ; partNumberIter++)
			{
				const PartNumber& pNum = *partNumberIter;
				if (pNum.pos.y < cursorPos.y || (pNum.pos.y == cursorPos.y && pNum.pos.x < cursorPos.x))
					continue;
				
				if (pNum.pos == cursorPos)
				{
					if (pNum.isValid)
						std::cout << GREEN;
					else
						std::cout << RED;
					std::cout << pNum.value << RESET;

					cursorPos.x += pNum.width;
					cursorPos.x--;
					partNumberIter++;
					hadPrinted = true;
				}
				break;
			}

			for (; !hadPrinted && symbolIter != s_allSymbols.cend() ; symbolIter++)
			{
				const Symbol& symbol = *symbolIter;
				const Position& pos = symbol.pos;
				if (pos.y < cursorPos.y || (pos.y == cursorPos.y && pos.x < cursorPos.x))
					continue;
				
				if (pos == cursorPos)
				{
					std::cout << YELLOW << symbol.value << RESET;
					symbolIter++;
					hadPrinted = true;
				}

				break;
			}

			if (!hadPrinted && cursorPos.x < width)
			{
				std::cout << '.';
			}
		}
		std::cout << std::endl;
	}
	std::cout << "legend:\n"
			  << "   - " << RED << "123" << RESET << " = invalid part number\n"
			  << "   - " << GREEN << "456" << RESET << " = valid part number\n"
			  << "   - " << YELLOW << "#" << RESET << "   = symbol" << std::endl;
}
