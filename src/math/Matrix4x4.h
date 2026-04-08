#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include <memory.h>

namespace dingus {


// --------------------------------------------------------------------------

/**
 *  4x4 matrix.
 */
struct SMatrix4x4 {
public:
	SMatrix4x4();
	SMatrix4x4( const float* f );
	SMatrix4x4( float _11, float _12, float _13, float _14,
				float _21, float _22, float _23, float _24,
				float _31, float _32, float _33, float _34,
				float _41, float _42, float _43, float _44 );
	SMatrix4x4( const SVector3& pos, const SQuaternion& rot );

	const SVector3& getAxisX() const;
	SVector3& getAxisX();
	const SVector3& getAxisY() const;
	SVector3& getAxisY();
	const SVector3& getAxisZ() const;
	SVector3& getAxisZ();
	const SVector3& getOrigin() const;
	SVector3& getOrigin();

	void	identify();
	void	transpose( SMatrix4x4& dest ) const;
	void	transpose();
	void	invert();

	friend SMatrix4x4 operator* (const SMatrix4x4& a, const SMatrix4x4& b)
	{
		SMatrix4x4 out;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				out.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
			}
		}
		return out;
	}

	void perspectiveFovLH(float fovy, float aspect, float zn, float zf)
	{
		identify();
		m[0][0] = 1.0f / (aspect * tanf(fovy / 2.0f));
		m[1][1] = 1.0f / tanf(fovy / 2.0f);
		m[2][2] = zf / (zf - zn);
		m[2][3] = 1.0f;
		m[3][2] = (zf * zn) / (zn - zf);
		m[3][3] = 0.0f;
	}

	void orthoLH(float w, float h, float zn, float zf)
	{
		identify();
		m[0][0] = 2.0f / w;
		m[1][1] = 2.0f / h;
		m[2][2] = 1.0f / (zf - zn);
		m[3][2] = zn / (zn - zf);
	}

	void rotationX(float angle)
	{
		identify();
		m[1][1] = cosf(angle);
		m[2][2] = cosf(angle);
		m[1][2] = sinf(angle);
		m[2][1] = -sinf(angle);
	}
	void rotationY(float angle)
	{
		identify();
		m[0][0] = cosf(angle);
		m[2][2] = cosf(angle);
		m[0][2] = -sinf(angle);
		m[2][0] = sinf(angle);
	}
	void rotationZ(float angle)
	{
		identify();
		m[0][0] = cosf(angle);
		m[1][1] = cosf(angle);
		m[0][1] = sinf(angle);
		m[1][0] = -sinf(angle);
	}

	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};


inline SMatrix4x4::SMatrix4x4() { };
inline SMatrix4x4::SMatrix4x4(const float* f) { memcpy(&_11, f, sizeof(*this)); }
inline SMatrix4x4::SMatrix4x4( float f11, float f12, float f13, float f14,
				float f21, float f22, float f23, float f24,
				float f31, float f32, float f33, float f34,
				float f41, float f42, float f43, float f44 )
{
	_11 = f11; _12 = f12; _13 = f13; _14 = f14;
	_21 = f21; _22 = f22; _23 = f23; _24 = f24;
	_31 = f31; _32 = f32; _33 = f33; _34 = f34;
	_41 = f41; _42 = f42; _43 = f43; _44 = f44;
}
inline SMatrix4x4::SMatrix4x4( const SVector3& pos, const SQuaternion& rot )
{
	identify();
	m[0][0] = 1.0f - 2.0f * (rot.y * rot.y + rot.z * rot.z);
	m[0][1] = 2.0f * (rot.x * rot.y + rot.z * rot.w);
	m[0][2] = 2.0f * (rot.x * rot.z - rot.y * rot.w);
	m[1][0] = 2.0f * (rot.x * rot.y - rot.z * rot.w);
	m[1][1] = 1.0f - 2.0f * (rot.x * rot.x + rot.z * rot.z);
	m[1][2] = 2.0f * (rot.y * rot.z + rot.x * rot.w);
	m[2][0] = 2.0f * (rot.x * rot.z + rot.y * rot.w);
	m[2][1] = 2.0f * (rot.y * rot.z - rot.x * rot.w);
	m[2][2] = 1.0f - 2.0f * (rot.x * rot.x + rot.y * rot.y);
	getOrigin() = pos;
}

