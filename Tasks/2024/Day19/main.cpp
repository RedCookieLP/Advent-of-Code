#include <AoC-Module.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <HashHelper.hpp>
#include <regex>
#include <queue>
#include <memory>
#include <cstring>
#include <stack>
#include <thread>
#include <chrono>

std::unordered_set<std::string> s_allTowels{};
std::vector<std::string> s_designs{};

static uint64_t getPossibleDesignCombinations(const std::string& design, const std::unordered_set<std::string>& towels);

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
			s_allTowels.emplace(towelColors);
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
		possibleDesignCount += (getPossibleDesignCombinations(design, s_allTowels) > 0ull);
	}
	std::cout << "Of all " << s_designs.size()  << " design, exactly " << possibleDesignCount << " design(s) are possible!" << std::endl;
#else
	uint64_t designVariationCount = 0u;
	for (const std::string& design : s_designs)
	{
		designVariationCount += getPossibleDesignCombinations(design, s_allTowels);
	}
	std::cout << "The total number of all possible towel variations is " << designVariationCount << std::endl;
#endif
}

static std::unordered_map<std::string, uint64_t> s_cache{};
static uint64_t recursivePossibilityTest(const std::string& design, const std::unordered_set<std::string>& towels, uint32_t minLen, uint32_t maxLen);
static uint64_t getPossibleDesignCombinations(const std::string& design, const std::unordered_set<std::string>& towels)
{
	uint32_t minLen = static_cast<uint32_t>(-1u);
	uint32_t maxLen = 0u;

	// First, determine the minimum and maximum length that a segment must be to be matched by a towel
	for (const std::string& towel : towels)
	{
		uint32_t towelLen = towel.length();
		if (towelLen > maxLen)
			maxLen = towelLen;
		if (towelLen < minLen)
			minLen = towelLen;
	}

	// Then - using the min- and max-length - determine the number of distinct possible arrangements
	return recursivePossibilityTest(design, towels, minLen, maxLen);
}
static uint64_t recursivePossibilityTest(const std::string& design, const std::unordered_set<std::string>& towels, uint32_t minLen, uint32_t maxLen)
{
	// Check if we've got a solution in the cache
	auto cacheIter = s_cache.find(design);
	if (cacheIter != s_cache.end())
		return cacheIter->second;
	
	// Then check if the design can even be matched by the towels.
	// If not, return false...
	size_t designLength = design.length();
	if (designLength < minLen)
		return design.empty(); // ...except when it's empty, then it matched 100%, so it's fine!
	
	uint32_t localMaxLen = std::min<uint32_t>(static_cast<uint32_t>(designLength), maxLen);
	for (uint32_t len = minLen ; len <= localMaxLen ; len++)
	{
		// Try to match every sub-design from minLen to maxLen to the towels
		if (towels.find(design.substr(0,len)) == towels.end())
			continue;
		
		// Accumulate the amount in the cache
		s_cache[design] += recursivePossibilityTest(design.substr(len), towels, minLen, maxLen);
	}
	// If we get here, then the total number of all distinct possibilities
	// is stored in the cache, so return the value stored in the cache
	return s_cache[design];
}
