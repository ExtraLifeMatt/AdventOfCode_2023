#pragma once

#include "Debug.h"
#include "IntVec.h"
#include "Vec.h"


struct AABB
{
public:
	AABB() : Min(Vec3::Zero), Max(Vec3::Zero) {};
	AABB(const Vec3& _min, const Vec3& _max) : Min(_min), Max(_max) {};

	bool Intersects(const AABB& Other) const
	{
		if (Min.AnyGreaterThan(Other.GetMax()) || Max.AnyLessThan(Other.GetMin()))
		{
			return false;
		}

		return true;
	}

	AABB Intersection(const AABB& Other) const
	{
		assert(Intersects(Other));
		return AABB(Min.PerComponentMax(Other.GetMin()), Max.PerComponentMin(Other.GetMax()));
	}

	AABB ExpandBy(const Vec3& Expansion) const
	{
		return AABB(Min - Expansion, Max + Expansion);
	}

	AABB ExpandToContain(const Vec3& Point) const
	{
		return AABB(Min.PerComponentMin(Point), Max.PerComponentMax(Point));
	}

	const Vec3& GetMin() const { return Min; }
	const Vec3& GetMax() const { return Max; }
	Vec3 GetCenter() const { return Min + (Max - Min) / 2; }
	Vec3 GetSize() const { return Max - Min; }

	bool Contains(const Vec3& Pos) const
	{
		return Pos.AllGreaterThanOrEqual(Min) && Pos.AllLessThanOrEqual(Max);
	}

	bool operator==(const AABB& RHS) const
	{
		return Min == RHS.Min && Max == RHS.Max;
	}

	bool operator!=(const AABB& RHS) const
	{
		return Min != RHS.Min || Max != RHS.Max;
	}

	static AABB MakeFromCenterAndExtents(const Vec3& Center, const Vec3& Extents)
	{
		return AABB(Center - Extents, Center + Extents);
	}
private:
	Vec3 Min;
	Vec3 Max;
};

struct IntAABB
{
public:
	IntAABB() : Min(IntVec3::Zero), Max(IntVec3::Zero) {};
	IntAABB(const IntVec3& _min, const IntVec3& _max) : Min(_min), Max(_max) {};
	IntAABB(const IntVec3& _center, const int32_t Extents) : Min(_center - IntVec3(Extents)), Max(_center + IntVec3(Extents)) {}
	bool Intersects(const IntAABB& Other) const
	{
		if (Min.AnyGreaterThan(Other.GetMax()) || Max.AnyLessThan(Other.GetMin()))
		{
			return false;
		}

		return true;
	}

	IntAABB Intersection(const IntAABB& Other) const
	{
		assert(Intersects(Other));
		return IntAABB(Min.PerComponentMax(Other.GetMin()), Max.PerComponentMin(Other.GetMax()));
	}

	bool IsOrthagonalTo(const IntAABB& Other) const
	{
		if (Intersects(Other))
		{
			return GetCenter().AnyEqualTo(Other.GetCenter());
		}

		return false;
	}

	IntAABB ExpandBy(const IntVec3& Expansion) const
	{
		return IntAABB(Min - Expansion, Max + Expansion);
	}

	IntAABB ExpandToContain(const IntVec3& Point) const
	{
		return IntAABB(Min.PerComponentMin(Point), Max.PerComponentMax(Point));
	}

	const IntVec3& GetMin() const { return Min; }
	const IntVec3& GetMax() const { return Max; }
	IntVec3 GetCenter() const { return Min + (Max - Min) / 2; }
	IntVec3 GetSize() const { return Max - Min; }

	bool Contains(const IntVec3& Pos) const
	{
		return Pos.AllGreaterThanOrEqual(Min) && Pos.AllLessThanOrEqual(Max);
	}

	bool operator==(const IntAABB& RHS) const
	{
		return Min == RHS.Min && Max == RHS.Max;
	}

	bool operator!=(const IntAABB& RHS) const
	{
		return Min != RHS.Min || Max != RHS.Max;
	}

	static IntAABB MakeFromCenterAndExtents(const IntVec3& Center, const IntVec3& Extents)
	{
		return IntAABB(Center - Extents, Center + Extents);
	}
private:
	IntVec3 Min;
	IntVec3 Max;
};

