#include <AoC-Module.h>
#include <regex>

struct Mapping
{
	size_t dstStart;
	size_t srcStart;
	size_t range;
};

using MappingVector = std::vector<Mapping>;
using SeedVector = std::vector<size_t>;

static bool parseToMapping(const std::string& line, Mapping& mapping);
static size_t mapTo(size_t value, const Mapping& mapping);
static size_t translate(size_t value, const MappingVector& translationVector);

const static std::regex sc_mappingRegex(R"(^(\d+) (\d+) (\d+)$)");

const static std::string sc_seedToSoilStr = "seed-to-soil";
const static std::string sc_soilToFertilizerStr = "soil-to-fertilizer";
const static std::string sc_fertilizerToWaterStr = "fertilizer-to-water";
const static std::string sc_waterToLightStr = "water-to-light";
const static std::string sc_lightToTemperatureStr = "light-to-temperature";
const static std::string sc_temperatureToHumidityStr = "temperature-to-humidity";
const static std::string sc_humidityToLocationStr = "humidity-to-location";
const static std::string sc_seedRegexStr = R"((\d+)\s*)";

const static std::regex sc_seedRegex(sc_seedRegexStr);
const static std::regex sc_seedLineRegex(R"(^seeds:\s*(?:)" + sc_seedRegexStr + ")+");

static MappingVector s_seedToSoil;
static MappingVector s_soilToFertilizer;
static MappingVector s_fertilizerToWater;
static MappingVector s_waterToLight;
static MappingVector s_lightToTemperature;
static MappingVector s_temperatureToHumidity;
static MappingVector s_humidityToLocation;
static SeedVector s_seeds;

#define SELECT_AND_RETURN_IF_CURRENT(line, name) \
if (line.find(sc_##name##Str) == 0) \
{ \
	s_currentMappingVector = &s_##name; \
	return true; \
}

static MappingVector* s_currentMappingVector;

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	if (s_seeds.empty())
	{
		if (!std::regex_match(line, sc_seedLineRegex))
		{
			std::cerr << "Line \"" << line << "\" doesn't match the expected seed-line!" << std::endl;
			return false;
		}

		std::string lineCopy = line;
		for (std::smatch match ; std::regex_search(lineCopy, match, sc_seedRegex) ; s_seeds.push_back(static_cast<size_t>(std::atoll(match[1].str().c_str()))), lineCopy = match.suffix());

		return true;
	}
	
	SELECT_AND_RETURN_IF_CURRENT(line, seedToSoil)
	SELECT_AND_RETURN_IF_CURRENT(line, soilToFertilizer)
	SELECT_AND_RETURN_IF_CURRENT(line, fertilizerToWater)
	SELECT_AND_RETURN_IF_CURRENT(line, waterToLight)
	SELECT_AND_RETURN_IF_CURRENT(line, lightToTemperature)
	SELECT_AND_RETURN_IF_CURRENT(line, temperatureToHumidity)
	SELECT_AND_RETURN_IF_CURRENT(line, humidityToLocation)

	Mapping newMapping;
	if (!parseToMapping(line, newMapping))
	{
		std::cerr << "Failed to parse line \"" << line << "\" to a mapping!" << std::endl;
		return false;
	}
	s_currentMappingVector->push_back(newMapping);
	
	return true;
}

void finalize()
{
	size_t lowestLocation = SIZE_MAX;
#ifdef DO_PART_1
	for (size_t seed : s_seeds)
	{
		size_t locationNumber = translate(translate(translate(translate(translate(translate(translate(seed, s_seedToSoil), s_soilToFertilizer), s_fertilizerToWater), s_waterToLight), s_lightToTemperature), s_temperatureToHumidity), s_humidityToLocation);
		std::cout << "Seed " << seed << " matches to location " << locationNumber << '!' << std::endl;
		if (locationNumber < lowestLocation)
			lowestLocation = locationNumber;
	}
#else // DO_PART_1
	if (s_seeds.size() % 2 != 0)
	{
		std::cerr << "Seed line doesn't contain pairs of seed-ranges only!" << std::endl;
		return;
	}

	for (size_t idx = 0 ; idx < s_seeds.size() ; idx += 2)
	{
		size_t seedStart = s_seeds[idx];
		size_t seedRange = s_seeds[idx+1];
		for (size_t i = 0 ; i < seedRange ; i++)
		{
			size_t seed = seedStart + i;
			size_t locationNumber = translate(translate(translate(translate(translate(translate(translate(seed, s_seedToSoil), s_soilToFertilizer), s_fertilizerToWater), s_waterToLight), s_lightToTemperature), s_temperatureToHumidity), s_humidityToLocation);
			//	std::cout << "Seed " << seed << " matches to location " << locationNumber << '!' << std::endl;
			if (locationNumber < lowestLocation)
				lowestLocation = locationNumber;
		}
	}
#endif // ~DO_PART_1
	std::cout << "The lowest location number is " << lowestLocation << '!' << std::endl;
}

static size_t translate(size_t value, const MappingVector& translationVector)
{
	for (size_t origVal = value, idx = 0 ; value == origVal && idx < translationVector.size() ; value = mapTo(value, translationVector[idx++]));
	return value;
}

static size_t mapTo(size_t value, const Mapping& mapping)
{
	if (value < mapping.srcStart || value >= (mapping.srcStart + mapping.range))
		return value;
	return ((value - mapping.srcStart) + mapping.dstStart);
}

static bool parseToMapping(const std::string& line, Mapping& mapping)
{
	if (!std::regex_match(line, sc_mappingRegex))
		return false;
	
	std::smatch match;
	if (!std::regex_search(line, match, sc_mappingRegex) || match.size() != 4)
	{
		std::cerr << "Failed to search! Match-size: " << match.size() << std::endl;
		return false;
	}
	
	mapping.dstStart = static_cast<size_t>(std::atoll(match[1].str().c_str()));
	mapping.srcStart = static_cast<size_t>(std::atoll(match[2].str().c_str()));
	mapping.range = static_cast<size_t>(std::atoll(match[3].str().c_str()));

	return true;
}