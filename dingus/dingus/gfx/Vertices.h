// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __VERTICES_H
#define __VERTICES_H

#include "../math/Vector3.h"


namespace dingus {

#pragma pack(1)

/// Vertex format: position, color, UV.
const DWORD FVF_XYZ_DIFFUSE_TEX1 = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
/// Vertex: position, color, UV.
struct SVertexXyzDiffuseTex1 {
	SVector3	p;
	D3DCOLOR	diffuse;
	float		tu, tv;
};

#pragma pack()


}; // namespace


#endif
