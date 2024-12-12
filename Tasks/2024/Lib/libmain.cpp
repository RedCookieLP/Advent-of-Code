#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include <AoC-Module.h>

// In case we don't need or want this function, use this as a fallback
#ifdef _MSC_FULL_VER
extern "C" void __cdecl no_initialize(MAYBE_UNUSED uint64_t lineCount) {}
#else
void __attribute__((weak)) initialize(MAYBE_UNUSED uint64_t lineCount) {}
#endif

int main(int argc, const char** argv)
{
	const auto cwd = std::filesystem::current_path();
    std::string line;
    std::string thisDay = std::filesystem::path{argv[0]}.filename().stem().string();
	auto filePath = cwd / "Tasks" / "2024" / thisDay;
    if (argc > 1)
	{
		filePath /= argv[1];
	}
    else
	{
        filePath /= "input.txt";
	}

	std::cout << "Using file \"" << filePath << "\"!" << std::endl;
    std::ifstream file(filePath);
	if (!file)
	{
		std::cout << "Failed to open file!" << std::endl;
		return EXIT_FAILURE;
	}

	// Count the number of newlines in the file for `initialize()`
	uint64_t newlineCount = std::count(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>{},
		'\n'
	);
	initialize(newlineCount + 1u);

	file.seekg(0, file.beg);

    bool earlyBreak = false;
    while (file)
    {
        std::getline(file, line);
    	if (!handleLine(line))
        {
			std::cout << " --- Early break! ---" << std::endl;
            earlyBreak = true;
            break;
        }
    }
    file.close();
    if (!earlyBreak)
        finalize();

    return EXIT_SUCCESS;
}