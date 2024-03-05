#include <AoC-Module.h>
#include <regex>
#include <vector>

class SpringReport
{
public:
	SpringReport(const std::string& springStatuses, const std::string& defectiveCountGroups)
		: m_springStatuses(springStatuses), m_defectiveCountGroups(defectiveCountGroups)
	{}
	~SpringReport() = default;
public:
	size_t getPossibleConfigsCount() const;
	const std::string& getSpringStatusString() const { return m_springStatuses; }
	const std::string& getDefectiveCountGroupsString() const { return m_defectiveCountGroups; }
private:
	size_t calcPermutationCount(const std::string& pattern, size_t groupSize) const;
	bool isGroupPossible(const std::string& pattern, size_t groupSize) const;
	size_t checkByBruteforce(const std::string& pattern, std::vector<size_t> groupSizes) const;
private:
	std::string m_springStatuses;
	std::string m_defectiveCountGroups;
};

static std::vector<std::string> stringSplit(const std::string& str, const std::string& delimiter);

const static std::string sc_springReportRegexStr = R"([#.?]+)";
const static std::string sc_damagedGroupsRegexStr = R"((?:\d+,?)+)";

const static std::regex sc_springReportRegex(sc_springReportRegexStr);
const static std::regex sc_damagedGroupsRegex(sc_damagedGroupsRegexStr);
const static std::regex sc_lineRegex("^(" + sc_springReportRegexStr + ")\\s+(" + sc_damagedGroupsRegexStr + ')');

static std::vector<SpringReport> s_allReports;

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	std::smatch lineParts;
	if (!std::regex_match(line, lineParts, sc_lineRegex))
	{
		std::cerr << "Line \"" << line << "\" doesn't match regex!" << std::endl;
		return false;
	}

	s_allReports.emplace_back(lineParts[1], lineParts[2]);

	return true;
}

void finalize()
{
	size_t allPossibleConfigs = 0;
	std::cout << "Parsed all reports:" << std::endl;
	for (SpringReport& report : s_allReports)
	{
		size_t possibleConfigs = report.getPossibleConfigsCount();
		std::cout << "  - Report:" << std::endl;
		std::cout << "    - spring-status: \"" << report.getSpringStatusString() << '"' << std::endl;
		std::cout << "    - defective-groups: \"" << report.getDefectiveCountGroupsString() << '"' << std::endl;
		std::cout << "    - possible configs: " << possibleConfigs << std::endl;
		allPossibleConfigs += possibleConfigs;
	}
	std::cout << "The sum of all possible configs is " << allPossibleConfigs << '!' << std::endl;
}

size_t SpringReport::getPossibleConfigsCount() const
{
	std::vector<std::string> statusGroups = stringSplit(m_springStatuses, ".");
	std::vector<size_t> defectiveGroupCounts;
	for (const std::string& str : stringSplit(m_defectiveCountGroups, ","))
		defectiveGroupCounts.push_back(std::stoull(str));
	
	//	std::cout << "##### " << statusGroups.size() << std::endl;
	// Never, ever code like this in production...
	size_t minimumPatternLength = [&defectiveGroupCounts]() -> size_t
	{
		size_t sum = 0;
		for (auto count : defectiveGroupCounts)
			sum += count;
		return sum;
	}() + defectiveGroupCounts.size()-1;
	// Simplest case: If the pattern's length is less than the minimum required pattern length (sum of all group sizes + (group count)-1 (for spacing in between))
	// then return 0
	if (m_springStatuses.length() < minimumPatternLength)
		return 0;
	
	// The assumption below doesn't account for the case where at least one pattern doesn't match "its" group size and another pattern matches for multiple group sizes...
	#if 0
	// Still relatively simple case: If we've got the same number of group sizes as patterns, check if each group satisfies 
	if (statusGroups.size() == defectiveGroupCounts.size())
	{
		finalConfigCount = 1;
		for (size_t i = 0 ; i < defectiveGroupCounts.size() ; i++)
		{
			const std::string& group = statusGroups[i];
			size_t groupSize = defectiveGroupCounts[i];
			//	std::cout << "Testing pattern \"" << group << "\" against group size of " << groupSize << "..." << std::endl;
			size_t permCount = calcPermutationCount(group, groupSize);
			//	std::cout << "Resulting number of permutations: " << permCount << std::endl;
			finalConfigCount *= permCount;
		}
	}
	#endif
	// Fuck this, let's bruteforce it...
	return checkByBruteforce(m_springStatuses, defectiveGroupCounts);
}

