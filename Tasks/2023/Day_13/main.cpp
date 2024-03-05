#include <AoC-Module.h>
#include <vector>

static size_t getBlockScore();
static void transposeLines();

static size_t s_blockScoreSum = 0;
static size_t s_blockCount = 0;
static std::vector<std::string> s_lines;

bool handleLine(const std::string& line)
{
	if (line.empty())
	{
		size_t blockSum = getBlockScore();
		std::cout << "Block #" << ++s_blockCount << " got a score of " << blockSum << std::endl;
		s_blockScoreSum += blockSum;
		s_lines.clear();
		return true;
	}
	s_lines.push_back(line);
	return true;
}

void finalize()
{
	std::cout << "The block score sum is " << s_blockScoreSum << '!' << std::endl;
}

static size_t getBlockScore()
{
	if (s_lines.empty())
		return 0;
	
	for (size_t round = 0, scalar = 100 ; round < 2 ; round++, scalar = 1)
	{
		//	std::cout << "Round #" << (round+1) << std::endl;
		for (size_t mirroAxis = 1 ; mirroAxis < s_lines.size() ; mirroAxis++ )
		{
			size_t count = std::min(mirroAxis, s_lines.size() - mirroAxis);
			//	std::cout << "Trying to match " << count << " rows/columns..." << std::endl;
			bool matches = true;
#ifndef DO_PART_1
			char* smudge = nullptr;
#endif // ~DO_PART_1
			for (size_t i = 0 ; i < count ; i++)
			{
				std::string& str1 = s_lines[mirroAxis + i];
				std::string& str2 = s_lines[mirroAxis - i - 1];
				for (char* c1 = &str1[0], *c2 = &str2[0] ; *c1 != '\0' && matches ; c1++, c2++)
				{
					if (*c1 != *c2)
					{
#ifndef DO_PART_1
						if (!smudge)
						{
							smudge = c1;
							continue;
						}
#endif // ~DO_PART_1
						matches = false;
						break;
					}
				}
			}
			if (matches)
			{
#ifndef DO_PART_1
				if (smudge)
				{
					//	std::cout << "Found a smudge!" << std::endl;
					*smudge = (*smudge == '#' ? '.' : '#');
				}
#endif // ~DO_PART_1
				//	std::cout << "Mirror after " << mirroAxis << " lines from the left/top!" << std::endl;
				return scalar * mirroAxis;
			}
		}
		//	std::cout << "Before transposition:" << std::endl;
		//	for (const auto& str : s_lines)
		//		std::cout << str << std::endl;
		transposeLines();
		//	std::cout << "\nAfter transposition:" << std::endl;
		//	for (const auto& str : s_lines)
		//		std::cout << str << std::endl;
	}

	return 0;
}

static void transposeLines()
{
	std::vector<std::string> transposedLines(s_lines[0].length());
	for (size_t newStrIdx = 0 ; newStrIdx < transposedLines.size() ; newStrIdx++)
	{
		std::string& str = transposedLines[newStrIdx];
		size_t charIndex = newStrIdx;
		for (size_t j = 0 ; j < s_lines.size() ; j++)
		{
			str.push_back(s_lines[j][charIndex]);
		}
	}
	s_lines = std::move(transposedLines);
}
