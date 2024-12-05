#include <AoC-Module.h>
#include <vector>
#include <unordered_map>
#include <regex>

template <typename IntType>
inline static IntType strToInt(const std::string& str)
{
	static_assert(std::is_integral_v<IntType>, "IntType must be an integer type!");
	IntType retVal;
	(std::stringstream{} << str >> retVal);
	return retVal;
}

struct PageOrderRule
{
public:
	PageOrderRule(uint32_t pageA, uint32_t pageB)
		: m_pageA{pageA}, m_pageB{pageB}
	{}
public:
	uint32_t getPageA() const noexcept { return m_pageA; }
	uint32_t getPageB() const noexcept { return m_pageB; }
	bool update(uint32_t page)
	{
		if (page == m_pageA)
		{
			m_aIsPresent = true;
			if (m_bIsPresent)
			{
				return false;
			}
		}
		else if (page == m_pageB)
		{
			m_bIsPresent = page;
		}
		return true;
	}
	void reset()
	{
		m_aIsPresent = false;
		m_bIsPresent = false;
	}
#ifndef PART_1
	void fix(std::vector<uint32_t>& pageOrder) const
	{
		// First find a and b (there should only be one of each)
		auto aIter = std::find_if(pageOrder.begin(), pageOrder.end(), [&](uint32_t pageNum){ return pageNum == m_pageA; });
		auto bIter = std::find_if(pageOrder.begin(), pageOrder.end(), [&](uint32_t pageNum){ return pageNum == m_pageB; });

		// Then check if we've even got both positions. If not, we can't (and won't) swap
		if (aIter == pageOrder.end() || bIter == pageOrder.end())
		{
			return;
		}

		// Check if a is past b. If so, swap them
		if (aIter > bIter)
		{
			auto temp = *aIter;
			*aIter = *bIter;
			*bIter = temp;
		}
	}
#endif
private:
	uint32_t m_pageA;
	uint32_t m_pageB;
	bool m_aIsPresent{false};
	bool m_bIsPresent{false};
};

std::vector<PageOrderRule> s_rules{};
std::unordered_map<uint32_t, std::vector<PageOrderRule*>> s_pageToRulesMap{};
std::vector<uint32_t> s_updateMiddlePages{};

void initialize(uint64_t lineCount)
{
	// Due to the fact that the input-file also contains the updates themselves,
	// this will make the vector a bit bigger than needed... but that's *fine*
	s_rules.reserve(lineCount);
}

bool handleLine(const std::string& line)
{
	static const std::regex RULE_REGEX{ R"(^(\d+)\|(\d+)$)" };
	static const std::regex UPDATE_REGEX{ R"((\d+),?)" };
	static bool isInUpdateSection{false};

	if (line == "")
	{
		isInUpdateSection = true;
		return true;
	}

	std::smatch strMatch;
	if (!isInUpdateSection)
	{
		if (!std::regex_search(line, strMatch, RULE_REGEX))
		{
			std::cerr << "Line \"" << line << "\" doesn't match the rule's regex!" << std::endl;
			return false;
		}

		uint32_t pageA = strToInt<uint32_t>(strMatch[1]);
		uint32_t pageB = strToInt<uint32_t>(strMatch[2]);

		// std::cout << "Page A: " << pageA << " | Page B: " << pageB << std::endl;
		
		PageOrderRule* rulePtr = &s_rules.emplace_back(pageA, pageB);
		s_pageToRulesMap[pageA].emplace_back(rulePtr);
		s_pageToRulesMap[pageB].emplace_back(rulePtr);
		
		return true;
	}

	// For the checks, reset the rules for the check first
	for (const auto& [_, pageRuleVec] : s_pageToRulesMap)
	{
		for (auto* pageRule : pageRuleVec)
		{
			pageRule->reset();
		}
	}

	// Then convert the update to a list of pages
	auto regBeg = std::sregex_iterator(line.cbegin(), line.cend(), UPDATE_REGEX);
	auto regEnd = std::sregex_iterator();
	uint32_t pageCount = std::distance(regBeg, regEnd);
	std::vector<uint32_t> updatePages{};
	updatePages.reserve(pageCount);
	for (auto iter = regBeg ; iter != regEnd ; iter++)
	{
		updatePages.emplace_back(strToInt<uint32_t>((*iter)[1]));
	}

	// Then check if the update is correct
#ifndef PART_1
	bool bIsValid = false;
#endif
	for (uint32_t page : updatePages)
	{
		const auto iter = s_pageToRulesMap.find(page);
		if (iter == s_pageToRulesMap.cend())
			continue;
		
		for (PageOrderRule* rule : iter->second)
		{
			if (!rule->update(page))
			{
#ifndef PART_1
				if (!bIsValid)
#endif
				std::cout << "Rule doesn't apply for update \"" << line << "\"!" << std::endl;
#ifdef PART_1
				return true; // Kick out rules that DON'T apply
#else
				bIsValid = true;
#endif
			}
		}
	}

#ifndef PART_1
	if (!bIsValid)
	{
		return true;
	}

	std::cout << "SANITY!" << std::endl;

	// Now let the update get fixed by all rules that take part in the update.
	// Do that by first creating a vector with all rules and then checking against them
	std::vector<PageOrderRule*> appliedRules;
	for (uint32_t page : updatePages)
	{
		auto iter = s_pageToRulesMap.find(page);
		if (iter == s_pageToRulesMap.end())
			continue;
		
		appliedRules.insert(appliedRules.end(), iter->second.begin(), iter->second.end());
	}

	// Now let the rules fix the update
	for (const auto* pageOrderRule : appliedRules)
	{
		pageOrderRule->fix(updatePages);
	}

	std::cout << "The fixed rule reads: \"";
	for (uint32_t x : updatePages)
	{
		std::cout << x << ',';
	}
	std::cout << '"' << std::endl;
#endif

	// If we get here, the update is fine. So get
	// the middle page number and add it to the vector
	s_updateMiddlePages.push_back(updatePages[(pageCount/2u)]);

	return true;
}

void finalize()
{
	uint64_t sum = 0;
	for (uint32_t page : s_updateMiddlePages)
	{
		sum += page;
	}
	std::cout << "The sum of all middle page numbers of all successfull updates is " << sum << " !" << std::endl;
}
