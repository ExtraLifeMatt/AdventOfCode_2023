#pragma once

#include <algorithm>
#include <cstdint>
#include <intrin.h>

namespace Bits
{
	constexpr size_t BitArraySize32(size_t totalElements) { return std::max((totalElements + 31) / 32, 1ULL); }
	constexpr size_t BitArraySize64(size_t totalElements) { return std::max((totalElements + 63) / 64, 1ULL); }

	constexpr uint32_t CreateBitMask(uint32_t offset, uint32_t numberOfBits)
	{
		return numberOfBits == 32 ? ~0U : ((1U << numberOfBits) - 1) << offset; 
	}

	constexpr uint64_t CreateBitMask64(uint32_t offset, uint32_t numberOfBits)
	{
		return numberOfBits == 64 ? ~0ULL : ((1ULL << numberOfBits) - 1ULL) << offset;
	}

	constexpr uint32_t CountLeadingZeros(uint32_t value)
	{
		return (value == 0U) ? 32 : (uint32_t)_lzcnt_u32(value);
	}

	constexpr uint32_t CountLeadingZeros64(uint64_t value)
	{
		return (value == 0UL) ? 64 : (uint32_t)_lzcnt_u64(value);
	}

	constexpr uint32_t CountTrailingZeros(uint32_t value)
	{
		return (value == 0U) ? 32 : (uint32_t)_tzcnt_u32(value);
	}

	constexpr uint32_t CountTrailingZeros64(uint64_t value)
	{
		return (value == 0UL) ? 64 : (uint32_t)_tzcnt_u64(value);
	}

	constexpr uint32_t PopCount32(uint32_t value)
	{
		return (value == ~0) ? 32 : (uint32_t)__popcnt(value);
	}

	constexpr uint32_t PopCount64(uint64_t value)
	{
		return (value == ~0UL) ? 64 : (uint32_t)__popcnt64(value);
	}

	constexpr uint32_t GetLeastSignificantBitIndex(uint32_t value)
	{
		return CountTrailingZeros(value);
	}

	constexpr uint64_t GetLeastSignificanBitIndex(uint64_t value)
	{
		return CountTrailingZeros64(value);
	}

	constexpr uint32_t GetMostSignificantBitIndex(uint32_t value)
	{
		return 31 - CountLeadingZeros(value);
	}

	constexpr uint32_t GetMostSignificantBitIndex(uint64_t value)
	{
		return 63 - CountLeadingZeros64(value);
	}

	inline void GetContiguousBitsLSB64(uint64_t bits, uint32_t& outIndex, uint32_t& outCount)
	{
		uint32_t bitTrz = CountTrailingZeros64(bits);
		uint64_t shifted = ~(bits >> bitTrz);
		uint32_t bitTrzEnd = CountTrailingZeros64(shifted);
		outIndex = bitTrz;
		outCount = bitTrzEnd;
	}

	inline void GetContiguousBitsLSB(uint32_t bits, uint32_t& outIndex, uint32_t& outCount)
	{
		uint32_t bitTrz = CountTrailingZeros(bits);
		uint32_t shifted = ~(bits >> bitTrz);
		uint32_t bitTrzEnd = CountTrailingZeros(shifted);
		outIndex = bitTrz;
		outCount = bitTrzEnd;
	}

	inline void GetContiguousBitsMSB64(uint64_t bits, uint32_t& outIndex, uint32_t& outCount)
	{
		uint32_t bitTrz = CountLeadingZeros64(bits);
		uint64_t shifted = ~(bits << bitTrz);
		uint32_t bitTrzEnd = CountLeadingZeros64(shifted);
		outIndex = bitTrz;
		outCount = bitTrzEnd;
	}

	inline void GetContiguousBitsMSB(uint32_t bits, uint32_t& outIndex, uint32_t& outCount)
	{
		uint32_t bitTrz = CountLeadingZeros(bits);
		uint32_t shifted = ~(bits << bitTrz);
		uint32_t bitTrzEnd = CountLeadingZeros(shifted);
		outIndex = bitTrz;
		outCount = bitTrzEnd;
	}

} // Bits


class Bitfield128
{
public:
	Bitfield128() : bitfield{ 0ULL, 0ULL } {}
	Bitfield128(uint64_t low, uint64_t high)
		: bitfield{ low, high }
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

				Bits::GetContiguousBitsLSB64(bitfield.GetHigh(), highIndex, highNumBits);
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

	static uint32_t CountLeadingZeros(const Bitfield128& bitfield)
	{
		if (bitfield.GetHigh() == 0)
		{
			return 64 + Bits::CountLeadingZeros64(bitfield.GetLow());
		}

		return Bits::CountLeadingZeros64(bitfield.GetHigh());
	}

	static uint32_t CountTrailingZeros(const Bitfield128& bitfield)
	{
		if (bitfield.GetLow() == 0)
		{
			return 64 + Bits::CountTrailingZeros64(bitfield.GetHigh());
		}

		return Bits::CountTrailingZeros64(bitfield.GetHigh());
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