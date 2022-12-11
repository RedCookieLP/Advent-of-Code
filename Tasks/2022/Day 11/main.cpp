#include <iostream>
#include <fstream>
#include <string>
#include <Monkey.h>
#include <vector>
#include <algorithm>

constexpr static size_t sc_lineLen = 75;

static std::vector<Monkey> s_monkeyVec;

static bool cmpMonkeys(const Monkey& m1, const Monkey& m2)
{
    return (m1.getInspectionCount() > m2.getInspectionCount());
}

int main(int argc, const char** argv)
{
    std::ifstream file("input.txt");
    while (file)
    {
        std::string paragraph;
        size_t lineLen = 0;
        do
        {
            std::string line(sc_lineLen, '\0');
            file.getline(line.data(), sc_lineLen, '\n');
            lineLen = std::strlen(line.c_str());

            if (lineLen != 0)
            {
                paragraph += line.c_str();
                paragraph += '\n';
            }
        }
        while (lineLen != 0);

        s_monkeyVec.emplace_back(paragraph);
    }
    file.close();

    size_t rounds = 0;
    while (rounds != 10000)
    {
        //  std::cout << " --- Playing round #" << (rounds + 1) << "! ---" << std::endl;

        for (Monkey& monkey : s_monkeyVec)
        {
            while (monkey.inspectItem())
                monkey.throwItem(s_monkeyVec[monkey.getThrowingTarget()]);
        }

        rounds++;

        if (rounds == 1 || rounds == 20 || (rounds % 1000) == 0)
        {
            std::cout << "Results after " << rounds << " Round(s):" << std::endl;
            for (size_t i = 0 ; i < s_monkeyVec.size() ; i++)
            {
                const Monkey& m = s_monkeyVec[i];
                std::cout << "\tMonkey #" << i << ": " << m.getInspectionCount() << std::endl;
            }
        }
    }

    /*
    std::cout << "20 Rounds are up! The monkeys had the following activities:\n" << std::endl;
    for (size_t i = 0 ; i < s_monkeyVec.size() ; i++)
    {
        const Monkey& m = s_monkeyVec[i];
        std::cout << "Monkey #" << i << ": " << m.getInspectionCount() << std::endl;
    }
    */

    std::sort(s_monkeyVec.begin(), s_monkeyVec.end(), cmpMonkeys);

    size_t most = s_monkeyVec[0].getInspectionCount();
    size_t second = s_monkeyVec[1].getInspectionCount();
    
    std::cout << "The 2 monkeys with the most inspections are " << most << " & " << second << '\n'
              << "The resulting monkey business value is "<< most*second << '!' << std::endl;

    return EXIT_SUCCESS;
}