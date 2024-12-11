#include <AoC-Module.h>
#include <vector>
#include <regex>
#include <iostream>
#include <sstream>

static std::vector<uint32_t> lineToEntries(const std::string& line)
{
	static const std::regex lineRegex{R"((\d+)\s*)"};
	std::string str = line;

	std::vector<uint32_t> vec{};
	std::smatch strMatches;
	while (std::regex_search(str, strMatches, lineRegex))
	{
		uint32_t temp;
		{std::stringstream{} << strMatches[1u] >> temp;}
		vec.push_back(temp);
		str = strMatches.suffix();
	}

	return vec;
}

static bool isValidStep(uint32_t a, uint32_t b, bool shouldBeRising)
{
	uint32_t distance = std::max(a,b) - std::min(a,b);
	return ((shouldBeRising == (a < b)) && (distance >= 1u && distance <= 3u));
}

static uint32_t s_safeReportCounter = 0;

bool handleLine(const std::string& line)
{
	if (line == "")
	{
		return true;
	}

	auto entries = lineToEntries(line);
	if (entries.size() < 2u)
	{
		std::cerr << "Error! Couldn't pass non-empty line \"" << line << "\"!" << std::endl;
		return false;
	}

	// First, determine the direction of the reports
	bool isRising = (entries[0] < entries[1]);

	// Then, for each subsequent report, make sure the direction is the same and that the distance is between 1 and 3
#ifndef PART_1
	auto unsafeEntry = entries.end();
#endif
	bool isSafe = true;
	for (auto lIter = entries.begin(), rIter = (entries.begin()+1) ; rIter != entries.end() ; )
	{
#ifndef PART_1
		if (lIter == unsafeEntry)
		{
			++lIter;
		}
#endif

		// If the step isn't valid, bail out
		if (!isValidStep(*lIter, *rIter, isRising))
		{
#ifndef PART_1
			// Ugly, but important, special case:
			// If we're find so far, but the step to the last entry is invalid,
			// then let it pass, since we only need to "drop" the last entry
			if (unsafeEntry != entries.end() && rIter+1 == entries.end())
			{
				break;
			}
			else if (unsafeEntry == entries.end())
			{
				unsafeEntry = lIter;
				--lIter;
				continue;
			}
#endif
			std::cerr << "Found unsafe entry: " << line << std::endl;
			isSafe = false;
			break;
		}
		++lIter, ++rIter;
	}

	if (isSafe)
	{
		s_safeReportCounter++;
	}

	return true;
}

void finalize()
{
	std::cout << "The number of safe-reports is " << s_safeReportCounter << std::endl;
}
