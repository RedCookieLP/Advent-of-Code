#include <AoC-Module.h>
#include <Vector2.hpp>
#include <vector>
#include <regex>
#include <sstream>

using Position = Vector2u64;

struct ClawMachine
{
	Position movementA{};
	Position movementB{};
	Position prizePos{};
};

static std::vector<ClawMachine> s_clawMachines{};

static Position findMultipliers(const ClawMachine& clawMachine);

void initialize(uint64_t lineCount)
{
	s_clawMachines.resize(lineCount/4u);
}

bool handleLine(const std::string& line)
{
	static size_t clawIndex = 0ull;
	if (line == "")
	{
		clawIndex++;
		return true;
	}

	static const std::regex BUTTON_REGEX{ R"(^Button (A|B): X\+(\d+), Y\+(\d+)$)" };
	static const std::regex PRIZE_REGEX{ R"(^Prize: X=(\d+), Y=(\d+)$)" };

	ClawMachine& clawMachine = s_clawMachines.at(clawIndex);

	std::smatch strMatch;
	if (std::regex_search(line, strMatch, BUTTON_REGEX))
	{
		uint32_t x, y;
		{std::stringstream{} << strMatch[2].str() >> x;}
		{std::stringstream{} << strMatch[3].str() >> y;}
		Position movement{x,y};
		
		if (strMatch[1] == "A")
			clawMachine.movementA = movement;
		else
			clawMachine.movementB = movement;
	}
	else if (std::regex_search(line, strMatch, PRIZE_REGEX))
	{
		uint32_t x, y;
		{std::stringstream{} << strMatch[1].str() >> x;}
		{std::stringstream{} << strMatch[2].str() >> y;}
		clawMachine.prizePos = Position{x,y};
	}
	else
	{
		std::cerr << "Line doesn't match any regex! Line is \"" << line << '"' << std::endl;
		return false;
	}
	return true;
}

void finalize()
{
	uint64_t tokens = 0ull;
	std::cout << "Got all claw-machines!" << std::endl;
	for (ClawMachine& machine : s_clawMachines)
	{
#ifndef PART_1
		// Achievement unlocked: "How did we get here?..."
		machine.prizePos += Position{10000000000000ull, 10000000000000ull};
#endif
		// Since our deltas are positive values only, we just need
		// to find the pair of multipliers needed to find the prize
		Position multipliers = findMultipliers(machine);
		if (multipliers.len2() == 0u)
		{
			std::cout << "The current claw-machine won't reach its prize..." << std::endl;
			continue;
		}
		std::cout << "The multipliers for the current claw-machine are [" << multipliers.x() << "] for A and [" << multipliers.y() << "] for B!" << std::endl;
		tokens += (multipliers.x() * 3ull) + (multipliers.y() * 1ull);
	}
	std::cout << "In total we need " << tokens << " tokens" << std::endl;
}

static Position findMultipliers(const ClawMachine& clawMachine)
{
	// Apparently this is a "simple" set of linear equations
	// P1 = [0,0] + A * mulA
	// P2 = [pX,pY] - B * mulB
	// P1 == P2
	// 
	// Found the following on StackOverflow, so it must be correct:
	// A*x + B*y = C
	// D*x + E*y = F
	//
	//  where x = (CE-BF)/(AE-BD) and y = (AF-CD)/(AE-BD)
	// 
	// in our case this would mean that
	//  - A = Ax
	//  - B = Bx
	//  - C = Px
	//  - D = Ay
	//  - E = By
	//  - F = Py
	//  - x = mulA
	//  - y = mulB
	//
	// so to get the multipliers using the formulars from above
	// (take them as doubles to not mess up the calculation. Convert them back later!)
	double A = clawMachine.movementA.x();
	double B = clawMachine.movementB.x();
	double C = clawMachine.prizePos.x();
	double D = clawMachine.movementA.y();
	double E = clawMachine.movementB.y();
	double F = clawMachine.prizePos.y();

	Position muls
	{
		((C*E)-(B*F))/((A*E)-(B*D)),
		((A*F)-(C*D))/((A*E)-(B*D))
	};

	uint64_t calculatedPrizePosX = static_cast<uint64_t>(A)*muls.x() + static_cast<uint64_t>(B)*muls.y();
	uint64_t calculatedPrizePosY = static_cast<uint64_t>(D)*muls.x() + static_cast<uint64_t>(E)*muls.y();
	Position calculatedPrizePos{calculatedPrizePosX, calculatedPrizePosY};

	// Check if we can reach the prize with these multipliers.
	// If not, then there aren't any multipliers (they'd need to be floating)
	return (clawMachine.prizePos == calculatedPrizePos ? muls : Position{});
}
