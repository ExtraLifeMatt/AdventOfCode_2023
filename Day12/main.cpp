// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Bit.h"
#include "ACUtils/Debug.h"
#include "ACUtils/Hash.h"
#include "ACUtils/StringUtil.h"
#include <stack>
#include <vector>
#include <unordered_set>
#include <xmmintrin.h>

class Bitfield128
{
public:
	Bitfield128(): bitfield{0ULL, 0ULL}{}
	Bitfield128(uint64_t low, uint64_t high)
	: bitfield{low, high}
	{
	}

	bool IsZero() const { return bitfield[0] == 0ULL && bitfield[1] == 0ULL; }

	uint32_t PopCount() const
	{
		return Bits::PopCount64(bitfield[0]) + Bits::PopCount64(bitfield[1]);
	}

	uint32_t ExtractLSB() const
	{
		if (bitfield[0])
		{
			return Bits::CountTrailingZeros64(bitfield[0]);
		}
		
		if (bitfield[1])
		{
			return 64 + Bits::CountTrailingZeros64(bitfield[1]);
		}

		return 128;
	}

	uint32_t ExtractMSB() const
	{
		if (bitfield[1])
		{
			return 64 + Bits::CountLeadingZeros64(bitfield[1]);
		}

		if (bitfield[0])
		{
			return Bits::CountLeadingZeros64(bitfield[0]);
		}

		return 128;
	}

	void SetBit(uint32_t index)
	{
		bitfield[index / 64] |= 1ULL << (index % 64);
	}

	void ClearBit(uint32_t index)
	{
		bitfield[index / 64] &= ~(1ULL << (index % 64));
	}

	bool IsBitSet(uint32_t index) const
	{
		return (bitfield[index / 64] & (1ULL << (index % 64))) != 0;
	}

	static Bitfield128 BuildMask(uint32_t startIndex, uint32_t count)
	{
		uint64_t lowMask = 0;
		uint64_t highMask = 0;

		if (startIndex < 64)
		{
			if (startIndex + count > 64)
			{
				// Straddles barrier
				lowMask = Bits::CreateBitMask64(startIndex, 64 - startIndex);
				highMask = Bits::CreateBitMask64(0, count - (64 - startIndex));
			}
			else
			{
				lowMask = Bits::CreateBitMask64(startIndex, count);
			}
		}
		else
		{
			highMask = Bits::CreateBitMask64(startIndex % 64, count);
		}

		return Bitfield128(lowMask, highMask);
	}

	static void ExtractContiguousBitsLSB(const Bitfield128& bitfield, uint32_t& outIndex, uint32_t& outCount)
	{
		uint32_t bitRange = 128;
		if (bitfield.GetLow())
		{
			Bits::GetContiguousBitsLSB64(bitfield.GetLow(), outIndex, outCount);

			if (outIndex + outCount == 64)
			{
				uint32_t highIndex = 0;
				uint32_t highNumBits = 0;

				Bits::GetContiguousBitsLSB64(bitfield.GetHigh(), highIndex, highNumBits );
				if (highIndex == 0)
				{
					outCount += highNumBits;
				}
			}
		}
		else if (bitfield.GetHigh())
		{
			Bits::GetContiguousBitsLSB64(bitfield.GetHigh(), outIndex, outCount);
			outIndex += 64;
		}
	}

	bool operator==(const Bitfield128& RHS) const
	{
		return GetLow() == RHS.GetLow() && GetHigh() == RHS.GetHigh();
	}

	bool operator!=(const Bitfield128& RHS) const
	{
		return GetLow() != RHS.GetLow() || GetHigh() != RHS.GetHigh();
	}

	Bitfield128& operator<<=(size_t pos)
	{
		uint64_t overflowMask = bitfield[0] >> (64 - pos);
		bitfield[0] <<= pos;
		bitfield[1] <<= pos;
		bitfield[1] |= overflowMask;

		return *this;
	}

	Bitfield128 operator<<(size_t pos) const
	{
		uint64_t overflowMask = bitfield[0] >> (64 - pos);
		return Bitfield128(bitfield[0] << pos, (bitfield[1] << pos) | overflowMask);
	}

	Bitfield128& operator>>=(size_t pos)
	{
		uint64_t underflowMask = bitfield[1] & ((1 << pos) - 1);
		bitfield[1] >>= pos;
		bitfield[0] >>= pos;
		bitfield[0] |= (underflowMask << (64 - pos));

		return *this;
	}

	Bitfield128 operator>>(size_t pos) const
	{
		uint64_t underflowMask = bitfield[1] & ((1 << pos) - 1);
		return Bitfield128((bitfield[0] >> pos) | (underflowMask << (64 - pos)), bitfield[1] >> pos);
	}

