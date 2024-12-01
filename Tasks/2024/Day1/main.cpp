#include <AoC-Module.h>
#include <iostream>
#include <regex>
#include <vector>
#include <algorithm>
#include <sstream>
#ifndef PART_1
#	include <map>
#endif

static const std::regex sc_lineRegex{ R"(^(\d+)\s*(\d+)$)" };
static std::vector<uint32_t> leftList{};
#ifdef PART_1
static std::vector<uint32_t> rightList{};
#else
static std::map<uint32_t, uint32_t> rightMap{};
#endif

bool handleLine(const std::string& line)
{
	if (line == "")
	{
		return true;
	}

	std::smatch regMatches;
	if (!std::regex_search(line, regMatches, sc_lineRegex))
	{
		std::cerr << "Failed to parse line \"" << line << "\" with regex!" << std::endl;
		return false;
	}

	uint32_t leftValue, rightValue;
	std::stringstream ss{};
	ss << regMatches[1].str();
	ss >> leftValue;
	ss.clear();
	ss << regMatches[2].str();
	ss >> rightValue;

	leftList.push_back(leftValue);
#ifdef PART_1
	rightList.push_back(rightValue);
#else
	rightMap[rightValue]++;
#endif
	
	return true;
}

#ifdef PART_1
void finalize()
{
	std::sort(leftList.begin(), leftList.end());
	std::sort(rightList.begin(), rightList.end());

	uint64_t totalDistance = 0ull;
	for (size_t i = 0 ; i < leftList.size() ; i++)
	{
		uint32_t lValue = leftList[i];
		uint32_t rValue = rightList[i];

		totalDistance += ((lValue < rValue) ? (rValue - lValue) : (lValue - rValue));
	}

	std::cout << "The sum of all distances between the smallest numbers in each list is " << totalDistance << '!' << std::endl;
}
#else
void finalize()
{
	uint64_t similarityScore = 0ull;
	for (uint32_t leftValue : leftList)
	{
		similarityScore += (leftValue * rightMap[leftValue]);
	}
	std::cout << "The similarity score between the two lists is " << similarityScore << '!' << std::endl;
}
#endif
