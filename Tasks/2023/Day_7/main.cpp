#include <AoC-Module.h>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <functional>
#include <vector>
#include <regex>
#ifdef WINDOOF
#	include <Windows.h>
#endif // WINDOOF

// Heads up: this code went a bit off-the-rails and is thereby not organized/written cleanly in some...

enum class Card
{
#ifndef DO_PART_1
	// Value of J's for part 2
	SYM_J,
#endif // ~DO_PART_1
	NUM_2,
	NUM_3,
	NUM_4,
	NUM_5,
	NUM_6,
	NUM_7,
	NUM_8,
	NUM_9,
	NUM_T,
#ifdef DO_PART_1
	// Value of J's for part 1
	SYM_J,
#endif // DO_PART_1
	SYM_Q,
	SYM_K,
	SYM_A,
	UNKNOWN,
};

char cardToChar(Card card)
{
	switch (card)
	{
		case Card::NUM_2:
			return '2';
		case Card::NUM_3:
			return '3';
		case Card::NUM_4:
			return '4';
		case Card::NUM_5:
			return '5';
		case Card::NUM_6:
			return '6';
		case Card::NUM_7:
			return '7';
		case Card::NUM_8:
			return '8';
		case Card::NUM_9:
			return '9';
		case Card::NUM_T:
			return 'T';
		case Card::SYM_J:
			return 'J';
		case Card::SYM_Q:
			return 'Q';
		case Card::SYM_K:
			return 'K';
		case Card::SYM_A:
			return 'A';
		case Card::UNKNOWN:
			return '?';
	}
}

Card charToCard(char c)
{
	// Numbers 0 and 1 do not count...
	if (isdigit(c) && c > '1')
	{
		// This calculation is done this way thx to part-2's changed value for J...
		// Basically this is "NUM_2 + (c - '2')", but with static_casts because modern C++ and enum-classes
		return static_cast<Card>(static_cast<size_t>(Card::NUM_2) + static_cast<size_t>(c - '2'));
	}
	switch(c)
	{
		case 'T':
			return Card::NUM_T;
			break;
		case 'J':
			return Card::SYM_J;
			break;
		case 'Q':
			return Card::SYM_Q;
			break;
		case 'K':
			return Card::SYM_K;
			break;
		case 'A':
			return Card::SYM_A;
			break;
		default:
			return Card::UNKNOWN;
	}
}

enum class HandType
{
	FIVE_OF_A_KIND,
	FOUR_OF_A_KIND,
	FULL_HOUSE,
	THREE_OF_A_KIND,
	TWO_PAIRS,
	ONE_PAIR,
	HIGH_CARD
};

std::string handTypeToString(HandType type)
{
	switch (type)
	{
		case HandType::FIVE_OF_A_KIND:
			return "FIVE_OF_A_KIND";
		case HandType::FOUR_OF_A_KIND:
			return "FOUR_OF_A_KIND";
		case HandType::FULL_HOUSE:
			return "FULL_HOUSE";
		case HandType::THREE_OF_A_KIND:
			return "THREE_OF_A_KIND";
		case HandType::TWO_PAIRS:
			return "TWO_PAIRS";
		case HandType::ONE_PAIR:
			return "ONE_PAIR";
		case HandType::HIGH_CARD:
			return "HIGH_CARD";
		default:
			return "<Unknown>";
	}
}

#ifndef DO_PART_1
constexpr static const char* const RESET = "\x1b[0m";
constexpr static const char* const YELLOW = "\x1B[1;33m";
#endif // ~DO_PART_1

// First time using classes for AoC 2023 \o/
class Hand
{
public: // Con-/Destructors
	Hand()
		: m_cards({}), m_type(HandType::HIGH_CARD), m_handStr()
	{}
	Hand(const std::string& handStr)
		: m_cards({}), m_type(HandType::HIGH_CARD), m_handStr()
	{
		// Create a new, temporary hand from string
		Hand tempHand;
		// If it fails, leave this hand empty (great design, I know!)
		if (!fromString(handStr, tempHand))
			return;
		
		// Otherwise, move all the stuff over
		m_cards = std::move(tempHand.m_cards);
#ifndef DO_PART_1
		m_optimalCards = std::move(tempHand.m_optimalCards);
#endif // ~DO_PART_1
		m_type = std::move(tempHand.m_type);
		m_handStr = handStr;
	}
public: // Static functions
	static bool fromString(const std::string& handStr, Hand& hand);
public: // Methods
	const auto& getCards() const { return m_cards; }
	HandType getType() const { return m_type; }
	const std::string& getString() const { return m_handStr; }
public: // Operators
	Card operator[](size_t idx) const { return m_cards[idx]; }
	bool operator==(const Hand& rhs) const { return (rhs.m_cards == m_cards); }
	bool operator<(const Hand& rhs) const { return rhs > *this; }
	bool operator>(const Hand& rhs) const;
private: // Methods
	void evaluateHandType();
#ifndef DO_PART_1
	void optimizeJokers();
#endif // ~DO_PART_1
private: // Members
	std::array<Card,5ULL> m_cards;
#ifndef DO_PART_1
	std::array<Card,5ULL> m_optimalCards{};
#endif // ~DO_PART_1
	HandType m_type;
	std::string m_handStr;
};

