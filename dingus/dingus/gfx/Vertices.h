// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __VERTICES_H
#define __VERTICES_H

#include "../math/Vector3.h"


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


#endif
