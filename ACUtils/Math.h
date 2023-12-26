#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>
#include "Bit.h"

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])
#endif

namespace Math
{
	static constexpr int32_t MANHATTAN_PI = 4;
	static constexpr float PI = 3.1415926535897932f;
	static constexpr float TWO_PI = PI * 2.0f;
	static constexpr float HALF_PI = PI * 0.5f;
	static constexpr float INV_PI = 1.0f / PI;

	constexpr float DegreesToRadians(float deg) { return deg * (PI / 180.0f); }
	constexpr float RadiansToDegrees(float rad) { return rad * (180.0f / PI); }
		
	void SinCos(float& outSine, float& outCosine, float value);

	constexpr uint64_t Factorial(uint32_t fac) { return fac == 1 ? 1 : fac * Factorial(fac - 1); }
	constexpr uint64_t Permutations(uint32_t n, uint32_t r) { return Factorial(n) / Factorial(n - r); }

	constexpr int32_t Sign(int32_t v) { return v < 0 ? -1 : 1; } // Returns -1 or 1
	constexpr int32_t SignWithZero(int32_t v) { return v < 0 ? -1 : v > 0 ? 1 : 0; } // Returns -1, 0, or 1

	constexpr int32_t FloorToInt32(float f)
	{
		int32_t fAsInt = (int32_t)f;
		return fAsInt - ((float)fAsInt > f);
	}

	constexpr int32_t CeilToInt32(float f)
	{
		int32_t fAsInt = (int32_t)f;
		return fAsInt + ((float)fAsInt > f);
	}

	int32_t RoundToInt32(float f);

	constexpr uint32_t CeilLog2_32(uint32_t value) { return 32 - Bits::CountLeadingZeros(value - 1); }
	constexpr uint32_t CeilLog2_64(uint64_t value) { return 64 - Bits::CountLeadingZeros64(value - 1); }
	constexpr uint32_t FloorLog2_32(uint32_t value) { return 32 - Bits::CountLeadingZeros(value); }
	constexpr uint32_t FloorLog2_64(uint64_t value) { return 64 - Bits::CountLeadingZeros64(value); }

	// Pow
	constexpr uint32_t Pow(const uint32_t base, uint32_t pow)
	{
		return pow == 0 ? 1 : base * Pow(base, pow - 1);
	}

	constexpr uint32_t Exp(const uint32_t value, uint32_t pow)
	{
		return value < pow ? 0 : 1 + Exp(value / pow, pow);
	}

	uint64_t LCM(uint64_t a, uint64_t b);
	uint64_t GCD(uint64_t a, uint64_t b);

	int32_t EstimatePrimeNumbersInRange(int32_t UpperLimit);
	void PrimeFactorization32(uint32_t value, std::vector<uint32_t>& outFactors);
	bool IsPrime(uint32_t WholeNumber);
	constexpr uint32_t GetNthPrime(uint32_t Index);

	// Finds all the primes
	void GeneratePrimeNumbers(int32_t UpperLimit, std::vector<int32_t>& outPrimes); // Finds all the primes between 2 - UpperLimit
	void PopulatePrimeNumbers(int32_t TotalRequested, std::vector<int32_t>& outPrimes); // Returns X primes (the upper limit is found dynamically)

	// Manhattan / Taxicab
	//uint32_t GetManhattanDistance(const struct IntVec2& a, const IntVec2& b);
	//uint64_t GetManhattanDistance64(const struct Int64Vec2& a, const Int64Vec2& b);
}