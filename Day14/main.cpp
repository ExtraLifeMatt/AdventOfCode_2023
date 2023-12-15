// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Bit.h"
#include "ACUtils/Hash.h"
#include "ACUtils/Math.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <unordered_set>
#include <cinttypes>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params), m_MapWidth(0)
	{};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		Bitfield128 dishes;
		Bitfield128 rocks;
		Bitfield128 rockMask;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (m_MapWidth == 0)
			{
				m_MapWidth = (uint32_t)line.size();
				rockMask = Bits::CreateBitMask128(m_MapWidth, 128 - m_MapWidth);
			}
			assert(m_MapWidth == line.size());

			dishes &= ~dishes;
			rocks &= ~rocks;

			for (size_t i = 0; i < line.size(); ++i)
			{
				if (line[i] == 'O')
				{
					dishes.SetBit((uint32_t)i);
				}
				else if (line[i] == '#')
				{
					rocks.SetBit((uint32_t)i);
				}
			}

			m_Dishes.push_back(dishes);
			
			// Add rocks past our width to make for an easy mask.
			m_Rocks.push_back(rocks | rockMask);
		}
	}

	enum class Direction : uint8_t
	{
		North,
		East,
		South,
		West
	};

	static void Tilt(const std::vector<Bitfield128>& currentDishes, const std::vector<Bitfield128>& rocks, Direction dir,  uint32_t mapWidth, std::vector<Bitfield128>& outTilted)
	{
		outTilted.clear();
		outTilted.reserve(currentDishes.size());
		outTilted.insert(outTilted.begin(), currentDishes.begin(), currentDishes.end());

		Bitfield128 shiftedBits;
		switch (dir)
		{
			case Direction::North:
			{
				for (uint32_t i = 1; i < currentDishes.size(); ++i)
				{
					for (uint32_t j = i; j != 0; --j)
					{
						// Shift up as far as possible
						shiftedBits = (outTilted[j] ^ outTilted[j - 1]) & ~rocks[j - 1];
						outTilted[j] &= ~shiftedBits;
						outTilted[j - 1] |= shiftedBits;
					}
				}
			}
			break;
			case Direction::West:
			{
				// MSB = West
				uint32_t segmentStart = 0;
				uint32_t segmentSize = 127;
				Bitfield128 rockIter;
				for (uint32_t i = 0; i < currentDishes.size(); ++i)
				{
					shiftedBits = currentDishes[i];
					rockIter = ~rocks[i];

					while (!rockIter.IsZero())
					{
						Bits::GetContiguousBitsLSB128(rockIter, segmentStart, segmentSize);
						if (segmentStart >= mapWidth)
						{
							break; // Done.
						}

						Bitfield128 moveMask = Bits::CreateBitMask128(segmentStart, segmentSize);
						Bitfield128 toggledBits = shiftedBits & moveMask;
						uint32_t totalBits = Bits::PopCount128(toggledBits);
						shiftedBits &= ~toggledBits;
						shiftedBits |= Bits::CreateBitMask128(segmentStart, totalBits);
						rockIter.ClearBit(segmentStart);
					}

					outTilted[i] = shiftedBits;
				}
			
			}
			break;
			case Direction::South:
			{
				for (uint32_t i = (uint32_t)currentDishes.size() - 1; i != ~0U ; --i)
				{
					for (uint32_t j = i; j < currentDishes.size() - 1; ++j)
					{
						// Shift up as far as possible
						shiftedBits = (outTilted[j] ^ outTilted[j + 1]) & ~rocks[j + 1];
						outTilted[j] &= ~shiftedBits;
						outTilted[j + 1] |= shiftedBits;
					}
				}
			}
			break;
			case Direction::East:
			{
				uint32_t segmentStart = 0;
				uint32_t segmentSize = 0;
				Bitfield128 rockIter;
				for (uint32_t i = 0; i < currentDishes.size(); ++i)
				{
					shiftedBits = currentDishes[i];
					rockIter = ~rocks[i];

					while (!rockIter.IsZero())
					{
						Bits::GetContiguousBitsLSB128(rockIter, segmentStart, segmentSize);
						if (segmentStart >= mapWidth)
						{
							break; // Done.
						}

						Bitfield128 moveMask = Bits::CreateBitMask128(segmentStart, segmentSize);
						Bitfield128 toggledBits = shiftedBits & moveMask;
						uint32_t totalBits = Bits::PopCount128(toggledBits);
						shiftedBits &= ~toggledBits;
						shiftedBits |= Bits::CreateBitMask128( segmentStart + segmentSize - totalBits, totalBits);
						rockIter.ClearBit(segmentStart);
					}

					outTilted[i] = shiftedBits; 
				}
			}
			default:
			break;
		}
	}

	static uint32_t ScoreDishes(const std::vector<Bitfield128>& dishes)
	{
		uint32_t returnValue = 0;
		for (uint32_t i = 0; i < (uint32_t)dishes.size(); ++i)
		{
			returnValue += ((uint32_t)dishes.size() - i) * Bits::PopCount128(dishes[i]);
		}

		return returnValue;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::vector<Bitfield128> shiftedRocks;
		Tilt(m_Dishes, m_Rocks, Direction::North, m_MapWidth, shiftedRocks);

		uint32_t totalScore = ScoreDishes(shiftedRocks);

		Log("Total Score: %u", totalScore);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	void PrintMap(const std::vector<Bitfield128>& dishes, const std::vector<Bitfield128>& rocks) const
	{
		char printBuffer[2048] = { 0 };
		char lineBuffer[128] = { 0 };
		Bitfield128 currentBitfield;
		for (uint32_t i = 0; i < (uint32_t)dishes.size(); ++i)
		{
			currentBitfield = dishes[i] | rocks[i];
			for (uint32_t j = 0; j < m_MapWidth; ++j)
			{
				if (currentBitfield.IsBitSet(j))
				{
					lineBuffer[j] = dishes[i].IsBitSet(j) ? 'O' : '#';
				}
				else
				{
					lineBuffer[j] = '.';
				}
			}

			lineBuffer[m_MapWidth] = '\n';

			memcpy(printBuffer + i * (m_MapWidth + 1), lineBuffer, m_MapWidth + 1);
		}

		printBuffer[(uint32_t)dishes.size() * (m_MapWidth + 1)] = '\0';

		Log("\n%s", printBuffer);
	}

	struct CycleRecord
	{
		CycleRecord()
		{
		}

		CycleRecord(const std::vector<Bitfield128>& _record)
		: record(_record)
		{
			hash = 0;
			uint32_t primeIndex = 1;
			for (const Bitfield128& bits : record)
			{
				hash = Hash::HashCombineU64(hash, bits.GetLow());
				hash = Hash::HashCombineU64(hash, bits.GetHigh());
			}
		};

		std::vector<Bitfield128> record;
		uint64_t hash;

		bool operator==(const CycleRecord& RHS) const
		{
			if (record.size() == RHS.record.size())
			{
				for (size_t i = 0; i < record.size(); ++i)
				{
					if (record[i] != RHS.record[i])
					{
						return false;
					}
				}

				return true;
			}

			return false;
		}

		bool operator!=(const CycleRecord& RHS) const
		{
			if (record.size() != RHS.record.size())
			{
				return true;
			}
			else
			{
				for (size_t i = 0; i < record.size(); ++i)
				{
					if (record[i] != RHS.record[i])
					{
						return true;
					}
				}
			}

			return false;
		}
	};

	class CycleRecordHasher
	{
	public:
		std::size_t operator()(const CycleRecord& LHS) const
		{
			return LHS.hash;
		}
	};

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		std::vector<CycleRecord> allRecords;
		std::unordered_set<CycleRecord, CycleRecordHasher> uniqueRecords;

		Direction dirs[] = { Direction::North, Direction::West, Direction::South, Direction::East };

		std::vector<Bitfield128> pingPong[2] = { m_Dishes, m_Dishes };
		uint32_t pingPongIndex = 0;

		allRecords.reserve(1024);
		allRecords.emplace_back(m_Dishes);
		uniqueRecords.insert(allRecords.back());

		constexpr size_t totalIters = 1000000000;

		for (size_t i = 0; i < totalIters; ++i)
		{
			for (Direction dir : dirs)
			{
				Tilt(pingPong[pingPongIndex], m_Rocks, dir, m_MapWidth, pingPong[pingPongIndex ^ 1]);
				pingPongIndex ^= 1;
			}

			CycleRecord newRecord(pingPong[pingPongIndex]);

			// Looping?
			std::unordered_set<CycleRecord, CycleRecordHasher>::iterator itFind = uniqueRecords.find(newRecord);
			if ( itFind == uniqueRecords.end())
			{
				uniqueRecords.insert(newRecord);
			}
			else
			{
				assert(*itFind == newRecord);
				size_t lastSeen = std::find(allRecords.begin(), allRecords.end(), newRecord) - allRecords.begin();
				size_t cycleLength = allRecords.size() - lastSeen;
				Log("Found loop on iteration %zd, length is %zd", i, cycleLength);
				i = totalIters - (totalIters - i) % cycleLength;
				break;
			}

			allRecords.push_back(newRecord);
		}

		uint32_t totalScore = ScoreDishes(allRecords.back().record);

		Log("Total Score after spin cycle: %u", totalScore);

		// Done.
		AdventGUIInstance::PartTwo(context);


	}

	std::vector<Bitfield128> m_Dishes;
	std::vector<Bitfield128> m_Rocks;
	uint32_t m_MapWidth;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 14;
	newParams.year = 2023;
	newParams.puzzleTitle = "Parabolic Reflector Dish";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
