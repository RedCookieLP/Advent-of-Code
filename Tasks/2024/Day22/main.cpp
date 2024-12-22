#include <AoC-Module.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <sstream>

static std::vector<uint64_t> s_pRNumbers;

struct SellSequence
{
	SellSequence() = default;
	inline bool operator==(SellSequence other) const noexcept
	{
		return *static_cast<const uint32_t*>(static_cast<const void*>(values)) == *static_cast<const uint32_t*>(static_cast<const void*>(other.values));
	}
	inline void add(int8_t value) noexcept
	{
		for (size_t i = 3 ; i > 0 ; i--)
			values[i] = values[i-1];
		values[0] = value;
	}
	int8_t values[4] {0,0,0,0};
};
template <>
struct std::hash<SellSequence>
{
	inline size_t operator()(SellSequence sequence) const noexcept
	{
		return std::hash<uint32_t>{}(*static_cast<const uint32_t*>(static_cast<const void*>(sequence.values)));
	}
};

void initialize(uint64_t lineCount)
{
	s_pRNumbers.reserve(lineCount-1u);
}

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	uint64_t value;
	{std::stringstream{} << line >> value;}
	s_pRNumbers.emplace_back(std::move(value));
	return true;
}

void finalize()
{
#ifndef PART_1
	std::vector<std::unordered_map<SellSequence, uint8_t>> buyerSellSequences{};
	buyerSellSequences.resize(s_pRNumbers.size());
#endif
	uint64_t pRNumberSum = 0ull;
	for (uint64_t pRNumber : s_pRNumbers)
	{
#ifndef PART_1
		static size_t index = -1;
		index++;
		auto& sellSequences = buyerSellSequences[index];
		SellSequence currentSequence{};
		int8_t previousPrice{-1};
		int8_t price = pRNumber % 10u;
#endif
		// Simulate generating 2000 numbers
		for (size_t i = 0ull ; i < 2000ull ; i++)
		{
			pRNumber = (pRNumber ^ (pRNumber << 6ull))  & 0x00FF'FFFFull;
			pRNumber = (pRNumber ^ (pRNumber >> 5ull))  & 0x00FF'FFFFull; // Masking here is useless, we're just dividing...
			pRNumber = (pRNumber ^ (pRNumber << 11ull)) & 0x00FF'FFFFull;
#ifndef PART_1
			previousPrice = price;
			price = pRNumber % 10u;
			currentSequence.add(price - previousPrice);
			// Don't test with ">= 4ull", since that would be a nasty off-by-one error (thx reddit-user "i_have_no_biscuits" with their post:
			// https://www.reddit.com/r/adventofcode/comments/1hjz1w4/2024_day_22_part_2_a_couple_of_diagnostic_test/)
			if (i >= 3ull && sellSequences.find(currentSequence) == sellSequences.end())
			{
				sellSequences.emplace(currentSequence, price);
			}
#endif
		}
		pRNumberSum += pRNumber;
	}
#ifdef PART_1
	std::cout << "The sum of all pseudo-random-numbers is " << pRNumberSum << std::endl;
#else
	// Determining the best possible price is *easy* now:
	// - First build a set of all sequences found in the price-changes of all buyers
	// - For each sequence we encounter, add the price for the current buyer to the currently stored sum
	// - While summing up the prices, keep track of what our current maximum we'd seen is
	// In the end, just print out the maximum price we'd seen
	std::unordered_map<SellSequence, uint32_t> sellSequenceSums{};
	sellSequenceSums.reserve(buyerSellSequences.size() * 1997ull); // Since with 2000 price changes, a total of 1997 sell-sequences can occur at max
	uint32_t bestSum = 0u;
	SellSequence bestSequence{};
	for (const auto& sellSequences : buyerSellSequences)
	{
		for (const auto& [sellSequence, price] : sellSequences)
		{
			uint32_t newPrice = (sellSequenceSums[sellSequence] += price);
			if (newPrice > bestSum)
			{
				bestSequence = sellSequence;
				bestSum = newPrice;
			}
		}
	}
	std::cout << "The maximum sum we could get with all sequences is " << bestSum << " with the sell-sequence being [" << static_cast<int32_t>(bestSequence.values[3]) << ','<< static_cast<int32_t>(bestSequence.values[2]) << ','<< static_cast<int32_t>(bestSequence.values[1]) << ','<< static_cast<int32_t>(bestSequence.values[0]) << ']' << std::endl;
#endif
}
