// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"
#include "imgui.h"

#include "ACUtils/AABB.h"
#include "ACUtils/Algorithm.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <cctype>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	enum class ENode
	{
		ENode_Number,
		ENode_Symbol,
	};
	
	struct Node
	{
		ENode type;
		int32_t number;
		char  symbol;
		IntAABB2D  bounds;
	};

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		const char* currentChar = nullptr;
		char numberBuffer[8];
		int32_t yPos = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			currentChar = line.c_str();
			while (*currentChar != '\0' && *currentChar != '\r')
			{
				if (isdigit(*currentChar))
				{
					const char* numStart = currentChar;
					do 
					{
						++currentChar;
					} while (isdigit(*currentChar) && *currentChar != '\0' && *currentChar != '\r');
				
					size_t numberLength = (currentChar - numStart);
					memcpy(numberBuffer, numStart, numberLength);
					numberBuffer[numberLength] = '\0';

					size_t numberStart = numStart - line.c_str();

					IntVec2 boundsMin((int32_t)numberStart, yPos);
					IntVec2 boundsMax((int32_t)(numberStart + numberLength - 1), yPos);

					Node newNode;
					newNode.type = ENode::ENode_Number;
					newNode.number = atoi(numberBuffer);				
					newNode.bounds = IntAABB2D(boundsMin, boundsMax);

					Log("Found Number %d at [%d, %d]. Bounds: [%d, %d] -> [%d, %d]", newNode.number, 
					newNode.bounds.GetCenter().x, newNode.bounds.GetCenter().y,
					newNode.bounds.GetMin().x, newNode.bounds.GetMin().y,
					newNode.bounds.GetMax().x, newNode.bounds.GetMax().y);
					
					m_NumberNodes.push_back(newNode);
				}
				else if (*currentChar != '.')
				{
					Node newNode;
					newNode.type = ENode::ENode_Symbol;
					newNode.symbol = *currentChar;
					newNode.bounds = IntAABB2D(IntVec2((int32_t)(currentChar - line.c_str()), yPos), 1);

					m_SymbolNodes.push_back(newNode);

					++currentChar;
				}
				else
				{
					++currentChar;
				}
			}

			++yPos;
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint32_t sum = 0;
		bool validNode = true;

		for (const Node& symNode : m_SymbolNodes)
		{
			validNode = false;
			for (const Node& numNode : m_NumberNodes)
			{
				if (numNode.bounds.Intersects(symNode.bounds))
				{
					sum += numNode.number;
				}
			}
		}

		Log("Total Sum = %u", sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		uint64_t gearRatio = 0;

		std::vector<Node> overlappingNodes;
		for (const Node& symNode : m_SymbolNodes)
		{
			if (symNode.symbol != '*')
			{
				continue;
			}

			overlappingNodes = std::move(Algorithm::find_all(m_NumberNodes.begin(), m_NumberNodes.end(), [&](const Node& LHS) 
			{ 
				return LHS.bounds.Intersects(symNode.bounds); 
			}));

			if (overlappingNodes.size() == 2)
			{
				Log("Found Gear at [%d, %d]. Ratios: [%d, %d]", symNode.bounds.GetCenter().x, symNode.bounds.GetCenter().y, overlappingNodes[0].number, overlappingNodes[1].number);
				gearRatio += overlappingNodes[0].number * overlappingNodes[1].number;
			}
		}

		Log("Total Gear Ratio = %u", gearRatio);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	std::vector<Node> m_NumberNodes;
	std::vector<Node> m_SymbolNodes;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 3;
	newParams.year = 2023;
	newParams.puzzleTitle = "Gear Ratios";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
