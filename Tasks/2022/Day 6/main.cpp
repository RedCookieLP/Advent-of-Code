#include <iostream>
#include <fstream>

bool areUnique(const char* a, size_t matchingChars)
{
	for (int i = 0 ; i < matchingChars ; i++)
	{
		if (a[i] == '\0')
			return false;
	}
	for (int i = 0 ; i < matchingChars-1 ; i++)
	{
		for (int j = i+1 ; j < matchingChars ; j++)
		{
			if (a[i] == a[j])
				return false;
		}
	}
	return true;
}

constexpr static size_t charCount = 14;

int main()
{
	char seqBuf[charCount] = {'\0'};

	std::ifstream file("input.txt");
	size_t i;
	for (i = 0 ; file ; i++)
	{
		if (areUnique(seqBuf, charCount))
			break;
		seqBuf[i%charCount] = file.get();
	}
	std::cout << "First unique sequence after " << i << " characters (sequence is \"";
	for (size_t i = 0 ; i < charCount ; i++)
		std::cout << seqBuf[i];
	std::cout << "\")!\n";

	return EXIT_SUCCESS;
}