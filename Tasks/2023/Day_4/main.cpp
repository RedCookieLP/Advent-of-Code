#include <AoC-Module.h>
#include <regex>
#include <unordered_map>

const static std::string sc_numberRegexText = R"((?:\s+(\d+)))";
const static std::string sc_numberGroupRegexText = sc_numberRegexText + '+';
const static std::string sc_cardRegexText = "Card" + sc_numberRegexText + ':' + sc_numberGroupRegexText + R"(\s+\|)" + sc_numberGroupRegexText;

const static std::regex sc_numberRegex(sc_numberRegexText);
const static std::regex sc_numberGroupRegex(sc_numberGroupRegexText);
const static std::regex sc_cardRegex(sc_cardRegexText);

struct ScratchCard
{
	uint32_t cardID;
	std::vector<uint32_t> winningNumbers;
	std::vector<uint32_t> cardNumbers; // How original, I know
};

#ifdef DO_PART_1
static uint32_t s_cardScoreSum = 0u;
#else // DO_PART_1
static std::unordered_map<uint32_t/*card-ID*/, uint32_t/*number of cards*/> s_cardsCount;
#endif // ~DO_PART_1

static void extractNumbers(std::string str, std::vector<uint32_t>& numberVector);
static uint32_t getMatchingNumberCount(const ScratchCard& card);

bool handleLine(const std::string& line)
{
	// Just like always: If the line is empty, do nothing
	if (line.empty())
		return true;
	
	// If we've got a line, but it doesn't match the expected format, then give up and error-out
	if (!std::regex_match(line, sc_cardRegex))
	{
		std::cerr << "Line \"" << line << "\" doesn't match card-regex!" << std::endl;
		return false;
	}

	// (admittedly, this is a bit messy)
	// Extract all groups of number(s) into an array of size 3.
	// Why 3? Because we'll get these 3 groups:
	//   1. ID of the card
	//   2. Group of winning numbers
	//   3. Group of number we've got
	// Extract them by - like always - using regex-search
	uint32_t idx = 0;
	std::string numberGroups[3];
	std::smatch numberGroupMatches;
	for (std::string lineCopy = line ; std::regex_search(lineCopy, numberGroupMatches, sc_numberGroupRegex) ; lineCopy = numberGroupMatches.suffix())
	{
		// If we'd get more than 3 groups, something is wrong, so stop what we're doing...
		if (idx == 3)
			break;
		// Otherwise add it to the array of number-group-strings
		numberGroups[idx++] = numberGroupMatches.str();
	}
	// Check if we actually got exactly 3 groups.
	// If not, give up and error-out
	if (idx != 3)
	{
		std::cerr << "Unkown number of number-group-matches (" << idx << ") for line \"" << line << "\"!" << std::endl;
	}

	// Using the freshly extracted groups, construct a new ScratchCard
	ScratchCard newCard;
	newCard.cardID = static_cast<uint32_t>(std::atol(numberGroups[0].c_str()));
	extractNumbers(numberGroups[1], newCard.winningNumbers);
	extractNumbers(numberGroups[2], newCard.cardNumbers);

	// Now get the count for matching numbers
	uint32_t matchingNumberCount = getMatchingNumberCount(newCard);
#ifdef DO_PART_1
	// Calculate the card's score using bitshifts (fancy).
	// Since "1u << matchingNumberCount" would result in a score (almost) always double the actual score, shift it over once to the right.
	// Coincidentally this also handles the case where matchingNumberCount = 0 just fine, since (1u << 0) = 1u ==> (1u >> 1u) = 0u.
	// No matching numbers means no score...
	uint32_t scratchCardScore = ((1u << matchingNumberCount) >> 1u);
	std::cout << "The score for this scratch card is " << scratchCardScore << '!' << std::endl;
	// Finally, add the card's score to the total score sum
	s_cardScoreSum += scratchCardScore;
#else // DO_PART_1
	// Part 2 is as simple as that: Keep track of the amount of cards for each card's ID (easiest when using a map).
	
	// Increment the card count for the current card's ID, because... duh, we're working on an instance of that card right now...
	// (thanks to the STL, this will add a new map-entry with a count of 0, in case the card ID is yet unknown)
	uint32_t currentIDCardCount = ++s_cardsCount[newCard.cardID];
	//	std::cout << "Adding " << matchingNumberCount << " card(s) " << s_cardsCount[newCard.cardID] << " time(s) after Card #" << newCard.cardID << "..." << std::endl;
	// Now, for the card's matching-number-count of following neighbours add the
	// current card ID's instance count to each neighbour's ID counter
	// To put it simply: For the following 'matchingNumberCount'-number of ID-neighbours, add the current card-ID's instance count (so how
	// many cards [original + copies] we've got for the current card ID) to their instance count
	for (uint32_t cardCopyID = newCard.cardID + 1u, matchCountCopy = matchingNumberCount ; matchCountCopy-- ; cardCopyID++)
	{
		s_cardsCount[cardCopyID] += currentIDCardCount;
	}
#endif // ~DO_PART_1
	return true;
}

void finalize()
{
#ifdef DO_PART_1
	// For part 1, just print the total sum of all scratchcard scores
	std::cout << "The total sum of all card's scores is " << s_cardScoreSum << '!' << std::endl;
#else
	// For part 2, sum up the card counts of each ID and print the sum out
	uint32_t totalCardCount = 0u;
	std::cout << "According to the new rule set we've got these numbers of cards for each card:" << std::endl;
	for (auto iter = s_cardsCount.cbegin() ; iter != s_cardsCount.cend() ; iter++)
	{
		std::cout << "\t- Card #" << iter->first << ": " << iter->second << " instance(s)" << std::endl;
		totalCardCount += iter->second;
	}
	std::cout << "The total number of scratch-cards is " << totalCardCount << '!' << std::endl;
#endif // ~DO_PART_1
}

static void extractNumbers(std::string str, std::vector<uint32_t>& numberVector)
{
	// Just try to regex-search for all numbers (separated by space(s)) and push them in the vector
	for (std::smatch numMatch ; std::regex_search(str, numMatch, sc_numberRegex) ; str = numMatch.suffix())
	{
		numberVector.push_back(static_cast<uint32_t>(std::atol(numMatch[1].str().c_str())));
	}
}

static uint32_t getMatchingNumberCount(const ScratchCard& card)
{
	// Count the number of our numbers present in the list of winning numbers.
	// I won't use anything fancy here, i'm just going to check for a number using linear search...
	uint32_t matchCount = 0u;
	for (uint32_t cardNum : card.cardNumbers)
	{
		for (uint32_t winningNum : card.winningNumbers)
		{
			if (cardNum == winningNum)
			{
				matchCount++;
				break;
			}
		}
	}
	return matchCount;
}