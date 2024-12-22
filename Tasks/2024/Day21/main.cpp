#include <AoC-Module.h>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <Vector2.hpp>
#include <vector>
#include <queue>
#include <algorithm>
#include <sstream>
#ifdef DEBUG
#include <thread>
#include <chrono>
#endif
#include <array>

using Position = Vector2i32;

class KeyPad
{
public:
	KeyPad()
		: m_stepCache{}, m_pathMatrix{}, m_controller{nullptr}, m_currentKey('A')
	{}
public:
	inline const std::unordered_set<std::string>& operator()(char from, char to) const
	{
		auto fromIter = m_pathMatrix.find(from);
		if (fromIter == m_pathMatrix.end())
			throw std::invalid_argument("from");
		const auto& fromMap = fromIter->second;
		auto toIter = fromMap.find(to);
		if (toIter == fromMap.end())
			throw std::invalid_argument("to");
		return toIter->second;
	}
public:
	inline void registerMovement(char from, char to, const std::string& path)
	{
		m_pathMatrix[from][to].insert(path);
	}
	inline void registerMovement(char from, char to, const std::unordered_set<std::string>& paths)
	{
		m_pathMatrix[from][to].insert(paths.begin(), paths.end());
	}
	inline void setController(KeyPad* controller) noexcept { m_controller = controller; }
	uint64_t pressKey(char key) noexcept;
	uint64_t moveToKey(char key) noexcept;
	inline char getCurrentKey() const noexcept { return m_currentKey; }
	inline void setCurrentKey(char key) noexcept { m_currentKey = key; }
private:
	std::unordered_map<std::string, std::pair<uint64_t, std::string>> m_stepCache;
	std::unordered_map<char,std::unordered_map<char,std::unordered_set<std::string>>> m_pathMatrix;
	KeyPad* m_controller;
	char m_currentKey;
};

static const std::unordered_map<char, Position> s_numericKeyPadLayout
{
	{'7', {0,0}},
	{'8', {1,0}},
	{'9', {2,0}},
	{'4', {0,1}},
	{'5', {1,1}},
	{'6', {2,1}},
	{'1', {0,2}},
	{'2', {1,2}},
	{'3', {2,2}},
	{'0', {1,3}},
	{'A', {2,3}},
};
static const std::unordered_map<char, Position> s_movementKeyPadLayout
{
	{'^', {1,0}},
	{'A', {2,0}},
	{'<', {0,1}},
	{'v', {1,1}},
	{'>', {2,1}},
};
static const std::unordered_map<Position, char> DIRECTION_CHAR_MAP
{
	{Position{1,0},'>'},
	{Position{0,1},'v'},
	{Position{-1,0},'<'},
	{Position{0,-1},'^'}
};
static KeyPad s_numericKeyPad;
static KeyPad s_movementKeyPad;
static std::vector<std::string> s_keyCodes;

static std::unordered_set<std::string> findAllPaths(Position from, Position to, Position blank);

void initialize([[maybe_unused]] uint64_t lineCount)
{
	for (auto [fromKey, fromPos] : s_numericKeyPadLayout)
	{
		for (auto [toKey, toPos] : s_numericKeyPadLayout)
		{
			if (toKey == fromKey)
				continue;
			s_numericKeyPad.registerMovement(fromKey, toKey, findAllPaths(fromPos, toPos, Position{0,3}));
		}
	}
	for (auto [fromKey, fromPos] : s_movementKeyPadLayout)
	{
		for (auto [toKey, toPos] : s_movementKeyPadLayout)
		{
			if (toKey == fromKey)
				continue;
			s_movementKeyPad.registerMovement(fromKey, toKey, findAllPaths(fromPos, toPos, Position{0,0}));
		}
	}
}

bool handleLine(const std::string& line)
{
	if (line != "")
		s_keyCodes.push_back(line);
	return true;
}

