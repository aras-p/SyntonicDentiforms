#pragma once

#include "../math/Vector3.h"
#include <stdint.h>

namespace dingus {

#pragma pack(1)

// Vertex: position, color, UV.
struct SVertexXyzDiffuseTex1 {
	SVector3	p;
	uint32_t	diffuse;
	float		tu, tv;
};

#pragma pack()


}; // namespace


