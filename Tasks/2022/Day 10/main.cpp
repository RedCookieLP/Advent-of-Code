#include <iostream>
#include <fstream>
#include <string>

constexpr static size_t sc_lineLen = 10;
constexpr static size_t CRT_WIDTH = 40;
constexpr static size_t CRT_HEIGHT = 6;
constexpr static size_t CRT_SCREEN_CAP = CRT_WIDTH * CRT_HEIGHT;

inline char getCRTChar(int x, int screenPos)
{
    int dif = x - screenPos;
    return ((-1 <= dif && dif <= 1) ? '#' : '.');
}

int main(int argc, const char** argv)
{
    int x = 1;
    size_t clockCycle = 0;
    char arrCRT[CRT_SCREEN_CAP];
    for (size_t i = 0 ; i < CRT_SCREEN_CAP ; i++)
        arrCRT[i] = '.';

    std::ifstream file("input.txt");
    while (file)
    {
        std::string line(sc_lineLen,'\0');
        file.getline(line.data(), sc_lineLen);
        const size_t lineLen = std::strlen(line.c_str());
        if (lineLen == 0)
            continue;
        
        int change = 0;
        std::string operation = line.substr(0,4);
        if (operation == "noop")
        {
            arrCRT[clockCycle] = getCRTChar(x, clockCycle % 40);
            clockCycle++;
        }
        else if (operation == "addx")
        {
            arrCRT[clockCycle] = getCRTChar(x, clockCycle % 40);
            clockCycle++;
            arrCRT[clockCycle] = getCRTChar(x, clockCycle % 40);
            clockCycle++;
            x += std::atoi(line.substr(5).c_str());
        }
        else
        {
            std::cout << "Unkown operation \"" << operation.c_str() << "\"!" << std::endl;
        }
    }
    file.close();

    std::cout << "Image of the CRT:\n\n";
    for (size_t y = 0 ; y < CRT_HEIGHT ; y++)
    {
        for (size_t x = 0 ; x < CRT_WIDTH ; x++)
        {
            std::cout << arrCRT[(y * CRT_WIDTH) + x];
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}