struct IntAABB2D
{
public:
	IntAABB2D() : Min(IntVec2::Zero), Max(IntVec2::Zero) {};
	IntAABB2D(const IntVec2& _min, const IntVec2& _max) : Min(_min), Max(_max) {};
	IntAABB2D(const IntVec2& _center, const int32_t Extents) : Min(_center - IntVec2(Extents)), Max(_center + IntVec2(Extents)) {}
	bool Intersects(const IntAABB2D& Other) const
	{
		if (Min.AnyGreaterThan(Other.GetMax()) || Max.AnyLessThan(Other.GetMin()))
		{
			return false;
		}

		return true;
	}

	IntAABB2D Intersection(const IntAABB2D& Other) const
	{
		assert(Intersects(Other));
		return IntAABB2D(Min.PerComponentMax(Other.GetMin()), Max.PerComponentMin(Other.GetMax()));
	}

	IntAABB2D ExpandBy(const IntVec2& Expansion) const
	{
		return IntAABB2D(Min - Expansion, Max + Expansion);
	}

	IntAABB2D ExpandToContain(const IntVec2& Point) const
	{
		return IntAABB2D(Min.PerComponentMin(Point), Max.PerComponentMax(Point));
	}

	const IntVec2& GetMin() const { return Min; }
	const IntVec2& GetMax() const { return Max; }
	IntVec2 GetCenter() const { return Min + (Max - Min) / 2; }
	IntVec2 GetSize() const { return Max - Min; }

	bool Contains(const IntVec2& Pos) const
	{
		return Pos.AllGreaterThanOrEqual(Min) && Pos.AllLessThanOrEqual(Max);
	}

	bool operator==(const IntAABB2D& RHS) const
	{
		return Min == RHS.Min && Max == RHS.Max;
	}

	bool operator!=(const IntAABB2D& RHS) const
	{
		return Min != RHS.Min || Max != RHS.Max;
	}

	static IntAABB2D MakeFromCenterAndExtents(const IntVec2& Center, const IntVec2& Extents)
	{
		return IntAABB2D(Center - Extents, Center + Extents);
	}

private:
	IntVec2 Min;
	IntVec2 Max;
};

struct Int64AABB2D
{
public:
	Int64AABB2D() : Min(Int64Vec2::Zero), Max(Int64Vec2::Zero) {};
	Int64AABB2D(const Int64Vec2& _min, const Int64Vec2& _max) : Min(_min), Max(_max) {};
	Int64AABB2D(const Int64Vec2& _center, const int64_t Extents) : Min(_center - Int64Vec2(Extents)), Max(_center + Int64Vec2(Extents)) {}
	bool Intersects(const Int64AABB2D& Other) const
	{
		if (Min.AnyGreaterThan(Other.GetMax()) || Max.AnyLessThan(Other.GetMin()))
		{
			return false;
		}

		return true;
	}

	Int64AABB2D Intersection(const Int64AABB2D& Other) const
	{
		assert(Intersects(Other));
		return Int64AABB2D(Min.PerComponentMax(Other.GetMin()), Max.PerComponentMin(Other.GetMax()));
	}

	Int64AABB2D ExpandBy(const Int64Vec2& Expansion) const
	{
		return Int64AABB2D(Min - Expansion, Max + Expansion);
	}

	Int64AABB2D ExpandToContain(const Int64Vec2& Point) const
	{
		return Int64AABB2D(Min.PerComponentMin(Point), Max.PerComponentMax(Point));
	}

	const Int64Vec2& GetMin() const { return Min; }
	const Int64Vec2& GetMax() const { return Max; }
	Int64Vec2 GetCenter() const { return Min + (Max - Min) / 2; }
	Int64Vec2 GetSize() const { return Max - Min; }

	bool Contains(const Int64Vec2& Pos) const
	{
		return Pos.AllGreaterThanOrEqual(Min) && Pos.AllLessThanOrEqual(Max);
	}

	bool operator==(const Int64AABB2D& RHS) const
	{
		return Min == RHS.Min && Max == RHS.Max;
	}

	bool operator!=(const Int64AABB2D& RHS) const
	{
		return Min != RHS.Min || Max != RHS.Max;
	}

	static Int64AABB2D MakeFromCenterAndExtents(const Int64Vec2& Center, const Int64Vec2& Extents)
	{
		return Int64AABB2D(Center - Extents, Center + Extents);
	}

private:
	Int64Vec2 Min;
	Int64Vec2 Max;
};