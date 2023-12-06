#include "Vec.h"
#include "Math.h"

#include <cassert>

const Vec2 Vec2::UnitX(1.0f, 0.0f);
const Vec2 Vec2::UnitY(0.0f, 1.0f);
const Vec2 Vec2::Zero(0.0f, 0.0f);

const Vec3 Vec3::UnitX(1.0f, 0.0f, 0.0f);
const Vec3 Vec3::UnitY(0.0f, 1.0f, 0.0f);
const Vec3 Vec3::UnitZ(0.0f, 0.0f, 1.0f);
const Vec3 Vec3::Zero(0.0f, 0.0f, 0.0f);

const Vec4 Vec4::UnitX(1.0f, 0.0f, 0.0f, 0.0f);
const Vec4 Vec4::UnitY(0.0f, 1.0f, 0.0f, 0.0f);
const Vec4 Vec4::UnitZ(0.0f, 0.0f, 1.0f, 0.0f);
const Vec4 Vec4::UnitW(0.0f, 0.0f, 0.0f, 1.0f);
const Vec4 Vec4::Zero(0.0f, 0.0f, 0.0f, 0.0f);

Vec2::Vec2(): x(0.0f), y(0.0f)
{

}

Vec2::Vec2(float _x, float _y): x(_x), y(_y)
{

}

Vec2::Vec2(float xy): x(xy), y(xy)
{

}

bool Vec2::IsZero() const
{
	return x == 0.0f && y == 0.0f;
}

Vec2 Vec2::RotateByRadians(float radians) const
{
	float sine = 0.0f;
	float cosine = 0.0f;
	Math::SinCos(sine, cosine, radians);

	return Vec2(cosine * x - y * sine, sine * x + y * cosine);
}

Vec2 Vec2::RotateByDegrees(float degrees) const
{
	return RotateByRadians(Math::DegreesToRadians(degrees));
}

float Vec2::Length() const
{
	return (float)sqrt(LengthSqr());
}

float Vec2::LengthSqr() const
{
	return x * x + y * y;
}

Vec2 Vec2::operator+(const Vec2& RHS) const
{
	return Vec2(x + RHS.x, y + RHS.y);
}

Vec2 Vec2::operator-(const Vec2& RHS) const
{
	return Vec2(x - RHS.x, y - RHS.y);
}

Vec2& Vec2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

Vec2 Vec2::operator/(float scalar) const
{
	return Vec2(x / scalar, y / scalar);
}

Vec2& Vec2::operator-=(const Vec2& RHS)
{
	x -= RHS.x;
	y -= RHS.y;

	return *this;
}

Vec2 Vec2::operator/(const Vec2& RHS) const
{
	return Vec2(x / RHS.x, y / RHS.y);
}

Vec2& Vec2::operator+=(const Vec2& RHS)
{
	x += RHS.x;
	y += RHS.y;

	return *this;
}

Vec2 Vec2::operator*(float scalar) const
{
	return Vec2(x * scalar, y * scalar);
}

Vec2& Vec2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;

	return *this;
}

bool Vec2::operator==(const Vec2& RHS) const
{
	return x != RHS.x || y != RHS.y;
}

bool Vec2::operator!=(const Vec2& RHS) const
{
	return x == RHS.x && y == RHS.y;
}

float Vec2::operator[](int i) const
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	default: assert(false);
	}

	return x;
}

float& Vec2::operator[](int i)
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	default: assert(false);
	}

	return x;
}

__m128 Vec2::AsVector(const Vec2& highValues /*= Vec2::Zero*/) const
{
	alignas(16) float tmp[4] = { x, y, highValues.x, highValues.y };
	return _mm_load_ss(tmp);
}

bool Vec3::operator!=(const Vec3& RHS) const
{
	return x != RHS.x || y != RHS.y || z != RHS.z;
}

bool Vec3::operator==(const Vec3& RHS) const
{
	return x == RHS.x && y == RHS.y && z == RHS.z;
}

Vec3& Vec3::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

Vec3 Vec3::operator/(const Vec3& RHS) const
{
	return Vec3(x / RHS.x, y / RHS.y, z / RHS.z);
}

Vec3 Vec3::operator*(const Vec3& RHS) const
{
	return Vec3(x * RHS.x, y * RHS.y, z * RHS.z);
}

Vec3 Vec3::operator/(float scalar) const
{
	return Vec3(x / scalar, y / scalar, z / scalar);
}

Vec3 Vec3::operator*(float scalar) const
{
	return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3::Vec3(const __m128& _vec) : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
	_mm_store_ps(reinterpret_cast<float*>(this), _vec);
}

Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{

}

