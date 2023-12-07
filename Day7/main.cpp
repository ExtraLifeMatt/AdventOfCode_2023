// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Algorithm.h"
#include "ACUtils/Bit.h"
#include "ACUtils/Math.h"
#include "ACUtils/StringUtil.h"
#include <algorithm>
#include <vector>
#include <cctype>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) : AdventGUIInstance(params) {};

private:
	enum class HandResult : uint8_t
	{
		HR_None = 0,
		HR_HighCard,
		HR_OnePair,
		HR_TwoPair,
		HR_ThreeOfAKind,
		HR_FullHouse,
		HR_FourOfAKind,
		HR_FiveOfAKind,

		HR_TotalResults
	};

	static constexpr const char* HandResultAsString[] = 
	{
		"None",
		"High Card",
		"One Pair",
		"Two Pair",
		"Three of a Kind",
		"Full House",
		"Four of a Kind",
		"Five of a Kind"
	};

	struct Hand
	{
		char cards[5];
		uint32_t cardBits;
		HandResult result;
		uint32_t bet;
	};

	static constexpr char s_validChars[] = { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };
	static uint8_t GetCharacterValue(char c)
	{
		if (!isdigit(c))
		{
			switch (c)
			{
				case 'T': return 8;
				case 'J': return 9;
				case 'Q': return 10;
				case 'K': return 11;
				case 'A': return 12;
				default: assert(false);
			}
		}

		return c - '2';
	}
	HandResult EvaluateHand(const Hand& hand) const
	{
		HandResult outResult = HandResult::HR_None;
		uint8_t evalBuffer[ARRAY_SIZE(s_validChars)] = { 0 };
		uint32_t popCnt = Bits::PopCount32(hand.cardBits);

		// Few simple cases.
		
		switch (popCnt)
		{
			case 1:	{ outResult = HandResult::HR_FiveOfAKind; } break;
			case 2:
			case 3:
			{
				// Could be Four of a kind, three of a kind, full house, or two pairs
				for (char c : hand.cards)
				{
					++evalBuffer[GetCharacterValue(c)];
				}

				for (size_t i = 0; i < ARRAY_SIZE(evalBuffer); ++i)
				{
					if (evalBuffer[i] < 2)
					{
						continue;
					}

					if (evalBuffer[i] == 4)
					{
						outResult = HandResult::HR_FourOfAKind;
						break;
					}
					else if (evalBuffer[i] == 3)
					{
						outResult = HandResult::HR_ThreeOfAKind;
						for (size_t j = i + 1; j < ARRAY_SIZE(evalBuffer); ++j)
						{
							if (evalBuffer[j] == 2)
							{
								outResult = HandResult::HR_FullHouse;
								break;
							}
						}
						break;

					}
					else if (evalBuffer[i] == 2)
					{
						outResult = HandResult::HR_OnePair;
						for (size_t j = i + 1; j < ARRAY_SIZE(evalBuffer); ++j)
						{
							if (evalBuffer[j] == 2)
							{
								outResult = HandResult::HR_TwoPair;
								break;
							}
							else if (evalBuffer[j] == 3)
							{
								outResult = HandResult::HR_FullHouse;
								break;
							}
						}
					}

					break;
				}
			}
			break;
			case 4: { outResult = HandResult::HR_OnePair; } break;
			case 5: { outResult = HandResult::HR_HighCard;} break;
			default: assert(false);
		}

		return outResult;
	}

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, " ", tokens);
			assert(tokens.size() == 2);

			Hand newHand;
			assert(tokens[0].size() == 5);
			memcpy((void*)newHand.cards, tokens[0].c_str(), tokens[0].size());
			newHand.bet = atoi(tokens[1].c_str());
			newHand.cardBits = 0;
			for (char c : newHand.cards)
			{
				newHand.cardBits |= 1 << GetCharacterValue(c);
			}
			newHand.result = EvaluateHand(newHand);
			m_Hands.push_back(newHand);
		}
	}

	struct CompareHandPred  
	{
		CompareHandPred(bool withJokers = false) : m_WithJokers(withJokers) {}

		bool operator()(const Hand& LHS, const Hand& RHS) const
		{
			if (LHS.result == RHS.result)
			{
				uint8_t LHSCharValue = 0;
				uint8_t RHSCharValue = 0;
				for (size_t i = 0; i < ARRAY_SIZE(LHS.cards); ++i)
				{
					LHSCharValue = GetCharacterValue(LHS.cards[i]);
					RHSCharValue = GetCharacterValue(RHS.cards[i]);
					
					if (LHSCharValue == RHSCharValue)
					{
						continue;
					}

					if (m_WithJokers)
					{
						if (LHSCharValue == 9)
						{
							return true;
						}
						
						if (RHSCharValue == 9)
						{
							return false;
						}
					}

					if (LHSCharValue != RHSCharValue)
					{
						return LHSCharValue < RHSCharValue;
					}
				}
			}

			return LHS.result < RHS.result;
		}
		bool m_WithJokers;
	};

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		std::sort(m_Hands.begin(), m_Hands.end(), CompareHandPred());

		uint64_t sum = 0;
		char cardBuffer[6] = {0};
		for (size_t i = 0; i < m_Hands.size(); ++i)
		{
			memcpy(cardBuffer, m_Hands[i].cards, 5);
			Log("Rank %zd: Card %s resolved to result %s with a bet of %u.", i + 1, cardBuffer, HandResultAsString[static_cast<uint32_t>(m_Hands[i].result)], m_Hands[i].bet);
			sum += (m_Hands[i].bet * (i + 1));
		}

		Log("Total Sum = %u", sum);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	Hand GetOptimalHand(const Hand& originalHand)
	{
		Hand outHand = originalHand;
		if (outHand.result == HandResult::HR_FiveOfAKind || ( outHand.cardBits & (1 << 9) ) == 0)
		{
			// No Jack / Joker, or we can't get any better.
			return outHand;
		}

		uint16_t uniqueCardsSansJokers = Bits::PopCount32(outHand.cardBits & ~(1 << 9));
		if (uniqueCardsSansJokers == 1)
		{
			outHand.result = HandResult::HR_FiveOfAKind;
		}
		else
		{
			uint8_t numCards[ARRAY_SIZE(s_validChars)] = { 0 };
			uint8_t maxInstanceOfCards = 0;
			uint8_t numJokers = 0;

			for (char c : outHand.cards)
			{
				if (c == 'J')
				{
					++numJokers;
				}
				else
				{
					numCards[GetCharacterValue(c)]++;
				}
			}

			for (uint8_t instanceCount : numCards)
			{
				maxInstanceOfCards = std::max(maxInstanceOfCards, instanceCount);
			}

			if (uniqueCardsSansJokers == 2)
			{
				if (originalHand.result == HandResult::HR_TwoPair)
				{
					outHand.result = static_cast<HandResult>(static_cast<uint8_t>(HandResult::HR_ThreeOfAKind) + numJokers);
				}
				else if (originalHand.result == HandResult::HR_ThreeOfAKind)
				{
					outHand.result = HandResult::HR_FourOfAKind;
				}
				else
				{
					assert(false);
				}
			}
			else if (uniqueCardsSansJokers == 3)
			{
				outHand.result = (maxInstanceOfCards == 2 || numJokers == 2) ? HandResult::HR_ThreeOfAKind : HandResult::HR_TwoPair;
			}
			else if (uniqueCardsSansJokers == 4)
			{
				assert(originalHand.result == HandResult::HR_HighCard);
				outHand.result = HandResult::HR_OnePair;
			}
			else
			{
				assert(false);
			}
		}

		char bufferA[6] = { 0 };
		memcpy((void*)bufferA, originalHand.cards, 5);
		Log("Changed Hand %s with result %s to result %s", bufferA, HandResultAsString[static_cast<uint8_t>(originalHand.result)], HandResultAsString[static_cast<uint8_t>(outHand.result)]);
		return outHand;
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		std::vector<Hand> optimalHand;
		optimalHand.reserve(m_Hands.size());

		for (const Hand& hand : m_Hands)
		{
			optimalHand.push_back(GetOptimalHand(hand));
		}

		std::sort(optimalHand.begin(), optimalHand.end(), CompareHandPred(true));

		uint64_t sum = 0;
		char cardBuffer[6] = { 0 };
		for (size_t i = 0; i < optimalHand.size(); ++i)
		{
			memcpy(cardBuffer, optimalHand[i].cards, 5);
			Log("Rank %zd: Card %s resolved to result %s with a bet of %u.", i + 1, cardBuffer, HandResultAsString[static_cast<uint32_t>(optimalHand[i].result)], optimalHand[i].bet);
			sum += (optimalHand[i].bet * (i + 1));
		}

		Log("Total Optimal Sum = %u", sum);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<Hand> m_Hands;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 7;
	newParams.year = 2023;
	newParams.puzzleTitle = "Camel Cards";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
