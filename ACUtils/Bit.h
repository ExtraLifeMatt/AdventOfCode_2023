#pragma once

#include <algorithm>
#include <cstdint>
#include <intrin.h>

class Bitfield128
{
public:
	Bitfield128() : bitfield{ 0ULL, 0ULL } {}
	Bitfield128(uint64_t low, uint64_t high)
		: bitfield{ low, high }
	{
	}

	bool IsZero() const { return bitfield[0] == 0ULL && bitfield[1] == 0ULL; }

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

namespace Bits
{
	constexpr size_t BitArraySize32(size_t totalElements) { return std::max((totalElements + 31) / 32, 1ULL); }
	constexpr size_t BitArraySize64(size_t totalElements) { return std::max((totalElements + 63) / 64, 1ULL); }
	constexpr size_t BitArraySize128(size_t totalElements) { return std::max((totalElements + 127) / 127, 1ULL); }

	constexpr uint32_t CreateBitMask(uint32_t offset, uint32_t numberOfBits)
	{
		return numberOfBits == 32 ? ~0U : ((1U << numberOfBits) - 1) << offset; 
	}

	constexpr uint64_t CreateBitMask64(uint32_t offset, uint32_t numberOfBits)
	{
		return numberOfBits == 64 ? ~0ULL : ((1ULL << numberOfBits) - 1ULL) << offset;
	}

	inline Bitfield128 CreateBitMask128(uint32_t offset, uint32_t numberOfBits)
	{
		uint64_t lowMask = 0;
		uint64_t highMask = 0;

		if (offset < 64)
		{
			if (offset + numberOfBits > 64)
			{
				// Straddles barrier
				lowMask = Bits::CreateBitMask64(offset, 64 - offset);
				highMask = Bits::CreateBitMask64(0, numberOfBits - (64 - offset));
			}
			else
			{
				lowMask = Bits::CreateBitMask64(offset, numberOfBits);
			}
		}
		else
		{
			highMask = Bits::CreateBitMask64(offset % 64, numberOfBits);
		}

		return Bitfield128(lowMask, highMask);
	}

	constexpr uint32_t CountLeadingZeros(uint32_t value)
	{
		return (value == 0U) ? 32 : (uint32_t)_lzcnt_u32(value);
	}

	constexpr uint32_t CountLeadingZeros64(uint64_t value)
	{
		return (value == 0UL) ? 64 : (uint32_t)_lzcnt_u64(value);
	}

	inline uint32_t CountLeadingZeros128(const Bitfield128& value)
	{
		if (value.GetHigh() == 0)
		{
			return 64 + Bits::CountLeadingZeros64(value.GetLow());
		}

		return Bits::CountLeadingZeros64(value.GetHigh());
	}

	constexpr uint32_t CountTrailingZeros(uint32_t value)
	{
		return (value == 0U) ? 32 : (uint32_t)_tzcnt_u32(value);
	}

	constexpr uint32_t CountTrailingZeros64(uint64_t value)
	{
		return (value == 0UL) ? 64 : (uint32_t)_tzcnt_u64(value);
	}

	inline uint32_t CountTrailingZeros128(const Bitfield128& value)
	{
		if (value.GetLow() == 0)
		{
			return 64 + Bits::CountTrailingZeros64(value.GetHigh());
		}

		return Bits::CountTrailingZeros64(value.GetHigh());
	}

	constexpr uint32_t PopCount32(uint32_t value)
	{
		return (value == ~0) ? 32 : (uint32_t)__popcnt(value);
	}

	constexpr uint32_t PopCount64(uint64_t value)
	{
		return (value == ~0UL) ? 64 : (uint32_t)__popcnt64(value);
	}

	inline uint32_t PopCount128(const Bitfield128& value)
	{
		return PopCount64(value.GetLow()) + PopCount64(value.GetHigh());
	}

	constexpr uint32_t GetLeastSignificantBitIndex(uint32_t value)
	{
		return CountTrailingZeros(value);
	}

	constexpr uint64_t GetLeastSignificantBitIndex(uint64_t value)
	{
		return CountTrailingZeros64(value);
	}

	inline uint64_t GetLeastSignificantBitIndex(const Bitfield128& value)
	{
		if (value.GetLow())
		{
			return Bits::CountTrailingZeros64(value.GetLow());
		}

		if (value.GetHigh())
		{
			return 64 + Bits::CountTrailingZeros64(value.GetHigh());
		}

		return 128;
	}

	constexpr uint32_t GetMostSignificantBitIndex(uint32_t value)
	{
		return 31 - CountLeadingZeros(value);
	}

	constexpr uint32_t GetMostSignificantBitIndex(uint64_t value)
	{
		return 63 - CountLeadingZeros64(value);
	}

	inline uint32_t GetMostSignificantBitIndex(const Bitfield128& value)
	{
		if (value.GetHigh())
		{
			return 64 + 64 - Bits::CountLeadingZeros64(value.GetHigh());
		}

		if (value.GetLow())
		{
			return 64 - Bits::CountLeadingZeros64(value.GetLow());
		}

		return 128;
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

	inline void GetContiguousBitsLSB128(const Bitfield128& bits, uint32_t& outIndex, uint32_t& outCount)
	{
		uint32_t bitRange = 128;
		if (bits.GetLow())
		{
			Bits::GetContiguousBitsLSB64(bits.GetLow(), outIndex, outCount);

			if (outIndex + outCount == 64)
			{
				uint32_t highIndex = 0;
				uint32_t highNumBits = 0;

				Bits::GetContiguousBitsLSB64(bits.GetHigh(), highIndex, highNumBits);
				if (highIndex == 0)
				{
					outCount += highNumBits;
				}
			}
		}
		else if (bits.GetHigh())
		{
			Bits::GetContiguousBitsLSB64(bits.GetHigh(), outIndex, outCount);
			outIndex += 64;
		}
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

	inline void GetContiguousBitsMSB128(const Bitfield128& bits, uint32_t& outIndex, uint32_t& outCount)
	{
		uint32_t bitRange = 128;
		if (bits.GetHigh())
		{
			Bits::GetContiguousBitsMSB64(bits.GetHigh(), outIndex, outCount);
			outIndex = 128 - outIndex;
			if (outIndex - outCount == 0)
			{
				uint32_t lowIndex = 0;
				uint32_t lowNumBits = 0;

				Bits::GetContiguousBitsMSB64(bits.GetLow(), lowIndex, lowNumBits);
				outCount = 64 - outIndex;
				if (lowIndex == 64)
				{
					outCount += lowNumBits;
				}
			}
		}
		else if (bits.GetLow())
		{
			Bits::GetContiguousBitsMSB64(bits.GetLow(), outIndex, outCount);
			outIndex = 64 - outIndex;
		}
	}
} // Bits

