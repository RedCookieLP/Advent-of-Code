#include <iostream>
#include <fstream>

bool overlap(long min1, long max1, long min2, long max2)
{
	return ((min1 <= min2 && min2 <= max1)/*2 is leftsided overlapping with 1*/ || (min2 <= min1 && min1 <= max2)/*1 is rightsided overlapping with 2*/);
}

bool fullyOverlap(long min1, long max1, long min2, long max2)
{
	return ((min1 <= min2 && max2 <= max1)/*1 fully contains 2*/ || (min2 <= min1 && max1 <= max2)/*2 fully contains 1*/);
}

int main()
{
	std::ifstream file("input.txt");
	
	long containCount = 0;
	while (file)
	{
		std::string line(20,'\0');
		file.getline(line.data(), 20, '\n');
		
		if (line[0] == '\0')
			break;
		
		size_t commaPos = line.find_first_of(',');
		
		std::string elveOne = line.substr(0,commaPos);
		std::string elveTwo = line.substr(commaPos + 1);
		
		size_t dashPosOne = elveOne.find_first_of('-');
		long minElveOne = std::atol(elveOne.substr(0,dashPosOne).c_str());
		long maxElveOne = std::atol(elveOne.substr(dashPosOne + 1).c_str());
		
		size_t dashPosTwo = elveTwo.find_first_of('-');
		long minElveTwo = std::atol(elveTwo.substr(0,dashPosTwo).c_str());
		long maxElveTwo = std::atol(elveTwo.substr(dashPosTwo + 1).c_str());
		
		if (overlap(minElveOne, maxElveOne, minElveTwo, maxElveTwo))
			containCount++;
	}
	file.close();
	
	std::cout << "There are " << containCount << " pairs who'se ranges are fully contained be the other elve!\n";

	return 0;
}