inline const SVector3& SMatrix4x4::getAxisX() const { return *(SVector3*)&_11; };
inline SVector3& SMatrix4x4::getAxisX() { return *(SVector3*)&_11; };
inline const SVector3& SMatrix4x4::getAxisY() const { return *(SVector3*)&_21; };
inline SVector3& SMatrix4x4::getAxisY() { return *(SVector3*)&_21; };
inline const SVector3& SMatrix4x4::getAxisZ() const { return *(SVector3*)&_31; };
inline SVector3& SMatrix4x4::getAxisZ() { return *(SVector3*)&_31; };
inline const SVector3& SMatrix4x4::getOrigin() const { return *(SVector3*)&_41; };
inline SVector3& SMatrix4x4::getOrigin() { return *(SVector3*)&_41; };

inline void SMatrix4x4::identify()
{
	m[0][1] = m[0][2] = m[0][3] =
	m[1][0] = m[1][2] = m[1][3] =
	m[2][0] = m[2][1] = m[2][3] =
	m[3][0] = m[3][1] = m[3][2] = 0.0f;

	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}
inline void SMatrix4x4::transpose( SMatrix4x4& dest ) const
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dest.m[i][j] = m[j][i];
}
inline void SMatrix4x4::transpose()
{
	const SMatrix4x4 c = *this;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = c.m[j][i];
}
inline void SMatrix4x4::invert()
{
	float det, t[3], v[16];

	t[0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
	t[1] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
	t[2] = m[1][2] * m[2][3] - m[1][3] * m[2][2];
	v[0] = m[1][1] * t[0] - m[2][1] * t[1] + m[3][1] * t[2];
	v[4] = -m[1][0] * t[0] + m[2][0] * t[1] - m[3][0] * t[2];

	t[0] = m[1][0] * m[2][1] - m[2][0] * m[1][1];
	t[1] = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	t[2] = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	v[8] = m[3][3] * t[0] - m[2][3] * t[1] + m[1][3] * t[2];
	v[12] = -m[3][2] * t[0] + m[2][2] * t[1] - m[1][2] * t[2];

	det = m[0][0] * v[0] + m[0][1] * v[4] + m[0][2] * v[8] + m[0][3] * v[12];
	if (det == 0.0f)
		return;

	t[0] = m[2][2] * m[3][3] - m[2][3] * m[3][2];
	t[1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
	t[2] = m[0][2] * m[2][3] - m[0][3] * m[2][2];
	v[1] = -m[0][1] * t[0] + m[2][1] * t[1] - m[3][1] * t[2];
	v[5] = m[0][0] * t[0] - m[2][0] * t[1] + m[3][0] * t[2];

	t[0] = m[0][0] * m[2][1] - m[2][0] * m[0][1];
	t[1] = m[3][0] * m[0][1] - m[0][0] * m[3][1];
	t[2] = m[2][0] * m[3][1] - m[3][0] * m[2][1];
	v[9] = -m[3][3] * t[0] - m[2][3] * t[1] - m[0][3] * t[2];
	v[13] = m[3][2] * t[0] + m[2][2] * t[1] + m[0][2] * t[2];

	t[0] = m[1][2] * m[3][3] - m[1][3] * m[3][2];
	t[1] = m[0][2] * m[3][3] - m[0][3] * m[3][2];
	t[2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];
	v[2] = m[0][1] * t[0] - m[1][1] * t[1] + m[3][1] * t[2];
	v[6] = -m[0][0] * t[0] + m[1][0] * t[1] - m[3][0] * t[2];

	t[0] = m[0][0] * m[1][1] - m[1][0] * m[0][1];
	t[1] = m[3][0] * m[0][1] - m[0][0] * m[3][1];
	t[2] = m[1][0] * m[3][1] - m[3][0] * m[1][1];
	v[10] = m[3][3] * t[0] + m[1][3] * t[1] + m[0][3] * t[2];
	v[14] = -m[3][2] * t[0] - m[1][2] * t[1] - m[0][2] * t[2];

	t[0] = m[1][2] * m[2][3] - m[1][3] * m[2][2];
	t[1] = m[0][2] * m[2][3] - m[0][3] * m[2][2];
	t[2] = m[0][2] * m[1][3] - m[0][3] * m[1][2];
	v[3] = -m[0][1] * t[0] + m[1][1] * t[1] - m[2][1] * t[2];
	v[7] = m[0][0] * t[0] - m[1][0] * t[1] + m[2][0] * t[2];

	v[11] = -m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) +
		m[1][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]) -
		m[2][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]);

	v[15] = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
		m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
		m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);

	det = 1.0f / det;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = v[4 * i + j] * det;
}

}; // namespace

