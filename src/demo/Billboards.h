#pragma once

#include "../math/Vector3.h"
#include "external/sokol_gfx.h"
#include <vector>

struct SOBillboard {
public:
	void	setWholeTexture() { tu1=tv1=0; tu2=tv2=1; }
public:
	float			x1, y1, x2, y2;
	float			tu1, tv1, tu2, tv2;
	uint32_t        color;
	sg_view			texture;
};

class CBillboards {
public:
	CBillboards(sg_buffer ib, sg_sampler sampler);
	
	SOBillboard& addBill() { mBills.push_back(SOBillboard()); return mBills.back(); };
	void clear() { mBills.clear(); }
	void renderBills();

private:
	struct TVertex {
		SVector3	p;
		uint32_t	diffuse;
		float		tu, tv;
	};
	typedef std::vector<SOBillboard>	TBillVector;

private:
	sg_buffer mIB;
	sg_sampler mSampler;
	TBillVector mBills;
};
