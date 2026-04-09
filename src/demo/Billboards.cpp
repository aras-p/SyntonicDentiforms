#include "Billboards.h"
#include "DemoResources.h"
#include "DynamicVB.h"
#include "../math/Vector3.h"
#include <vector>
#include <assert.h>

struct TVertex {
	SVector3	p;
	uint32_t	diffuse;
	float		tu, tv;
};

static std::vector<SOBillboard> s_bills;
static std::vector<TVertex> s_vertices;
static sg_buffer s_ib_quads;

SOBillboard& billboards_add()
{
	s_bills.emplace_back(SOBillboard());
	return s_bills.back();
}

void billboards_clear()
{
	s_bills.clear();
}

void billboards_render()
{
	if (s_ib_quads.id == 0)
	{
		uint16_t quadIBData[6] = { 0, 1, 3, 1, 2, 3 };
		sg_buffer_desc desc = {};
		desc.usage.index_buffer = true;
		desc.data.ptr = quadIBData;
		desc.data.size = 6 * 2;
		desc.usage.immutable = true;
		s_ib_quads = sg_make_buffer(&desc);
		assert(sg_query_buffer_state(s_ib_quads) == SG_RESOURCESTATE_VALID);
	}

	if (s_bills.empty())
		return;

	// construct vertices
	size_t vertsNeeded = s_bills.size() * 4;
	if (s_vertices.size() < vertsNeeded)\
	{
		s_vertices.resize(vertsNeeded);
	}
	TVertex* vb = s_vertices.data();
	for (const SOBillboard& bill : s_bills)
	{
		const float z = 0.1f;
		vb->p.x = bill.x1; vb->p.y = bill.y1; vb->p.z = z;
		vb->diffuse = bill.color;
		vb->tu = bill.tu1; vb->tv = bill.tv1;
		++vb;
		vb->p.x = bill.x2; vb->p.y = bill.y1; vb->p.z = z;
		vb->diffuse = bill.color;
		vb->tu = bill.tu2; vb->tv = bill.tv1;
		++vb;
		vb->p.x = bill.x2; vb->p.y = bill.y2; vb->p.z = z;
		vb->diffuse = bill.color;
		vb->tu = bill.tu2; vb->tv = bill.tv2;
		++vb;
		vb->p.x = bill.x1; vb->p.y = bill.y2; vb->p.z = z;
		vb->diffuse = bill.color;
		vb->tu = bill.tu1; vb->tv = bill.tv2;
		++vb;
	}

	// fill int the GPU buffer
	int vbOffset = dynamic_vb_append(s_vertices.data(), vertsNeeded * sizeof(TVertex));

	// base bindings
	sg_bindings bind = {};
	bind.vertex_buffers[0] = dynamic_vb_get();
	bind.vertex_buffer_offsets[0] = vbOffset;
	bind.index_buffer = s_ib_quads;
	bind.samplers[0] = s_smp_linear_clamp;

	for (int i = 0; i < s_bills.size(); ++i)
	{
		const SOBillboard& b = s_bills[i];
		bind.views[0] = b.texture;
		bind.vertex_buffer_offsets[0] = vbOffset + i * sizeof(TVertex) * 4;
		sg_apply_bindings(&bind);
		sg_draw(0, 6, 1);
	}
}
