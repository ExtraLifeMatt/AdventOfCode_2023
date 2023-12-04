// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"
#include "imgui.h"

#include "ACUtils/IntVec.h"
#include "ACUtils/StringUtil.h"
#include <vector>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		// Format: Game ##: # color, # color, # color; ...
		std::string currentLine;
		std::vector<std::string> rounds;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			currentLine = fileReader.ReadLine(true);
			size_t colonIdx = currentLine.find(':');
			
			Log("%s", currentLine.c_str());

			assert(colonIdx != std::string::npos);
			currentLine = currentLine.substr(colonIdx + 1); // Chop off the "Game ##:", it's implied by order.
			
			StringUtil::SplitBy(currentLine, ";", rounds, true); // Split rounds by ;
			assert(rounds.size() != 0);

			GameState newState;
			for (const std::string& round : rounds)
			{
				IntVec3 roundResults(0);
				StringUtil::SplitBy(round, ",", tokens, true);
				for (size_t i = 0; i < tokens.size(); ++i)
				{
					std::string& currentToken = tokens[i];
					size_t spaceIndex = currentToken.find(' ');
					assert(spaceIndex != std::string::npos);
					std::string numSubStr = currentToken.substr(0, spaceIndex);
					std::string colorStr = currentToken.substr(spaceIndex + 1);
					if (_stricmp(colorStr.c_str(), "red") == 0)
					{
						roundResults.x = atoi(numSubStr.c_str());
					}
					else if (_stricmp(colorStr.c_str(), "green") == 0)
					{
						roundResults.y = atoi(numSubStr.c_str());
					}
					else if (_stricmp(colorStr.c_str(), "blue") == 0)
					{
						roundResults.z = atoi(numSubStr.c_str());
					}
					else
					{
						assert(false); // Unknown color?
					}

					Log("\tred: %d, green: %d, blue: %d", roundResults.x, roundResults.y, roundResults.z);
				}

				newState.m_Rounds.push_back(roundResults);
			}

			m_GameStates.push_back(newState);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One	
		const IntVec3 MaximumCubes(12, 13, 14);

		size_t idSum = 0;
		bool isPossible = false;
		for (size_t i = 0; i < m_GameStates.size(); ++i)
		{
			isPossible = true;
			for (const IntVec3& round : m_GameStates[i].m_Rounds)
			{
				isPossible &= round.AllLessThanOrEqual(MaximumCubes);
			}

			if (isPossible)
			{
				idSum += i + 1;
			}
		}

		Log("Total ID Sum: %zd", idSum);
		
		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		size_t powSum = 0;
		IntVec3 roundMinimum;
		for (size_t i = 0; i < m_GameStates.size(); ++i)
		{
			roundMinimum = IntVec3(1);
			for (const IntVec3& round : m_GameStates[i].m_Rounds)
			{
				roundMinimum = round.PerComponentMax(roundMinimum);
			}

			powSum += roundMinimum.x * roundMinimum.y * roundMinimum.z;
		}

		Log("Total Power Sum: %zd", powSum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	struct GameState
	{
		std::vector<IntVec3> m_Rounds;
	};

	std::vector<GameState> m_GameStates;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 2;
	newParams.year = 2023;
	newParams.puzzleTitle = "Cube Conundrum";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
