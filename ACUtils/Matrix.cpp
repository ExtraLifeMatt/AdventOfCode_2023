#include "Matrix.h"
#include "Math.h"

Matrix4x4::Matrix4x4()
:rows{Vec4::Zero, Vec4::Zero, Vec4::Zero, Vec4::Zero}
{

}

Matrix4x4::Matrix4x4(float x, float y, float z)
: rows{ Vec4::UnitX, Vec4::UnitY, Vec4::UnitZ, Vec4(x, y, z, 1.0f)}
{

}

Matrix4x4::Matrix4x4(float x, float y, float z, float yaw, float pitch, float roll)
	: rows{ Vec4::UnitX, Vec4::UnitY, Vec4::UnitZ, Vec4(x, y, z, 1.0f) }
{
	float yawSin;
	float yawCos;
	float pitchSin;
	float pitchCos;
	float rollSin;
	float rollCos;

	Math::SinCos(yawSin, yawCos, Math::DegreesToRadians(yaw));
	Math::SinCos(pitchSin, pitchCos, Math::DegreesToRadians(pitch));
	Math::SinCos(rollSin, rollCos, Math::DegreesToRadians(roll));

	float tmp0 = rollCos * pitchSin;
	float tmp1 = rollSin * pitchSin;
	rows[0] = Vec4(rollCos * pitchCos, rollSin * pitchCos, -pitchSin, 0.0f);
	rows[1] = Vec4((tmp0 * yawSin) - (rollSin * yawCos), (tmp1 * yawSin) + (rollCos * yawCos), (pitchCos * yawSin), 0.0f);
	rows[2] = Vec4((tmp0 * yawCos) - (rollSin * yawSin), (tmp1 * yawCos) + (rollCos * yawSin), (pitchCos * yawCos), 0.0f);
}

Matrix4x4::Matrix4x4(float x, float y, float z, float yaw, float pitch, float roll, float scaleX, float scaleY, float scaleZ)
{

}

Matrix4x4::Matrix4x4(const Vec3& translation)
{

}

Matrix4x4::Matrix4x4(const Vec3& translation, const Quat& rotation)
{

}

Matrix4x4::Matrix4x4(const Vec3& translation, const Quat& rotation, const Vec3& scale)
{

}

Matrix4x4::Matrix4x4(const Vec4& row0, const Vec4& row1, const Vec4& row2, const Vec4& row3)
: rows{row0, row1, row2, row3}
{

}

Matrix4x4 Matrix4x4::IDENTITY = Matrix4x4(Vec4::UnitX, Vec4::UnitY, Vec4::UnitZ, Vec4::UnitW);

Quat Matrix4x4::GetRotation() const
{

}

Vec3 Matrix4x4::GetScale() const
{

}

Matrix4x4 Matrix4x4::GetRotationMatrix() const
{

}

Matrix4x4 Matrix4x4::Inverse() const
{

}

Matrix4x4 Matrix4x4::Transpose() const
{

}

Matrix4x4 Matrix4x4::AsColumnOrder() const
{

}

void Matrix4x4::SetScale(float xyz)
{

}

void Matrix4x4::SetScale(const Vec3& xyz)
{

}

void Matrix4x4::SetScale(float x, float y, float z)
{

}

void Matrix4x4::SetRotation(const Quat& rotation)
{

}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& RHS) const
{

}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& RHS) const
{

}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& RHS) const
{

}

Matrix4x4 Matrix4x4::operator*=(const Quat& RHS) const
{

}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& RHS)
{

}

Matrix4x4& Matrix4x4::operator*=(const Quat& RHS)
{

}

bool Matrix4x4::operator!=(const Matrix4x4& RHS) const
{

}

bool Matrix4x4::operator==(const Matrix4x4& RHS) const
{

}

Matrix4x4& Matrix4x4::operator-=(const Vec4& RHS)
{

}

Matrix4x4& Matrix4x4::operator+=(const Vec4& RHS)
{

}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& RHS)
{

}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& RHS)
{

}

Matrix4x4 Matrix4x4::operator-=(const Vec4& RHS) const
{

}

Matrix4x4 Matrix4x4::operator+=(const Vec4& RHS) const
{

}

