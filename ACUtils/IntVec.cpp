#include "IntVec.h"

const IntVec2 IntVec2::UnitX = IntVec2(1, 0);
const IntVec2 IntVec2::UnitY = IntVec2(0, 1);
const IntVec2 IntVec2::Zero = IntVec2(0);

const IntVec3 IntVec3::UnitX = IntVec3(1, 0, 0);
const IntVec3 IntVec3::UnitY = IntVec3(0, 1, 0);
const IntVec3 IntVec3::UnitZ = IntVec3(0, 0, 1);
const IntVec3 IntVec3::Zero = IntVec3(0);

const IntVec4 IntVec4::UnitX = IntVec4(1, 0, 0, 0);
const IntVec4 IntVec4::UnitY = IntVec4(0, 1, 0, 0);
const IntVec4 IntVec4::UnitZ = IntVec4(0, 0, 1, 0);
const IntVec4 IntVec4::UnitW = IntVec4(0, 0, 0, 1);
const IntVec4 IntVec4::Zero = IntVec4(0);
const Int64Vec2 Int64Vec2::Zero = Int64Vec2(0LL);
const Int64Vec3 Int64Vec3::Zero = Int64Vec3(0LL);

bool IntVec2::operator!=(const IntVec2& RHS) const
{
	return x != RHS.x || y != RHS.y;
}

bool IntVec2::operator==(const IntVec2& RHS) const
{
	return x == RHS.x && y == RHS.y;
}

IntVec2& IntVec2::operator/=(int32_t scalar)
{
	x /= scalar;
	y /= scalar;

	return *this;
}

IntVec2::IntVec2(const Vec2& vec) : x(Math::FloorToInt32(vec.x)), y(Math::FloorToInt32(vec.y))
{

}

IntVec2::IntVec2() : x(0), y(0)
{

}

IntVec2::IntVec2(int32_t xy) : x(xy), y(xy)
{

}

IntVec2::IntVec2(int32_t _x, int32_t _y) : x(_x), y(_y)
{

}

bool IntVec2::IsZero() const
{
	return x == 0 && y == 0;
}

IntVec2 IntVec2::RotateByDegrees(float degrees) const
{
	return RotateByRadians(Math::DegreesToRadians(degrees));
}

IntVec2 IntVec2::RotateByRadians(float radians) const
{
	float sine = 0.0f;
	float cosine = 0.0f;
	Math::SinCos(sine, cosine, radians);
	float x1 = cosine * (float)x - (float)y * sine;
	float y1 = sine * (float)x + (float)y * cosine;
	return IntVec2((int32_t)x1, (int32_t)y1);
}

float IntVec2::Length() const
{
	return (float)sqrt((float)LengthSqr());
}

uint32_t IntVec2::LengthSqr() const
{
	return x * x + y * y;
}

size_t IntVec2::ToHash() const
{
	return (size_t)Hash::Hash64((uint64_t)(x * 17 + y * 23));
}

Vec2 IntVec2::Normalize() const
{
	const float len = Length();
	return Vec2((float)x / len, (float)y / len);
}

IntVec2& IntVec2::operator+=(const IntVec2& RHS)
{
	x += RHS.x;
	y += RHS.y;

	return *this;
}

IntVec2 IntVec2::operator+(const IntVec2& RHS) const
{
	return IntVec2(x + RHS.x, y + RHS.y);
}

IntVec2 IntVec2::operator-(const IntVec2& RHS) const
{
	return IntVec2(x - RHS.x, y - RHS.y);
}

IntVec2& IntVec2::operator-=(const IntVec2& RHS)
{
	x -= RHS.x;
	y -= RHS.y;

	return *this;
}

IntVec2 IntVec2::operator*(int32_t scalar) const
{
	return IntVec2(x * scalar, y * scalar);
}

IntVec2& IntVec2::operator*=(int32_t scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

IntVec2 IntVec2::operator/(int32_t scalar) const
{
	return IntVec2(x / scalar, y / scalar);
}

int32_t IntVec2::operator[](int i) const
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	default: assert(false);
	}

	return x;
}

int32_t& IntVec2::operator[](int i)
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	default: assert(false);
	}

	return x;
}

bool IntVec2::AllLessThan(const IntVec2& RHS) const
{
	return x < RHS.x && y < RHS.y;
}

bool IntVec2::AnyLessThan(const IntVec2& RHS) const
{
	return x < RHS.x || y < RHS.y;
}

