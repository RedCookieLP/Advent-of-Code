#include <iostream>
#include <fstream>
#include <vector>

char charToIndex(char c)
{
	return (islower(c) ? (c - 'a') : ((c - 'A') + 26));
}

long findBadgePriority(const std::string& back1, const std::string& back2, const std::string& back3)
{
	char count[52] = {0};
	for (const char& c : back1)
	{
		char val = charToIndex(c);
		count[val] = 1;
	}
	for (const char& c : back2)
	{
		char val = charToIndex(c);
		if (count[val] == 1)
			count[val] = 2;
	}
	for (const char& c : back3)
	{
		char val = charToIndex(c);
		if (count[val] == 2)
			return (val + 1);
	}
	return 0;
}

long findCommonsPriority(const std::string& s1, const std::string& s2)
{
	bool present[52] = {false};
	for (const char& c : s1)
	{
		char val = charToIndex(c);
		present[val] = true;
	}
	
	for (const char& c : s2)
	{
		char val = charToIndex(c);
		if (present[val])
			return (val + 1);
	}
	
	return 0;
}

int main()
{
	std::ifstream file("input.txt");
	long prioSum = 0;
	while (file)
	{
		std::string back1(50,'\0');
		file.getline(back1.data(), 50, '\n');
		std::string back2(50,'\0');
		file.getline(back2.data(), 50, '\n');
		std::string back3(50,'\0');
		file.getline(back3.data(), 50, '\n');
		
		prioSum += findBadgePriority(back1, back2, back3);
	}
	file.close();
	
	std::cout << "The sum of all common items is " << prioSum << "!\n";

	return 0;
}