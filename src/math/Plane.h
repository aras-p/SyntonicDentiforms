#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

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
            return SPlane(0,0,0,0);
        }
	}

	float a = 0, b = 0, c = 0, d = 1;
};

inline void extractFrustumPlanes(const SMatrix4x4& viewProj, SPlane planes[6])
{
	planes[0] = SPlane(viewProj.m[0][0]+viewProj.m[0][3], viewProj.m[1][0]+viewProj.m[1][3], viewProj.m[2][0]+viewProj.m[2][3], viewProj.m[3][0]+viewProj.m[3][3]);	// left
	planes[1] = SPlane(viewProj.m[0][3]-viewProj.m[0][0], viewProj.m[1][3]-viewProj.m[1][0], viewProj.m[2][3]-viewProj.m[2][0], viewProj.m[3][3]-viewProj.m[3][0]);	// right
	planes[2] = SPlane(viewProj.m[0][1]+viewProj.m[0][3], viewProj.m[1][1]+viewProj.m[1][3], viewProj.m[2][1]+viewProj.m[2][3], viewProj.m[3][1]+viewProj.m[3][3]);	// bottom
	planes[3] = SPlane(viewProj.m[0][3]-viewProj.m[0][1], viewProj.m[1][3]-viewProj.m[1][1], viewProj.m[2][3]-viewProj.m[2][1], viewProj.m[3][3]-viewProj.m[3][1]);	// top
	planes[4] = SPlane(viewProj.m[0][2],                  viewProj.m[1][2],                  viewProj.m[2][2],                  viewProj.m[3][2]                 ); // near
	planes[5] = SPlane(viewProj.m[0][3]-viewProj.m[0][2], viewProj.m[1][3]-viewProj.m[1][2], viewProj.m[2][3]-viewProj.m[2][2], viewProj.m[3][3]-viewProj.m[3][2]); // far
}
