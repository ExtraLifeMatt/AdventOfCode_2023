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
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<uint64_t> m_Time;
	std::vector<uint64_t> m_Distance;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 7;
	newParams.year = 2023;
	newParams.puzzleTitle = "Wait For It";
	newParams.inputFilename = "sample.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
