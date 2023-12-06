#include <AoC-Module.h>
#include <regex>

struct Entry
{
	size_t raceDuration;
	size_t recordDistance;
};

const static std::string sc_numberRegexStr = R"((\d+)\s*)";
const static std::string sc_timeLineRegexStr = R"(^Time:\s*(?:)" + sc_numberRegexStr + ")+";
const static std::string sc_distanceLineRegexStr = R"(^Distance:\s*(?:)" + sc_numberRegexStr + ")+";

const static std::regex sc_numberRegex(sc_numberRegexStr);
const static std::regex sc_timeLineRegex(sc_timeLineRegexStr);
const static std::regex sc_distanceLineRegex(sc_distanceLineRegexStr);

static std::vector<Entry> s_records;
static size_t s_productOfPossibilities = 1;

static bool extractNumbers(std::string line, std::vector<size_t>& storage);
static Entry determineMaximumHoldTime(const Entry& entry);
static Entry determineMinimumHoldTime(const Entry& entry);

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	std::vector<size_t> extractionVector;
	if (std::regex_match(line, sc_timeLineRegex))
	{
		if (extractNumbers(line, extractionVector))
		{
			for (uint32_t i = 0 ; i < extractionVector.size() ; i++)
			{
				if (s_records.size() <= i)
					s_records.resize(i+1);
				s_records[i].raceDuration = extractionVector[i];
			}
		}
		else
		{
			std::cerr << "Failed to extract time-data from line \"" << line << "\"!" << std::endl;
			return false;
		}
	}
	else if (std::regex_match(line, sc_distanceLineRegex))
	{
		if (extractNumbers(line, extractionVector))
		{
			for (uint32_t i = 0 ; i < extractionVector.size() ; i++)
			{
				if (s_records.size() <= i)
					s_records.resize(i+1);
				s_records[i].recordDistance = extractionVector[i];
			}
		}
		else
		{
			std::cerr << "Failed to extract distance-data from line \"" << line << "\"!" << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << "Unknown line \"" << line << "\"!" << std::endl;
		return false;
	}
	return true;
}

void finalize()
{
#ifndef DO_PART_1
	// Push all numbers together...
	std::string timeStr = "", distanceStr = "";
	for (const Entry& record : s_records)
	{
		timeStr += std::to_string(record.raceDuration);
		distanceStr += std::to_string(record.recordDistance);
	}

	s_records.clear();
	s_records.emplace_back(Entry{ static_cast<size_t>(std::atoll(timeStr.c_str())), static_cast<size_t>(std::atoll(distanceStr.c_str())) });
#endif // ~DO_PART_1

	std::cout << "The extracted times and distances are:" << std::endl;
	for (const auto& record : s_records)
	{
		std::cout << "\t- " << record.raceDuration << "ms [record-distance: " << record.recordDistance << "mm]" << std::endl;
		Entry maximum = determineMaximumHoldTime(record);
		Entry minimum = determineMinimumHoldTime(record);
		std::cout << "\t  --> The minimum hold time for this race is " << minimum.raceDuration << "ms, resulting in a distance of " << minimum.recordDistance << "mm!" << std::endl;
		std::cout << "\t  --> The maximum hold time for this race is " << maximum.raceDuration << "ms, resulting in a distance of " << maximum.recordDistance << "mm!" << std::endl;

		size_t possibilityCount = (maximum.raceDuration - minimum.raceDuration) + 1;
		s_productOfPossibilities *= possibilityCount;
		std::cout << "\t   ==> Meaning you've got " << possibilityCount << " possibilit(y/ies) to win this race!" << std::endl;
	}

	std::cout << "The product of all numbers of possibilities is " << s_productOfPossibilities << '!' << std::endl;
}

static bool extractNumbers(std::string line, std::vector<size_t>& storage)
{
	storage.clear();
	for (std::smatch match ; std::regex_search(line, match, sc_numberRegex) ; line = match.suffix())
		storage.push_back(static_cast<size_t>(std::atoll(match.str().c_str())));
	return !storage.empty();
}

static Entry determineMaximumHoldTime(const Entry& entry)
{
	size_t holdTime = entry.raceDuration;
	size_t distance = 0;
	for ( ; holdTime > 0 && distance <= entry.recordDistance ; holdTime--, distance = (entry.raceDuration - holdTime)*holdTime);

	return { holdTime, distance};
}

static Entry determineMinimumHoldTime(const Entry& entry)
{
	size_t holdTime = 0;
	size_t distance = 0;
	for ( ; holdTime < entry.raceDuration && distance <= entry.recordDistance ; holdTime++, distance = (entry.raceDuration - holdTime)*holdTime);

	return { holdTime, distance};
}