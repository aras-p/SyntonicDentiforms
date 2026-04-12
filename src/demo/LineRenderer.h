#pragma once

#include <stdint.h>
#include "../math/Maths.h"
#include "../../external/sokol_gfx.h"

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
	struct TVertex {
		SVector3	p;
		uint32_t	diffuse;
		float		tu, tv;
	};
	sg_buffer mIB;
};
