#include "Vector3.h"
#include "Matrix4x4.h"


SVector3 SVector3::transformCoord(const SMatrix4x4& pm) const
{
	SVector3 out;
	float norm = pm.m[0][3] * x + pm.m[1][3] * y + pm.m[2][3] * z + pm.m[3][3];

	out.x = (pm.m[0][0] * x + pm.m[1][0] * y + pm.m[2][0] * z + pm.m[3][0]) / norm;
	out.y = (pm.m[0][1] * x + pm.m[1][1] * y + pm.m[2][1] * z + pm.m[3][1]) / norm;
	out.z = (pm.m[0][2] * x + pm.m[1][2] * y + pm.m[2][2] * z + pm.m[3][2]) / norm;
	return out;
}
