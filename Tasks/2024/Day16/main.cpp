#include <AoC-Module.h>
#include <vector>
#include <Vector2.hpp>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <queue>

using Position = Vector2i64;

static const Position NO_NEIGHBOUR{};
static const std::array<Position, 4u> DIRECTIONS
{
	Position{0,-1},	// North
	Position{1, 0},	// East
	Position{0,1},	// South
	Position{-1, 0}	// West
};

struct Node
{
	std::array<Position, 4u> neighbours
	{
		NO_NEIGHBOUR,
		NO_NEIGHBOUR,
		NO_NEIGHBOUR,
		NO_NEIGHBOUR
	};
	uint32_t neighbourCount = 0u;
};

static Position s_startPos{};
static Position s_endPos{};
static Position s_mazeSize{};
// I'm totally lost...
static std::vector<std::string> s_rawMaze{};

static std::unordered_map<Position, Node> constructMaze();
static uint64_t getFastestPathsScore(const std::unordered_map<Position, Node>& maze);

void initialize(uint64_t lineCount)
{
	s_rawMaze.reserve(lineCount);
}

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	if (s_mazeSize.x() == 0u)
		s_mazeSize.x() = static_cast<uint32_t>(line.length());
	
	// First get the entire raw maze into a vector.
	// I've got no other simple-ish idea other than to iterate
	// over the maze using a 3x3 cutout to look for intersection/corners and their neighbours
	for (size_t charPos = line.find_first_of("ES") ; charPos != line.npos ; charPos = line.find_first_of("ES", charPos+1u))
	{
		Position pos{static_cast<uint32_t>(charPos), static_cast<uint32_t>(s_rawMaze.size())};
		if (line[charPos] == 'S')
			s_startPos = std::move(pos);
		else
			s_endPos = std::move(pos);
	}
	s_rawMaze.emplace_back(line);
	s_mazeSize.y() = s_rawMaze.size();
	return true;
}

void finalize()
{
	std::unordered_map<Position, Node> maze = constructMaze();
	std::cout << "Constructed the maze! It's structured like this:" << std::endl;
	for (const auto& [pos, node] : maze)
	{
		std::cout << "\tNode at [" << pos.x() << ',' << pos.y() << "]: " << node.neighbourCount << " neighbour(s)" << std::endl;
		for (uint8_t i = 0u ; i < node.neighbourCount ; i++)
		{
			std::cout << "\t\tNeighbour at [" << node.neighbours[i].x() << ',' << node.neighbours[i].y() << ']' << std::endl;
		}
	}

	uint64_t bestScore = getFastestPathsScore(maze);
	std::cout << "The best score the deer could achieve is " << bestScore << std::endl;
}