void finalize()
{
#ifdef PART_1
	constexpr static size_t KEYPAD_COUNT = 4u;
#else
	constexpr static size_t KEYPAD_COUNT = 27u; // One for us, 25 for the intermediate robots and one for the door
#endif
	std::array<KeyPad, KEYPAD_COUNT> allKeypads{};
	allKeypads[0] = s_numericKeyPad;
	for (size_t i = 1u ; i < KEYPAD_COUNT ; i++)
	{
		allKeypads[i] = s_movementKeyPad;
		allKeypads[i-1u].setController(&allKeypads[i]);
	}

	uint64_t totalComplexity = 0ull;
	for (const std::string& keyCode : s_keyCodes)
	{
		uint64_t numericPart;
		{std::stringstream{} << std::dec << keyCode >> numericPart;}

		std::cout << "Currently checking key-code \"" << keyCode << '"' << std::endl;
		uint64_t keyPresses = 0ull;
		for (char key : keyCode)
		{
			std::cout << "Pressing key '" << key << "'..." << std::endl;
			keyPresses += allKeypads[0].pressKey(key);
		}
		uint64_t complexity = keyPresses * numericPart;
		std::cout << "The previous key-code took " << keyPresses << " key-presses, which results in a complexity of " << complexity << std::endl;
		totalComplexity += complexity;
	}
	std::cout << "The total complexity of all key-codes is " << totalComplexity << std::endl;
}

struct QueueEntry
{
	std::vector<Position> path{};
	Position pos{};
};

static std::unordered_set<std::string> findAllPaths(Position from, Position to, Position blank)
{
	std::unordered_set<std::string> optimalPaths{};
	std::queue<QueueEntry> priorityQueue{};
	priorityQueue.push(QueueEntry{{}, from});
	uint64_t lowestScore = std::numeric_limits<uint64_t>::max();
	//	std::unordered_map<Position, uint64_t> visited{};
	while (!priorityQueue.empty())
	{
		QueueEntry entry = priorityQueue.front();
		priorityQueue.pop();
		
		if (std::find(entry.path.begin(), entry.path.end(), entry.pos) != entry.path.end())
			continue;
		entry.path.push_back(entry.pos);
		
		if (entry.pos == to)
		{
			uint64_t pathLen = entry.path.size();
			if (pathLen == 1)
				continue;
			
			if (pathLen < lowestScore)
			{
				lowestScore = pathLen;
				optimalPaths.clear();
			}
			if (pathLen == lowestScore)
			{
				// Build the path
				std::string path{};
				for (auto prevIter = entry.path.begin(), curIter = entry.path.begin()+1 ; curIter != entry.path.end() ; ++prevIter, ++curIter)
				{
					Position diff = *curIter - *prevIter;
					path += DIRECTION_CHAR_MAP.at(diff);
				}
				optimalPaths.insert(path);
			}
			continue;
		}

		// Based on the difference between `entry.pos` and `to`, determine the possible directions we can walk towards.
		// This will at most be two directions (one horizontally, one vertically)
		Position diff = to - entry.pos;
		Position horizontalDir{}, verticalDir{};
		if (diff.x() < 0)
			horizontalDir.x() = -1;
		else if (diff.x() > 0)
			horizontalDir.x() = 1;
		
		if (diff.y() < 0)
			verticalDir.y() = -1;
		else if (diff.y() > 0)
			verticalDir.y() = 1;
		
		if (horizontalDir.len2() != 0 && (entry.pos + horizontalDir) != blank)
			priorityQueue.emplace(QueueEntry{entry.path, entry.pos + horizontalDir});
		if (verticalDir.len2() != 0 && (entry.pos + verticalDir) != blank)
			priorityQueue.emplace(QueueEntry{entry.path, entry.pos + verticalDir});
	}
	return optimalPaths;
}

uint64_t KeyPad::pressKey(char key) noexcept
{
	// To press a key, we must first move to it, and then press it
	return moveToKey(key) + (m_controller ? m_controller->pressKey('A') : 1ull);
}

