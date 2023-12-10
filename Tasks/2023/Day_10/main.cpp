#include <AoC-Module.h>
#include <unordered_map>
#include <regex>
#ifdef WINDOOF
#	include <Windows.h>
#endif // WINDOOF

struct Position
{
	int x;
	int y;
	bool operator==(const Position& other) const { return (other.x == x && other.y == y); }
	bool operator!=(const Position& other) const { return !(*this == other); }
	Position operator-(const Position& other) const { return Position{x - other.x, y - other.y}; }
	Position operator+(const Position& other) const { return Position{x + other.x, y + other.y}; }
};

template <>
class std::hash<Position>
{
public:
	std::size_t operator()(const Position& pos) const
	{
		// Since size_t is always* at least double the size of ints... just bitshift them around...
		size_t quote_hash_quote = ((size_t(pos.x) << 32) | size_t(pos.y));
		return quote_hash_quote;
	}
};

#ifndef INT_MAX
#	define INT_MAX 0x7FFF'FFFF
#endif

constexpr const static Position NO_POSITION{ INT_MAX, INT_MAX };
constexpr const static Position NORTH_DIR{ 0, -1 };
constexpr const static Position SOUTH_DIR{ 0, 1 };
constexpr const static Position EAST_DIR{ 1, 0 };
constexpr const static Position WEST_DIR{ -1, 0 };

enum class TileType
{
	VERTICAL_PIPE,		// '|'
	HORIZONTAL_PIPE,	// '-'
	NORTH_EAST_PIPE,	// 'L'
	NORTH_WEST_PIPE,	// 'J'
	SOUTH_WEST_PIPE,	// '7'
	SOUTH_EAST_PIPE,	// 'F'
	GROUND,				// '.'
	START_POS,			// 'S'
	UNKNOWN,
};

#define TILE_UNVISITED	-1i64
#define TILE_INSIDE		-2i64
#define TILE_OUTSIDE	-3i64

struct Tile
{
	TileType type = TileType::UNKNOWN;
	int64_t currenBesttDistance = TILE_UNVISITED;
};

TileType charToTileType(char c)
{
	switch (c)
	{
		case '|': return TileType::VERTICAL_PIPE;
		case '-': return TileType::HORIZONTAL_PIPE;
		case 'L': return TileType::NORTH_EAST_PIPE;
		case 'J': return TileType::NORTH_WEST_PIPE;
		case '7': return TileType::SOUTH_WEST_PIPE;
		case 'F': return TileType::SOUTH_EAST_PIPE;
		case '.': return TileType::GROUND;
		case 'S': return TileType::START_POS;
		default: return TileType::UNKNOWN;
	}
}
char tileTypeToChar(TileType tileType)
{
	switch (tileType)
	{
		case TileType::VERTICAL_PIPE: return '|';
		case TileType::HORIZONTAL_PIPE: return '-';
		case TileType::NORTH_EAST_PIPE: return 'L';
		case TileType::NORTH_WEST_PIPE: return 'J';
		case TileType::SOUTH_WEST_PIPE: return '7';
		case TileType::SOUTH_EAST_PIPE: return 'F';
		case TileType::GROUND: return '.';
		case TileType::START_POS: return 'S';
		default: return '?';
	}
}

static size_t findDistanceToFarthestPointInLoop(Position currentPosition);
static bool isMovementPossible(Position from, Position to);

const static std::regex sc_lineRegex(R"(^[|\-LJ7F.S]+)");
static std::unordered_map<Position, Tile> s_map;
static Position startPosition{0,0};
static int s_mapWidth = 0;
static int s_lineIndex = 0;
static size_t s_furthestPoint = 0;

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	if (!std::regex_match(line, sc_lineRegex))
	{
		std::cerr << "Line \"" << line << "\" doesn't match regex!" << std::endl;
		return false;
	}

	// Stupid, but it works :/
	s_mapWidth = 0;
	int xPos = 0;
	for (char c : line)
	{
		TileType tileType = charToTileType(c);
		if (tileType == TileType::START_POS)
			startPosition = Position{xPos, s_lineIndex};
		s_map[Position{xPos++, s_lineIndex}] = Tile{tileType};
		s_mapWidth++;
	}
	s_lineIndex++;

	return true;
}

