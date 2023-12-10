// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/AABB.h"
#include "ACUtils/Bit.h"
#include "ACUtils/Enum.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/Math.h"
#include "ACUtils/StringUtil.h"
#include <queue>
#include <inttypes.h>
#include <vector>
#include <unordered_set>

enum class ExitDir : uint8_t
{
	None = 0,
	North = 1 << 0,
	East  = 1 << 1,
	South = 1 << 2,
	West  = 1 << 3
};

DECLARE_ENUM_BITFIELD_OPERATORS(ExitDir);

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		m_MapWidth = 0;

		ExitDir dir;
		std::string line;
		while(!fileReader.IsEOF())
		{
			/*
				| is a vertical pipe connecting north and south.
				- is a horizontal pipe connecting east and west.
				L is a 90-degree bend connecting north and east.
				J is a 90-degree bend connecting north and west.
				7 is a 90-degree bend connecting south and west.
				F is a 90-degree bend connecting south and east.
				. is ground; there is no pipe in this tile.
				S is the starting position of the animal; there is a pipe on this
			*/
			line = fileReader.ReadLine();

			if (m_MapWidth == 0)
			{
				m_MapWidth = (uint32_t)line.size();
				m_Distances.reserve(m_MapWidth * m_MapWidth);
				m_Map.reserve(m_MapWidth * m_MapWidth);
			}
			assert((uint32_t)line.size() == m_MapWidth);

			for (char c : line)
			{
				switch (c)
				{
					case '|' : dir = ExitDir::North | ExitDir::South; break;
					case '-' : dir = ExitDir::East | ExitDir::West; break;
					case 'L' : dir = ExitDir::North | ExitDir::East; break;
					case 'J' : dir = ExitDir::North | ExitDir::West; break;
					case '7' : dir = ExitDir::South | ExitDir::West; break;
					case 'F' : dir = ExitDir::South | ExitDir::East; break;
					case 'S' : m_StartPos = IntVec2((uint32_t)m_Map.size() % m_MapWidth, (uint32_t)m_Map.size() / m_MapWidth); dir = ExitDir::None; break;
					case '.' : dir = ExitDir::None; break;
					default: assert(false);
				}

				m_Map.push_back(dir);
				m_Distances.emplace_back(INT_MAX);
			}
		}
		
		// Figure out the tile at S real quick.
		const ExitDir neighbors[] = 
		{ 
			GetPipe(m_StartPos + IntVec2(0, -1)), // North
			GetPipe(m_StartPos + IntVec2(1, 0)),  // East
			GetPipe(m_StartPos + IntVec2(0, 1)),  // South
			GetPipe(m_StartPos + IntVec2(-1, 0))  // West
		};

		const ExitDir requiredMask[] =
		{
			ExitDir::South,
			ExitDir::West,
			ExitDir::North,
			ExitDir::East
		};

		ExitDir startMask = ExitDir::None;
		for (size_t i = 0; i < ARRAY_SIZE(neighbors); ++i)
		{
			if ((neighbors[i] & requiredMask[i]) == requiredMask[i])
			{
				ExitDir startExit = GetOppositeDir(requiredMask[i]);
				startMask |= startExit;
			}
		}

		m_Map[m_StartPos.y * m_MapWidth + m_StartPos.x] = startMask;
		m_Distances[m_StartPos.y * m_MapWidth + m_StartPos.x] = 0;

		while (startMask != ExitDir::None)
		{
			ExitDir currentExit = (ExitDir)(1 << Bits::CountTrailingZeros((uint32_t)startMask));
			MapDistances(m_StartPos + GetOffset(currentExit), currentExit);
			startMask &= ~currentExit;
		}
	}

	void MapDistances(IntVec2 loc, ExitDir dir)
	{
		IntVec2 nextLoc;
		int32_t totalSteps = 0;
		while (loc != m_StartPos)
		{
			++totalSteps;
			m_Distances[loc.y * m_MapWidth + loc.x] = std::min(m_Distances[loc.y * m_MapWidth + loc.x], totalSteps);
			dir = GetPipe(loc) ^ GetOppositeDir(dir);
			loc += GetOffset(dir);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int32_t maxDistance = 0;
		for (int32_t distance : m_Distances)
		{
			if (distance != INT_MAX)
			{
				maxDistance = std::max(maxDistance, distance);
			}
		}

		Log("Max Steps %d", maxDistance);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	bool IsPointInPoly(const IntVec2& pos, const std::vector<IntVec2>& polyVerts) const
	{
		bool contained = false;

		size_t i = 0;
		size_t j = polyVerts.size() - 1;

		for (; i < polyVerts.size(); j = i++)
		{
			if ((polyVerts[i].y > pos.y) != (polyVerts[j].y > pos.y) && pos.x < (polyVerts[j].x - polyVerts[i].x) * (pos.y - polyVerts[i].y) / (polyVerts[j].y - polyVerts[i].y) + polyVerts[i].x)
			{
				contained = !contained;
			}
		}

		return contained;
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		// 
		// Build Poly Representation.
		m_PipeVerts.push_back(m_StartPos);
		
		ExitDir currentDir = (ExitDir)(1 << Bits::CountTrailingZeros((uint32_t)GetPipe(m_StartPos)));
		ExitDir nextDir;

		IntAABB2D bounds(m_StartPos, 0);
		
		IntVec2 loc = m_StartPos + GetOffset(currentDir);
		while (loc != m_StartPos)
		{
			nextDir = GetPipe(loc) ^ GetOppositeDir(currentDir);
			if (nextDir != currentDir)
			{
				m_PipeVerts.push_back(loc);
				bounds = bounds.ExpandToContain(loc);
			}
			currentDir = nextDir;
			loc += GetOffset(currentDir);
		}
		m_PipeVerts.push_back(m_StartPos); // Dupe the first.

		uint32_t totalArea = 0;
		IntVec2 iterPos;
		for (size_t i = 0; i < m_Map.size(); ++i)
		{
			iterPos = IntVec2(i % m_MapWidth, i / m_MapWidth);
			if (!bounds.Contains(iterPos))
			{
				continue;
			}

			bool IsOnPoly = false;

			for (size_t i = 0; i <  m_PipeVerts.size() -1; ++i)
			{
				int32_t minLineX = std::min(m_PipeVerts[i].x, m_PipeVerts[i + 1].x);
				int32_t maxLineX = std::max(m_PipeVerts[i].x, m_PipeVerts[i + 1].x);
				int32_t minLineY = std::min(m_PipeVerts[i].y, m_PipeVerts[i + 1].y);
				int32_t maxLineY = std::max(m_PipeVerts[i].y, m_PipeVerts[i + 1].y);

				if (iterPos.x >= minLineX && iterPos.x <= maxLineX && iterPos.y >= minLineY && iterPos.y <= maxLineY)
				{
					IsOnPoly = true;
				}
			}

			if (!IsOnPoly && IsPointInPoly(iterPos, m_PipeVerts))
			{
				++totalArea;
			}
		}

		Log("Total Area: %u", totalArea);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	ExitDir GetPipe(const IntVec2& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.x >= m_MapWidth || pos.y >= ((int32_t)m_Map.size() / m_MapWidth))
		{
			return ExitDir::None;
		}

		return m_Map[pos.y * m_MapWidth + pos.x];
	}

	ExitDir GetOppositeDir(ExitDir inDir) const
	{
		// Poor man's bit rotate.
		return (ExitDir)(1 << ((Bits::CountTrailingZeros((uint32_t)inDir) + 2) % 4));
	}
	IntVec2 GetOffset(ExitDir inDir) const
	{
		static IntVec2 s_offsets[] =  
		{
			IntVec2(0, -1), // North
			IntVec2(1, 0),  // East
			IntVec2(0, 1),  // South
			IntVec2(-1, 0)  // West
		};

		return s_offsets[Bits::CountTrailingZeros((uint32_t)inDir)];
	}

	IntVec2 m_StartPos;
	std::vector<char>    m_RawMap;
	std::vector<ExitDir> m_Map;
	std::vector<int32_t> m_Distances;
	std::vector<IntVec2> m_PipeVerts;
	int32_t m_MapWidth;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 10;
	newParams.year = 2023;
	newParams.puzzleTitle = "Pipe Maze";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
