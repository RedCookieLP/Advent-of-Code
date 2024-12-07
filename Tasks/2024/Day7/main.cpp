#include <AoC-Module.h>
#include <vector>
#include <regex>
#include <sstream>

static uint64_t s_sum{0};

static bool isSolveable(uint64_t target, const std::vector<uint64_t>::const_iterator& begin, const std::vector<uint64_t>::const_iterator& end, uint64_t intermediate = 0u);

bool handleLine(const std::string& line)
{
	if (line.empty())
	{
		return true;
	}
	
	static const std::regex LINE_REGEX{ R"((\d+)\:?\s*)" };

	// First extract all the numbers from the line
	std::vector<uint64_t> numbers;
	for (auto iter = std::sregex_iterator{line.begin(), line.end(), LINE_REGEX} ; iter != std::sregex_iterator{} ; iter++)
	{
		uint64_t num;
		std::stringstream ss{};
		ss << iter->str(1);
		ss >> num;
		numbers.emplace_back(num);
	}

	// If we didn't match any regex, something's wrong, so bail out
	if (numbers.empty())
	{
		std::cerr << "Line \"" << line << "\" didn't match for the regex!" << std::endl;
		return false;
	}

	// Then check if the equation is solvable.
	// If so, then we've found a match and can add the target value to the sum
	uint64_t target = numbers.front();
	if (isSolveable(target, numbers.cbegin()+1, numbers.cend()))
	{
		std::cout << target << " is solvable for" << std::endl;
		s_sum += target;
	}
	return true;
}

void finalize()
{
	std::cout << "The sum of all solvable equations is " << s_sum << std::endl;
}

// Check if an equation is solvable by using recursion.
static bool isSolveable(uint64_t target, const std::vector<uint64_t>::const_iterator& begin, const std::vector<uint64_t>::const_iterator& end, uint64_t intermediate)
{
	// First, check if we're at (or past) the end. If so, we didn't find a solution
	if (begin >= end)
	{
		return (target == intermediate);
	}
	
	uint64_t val = *begin;

	// Then check if our current number is greater than the target.
	// If so, then the equation won't be solvable
	if (target < val)
	{
		return false;
	}
	
	// Otherwise check the next step: First with addition, then multiplication (PART 2: then concatenation)
	if (isSolveable(target, begin+1u, end, intermediate + val))
	{
		// We've found a solution using addition!
		return true;
	}
	else if (isSolveable(target, begin+1u, end, (intermediate == 0u ? 1u : intermediate) * val))
	{
		// We've found a solution using multiplication
		return true;
	}
#ifndef PART_1
	else
	{
		std::stringstream ss;
		ss << intermediate << *begin;
		ss >> intermediate;
		if (isSolveable(target, begin+1u, end, intermediate))
		{
			// We've found a solution using concatenation
			return true;
		}
	}
#endif
	return false;
}
