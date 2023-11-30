// Day 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"
#include "imgui.h"

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
		// Uncomment this to transition to Part Two on the next frame.
		// AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two

		// Uncomment this to exit on the next frame.
		// AdventGUIInstance::PartTwo(context);
	}
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 0;
	newParams.year = 2023;
	newParams.puzzleTitle = "puzzletitle";
	newParams.inputFilename = nullptr; //"input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
