#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <xmmintrin.h>

#include "Hash.h"
#include "Math.h"
#include "Vec.h"


struct IntVec2
{
public:
	IntVec2();
	IntVec2(int32_t xy);
	IntVec2(int32_t _x, int32_t _y);
	IntVec2(const Vec2& vec);

	static const IntVec2 UnitX;
	static const IntVec2 UnitY;
	static const IntVec2 Zero;

	bool IsZero() const;
	float Length() const;
	uint32_t LengthSqr() const;
	size_t ToHash() const;

	IntVec2 operator+(const IntVec2& RHS) const;
	IntVec2 operator-(const IntVec2& RHS) const;
	IntVec2 operator*(int32_t scalar) const;
	IntVec2 operator/(int32_t scalar) const;
	IntVec2& operator+=(const IntVec2& RHS);
	IntVec2& operator-=(const IntVec2& RHS);
	IntVec2& operator*=(int32_t scalar);
	IntVec2& operator/=(int32_t scalar);
	int32_t& operator[](int i);
	int32_t operator[](int i) const;
	bool operator==(const IntVec2& RHS) const;
	bool operator!=(const IntVec2& RHS) const;

	bool AllLessThan(const IntVec2& RHS) const;
	bool AnyLessThan(const IntVec2& RHS) const;
	bool AllLessThanOrEqual(const IntVec2& RHS) const;
	bool AnyLessThanOrEqual(const IntVec2& RHS) const;
	bool AllGreaterThan(const IntVec2& RHS) const;
	bool AnyGreaterThan(const IntVec2& RHS) const;
	bool AllGreaterThanOrEqual(const IntVec2& RHS) const;
	bool AnyGreaterThanOrEqual(const IntVec2& RHS) const;

	Vec2 Normalize() const;
	IntVec2 RotateByDegrees(float degrees) const;
	IntVec2 RotateByRadians(float radians) const;
	IntVec2 PerComponentMin(const IntVec2& RHS) const;
	IntVec2 PerComponentMax(const IntVec2& RHS) const;
	__m128i AsVector(const IntVec2& highValues = IntVec2::Zero) const;

	int32_t x;
	int32_t y;
};

struct Int64Vec2
{
public:
	Int64Vec2();
	Int64Vec2(int64_t xy);
	Int64Vec2(int64_t _x, int64_t _y);
	Int64Vec2(const Vec2& vec);

	static const Int64Vec2 UnitX;
	static const Int64Vec2 UnitY;
	static const Int64Vec2 Zero;

	bool IsZero() const;
	float Length() const;
	uint64_t LengthSqr() const;

	size_t ToHash() const;

	Int64Vec2 operator+(const Int64Vec2& RHS) const;
	Int64Vec2 operator-(const Int64Vec2& RHS) const;
	Int64Vec2 operator*(int64_t scalar) const;
	Int64Vec2 operator/(int64_t scalar) const;
	Int64Vec2& operator+=(const Int64Vec2& RHS);
	Int64Vec2& operator-=(const Int64Vec2& RHS);
	Int64Vec2& operator*=(int64_t scalar);
	Int64Vec2& operator/=(int64_t scalar);
	int64_t& operator[](int i);
	int64_t operator[](int i) const;
	bool operator==(const Int64Vec2& RHS) const;
	bool operator!=(const Int64Vec2& RHS) const;

	bool AllLessThan(const Int64Vec2& RHS) const;
	bool AnyLessThan(const Int64Vec2& RHS) const;
	bool AllLessThanOrEqual(const Int64Vec2& RHS) const;
	bool AnyLessThanOrEqual(const Int64Vec2& RHS) const;
	bool AllGreaterThan(const Int64Vec2& RHS) const;
	bool AnyGreaterThan(const Int64Vec2& RHS) const;
	bool AllGreaterThanOrEqual(const Int64Vec2& RHS) const;
	bool AnyGreaterThanOrEqual(const Int64Vec2& RHS) const;

	Vec2 Normalize() const;
	Int64Vec2 RotateByDegrees(float degrees) const;
	Int64Vec2 RotateByRadians(float radians) const;
	Int64Vec2 PerComponentMin(const Int64Vec2& RHS) const;
	Int64Vec2 PerComponentMax(const Int64Vec2& RHS) const;

	int64_t x;
	int64_t y;
};

struct Int64Vec3
{
public:
	Int64Vec3();
	Int64Vec3(int64_t xyz);
	Int64Vec3(int64_t _x, int64_t _y, int64_t _z);
	Int64Vec3(const Vec3& vec);

	static const Int64Vec3 UnitX;
	static const Int64Vec3 UnitY;
	static const Int64Vec3 Zero;

	bool IsZero() const;
	double Length() const;
	uint64_t LengthSqr() const;

	size_t ToHash() const;

	Int64Vec3 operator+(const Int64Vec3& RHS) const;
	Int64Vec3 operator-(const Int64Vec3& RHS) const;
	Int64Vec3 operator*(int64_t scalar) const;
	Int64Vec3 operator/(int64_t scalar) const;
	Int64Vec3& operator+=(const Int64Vec3& RHS);
	Int64Vec3& operator-=(const Int64Vec3& RHS);
	Int64Vec3& operator*=(int64_t scalar);
	Int64Vec3& operator/=(int64_t scalar);
	int64_t& operator[](int i);
	int64_t operator[](int i) const;
	bool operator==(const Int64Vec3& RHS) const;
	bool operator!=(const Int64Vec3& RHS) const;

