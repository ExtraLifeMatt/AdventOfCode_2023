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
		return ((1 << (numberOfBits + offset)) - 1) & ~((1 << offset) - 1);
	}

	constexpr uint32_t CreateBitMask64(uint32_t offset, uint32_t numberOfBits)
	{
		return ((1ULL << (numberOfBits + offset)) - 1) & ~((1ULL << offset) - 1);
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
		return CountTrailingZeros(value) - 1;
	}

	constexpr uint64_t GetLeastSignificanBitIndex(uint64_t value)
	{
		return CountTrailingZeros64(value) - 1;
	}

	constexpr uint32_t GetMostSignificantBitIndex(uint32_t value)
	{
		return 31 - CountLeadingZeros(value);
	}

	constexpr uint32_t GetMostSignificantBitIndex(uint64_t value)
	{
		return 63 - CountLeadingZeros64(value);
	}

} // Bits