bool IntVec2::AllLessThanOrEqual(const IntVec2& RHS) const
{
	return x <= RHS.x && y <= RHS.y;
}

bool IntVec2::AnyLessThanOrEqual(const IntVec2& RHS) const
{
	return x <= RHS.x || y <= RHS.y;
}

bool IntVec2::AllGreaterThan(const IntVec2& RHS) const
{
	return x > RHS.x && y > RHS.y;
}

bool IntVec2::AnyGreaterThan(const IntVec2& RHS) const
{
	return x > RHS.x || y > RHS.y;
}

bool IntVec2::AllGreaterThanOrEqual(const IntVec2& RHS) const
{
	return x >= RHS.x && y >= RHS.y;
}

bool IntVec2::AnyGreaterThanOrEqual(const IntVec2& RHS) const
{
	return x >= RHS.x || y >= RHS.y;
}

IntVec2 IntVec2::PerComponentMin(const IntVec2& RHS) const
{
	return IntVec2(std::min(x, RHS.x), std::min(y, RHS.y));
}

IntVec2 IntVec2::PerComponentMax(const IntVec2& RHS) const
{
	return IntVec2(std::max(x, RHS.x), std::max(y, RHS.y));
}

__m128i IntVec2::AsVector(const IntVec2& highValues /*= IntVec2::Zero*/) const
{
	alignas(16) int32_t tmp[4] = { x, y, highValues.x, highValues.y };
	return _mm_load_si128(reinterpret_cast<__m128i*>(tmp));
}

bool Int64Vec2::operator!=(const Int64Vec2& RHS) const
{
	return x != RHS.x || y != RHS.y;
}

bool Int64Vec2::operator==(const Int64Vec2& RHS) const
{
	return x == RHS.x && y == RHS.y;
}

Int64Vec2& Int64Vec2::operator/=(int64_t scalar)
{
	x /= scalar;
	y /= scalar;

	return *this;
}

Int64Vec2::Int64Vec2(const Vec2& vec) : x(Math::FloorToInt32(vec.x)), y(Math::FloorToInt32(vec.y))
{

}

Int64Vec2::Int64Vec2() : x(0), y(0)
{

}

Int64Vec2::Int64Vec2(int64_t xy) : x(xy), y(xy)
{

}

Int64Vec2::Int64Vec2(int64_t _x, int64_t _y) : x(_x), y(_y)
{

}

bool Int64Vec2::IsZero() const
{
	return x == 0 && y == 0;
}

Int64Vec2 Int64Vec2::RotateByDegrees(float degrees) const
{
	return RotateByRadians(Math::DegreesToRadians(degrees));
}

Int64Vec2 Int64Vec2::RotateByRadians(float radians) const
{
	float sine = 0.0f;
	float cosine = 0.0f;
	Math::SinCos(sine, cosine, radians);
	float x1 = cosine * (float)x - (float)y * sine;
	float y1 = sine * (float)x + (float)y * cosine;
	return Int64Vec2((int64_t)x1, (int64_t)y1);
}

float Int64Vec2::Length() const
{
	return (float)sqrt((float)LengthSqr());
}

uint64_t Int64Vec2::LengthSqr() const
{
	return x * x + y * y;
}

size_t Int64Vec2::ToHash() const
{
	return (size_t)Hash::Hash64(x * 17ULL + y * 23ULL);
}

Vec2 Int64Vec2::Normalize() const
{
	const float len = Length();
	return Vec2((float)x / len, (float)y / len);
}

Int64Vec2& Int64Vec2::operator+=(const Int64Vec2& RHS)
{
	x += RHS.x;
	y += RHS.y;

	return *this;
}

Int64Vec2 Int64Vec2::operator+(const Int64Vec2& RHS) const
{
	return Int64Vec2(x + RHS.x, y + RHS.y);
}

Int64Vec2 Int64Vec2::operator-(const Int64Vec2& RHS) const
{
	return Int64Vec2(x - RHS.x, y - RHS.y);
}

Int64Vec2& Int64Vec2::operator-=(const Int64Vec2& RHS)
{
	x -= RHS.x;
	y -= RHS.y;

	return *this;
}

Int64Vec2 Int64Vec2::operator*(int64_t scalar) const
{
	return Int64Vec2(x * scalar, y * scalar);
}

Int64Vec2& Int64Vec2::operator*=(int64_t scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

Int64Vec2 Int64Vec2::operator/(int64_t scalar) const
{
	return Int64Vec2(x / scalar, y / scalar);
}

int64_t Int64Vec2::operator[](int i) const
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	default: assert(false);
	}

	return x;
}