struct BidEntry
{
	Hand hand;
	size_t bidAmount;
};

const static std::regex sc_lineRegex(R"(^([2-9TJQKA]{5})\s+(\d+)$)");
static std::vector<BidEntry> s_allBids;

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	if (!std::regex_match(line, sc_lineRegex))
	{
		std::cerr << "Line \"" << line << "\" doesn't match regex!" << std::endl;
		return false;
	}

	std::smatch match;
	if (!std::regex_search(line, match, sc_lineRegex) || match.size() != 3)
	{
		std::cerr << "regex_search failed!" << std::endl;
		return false;
	}

	std::string handStr = match[1].str();
	// Found out that "std::stoXYZ" exists to convert std::string to a number!
	size_t bidAmount = std::stoull(match[2].str());

	Hand newHand;
	if (!Hand::fromString(handStr, newHand))
	{
		std::cerr << "Failed to parse hand-string \"" << handStr << "\" to a valid hand!" << std::endl;
		return false;
	}

	s_allBids.emplace_back(BidEntry{ newHand, bidAmount });

	return true;
}

void finalize()
{
	#ifdef WINDOOF
	// Windows-stuff, enable ANSI-escape-codes
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif // WINDOOF

	std::cout << "All lines have been parsed to the following BidEntries:" << std::endl;

	// Sort the entries by the "strength" of the hand. Unlike before, sort in ascending order.
	// If the hands are matching, compare them by the bid-amount.
	// This makes calculating the winning amount way easier, since now the rank of a hand is the index (plus 1)
	std::sort(s_allBids.begin(), s_allBids.end(), [](const BidEntry& left, const BidEntry& right) -> bool
	{
		if (left.hand < right.hand)
			return true;
		else if (left.hand == right.hand)
			return (left.bidAmount < right.bidAmount);
		return false;
	});

	size_t totalWinnings = 0;
	size_t rank = 1;
	for (const BidEntry& play : s_allBids)
	{
		totalWinnings += play.bidAmount * rank;
		std::cout << "  - Hand: [rank: " << rank++ << ", str: \"" << play.hand.getString() << "\", type: " << handTypeToString(play.hand.getType()) << "] - bid-amount: " << play.bidAmount << std::endl;
	}
	std::cout << "This results in a winnings-total of " << totalWinnings << '!' << std::endl;
}

bool Hand::fromString(const std::string& handStr, Hand& hand)
{
	if (handStr.length() < 5)
		return false;
	
	for (size_t i = 0 ; i < 5 ; i++)
	{
		char c = handStr[i];
		Card newCard = charToCard(c);
		if (newCard == Card::UNKNOWN)
			return false;
		hand.m_cards[i] = newCard;
	}

	hand.m_handStr = handStr;
#ifndef DO_PART_1
	hand.optimizeJokers();
#endif // ~DO_PART_1
	hand.evaluateHandType();

	return true;
}

bool Hand::operator>(const Hand& rhs) const
{
	if (m_type < rhs.m_type)
		return true;
	else if (m_type > rhs.m_type)
		return false;
	
	bool doPrint = (m_cards[0] == Card::SYM_J || rhs.m_cards[0] == Card::SYM_J);
	doPrint = false;

	if (doPrint)
		std::cout << "Comparing lhs \"" << getString() << "\" > rhs \"" << rhs.getString() << "\"..." << std::endl;
	
	for (uint32_t i = 0 ; i < m_cards.size() ; i++)
	{
		const Card& lhsCard = m_cards[i];
		const Card& rhsCard = rhs[i];
		if (doPrint)
			std::cout << "  Card '" << cardToChar(lhsCard) << "' < '" << cardToChar(rhsCard) << "'?..." << std::endl;
		if (lhsCard > rhsCard)
		{
			if (doPrint)
				std::cout << "  Card #" << i << " --> True!" << std::endl;
			return true;
		}
		else if (lhsCard < rhsCard)
		{
			if (doPrint)
				std::cout << "  Card #" << i << " --> False!" << std::endl;
			return false;
		}
	}
	if (doPrint)
		std::cout << " --> False!" << std::endl;
	return false;
}

