#include <iostream>
#include <fstream>

#include <AoC-Module.h>

int main(int argc, const char** argv)
{
	std::string exeDir = argv[0];
	size_t lastSlashPos = exeDir.find_last_of('\\');
	exeDir = exeDir.substr(0, (lastSlashPos != std::string::npos ? lastSlashPos + 1 : lastSlashPos));
	
    std::string line;
    std::string filePath;
    if (argc > 1)
        filePath = argv[1];
    else
        filePath = exeDir + "input.txt";
    std::ifstream file(filePath);
	if (!file)
	{
		std::cout << "Failed to open file!" << std::endl;
		return EXIT_FAILURE;
	}
    bool earlyBreak = false;
    while (file)
    {
        std::getline(file, line);
    	if (!handleLine(line))
        {
            earlyBreak = true;
            break;
        }
    }
    file.close();
    if (!earlyBreak)
        finalize();

    return EXIT_SUCCESS;
}