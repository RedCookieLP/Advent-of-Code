#include <AoC-Module.h>
#include <regex>
#include <unordered_map>
#include <chrono>
#include <thread>

const static std::regex sc_instructionsRegex("^([LR]+)");
const static std::regex sc_junctionRegex(R"(^(\S{3})\s+=\s+\((\S{3}),\s+(\S{3})\)$)");

struct Junction
{
	std::string left;
	std::string right;
};

#ifndef DO_PART_1
class Ghost
{
public:
	Ghost(std::string startJunction)
		: m_startJunction(startJunction), m_currentJunction(startJunction), m_loopDetected(false), m_loopOffset(0), m_loopLength(0), m_exitJunctions({}), m_fieldStats({})
	{}
	~Ghost() = default;
public:
	size_t nextStepOnExit(size_t startStep) const;
	bool visitNext(size_t step, char direction);
	bool loopDetected() const { return m_loopDetected; }
private:
	void fillLocalExitSteps();
private:
	std::string m_startJunction;
	std::string m_currentJunction;
	bool m_loopDetected;
	size_t m_loopOffset;
	size_t m_loopLength;
	std::vector<size_t> m_exitJunctions;
	std::unordered_map<std::string/*junction-name + instructionIndex*/, size_t/*first encounter (steps)*/> m_fieldStats;
};
#endif // ~DO_PART_1

static std::string s_instructions;
static std::unordered_map<std::string, Junction> s_junctionMap;
#ifndef DO_PART_1
static std::vector<Ghost> s_ghosts;
#endif // ~DO_PART_1

bool handleLine(const std::string& line)
{
	if (line.empty())
		return true;
	
	std::smatch match;
	if (std::regex_match(line, match, sc_instructionsRegex))
	{
		s_instructions = match.str();
		std::cout << "Length of the string: " << s_instructions.length() << std::endl;
	}
	else if (std::regex_match(line, match, sc_junctionRegex))
	{
		/*
		[0] = whole match
		[1] = own junction-ID
		[2] = next junction when going left
		[3] = next junction when going right
		*/
		if (match.size() != 4)
		{
			std::cerr << "Failed to match line \"" << line << "\" against junction-format!" << std::endl;
			return false;
		}
		std::string currentJunction = match[1];
		std::string leftJunction = match[2];
		std::string rightJunction = match[3];

		s_junctionMap.insert_or_assign(currentJunction, Junction{leftJunction, rightJunction});
#ifndef DO_PART_1
		if (currentJunction[2] == 'A')
			s_ghosts.emplace_back(Ghost(currentJunction));
#endif // ~DO_PART_1
	}
	else
	{
		std::cerr << "Unknown line-format for line \"" << line << "\"!" << std::endl;
		return false;
	}
	return true;
}

void finalize()
{
	size_t steps = 0;
#ifdef DO_PART_1
	std::string currentJunction = "AAA";
	std::cout << "Following map until reaching 'ZZZ'..." << std::endl;
	for (char instruction = s_instructions[steps % s_instructions.length()] ; currentJunction != "ZZZ" ; (currentJunction = (instruction == 'L' ? s_junctionMap[currentJunction].left : s_junctionMap[currentJunction].right)), (steps++), (instruction = s_instructions[steps % s_instructions.length()]))
		std::cerr << "Step #" << steps << ": at '" << currentJunction << "'..." << std::endl;
	std::cout << "Found the way to 'ZZZ' in " << steps << " steps while following the instructions!" << std::endl;
#else // DO_PART_1
	bool foundLoops = false;
	bool foundExits = false;
	std::cout << "Following all paths until reaching 'xxZ'..." << std::endl;
	for (char instruction = s_instructions[steps % s_instructions.length()] ; !foundLoops && !foundExits ; (steps++), (instruction = s_instructions[steps % s_instructions.length()]))
	{
		// Update all paths and check if they've reached an end...
		foundLoops = true;
		foundExits = true;
		//	size_t ghostID = 0;
		for (Ghost& ghost : s_ghosts)
		{
			bool hasGhostFoundExit = ghost.visitNext(steps, instruction);
			bool hasGhostFoundLoop = ghost.loopDetected();
			//	std::cout << "Ghost #" << ghostID++ << ": [Loop? " << (hasGhostFoundLoop ? "yes" : "no") << " | Exit? " << (hasGhostFoundExit ? "yes]" : "no]") << std::endl;
			foundExits = foundExits && hasGhostFoundExit;
			foundLoops = foundLoops && hasGhostFoundLoop;
		}
	}

	if (foundLoops)
	{
		std::cout << "Found a loop! Trying to agree on a step, where everyone is on a xxZ..." << std::endl;
		size_t currentMinimumStep = 0;
		size_t old = SIZE_MAX;
		for (size_t smallestNextStep ; true ; currentMinimumStep = smallestNextStep+1)
		{
			smallestNextStep = SIZE_MAX;
			bool allAgree = true;
			size_t ghostID = 0;
			for (const Ghost& g : s_ghosts)
			{
				size_t nextStep = g.nextStepOnExit(currentMinimumStep);
				//	std::cerr << "Ghost #" << ghostID++ << ": " << nextStep << std::endl;
				if (nextStep < smallestNextStep)
					smallestNextStep = nextStep;
				if (nextStep != currentMinimumStep)
				{
					allAgree = false;
				}
			}
			//	std::cerr << "--------------------" << std::endl;
			
			size_t temp = smallestNextStep / 10'000'000'000;
			if (old != temp)
			{
				std::cerr << "Currently at " << smallestNextStep << std::endl;
				std::cerr << "Target:      " << 20685524831999ULL << std::endl;
				old = temp;
			}

			if (allAgree)
				break;
		}
		std::cout << "Found smallest minimum: " << currentMinimumStep << std::endl;
		return;
	}
	std::cout << "Found the ways to 'xxZ' in " << steps << " steps while following the instructions!" << std::endl;
#endif // ~DO_PART_1
}



