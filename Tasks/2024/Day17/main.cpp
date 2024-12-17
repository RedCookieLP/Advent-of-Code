#include <AoC-Module.h>
#include <vector>
#include <regex>
#include <sstream>
#include <array>

static uint64_t s_registerA, s_registerB, s_registerC;

enum class OpCode
{
	ADV = 0,
	BXL,
	BST,
	JNZ,
	BXC,
	OUT,
	BDV,
	CDV,
	_COUNT
};

class Operand
{
public:
	Operand(uint8_t operand = 0u) noexcept
		: m_rawOperand{operand}
	{}
public:
	inline uint64_t asLiteral() const noexcept
	{
		return m_rawOperand;
	}
	inline uint64_t asCombo() const
	{
		if (m_rawOperand < 4u)
			return m_rawOperand;
		else if (m_rawOperand == 4u)
			return s_registerA;
		else if (m_rawOperand == 5u)
			return s_registerB;
		else if (m_rawOperand == 6u)
			return s_registerC;
		throw std::logic_error("Invalid combo operand " + std::to_string(m_rawOperand));
	}
private:
	uint8_t m_rawOperand;
};

struct Instruction
{
	OpCode opCode{OpCode::_COUNT};
	Operand operand{};
};

static std::vector<Instruction> s_program;
#ifndef PART_1
static std::string s_programStr;
#endif

bool handleLine(const std::string& line)
{
	static const std::regex REGISTER_REGEX{ R"(^Register ([ABC]): (\d+)$)" };
	static const std::regex PROGRAM_REGEX{ R"(Program: )" };
	static const std::regex INSTRUCTION_REGEX{ R"((\d+),(\d+?)(?:,|$))" };
	
	if (line.empty())
		return true;
	
	std::smatch strMatch;
	if (std::regex_match(line, strMatch, REGISTER_REGEX))
	{
		char reg = strMatch[1].str()[0];
		uint64_t value;
		{std::stringstream{} << strMatch[2].str() >> value;}
		if (reg == 'A')
			s_registerA = value;
		else if (reg == 'B')
			s_registerB = value;
		else
			s_registerC = value;
	}
	else if (std::regex_search(line, strMatch, PROGRAM_REGEX))
	{
		std::string instrStr = strMatch.suffix();
#ifndef PART_1
		s_programStr = instrStr;
#endif
		for ( ; std::regex_search(instrStr, strMatch, INSTRUCTION_REGEX) ; instrStr = strMatch.suffix())
		{
			uint64_t rawOpCode, rawOperand;
			{std::stringstream{} << strMatch[1].str() >> rawOpCode;}
			{std::stringstream{} << strMatch[2].str() >> rawOperand;}
			if (rawOpCode >= static_cast<uint64_t>(OpCode::_COUNT))
			{
				std::cerr << "Invalid OpCode-value " << rawOpCode << std::endl;
				return false;
			}
			if (rawOperand >= 8u)
			{
				std::cerr << "Invalid Operand-value " << rawOperand << std::endl;
				return false;
			}

			s_program.emplace_back(Instruction{static_cast<OpCode>(rawOpCode), Operand{static_cast<uint8_t>(rawOperand)}});
		}
	}
	return true;
}

#ifdef PART_1
void finalize()
{
	std::string outputStr{};
	// Just run the program and wait for it to come to a stop (aka. for the IP to go out of scope)
	for (uint64_t regIP = 0u ; regIP < s_program.size() ; regIP++)
	{
		const Instruction& instr = s_program[regIP];
		const Operand& operand = instr.operand;
		bool isViable = true;
		switch (instr.opCode)
		{
			case OpCode::ADV:
			{
				s_registerA >>= operand.asCombo();
			}
			break;
			case OpCode::BXL:
			{
				s_registerB ^= operand.asLiteral();
			}
			break;
			case OpCode::BST:
			{
				s_registerB = (operand.asCombo() & 0x07ull);
			}
			break;
			case OpCode::JNZ:
			{
				if (s_registerA)
					regIP = operand.asLiteral()-1u;
			}
			break;
			case OpCode::BXC:
			{
				s_registerB ^= s_registerC;
			}
			break;
			case OpCode::OUT:
			{
				if (!outputStr.empty())
					outputStr += ',';
				outputStr += std::to_string(operand.asCombo() & 0x07ull);
			}
			break;
			case OpCode::BDV:
			{
				s_registerB = s_registerA / (1u << operand.asCombo());
			}
			break;
			case OpCode::CDV:
			{
				s_registerC = s_registerA / (1u << operand.asCombo());
			}
			break;
		default:
			break;
		}
		if (!isViable)
			break;
	}
	std::cout << "The generated output reads the following: \"" << outputStr << '"' << std::endl;
}
#else
static uint64_t findA(uint64_t index, const std::vector<uint8_t>& targets, uint64_t fakeA) noexcept;