// Tip for others: Don't look at this code. It's bodge'd beyond repair just to fix a bug in part 2 where the controller chain
// isn't reset properly on calculating the penalty for the `A`-press after a movement's been done. This, or there was some bug
// in my previous code (the commented-out section inside the cache-hit-code) which left the controller's in a weird state.
// 
// Either way, the duct-tape fix of lines 302-304/306-308 fixed it, which lead me to my already stated suspision...
uint64_t KeyPad::moveToKey(char key) noexcept
{
	// To move to a key, we must instruct our controller to press
	// the movement keys in way which moves us to said key.
	// If we don't have a controller (aka. we're human), then we can move instantly,
	// so no previous key-presses necessary.
	// We also don't need any movement when we're already at the key in question
	if (!m_controller || m_currentKey == key)
	{
		m_currentKey = key;
		return 0ull;
	}

	//	std::cout << "Moving with ID #" << m_debugID << std::endl;

	// Check if this move is already stored in the cache. If it is, take the entry from the cache
	std::string cacheKey{};
	cacheKey += key;
	for (KeyPad* controller = this ; controller ; controller = controller->m_controller)
		cacheKey += controller->m_currentKey;
	auto cacheIter = m_stepCache.find(cacheKey);
#if 1
	if (cacheIter != m_stepCache.end())
#else
	if (false)
#endif
	{
		//	std::cout << "Cache-hit with ID #" << m_debugID << '!' << std::endl;
		KeyPad* controller = this;
		auto cacheEntry = cacheIter->second;
		auto cacheVector = cacheEntry.second;
		for (char endChar : cacheVector)
		{
			controller->m_currentKey = endChar;
			controller = controller->m_controller;
		}
		return cacheEntry.first;
		// Do NOT forget to move the controller to the key where
		// it WOULD end on if we'd actually simulate that path again!
		//	m_controller->moveToKey(cacheEntry.second, simulateMovement);
		//	//	m_controller->m_currentKey = cacheEntry.second;
		//	return cacheEntry.first;
	}

	// Try all possible paths from our current location to our new key.
	// This should optimize to use the path with the lowest man-made key-presses!
	uint64_t bestScore = -1;
	uint64_t lowestPenaltyForMoveToA = 0ull;
	char originalChar = m_controller->m_currentKey;
	char controllerBestLastKey;
	const std::unordered_set<std::string>& possiblePaths = m_pathMatrix[m_currentKey][key];
	for (const std::string& path : possiblePaths)
	{
		// For each step in the path, let our controller press the movement buttons and
		// sum up how many presses it took.
		uint64_t currentScore = 0u;
		char controllerCurrentKey = m_controller->m_currentKey;
		for (char movementKey : path)
			currentScore += m_controller->pressKey(movementKey);
		
		// Then store the key where the controller stopped on (which should technically be the
		// last direction to get us here, aka. the last char in the path, but better be safe than sorry)
		char controllerLastKey = m_controller->m_currentKey;

		// Now get the presses needed the controller from the last input to the 'A'-button (which could be many for some combinations)
		// UPDATE: This is the most... how'd I put it... "paranoid" approach, but somewhere here's a logic bug which I'm too stupid to find...
		std::string temp{};
		for (KeyPad* controller = this ; controller ; controller = controller->m_controller)
			temp += controller->m_currentKey;
		uint64_t penaltyForMoveToA = m_controller->moveToKey('A');
		size_t tempI = 0u;
		for (KeyPad* controller = this ; controller ; controller = controller->m_controller)
			controller->m_currentKey = temp[tempI++];
		
		// Now determine if this path is the one to use. Do that by adding up the steps needed
		// to press all path-keys AND the steps needed to press the 'A'-button. If the sum
		// is smaller than our current best, then we've found something!
		if ((currentScore + penaltyForMoveToA) < (bestScore + lowestPenaltyForMoveToA))
		{
			bestScore = currentScore;
			lowestPenaltyForMoveToA = penaltyForMoveToA;
			controllerBestLastKey = controllerLastKey;
		}

		// Finally, restore the controller's key to where it was before this check
		m_controller->m_currentKey = controllerCurrentKey;
	}

	// If we're here, than we must've found an optimal path.
	// Update the controller's current-key to the last key pressed on the best path
	m_controller->m_currentKey = controllerBestLastKey;
	m_currentKey = key;
	
	std::string cacheString{};
	for (KeyPad* controller = this ; controller ; controller = controller->m_controller)
		cacheString += controller->m_currentKey;
	// And add the best path's score AND the controller's last key of the best path to the cache
	m_stepCache[cacheKey].first = bestScore;
	m_stepCache[cacheKey].second = cacheString;

	if (bestScore == -1)
		std::cerr << "Somehow I got a score of -1!" << std::endl;
	
	return bestScore;
}
