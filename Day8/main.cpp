// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Hash.h"
#include "ACUtils/Math.h"
#include "ACUtils/StringUtil.h"
#include <algorithm>
#include <vector>
#include <unordered_map>


struct DesertNode
{
	DesertNode() : m_RawName(), m_Hash(0), m_Destinations{}, m_DestinationHashes{ 0, 0 } {}
	DesertNode(const std::string& _rawName, const std::string& _leftName, const std::string& _rightName)
		: m_RawName(_rawName),
		m_Hash(Hash::HashString32(_rawName.c_str())),
		m_Destinations{ _leftName, _rightName },
		m_DestinationHashes{ Hash::HashString32(_leftName.c_str()), Hash::HashString32(_rightName.c_str()) }
	{
	}

	bool operator==(const DesertNode& RHS) const
	{
		return m_Hash == RHS.m_Hash && 
		m_DestinationHashes[0] == RHS.m_DestinationHashes[0] &&
		m_DestinationHashes[1] == RHS.m_DestinationHashes[1];
	}
	bool operator!=(const DesertNode& RHS) const
	{
		return m_Hash != RHS.m_Hash ||
			m_DestinationHashes[0] != RHS.m_DestinationHashes[0] ||
			m_DestinationHashes[1] != RHS.m_DestinationHashes[1];
	}

	std::string m_RawName;
	uint32_t m_Hash;
	std::string m_Destinations[2];
	uint32_t m_DestinationHashes[2];
};

namespace std 
{
	template<>struct hash<DesertNode>
	{
		std::size_t operator()(DesertNode const& val) const
		{
			return val.m_Hash;
		}
	}; 
}

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:


	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		assert(!fileReader.IsEOF());
		m_Steps = fileReader.ReadLine();
		assert(m_Steps.size() != 0);
		
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (line.size() == 0)
			{
				continue;
			}

			StringUtil::SplitBy(line, " ", tokens);
			assert(tokens.size() == 4);
			DesertNode newNode(tokens[0], tokens[2].substr(1, 3), tokens[3].substr(0, 3));
			m_Nodes.emplace(std::make_pair(newNode.m_Hash, newNode));

			if (newNode.m_RawName[2] == 'A')
			{
				m_StartingNodes.push_back(newNode.m_Hash);
			}
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint64_t totalSteps = 0;
		uint32_t nextIndex = 0;
		uint32_t currentHash = m_StartHash;
		std::unordered_map<uint32_t, DesertNode>::const_iterator itFind = m_Nodes.end();
		while (currentHash != m_ExitHash)
		{
			itFind = m_Nodes.find(currentHash);
			assert(itFind != m_Nodes.end());
			nextIndex = m_Steps[totalSteps % m_Steps.size()] == 'L' ? 0 : 1;
			currentHash = itFind->second.m_DestinationHashes[nextIndex];
			++totalSteps;
		}

		Log("Route took %u steps.", totalSteps);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::vector<uint32_t> successTotalHistory;
		uint64_t totalSteps = 0;
		uint32_t iterationSteps =0;
		uint32_t nextIndex = 0;
		std::unordered_map<uint32_t, DesertNode>::const_iterator itFind = m_Nodes.end();
		for (uint32_t startNode : m_StartingNodes)
		{
			totalSteps = 0;
			uint32_t currentNode = startNode;
			while (true)
			{
				itFind = m_Nodes.find(currentNode);
				assert(itFind != m_Nodes.end());
				nextIndex = m_Steps[totalSteps % m_Steps.size()] == 'L' ? 0 : 1;
				if (itFind->second.m_RawName[2] == 'Z')
				{
					Log("Node %u success after %u steps", startNode, totalSteps);
					successTotalHistory.push_back((uint32_t)totalSteps);
					break;
				}
				currentNode = itFind->second.m_DestinationHashes[nextIndex];
				++totalSteps;
			}

		}

		std::vector<uint32_t> primeFactors;
		std::vector<uint32_t> conjoinedFactors;
		uint64_t LCM = 1;

		assert(successTotalHistory.size() < 16);
		for (uint32_t success : successTotalHistory)
		{
			Math::PrimeFactorization32(success, primeFactors);
			Log("Prime Factors for %u:", success);

			for (uint32_t prime : primeFactors)
			{
				Log("\t %u", prime);
				if (std::find(conjoinedFactors.begin(), conjoinedFactors.end(), prime) == conjoinedFactors.end())
				{
					conjoinedFactors.push_back(prime);
					LCM *= prime;
				}
			}
		}

		Log("Total Steps = %llu", LCM);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	std::string m_Steps;
	std::unordered_map<uint32_t, DesertNode> m_Nodes;
	std::vector<uint32_t> m_StartingNodes;
	static constexpr uint32_t m_StartHash = Hash::HashString32("AAA");
	static constexpr uint32_t m_ExitHash = Hash::HashString32("ZZZ");
	
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 8;
	newParams.year = 2023;
	newParams.puzzleTitle = "Haunted Wasteland";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
