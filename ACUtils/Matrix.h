#pragma once

#include <cassert>
#include "Quat.h"
#include "Vec.h"

// Row Major Order since that's what I'm most used to. 
struct alignas(16) Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4(const Vec4& row0, const Vec4& row1, const Vec4& row2, const Vec4& row3);
	Matrix4x4(float x, float y, float z); // Translation, scale is 1, rotation is 0.
	Matrix4x4(float x, float y, float z, float yaw, float pitch, float roll);
	Matrix4x4(float x, float y, float z, float yaw, float pitch, float roll, float scaleX, float scaleY, float scaleZ);
	Matrix4x4(const Vec3& translation);
	Matrix4x4(const Vec3& translation, const Quat& rotation);
	Matrix4x4(const Vec3& translation, const Quat& rotation, const Vec3& scale);

	static Matrix4x4 IDENTITY;

	Quat GetRotation() const;
	Vec3 GetScale() const;
	inline const Vec4& GetTranslation() const { return rows[3]; }

	Matrix4x4 GetRotationMatrix() const;
	Matrix4x4 Inverse() const;
	Matrix4x4 Transpose() const;
	Matrix4x4 AsColumnOrder() const;

	void SetScale(float xyz);
	void SetScale(const Vec3& xyz);
	void SetScale(float x, float y, float z);

	inline void SetTranslation(const Vec3& translation) { rows[3] = translation; }
	void SetRotation(const Quat& rotation);

	Matrix4x4 operator+(const Matrix4x4& RHS) const;
	Matrix4x4 operator-(const Matrix4x4& RHS) const;
	Matrix4x4 operator*(const Matrix4x4& RHS) const;
	Matrix4x4 operator+=(const Vec4& RHS) const;
	Matrix4x4 operator-=(const Vec4& RHS) const;
	Matrix4x4 operator*=(const Quat& RHS) const;

	Matrix4x4& operator+=(const Matrix4x4& RHS);
	Matrix4x4& operator-=(const Matrix4x4& RHS);
	Matrix4x4& operator*=(const Matrix4x4& RHS);
	Matrix4x4& operator+=(const Vec4& RHS);
	Matrix4x4& operator-=(const Vec4& RHS);
	Matrix4x4& operator*=(const Quat& RHS);

	bool operator==(const Matrix4x4& RHS) const;
	bool operator!=(const Matrix4x4& RHS) const;
	inline Vec4& operator[](int i) { assert(i >= 0 && i < 4); return rows[i]; }
	inline Vec4 operator[](int i) const { assert(i >= 0 && i < 4); return rows[i]; }

	Vec4 rows[4];
};
