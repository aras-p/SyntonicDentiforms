#include "Billboards.h"
#include "../gfx/DynamicVBManager.h"

#include <assert.h>


CBillboards::CBillboards(sg_buffer ib, sg_sampler sampler)
:	mIB( ib ),
	mSampler(sampler)
{
    assert(ib.id != 0);
    assert(sampler.id != 0);
}

void CBillboards::renderBills()
{
	if (mBills.empty())
		return;
	
	// render into VB
	std::vector<TVertex> vbData(mBills.size() * 4); // 4 verts per billboard
	TVertex* vb = vbData.data();
	TBillVector::const_iterator bit, bitEnd = mBills.end();
	for( bit = mBills.begin(); bit != bitEnd; ++bit ) {
		const SOBillboard& bill = *bit;
		vb->p.x		= bill.x1; vb->p.y = bill.y1; vb->p.z = 0.1f; // TBD
		vb->diffuse	= bill.color;
		vb->tu		= bill.tu1; vb->tv = bill.tv1;
		++vb;
		vb->p.x		= bill.x2; vb->p.y = bill.y1; vb->p.z = 0.1f; // TBD
		vb->diffuse	= bill.color;
		vb->tu		= bill.tu2; vb->tv = bill.tv1;
		++vb;
		vb->p.x		= bill.x2; vb->p.y = bill.y2; vb->p.z = 0.1f; // TBD
		vb->diffuse	= bill.color;
		vb->tu		= bill.tu2; vb->tv = bill.tv2;
		++vb;
		vb->p.x		= bill.x1; vb->p.y = bill.y2; vb->p.z = 0.1f; // TBD
		vb->diffuse	= bill.color;
		vb->tu		= bill.tu1; vb->tv = bill.tv2;
		++vb;
	}

	int vbOffset = dynamic_vb_append(vbData.data(), vbData.size() * sizeof(vbData[0]));

	// set IB/VB
	sg_bindings bind = {};
	bind.vertex_buffers[0] = dynamic_vb_get();
	bind.vertex_buffer_offsets[0] = vbOffset;
	bind.index_buffer = mIB;
	bind.samplers[0] = mSampler;

	for (int i = 0; i < mBills.size(); ++i)
	{
		const SOBillboard& b = mBills[i];
		bind.views[0] = b.texture;
		bind.vertex_buffer_offsets[0] = vbOffset + i * sizeof(TVertex) * 4;
		sg_apply_bindings(&bind);
		sg_draw(0, 6, 1);
	}
}
