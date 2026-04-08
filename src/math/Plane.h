#pragma once

#include "Vector3.h"

namespace dingus {

class SPlane {
public:
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

}; // namespace
