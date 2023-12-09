#include <AoC-Module.h>
#include <vector>
#include <regex>

const static std::string sc_numberRegexStr = R"((-?[0-9]+)\s*)";
const static std::string sc_numberLineRegexStr = "(?:" + sc_numberRegexStr + ")+";

const static std::regex sc_numberRegex(sc_numberRegexStr);
const static std::regex sc_numberLineRegex(sc_numberLineRegexStr);

class Report
{
public:
	using ValueVector = std::vector<int64_t>;
	Report(ValueVector&& values = {})
		: m_valueVectors(std::vector<ValueVector>(1, values))
	{}
	~Report() = default;
public:
	int64_t getNextValue();
private:
	ValueVector generateDifferenceVector(ValueVector& dataSource);
private:
	std::vector<ValueVector> m_valueVectors;
};

static bool isAllZero(const Report::ValueVector& vec);

std::vector<Report> s_allReports;

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	if (!std::regex_match(line, sc_numberLineRegex))
	{
		std::cerr << "Line '" << line << "' doesn't match line-regex!" << std::endl;
		return false;
	}

	std::vector<int64_t> vec;
	std::string restLine = line;
	for (std::smatch match ; std::regex_search(restLine, match, sc_numberRegex) ; restLine = match.suffix())
	{
		vec.push_back(std::stoll(match[1].str()));
	}
#ifndef DO_PART_1
	std::reverse(vec.begin(), vec.end());
#endif // ~DO_PART_1
	s_allReports.emplace_back(Report(std::move(vec)));

	return true;
}

void finalize()
{
	int64_t sumOfNewValues = 0ll;
	for (Report& report : s_allReports)
	{
		int64_t nextValue = report.getNextValue();
		std::cout << "Report reported next value as " << nextValue << "..." << std::endl;
		sumOfNewValues += nextValue;
	}
	
	std::cout << "The sum of all new generated values is " << sumOfNewValues << '!' << std::endl;
}

Report::ValueVector Report::generateDifferenceVector(Report::ValueVector& dataSource)
{
	if (dataSource.size() < 2)
		return {};
	
	ValueVector newVec;
	for (size_t i = 1 ; i < dataSource.size() ; i++)
	{
		newVec.push_back(dataSource[i] - dataSource[i-1]);
	}
	return newVec;
}

int64_t Report::getNextValue()
{
	if (m_valueVectors.empty())
		return -1;
	
	std::cout << "Filling up vectors until their values are all 0..." << std::endl;
	for (auto lastIter = m_valueVectors.rbegin() ; !isAllZero(*lastIter) ; lastIter = m_valueVectors.rbegin())
	{
		m_valueVectors.push_back(generateDifferenceVector(*lastIter));
	}
	std::cout << "Done! Report now has " << m_valueVectors.size() << " vectors!" << std::endl;
	
	size_t retVal = 0;
	for (auto rIter = m_valueVectors.rbegin() + 1 ; rIter != m_valueVectors.rend() ; rIter++)
	{
		auto rBeforeIter = rIter - 1;
		if (rBeforeIter == m_valueVectors.rbegin())
			rBeforeIter->push_back(0);
		
		size_t lastIndex = rIter->size() - 1;
		retVal = (*rIter)[lastIndex] + (*rBeforeIter)[lastIndex];
		rIter->push_back(retVal);
	}
	return retVal;
}

static bool isAllZero(const Report::ValueVector& vec)
{
	for (int64_t val : vec)
	{
		if (val != 0)
			return false;
	}
	return true;
}
