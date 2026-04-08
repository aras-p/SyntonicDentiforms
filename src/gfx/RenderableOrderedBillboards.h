// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#pragma once

#include "external/sokol_gfx.h"
#include <vector>

namespace dingus {

struct SVertexXyzDiffuseTex1;
struct SMatrix4x4;


struct SOBillboard {
public:
	void	setWholeTexture() { tu1=tv1=0; tu2=tv2=1; }
public:
	float			x1, y1, x2, y2;
	float			tu1, tv1, tu2, tv2;
	uint32_t        color;
	sg_view			texture;
};


/**
 *  Billboard renderer with order-preserving.
 *
 *  Renders billboards (aka sprites, or screen-aligned textured
 *  quads). Preserves billboard submitting order - this may be not very
 *  efficient if billboards use different textures.
 */
class CRenderableOrderedBillboards {
public:
	CRenderableOrderedBillboards(sg_buffer ib, sg_sampler sampler);
	virtual ~CRenderableOrderedBillboards();
	
	SOBillboard& addBill() { mBills.push_back(SOBillboard()); return mBills.back(); };
	void clear() { mBills.clear(); }

	void render();

private:
	typedef SVertexXyzDiffuseTex1		TVertex;
	typedef std::vector<SOBillboard>	TBillVector;

private:
	sg_buffer mIB;
	sg_sampler mSampler;
	TBillVector mBills;
};


}; // namespace