size_t Ghost::nextStepOnExit(size_t startStep) const
{
	if(startStep <= m_loopOffset)
	{
		// Search from the beginning...
		std::string junc = m_currentJunction;
		for (size_t iter = 0; iter < startStep ; iter++)
		{
			junc = (s_instructions[iter % s_instructions.length()] == 'L' ? s_junctionMap[junc].left : s_junctionMap[junc].right);
		}

		// Now search for the next Z...
		// It might happen that we stumble into the loop... if that happens, oh well...
		size_t finPos = startStep;
		for ( ; junc[2] != 'Z' ; finPos++)
		{
			junc = (s_instructions[finPos % s_instructions.length()] == 'L' ? s_junctionMap[junc].left : s_junctionMap[junc].right);
		}
		return finPos;
	}
	
	size_t multiplier = (startStep - m_loopOffset) / m_loopLength;
	size_t localStep = (startStep - m_loopOffset) % m_loopLength;

	size_t closestStep = 0;
	for (size_t i = 0 ; i <= m_exitJunctions.size() ; i++)
	{
		// Nothing found, so use the next step in the next loop
		if (i == m_exitJunctions.size())
		{
			closestStep = m_loopLength + m_exitJunctions[0];
			break;
		}

		if (localStep <= m_exitJunctions[i])
		{
			closestStep = m_exitJunctions[i];
			break;
		}
	}

	return m_loopOffset + (multiplier * m_loopLength) + closestStep;
}
bool Ghost::visitNext(size_t step, char direction)
{
	if (m_loopDetected)
		return (nextStepOnExit(step) == step);
	
	size_t instructionIndex = (step % (s_instructions.length()));
	std::string index = m_currentJunction + std::to_string(instructionIndex);

	auto iter = m_fieldStats.find(index);
	if (iter != m_fieldStats.end())
	{
		size_t firstEncounterStep = iter->second;
		m_loopDetected = true;
		m_loopLength = (step - firstEncounterStep);
		m_loopOffset = firstEncounterStep;
		std::cout << "Step #" << step << " - Found a loop for ghost: Started in step #" << firstEncounterStep << " on instruction #" << instructionIndex << ", beginning with junction '" << iter->first << "'. This makes the loop " << m_loopLength << " instructions long..." << std::endl;
		fillLocalExitSteps();
	}
	else
	{
		m_fieldStats[index] = step;
	}
	
	m_currentJunction = (direction == 'L' ? s_junctionMap[m_currentJunction].left : s_junctionMap[m_currentJunction].right);

	// In a perfect world, "foundEnds &&= (junction[2] == 'Z')" would be possible...
	return (m_currentJunction[2] == 'Z');
}

void Ghost::fillLocalExitSteps()
{
	if (!m_loopDetected)
		return;
	
	std::string startJunction = m_startJunction;
	for (size_t i = 0 ; i < (m_loopOffset + m_loopLength) ; )
	{
		char direction = s_instructions[i++ % s_instructions.length()];
		startJunction = (direction == 'L' ? s_junctionMap[startJunction].left : s_junctionMap[startJunction].right);
		
		if (startJunction[2] == 'Z')
		{
			if (i < m_loopOffset)
				continue;
			m_exitJunctions.push_back(i - m_loopOffset);
		}
	}
}