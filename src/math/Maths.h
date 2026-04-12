#pragma once

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>

struct SPlane;
struct SMatrix4x4;
struct SQuaternion;

inline float clamp(float value, float vmin = 0.0f, float vmax = 1.0f)
{
	assert(vmin <= vmax);
	if (value < vmin)
		value = vmin;
	else if (value > vmax)
		value = vmax;
	return value;
}

struct SVector3 {
public:
	SVector3() {}
	SVector3( const float* f ) : x(f[0]), y(f[1]), z(f[2]) {}
	SVector3( float vx, float vy, float vz ) : x(vx), y(vy), z(vz) {}

	void		set( float vx, float vy, float vz ) { x = vx; y = vy; z = vz; }
	float		length() const { return sqrtf(x * x + y * y + z * z); }
	float		lengthSq() const { return x * x + y * y + z * z; }
	float		dot( const SVector3& v ) const { return x * v.x + y * v.y + z * v.z; }
	void		normalize();

	SVector3	cross( const SVector3& v ) const
	{
		SVector3 res;
		res.x = y * v.z - z * v.y;
		res.y = z * v.x - x * v.z;
		res.z = x * v.y - y * v.x;
		return res;
	}
	SVector3	getNormalized() const;

	friend SVector3 operator+(const SVector3& a, const SVector3& b)
	{
		return SVector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	friend SVector3 operator-(const SVector3& a, const SVector3& b)
	{
		return SVector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}
	friend SVector3 operator-(const SVector3& a)
	{
		return SVector3(-a.x, -a.y, -a.z);
	}
	friend SVector3 operator*(const SVector3& a, float b)
	{
		return SVector3(a.x * b, a.y * b, a.z * b);
	}

	void operator+=(const SVector3& o)
	{
		x += o.x;
		y += o.y;
		z += o.z;
	}
	void operator-=(const SVector3& o)
	{
		x -= o.x;
		y -= o.y;
		z -= o.z;
	}
	void operator*=(float o)
	{
		x *= o;
		y *= o;
		z *= o;
	}

	SVector3 transformCoord(const SMatrix4x4& pm) const;

	float x, y, z;
};


inline void	SVector3::normalize()
{
	float norm = length();
	if (norm)
	{
		x /= norm;
		y /= norm;
		z /= norm;
	}
	else
	{
		x = y = z = 0.0f;
	}
}

inline SVector3 SVector3::getNormalized() const
{
	SVector3 v;
	float norm = length();
	if (norm)
	{
		v.x = x / norm;
		v.y = y / norm;
		v.z = z / norm;
	}
	else
	{
		v.x = v.y = v.z = 0.0f;
	}
	return v;
}

class CAABox {
public:
	static constexpr float kBigFloat = 1.0e30f;
	CAABox()
		: mMin(kBigFloat, kBigFloat, kBigFloat),
		mMax(-kBigFloat, -kBigFloat, -kBigFloat) {
	}
	void setNull() {
		mMin.x = mMin.y = mMin.z = kBigFloat;
		mMax.x = mMax.y = mMax.z = -kBigFloat;
	}

	const SVector3& getMin() const { return mMin; }
	SVector3& getMin() { return mMin; }
	const SVector3& getMax() const { return mMax; }
	SVector3& getMax() { return mMax; }

	void	extend(const SVector3& p)
	{
		if (p.x < mMin.x) mMin.x = p.x;
		if (p.y < mMin.y) mMin.y = p.y;
		if (p.z < mMin.z) mMin.z = p.z;
		if (p.x > mMax.x) mMax.x = p.x;
		if (p.y > mMax.y) mMax.y = p.y;
		if (p.z > mMax.z) mMax.z = p.z;
	}

	// Culls this AABB, transformed by the given world matrix, against frustum planes
	// (extracted from view*projection matrix).
	bool frustumCull(const SMatrix4x4& world, const SPlane planes[6]) const;

private:
	SVector3	mMin;
	SVector3	mMax;
};


class SLine3 {
public:
	SLine3() {};
	SLine3(const SVector3& v0, const SVector3& v1) : pos(v0), vec(v1 - v0) {};

	void set(const SVector3& v0, const SVector3& v1) {
		pos = v0;
		vec = v1 - v0;
	}
	const SVector3& getStart() const { return pos; }
	const SVector3& getVector() const { return vec; }
	SVector3	getEnd() const { return (pos + vec); };
	float		length() const { return vec.length(); };
	float		lengthSq() const { return vec.lengthSq(); }

	/** Distance from point to line. */
	float distance(const SVector3& p) const {
		SVector3 diff(p - pos);
		float l = vec.lengthSq();
		if (l > 1.0e-6f) {
			float t = vec.dot(diff) / l;
			diff = diff - vec * t;
			return diff.length();
		}
		else {
			// line is really a point...
			SVector3 v(p - pos);
			return v.length();
		}
	};

