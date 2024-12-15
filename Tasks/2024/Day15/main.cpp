#include <AoC-Module.h>
#include <Vector2.hpp>
#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>

using Vec2 = Vector2u32;

static const std::array<Vec2,4u> DIRECTIONS
{
	Vec2{0u,-1u},
	Vec2{1u,0u},
	Vec2{0u,1u},
	Vec2{-1u,0u}
};

#ifdef PART_1
using Box = Vec2;
#else
// I had no better idea...
struct Box
{
	Box(const Vec2& leftBox)
		: leftBox{leftBox}, rightBox{leftBox + Vec2{1u,0u}}
	{}
	Box(uint32_t leftX, uint32_t leftY)
		: leftBox{leftX,leftY}, rightBox{leftX+1u,leftY}
	{}
	inline Box operator+(const Vec2& vec) const noexcept { return Box{leftBox + vec}; }
	inline bool operator==(const Box& other) const noexcept { return (leftBox == other.leftBox && rightBox == other.rightBox); }
	Vec2 leftBox{};
	Vec2 rightBox{};
};
template <>
struct std::hash<Box>
{
	inline size_t operator()(const Box& box) const noexcept
	{
		return Hash::combine<Vec2,Vec2>(box.leftBox, box.rightBox);
	}
};
#endif

static Vec2 s_area{};
static std::unordered_set<Box> s_boxes{};
static std::unordered_set<Vec2> s_walls{};
static std::vector<Vec2> s_movements{};
static Vec2 s_robotPos{};

static void printMap();
static bool markBoxesForMovement(const Vec2& movement, std::stack<Box>& boxesToMove);

bool handleLine(const std::string& line)
{
	static bool isMovements = false;
	if (line == "")
	{
		isMovements = true;
		return true;
	}

	static uint32_t yPos{0u};

	// Parse the map
	if (!isMovements)
	{
		if (s_area.x() == 0u)
		{
#ifdef PART_1
			s_area.x() = line.length();
#else
			s_area.x() = line.length() * 2u;
#endif
		}

		for (size_t charPos = line.find_first_of("O@#") ; charPos != line.npos ; charPos = line.find_first_of("O@#", charPos+1u))
		{
#ifdef PART_1
			size_t xPos = charPos;
#else
			size_t xPos = charPos * 2u;
#endif
			if (line[charPos] == 'O')
			{
				// Handled by the constructor
				s_boxes.emplace(xPos, yPos);
			}
			else if (line[charPos] == '#')
			{
				s_walls.emplace(xPos, yPos);
#ifndef PART_1
				s_walls.emplace(xPos+1u, yPos);
#endif
			}
			else
			{
				s_robotPos = Vec2{xPos, yPos};
			}
		}
		yPos++;
		s_area.y() = yPos;
	}
	else
	{
		for (char c : line)
		{
			switch (c)
			{
				case '^':
					s_movements.emplace_back(DIRECTIONS[0]);
					break;
				case '>':
					s_movements.emplace_back(DIRECTIONS[1]);
					break;
				case 'v':
					s_movements.emplace_back(DIRECTIONS[2]);
					break;
				case '<':
					s_movements.emplace_back(DIRECTIONS[3]);
					break;
				default:
					std::cerr << "Unknown character in movement!" << std::endl;
					return false;
			};
		}
	}

	return true;
}

