#include <AoC-Module.h>
#include <regex>

const static std::string sc_setRegexText = R"((?:\s?(\d+) (blue|red|green),?))";
const static std::string sc_gameRegexText = "(?:" + sc_setRegexText + "+(?:;|$))";
const static std::string sc_lineRegexText = R"(^Game (\d+): )" + sc_gameRegexText + '+';
const static std::regex sc_setRegex(sc_setRegexText);
const static std::regex sc_gameRegex(sc_gameRegexText);
const static std::regex sc_lineRegex(sc_lineRegexText);

struct Set
{
	uint32_t id;
	uint32_t redCubes;
	uint32_t greenCubes;
	uint32_t blueCubes;
};

struct Game
{
	uint32_t id;
	std::vector<Set> sets;
};

static bool parseGame(Game& game, std::string gameStr);
static bool parseSet(Set& set, std::string setStr);

#ifdef DO_PART_1
constexpr static size_t sc_maxRedCubes = 12ULL;
constexpr static size_t sc_maxGreenCubes = 13ULL;
constexpr static size_t sc_maxBlueCubes = 14ULL;

static bool isGamePossible(const Game& game);
static bool isSetPossible(const Set& set);

static size_t s_idSum = 0ULL;
#else // DO_PART_1
static size_t getGamePower(const Game& game);

static size_t s_powerSum = 0ULL;
#endif // ~DO_PART_1

bool handleLine(const std::string& line)
{
	// If the line is empty, just ignore it
	if (line.empty())
		return true;
	
	// Check if the non-empty line matches the general structure of a game.
	// If not, then the input might be completely wrong, so give up
	if (!std::regex_match(line, sc_lineRegex))
	{
		std::cerr << "Line doesn't match regex!" << std::endl;
		return false;
	}

	Game game;
	if (!parseGame(game, line))
	{
		std::cerr << "Failed to parse game!" << std::endl;
		return false;
	}

#ifdef DO_PART_1
	// Check if this game is possible. If so, add it's ID to the current sum
	if (isGamePossible(game))
		s_idSum += game.id;
	else
		std::cerr << " --> Game #" << game.id << " is not possible!" << std::endl;
#else // DO_PART_1
	s_powerSum += getGamePower(game);
#endif // ~DO_PART_1

	return true;
}

static bool parseSet(Set& set, std::string setStr)
{
	for (std::smatch polls ; std::regex_search(setStr, polls, sc_setRegex) ; setStr = polls.suffix())
	{
		// Since every poll captures only an amount and a color, the size should be *3*
		// The very first "group" (polls[0]) is the entire matched string
		if (polls.size() != 3)
		{
			std::cerr << "Invalid poll: Evaluation not possible!" << std::endl;
			return false;
		}

		// If we have 3 groups, then polls[1] is the amount and polls[2] is the color
		const std::string& amount = polls[1];
		const std::string& color = polls[2];

		uint32_t pollAmount = static_cast<uint32_t>(std::atol(amount.c_str()));
		// Since switch-statements don't work with std::string, simple if-else-ifs have to do
		// It's only a small range of possible values, so no worries here...
		// also this is not a performance critical application, so f*ck performance
		if (color == "red")
			set.redCubes = pollAmount;
		else if (color == "green")
			set.greenCubes = pollAmount;
		else if (color == "blue")
			set.blueCubes = pollAmount;
	}
	return true;
}

static bool parseGame(Game& game, std::string gameStr)
{
	// Extract the game's ID.
	// If extracting the ID fails, give up
	std::smatch gameLineMatch;
	if (!std::regex_search(gameStr, gameLineMatch, sc_lineRegex) || gameLineMatch.size() == 2)
	{
		std::cerr << "Failed to extract the game's ID!" << std::endl;
		return false;
	}
	game.id = static_cast<uint32_t>(std::atol(gameLineMatch[1].str().c_str()));

	// Parse all sets
	uint32_t setID = 0;
	for (std::smatch sets ; std::regex_search(gameStr, sets, sc_gameRegex) ; gameStr = sets.suffix())
	{
		// If parsing a set fails, this game is uselesss, so give up
		Set newSet{ setID++, 0, 0, 0 };
		if (!parseSet(newSet, sets.str()))
		{
			std::cerr << "Failed to parse set!" << std::endl;
			return false;
		}
		game.sets.push_back(newSet);
	}

	return true;
}

#ifdef DO_PART_1
static bool isSetPossible(const Set& set)
{
	return (
		set.redCubes <= sc_maxRedCubes &&
		set.greenCubes <= sc_maxGreenCubes &&
		set.blueCubes <= sc_maxBlueCubes
	);
}
static bool isGamePossible(const Game& game)
{
	// Iterate through every set. If any set is impossible, the entire game is impossible
	for (auto set : game.sets)
	{
		// If the current set is not possible, return false instantly, we don't need to check the other ones
		if (!isSetPossible(set))
		{
			std::cerr << "Set #" << set.id << " is not possible!" << std::endl;
			if (set.redCubes > sc_maxRedCubes)
				std::cerr << "\tToo many red cubes polled [" << set.redCubes << " > " << sc_maxRedCubes << "]!" << std::endl;
			if (set.greenCubes > sc_maxGreenCubes)
				std::cerr << "\tToo many green cubes polled [" << set.greenCubes << " > " << sc_maxGreenCubes << "]!" << std::endl;
			if (set.blueCubes > sc_maxBlueCubes)
				std::cerr << "\tToo many blue cubes polled [" << set.blueCubes << " > " << sc_maxBlueCubes << "]!" << std::endl;
			return false;
		}
	}
	return true;
}
#else // DO_PART_1

#define SET_IF_GREATER(a,b) if ((a) < (b)) (a) = (b) 
static size_t getGamePower(const Game& game)
{
	// If the game doesn't have any sets, it doesn't have any power...
	if (game.sets.size() == 0)
		return 0;

	// Determine the minimum amount of required cubes by iterating through all sets and checking for the minimum
	Set minimumSet { 0, 0, 0, 0 };
	for (const Set& set : game.sets)
	{
		SET_IF_GREATER(minimumSet.redCubes, set.redCubes);
		SET_IF_GREATER(minimumSet.greenCubes, set.greenCubes);
		SET_IF_GREATER(minimumSet.blueCubes, set.blueCubes);
	}

	// Calculate the power of the minimal set
	size_t setPower = size_t(minimumSet.redCubes * minimumSet.greenCubes * minimumSet.blueCubes);
	std::cout << "The minimal set of this game ([R:" << minimumSet.redCubes << "|G:" << minimumSet.greenCubes << "|B:" << minimumSet.blueCubes << "]) has a power of " << setPower << '!' << std::endl;

	// Return the power of the minimal set
	return setPower;
}
#endif // ~DO_PART_1

void finalize()
{
#ifdef DO_PART_1
	std::cout << "The sum of all possible game's IDs is " << s_idSum << '!' << std::endl;
#else // DO_PART_1
	std::cout << "The sum of all game's powers is " << s_powerSum << '!' << std::endl;
#endif // ~DO_PART_1
}