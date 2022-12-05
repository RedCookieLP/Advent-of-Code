#include <iostream>
#include <fstream>
#include <vector>

#define STRAT(x) ((x) == -1 ? "Lose" : ((x) == 0 ? "Draw" : ((x) == 1 ? "Win" : "Unknown...")))
#define UNIT(x) ((x) == 0 ? "Rock" : ((x) == 1 ? "Paper" : ((x) == 2 ? "Scissors" : "Unknown...")))

long evaluateRound2(char o, char r)
{
	long score = r*3;
	char offset = (r-1);

	char oldOpponent = o;
	o = (o == 0 ? 3 : o);

	char own = (o + offset)%3;
	score += (own+1);

	std::cout << "Round - Supposed to: " << STRAT(offset) << " | Opponent plays: " << UNIT(oldOpponent) << "\n\tYou play: " << UNIT(own) << " ==> Results in Score of: " << score << '\n';

	return score;
}

long evaluateRound(char o, char s)
{
	long score = s+1;

	if (s == ((o+1)%3)) // You've won
		score += 6;
	else if (s == o) // Draw
		score +=3;
	
	return score;
}

int main()
{
	std::ifstream file("input.txt");

	long score = 0;
	long round = 1;
	while (file)
	{
		std::string line(5,'\0');
		file.getline(line.data(), 5, '\n');

		if (strnlen_s(line.c_str(), 5) == 0)
			break;

		char opponent = line[0]-'A';
		char self = line[2]-'X';
		
		long roundScore = evaluateRound2(opponent, self);

		//	std::cout << "Round #" << round++ << "\n\tSelf: " << (self == 0 ? "Rock" : (self == 1 ? "Paper" : "Scissors")) << " | Opponent: " << (opponent == 0 ? "Rock" : (opponent == 1 ? "Paper" : "Scissors"))
		//			  << "\n\tScore this round: " << roundScore << '\n';

		score += roundScore;
	}

	file.close();

	std::cout << "Final score: " << score << '\n';

	return 0;
}