Vec3::Vec3(float xyz) : x(xyz), y(xyz), z(xyz), w(0.0f)
{

}

Vec3::Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(0.0f)
{

}

bool Vec3::IsZero() const
{
	return x == 0.0f && y == 0.0f && z == 0.0f;
}

Vec3& Vec3::operator+=(const Vec3& RHS)
{
	x += RHS.x;
	y += RHS.y;
	z += RHS.z;

	return *this;
}

Vec3 Vec3::operator+(const Vec3& RHS) const
{
	return Vec3(x + RHS.x, y + RHS.y, z + RHS.z);
}

Vec3 Vec3::operator-(const Vec3& RHS) const
{
	return Vec3(x - RHS.x, y - RHS.y, z - RHS.z);
}

float Vec3::operator[](int i) const
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	case 2: return z; break;
	default: assert(false);
	}

	return x;
}

float& Vec3::operator[](int i)
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	case 2: return z; break;
	default: assert(false);
	}

	return x;
}

bool Vec3::AllLessThan(const Vec3& RHS) const
{
	return x < RHS.x && y < RHS.y && z < RHS.z;
}

bool Vec3::AnyLessThan(const Vec3& RHS) const
{
	return x < RHS.x || y < RHS.y || z < RHS.z;
}

bool Vec3::AllLessThanOrEqual(const Vec3& RHS) const
{
	return x <= RHS.x && y <= RHS.y && z <= RHS.z;
}

bool Vec3::AnyLessThanOrEqual(const Vec3& RHS) const
{
	return x <= RHS.x || y <= RHS.y || z <= RHS.z;
}

bool Vec3::AllGreaterThan(const Vec3& RHS) const
{
	return x > RHS.x && y > RHS.y && z > RHS.z;
}

bool Vec3::AnyGreaterThan(const Vec3& RHS) const
{
	return x > RHS.x || y > RHS.y || z > RHS.z;
}

bool Vec3::AllGreaterThanOrEqual(const Vec3& RHS) const
{
	return x >= RHS.x && y >= RHS.y && z >= RHS.z;
}

bool Vec3::AnyGreaterThanOrEqual(const Vec3& RHS) const
{
	return x >= RHS.x || y >= RHS.y || z >= RHS.z;
}

Vec3 Vec3::PerComponentMin(const Vec3& RHS) const
{
	return Vec3(std::min(x, RHS.x), std::min(y, RHS.y), std::min(z, RHS.z));
}

Vec3 Vec3::PerComponentMax(const Vec3& RHS) const
{
	return Vec3(std::max(x, RHS.x), std::max(y, RHS.y), std::max(z, RHS.z));
}

Vec2 Vec3::XY() const
{
	return Vec2(x, y);
}

Vec2 Vec3::XZ() const
{
	return Vec2(x, z);
}

Vec2 Vec3::YZ() const
{
	return Vec2(y, z);
}

__m128 Vec3::AsVector() const
{
	return _mm_load_ps(reinterpret_cast<const float*>(this));
}

float Vec3::Length() const
{
	return sqrtf(LengthSqr());
}

float Vec3::LengthSqr() const
{
	return x * x + y * y + z * z;
}

float Vec3::DistanceSquared(const Vec3& Dest) const
{
	float dx = (Dest.x - x);
	float dy = (Dest.y - y);
	float dz = (Dest.z - z);
	return dx * dx + dy * dy + dz * dz;
}

float Vec3::Distance(const Vec3& Dest) const
{
	return sqrtf(DistanceSquared(Dest));
}

bool Vec4::operator!=(const Vec4& RHS) const
{
	return x != RHS.x || y != RHS.y || z != RHS.z || w != RHS.w;
}

bool Vec4::operator==(const Vec4& RHS) const
{
	return x == RHS.x && y == RHS.y && z == RHS.z && w == RHS.w;
}

Vec4& Vec4::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

Vec4 Vec4::operator/(const Vec4& RHS) const
{
	return Vec4 (x / RHS.x, y / RHS.y, z / RHS.z, w / RHS.z);
}

Vec4& Vec4::operator+=(const Vec4& RHS)
{
	x += RHS.x;
	y += RHS.y;
	z += RHS.z;
	w += RHS.w;

	return *this;
}

Vec4 Vec4::operator*(const Vec4& RHS) const
{
	return Vec4(x * RHS.x, y * RHS.y, z * RHS.z, w * RHS.w);
}

Vec4 Vec4::operator/(float scalar) const
{
	return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
}

