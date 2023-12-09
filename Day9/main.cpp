// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/StringUtil.h"
#include <inttypes.h>
#include <vector>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

	struct NumberTree
	{
		std::vector<int64_t> m_Values;
		NumberTree* m_Parent = nullptr;
		NumberTree* m_Child = nullptr;
	};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			assert(line.size() != 0);
			StringUtil::SplitBy(line, " ", tokens);
			NumberTree* newTree = new NumberTree();
			for (const std::string& token : tokens)
			{
				newTree->m_Values.push_back(StringUtil::AtoiI64(token.c_str()));
			}

			m_Trees.push_back(newTree);
		}
	}

	void SolveTree(NumberTree* currentTree) const
	{
		std::vector<int64_t> diffs;
		diffs.reserve(currentTree->m_Values.size() - 1);
		bool allZero = true;
		int64_t delta = 0;
		for (size_t i = 0; i < currentTree->m_Values.size() - 1; ++i)
		{
			delta = currentTree->m_Values[i + 1] - currentTree->m_Values[i];
			diffs.push_back(delta);
			allZero &= (delta == 0);
		}

		NumberTree* Leaf = new NumberTree();
		Leaf->m_Values = std::move(diffs);
		Leaf->m_Parent = currentTree;
		currentTree->m_Child = Leaf;

		if (!allZero)
		{
			SolveTree(Leaf);
		}
	}

	int64_t GetExtrapolatedSum(const NumberTree* tree) const
	{
		if (tree->m_Child)
		{
			return tree->m_Values.back() + GetExtrapolatedSum(tree->m_Child);
		}

		return tree->m_Values.back();
	}

	int64_t GetReverseExtrapolatedSum(const NumberTree* tree) const
	{
		if (tree->m_Child)
		{
			return tree->m_Values.front() - GetReverseExtrapolatedSum(tree->m_Child);
		}

		return tree->m_Values.front();
	}

	void PrintTree(const NumberTree* tree)
	{
		char displayBuffer[256] = { 0 };
		for (size_t i = 0; i < tree->m_Values.size(); ++i)
		{
			if (i == 0)
			{
				sprintf_s(displayBuffer, 256, "%" PRId64, tree->m_Values[i]);
			}
			else
			{
				sprintf_s(displayBuffer, 256, "%s %" PRId64, displayBuffer, tree->m_Values[i]);
			}
		}

		if (tree->m_Parent != nullptr)
		{
			int depth = 0;
			const NumberTree* current = tree->m_Parent;
			while (current)
			{
				sprintf_s(displayBuffer, 256, " %s", displayBuffer);
				current = current->m_Parent;
			}
		}

		Log("%s", displayBuffer);

		if (tree->m_Child)
		{
			PrintTree(tree->m_Child);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		int64_t sum = 0;
		for (NumberTree* tree : m_Trees)
		{
			SolveTree(tree);
			
			//PrintTree(tree);

			sum += GetExtrapolatedSum(tree);
		}

		Log("Total Sum = %" PRId64, sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		int64_t sum = 0;
		for (NumberTree* tree : m_Trees)
		{
			SolveTree(tree);

			//PrintTree(tree);

			sum += GetReverseExtrapolatedSum(tree);
		}

		Log("Total Reversed Sum = %" PRId64, sum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<NumberTree*> m_Trees;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 9;
	newParams.year = 2023;
	newParams.puzzleTitle = "Mirage Maintenance";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