int64_t& Int64Vec2::operator[](int i)
{
	switch (i)
	{
	case 0: return x; break;
	case 1: return y; break;
	default: assert(false);
	}

	return x;
}

bool Int64Vec2::AllLessThan(const Int64Vec2& RHS) const
{
	return x < RHS.x && y < RHS.y;
}

bool Int64Vec2::AnyLessThan(const Int64Vec2& RHS) const
{
	return x < RHS.x || y < RHS.y;
}

bool Int64Vec2::AllLessThanOrEqual(const Int64Vec2& RHS) const
{
	return x <= RHS.x && y <= RHS.y;
}

bool Int64Vec2::AnyLessThanOrEqual(const Int64Vec2& RHS) const
{
	return x <= RHS.x || y <= RHS.y;
}

bool Int64Vec2::AllGreaterThan(const Int64Vec2& RHS) const
{
	return x > RHS.x && y > RHS.y;
}

bool Int64Vec2::AnyGreaterThan(const Int64Vec2& RHS) const
{
	return x > RHS.x || y > RHS.y;
}

bool Int64Vec2::AllGreaterThanOrEqual(const Int64Vec2& RHS) const
{
	return x >= RHS.x && y >= RHS.y;
}

bool Int64Vec2::AnyGreaterThanOrEqual(const Int64Vec2& RHS) const
{
	return x >= RHS.x || y >= RHS.y;
}

Int64Vec2 Int64Vec2::PerComponentMin(const Int64Vec2& RHS) const
{
	return Int64Vec2(std::min(x, RHS.x), std::min(y, RHS.y));
}

Int64Vec2 Int64Vec2::PerComponentMax(const Int64Vec2& RHS) const
{
	return Int64Vec2(std::max(x, RHS.x), std::max(y, RHS.y));
}


bool Int64Vec3::operator!=(const Int64Vec3 & RHS) const
{
	return x != RHS.x || y != RHS.y || z != RHS.z;
}

bool Int64Vec3::operator==(const Int64Vec3 & RHS) const
{
	return x == RHS.x && y == RHS.y && z == RHS.z;
}

Int64Vec3& Int64Vec3::operator/=(int64_t scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;

	return *this;
}

Int64Vec3::Int64Vec3(const Vec3& vec) : x(Math::FloorToInt32(vec.x)), y(Math::FloorToInt32(vec.y)), z(Math::FloorToInt32(vec.z))
{

}

Int64Vec3::Int64Vec3() : x(0), y(0), z(0)
{

}

Int64Vec3::Int64Vec3(int64_t xyz) : x(xyz), y(xyz), z(xyz)
{

}

Int64Vec3::Int64Vec3(int64_t _x, int64_t _y, int64_t _z) : x(_x), y(_y), z(_z)
{

}

bool Int64Vec3::IsZero() const
{
	return x == 0 && y == 0 && z == 0;
}

Int64Vec3 Int64Vec3::RotateByDegrees(float degrees) const
{
	return RotateByRadians(Math::DegreesToRadians(degrees));
}

Int64Vec3 Int64Vec3::RotateByRadians(float radians) const
{
	float sine = 0.0f;
	float cosine = 0.0f;
	Math::SinCos(sine, cosine, radians);
	float x1 = cosine * (float)x - (float)y * sine;
	float y1 = sine * (float)x + (float)y * cosine;
	return Int64Vec3((int64_t)x1, (int64_t)y1, z);
}

double Int64Vec3::Length() const
{
	return sqrt((double)LengthSqr());
}

uint64_t Int64Vec3::LengthSqr() const
{
	return x * x + y * y + z * z;
}

size_t Int64Vec3::ToHash() const
{
	return (size_t)Hash::Hash64(x * 17ULL + y * 23ULL + z * 51ULL);
}

Vec3 Int64Vec3::Normalize() const
{
	const double len = Length();
	return Vec3((float)(x / len), (float)(y / len), (float)(z / len));
}

Int64Vec3& Int64Vec3::operator+=(const Int64Vec3& RHS)
{
	x += RHS.x;
	y += RHS.y;
	z += RHS.z;

	return *this;
}

Int64Vec3 Int64Vec3::operator+(const Int64Vec3& RHS) const
{
	return Int64Vec3(x + RHS.x, y + RHS.y, z + RHS.z);
}

Int64Vec3 Int64Vec3::operator-(const Int64Vec3& RHS) const
{
	return Int64Vec3(x - RHS.x, y - RHS.y, z - RHS.z);
}

