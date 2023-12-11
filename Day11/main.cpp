// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/IntVec.h"
#include "ACUtils/StringUtil.h"
#include <inttypes.h>
#include <vector>


class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		size_t rowIdx = 0;
		size_t colIdx = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			
			// Track empty space as we go.
			if (m_EmptyRowCol.empty())
			{
				m_MapWidth = line.size();
				m_EmptyRowCol.reserve(m_MapWidth * 2); // Assume squared dimensions
				while (m_EmptyRowCol.size() != m_MapWidth * 2)
				{
					m_EmptyRowCol.emplace_back(true);
				}
			}

			for (char c : line)
			{
				if (c != '.')
				{
					colIdx = m_RawMap.size() % m_MapWidth;
					rowIdx = m_RawMap.size() / m_MapWidth;

					m_Galaxies.push_back(IntVec2((int32_t)colIdx, (int32_t)rowIdx));

					// Mark col
					m_EmptyRowCol[colIdx] = false;
					// Mark row
					m_EmptyRowCol[m_MapWidth + rowIdx] = false;
				}
				m_RawMap.push_back(c);
			}
		}

	}

	uint32_t GetManhattanDistance(const IntVec2& a, const IntVec2& b) const
	{
		return abs(a.x - b.x) + abs(a.y - b.y);
	}

	uint64_t GetManhattanDistance64(const Int64Vec2& a, const Int64Vec2& b) const
	{
		return abs(a.x - b.x) + abs(a.y - b.y);
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int32_t xOffset = 0;
		int32_t yOffset = 0;
		std::vector<IntVec2> offsetGalaxies;
		offsetGalaxies.reserve(m_Galaxies.size());

		for (const IntVec2& galaxy : m_Galaxies)
		{
			xOffset = 0;
			yOffset = 0;
			for (int32_t i = 0; i < m_MapWidth; ++i)
			{
				if (m_EmptyRowCol[i] && i > galaxy.x)
				{
					++xOffset;
				}
			}

			for (int32_t i = 0; i < m_MapWidth; ++i)
			{
				if (m_EmptyRowCol[m_MapWidth + i] && i > galaxy.y)
				{
					++yOffset;
				}
			}

			offsetGalaxies.push_back(galaxy - IntVec2(xOffset, yOffset));
		}

		uint32_t totalSum = 0;
		uint32_t distance = 0;
		for (size_t i = 0; i < offsetGalaxies.size(); ++i)
		{
			for (size_t j = i + 1; j < offsetGalaxies.size(); ++j)
			{
				distance = GetManhattanDistance(offsetGalaxies[i], offsetGalaxies[j]);
				totalSum += distance;
			}
		}

		Log("Total Sum of Smallest Distances = %u", totalSum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		int64_t xOffset = 0;
		int64_t yOffset = 0;
		std::vector<Int64Vec2> offsetGalaxies;
		offsetGalaxies.reserve(m_Galaxies.size());

		for (const IntVec2& galaxy : m_Galaxies)
		{
			xOffset = 0;
			yOffset = 0;
			for (int32_t i = 0; i < m_MapWidth; ++i)
			{
				if (m_EmptyRowCol[i] && i > galaxy.x)
				{
					++xOffset;
				}
			}

			for (int32_t i = 0; i < m_MapWidth; ++i)
			{
				if (m_EmptyRowCol[m_MapWidth + i] && i > galaxy.y)
				{
					++yOffset;
				}
			}

			xOffset =  (xOffset * 1000000) - xOffset;
			yOffset =  (yOffset * 1000000) - yOffset;

			offsetGalaxies.push_back(Int64Vec2((int64_t)galaxy.x, (int64_t)galaxy.y) - Int64Vec2((int64_t)xOffset, (int64_t)yOffset));
		}

		uint64_t totalSum = 0;
		uint64_t distance = 0;
		for (size_t i = 0; i < offsetGalaxies.size(); ++i)
		{
			for (size_t j = i + 1; j < offsetGalaxies.size(); ++j)
			{
				distance = GetManhattanDistance64(offsetGalaxies[i], offsetGalaxies[j]);
				totalSum += distance;
			}
		}

		Log("Total Sum of Smallest Distances = %" PRIu64, totalSum);
		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	
	size_t m_MapWidth;
	std::vector<char> m_RawMap;
	std::vector<bool> m_EmptyRowCol;
	std::vector<IntVec2> m_Galaxies;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 11;
	newParams.year = 2023;
	newParams.puzzleTitle = "Cosmic Expansion";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
