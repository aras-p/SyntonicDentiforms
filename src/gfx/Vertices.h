#pragma once

#include "../math/Vector3.h"
#include <stdint.h>

struct SVertexXyzDiffuseTex1 {
	SVector3	p;
	uint32_t	diffuse;
	float		tu, tv;
};