Int64Vec3& Int64Vec3::operator-=(const Int64Vec3& RHS)
{
	x -= RHS.x;
	y -= RHS.y;
	z -= RHS.z;

	return *this;
}

Int64Vec3 Int64Vec3::operator*(int64_t scalar) const
{
	return Int64Vec3(x * scalar, y * scalar, z * scalar);
}

Int64Vec3& Int64Vec3::operator*=(int64_t scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

Int64Vec3 Int64Vec3::operator/(int64_t scalar) const
{
	return Int64Vec3(x / scalar, y / scalar, z / scalar);
}

int64_t Int64Vec3::operator[](int i) const
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

int64_t& Int64Vec3::operator[](int i)
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

bool Int64Vec3::AllLessThan(const Int64Vec3& RHS) const
{
	return x < RHS.x && y < RHS.y && z < RHS.z;
}

bool Int64Vec3::AnyLessThan(const Int64Vec3& RHS) const
{
	return x < RHS.x || y < RHS.y || z < RHS.z;
}

bool Int64Vec3::AllLessThanOrEqual(const Int64Vec3& RHS) const
{
	return x <= RHS.x && y <= RHS.y && z <= RHS.z;
}

bool Int64Vec3::AnyLessThanOrEqual(const Int64Vec3& RHS) const
{
	return x <= RHS.x || y <= RHS.y || z <= RHS.z;
}

bool Int64Vec3::AllGreaterThan(const Int64Vec3& RHS) const
{
	return x > RHS.x && y > RHS.y && z > RHS.z;
}

bool Int64Vec3::AnyGreaterThan(const Int64Vec3& RHS) const
{
	return x > RHS.x || y > RHS.y || z > RHS.z;
}

bool Int64Vec3::AllGreaterThanOrEqual(const Int64Vec3& RHS) const
{
	return x >= RHS.x && y >= RHS.y && z >= RHS.z;
}

bool Int64Vec3::AnyGreaterThanOrEqual(const Int64Vec3& RHS) const
{
	return x >= RHS.x || y >= RHS.y || z >= RHS.z;
}

Int64Vec3 Int64Vec3::PerComponentMin(const Int64Vec3& RHS) const
{
	return Int64Vec3(std::min(x, RHS.x), std::min(y, RHS.y), std::min(z, RHS.z));
}

Int64Vec3 Int64Vec3::PerComponentMax(const Int64Vec3& RHS) const
{
	return Int64Vec3(std::max(x, RHS.x), std::max(y, RHS.y), std::max(z, RHS.z));
}

bool IntVec3::operator!=(const IntVec3& RHS) const
{
	return x != RHS.x || y != RHS.y || z != RHS.z;
}

bool IntVec3::operator==(const IntVec3& RHS) const
{
	return x == RHS.x && y == RHS.y && z == RHS.z;
}

IntVec3& IntVec3::operator*=(int32_t scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

IntVec3 IntVec3::operator/(const IntVec3& RHS) const
{
	return IntVec3(x / RHS.x, y / RHS.y, z / RHS.z);
}

IntVec3 IntVec3::operator*(const IntVec3& RHS) const
{
	return IntVec3(x * RHS.x, y * RHS.y, z * RHS.z);
}

IntVec3 IntVec3::operator/(int32_t scalar) const
{
	return IntVec3(x / scalar, y / scalar, z / scalar);
}

IntVec3 IntVec3::operator*(int32_t scalar) const
{
	return IntVec3(x * scalar, y * scalar, z * scalar);
}

size_t IntVec3::ToHash() const
{
	return (size_t)Hash::Hash64((uint64_t)(x * 17 + y * 23 + z * 53));
}

IntVec3::IntVec3(const __m128i& _vec) : x(0), y(0), z(0), w(0)
{
	_mm_store_si128(reinterpret_cast<__m128i*>(this), _vec);
}

IntVec3::IntVec3() : x(0), y(0), z(0), w(0)
{

}

IntVec3::IntVec3(int32_t xyz) : x(xyz), y(xyz), z(xyz), w(0)
{

}

IntVec3::IntVec3(int32_t _x, int32_t _y, int32_t _z) : x(_x), y(_y), z(_z), w(0)
{

}

bool IntVec3::IsZero() const
{
	return x == 0 && y == 0 && z == 0;
}

IntVec3& IntVec3::operator+=(const IntVec3& RHS)
{
	x += RHS.x;
	y += RHS.y;
	z += RHS.z;

	return *this;
}

IntVec3 IntVec3::operator+(const IntVec3& RHS) const
{
	return IntVec3(x + RHS.x, y + RHS.y, z + RHS.z);
}

IntVec3 IntVec3::operator-(const IntVec3& RHS) const
{
	return IntVec3(x - RHS.x, y - RHS.y, z - RHS.z);
}

int32_t IntVec3::operator[](int i) const
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

int32_t& IntVec3::operator[](int i)
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

bool IntVec3::AllLessThan(const IntVec3& RHS) const
{
	return x < RHS.x && y < RHS.y && z < RHS.z;
}

bool IntVec3::AnyLessThan(const IntVec3& RHS) const
{
	return x < RHS.x || y < RHS.y || z < RHS.z;
}

bool IntVec3::AllLessThanOrEqual(const IntVec3& RHS) const
{
	return x <= RHS.x && y <= RHS.y && z <= RHS.z;
}

bool IntVec3::AnyLessThanOrEqual(const IntVec3& RHS) const
{
	return x <= RHS.x || y <= RHS.y || z <= RHS.z;
}

bool IntVec3::AllGreaterThan(const IntVec3& RHS) const
{
	return x > RHS.x && y > RHS.y && z > RHS.z;
}

bool IntVec3::AnyGreaterThan(const IntVec3& RHS) const
{
	return x > RHS.x || y > RHS.y || z > RHS.z;
}

bool IntVec3::AllGreaterThanOrEqual(const IntVec3& RHS) const
{
	return x >= RHS.x && y >= RHS.y && z >= RHS.z;
}

bool IntVec3::AnyGreaterThanOrEqual(const IntVec3& RHS) const
{
	return x >= RHS.x || y >= RHS.y || z >= RHS.z;
}

bool IntVec3::AnyEqualTo(const IntVec3& RHS) const
{
	return x == RHS.x || y == RHS.y || z == RHS.z;
}

IntVec3 IntVec3::PerComponentMin(const IntVec3& RHS) const
{
	return IntVec3(std::min(x, RHS.x), std::min(y, RHS.y), std::min(z, RHS.z));
}

IntVec3 IntVec3::PerComponentMax(const IntVec3& RHS) const
{
	return IntVec3(std::max(x, RHS.x), std::max(y, RHS.y), std::max(z, RHS.z));
}

IntVec2 IntVec3::XY() const
{
	return IntVec2(x, y);
}

IntVec2 IntVec3::XZ() const
{
	return IntVec2(x, z);
}

IntVec2 IntVec3::YZ() const
{
	return IntVec2(y, z);
}

int32_t IntVec3::DistanceSquared(const IntVec3& Dest) const
{
	int32_t dx = (Dest.x - x);
	int32_t dy = (Dest.y - y);
	int32_t dz = (Dest.z - z);
	return dx * dx + dy * dy + dz * dz;
}

int32_t IntVec3::Distance(const IntVec3& Dest) const
{
	return (int32_t)sqrtf((float)DistanceSquared(Dest));
}

float IntVec3::Length() const
{
	return sqrtf((float)LengthSqr());
}

uint32_t IntVec3::LengthSqr() const
{
	return x * x + y * y + z * z;
}

IntVec3 IntVec3::Abs() const
{
	return IntVec3(abs(x), abs(y), abs(z));
}

__m128i IntVec3::AsVector() const
{
	return _mm_load_si128(reinterpret_cast<const __m128i*>(this));
}

bool IntVec4::operator!=(const IntVec4& RHS) const
{
	return x != RHS.x || y != RHS.y || z != RHS.z || w != RHS.w;
}

bool IntVec4::operator==(const IntVec4& RHS) const
{
	return x == RHS.x && y == RHS.y && z == RHS.z && w == RHS.w;
}

IntVec4& IntVec4::operator*=(int32_t scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

IntVec4 IntVec4::operator/(const IntVec4& RHS) const
{
	return IntVec4(x / RHS.x, y / RHS.y, z / RHS.z, w / RHS.w);
}

IntVec4 IntVec4::operator*(const IntVec4& RHS) const
{
	return IntVec4(x * RHS.x, y * RHS.y, z * RHS.z, w * RHS.w);
}

IntVec4 IntVec4::operator/(int32_t scalar) const
{
	return IntVec4(x / scalar, y / scalar, z / scalar, w / scalar);
}

IntVec4 IntVec4::operator*(int32_t scalar) const
{
	return IntVec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

int32_t IntVec4::operator[](int i) const
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

int32_t& IntVec4::operator[](int i)
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

IntVec4::IntVec4(const __m128i& _vec) : x(0), y(0), z(0), w(0)
{
	_mm_store_si128(reinterpret_cast<__m128i*>(this), _vec);
}

IntVec4::IntVec4() : x(0), y(0), z(0), w(0)
{

}

IntVec4::IntVec4(int32_t xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
{

}

IntVec4::IntVec4(int32_t _x, int32_t _y, int32_t _z, int32_t _w) : x(_x), y(_y), z(_z), w(_w)
{

}

bool IntVec4::IsZero() const
{
	return x == 0 && y == 0 && z == 0 && w == 0;
}

IntVec4& IntVec4::operator+=(const IntVec4& RHS)
{
	x += RHS.x;
	y += RHS.y;
	z += RHS.z;
	w += RHS.w;

	return *this;
}

IntVec4 IntVec4::operator+(const IntVec4& RHS) const
{
	return IntVec4(x + RHS.x, y + RHS.y, z + RHS.z, w + RHS.w);
}

IntVec4 IntVec4::operator-(const IntVec4& RHS) const
{
	return IntVec4(x - RHS.x, y - RHS.y, z - RHS.z, w - RHS.w);
}

size_t IntVec4::ToHash() const
{
	return (size_t)Hash::Hash64(w * 71ULL + x * 17ULL + y * 23ULL + z * 53ULL);
}

bool IntVec4::AllLessThan(const IntVec4& RHS) const
{
	return x < RHS.x && y < RHS.y && z < RHS.z && w < RHS.w;
}

bool IntVec4::AnyLessThan(const IntVec4& RHS) const
{
	return x < RHS.x || y < RHS.y || z < RHS.z || w < RHS.w;
}

bool IntVec4::AllLessThanOrEqual(const IntVec4& RHS) const
{
	return x <= RHS.x && y <= RHS.y && z <= RHS.z && w <= RHS.w;
}

bool IntVec4::AnyLessThanOrEqual(const IntVec4& RHS) const
{
	return x <= RHS.x || y <= RHS.y || z <= RHS.z || w <= RHS.w;
}

bool IntVec4::AllGreaterThan(const IntVec4& RHS) const
{
	return x > RHS.x && y > RHS.y && z > RHS.z && w > RHS.w;
}

bool IntVec4::AnyGreaterThan(const IntVec4& RHS) const
{
	return x > RHS.x || y > RHS.y || z > RHS.z || w > RHS.w;
}

bool IntVec4::AllGreaterThanOrEqual(const IntVec4& RHS) const
{
	return x >= RHS.x && y >= RHS.y && z >= RHS.z && w >= RHS.w;
}

bool IntVec4::AnyGreaterThanOrEqual(const IntVec4& RHS) const
{
	return x >= RHS.x || y >= RHS.y || z >= RHS.z || w >= RHS.w;
}

IntVec4 IntVec4::PerComponentMin(const IntVec4& RHS) const
{
	return IntVec4(std::min(x, RHS.x), std::min(y, RHS.y), std::min(z, RHS.z), std::min(w, RHS.w));
}

IntVec4 IntVec4::PerComponentMax(const IntVec4& RHS) const
{
	return IntVec4(std::max(x, RHS.x), std::max(y, RHS.y), std::max(z, RHS.z), std::max(w, RHS.w));
}

IntVec2 IntVec4::XY() const
{
	return IntVec2(x, y);
}

IntVec2 IntVec4::XZ() const
{
	return IntVec2(x, z);
}

IntVec2 IntVec4::YZ() const
{
	return IntVec2(y, z);
}

IntVec3 IntVec4::XYZ() const
{
	return IntVec3(x, y, z);
}

int32_t IntVec4::DistanceSquared(const IntVec4& Dest) const
{
	int32_t dx = (Dest.x - x);
	int32_t dy = (Dest.y - y);
	int32_t dz = (Dest.z - z);
	int32_t dw = (Dest.w - w);
	return dx * dx + dy * dy + dz * dz + dw * dw;
}

int32_t IntVec4::Distance(const IntVec4& Dest) const
{
	return (int32_t)sqrtf((float)DistanceSquared(Dest));
}

IntVec4 IntVec4::Abs() const
{
	return IntVec4(abs(x), abs(y), abs(z), abs(w));
}

__m128i IntVec4::AsVector() const
{
	return _mm_load_si128(reinterpret_cast<const __m128i*>(this));
}
