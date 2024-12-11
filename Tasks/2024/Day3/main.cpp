#include <AoC-Module.h>
#include <regex>
#include <sstream>

#ifdef PART_1
static const std::regex sc_mulRegex{ R"(mul\((\d{1,3}),(\d{1,3})\))" };
#else
static const std::regex sc_mulRegex{ R"(mul\((\d{1,3}),(\d{1,3})\)|do(?:n't)?\(\))" };
#endif
static uint64_t s_result{0ull};

bool handleLine(const std::string& line)
{
	std::smatch strMatch;
#ifndef PART_1
	// Must be static because by the rules of the puzzle "at the **beginning of the program**,
	// `mul` instructions are enabled", not on every new line
	static bool enabled = true;
#endif
	for (std::string str = line ; std::regex_search(str, strMatch, sc_mulRegex) ; str = strMatch.suffix())
	{
		//	std::cout << "Match: \"" << strMatch[0] << '"' << std::endl;
#ifndef PART_1
		if (strMatch[0] == "do()" || strMatch[0] == "don't()")
		{
			enabled = (strMatch[0] == "do()");
			continue;
		}
		if (!enabled)
		{
			continue;
		}
#endif
		uint32_t a, b;
		{std::stringstream{} << strMatch[1] >> a;}
		{std::stringstream{} << strMatch[2] >> b;}
		s_result += (a * b);
	}
	return true;
}

void finalize()
{
	std::cout << "The sum of all multiplications is " << s_result << " !" << std::endl;
}
