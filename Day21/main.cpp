// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Hash.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/Math.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <cinttypes>
#include <queue>
#include <unordered_set>

struct WalkState
{
	WalkState(const IntVec2& _pos, uint32_t _steps) : Pos(_pos), StepsRemaining(_steps) {};
	IntVec2 Pos;
	uint32_t StepsRemaining;
	uint64_t GetHash() const { return Pos.x | Pos.y << 12 | StepsRemaining << 24UL; }
	bool operator==(const WalkState& RHS) const { return GetHash() == RHS.GetHash(); }
};

ENABLE_STL_HASH(WalkState, GetHash);

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
		std::vector<std::string> tokens;

		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (m_MapWidth == 0)
			{
				m_MapWidth = (uint32_t)line.size();
			}
			assert(m_MapWidth == line.size());

			size_t startPosIdx = line.find('S');
			if (startPosIdx != std::string::npos)
			{
				m_StartPos = IntVec2((uint32_t)startPosIdx, (uint32_t)m_Map.size() / m_MapWidth);
				line[startPosIdx] = '.';
			}
			m_Map.insert(m_Map.end(), line.begin(), line.end());
		}

		m_MapHeight = (uint32_t)m_Map.size() / m_MapWidth;
	}

	bool IsValidPos(const IntVec2& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || (uint32_t)pos.x >= m_MapWidth || (uint32_t)pos.y >= m_MapHeight)
		{
			return false;
		}

		return m_Map[pos.y * m_MapWidth + pos.x] == '.';
	}


	bool IsValidPosInf(const IntVec2& pos) const
	{
		// This is dumb, not sure why it has to be this way.
		int yTest = pos.y;
		int xTest = pos.x;

		if (yTest < 0)
		{
			yTest = m_MapHeight - (abs(yTest) % m_MapHeight);
		}

		if (xTest < 0)
		{
			xTest = m_MapWidth - (abs(xTest) % m_MapWidth);
		}

		yTest = (yTest + m_MapHeight) % m_MapHeight;
		xTest = (xTest + m_MapWidth) % m_MapWidth;

		return m_Map[yTest * m_MapWidth + xTest] == '.';
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint64_t totalSteps = SolveForSteps(64);

		Log("Total Reachable Steps: %zd", totalSteps);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	uint64_t SolveForSteps(uint32_t numSteps) const
	{
		std::unordered_set<IntVec2> outValues;
		std::unordered_set<IntVec2> cache;
		std::queue<WalkState> queue;

		queue.emplace(m_StartPos, numSteps);
		cache.insert(m_StartPos);

		WalkState currentState(m_StartPos, 0);
		while (!queue.empty())
		{
			currentState = queue.front();
			queue.pop();

			if (currentState.StepsRemaining % 2 == 0)
			{
				// Can always reach even.
				outValues.insert(currentState.Pos);
			}

			if (currentState.StepsRemaining == 0)
			{
				continue;
			}

			static const IntVec2 offsets[] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
			for (const IntVec2 offset : offsets)
			{
				if (IsValidPosInf(currentState.Pos + offset) && cache.find(currentState.Pos + offset) == cache.end())
				{
					queue.emplace(currentState.Pos + offset, currentState.StepsRemaining - 1);
					cache.insert(currentState.Pos + offset);
				}
			}
		}

		return outValues.size();
	}

	static constexpr uint64_t STEP_LIMIT = 26501365;
	static constexpr uint64_t TEST_MAP_SIZE = 131;
	static constexpr uint64_t REMAINDER_IN_LOOP = STEP_LIMIT % TEST_MAP_SIZE;

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		std::vector<int64_t> quadFormulaParams;

		for (size_t i = 0; i < 3; ++i)
		{
			quadFormulaParams.push_back((int64_t)SolveForSteps(i * m_MapWidth + REMAINDER_IN_LOOP));
		}

		int64_t p0 = quadFormulaParams[0];
		int64_t p1 = quadFormulaParams[1] - quadFormulaParams[0];
		int64_t p2 = quadFormulaParams[2] - quadFormulaParams[1];
		int64_t iters = (int64_t)(STEP_LIMIT - REMAINDER_IN_LOOP) / (int64_t)m_MapWidth;

		int64_t maxTouchesAtLimit = p0 + p1 * iters + (iters * (iters - 1LL) / 2LL) * (p2 - p1);

		Log("Total Reachable Steps: %lld", maxTouchesAtLimit);

		// Done.
		AdventGUIInstance::PartTwo(context);

	}

	IntVec2 m_StartPos;
	std::vector<char> m_Map;
	uint32_t m_MapWidth;
	uint32_t m_MapHeight;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 21;
	newParams.year = 2023;
	newParams.puzzleTitle = "Step Counter";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
