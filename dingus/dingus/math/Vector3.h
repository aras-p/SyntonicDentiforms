// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#pragma once

namespace dingus {

struct SMatrix4x4;


// --------------------------------------------------------------------------

/**
 *  3D vector.
 */
struct SVector3 {
public:
	SVector3();
	SVector3( const float* f );
	SVector3( float x, float y, float z );

	void		set( float vx, float vy, float vz );
	float		length() const;
	float		lengthSq() const;
	float		dot( const SVector3& v ) const;
	void		normalize();

	SVector3	cross( const SVector3& v ) const;
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


inline SVector3::SVector3()  { };
inline SVector3::SVector3( const float *f ) : x(f[0]), y(f[1]), z(f[2]) { };
inline SVector3::SVector3( float vx, float vy, float vz ) : x(vx), y(vy), z(vz) { };

inline void SVector3::set( float vx, float vy, float vz ) { x=vx; y=vy; z=vz; };
inline float SVector3::length() const { return sqrtf(x * x + y * y + z * z); };
inline float SVector3::lengthSq() const { return x * x + y * y + z * z; };
inline float SVector3::dot( const SVector3& v ) const { return x * v.x + y * v.y + z * v.z; }
inline SVector3 SVector3::cross( const SVector3& v ) const {
	SVector3 res;
	res.x = y * v.z - z * v.y;
    res.y = z * v.x - x * v.z;
    res.z = x * v.y - y * v.x;
	return res;
}
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

}; // namespace
