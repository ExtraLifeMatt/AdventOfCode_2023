// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/StringUtil.h"
#include <algorithm>
#include <vector>
#include <cctype>
#include <unordered_map>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params), m_partTwoTotalIterations(0), m_partTwoWorkItemsCompleted(0) {};

private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		const char* dataHeaders[] = 
		{
			"seeds:",
			"seed-to-soil map:",
			"soil-to-fertilizer map:",
			"fertilizer-to-water map:",
			"water-to-light map:",
			"light-to-temperature map:",
			"temperature-to-humidity map:",
			"humidity-to-location map:"
		};

		std::vector<Range>* dataOutput[] = 
		{
			nullptr,
			&m_SeedToSoil,
			&m_SoilToFertilizer,
			&m_FertilizerToWater,
			&m_WaterToLight,
			&m_LightToTemp,
			&m_TempToHumidity,
			&m_HumidityToLoc
		};

		uint32_t expectedHeader = 0;
		uint64_t source = 0;
		uint64_t dest = 0;
		uint64_t count = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (line.size() == 0)
			{
				continue;
			}

			if (strncmp(line.c_str(), dataHeaders[expectedHeader], strlen(dataHeaders[expectedHeader])) == 0)
			{
				if (line.size() != strlen(dataHeaders[expectedHeader]))
				{
					line = line.substr(7); // Cut off the "seeds: "
				}
				else
				{
					line = fileReader.ReadLine(); // Just go to the next line.
				}

				while (line.size() != 0)
				{
					StringUtil::SplitBy(line, " ", tokens);
					if (expectedHeader == 0)
					{
						for (const std::string& tok : tokens)
						{
							m_Seeds.emplace_back(StringUtil::AtoiU64(tok.c_str()));
						}
					}
					else
					{
						assert(tokens.size() == 3);
						dest = StringUtil::AtoiU64(tokens[0].c_str());
						source = StringUtil::AtoiU64(tokens[1].c_str());
						count = StringUtil::AtoiU64(tokens[2].c_str());

						dataOutput[expectedHeader]->emplace_back(source, dest, count);
					}		

					line = fileReader.ReadLine();
				}

				++expectedHeader;
			}
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		uint64_t lowestLocation = ~0ULL;
		typedef uint64_t (AdventDay::*RangeFunc)(uint64_t value) const;

		RangeFunc allFuncs[] = { &AdventDay::GetSoil, &AdventDay::GetFertilizer, &AdventDay::GetWater, &AdventDay::GetLight, &AdventDay::GetTemp, &AdventDay::GetHumidity, &AdventDay::GetLocation };

		// Part One
		uint64_t resolvedValue = 0;
		uint64_t tempForLogging = 0;
		for (uint64_t seed : m_Seeds)
		{
			Log("Resolving Seed %u:", seed);
			resolvedValue = seed;
			for (RangeFunc func : allFuncs)
			{
				tempForLogging = (this->*func)(resolvedValue);

				Log("\t%u -> %u", resolvedValue, tempForLogging);

				resolvedValue = tempForLogging;
			}

			if (resolvedValue < lowestLocation)
			{
				lowestLocation = resolvedValue;
			}
		}

		Log("Lowest Location = %u", lowestLocation);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		if (m_partTwoTotalIterations == 0)
		{
			uint64_t totalSeedsToEval = 0;
			for (size_t i = 0; i < m_Seeds.size(); ++i)
			{
				if (i % 2 == 0)
				{
					continue;
				}

				totalSeedsToEval += m_Seeds[i];
			}

			m_partTwoTotalIterations = totalSeedsToEval -1;
			Log("Total Seeds To Eval: %u", totalSeedsToEval);

			m_partTwoSourceIndex = 0;
			m_partTwoSourceOffset = 0;
			m_partTwoLowestLocation = ~0ULL;
		}

		uint64_t totalPerIteration = std::max(m_partTwoTotalIterations / 1000ULL, 1ULL);

		uint64_t numToCompleteThisFrame = std::min(totalPerIteration, m_partTwoTotalIterations - m_partTwoWorkItemsCompleted);
		
		Log("Starting Iterations %u -> %u", m_partTwoWorkItemsCompleted, m_partTwoWorkItemsCompleted + numToCompleteThisFrame);
		uint64_t resolvedValue = ~0ULL;

		const std::vector<Range>* AllRanges[] = { &m_SeedToSoil, &m_SoilToFertilizer, &m_FertilizerToWater, &m_WaterToLight, &m_LightToTemp, &m_TempToHumidity, &m_HumidityToLoc };

		for(uint64_t i = 0; i < numToCompleteThisFrame; ++i)
		{
			// Pick up where we left off.
			if (m_partTwoSourceOffset == m_Seeds[m_partTwoSourceIndex + 1] - 1)
			{
				m_partTwoSourceIndex += 2;
				m_partTwoSourceOffset = 0;

				if (m_partTwoSourceIndex >= m_Seeds.size())
				{
					break;
				}
			}

			resolvedValue = ResolveValue(m_Seeds[m_partTwoSourceIndex] + m_partTwoSourceOffset, AllRanges, 0, 7);
			if (resolvedValue < m_partTwoLowestLocation)
			{
				m_partTwoLowestLocation = resolvedValue;
			}
			++m_partTwoSourceOffset;
		}

		m_partTwoWorkItemsCompleted += numToCompleteThisFrame;

		Log("Total Completed: %3.4f.", (double)m_partTwoWorkItemsCompleted / (double)m_partTwoTotalIterations * 100.0);


		if (m_partTwoWorkItemsCompleted == m_partTwoTotalIterations)
		{
			Log("Lowest Location = %u", m_partTwoLowestLocation);

			// Done.
			AdventGUIInstance::PartTwo(context);
		}

	}

	struct Range
	{
		Range(uint64_t _start, uint64_t _dest, uint64_t _count) : start(_start), dest(_dest), count(_count) {};
		
		uint64_t start;
		uint64_t dest;
		uint64_t count;
	};

	uint64_t GetRangeValueOrDefault(uint64_t value, const std::vector<Range>& ranges) const
	{
		for (const Range& range : ranges)
		{
			if (value >= range.start && value < range.start + range.count)
			{
				return range.dest + (value - range.start);
			}
		}

		return value;
	}

	void ResolveValueRange(Range value, const std::vector<Range>* ranges[7], uint32_t depth, uint32_t maxDepth, std::vector<Range>& outRanges)
	{
		if (depth == maxDepth)
		{
			outRanges.push_back(value);
			return;
		}

		const std::vector<Range>& nextRanges =*(ranges[depth]);
		for (const Range& range : nextRanges)
		{
			if (value.start > (range.start + range.count) || value.start + value.count < range.start)
			{
				continue;
			}

			value.start = range.dest + (value.start - range.start);
			value.count = std::min(value.count, range.count);

			ResolveValueRange(value, ranges, depth + 1, 7, outRanges);
		}
	}

	uint64_t ResolveValue(uint64_t value, const std::vector<Range>* ranges[7], uint32_t depth, uint32_t maxDepth)
	{
		if (depth == maxDepth)
		{
			return value;
		}

		uint64_t nextValue = GetRangeValueOrDefault(value, *(ranges[depth]));
		uint64_t resolvedValue = ResolveValue(nextValue, ranges, depth + 1, maxDepth);
		return resolvedValue;
	}

	uint64_t GetSoil(uint64_t seed) const	{ return GetRangeValueOrDefault(seed, m_SeedToSoil); }
	uint64_t GetFertilizer(uint64_t soil) const { return GetRangeValueOrDefault(soil, m_SoilToFertilizer); }
	uint64_t GetWater(uint64_t fertilizer) const { return GetRangeValueOrDefault(fertilizer, m_FertilizerToWater); }
	uint64_t GetLight(uint64_t water) const { return GetRangeValueOrDefault(water, m_WaterToLight); }
	uint64_t GetTemp(uint64_t light) const { return GetRangeValueOrDefault(light, m_LightToTemp); }
	uint64_t GetHumidity(uint64_t temp) const { return GetRangeValueOrDefault(temp, m_TempToHumidity); }
	uint64_t GetLocation(uint64_t humidity) const { return GetRangeValueOrDefault(humidity, m_HumidityToLoc); }

	std::vector<uint64_t> m_Seeds;
	std::vector<std::pair<uint64_t, uint64_t>> m_SeedRange;
	std::vector<Range> m_SeedToSoil;
	std::vector<Range> m_SoilToFertilizer;
	std::vector<Range> m_FertilizerToWater;
	std::vector<Range> m_WaterToLight;
	std::vector<Range> m_LightToTemp;
	std::vector<Range> m_TempToHumidity;
	std::vector<Range> m_HumidityToLoc;

	uint64_t m_partTwoTotalIterations;
	uint64_t m_partTwoWorkItemsCompleted;
	uint64_t m_partTwoSourceIndex;
	uint64_t m_partTwoSourceOffset;
	uint64_t m_partTwoLowestLocation;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 5;
	newParams.year = 2023;
	newParams.puzzleTitle = "If You Give A Seed A Fertilizer";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
