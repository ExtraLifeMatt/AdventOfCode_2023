// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Math.h"
#include "ACUtils/Debug.h"
#include "ACUtils/Hash.h"
#include "ACUtils/StringUtil.h"
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <inttypes.h>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params)
	{};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<char> currentMap;
		uint32_t rowWidth = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (line.empty())
			{
				if (currentMap.size())
				{
					m_Maps.emplace_back(currentMap, rowWidth);
				}
				currentMap.clear();
				rowWidth = 0;
			}
			else
			{
				if (rowWidth == 0)
				{
					rowWidth = (uint32_t)line.size();
				}
				assert(rowWidth == (uint32_t)line.size());
				currentMap.insert(currentMap.end(), line.begin(), line.end());
			}
		}

		// Grab any remainder.
		if (currentMap.size())
		{
			m_Maps.emplace_back(currentMap, rowWidth);
			currentMap.clear();
			rowWidth = 0;
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		EReflectionType foundReflection = EReflectionType::None;
		uint32_t outIndex = 0;
		uint32_t outDistance = 0;
		uint32_t totalScore = 0;
		constexpr uint32_t multiplier[] = {0, 1, 100};
		for (const Map& currentMap : m_Maps)
		{
			foundReflection = FindReflectionIndex(currentMap, outIndex, outDistance);
			Log("Map [%u x %u] Index %u Distance %u Orientation %s", currentMap.width, currentMap.height, outIndex, outDistance, foundReflection == EReflectionType::Horizontal ? "Horizontal" : "Vertical");
			assert(foundReflection != EReflectionType::None);
			totalScore += outDistance * multiplier[static_cast<uint8_t>(foundReflection)];
		}

		Log("Total %u", totalScore);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		EReflectionType foundReflection = EReflectionType::None;
		uint32_t outIndex = 0;
		uint32_t outDistance = 0;
		uint32_t totalScore = 0;
		constexpr uint32_t multiplier[] = { 0, 1, 100 };
		for (const Map& currentMap : m_Maps)
		{
			foundReflection = FindReflectionIndex(currentMap, outIndex, outDistance, true);
			Log("Map (SMUDGED) [%u x %u] Index %u Distance %u Orientation %s", currentMap.width, currentMap.height, outIndex, outDistance, foundReflection == EReflectionType::Horizontal ? "Horizontal" : "Vertical");
			assert(foundReflection != EReflectionType::None);
			totalScore += outDistance * multiplier[static_cast<uint8_t>(foundReflection)];
		}

		Log("Total %u", totalScore);
		
		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	enum class EReflectionType : uint8_t
	{
		None = 0,
		Vertical,
		Horizontal
	};

	struct Map
	{
		Map(): width(0), height(0), map(){};
		Map(const std::vector<char>& _map, uint32_t _width) : width(_width), height((uint32_t)_map.size() / _width), map(_map)
		{
			// Build rows
			uint64_t temp = 0;
			for (uint32_t i = 0; i < height; ++i)
			{
				temp = 0;
				for (uint32_t j = 0; j < width; ++j)
				{
					if (map[i * width + j] == '#')
					{
						temp |= 1ULL << j;
					}
				}
				rows.emplace_back(temp);
			}

			// build cols
			for (uint32_t i = 0; i < width; ++i)
			{
				temp = 0;
				for (uint32_t j = 0; j < height; ++j)
				{
					if (map[j * width + i] == '#')
					{
						temp |= 1ULL << j;
					}
				}
				cols.emplace_back(temp);
			}
		}

		// Check for a Horizontal Line
		bool CheckRows(uint32_t LHS, uint32_t RHS) const
		{
			assert(LHS < height && RHS < height);
			return rows[LHS] == rows[RHS];
		}

		bool CanSmudgeRows(uint32_t LHS, uint32_t RHS) const
		{
			return Bits::PopCount64(rows[LHS] ^ rows[RHS]) == 1;
		}

		// Check for a Vertical Line
		bool CheckCols(uint32_t LHS, uint32_t RHS) const
		{
			assert(LHS < width && RHS < width);
			return cols[LHS] == cols[RHS];
		}

		bool CanSmudgeCol(uint32_t LHS, uint32_t RHS) const
		{
			return Bits::PopCount64(cols[LHS] ^ cols[RHS]) == 1;
		}

		uint32_t width;
		uint32_t height;
		std::vector<char> map;
		std::vector<uint64_t> rows;
		std::vector<uint64_t> cols;
	};


	EReflectionType FindReflectionIndex(const Map& map, uint32_t& outIndex, uint32_t& outDistance, bool fixSmudge = false) const
	{
		outIndex = 0;
		outDistance = 0;
		bool hasSmudge = fixSmudge;

		// Horizontal Line check
		for (uint32_t i = 0; i < map.height -1; ++i )
		{
			hasSmudge = fixSmudge;

			bool valid = map.CheckRows(i, i + 1);
			if (!valid && hasSmudge)
			{
				valid = map.CanSmudgeRows(i, i + 1);
				hasSmudge = false;
			}

			if (valid)
			{
				uint32_t maxDistance = std::min(i, map.height - 1 - (i + 1));
				for (size_t j = 0; j < maxDistance; ++j)
				{
					valid &= map.CheckRows(i - 1 - j, (i + 1) + 1 + j);
					if (!valid && hasSmudge)
					{
						valid = map.CanSmudgeRows(i - 1 - j, (i + 1) + 1 + j);
						hasSmudge = false;
					}
				}

				if (valid)
				{
					outIndex = i;
					outDistance = i + 1;
					return EReflectionType::Horizontal;
				}
			}
		}

		// Vertical Line Check
		for (uint32_t i = 0; i < map.width - 1; ++i)
		{
			hasSmudge = fixSmudge;

			bool valid = map.CheckCols(i, i + 1);
			if (!valid && hasSmudge)
			{
				valid = map.CanSmudgeCol(i, i + 1);
				hasSmudge = false;
			}

			if (valid)
			{
				uint32_t maxDistance = std::min(i, map.width - 1 -  (i + 1));
				for (size_t j = 0; j < maxDistance; ++j)
				{
					valid &= map.CheckCols(i - 1 - j, (i + 1 ) + 1 + j);
					if (!valid && hasSmudge)
					{
						valid = map.CanSmudgeCol(i - 1 - j, (i + 1) + 1 + j);
						hasSmudge = false;
					}
				}

				if (valid)
				{
					outIndex = i;
					outDistance = i + 1;
					return EReflectionType::Vertical;
				}
			}
		}

		return EReflectionType::None;
	}

	std::vector<Map> m_Maps; 
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 13;
	newParams.year = 2023;
	newParams.puzzleTitle = "Point of Incidence";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