Vec4 Vec4::operator*(float scalar) const
{
	return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

float Vec4::operator[](int i) const
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	case 2: return z; break;
	case 3: return w; break;
	default: assert(false);
	}

	return x;
}

float& Vec4::operator[](int i)
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	case 2: return z; break;
	case 3: return w; break;
	default: assert(false);
	}

	return x;
}

Vec4::Vec4(const __m128& _vec) : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
	_mm_store_ps(reinterpret_cast<float*>(this), _vec);
}

Vec4::Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{

}

Vec4::Vec4(float xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
{

}

Vec4::Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
{

}

Vec4::Vec4(const Vec3& vec): x(vec.x), y(vec.y), z(vec.z), w(0.0f)
{

}

bool Vec4::IsZero() const
{
	return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
}

Vec4 Vec4::operator+(const Vec4& RHS) const
{
	return Vec4(x + RHS.x, y + RHS.y, z + RHS.z, w + RHS.w);
}

Vec4 Vec4::operator-(const Vec4& RHS) const
{
	return Vec4(x - RHS.x, y - RHS.y, z - RHS.z, w - RHS.w);
}

bool Vec4::AllLessThan(const Vec4& RHS) const
{
	return x < RHS.x && y < RHS.y && z < RHS.z && w < RHS.w;
}

bool Vec4::AnyLessThan(const Vec4& RHS) const
{
	return x < RHS.x || y < RHS.y || z < RHS.z || w < RHS.w;
}

bool Vec4::AllLessThanOrEqual(const Vec4& RHS) const
{
	return x <= RHS.x && y <= RHS.y && z <= RHS.z && w <= RHS.w;
}

bool Vec4::AnyLessThanOrEqual(const Vec4& RHS) const
{
	return x <= RHS.x || y <= RHS.y || z <= RHS.z || w <= RHS.w;
}

bool Vec4::AllGreaterThan(const Vec4& RHS) const
{
	return x > RHS.x && y > RHS.y && z > RHS.z && w > RHS.w;
}

bool Vec4::AnyGreaterThan(const Vec4& RHS) const
{
	return x > RHS.x || y > RHS.y || z > RHS.z;
}

bool Vec4::AllGreaterThanOrEqual(const Vec4& RHS) const
{
	return x >= RHS.x && y >= RHS.y && z >= RHS.z && w >= RHS.w;
}

bool Vec4::AnyGreaterThanOrEqual(const Vec4& RHS) const
{
	return x >= RHS.x || y >= RHS.y || z >= RHS.z || w >= RHS.w;
}

Vec4 Vec4::PerComponentMin(const Vec4& RHS) const
{
	return Vec4(std::min(x, RHS.x), std::min(y, RHS.y), std::min(z, RHS.z), std::min(w, RHS.w));
}

Vec4 Vec4::PerComponentMax(const Vec4& RHS) const
{
	return Vec4(std::max(x, RHS.x), std::max(y, RHS.y), std::max(z, RHS.z), std::max(w, RHS.w));
}

Vec2 Vec4::XY() const
{
	return Vec2(x, y);
}

Vec2 Vec4::XZ() const
{
	return Vec2(x, z);
}

Vec2 Vec4::YZ() const
{
	return Vec2(y, z);
}

Vec3 Vec4::XYZ() const
{
	return Vec3(x, y, z);
}

__m128 Vec4::AsVector() const
{
	return _mm_load_ps(reinterpret_cast<const float*>(this));
}

float Vec4::Length() const
{
	return sqrtf(LengthSqr());
}

float Vec4::LengthSqr() const
{
	return x * x + y * y + z * z + w * w;
}

float Vec4::DistanceSquared(const Vec4& Dest) const
{
	float dx = (Dest.x - x);
	float dy = (Dest.y - y);
	float dz = (Dest.z - z);
	float dw = (Dest.w - w);
	return dx * dx + dy * dy + dz * dz + dw * dw;
}

float Vec4::Distance(const Vec4& Dest) const
{
	return sqrtf(DistanceSquared(Dest));
}

Vec4 Vec4::Normalized() const
{
	float length = Length();
	return Vec4(x / length, y / length, z / length, w / length);
}

uint32_t Vec4::ToRGBA() const
{
	Vec4 normalized = Normalized();
	uint32_t r = (uint32_t)(normalized.x * 255.0f);
	uint32_t g = (uint32_t)(normalized.y * 255.0f);
	uint32_t b = (uint32_t)(normalized.z * 255.0f);
	uint32_t a = (uint32_t)(normalized.w * 255.0f);

	return r << 24 | g << 16 | b << 8 | a;
}
