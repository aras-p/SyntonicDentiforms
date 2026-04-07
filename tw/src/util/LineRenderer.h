#pragma once

#include <dingus/gfx/Vertices.h>
#include "../external/sokol_gfx.h"


struct SLinePoint {
	SVector3	pos;
	uint32_t    color;
};

class CLineRenderer {
public:
	CLineRenderer();
	~CLineRenderer();

	void	renderStrip(int npoints, const SLinePoint* points, float halfWidth);

private:
	typedef SVertexXyzDiffuseTex1	TVertex;
	sg_buffer mIB;
};
