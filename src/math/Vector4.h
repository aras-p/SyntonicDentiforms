#pragma once

#include "Vector3.h"
#include <stdint.h>

struct SMatrix4x4;

// --------------------------------------------------------------------------

/**
 *  4D vector.
 */
struct SVector4 {
public:
	SVector4() {};
	SVector4(const float* f) : x(f[0]), y(f[1]), z(f[2]), w(f[3]) {};
	SVector4(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}
	SVector4(const SVector3& v) : x(v.x), y(v.y), z(v.z), w(1.0f) {}

	void		set( float vx, float vy, float vz, float vw );

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


inline void SVector4::set( float vx, float vy, float vz, float vw ) { x=vx; y=vy; z=vz; w=vw; };

void SVector4TransformArray(SVector4* out, const SVector4* in, const SMatrix4x4* matrix, int elements);

