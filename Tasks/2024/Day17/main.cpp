#include <AoC-Module.h>
#include <vector>
#include <regex>
#include <sstream>

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
	inline uint64_t get(bool asCombo = false) const
	{
		if (!asCombo || m_rawOperand < 4u)
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
	
	std::cout << "Current line \"" << line << '"' << std::endl;

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

void finalize()
{
#ifndef PART_1
	uint64_t origB = s_registerB;
	uint64_t origC = s_registerC;
	for (s_registerA = 0ull ; s_registerA < 1E6 ; s_registerA++)
	{
	
	uint64_t workingA = s_registerA;
	s_registerB = origB;
	s_registerC = origC;
#endif
	std::string outputStr{};
	// Just run the program and wait for it to come to a stop (aka. for the IP to go out of scope)
	for (uint64_t regIP = 0u ; regIP < s_program.size() ; regIP++)
	{
		const Instruction& instr = s_program[regIP];
		const Operand& operand = instr.operand;
		switch (instr.opCode)
		{
			case OpCode::ADV:
			{
				s_registerA /= (1u << operand.get(true));
			}
			break;
			case OpCode::BXL:
			{
				s_registerB ^= operand.get();
			}
			break;
			case OpCode::BST:
			{
				s_registerB = (operand.get(true) & 0x07ull);
			}
			break;
			case OpCode::JNZ:
			{
				if (s_registerA)
					regIP = operand.get()-1u;
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
				outputStr += std::to_string(operand.get(true) & 0x07ull);
			}
			break;
			case OpCode::BDV:
			{
				s_registerB = s_registerA / (1u << operand.get(true));
			}
			break;
			case OpCode::CDV:
			{
				s_registerC = s_registerA / (1u << operand.get(true));
			}
			break;
		default:
			break;
		}
	}
#ifndef PART_1
	if (outputStr == s_programStr)
	{
		std::cout << "With a value for register A of " << workingA << " we generate the exact same output as our program!" << std::endl;
		break;
	}
	}
#else
	std::cout << "The generated output reads the following: \"" << outputStr << '"' << std::endl;
#endif
}