static std::unordered_map<Position, Node> constructMaze()
{
	std::unordered_map<Position, Node> newMaze{};

	// I'll construct the maze by iterating through the raw maze using a "3x3 cutout".
	// Using this I'm able to detect intersections (3/4 neighbours) and corners (2 neighbours not inline).
	// 
	// First, I'll use the cutout to find the positions all corners and intersection and "note down" in which direction
	// each node's neighbours are.
	//
	// In a second pass (just using the new map, not the raw maze) I'll actually connect the nodes with each other to
	// construct the final maze, which can then later be used to solve the puzzle...
	// 
	// This might be stupid, but that's the "easiest" way I could come up with...

	for (size_t y = 0u ; y < s_rawMaze.size() ; y++)
	{
		const std::string& line = s_rawMaze[y];
		for (size_t charPos = line.find_first_of(".ES") ; charPos != line.npos ; charPos = line.find_first_of(".ES", charPos+1u))
		{
			Position pos{static_cast<uint32_t>(charPos), static_cast<uint32_t>(y)};

			// Now that we've got a spot *somewhere* in the maze,
			// check if it's an intersection (3+ neighbours) or a corner (2 neighbours not inline)
			Node newNode;
			for (const Position& direction : DIRECTIONS)
			{
				Position nextPos = pos + direction;
				bool isNeighbour = (s_rawMaze[nextPos.y()].find_first_of(".ES", nextPos.x()) == nextPos.x()); // I'm lazy...
				if (isNeighbour)
				{
					newNode.neighbours[newNode.neighbourCount++] = nextPos;
				}
			}

			// Special case: If we're either at the end or the start, they count as separate nodes!
			if (line[charPos] == 'E' || line[charPos] == 'S')
			{
				newMaze.emplace(pos, std::move(newNode));
			}
			// If we got more than 2 neihgbours, it's an intersection.
			// If we got exactly 2 neighbours, check if they're not inline. If so, we got a corner.
			// Else we got nothing useful...
			else if (newNode.neighbourCount > 2)
			{
				newMaze.emplace(pos, std::move(newNode));
			}
			else if (newNode.neighbourCount == 2)
			{
				// To detect if the neighbours are inline or not, stretch a vector between
				// them and check if both components are non-zero
				Position diff = newNode.neighbours[0] - newNode.neighbours[1];
				if (diff.x() != 0u && diff.y() != 0u)
				{
					newMaze.emplace(pos, std::move(newNode));
				}
			}
		}
	}

	// By now should have a complete list of all intersections/corners, so now we play "connect the nodes"
	for (auto& [pos, node] : newMaze)
	{
		for (uint8_t i = 0u ; i < node.neighbourCount ; i++)
		{
			const Position dir = node.neighbours[i] - pos;
			// Walk along until we hit another node.
			// By the way this algorithm works, we *must* find one eventually!
			// (Or we'll hit a wall)
			Position neighbourPos = node.neighbours[i];
			bool hitWall = false;
			for ( ; !hitWall && newMaze.find(neighbourPos) == newMaze.end() ; neighbourPos += dir)
			{
				hitWall = (s_rawMaze[neighbourPos.y()][neighbourPos.x()] == '#');
			}

			if (hitWall)
			{
				// We've hit a wall, forget this direction...
				std::swap(node.neighbours[i], node.neighbours[--node.neighbourCount]);
				node.neighbours[node.neighbourCount] = NO_NEIGHBOUR;
				i--;
			}
			else
			{
				// We've found our neighbour, so store his position
				node.neighbours[i] = neighbourPos;
			}
		}
	}

	return newMaze;
}

struct PosDir
{
	inline bool operator==(const PosDir& other) const noexcept
	{
		return (pos == other.pos && dir == other.dir);
	}
	Position pos;
	Position dir;
};
template <>
struct std::hash<PosDir>
{
	inline size_t operator()(const PosDir& obj) const noexcept
	{
		return Hash::combine(obj.pos, obj.dir);
	}
};
struct QueueEntry
{
	PosDir posDir;
	uint64_t cost;
};

static uint64_t getFastestPathsScore(const std::unordered_map<Position, Node>& maze)
{
	// Solve this puzzle by using what seems like a modified version of Dijkstra (?... honestly, I've never used Dijkstra... successfully, that is)
	std::queue<QueueEntry> priorityQueue{};
	priorityQueue.push(QueueEntry{s_startPos, DIRECTIONS[1]/*facing east at the start*/, 0ull});

	uint64_t lowestScore = std::numeric_limits<uint64_t>::max();
	std::unordered_map<PosDir, uint64_t> visited{};
	while (!priorityQueue.empty())
	{
		QueueEntry entry = priorityQueue.front();
		priorityQueue.pop();
		
		if (entry.posDir.pos == s_endPos)
		{
			if (entry.cost < lowestScore)
				lowestScore = entry.cost;
			continue;
		}

		auto iter = visited.find(entry.posDir);
		if (iter != visited.end() && iter->second <= entry.cost)
			continue;
		visited.insert_or_assign(entry.posDir, entry.cost);

		// Now check all neighbours by adding them to the queue
		const Node& node = maze.at(entry.posDir.pos);
		for (uint8_t i = 0u ; i < node.neighbourCount ; i++)
		{
			Position neighbourPos = node.neighbours[i];
			Position posDiff = neighbourPos - entry.posDir.pos;
			int64_t pathLength = posDiff.len<int64_t>();
			Position newDir = (posDiff / pathLength);

			bool isTurn = (entry.posDir.dir != newDir);
			uint64_t newCost = entry.cost + pathLength + (isTurn ? 1000 : 0);

			//	std::cout << "At [" << entry.posDir.pos.x() << ',' << entry.posDir.pos.y() << "], checking in [" << newDir.x() << ',' << newDir.y() << "] with current cost at " << newCost << std::endl;
			priorityQueue.push(QueueEntry{neighbourPos, newDir, newCost});
		}
	}
	// No way had been found...
	return lowestScore;
}
