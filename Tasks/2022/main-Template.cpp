#include <iostream>
#include <fstream>
#include <string>

constexpr static size_t sc_lineLen = 10;

int main(int argc, const char** argv)
{
    std::ifstream file("input.txt");
    while (file)
    {
        std::string line(sc_lineLen,'\0');
        file.getline(line.data(), sc_lineLen);
        const size_t lineLen = std::strlen(line.c_str());
        if (lineLen == 0)
            continue;
    }
    file.close();

    return EXIT_SUCCESS;
}