	bool AllLessThan(const Int64Vec3& RHS) const;
	bool AnyLessThan(const Int64Vec3& RHS) const;
	bool AllLessThanOrEqual(const Int64Vec3& RHS) const;
	bool AnyLessThanOrEqual(const Int64Vec3& RHS) const;
	bool AllGreaterThan(const Int64Vec3& RHS) const;
	bool AnyGreaterThan(const Int64Vec3& RHS) const;
	bool AllGreaterThanOrEqual(const Int64Vec3& RHS) const;
	bool AnyGreaterThanOrEqual(const Int64Vec3& RHS) const;

	Vec3 Normalize() const;
	Int64Vec3 RotateByDegrees(float degrees) const;
	Int64Vec3 RotateByRadians(float radians) const;
	Int64Vec3 PerComponentMin(const Int64Vec3& RHS) const;
	Int64Vec3 PerComponentMax(const Int64Vec3& RHS) const;

	int64_t x;
	int64_t y;
	int64_t z;
};

struct alignas(16) IntVec3
{
public:
	IntVec3();
	IntVec3(int32_t xyz);
	IntVec3(int32_t _x, int32_t _y, int32_t _z);
	IntVec3(const __m128i& _vec);

	static const IntVec3 UnitX;
	static const IntVec3 UnitY;
	static const IntVec3 UnitZ;
	static const IntVec3 Zero;

	bool IsZero() const;

	IntVec3 operator+(const IntVec3& RHS) const;
	IntVec3 operator-(const IntVec3& RHS) const;
	IntVec3 operator*(const IntVec3& RHS) const;
	IntVec3 operator/(const IntVec3& RHS) const;
	IntVec3 operator*(int32_t scalar) const;
	IntVec3 operator/(int32_t scalar) const;
	IntVec3& operator+=(const IntVec3& RHS);
	IntVec3& operator*=(int32_t scalar);
	int32_t& operator[](int i);
	int32_t operator[](int i) const;
	bool operator==(const IntVec3& RHS) const;
	bool operator!=(const IntVec3& RHS) const;

	size_t ToHash() const;

	bool AllLessThan(const IntVec3& RHS) const;
	bool AnyLessThan(const IntVec3& RHS) const;
	bool AllLessThanOrEqual(const IntVec3& RHS) const;
	bool AnyLessThanOrEqual(const IntVec3& RHS) const;
	bool AllGreaterThan(const IntVec3& RHS) const;
	bool AnyGreaterThan(const IntVec3& RHS) const;
	bool AllGreaterThanOrEqual(const IntVec3& RHS) const;
	bool AnyGreaterThanOrEqual(const IntVec3& RHS) const;
	bool AnyEqualTo(const IntVec3& RHS) const;

	IntVec3 PerComponentMin(const IntVec3& RHS) const;
	IntVec3 PerComponentMax(const IntVec3& RHS) const;
	IntVec2 XY() const;
	IntVec2 XZ() const;
	IntVec2 YZ() const;

	int32_t DistanceSquared(const IntVec3& Dest) const;
	int32_t Distance(const IntVec3& Dest) const;

	float Length() const;
	uint32_t LengthSqr() const;
	IntVec3 Abs() const;

	__m128i AsVector() const;

	int32_t x;
	int32_t y;
	int32_t z;
	int32_t w; // Putting it here just so we can initialize it rather than have random data since I don't think padding bytes have any guarantee of initialization values.
};

struct alignas(16) IntVec4
{
public:
	IntVec4();
	IntVec4(int32_t xyz);
	IntVec4(int32_t _x, int32_t _y, int32_t _z, int32_t _w);
	IntVec4(const __m128i& _vec);

	static const IntVec4 UnitX;
	static const IntVec4 UnitY;
	static const IntVec4 UnitZ;
	static const IntVec4 UnitW;
	static const IntVec4 Zero;

	bool IsZero() const;

	IntVec4 operator+(const IntVec4& RHS) const;
	IntVec4 operator-(const IntVec4& RHS) const;
	IntVec4 operator*(const IntVec4& RHS) const;
	IntVec4 operator/(const IntVec4& RHS) const;
	IntVec4 operator*(int32_t scalar) const;
	IntVec4 operator/(int32_t scalar) const;
	IntVec4& operator+=(const IntVec4& RHS);
	IntVec4& operator*=(int32_t scalar);
	int32_t& operator[](int i);
	int32_t operator[](int i) const;
	bool operator==(const IntVec4& RHS) const;
	bool operator!=(const IntVec4& RHS) const;

	size_t ToHash() const;

	bool AllLessThan(const IntVec4& RHS) const;
	bool AnyLessThan(const IntVec4& RHS) const;
	bool AllLessThanOrEqual(const IntVec4& RHS) const;
	bool AnyLessThanOrEqual(const IntVec4& RHS) const;
	bool AllGreaterThan(const IntVec4& RHS) const;
	bool AnyGreaterThan(const IntVec4& RHS) const;
	bool AllGreaterThanOrEqual(const IntVec4& RHS) const;
	bool AnyGreaterThanOrEqual(const IntVec4& RHS) const;

	IntVec4 PerComponentMin(const IntVec4& RHS) const;
	IntVec4 PerComponentMax(const IntVec4& RHS) const;
	IntVec2 XY() const;
	IntVec2 XZ() const;
	IntVec2 YZ() const;
	IntVec3 XYZ() const;

	int32_t DistanceSquared(const IntVec4& Dest) const;
	int32_t Distance(const IntVec4& Dest) const;
	IntVec4 Abs() const;

	__m128i AsVector() const;

	int32_t x;
	int32_t y;
	int32_t z;
	int32_t w;
};


ENABLE_STL_HASH(Int64Vec2, ToHash);
ENABLE_STL_HASH(Int64Vec3, ToHash);
ENABLE_STL_HASH(IntVec3, ToHash);
ENABLE_STL_HASH(IntVec2, ToHash);
ENABLE_STL_HASH(IntVec4, ToHash);