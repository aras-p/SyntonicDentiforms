// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#pragma once

namespace dingus {

struct SQuaternion {
public:
	SQuaternion() { }
	SQuaternion( const float* f ) : x(f[0]), y(f[1]), z(f[2]), w(f[3]) { }
	SQuaternion( float xx, float yy, float zz, float ww ) : x(xx), y(yy), z(zz), w(ww) { }

	float	dot( const SQuaternion& q ) const { return x * q.x + y * q.y + z * q.z + w * q.w; }
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
	void	slerp(const SQuaternion& a, const SQuaternion& b, float t)
	{
		float temp = 1.0f - t;
		float dot = a.dot(b);
		if (dot < 0.0f)
		{
			t = -t;
			dot = -dot;
		}

		if (1.0f - dot > 0.001f)
		{
			float theta = acosf(dot);
			temp = sinf(theta * temp) / sinf(theta);
			t = sinf(theta * t) / sinf(theta);
		}

		x = temp * a.x + t * b.x;
		y = temp * a.y + t * b.y;
		z = temp * a.z + t * b.z;
		w = temp * a.w + t * b.w;
	}

	float x, y, z, w;
};

}; // namespace
