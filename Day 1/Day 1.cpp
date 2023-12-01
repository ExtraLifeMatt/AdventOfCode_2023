// Day 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"
#include "imgui.h"

#include "ACUtils/Algorithm.h"
#include <array>
#include <string>
#include <algorithm>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		fileReader.ReadAllLines(true, m_allLines);
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One	
		const char* firstDigit = nullptr;
		const char* lastDigit = nullptr;

		uint32_t parseValue;
		uint32_t sumTotal = 0;
		for (const std::string& line : m_allLines)
		{
			firstDigit = &line[0];
			lastDigit = &line[line.size() - 1];
			
			while((size_t)(firstDigit - line.data()) < line.size() && !isdigit(*firstDigit)) { ++firstDigit; }
			while((size_t)(lastDigit - line.data()) < line.size() && !isdigit(*lastDigit)) { --lastDigit; }

			parseValue = ((*firstDigit) - '0') * 10 + (*lastDigit - '0');
		
			Log("%s -> %c + %c -> %d", line.c_str(), *firstDigit, *lastDigit, parseValue);

			sumTotal += parseValue;
		}

		Log("Part One Sum Total = %d", sumTotal);
		
		// Done.
		 AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		const char* firstDigit = nullptr;
		const char* lastDigit = nullptr;

		uint32_t parseValue;
		uint32_t sumTotal = 0;

		char firstValueBuffer[16] = { 0 };
		char secondValueBuffer[16] = { 0 };

		for (const std::string& line : m_allLines)
		{
			firstDigit = &line[0];
			lastDigit = &line[line.size() - 1];

			// Find numeric values
			while ((size_t)(firstDigit - line.data()) < line.size() && !isdigit(*firstDigit)) { ++firstDigit; }
			while ((size_t)(lastDigit - line.data()) < line.size() && !isdigit(*lastDigit)) { --lastDigit; }

			// Find string values
			for (const char* digitStr : digitNames)
			{
				const char* foundStr = strstr(line.data(), digitStr);

				while (foundStr)
				{
					firstDigit = std::min(firstDigit, foundStr);
					lastDigit = std::max(lastDigit, foundStr);

					foundStr = strstr(foundStr + strlen(digitStr), digitStr);
				}
			}

			parseValue = ParseValue(firstDigit, firstValueBuffer, 16) * 10;
			parseValue += ParseValue(lastDigit, secondValueBuffer, 16);

			sumTotal += parseValue;

			Log("%s -> %s + %s -> %d", line.c_str(), firstValueBuffer, secondValueBuffer, parseValue);
		}

		Log("Part Two Sum Total = %d", sumTotal);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	int32_t ParseValue(const char* str, char* logBuffer, size_t logBufferSize) const
	{
		if (isdigit(*str))
		{
			sprintf_s(logBuffer, logBufferSize, "%c", *str);
			return ((*str) - '0');
		}

		int32_t index = Algorithm::find_index_of(digitNames.begin(), digitNames.end(), [&](const char* LHS)
		{ 
			return strncmp(LHS, str, strlen(LHS)) == 0; 
		});

		assert(index != -1);
		sprintf_s(logBuffer, logBufferSize, "%s", digitNames[index]);
		return (index + 1);
	}

	static constexpr std::array<const char*, 9> digitNames{ "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
	std::vector<std::string> m_allLines;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 0;
	newParams.year = 2023;
	newParams.puzzleTitle = "Trebuchet?!";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
