#include "Math.h"

#include "IntVec.h"

void Math::SinCos(float& outSine, float& outCosine, float value)
{
	float quotient = (INV_PI * 0.5f) * value;
	if (value >= 0.0f)
	{
		quotient = (float)((int)(quotient + 0.5f));
	}
	else
	{
		quotient = (float)((int)(quotient - 0.5f));
	}

	float y = value - (2.0f * PI) * quotient;

	// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
	float sign;
	if (y > HALF_PI)
	{
		y = PI - y;
		sign = -1.0f;
	}
	else if (y < -HALF_PI)
	{
		y = -PI - y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = y * y;

	// 11-degree minimax approximation
	outSine = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	// 10-degree minimax approximation
	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	outCosine = sign * p;
}

int32_t Math::RoundToInt32(float f)
{
	return (int32_t)((fabs(f) + 0.5f) * (f < 0.0f ? -1.0f : 1.0f));
}

int32_t Math::EstimatePrimeNumbersInRange(int32_t UpperLimit)
{
	return UpperLimit / (int32_t)ceilf(logf((float)UpperLimit));
}

bool Math::IsPrime(uint32_t WholeNumber)
{
	if (WholeNumber == 2)
	{
		return true;
	}

	// All evens besides two cannot be prime, so make sure we're odd.
	if ((WholeNumber & 1) == 0)
	{
		return false;
	}

	const int32_t totalIterations = (int32_t)sqrt(WholeNumber);
	for (int32_t i = 1; i < totalIterations; ++i)
	{
		if (WholeNumber % (1 + i * 2) == 0)
		{
			return false;
		}
	}

	return true;
}

void Math::GeneratePrimeNumbers(int32_t UpperLimit, std::vector<int32_t>& outPrimes)
{
	outPrimes.clear();
	outPrimes.reserve(EstimatePrimeNumbersInRange(UpperLimit));
	outPrimes.push_back(2); // Everything is divisible by 1, so we skip it.
	for (int32_t i = 3; i < UpperLimit; i += 2)
	{
		if (IsPrime(i))
		{
			outPrimes.push_back(i);
		}
	}
}

void Math::PopulatePrimeNumbers(int32_t TotalRequested, std::vector<int32_t>& outPrimes)
{
	// Count by Powers of 2 until we are >= our requested amount;
	int32_t estimatedNumPrimes = 0;
	for (uint32_t i = 1 << 8; i < (1U << 31); i <<= 1)
	{
		estimatedNumPrimes = EstimatePrimeNumbersInRange(i);
		if (estimatedNumPrimes >= TotalRequested)
		{
			GeneratePrimeNumbers(i, outPrimes);

			int32_t Remainder = estimatedNumPrimes - TotalRequested;
			if (Remainder != 0)
			{
				outPrimes.erase(outPrimes.begin() + TotalRequested, outPrimes.end());
			}
			break;
		}
	}
}

//uint32_t Math::GetManhattanDistance(const IntVec2& a, const IntVec2& b)
//{
//	return abs(a.x - b.x) + abs(a.y - b.y);
//}
//
//uint64_t Math::GetManhattanDistance64(const Int64Vec2& a, const Int64Vec2& b)
//{
//	return (uint64_t)abs(a.x - b.x) + (uint64_t)abs(a.y - b.y);
//}