constexpr static const char* const RESET = "\x1b[0m";
constexpr static const char* const BLUE = "\x1b[1;94m";
constexpr static const char* const GREEN_BACKGROUND = "\x1B[1;42m";
constexpr static const char* const YELLOW_BACKGROUND = "\x1B[1;43m";
constexpr static const char* const INSIDE_BORDER_COLOR = "\x1B[1;92m";
constexpr static const char* const OUTSIDE_BORDER_COLOR = "\x1B[1;91m";

void finalize()
{
	#ifdef WINDOOF
	// Windows-stuff, enable ANSI-escape-codes
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif // WINDOOF
	std::cout << "Finished parsing lines!" << std::endl;
	s_furthestPoint = findDistanceToFarthestPointInLoop(startPosition);
	std::cout << "The farthest point from the starting position is " << s_furthestPoint << " tiles away!" << std::endl;

	size_t insideTileCount = 0;

	enum class BoarderStatus
	{
		OUTSIDE,
		INSIDE,
		POTENTIALLY_OUTSIDE,
		POTENTIALLY_INSIDE,
	};

	#ifndef DO_PART_1
	for (int yPos = 0 ; yPos < s_lineIndex ; yPos++)
	{
		BoarderStatus status = BoarderStatus::OUTSIDE;
		bool cornerFromSouth = false;

		for (int xPos = 0 ; xPos < s_mapWidth ; xPos++)
		{
			Position curPos {xPos, yPos};
			const Tile& tile = s_map[curPos];
			const TileType& originalType = tile.type;
			TileType type = originalType;
			if (type == TileType::START_POS)
			{
				bool possibleMovements[4] {false};
				constexpr const static Position sc_allMovements[4] { NORTH_DIR, EAST_DIR, SOUTH_DIR, WEST_DIR };
				for (size_t i = 0 ; i < 4 ; i++)
				{
					possibleMovements[i] = isMovementPossible(curPos, curPos + sc_allMovements[i]);
				}
				// Ugly code incoming...
				if (possibleMovements[0] && possibleMovements[1]) // North & East
					type = TileType::NORTH_EAST_PIPE;
				else if (possibleMovements[0] && possibleMovements[2]) // North & South
					type = TileType::VERTICAL_PIPE;
				else if (possibleMovements[0] && possibleMovements[3]) // North & West
					type = TileType::NORTH_WEST_PIPE;
				else if (possibleMovements[1] && possibleMovements[2]) // East & South
					type = TileType::SOUTH_EAST_PIPE;
				else if (possibleMovements[1] && possibleMovements[3]) // East & West
					type = TileType::HORIZONTAL_PIPE;
				else if (possibleMovements[2] && possibleMovements[3]) // South & West
					type = TileType::SOUTH_WEST_PIPE;
			}

			if (tile.currenBesttDistance != TILE_UNVISITED)
			{
				switch (type)
				{
					case TileType::NORTH_EAST_PIPE:
						cornerFromSouth = false;
						status = (status == BoarderStatus::OUTSIDE ? BoarderStatus::POTENTIALLY_INSIDE : BoarderStatus::POTENTIALLY_OUTSIDE);
					break;
					case TileType::SOUTH_EAST_PIPE:
						cornerFromSouth = true;
						status = (status == BoarderStatus::OUTSIDE ? BoarderStatus::POTENTIALLY_INSIDE : BoarderStatus::POTENTIALLY_OUTSIDE);
					break;
					case TileType::NORTH_WEST_PIPE:
						if (cornerFromSouth)
							status = (status == BoarderStatus::POTENTIALLY_INSIDE ? BoarderStatus::INSIDE : BoarderStatus::OUTSIDE);
						else
							status = (status == BoarderStatus::POTENTIALLY_INSIDE ? BoarderStatus::OUTSIDE : BoarderStatus::INSIDE);
					break;
					case TileType::SOUTH_WEST_PIPE:
						if (cornerFromSouth)
							status = (status == BoarderStatus::POTENTIALLY_INSIDE ? BoarderStatus::OUTSIDE : BoarderStatus::INSIDE);
						else
							status = (status == BoarderStatus::POTENTIALLY_INSIDE ? BoarderStatus::INSIDE : BoarderStatus::OUTSIDE);
					break;
					case TileType::VERTICAL_PIPE:
						status = (status == BoarderStatus::INSIDE ? BoarderStatus::OUTSIDE : BoarderStatus::INSIDE);
					break;
					default:
					break;
				}
			}

			char tileChar = tileTypeToChar(originalType);
			if (originalType == TileType::START_POS)
				std::cout << YELLOW_BACKGROUND << tileChar << RESET;
			else if (tile.currenBesttDistance == s_furthestPoint)
				std::cout << GREEN_BACKGROUND << tileChar << RESET;
			else if (tile.currenBesttDistance != TILE_UNVISITED)
				std::cout << BLUE << tileChar << RESET;
			else
			{
				if (status == BoarderStatus::INSIDE)
				{
					insideTileCount++;
					std::cout << INSIDE_BORDER_COLOR << tileChar << RESET;
				}
				else
					std::cout << OUTSIDE_BORDER_COLOR << tileChar << RESET;
			}
		}
		std::cout << std::endl;

	}
	std::cout << "There are " << insideTileCount << " tiles inside the graph!" << std::endl;
	#endif // ~DO_PART_1
}

