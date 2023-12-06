// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/StringUtil.h"
#include <algorithm>
#include <vector>
#include <cctype>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		std::vector<uint64_t>* outputs[] = { &m_Time, &m_Distance };
		uint32_t outputIndex = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			size_t colonIdx = line.find(':');
			assert(colonIdx != std::string::npos);
			line = line.substr(colonIdx + 1);
			StringUtil::SplitBy(line, " ", tokens);
			for (const std::string& token : tokens)
			{
				if (token.size() == 0)
				{
					continue;
				}

				outputs[outputIndex]->emplace_back(StringUtil::AtoiU64(token.c_str()));
			}

			char partTwoValue[32] = { 0 };
			uint32_t charIndex =0;
			for (char c : line)
			{
				if (isdigit(c))
				{
					partTwoValue[charIndex++] = c;
				}
			}

			outputs[outputIndex]->emplace_back(StringUtil::AtoiU64(partTwoValue));

			++outputIndex;
		}
	}

	void FindSolutions(uint64_t time, uint64_t distanceToBeat, std::vector<uint64_t>& outResults)
	{
		uint64_t runDistance = 0;
		for (uint64_t i = 1; i < time; ++i)
		{
			runDistance = (time - i) * i;
			if (runDistance > distanceToBeat)
			{
				outResults.emplace_back(i);

				//Log("For Time [%u] with [%u] distance. Holding for [%u] seconds WILL win with a distance of [%u].", time, distanceToBeat, i, runDistance);
			}
			else
			{
				//Log("For Time [%u] with [%u] distance. Holding for [%u] seconds WILL NOT win. Distance [%u].", time, distanceToBeat, i, runDistance);
			}
		}
	}


	uint64_t FindSolutions(uint64_t time, uint64_t distanceToBeat)
	{
		uint64_t runDistance = 0;
		for (uint64_t i = 1; i < time; ++i)
		{
			runDistance = (time - i) * i;
			if (runDistance > distanceToBeat)
			{
				return runDistance - (i * 2) + 1;
			}
		}

		return 0;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint64_t sum = 1;
		std::vector<uint64_t> validTimes;
		for (size_t i = 0; i < m_Time.size() - 1; ++i)
		{
			validTimes.clear();
			FindSolutions(m_Time[i], m_Distance[i], validTimes);
			sum *= (uint64_t)std::max((uint64_t)validTimes.size(), 1ULL);
		}

		Log("Product of Valid Options: %u", sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::vector<uint64_t> validTimes;
		FindSolutions(m_Time.back(), m_Distance.back(), validTimes);
		uint64_t otherSolution = FindSolutions(m_Time.back(), m_Distance.back());

		Log("Valid Solutions: %zd", validTimes.size());

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<uint64_t> m_Time;
	std::vector<uint64_t> m_Distance;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 6;
	newParams.year = 2023;
	newParams.puzzleTitle = "Wait For It";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