	Bitfield128& operator|=(const Bitfield128& RHS)
	{
		bitfield[0] |= RHS.bitfield[0];
		bitfield[1] |= RHS.bitfield[1];
		return *this;
	}

	Bitfield128 operator|(const Bitfield128& RHS) const
	{
		return Bitfield128(bitfield[0] | RHS.bitfield[0], bitfield[1] | RHS.bitfield[1]);
	}

	Bitfield128& operator&=(const Bitfield128& RHS)
	{
		bitfield[0] &= RHS.bitfield[0];
		bitfield[1] &= RHS.bitfield[1];
		return *this;
	}

	Bitfield128 operator&(const Bitfield128& RHS) const
	{
		return Bitfield128(bitfield[0] & RHS.bitfield[0], bitfield[1] & RHS.bitfield[1]);
	}

	Bitfield128& operator^=(const Bitfield128& RHS)
	{
		bitfield[0] ^= RHS.bitfield[0];
		bitfield[1] ^= RHS.bitfield[1];
		return *this;
	}

	Bitfield128 operator^(const Bitfield128& RHS)
	{
		return Bitfield128(bitfield[0] ^ RHS.bitfield[0], bitfield[1] ^ RHS.bitfield[1]);
	}

	Bitfield128 operator~() const
	{
		return Bitfield128(~bitfield[0], ~bitfield[1]);
	}

	uint64_t GetLow() const { return bitfield[0]; }
	uint64_t GetHigh() const { return bitfield[1]; }
private:
	uint64_t bitfield[2];
};

inline size_t HashInts64(uint64_t value1, uint64_t value2) {
	uint32_t short_random1 = 842304669U;
	uint32_t short_random2 = 619063811U;
	uint32_t short_random3 = 937041849U;
	uint32_t short_random4 = 3309708029U;
	uint32_t value1a = static_cast<uint32_t>(value1 & 0xffffffff);
	uint32_t value1b = static_cast<uint32_t>((value1 >> 32) & 0xffffffff);
	uint32_t value2a = static_cast<uint32_t>(value2 & 0xffffffff);
	uint32_t value2b = static_cast<uint32_t>((value2 >> 32) & 0xffffffff);
	uint64_t product1 = static_cast<uint64_t>(value1a) * short_random1;
	uint64_t product2 = static_cast<uint64_t>(value1b) * short_random2;
	uint64_t product3 = static_cast<uint64_t>(value2a) * short_random3;
	uint64_t product4 = static_cast<uint64_t>(value2b) * short_random4;
	uint64_t hash64 = product1 + product2 + product3 + product4;
	if (sizeof(size_t) >= sizeof(uint64_t))
		return static_cast<size_t>(hash64);
	uint64_t odd_random = 1578233944LL << 32 | 194370989LL;
	uint32_t shift_random = 20591U << 16;
	hash64 = hash64 * odd_random + shift_random;
	size_t high_bits =
		static_cast<size_t>(hash64 >> (8 * (sizeof(uint64_t) - sizeof(size_t))));
	return high_bits;
}

namespace std 
{
	template<>struct hash<Bitfield128>
	{
		std::size_t operator()(Bitfield128 const& val) const
		{
			return HashInts64(val.GetHigh(), val.GetLow());
		}
	};
}

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params),
	m_totalArrangementsPartOne(0),
	m_totalArrangementsPartTwo(0),
	m_currentSpringIndex(~0ULL),
	m_cacheHits(0)
	{};
