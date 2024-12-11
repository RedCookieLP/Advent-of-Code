#include <AoC-Module.h>
#include <regex>
#include <sstream>
#include <cmath>
#include <vector>
#include <unordered_map>

// We don't do the intuitive way (anymore at least) of keeping track of all the
// stones in some sort of list (like std::forward_list) and modifying it for
// each blink, but we rather keep track of all *unique stone values* with the
// number of stones that are engraved with them
static std::unordered_map<uint64_t, uint64_t> s_stones;

bool handleLine(const std::string& line)
{
	static const std::regex LINE_REGEX{ "(\\d+)\\s?" };
	for (auto regIter = std::sregex_iterator(line.cbegin(), line.cend(), LINE_REGEX) ; regIter != std::sregex_iterator() ; regIter++)
	{
		uint64_t stoneVal;
		std::stringstream ss;
		ss << (*regIter)[1].str();
		ss >> stoneVal;

		s_stones[stoneVal]++;
	}
	return true;
}

void finalize()
{
#ifdef PART_1
	constexpr static uint64_t BLINK_COUNT = 25ull;
#else
	constexpr static uint64_t BLINK_COUNT = 75ull;
#endif
	for (uint64_t i = 0u ; i < BLINK_COUNT ; i++)
	{
		// For each blink-iteration, build a new stone-line with the new stones and just replace the old one.
		// Makes iteration simpler and doesn't introduce a weird bug like before where there was one stone too much
		std::unordered_map<uint64_t, uint64_t> newStones;
		for (const auto& [stone, stoneCount] : s_stones)
		{
			// Apply the rules for the stones:
			// 1. If the stone is engraved with the number 0, it's replaced by a stone engraved with the number 1
			// 2. If the stone is engraved with a number with an even number of digits, split it in the middle
			//    and make it two s_stones, each taking half of the digits
			// 3. If none of the rules apply, the stone's replaced by a new stone with the value multiplied by 2024

			// 1.
			if (stone == 0u)
			{
				newStones[1] += stoneCount;
				continue;
			}

			// 2.
			uint64_t digitCount = static_cast<uint64_t>(std::ceil(std::log10<uint64_t>(stone+1)));
			if (digitCount % 2u == 0)
			{
				uint64_t splitter = std::pow<uint64_t, uint64_t>(10u, digitCount/2u);
				uint64_t firstStone = stone / splitter;
				uint64_t secondStone = stone % splitter;

				newStones[firstStone] += stoneCount;
				newStones[secondStone] += stoneCount;
				continue;
			}

			// 3.
			newStones[stone*2024] += stoneCount;
		}

		s_stones = std::move(newStones);
	}

	// At the end, sum up all counts to get the total amount
	uint64_t lineLength = 0u;
	for (const auto& [_, stoneCount] : s_stones)
		lineLength += stoneCount;

	std::cout << "After " << BLINK_COUNT << " blinks, the stone-line has permutated to a line with length " << lineLength << std::endl;
}