	/** Returns parametric point projection onto this line. */
	float project(const SVector3& p) const {
		SVector3 diff(p - pos);
		float l = vec.lengthSq();
		assert(l > 1.0e-6f);
		return vec.dot(diff) / l;
	}

	SVector3 interpolate(float t) const { return pos + vec * t; }

public:
	SVector3	pos;
	SVector3	vec;
};

struct SMatrix4x4 {
public:
	SMatrix4x4();
	SMatrix4x4(const float* f);
	SMatrix4x4(float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44);
	SMatrix4x4(const SVector3& pos, const SQuaternion& rot);

	const SVector3& getAxisX() const;
	SVector3& getAxisX();
	const SVector3& getAxisY() const;
	SVector3& getAxisY();
	const SVector3& getAxisZ() const;
	SVector3& getAxisZ();
	const SVector3& getOrigin() const;
	SVector3& getOrigin();

	void	identify();
	void	invert();

	friend SMatrix4x4 operator* (const SMatrix4x4& a, const SMatrix4x4& b);

	void perspectiveFovLH(float fovy, float aspect, float zn, float zf);

	void orthoLH(float w, float h, float zn, float zf);

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


inline SMatrix4x4::SMatrix4x4() {};
inline SMatrix4x4::SMatrix4x4(const float* f) { memcpy(&_11, f, sizeof(*this)); }
inline SMatrix4x4::SMatrix4x4(float f11, float f12, float f13, float f14,
	float f21, float f22, float f23, float f24,
	float f31, float f32, float f33, float f34,
	float f41, float f42, float f43, float f44)
{
	_11 = f11; _12 = f12; _13 = f13; _14 = f14;
	_21 = f21; _22 = f22; _23 = f23; _24 = f24;
	_31 = f31; _32 = f32; _33 = f33; _34 = f34;
	_41 = f41; _42 = f42; _43 = f43; _44 = f44;
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

class SPlane {
public:
	SPlane() {}
	SPlane(float aa, float bb, float cc, float dd)
		: a(aa), b(bb), c(cc), d(dd)
	{

	}
	SPlane(const SVector3& p, const SVector3& n)
	{
		a = n.x;
		b = n.y;
		c = n.z;
		d = -p.dot(n);
	}

	SPlane normalize() const
	{
		float norm = sqrtf(a * a + b * b + c * c);
		if (norm)
		{
			return SPlane(a / norm, b / norm, c / norm, d / norm);
		}
		else
		{
			return SPlane(0, 0, 0, 0);
		}
	}

	float a = 0, b = 0, c = 0, d = 1;
};

void extractFrustumPlanes(const SMatrix4x4& viewProj, SPlane planes[6]);

struct SQuaternion {
public:
	SQuaternion() {}
	SQuaternion(const float* f) : x(f[0]), y(f[1]), z(f[2]), w(f[3]) {}
	SQuaternion(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}

	float	dot(const SQuaternion& q) const { return x * q.x + y * q.y + z * q.z + w * q.w; }
	void	identify() { x = y = z = 0.0f; w = 1.0f; }
	void	inverse()
	{
		float norm = lengthSq();
		x = -x / norm;
		y = -y / norm;
		z = -z / norm;
		w = w / norm;
	}
	bool	isIdentity() const { return x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f; }
	float	length() const { return sqrtf(x * x + y * y + z * z + w * w); }
	float	lengthSq() const { return x * x + y * y + z * z + w * w; }
	void	normalize()
	{
		float norm = lengthSq();
		x = x / norm;
		y = y / norm;
		z = z / norm;
		w = w / norm;
	}
	void	slerp(const SQuaternion& a, const SQuaternion& b, float t);

	float x, y, z, w;
};

struct SMatrix4x4;

struct SVector4 {
public:
	SVector4() {};
	SVector4(const float* f) : x(f[0]), y(f[1]), z(f[2]), w(f[3]) {};
	SVector4(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}
	SVector4(const SVector3& v) : x(v.x), y(v.y), z(v.z), w(1.0f) {}

	void		set(float vx, float vy, float vz, float vw);

	uint32_t toRGBA() const
	{
		uint32_t dwR = x >= 1.0f ? 0xff : x <= 0.0f ? 0x00 : (uint32_t)(x * 255.0f + 0.5f);
		uint32_t dwG = y >= 1.0f ? 0xff : y <= 0.0f ? 0x00 : (uint32_t)(y * 255.0f + 0.5f);
		uint32_t dwB = z >= 1.0f ? 0xff : z <= 0.0f ? 0x00 : (uint32_t)(z * 255.0f + 0.5f);
		uint32_t dwA = w >= 1.0f ? 0xff : w <= 0.0f ? 0x00 : (uint32_t)(w * 255.0f + 0.5f);

		return (dwA << 24) | (dwB << 16) | (dwG << 8) | dwR;
	}

	float x, y, z, w;
};

inline void SVector4::set(float vx, float vy, float vz, float vw) { x = vx; y = vy; z = vz; w = vw; };