#define INVALID_MOVE SIZE_MAX
static size_t findDistanceToFarthestPointInLoop(Position startPosition)
{
	Tile& possibleStartTile = s_map[startPosition];
	if (possibleStartTile.type != TileType::START_POS)
		return INVALID_MOVE;
	
	struct TileAnalysis
	{
		Tile* currentTile;
		Position currentTilePos;
		Position previousTilePos;
	};
	
	std::vector<TileAnalysis> inspectingTiles;
	inspectingTiles.push_back(TileAnalysis{&possibleStartTile, startPosition, NO_POSITION});
	size_t currentPathLength = 0;
	bool amDone = false;
	while (!inspectingTiles.empty() && !amDone)
	{
		auto currentTiles = inspectingTiles;
		inspectingTiles.clear();

		//	std::cout << " --- FLOODING #" << currentPathLength << std::endl;

		for (auto&[tile, position, prevPos] : currentTiles)
		{
			//	std::cout << "Inspecting at tile [" << position.x << ", " << position.y << "]..." << std::endl;
			if (!isMovementPossible(prevPos, position))
			{
				//	std::cout << "Inspection is useless: Movement is disallowed!" << std::endl;
				continue;
			}
			
			if (tile->currenBesttDistance == TILE_UNVISITED)
			{
				tile->currenBesttDistance = currentPathLength;
			}
			else
			{
				//	std::cout << "Reached the end of this path..." << std::endl;
				amDone = true;
				break;
			}
			
			Position moveDir = NO_POSITION;
			if (prevPos != NO_POSITION)
				moveDir = position - prevPos;
			
			switch (tile->type)
			{
				case TileType::UNKNOWN:
				case TileType::GROUND:
					//	std::cout << "Nothing to do..." << std::endl;
					continue;
				break;
				case TileType::HORIZONTAL_PIPE:
				{
					if (moveDir == WEST_DIR)
						inspectingTiles.push_back(TileAnalysis{&s_map[position + WEST_DIR], position + WEST_DIR, position});
					else
						inspectingTiles.push_back(TileAnalysis{&s_map[position + EAST_DIR], position + EAST_DIR, position});
				}
				break;
				case TileType::VERTICAL_PIPE:
				{
					if (moveDir == NORTH_DIR)
						inspectingTiles.push_back(TileAnalysis{&s_map[position + NORTH_DIR], position + NORTH_DIR, position});
					else
						inspectingTiles.push_back(TileAnalysis{&s_map[position + SOUTH_DIR], position + SOUTH_DIR, position});
				}
				break;
				case TileType::NORTH_EAST_PIPE:
				{
					if (moveDir == SOUTH_DIR)
						inspectingTiles.push_back(TileAnalysis{&s_map[position + EAST_DIR], position + EAST_DIR, position});
					else
						inspectingTiles.push_back(TileAnalysis{&s_map[position + NORTH_DIR], position + NORTH_DIR, position});
				}
				break;
				case TileType::NORTH_WEST_PIPE:
				{
					if (moveDir == SOUTH_DIR)
						inspectingTiles.push_back(TileAnalysis{&s_map[position + WEST_DIR], position + WEST_DIR, position});
					else
						inspectingTiles.push_back(TileAnalysis{&s_map[position + NORTH_DIR], position + NORTH_DIR, position});
				}
				break;
				case TileType::SOUTH_EAST_PIPE:
				{
					if (moveDir == NORTH_DIR)
						inspectingTiles.push_back(TileAnalysis{&s_map[position + EAST_DIR], position + EAST_DIR, position});
					else
						inspectingTiles.push_back(TileAnalysis{&s_map[position + SOUTH_DIR], position + SOUTH_DIR, position});
				}
				break;
				case TileType::SOUTH_WEST_PIPE:
				{
					if (moveDir == NORTH_DIR)
						inspectingTiles.push_back(TileAnalysis{&s_map[position + WEST_DIR], position + WEST_DIR, position});
					else
						inspectingTiles.push_back(TileAnalysis{&s_map[position + SOUTH_DIR], position + SOUTH_DIR, position});
				}
				break;
				case TileType::START_POS:
				{
					inspectingTiles.push_back(TileAnalysis{&s_map[position + NORTH_DIR], position + NORTH_DIR, position});
					inspectingTiles.push_back(TileAnalysis{&s_map[position + EAST_DIR], position + EAST_DIR, position});
					inspectingTiles.push_back(TileAnalysis{&s_map[position + SOUTH_DIR], position + SOUTH_DIR, position});
					inspectingTiles.push_back(TileAnalysis{&s_map[position + WEST_DIR], position + WEST_DIR, position});
				}
				break;
			}
		}
		currentPathLength++;
	}

	return currentPathLength-1;
}

static bool isMovementPossible(Position from, Position to)
{
	if (from == NO_POSITION)
		return true;
	if (to == NO_POSITION)
		return false;
	
	Position movement = to - from;
	TileType toTileType = s_map[to].type;

	switch (toTileType)
	{
		case TileType::UNKNOWN:
		case TileType::GROUND:
			return false;
		case TileType::HORIZONTAL_PIPE:
			return (movement == WEST_DIR || movement == EAST_DIR);
		case TileType::VERTICAL_PIPE:
			return (movement == NORTH_DIR || movement == SOUTH_DIR);
		case TileType::NORTH_EAST_PIPE:
			return (movement == SOUTH_DIR || movement == WEST_DIR);
		case TileType::NORTH_WEST_PIPE:
			return (movement == SOUTH_DIR || movement == EAST_DIR);
		case TileType::SOUTH_EAST_PIPE:
			return (movement == NORTH_DIR || movement == WEST_DIR);
		case TileType::SOUTH_WEST_PIPE:
			return (movement == NORTH_DIR || movement == EAST_DIR);
		default: // TileType::START_POS
			return true;
	}
}