size_t SpringReport::calcPermutationCount(const std::string& pattern, size_t groupSize) const
{
	// If the pattern isn't possible, return 0
	if (!isGroupPossible(pattern, groupSize))
		return 0;

	size_t patternLength = pattern.length();
	// Otherwise, if the length is equal to the group size, then there's only 1 permutation
	if (patternLength == groupSize)
		return 1;
	
	// Going on from here, we know that the size of the pattern is greater than the group size.
	// So if this pattern doesn't contain any unknown status ('?'), then this pattern is impossible, so return 0
	if (pattern.find('?') == pattern.npos)
		return 0;
	
	// Now try to find anchor points (most left and most right '#', they are fixed) on which to swipe along them to find the pattern count.
	// If the length between these anchor points is equal to the group size, then there's only one permutation:
	// All characters between the anchors are '#' and the rest must be '.'
	size_t anchorLeft = pattern.find_first_of('#');
	size_t anchorRight = pattern.find_last_of('#');
	size_t anchorWidth = (anchorRight - anchorLeft)+1;
	if (anchorLeft != pattern.npos && anchorWidth == groupSize)
		return 1;
	// Otherwise, if we haven't found any anchor points, then all characters must be '?', in which case there are (patternLength - groupSize)+1 permutations
	else if (anchorLeft == pattern.npos)
		return (patternLength - groupSize)+1;
	else
	{
		// If we get here, we've got at least 1 anchor point to go off...
		// In which case we've got (groupSize - anchorWidth) wiggle room
		size_t wiggleRoomSize = groupSize - anchorWidth;

		// From here we've got min(wiggleRoomSize, space-to-the-left-of-anchor /*anchorLeft*/) permutations to the left and
		//                     min(wiggleRoomSize, space-to-the-right-of-anchor /*(patternLength - anchorRight)-1*/) permutations to the right
		// And - of course - one extra permutation: not moving at all
		return std::min(wiggleRoomSize, anchorLeft) + std::min(wiggleRoomSize, (patternLength - anchorRight)-1);
	}
}

static std::vector<std::string> stringSplit(const std::string& str, const std::string& delimiter)
{
	if (str.empty() || delimiter.empty() || str.find(delimiter) == str.npos)
		return { str };
	
	size_t delimLength = delimiter.length();
	std::vector<std::string> retVec;

	std::size_t startIdx = 0;
	for (size_t endIdx = str.find(delimiter) ; endIdx != str.npos ; startIdx = (endIdx + delimLength), endIdx = str.find(delimiter, startIdx))
	{
		if (endIdx > startIdx)
			retVec.emplace_back(str.substr(startIdx, endIdx - startIdx));
	}
	if (startIdx < str.length())
		retVec.emplace_back(str.substr(startIdx));
	return retVec;
}

bool SpringReport::isGroupPossible(const std::string& pattern, size_t groupSize) const
{
	size_t patternLength = pattern.length();
	// If we've got less characters than the group requires, this group isn't possible
	if (patternLength < groupSize)
		return false;
	
	// If we've got the exact same number of characters as required, but at least one of them is not '#' or '?',
	// then this is also not possible
	if (patternLength == groupSize && pattern.find_first_not_of("#?") != pattern.npos)
		return false;
	
	// Otherwise, split the pattern by '.' and check if any group has at least the length of the required group size
	for (const std::string& subGroup : stringSplit(pattern, "."))
	{
		if (subGroup.length() >= groupSize)
			return true;
	}
	return false;
}

size_t SpringReport::checkByBruteforce(const std::string& pattern, std::vector<size_t> groupSizes) const
{
	size_t groupCount = groupSizes.size();
	if (groupCount == 0)
		return 0;
	
	size_t ownGroupSize = groupSizes[0];
	size_t minimumSize = [&groupSizes]() -> size_t
	{
		size_t sum = 0;
		for (auto count : groupSizes)
			sum += count;
		return sum;
	}() + groupCount-1;

	if (pattern.size() < ownGroupSize)
		return 0;
	
	size_t permutationCount = 0;
	for (size_t step = 0 ; step < (pattern.size() - minimumSize) ; step++)
	{
		if (!isGroupPossible(pattern.substr(step, ownGroupSize), ownGroupSize) || pattern.at(step + ownGroupSize) != '.')
			continue;
		permutationCount += 1 + checkByBruteforce(pattern.substr(step + ownGroupSize), {groupSizes.begin()+1, groupSizes.end()});
	}
	return permutationCount;
}
