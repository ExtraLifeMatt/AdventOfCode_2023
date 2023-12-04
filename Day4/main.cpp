// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"
#include "imgui.h"

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
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 4;
	newParams.year = 2023;
	newParams.puzzleTitle = "Gear Ratios";
	newParams.inputFilename = "sample.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