void Hand::evaluateHandType()
{
	/* 
	To make the evaluation as easy as possible, we consider the number of different cards for each hand type:
		- FIVE_OF_A_KIND
			==> all cards are the same type
			==> must be 1 different type of cards
		- FOUR_OF_A_KIND
			==> all but 1 card share a type
			==> must be 2 different types of cards
		- FULL_HOUSE
			==> three cards of 1 type and 2 cards of another type
			==> must be 2 different types of cards
		- THREE_OF_A_KIND
			==> three cards of 1 type and 2 cards of 2 different types
			==> must be 3 different types of cards
		- TWO_PAIRS
			==> two pairs of two card types and 1 card of another type
			==> must be 3 different types of cards
		- ONE_PAIR
			==> a pair of cards of 1 type and 3 cards of 3 different types
			==> must be 4 different types of cards
		- HIGH_CARD
			==> all cards are different types
			==> must be 5 different types of cards
	
	To keep track of the number of cards per Type AND the number of types, I'll throw an unordered map at this 5 item problem :)
	*/
	std::unordered_map<Card/*type of card*/, size_t/*card-count for type*/> cardMap{};
#ifdef DO_PART_1
	// When doing part 1, we'll use the cards in m_cards...
	for (const Card& card : m_cards)
		cardMap[card]++;
#else // DO_PART_1
	// When doing part 2, we'll use the optimized deck in m_optimalCards...
	for (const Card& card : m_optimalCards)
		cardMap[card]++;
#endif // ~DO_PART
	
	// Now push all card-type-assigned card counts in a vector
	std::vector<size_t> cardStats;
	for (auto iter = cardMap.cbegin() ; iter != cardMap.cend() ; iter++)
		cardStats.push_back(iter->second);
	
	// Sort the vector by card count in descending order
	std::sort(cardStats.begin(), cardStats.end(), std::greater<size_t>());

	// Since the keys of the map were the card types, the number of card types is the size of the vector!
	size_t cardTypeCount = cardStats.size();
	// And since the vector has been sorted by value (each card type's card count) in descending order,
	// the first value is the greatest number of cards of all types
	size_t highestCardTypeAmount = cardStats[0];
	
	// Now check the type of the hand
	// FIVE_OF_A_KIND: 1 type == 1 entry
	if (cardTypeCount == 1)
		m_type = HandType::FIVE_OF_A_KIND;
	// FOUR_OF_A_KIND: 2 types, the highest card count must be 4
	else if (cardTypeCount == 2 && highestCardTypeAmount == 4)
		m_type = HandType::FOUR_OF_A_KIND;
	// FULL_HOUSE: 2 types, the highest card count must be 3
	else if (cardTypeCount == 2 && highestCardTypeAmount == 3)
		m_type = HandType::FULL_HOUSE;
	// THREE_OF_A_KIND: 3 types, the highest card count must be 3
	else if (cardTypeCount == 3 && highestCardTypeAmount == 3)
		m_type = HandType::THREE_OF_A_KIND;
	// TWO_PAIRS: 3 types, the highest card count must be 2
	else if (cardTypeCount == 3 && highestCardTypeAmount == 2)
		m_type = HandType::TWO_PAIRS;
	// ONE_PAIR: 4 types, with 5 cards per hand one type must have 2 cards (the pair)
	else if (cardTypeCount == 4)
		m_type = HandType::ONE_PAIR;
	// HIGH_CARD: 5 types, no cards share a type...
	else // Last possible type, so no check necessary
		m_type = HandType::HIGH_CARD;
}

#ifndef DO_PART_1
void Hand::optimizeJokers()
{
	// We'll construct the optimal deck in m_optimalCards.
	// Even if this hand doesn't have any jokers, we must fill this array, since it's values are used when determining the hand's type
	m_optimalCards = m_cards;

	// If this hand doesn't have jokers, then there's nothing to do
	if (m_handStr.find('J') == std::string::npos)
		return;
	
	// Finding out with which card type to replace the jokers with is actually super simple:
	// replace them for the card type with the most amount of cards
	Card replacementCard = Card::UNKNOWN;
	
	// Lazy approach again: build an unordered map from the array
	std::unordered_map<Card/*type of card*/, size_t/*card-count for type*/> cardMap{};
	for (const Card& card : m_optimalCards)
		cardMap[card]++;
	
	// Now search for the pair with the greatest count...
	std::pair<Card, size_t> greatestPair{ Card::UNKNOWN, 0 };
	for (const auto& pair : cardMap)
	{
		if (pair.first != Card::SYM_J && (pair.second > greatestPair.second || (pair.second == greatestPair.second && pair.first > greatestPair.first)))
			greatestPair = pair;
	}

	// Now, if the card-type with the greatest card amount has been found, then replace all jokers with that type.
	// If that card-type is unknown (meaning: all we have is jokers), then replace them with aces
	replacementCard = (greatestPair.first == Card::UNKNOWN ? Card::SYM_A : greatestPair.first);
	for (Card& card : m_optimalCards)
	{
		if (card == Card::SYM_J)
			card = replacementCard;
	}

	// Also replace them in the string, for clarity...
	for (size_t cPos = 0 ; (cPos = m_handStr.find_first_of('J', cPos)) != std::string::npos ; )
		m_handStr.replace(cPos, 1, std::string(YELLOW) + cardToChar(replacementCard) + RESET);
	}
#endif // ~DO_PART_1