void finalize()
{
	// Now we need to simulate the robot's movement... why do I have a bad feeling about this?...
	// 
	// For each movement, do the following:
	//   - if we're facing a wall, don't move and handle the next movement
	//   - if we're facing a box, do a "ray cast" past the box until we either:
	//      - ...hit a wall, then we're done
	//      - ...hit an empty space, then we push every box between the robot and the empty space
	// Sounds simple, but just because the sheer size of the input, this might be slow-ho-ho...

	std::cout << "Initial state:" << std::endl;
	printMap();
	for (const Vec2& movement : s_movements)
	{
		// Optimize a bit: build a stack of all boxes in our way until we hit a wall or an empty spot.
		// If we hit a wall, just "continue;" and we're good.
		// If we hit an empty spot, move every box *in reverse order* (that's why we use a stack)
		// one step in the direction of the movement
		std::stack<Box> boxesToPush{};
		if (!markBoxesForMovement(movement, boxesToPush))
			continue;
		
		// Like said before, move all the boxes in reverse order
		while (!boxesToPush.empty())
		{
			Box boxPos = boxesToPush.top();
			boxesToPush.pop();
			s_boxes.erase(boxPos);
			s_boxes.insert(boxPos + movement);
		}

		// Finally, move the robot
		s_robotPos += movement;
	}

	// Print the map for debugging purposes
	std::cout << "Final state:" << std::endl;
	printMap();

	// Now for getting the GPS-score, just iterate over all boxes and get the score
	uint64_t gpsScore{0u};
	for (const Box& box : s_boxes)
	{
#ifdef PART_1
		gpsScore += ((box.y() * 100ull) + box.x());
#else
		gpsScore += ((box.leftBox.y() * 100ull) + box.leftBox.x());
#endif
	}
	std::cout << "The GPS-score of all boxes is " << gpsScore << std::endl;
}

static void printMap()
{
	for (uint32_t y = 0u ; y < s_area.y() ; y++)
	{
		for (uint32_t x = 0u ; x < s_area.x() ; x++)
		{
			const Vec2 pos{x,y};
			if (s_robotPos == pos)
				std::cout << '@';
			else if (s_boxes.find(pos) != s_boxes.end())
			{
#ifdef PART_1
				std::cout << "O";
#else
				std::cout << "[]";
				x++;
#endif
			}
			else if (s_walls.find(pos) != s_walls.end())
				std::cout << '#';
			else
				std::cout << '.';
		}
		std::cout << '\n';
	}
	std::cout << std::endl;
}

static bool markBoxesForMovement(const Vec2& movement, std::stack<Box>& boxesToMove)
{
#ifndef PART_1
	// I've lost **way** too much time with part 2, so f*ck it: stupid route it is
	// Use this set to test if we've already checked for a certain box.
	// If so, DON'T TOUCH IT! ENDLESS LOOP (or at least buggy double-loop) INCOMING
	std::unordered_set<Box> checkedBoxes{};
#endif
	std::stack<Vec2> positionsToCheck{};
	positionsToCheck.push(s_robotPos + movement);
	while (!positionsToCheck.empty())
	{
		Vec2 checkPos = positionsToCheck.top();
		positionsToCheck.pop();
		if (s_walls.find(checkPos) != s_walls.end())
			return false;
#ifdef PART_1
		if (s_boxes.find(checkPos) == s_boxes.end())
			continue;
		positionsToCheck.push(checkPos + movement);
		boxesToMove.push(checkPos);
#else
		// If we don't find a box in front of us, then we might be looking at a right-box, so look to our left
		auto iter = s_boxes.find(checkPos);
		if (iter == s_boxes.end())
			checkPos.x() -= 1u;
		
		// Now we're left-aligned, so check again
		if ((iter = s_boxes.find(checkPos)) == s_boxes.end())
			continue;
		
		// If we've already checked the box, don't check it again!
		if (checkedBoxes.find(*iter) != checkedBoxes.end())
			continue;
		
		// If we're here, we've found a valid box for pushing
		checkedBoxes.insert(*iter);
		boxesToMove.push(checkPos);
		// But now we also need to add the new positions to check against.
		// With 2-wide boxes, this is trickier than before, but there's still a trick:
		//   - If we're checking vertically, just take the positions in-front/behind of the left- and right-box
		//   - If we're checking horizontally, take the right-box positions of our possible neighbours
		if (movement.y() > 0u)
		{
			positionsToCheck.emplace(iter->leftBox + movement);
			positionsToCheck.emplace(iter->rightBox + movement);
		}
		else
		{
			if (movement.x() == 1u)
				positionsToCheck.emplace(iter->rightBox + movement);
			else
				positionsToCheck.emplace(iter->leftBox + movement);
		}
#endif
	}
	return true;
}
