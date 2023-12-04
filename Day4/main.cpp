// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"
#include "imgui.h"

#include "ACUtils/Bit.h"
#include "ACUtils/StringUtil.h"
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
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			size_t colonIdx = line.find(':');
			assert(colonIdx != std::string::npos);
			line = line.substr(colonIdx + 1);
			StringUtil::SplitBy(line, " ", tokens);
			Bitfield128 bitfield;
			for (std::string& token : tokens)
			{
				if (token.size() == 0)
				{
					continue;
				}

				if (token[0] == '|')
				{
					m_Cards.push_back(bitfield);
					bitfield = Bitfield128();
				}
				else
				{
					assert(isdigit(token[0]));
					bitfield.SetBit((uint32_t)atoi(token.c_str()));
				}
			}
			m_WinningNumbers.push_back(bitfield);
			m_Copies.push_back(1);
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint64_t totalScore = 0;
		Bitfield128 winners;
		assert(m_Cards.size() == m_WinningNumbers.size());
		for (size_t i = 0; i < m_Cards.size(); ++i)
		{
			winners = m_Cards[i] & m_WinningNumbers[i];
			uint32_t winnerCount = winners.PopCount();
			
			if (winnerCount == 0)
			{
				Log("Card %zd was NOT a winner. No points.", i);
				continue;
			}

			Log("Card %zd was a winner with %u matches for a total of %u points.", i, winnerCount, 1U << (winnerCount - 1));
			totalScore += 1ULL << (winnerCount - 1);
		}

		Log("Total Score: %u", totalScore);
		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		uint64_t totalCopies = 0;
		Bitfield128	winners;
		for (size_t i = 0; i < m_Copies.size(); ++i)
		{
			totalCopies += m_Copies[i];

			winners = m_Cards[i] & m_WinningNumbers[i];
			uint32_t winnerCount = winners.PopCount();
			
			if (winnerCount != 0)
			{
				Log("Card %zd had %u copies and generated %u dupes.", i, m_Copies[i], winnerCount);

				size_t dupeStart = i + 1;
				size_t dupeEnd = std::min(dupeStart + winnerCount, m_Copies.size());
				while (dupeStart != dupeEnd)
				{
					m_Copies[dupeStart] += 1 * m_Copies[i];
					++dupeStart;
				}
			}
			else
			{
				Log("Card %zd had %u copies but no winners. Added to total.", i, m_Copies[i]);
			}
		}

		Log("Total Cards: %u", totalCopies);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	struct Bitfield128
	{
		Bitfield128(): bits{0ULL} {};
		Bitfield128(uint64_t low, uint64_t high): bits{low, high} {};

		uint64_t bits[2];

		uint64_t GetLow() const { return bits[0]; }
		uint64_t GetHigh() const { return bits[1]; }
		uint32_t PopCount() { return Bits::PopCount64(bits[0]) + Bits::PopCount64(bits[1]); }
			
		void SetBit(uint32_t index)
		{
			assert(index >= 0 && index <= 127);
			bits[index / 64] |= 1ULL << (index % 64);
		}

		Bitfield128 operator|(const Bitfield128& RHS) const
		{
			return Bitfield128(bits[0] | RHS.bits[0], bits[1] | RHS.bits[1]);
		}

		Bitfield128 operator&(const Bitfield128& RHS) const
		{
			return Bitfield128(bits[0] & RHS.bits[0], bits[1] & RHS.bits[1]);
		}
	};

	std::vector<Bitfield128> m_Cards;
	std::vector<Bitfield128> m_WinningNumbers;
	std::vector<uint32_t> m_Copies;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 4;
	newParams.year = 2023;
	newParams.puzzleTitle = "Scratchcards";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
