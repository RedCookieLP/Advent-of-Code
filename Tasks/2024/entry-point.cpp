#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include <AoC-Module.h>

// In case we don't need or want this function, use this as a fallback
void __attribute__((weak)) initialize([[maybe_unused]] uint64_t lineCount) {}

int main(int argc, const char** argv)
{
	std::string exeDir = argv[0];
	size_t lastSlashPos = exeDir.find_last_of("\\/");
	exeDir = exeDir.substr(0, (lastSlashPos != std::string::npos ? lastSlashPos + 1 : lastSlashPos));
	
    std::string line;
    std::string filePath;
    if (argc > 1)
	{
		const std::string relPrefix("rel:");
		std::string rawPath = argv[1];

		// If the specified path starts with "rel:", treat the path as a path relative to the executables path
		if (rawPath.substr(0, relPrefix.length()) == relPrefix)
		{
			filePath = exeDir;
			rawPath = rawPath.substr(relPrefix.length());
		}
		filePath += rawPath;
	}
    else
	{
        filePath = "input.txt";
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