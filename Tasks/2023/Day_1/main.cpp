#include <iostream>
#include <vector>

#include <AoC-Module.h>

static uint32_t digitSum = 0u;
#ifndef DO_PART_1
const static std::string sc_numberStrings[] =
{
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};
constexpr static size_t sc_numberStringsSize = sizeof(sc_numberStrings)/sizeof(sc_numberStrings[0]);
#endif

bool handleLine(const std::string& line)
{
    std::vector<uint32_t> nums;
    #ifdef DO_PART_1
    for (char c : line)
    {
        if (isdigit(c))
            nums.push_back((uint32_t)(c - '0'));
    }
    #else
    for (uint32_t idx = 0 ; idx < line.length() ; idx++)
    {
        char c = line[idx];
        if (isdigit(c))
            nums.push_back((uint32_t)(c - '0'));
        else
        {
            const std::string* numStr = &sc_numberStrings[0];
            for (size_t i = 0 ; i < sc_numberStringsSize ; i++, numStr++)
            {
                if (line.find(*numStr, idx) == idx)
                {
                    //	std::cout << "Found \"" << *numStr << "\" at idx " << idx << "!\n   " << line << "\n   ";
                    //	for (uint32_t y = 0 ; y < idx ; y++)
                    //	    std::cout << ' ';
                    //	for (uint32_t y = 0 ; y < numStr->length() ; y++)
                    //	    std::cout << '~';
                    //	std::cout << std::endl;
                    nums.push_back(i+1);
                    //  idx += numStr->length()-1;
                    break;
                }
            }
        }
    }
    #endif
    if (nums.size() == 0)
        return true;
    
    //	std::cout << "All stored numbers for this line (\"" << line << "\") are:" << std::endl;
    //	for (const auto& n : nums)
    //	{
    //	    std::cout << "\t- " << n << std::endl;
    //	}

    digitSum += (nums[0] * 10) + nums[nums.size()-1];
    return true;
}

void finalize()
{
    std::cout << "The sum of all 2 digit numbers is " << digitSum << '!' << std::endl;
}