void finalize()
{
	// Since everyone did it, I'll do it too: Analyze the input data by hand and then write an algorithm to check for solutions
	// 
	// My input program looks like this: [2,4],[1,2],[7,5],[1,3],[4,4],[5,5],[0,3],[3,0]
	// This translates to
	//	- BST, A
	//	- BXL, 2
	//	- CDV, B
	//	- BXL, 3
	//  - BXC, 4
	//	- OUT, B
	//	- ADV, 3
	//	- JNZ, 0
	// 
	// Breaking it down further, we get this:
	//	- B = A % 8
	//	- B = B ^ 2
	//	- C = A >> B
	//	- B = B ^ 3
	//	- B = B ^ C
	//	- Output (B % 8)
	//	- A = A >> 3
	//	- IP = (A == 0 ? IP : 0)
	//
	// As we can see, A doesn't get modifed by *anything*, it only gets shifted by 3 digits every iteration.
	// Since A has a direct influence on B (and B doesn't grow anywhere), this gives us an upper limit!
	// Our program is 16 digits long. When each digit "takes" 3 bits, that makes our input between 46 and 48 bits long!
	// That's way too long for brute-force, so we must do this smart...
	// 
	// Another great simplification we can observe is that B and C don't keep their values between the iterations:
	// *Each iteration* their values are set directly dependent of the current 3 bits of A!
	// 
	// So to get the result, for each digit of the program, try a number between 0 and 7 until we find the program's digit. Then continue on

	// program
	//	const std::vector<uint8_t> targetValues{2,4,1,2,7,5,1,3,4,4,5,5,0,3,3,0};
	// sample
	const std::vector<uint8_t> targetValues{7,1,5,2,4,0,7,6,1};
	//	const std::vector<uint8_t> targetValues{0,3,5,4,3,0};
	uint64_t finalA = findA(0u, targetValues, 0u);
	std::cout << "Found a final A! Its value is " << finalA << std::endl;
	std::cout << "Does it match the expected value for A (" << s_registerA << ")? " << (finalA == s_registerA ? "yes" : "no") << std::endl;
}

static uint64_t findA(uint64_t index, const std::vector<uint8_t>& targets, uint64_t currentA) noexcept
{
	if (index >= targets.size())
		return currentA;
	
	uint8_t targetB = targets[targets.size() - index - 1u];
	// Test each value between 0 and 7 until we find a fitting one
	for (uint8_t rawA = 8u ; rawA > 0u ; rawA--)
	{
		uint8_t possibleA = rawA - 1u;
		//	if (index == 0 && possibleA == 0u)
		//		continue;
		uint64_t fakeA = (currentA << 3u) | possibleA;
		uint64_t fakeB = 0u, fakeC = 0u;

		// [2,4],[1,2],[7,5],[1,3],[4,4]
		fakeB = possibleA; // mod 8 is already given
		fakeB ^= 2u;
		fakeC = fakeA >> fakeB;
		fakeB ^= 3u;
		fakeB ^= fakeC;

		// If we've hit the target, expand on!
		if (static_cast<uint8_t>(fakeB % 8u) == targetB)
		{
			uint64_t retVal = findA(index+1u, targets, (currentA << 3u) | possibleA);
			if (retVal != static_cast<uint64_t>(-1))
			{
				std::cout << "New Part A for index #" << index << ": " << static_cast<uint32_t>(possibleA) << std::endl;
				return retVal;
			}
		}
	}
	return -1;
}
#endif
