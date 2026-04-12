#pragma once

#include "../../external/sokol_gfx.h"

struct SOBillboard {
public:
	void	setWholeTexture() { tu1=tv1=0; tu2=tv2=1; }
public:
	float			x1, y1, x2, y2;
	float			tu1, tv1, tu2, tv2;
	uint32_t        color;
	sg_view			texture;
};

SOBillboard& billboards_add();
void billboards_clear();
void billboards_render();
