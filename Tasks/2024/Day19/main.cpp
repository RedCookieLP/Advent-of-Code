#include <AoC-Module.h>
#include <vector>
#include <unordered_set>
#include <set>
#include <HashHelper.hpp>
#include <regex>
#include <queue>
#include <memory>
#include <cstring>
#include <stack>
#include <thread>
#include <chrono>

struct Towel
{
	inline bool operator==(const Towel& other) const noexcept
	{
		return (length == other.length && colors == other.colors);
	}
	inline bool operator>(const Towel& other) const noexcept
	{
		return length > other.length;
	}
	std::string colors;
	uint32_t length;
};
template <>
struct std::hash<Towel>
{
	inline size_t operator()(const Towel& towel) const noexcept
	{
		return Hash::combine(towel.colors, towel.length);
	}
};

std::unordered_set<Towel> s_allTowels{};
std::vector<std::string> s_designs{};

static uint64_t getTowelCombinationCount(const std::string& design, const std::unordered_set<Towel>& towels);

void initialize(uint32_t lineCount)
{
	// Remove the two new lines and the line containing all towel types
	s_designs.reserve(lineCount - 3u);
}

bool handleLine(const std::string& line)
{
	static bool inDesignBlock = false;
	if (line == "")
	{
		inDesignBlock = true;
		return true;
	}

	if (inDesignBlock)
	{
		s_designs.push_back(line);
	}
	else
	{
		static const std::regex TOWEL_REGEX{ "([a-z]+)(?:, |$)" };
		std::smatch strMatch;
		for (std::string str = line ; std::regex_search(str, strMatch, TOWEL_REGEX) ; str = strMatch.suffix())
		{
			const std::string towelColors = strMatch[1].str();
			s_allTowels.emplace(Towel{towelColors, static_cast<uint32_t>(towelColors.length())});
		}
	}
	return true;
}

void finalize()
{
#ifdef PART_1
	uint64_t possibleDesignCount = 0u;
	for (const std::string& design : s_designs)
	{
		std::cout << "Currently checking design \"" << design << "\"..." << std::endl;
		possibleDesignCount += (getTowelCombinationCount(design, s_allTowels) > 0u);
	}
	std::cout << "Of all " << s_designs.size()  << " design, exactly " << possibleDesignCount << " design(s) are possible!" << std::endl;
#else
#endif
}

struct QueueEntry
{
	Towel towel{};
	uint64_t position{0ull};
};

static inline bool towelMatches(const std::string& str, const Towel& towel) noexcept
{
	size_t strLen = str.size();
	size_t towelLen = towel.length;
	return (strLen >= towelLen && std::memcmp(str.c_str(), towel.colors.c_str(), towelLen) == 0);
}

static uint64_t getTowelCombinationCount(const std::string& design, const std::unordered_set<Towel>& towels)
{
	// pre-filter the towels to only includes the ones who's letters are in the design.
	std::multiset<Towel, std::greater<Towel>> filtered{};
	for (const Towel& towel : towels)
	{
		//	std::cout << "Checking towel \"" << towel.colors << "\" for design \"" << design << "\"..." << std::endl;
		if (towel.colors.find_first_of(design) == design.npos)
			continue;
		//	std::cout << " > Applicable" << std::endl;
		filtered.insert(towel);
	}

	uint64_t furtestPosition = 0u;
	uint64_t possibleCounter = 0u;
	// Depth-first approach
	std::stack<QueueEntry> towelQueue{};
	for (const Towel& towel : filtered)
	{
		if (towelMatches(design, towel))
			towelQueue.push(QueueEntry{towel, towel.length});
	}

	while (!towelQueue.empty())
	{
		auto entry = towelQueue.top();
		towelQueue.pop();
		//	std::this_thread::sleep_for(std::chrono::milliseconds(50u));
		if (entry.position >= design.size())
		{
#ifdef PART_1
			std::cout << std::endl;
			return 1;
#endif
			possibleCounter++;
			continue;
		}

		if (furtestPosition < entry.position)
		{
			furtestPosition = entry.position;
		}

		std::cout << "\x1B[2K\rCurrently checking here:   ";
		for (uint64_t i = 0u ; i < entry.position ; i++)
			std::cout << ' ';
		if (entry.position < furtestPosition)
			std::cout << '^' << std::flush;
		for (uint64_t i = entry.position+1u ; i < furtestPosition ; i++)
		{
			std::cout << ' ';
		}
		std::cout << "\x1B[1;92m^\x1B[0m" << std::flush;

		std::string subDesign = design.substr(entry.position);
		for (const Towel& towel : filtered)
		{
			if (towelMatches(subDesign, towel))
				towelQueue.push(QueueEntry{towel, entry.position + towel.length});
		}
	}

	std::cout << std::endl;

	return possibleCounter;
}
