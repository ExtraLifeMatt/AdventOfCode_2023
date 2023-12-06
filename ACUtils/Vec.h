#pragma once

#include <algorithm>
#include <cstdint>
#include <xmmintrin.h>


struct Vec2
{
public:
	Vec2();
	Vec2(float xy);
	Vec2(float _x, float _y);

	static const Vec2 UnitX;
	static const Vec2 UnitY;
	static const Vec2 Zero;

	bool IsZero() const;
	Vec2 RotateByRadians(float radians) const;
	Vec2 RotateByDegrees(float degrees) const;

	float Length() const;
	float LengthSqr() const;

	Vec2 operator+(const Vec2& RHS) const;
	Vec2 operator-(const Vec2& RHS) const;
	Vec2 operator/(const Vec2& RHS) const;
	Vec2 operator*(float scalar) const;
	Vec2 operator/(float scalar) const;
	Vec2& operator+=(const Vec2& RHS);
	Vec2& operator-=(const Vec2& RHS);
	Vec2& operator*=(float scalar);
	Vec2& operator/=(float scalar);
	bool operator==(const Vec2& RHS) const;
	bool operator!=(const Vec2& RHS) const;
	float& operator[](int i);
	float operator[](int i) const;

	__m128 AsVector(const Vec2& highValues = Vec2::Zero) const;

	float x;
	float y;
};

struct alignas(16) Vec3
{
public:
	Vec3();
	Vec3(float xyz);
	Vec3(float _x, float _y, float _z);
	Vec3(const __m128& _vec);

	static const Vec3 UnitX;
	static const Vec3 UnitY;
	static const Vec3 UnitZ;
	static const Vec3 Zero;

	bool IsZero() const;

	Vec3 operator+(const Vec3& RHS) const;
	Vec3 operator-(const Vec3& RHS) const;
	Vec3 operator*(const Vec3& RHS) const;
	Vec3 operator/(const Vec3& RHS) const;
	Vec3 operator*(float scalar) const;
	Vec3 operator/(float scalar) const;
	Vec3& operator+=(const Vec3& RHS);
	Vec3& operator*=(float scalar);
	bool operator==(const Vec3& RHS) const;
	bool operator!=(const Vec3& RHS) const;
	float& operator[](int i);
	float operator[](int i) const;

	bool AllLessThan(const Vec3& RHS) const;
	bool AnyLessThan(const Vec3& RHS) const;
	bool AllLessThanOrEqual(const Vec3& RHS) const;
	bool AnyLessThanOrEqual(const Vec3& RHS) const;
	bool AllGreaterThan(const Vec3& RHS) const;
	bool AnyGreaterThan(const Vec3& RHS) const;
	bool AllGreaterThanOrEqual(const Vec3& RHS) const;
	bool AnyGreaterThanOrEqual(const Vec3& RHS) const;

	Vec3 PerComponentMin(const Vec3& RHS) const;
	Vec3 PerComponentMax(const Vec3& RHS) const;
	Vec2 XY() const;
	Vec2 XZ() const;
	Vec2 YZ() const;

	__m128 AsVector() const;

	float Length() const;
	float LengthSqr() const;

	float DistanceSquared(const Vec3& Dest) const;
	float Distance(const Vec3& Dest) const;

	float x;
	float y;
	float z;
	float w; // Putting it here just so we can initialize it rather than have random data since I don't think padding bytes have any guarantee of initialization values.
};

struct alignas(16) Vec4
{
public:
	Vec4();
	Vec4(float xyzw);
	Vec4(float _x, float _y, float _z, float _w);
	Vec4(const __m128& _vec);
	Vec4(const Vec3& vec);

	static const Vec4 UnitX;
	static const Vec4 UnitY;
	static const Vec4 UnitZ;
	static const Vec4 UnitW;
	static const Vec4 Zero;

	bool IsZero() const;

	Vec4 operator+(const Vec4& RHS) const;
	Vec4 operator-(const Vec4& RHS) const;
	Vec4 operator*(const Vec4& RHS) const;
	Vec4 operator/(const Vec4& RHS) const;
	Vec4 operator*(float scalar) const;
	Vec4 operator/(float scalar) const;
	Vec4& operator+=(const Vec4& RHS);
	Vec4& operator*=(float scalar);
	bool operator==(const Vec4& RHS) const;
	bool operator!=(const Vec4& RHS) const;
	float& operator[](int i);
	float operator[](int i) const;

	bool AllLessThan(const Vec4& RHS) const;
	bool AnyLessThan(const Vec4& RHS) const;
	bool AllLessThanOrEqual(const Vec4& RHS) const;
	bool AnyLessThanOrEqual(const Vec4& RHS) const;
	bool AllGreaterThan(const Vec4& RHS) const;
	bool AnyGreaterThan(const Vec4& RHS) const;
	bool AllGreaterThanOrEqual(const Vec4& RHS) const;
	bool AnyGreaterThanOrEqual(const Vec4& RHS) const;

	Vec4 PerComponentMin(const Vec4& RHS) const;
	Vec4 PerComponentMax(const Vec4& RHS) const;
	Vec2 XY() const;
	Vec2 XZ() const;
	Vec2 YZ() const;
	Vec3 XYZ() const;

	__m128 AsVector() const;

	float Length() const;
	float LengthSqr() const;

	float DistanceSquared(const Vec4& Dest) const;
	float Distance(const Vec4& Dest) const;

	Vec4 Normalized() const;
	uint32_t ToRGBA() const;

	float x;
	float y;
	float z;
	float w;
};
