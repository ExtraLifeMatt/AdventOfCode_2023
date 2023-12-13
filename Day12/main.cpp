// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Math.h"
#include "ACUtils/Debug.h"
#include "ACUtils/Hash.h"
#include "ACUtils/StringUtil.h"
#include <stack>
#include <vector>
#include <unordered_set>
#include <inttypes.h>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params)
	{};
private:
	struct Spring
	{
		Spring(const std::string& raw)
		: springStr(),
		groups()
		{
			std::vector<std::string> tokens;
			StringUtil::SplitBy(raw, " ", tokens);
			assert(tokens.size() == 2);
			springStr = tokens[0];
			std::vector<std::string> digits;
			StringUtil::SplitBy(tokens[1], ",", digits);
			for (const std::string& digit : digits)
			{
				groups.push_back(atoi(digit.c_str()));
			}
		}

		Spring(const std::string& raw, const std::vector<uint32_t>& counts)
		: springStr(raw),
		groups(counts)
		{
		}

		inline size_t GetLength() const { return springStr.size(); }
		inline size_t GetGroupLength(uint32_t index) const { return groups[index]; }

		bool CheckGroupAgainstIndex(uint32_t index, uint32_t groupIndex) const
		{
			uint32_t count = groups[groupIndex] - 1;
			for (; count; --count)
			{
				// End goes past our string, invalid.
				if (index + count >= springStr.size())
				{
					return false;
				}

				// We run into a '.'. Invalid.
				if (springStr[index + count] == '.')
				{
					return false;
				}
			}

			// Make sure our ending character IS NOT a '#', otherwise we're good.
			return (springStr[index + groups[groupIndex]] != '#');
		}

		std::string springStr;
		std::vector<uint32_t> groups;
	};

	uint64_t MakeHash(uint32_t strIndex, uint32_t groupIndex) const
	{
		return (uint64_t)groupIndex << 32ULL | strIndex;
	}

	uint64_t Solve(const Spring& spring, uint32_t strIndex, uint32_t groupIndex, std::unordered_map<uint64_t, uint64_t>& cache) const
	{
		std::unordered_map<uint64_t, uint64_t>::const_iterator itCache = cache.find(MakeHash(strIndex, groupIndex));
		if (itCache != cache.end())
		{
			return itCache->second;
		}
		uint64_t cacheValue = InternalSolve(spring, strIndex, groupIndex, cache);
		if (cache.find(MakeHash(strIndex, groupIndex)) == cache.end())
		{
			cache.insert(std::make_pair(MakeHash(strIndex, groupIndex), cacheValue));
		}
		return cacheValue;
	}

	uint64_t InternalSolve(const Spring& spring, uint32_t strIndex, uint32_t groupIndex, std::unordered_map<uint64_t, uint64_t>& cache) const
	{
		// Success?
		// Reached the end and nothing left for us to do.
		if (strIndex == spring.springStr.size() || strIndex == (spring.springStr.size() + 1))
		{
			return groupIndex == spring.groups.size() ? 1 : 0;
		}

		// Useless ., just skip past it.
		while (strIndex < (uint32_t)spring.springStr.size() && spring.springStr[strIndex] == '.')
		{
			// Just Increment
			strIndex++;
		}

		// Too big an index? Bounce.
		if (strIndex > spring.springStr.size())
		{
			return 0;
		}

		// Finished our groups but the remaining entries are valid.
		if (groupIndex == spring.groups.size())
		{
			bool success = true;
			while (strIndex < spring.springStr.size())
			{
				if (spring.springStr[strIndex] == '#')
				{
					success = false;
					break;
				}
				++strIndex;
			}

			return success ? 1 : 0;
		}

		// Too many groups? Skip
		if (groupIndex >= spring.groups.size())
		{
			return 0;
		}

		// Process choices
		uint64_t cacheValue = 0;	
		if (spring.springStr[strIndex] == '#')
		{
			if (spring.CheckGroupAgainstIndex(strIndex, groupIndex))
			{
				// Success, increment by group length
				cacheValue = Solve(spring, strIndex + (uint32_t)spring.GetGroupLength(groupIndex) + 1, groupIndex + 1, cache);
			}
			else
			{
				// Group failed, bounce out.
				return 0;
			}
		}
		else if (spring.springStr[strIndex] == '?')
		{
			// Does turning this into a # cause the group to finish?
			if (spring.CheckGroupAgainstIndex(strIndex, groupIndex))
			{
				// Solve as both a '#' and a '.'
				cacheValue = Solve(spring, strIndex + (uint32_t)spring.GetGroupLength(groupIndex) + 1, groupIndex + 1, cache) + 
				Solve(spring, strIndex + 1, groupIndex, cache);
			}
			else
			{
				// Can only be a '.'
				cacheValue = Solve(spring, strIndex + 1, groupIndex, cache);
			}
		}

		// Return value
		return cacheValue;
	}

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			m_Springs.emplace_back(line);

			// Build our Large version
			std::string largeSpring = m_Springs.back().springStr;
			std::vector<uint32_t> counts = m_Springs.back().groups;

			for (uint32_t i = 0; i < 4; ++i)
			{
				largeSpring += ("?" + m_Springs.back().springStr);
				counts.insert(counts.end(), m_Springs.back().groups.begin(), m_Springs.back().groups.end());
			}

			m_LargeSprings.emplace_back(largeSpring, counts);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::unordered_map<uint64_t, uint64_t> cache;
		uint64_t totalValues = 0;
		uint64_t variations = 0;

		for (const Spring& spring : m_Springs)
		{
			cache.clear();
			variations = 0;
			variations = Solve(spring, 0, 0, cache);
			Log("---Solved Spring %s [%" PRIu64"]", spring.springStr.c_str(), variations);
			totalValues += variations;
		}

		Log("Total Perms: %u", totalValues);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		std::unordered_map<uint64_t, uint64_t> cache;
		uint64_t totalValues = 0;
		uint64_t variations = 0;
		for (const Spring& spring : m_LargeSprings)
		{
			cache.clear();
			variations = 0;
			variations = Solve(spring, 0, 0, cache);
			Log("---Solved Spring %s [%" PRIu64"]", spring.springStr.c_str(), variations);
			totalValues += variations;

		}

		Log("Total Perms: %" PRIu64, totalValues);
		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<Spring> m_Springs;
	std::vector<Spring> m_LargeSprings;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 12;
	newParams.year = 2023;
	newParams.puzzleTitle = "Hot Springs";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
