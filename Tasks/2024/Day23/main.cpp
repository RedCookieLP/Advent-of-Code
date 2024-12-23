#include <AoC-Module.h>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <regex>
#include <HashHelper.hpp>

using LanID = uint16_t;

static std::unordered_map<LanID,std::unordered_set<LanID>> s_connections;

#ifndef PART_1
static std::unordered_set<LanID> findLANParty() noexcept;
#endif

void initialize(uint64_t lineCount)
{
	s_connections.reserve(lineCount);
}

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	static const std::regex LINE_REGEX{ "^([a-z]{2})-([a-z]{2})$" };
	std::smatch strMatch;
	if (!std::regex_match(line, strMatch, LINE_REGEX))
	{
		std::cerr << "Line \"" << line << "\" doesn't match our expected regex!" << std::endl;
		return false;
	}

	std::string firstPCStr = strMatch[1u].str();
	std::string secondPCStr = strMatch[2u].str();

	LanID firstPCID = (firstPCStr[0u] << 8u | firstPCStr[1u]);
	LanID secondPCID = (secondPCStr[0u] << 8u | secondPCStr[1u]);

	if (firstPCID == secondPCID)
	{
		std::cerr << "Somehow we got duplicate PC-IDs of value " << static_cast<uint32_t>(firstPCID) << '!' << std::endl;
		return false;
	}

	s_connections[firstPCID].insert(secondPCID);
	s_connections[secondPCID].insert(firstPCID);
	return true;
}

#ifdef PART_1
void finalize()
{
	std::unordered_set<uint64_t> threeWayInterConnsWithT{};
	for (const auto& [pcID, pcConns] : s_connections)
	{
		if (((pcID >> 8u) & 0xFFu) != static_cast<uint8_t>('t'))
			continue;
		if (pcConns.size() <= 1ull)
			continue;
		
		// To detect the interconnection of three PCs, iterate through our neighbours in unique pairs
		// and check if they contain each other and this PC as well. If so, then we've found a triplet!
		for (auto secondPCIter = pcConns.begin(), thirdPCStartIter = ++pcConns.begin() ; thirdPCStartIter != pcConns.end() ; secondPCIter++, thirdPCStartIter++)
		{
			const auto& secondPCConns = s_connections[*secondPCIter];
			if (secondPCConns.find(pcID) == secondPCConns.end())
				continue;
			
			for (auto thirdPCIter = thirdPCStartIter ; thirdPCIter != pcConns.end() ; thirdPCIter++)
			{
				if (secondPCConns.find(*thirdPCIter) == secondPCConns.end())
					continue;
				const auto& thirdPCConns = s_connections[*thirdPCIter];
				if (thirdPCConns.find(pcID) == thirdPCConns.end())
					continue;
				if (thirdPCConns.find(*secondPCIter) == thirdPCConns.end())
					continue;
				// If we get here, the three PCs are in fact interconnected with each other!
				// Now build an ID that's unique to this set by shifting all three IDs into
				// a 64-bit number with the smaller numbers in the front and the largest numbers in the back
				LanID a = pcID, b = *secondPCIter, c = *thirdPCIter;
				if (a > c)
					std::swap(a,c);
				if (a > b)
					std::swap(a,b);
				if (b > c)
					std::swap(b,c);
				uint64_t uniqueSetID = a;
				uniqueSetID = (uniqueSetID << 16u) | b;
				uniqueSetID = (uniqueSetID << 16u) | c;
				threeWayInterConnsWithT.insert(uniqueSetID);
			}
		}
	}
	std::cout << "There are a total of " << threeWayInterConnsWithT.size() << " set of three inter-connected computers with at least one computer containing a name starting with 't'!" << std::endl;
}
#else
void finalize()
{
	auto lanPartySet = findLANParty();
	if (lanPartySet.empty())
	{
		std::cerr << "Failed to find the LAN-party between " << s_connections.size() << " computers..." << std::endl;
		return;
	}
	std::cout << "The LAN-party is a set of " << lanPartySet.size() << " computers!" << std::endl;
	std::set<LanID> orderedSet{};
	orderedSet.insert(lanPartySet.begin(), lanPartySet.end());
	std::string password{};
	for (LanID pcID : orderedSet)
	{
		password += static_cast<char>((pcID >> 8u) & 0xFFu);
		password += static_cast<char>(pcID & 0xFFu);
		password += ',';
	}
	password.pop_back();
	std::cout << "The password to the LAN-party is \"" << password << '"' << std::endl;
}

struct SearchEntry
{
	std::unordered_set<LanID> groupIDs{};
	LanID ownID{};
};

static inline size_t getSetHash(const std::unordered_set<LanID>& set) noexcept
{
	uint64_t hash = 0ull;
	std::set<LanID> sortedSet{};
	sortedSet.insert(set.begin(), set.end());
	for (LanID lanID : sortedSet)
	{
		hash = Hash::combine(hash, lanID);
	}
	return hash;
}
static bool isClique(const std::unordered_set<LanID>& set) noexcept;
static std::unordered_set<LanID> findLANParty() noexcept
{
	std::unordered_set<LanID> greatestSet{};
	std::unordered_set<uint64_t> knownSets{};
	for (const auto& [lanID, _] : s_connections)
	{
		std::queue<SearchEntry> queue{};
		queue.emplace(SearchEntry{{lanID}, lanID});

		while (!queue.empty())
		{
			SearchEntry entry = queue.front();
			queue.pop();

			// If our current group isn't a clique, then drop it
			if (!isClique(entry.groupIDs))
				continue;
			
			// If our current group is greater than our current maximum, overwrite our current maximum
			if (entry.groupIDs.size() > greatestSet.size())
				greatestSet = entry.groupIDs;
			
			// Then for each neighbour:
			for (LanID neighbourID : s_connections[entry.ownID])
			{
				// check if they've already been visited. If so, don't check them again
				if (entry.groupIDs.find(neighbourID) != entry.groupIDs.end())
					continue;
				
				// otherwise create a new group to check for
				std::unordered_set<LanID> newGroup = entry.groupIDs;
				newGroup.insert(neighbourID);
				uint64_t newGroupHash = getSetHash(newGroup);
				// but before committing, check if we'd already seen this group before.
				// If that's the case, drop it
				if (knownSets.find(newGroupHash) != knownSets.end())
					continue;
				// Otherwise mark it as seen and add it to the queue
				knownSets.emplace(newGroupHash);
				queue.emplace(SearchEntry{newGroup, neighbourID});
			}
		}
	}
	return greatestSet;
}

static inline bool isClique(const std::unordered_set<LanID>& set) noexcept
{
	bool isCliqueVal = true;
	for (auto leftIter = set.begin(), rightIterBegin = ++set.begin() ; rightIterBegin != set.end() ; leftIter = rightIterBegin++)
	{
		const auto& conns = s_connections[*leftIter];
		for (auto rightIter = rightIterBegin ; rightIter != set.end() ; rightIter++)
		{
			if (conns.find(*rightIter) == conns.end())
			{
				isCliqueVal = false;
				break;
			}
		}
	}
	return isCliqueVal;
}
#endif
