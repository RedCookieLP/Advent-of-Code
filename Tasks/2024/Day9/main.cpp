#include <AoC-Module.h>
#include <vector>
#include <unordered_set>

#ifndef PART_1
constexpr static uint32_t EMPTY_ID = -1u;
#endif
static std::vector<std::pair<uint32_t, uint8_t>> s_dataBlocks;
static std::vector<uint8_t> s_freeBlocks;
static uint64_t s_dataSize;

static std::vector<std::pair<uint32_t, uint8_t>> shuffleData() noexcept;

bool handleLine(const std::string& line)
{
	if (line == "")
		return true;
	
	size_t len = line.length();
	// Because we start on a data-block, we also end on a data-block when we've got an odd number of lengths
	s_dataBlocks.reserve((len / 2u) + (len % 2u));
	s_freeBlocks.reserve(len / 2u);

	uint32_t blockID{0u};
	bool isData = true;
	for (char block : line)
	{
		uint8_t blockSize = block - '0';
		if (isData)
		{
			s_dataBlocks.emplace_back(blockID++, blockSize);
			s_dataSize += blockSize;
		}
		else
		{
			s_freeBlocks.push_back(blockSize);
		}
		isData = !isData;
	}
	return true;
}

void finalize()
{
	auto shuffledData = shuffleData();
	uint64_t checkSum = 0u;
	uint64_t currentBlockPos = 0u;
	
	std::cout << "The shuffled data is structured like this:" << std::endl;
	for (const auto& [dataID, dataLen] : shuffledData)
	{
#ifndef PART_1
		if (dataID == EMPTY_ID)
		{
			currentBlockPos += dataLen;
			continue;
		}
#endif

		for (uint8_t i = 0 ; i < dataLen ; i++)
		{
			checkSum += (dataID * (i + currentBlockPos));
		}
		currentBlockPos += dataLen;
	}
	std::cout << "The checksum of the shuffled disk-data is " << checkSum << std::endl;
}

#ifdef PART_1
static std::vector<std::pair<uint32_t, uint8_t>> shuffleData() noexcept
{
	// Now try to compact the data by shuffling data at the end through to the front
	std::vector<std::pair<uint32_t, uint8_t>> shuffledData{};
	shuffledData.reserve(s_dataBlocks.size());

	// Copy the first pair manually, since it's always at the beginning
	const auto [firstIdx, firstSize] = s_dataBlocks.front();
	shuffledData.emplace_back(firstIdx, firstSize);
	s_dataSize -= firstSize;

	size_t dataForwardIdx = 1ull;
	size_t dataReverseIdx = s_dataBlocks.size() - 1u;
	// Then handle the rest
	for (uint8_t emptyLen : s_freeBlocks)
	{
		// To fill an empty spot, try to grab as many blocks as possible from the back
		while (emptyLen > 0u && dataForwardIdx < dataReverseIdx)
		{
			uint32_t dataIdx = s_dataBlocks[dataReverseIdx].first;
			uint8_t dataSize = s_dataBlocks[dataReverseIdx].second;
			if (dataSize > emptyLen)
			{
				s_dataBlocks[dataReverseIdx].second = dataSize - emptyLen;
			}
			else
			{
				dataReverseIdx--;
			}
			dataSize = std::min(emptyLen, dataSize);
			shuffledData.emplace_back(dataIdx, dataSize);
			emptyLen -= dataSize;
		}

		// Then try to append a new data block at the end
		if (dataForwardIdx <= dataReverseIdx)
		{
			shuffledData.emplace_back(s_dataBlocks[dataForwardIdx]);
			dataForwardIdx++;
		}
		else
		{
			break;
		}
	}
	return shuffledData;
}
#else
static std::vector<std::pair<uint32_t, uint8_t>> shuffleData() noexcept
{
	std::vector<std::pair<uint32_t, uint8_t>> shuffledData{};
	shuffledData.reserve(s_dataBlocks.size() + s_freeBlocks.size());

	// We'll fill these gaps by first encoding the gaps as data blocks with Index of -1,
	// so that we can later swap the empty blocks around with the data blocks
	for (size_t i = 0 ; i < s_freeBlocks.size() ; i++)
	{
		shuffledData.emplace_back(s_dataBlocks[i]);
		shuffledData.emplace_back(EMPTY_ID, s_freeBlocks[i]);
	}
	if (s_dataBlocks.size() > s_freeBlocks.size()) // Only possible on odd number of data-blocks
	{
		shuffledData.emplace_back(s_dataBlocks.back());
	}

	// Now begin with the ID of the last data block and try to swap it with an empty spot
	for (uint32_t reverseID = s_dataBlocks.back().first ; reverseID > 0u ; reverseID--)
	{
		// First, find the entrie's index with the matching ID
		auto revIDIter = shuffledData.rbegin();
		for ( ; revIDIter != shuffledData.rend() && revIDIter->first != reverseID ; revIDIter++);
		if (revIDIter == shuffledData.rend())
		{
			std::cerr << ":(" << std::endl;
			return {};
		}

		uint8_t idxSize = revIDIter->second;

		for (auto emptyIter = shuffledData.begin() ; emptyIter < revIDIter.base() ; emptyIter++)
		{
			if (emptyIter->first != EMPTY_ID || emptyIter->second < idxSize)
				continue;
			
			// If we've found a spot, then swap the two elements and append the remaining, empty space after the data
			uint32_t remainingSpace = emptyIter->second - idxSize;
			auto postGapIter = emptyIter+1;
			emptyIter->second = idxSize;
			std::iter_swap(emptyIter, revIDIter);
			if (remainingSpace > 0u)
			{
				shuffledData.insert(postGapIter, std::pair<uint32_t, uint8_t>{EMPTY_ID, remainingSpace});
			}
			break;
		}
	}

	return shuffledData;
}
#endif
