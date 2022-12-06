#include <iostream>
#include <fstream>
#include <vector>

int cmp(const void* p1, const void* p2)
{
	const long& v1 = *(const long*)p1;
	const long& v2 = *(const long*)p2;
	return (v1 < v2 ? 1 : (v1 > v2 ? -1 : 0));
}

int main()
{
	std::ifstream file("input.txt");

	std::vector<long> totalCals;
	long sum = 0;
	while (file)
	{
		std::string line(20,'\0');
		file.getline(line.data(), 20, '\n');

		//	std::cout << "line: \"" << line << "\"\n";

		if (std::strlen(line.c_str()) == 0)
		{
			//	std::cout << "sum of index #" << index << ": " << sum << '\n';
			totalCals.push_back(sum);
			sum = 0;
		}
		else
		{
			long value = std::atol(line.c_str());
			sum += value;
		}
	}

	std::qsort(totalCals.data(), totalCals.size(), sizeof(long), cmp);

	std::cout << "Greatest: " << totalCals[0] << " | Second: " << totalCals[1] << " | Third: " << totalCals[2] << "!\n"
			  << "Sum of those: " << totalCals[0]+totalCals[1]+totalCals[2] << "!\n";

	file.close();

	return 0;
}