private:
	struct Spring
	{
		Spring(const std::string& raw)
		: rawSpring(),
		knownBrokenMask(0U),
		unknownMask(0U),
		limitMask(0U),
		counts(),
		totalBrokenSegments(0)
		{
			std::vector<std::string> tokens;
			StringUtil::SplitBy(raw, " ", tokens);
			assert(tokens.size() == 2);
			rawSpring = tokens[0];
			limitMask = (1 << rawSpring.size()) - 1;
			assert(rawSpring.size() * 5 < 128);
			for (size_t i = 0; i < rawSpring.size(); ++i)
			{
				if (rawSpring[i] == '#')
				{
					knownBrokenMask |= 1 << i;
				}
				else if (rawSpring[i] == '?')
				{
					unknownMask |= 1 << i;
				}
			}

			std::vector<std::string> digits;
			StringUtil::SplitBy(tokens[1], ",", digits);
			for (const std::string& digit : digits)
			{
				counts.push_back(atoi(digit.c_str()));
				totalBrokenSegments += counts.back();
			}

			// Create BIG versions
			bigRawString = rawSpring;
			std::string bigTokens = tokens[1];
			for (size_t i = 0; i < 4; ++i)
			{
				bigRawString += ("?" + rawSpring);
				bigTokens += ("," + tokens[1]);
			}

			totalBigBrokenSegments = 0;

			digits.clear();
			StringUtil::SplitBy(bigTokens, ",", digits);
			for (const std::string& digit : digits)
			{
				bigCounts.push_back(atoi(digit.c_str()));
				totalBigBrokenSegments += bigCounts.back();
			}

			bigLimitMask = Bitfield128::BuildMask(0,  (uint32_t)bigRawString.size());
			for (size_t i = 0; i < bigRawString.size(); ++i)
			{
				if (bigRawString[i] == '#')
				{
					bigKnownBrokenMask.SetBit((uint32_t)i);
				}
				else if (bigRawString[i] == '?')
				{
					bigUnknownMask.SetBit((uint32_t)i);
					assert(bigUnknownMask.IsBitSet((uint32_t)i));
				}
			}
		}

		bool IsValidToIndex(uint32_t mask, uint32_t bitIndex) const
		{
			uint32_t combinedMask = mask | knownBrokenMask;

			uint32_t index = 0;
			uint32_t count = 0;

			for (uint32_t i = 0; i < counts.size(); ++i)
			{
				Bits::GetContiguousBitsLSB(combinedMask, index, count);
				assert(rawSpring[index] != '.'); // Check against our raw string just for paranoia sake.
				if (counts[i] != count)
				{
						return false;
				}

				if (Bits::CountTrailingZeros(combinedMask) >= bitIndex)
				{
					return true;
				}

				combinedMask &= ~(Bits::CreateBitMask(index, count));
			}

			return true;
		}

		bool IsValidMask(uint32_t mask) const
		{
			uint32_t combinedMask = mask | knownBrokenMask;
			if (Bits::PopCount32(combinedMask) != totalBrokenSegments)
			{
				return false;
			}

			uint32_t index = 0;
			uint32_t count = 0;

			for (uint32_t i = 0; i < counts.size(); ++i)
			{
				Bits::GetContiguousBitsLSB(combinedMask, index, count);
				assert(rawSpring[index] != '.'); // Check against our raw string just for paranoia sake.
				if (counts[i] != count)
				{
					return false;
				}
				combinedMask &= ~(Bits::CreateBitMask(index, count));
			}

			assert(combinedMask == 0);
			return true;
		}

		bool IsValidMask(const Bitfield128& mask) const
		{
			Bitfield128 combinedMask = mask | bigKnownBrokenMask;
			if (combinedMask.PopCount() != totalBigBrokenSegments)
			{
				return false;
			}

			uint32_t index = 0;
			uint32_t count = 0;

			for (uint32_t i = 0; i < bigCounts.size(); ++i)
			{
				Bitfield128::ExtractContiguousBitsLSB(combinedMask, index, count);
				assert(bigRawString[index] != '.'); // Check against our raw string just for paranoia sake.
				if (bigCounts[i] != count)
				{
					return false;
				}
				combinedMask &= ~(Bitfield128::BuildMask(index, count));
			}

			assert(combinedMask.GetLow() == 0 && combinedMask.GetHigh() == 0);
			return true;
		}

		std::string rawSpring;
		uint32_t knownBrokenMask;
		uint32_t unknownMask;
		uint32_t limitMask;
		uint32_t validMask;
		std::vector<uint32_t> counts;
		uint32_t totalBrokenSegments;

		std::string bigRawString;
		Bitfield128 bigKnownBrokenMask;
		Bitfield128 bigUnknownMask;
		Bitfield128 bigLimitMask;
		std::vector<uint32_t> bigCounts;
		uint32_t totalBigBrokenSegments;
	};

	void CreateAllVariants(const Spring& spring)
	{
		uint32_t availableSpots = spring.unknownMask;
		
		// Simple DFS
		uint32_t candidate;
		uint32_t nextCandidate;
		uint32_t possibleBitPositions;
		uint32_t skippedFailures = 0;

		uint32_t totalProcessed = 0;

		while (!m_variantCandidates.empty())
		{
			candidate = m_variantCandidates.top();
			m_variantCandidates.pop();

			if (spring.IsValidMask(candidate))
			{	
				m_foundCandidates.insert(candidate | spring.knownBrokenMask);
			}
			else
			{
				m_failedCandidates.insert(candidate);
			}

			possibleBitPositions = ~candidate & availableSpots;
			while (possibleBitPositions)
			{
				uint32_t nextIdx = Bits::CountTrailingZeros(possibleBitPositions);
				nextCandidate = (candidate | (1 << nextIdx));
				
				if (m_failedCandidates.find(nextCandidate) == m_failedCandidates.end())
				{
					m_variantCandidates.push(nextCandidate);
				}
				else
				{
					++m_cacheHits;
				}

				possibleBitPositions &= ~(1 << nextIdx);
			}
			++totalProcessed;
			if (totalProcessed % 1000 == 0)
			{
				break;
			}
		}
	}

	void CreateAllBigVariants(const Spring& spring)
	{
		Bitfield128 availableSpots = spring.bigUnknownMask;

		// Toss on the first one
		Bitfield128 candidate;
		Bitfield128 nextCandidate;
		Bitfield128 possibleBitPositions;

		uint32_t totalProcessed = 0;

		while (!m_bigVariantCandidates.empty())
		{
			candidate = m_bigVariantCandidates.top();
			m_bigVariantCandidates.pop();

			if (spring.IsValidMask(candidate))
			{
				m_bigFoundCandidates.insert(candidate | spring.bigKnownBrokenMask);
			}
			else
			{
				m_bigFailedCandidates.insert(candidate);
			}

			possibleBitPositions = ~candidate & availableSpots;
			while (!possibleBitPositions.IsZero())
			{
				uint32_t nextIdx = possibleBitPositions.ExtractLSB();
				nextCandidate = (candidate | Bitfield128::BuildMask(nextIdx, 1));

				if (m_bigFailedCandidates.find(nextCandidate) == m_bigFailedCandidates.end())
				{
					m_bigVariantCandidates.push(nextCandidate);
				}
				else
				{
					++m_cacheHits;
				}
				possibleBitPositions.ClearBit(nextIdx);
			}
			++totalProcessed;
			if (totalProcessed % 10000 == 0)
			{
				break;
			}
		}
	}

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			m_Springs.emplace_back(line);
		}

		// Quick unit tests for Bitfield128
		Bitfield128 allShifts(1,0);
		for (size_t i = 1; i < 128; ++i)
		{
			allShifts <<= 1;
			assert(allShifts.ExtractLSB() == i);

			if (i < 64)
			{
				assert(allShifts.GetLow() & (1ULL << i));
			}
			else
			{
				assert(allShifts.GetHigh() & (1ULL << (i%64)));
			}
		}

		Bitfield128 simpleMask = Bitfield128::BuildMask(0, 16);
		assert(simpleMask.GetLow() == Bits::CreateBitMask64(0, 16));
		simpleMask = Bitfield128::BuildMask(64, 16);
		assert(simpleMask.GetHigh() == Bits::CreateBitMask64(0, 16));
		simpleMask = Bitfield128::BuildMask(56, 16); // Across boundary.
		assert(simpleMask.GetLow() == Bits::CreateBitMask64(56, 8) && simpleMask.GetHigh() == Bits::CreateBitMask64(0, 8));
		uint32_t extractIndex = 0;
		uint32_t extractCount = 0;
		Bitfield128::ExtractContiguousBitsLSB(simpleMask, extractIndex, extractCount);
		assert(extractIndex == 56 && extractCount == 16);
		simpleMask >>= 8;
		Bitfield128::ExtractContiguousBitsLSB(simpleMask, extractIndex, extractCount);
		assert(extractIndex == 48 && extractCount == 16);
		simpleMask <<= 8;
		Bitfield128::ExtractContiguousBitsLSB(simpleMask, extractIndex, extractCount);
		assert(extractIndex == 56 && extractCount == 16);

		//uint64_t test2 = 0b111010101110101011101010111010101110101;
		//assert(m_Springs[0].IsValidMask(Bitfield128(test2, 0)));
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		if (m_currentSpringIndex == ~0ULL)
		{
			m_currentSpringIndex = 0;
			m_cacheHits = 0;
			m_totalArrangementsPartOne = 0;
			m_variantCandidates.push(m_Springs[0].knownBrokenMask);
			Log("Now Processing Spring %zd of %zd [%s] (%3.2f%%)...", m_currentSpringIndex, m_Springs.size(), m_Springs[m_currentSpringIndex].rawSpring.c_str(), (double)m_currentSpringIndex / (double)m_Springs.size() * 100.0);
			m_variantStopWatch.Start();
		}

		// Part One
		while(m_currentSpringIndex < m_Springs.size())
		{
			CreateAllVariants(m_Springs[m_currentSpringIndex]);

			if (m_variantCandidates.empty())
			{
				Log("Found %zd variants for Spring [%s] in %f ms.", m_foundCandidates.size(), m_Springs[m_currentSpringIndex].rawSpring.c_str(), m_variantStopWatch.Stop());
				m_totalArrangementsPartOne += (uint32_t)m_foundCandidates.size();
				m_foundCandidates.clear();
				m_failedCandidates.clear();
				++m_currentSpringIndex;
				if (m_currentSpringIndex < m_Springs.size())
				{
					m_variantCandidates.push(m_Springs[m_currentSpringIndex].knownBrokenMask);
					Log("Now Processing Spring %zd of %zd [%s] (%3.2f%%)...", m_currentSpringIndex, m_Springs.size(), m_Springs[m_currentSpringIndex].rawSpring.c_str(), (double)m_currentSpringIndex / (double)m_Springs.size() * 100.0);
					m_variantStopWatch.Start();
				}

			}
			else
			{
				Log("...%zd solutions found, %zd invalid cases, cache hits %llu, current stack %zd, total processing time %f in  ms...", m_foundCandidates.size(), m_failedCandidates.size(), m_cacheHits, m_variantCandidates.size(), m_variantStopWatch.Peek());
				break;
			}
		}

		if (m_currentSpringIndex >= m_Springs.size())
		{
			m_currentSpringIndex = ~0ULL;
			Log("Total Arrangements = %zd", m_totalArrangementsPartOne);

			// Done.
			AdventGUIInstance::PartOne(context);
		}

	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{
		// Part Two
		if (m_currentSpringIndex == ~0ULL)
		{
			m_currentSpringIndex = 0;
			m_cacheHits = 0;
			m_totalArrangementsPartTwo = 0;
			m_bigVariantCandidates.push(m_Springs[0].bigKnownBrokenMask);
			Log("Now Processing BIG Spring %zd of %zd [%s] (%3.2f%%)...", m_currentSpringIndex, m_Springs.size(), m_Springs[m_currentSpringIndex].bigRawString.c_str(), (double)m_currentSpringIndex / (double)m_Springs.size() * 100.0);
			m_variantStopWatch.Start();
		}

		// Part Two
		while (m_currentSpringIndex < m_Springs.size())
		{
			CreateAllBigVariants(m_Springs[m_currentSpringIndex]);

			if (m_bigVariantCandidates.empty())
			{
				Log("Found %zd variants for Spring [%s] in %f ms.", m_bigFoundCandidates.size(), m_Springs[m_currentSpringIndex].bigRawString.c_str(), m_variantStopWatch.Stop());
				m_totalArrangementsPartTwo += m_bigFoundCandidates.size();
				m_bigFoundCandidates.clear();
				m_bigFailedCandidates.clear();
				++m_currentSpringIndex;
				if (m_currentSpringIndex < m_Springs.size())
				{
					m_bigVariantCandidates.push(m_Springs[m_currentSpringIndex].bigKnownBrokenMask);
					Log("Now Processing Spring %zd of %zd [%s] (%3.2f%%)...", m_currentSpringIndex, m_Springs.size(), m_Springs[m_currentSpringIndex].bigRawString.c_str(), (double)m_currentSpringIndex / (double)m_Springs.size() * 100.0);
					m_variantStopWatch.Start();
				}

			}
			else
			{
				Log("...%zd solutions found, %zd invalid cases, cache hits %llu, current stack %zd, total processing time %f in ms...", m_bigFoundCandidates.size(), m_bigFailedCandidates.size(), m_cacheHits, m_bigVariantCandidates.size(), m_variantStopWatch.Peek());
				break;
			}

		}

		if (m_currentSpringIndex >= m_Springs.size())
		{
			Log("Total BIG Arrangements = %zd", m_totalArrangementsPartOne);

			// Done.
			AdventGUIInstance::PartTwo(context);
		}

	}

	// 

	Debug::ACStopWatch m_variantStopWatch;

	std::stack<uint32_t> m_variantCandidates;
	std::unordered_set<uint32_t> m_foundCandidates;
	std::unordered_set<uint32_t> m_failedCandidates;
	uint32_t m_solvedIndex;

	//
	std::stack<Bitfield128> m_bigVariantCandidates;
	std::unordered_set<Bitfield128> m_bigFoundCandidates;
	std::unordered_set<Bitfield128> m_bigFailedCandidates;
	Bitfield128 solvedBigMask;

	uint32_t m_totalArrangementsPartOne;
	uint64_t m_totalArrangementsPartTwo;

	size_t m_currentSpringIndex;

	uint64_t m_cacheHits;
	std::vector<Spring> m_Springs;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 12;
	newParams.year = 2023;
	newParams.puzzleTitle = "Hot Springs";
	newParams.inputFilename = "sample.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
