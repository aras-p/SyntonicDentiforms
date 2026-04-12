#include "Rendering.h"

#include "Camera.h"
#include "Demo.h"
#include "Pipelines.h"
#include "DataFiles.h"

#include <assert.h>
#include <vector>

#include "../../external/sokol_app.h"
#include "../../external/sokol_glue.h"


CLineRenderer::CLineRenderer()
{
	constexpr int kSegments = 1000;
	constexpr int kQuadSize = 6;

	// index data
	uint16_t* ib = new uint16_t[kSegments * kQuadSize];
	{
		uint16_t* pib = ib;
		for (int i = 0; i < kSegments; ++i) {
			short base = i * 2;
			pib[0] = base;
			pib[1] = base + 2;
			pib[2] = base + 1;
			pib[3] = base + 2;
			pib[4] = base + 3;
			pib[5] = base + 1;
			pib += 6;
		}
	}


	// index buffer
	sg_buffer_desc desc = {};
	desc.usage.index_buffer = true;
	desc.data.ptr = ib;
	desc.data.size = kSegments * kQuadSize * 2;
	desc.usage.immutable = true;
	mIB = sg_make_buffer(&desc);
    assert(sg_query_buffer_state(mIB) == SG_RESOURCESTATE_VALID);

	delete[] ib;
}

CLineRenderer::~CLineRenderer()
{
	sg_destroy_buffer(mIB);
}

void CLineRenderer::renderStrip( int npoints, const SLinePoint* points, float halfWidth )
{
	if( npoints < 2 )
		return;
	assert( points );

	const SMatrix4x4& camRotMat = gRenderCam.getCameraRotMatrix();
	const SMatrix4x4& camViewMat = gRenderCam.getViewMatrix();
	const SVector3& camPos = gRenderCam.getEye3();

	int nverts = npoints * 2;
	int ntris = (npoints-1)*2;

	TVertex* chunk = new TVertex[nverts];
	TVertex* vb = chunk;

	// fill VB with lines
	for (int i = 1; i < npoints; ++i) {
		const SLinePoint& ptA = points[i - 1];
		const SLinePoint& ptB = points[i];
		SVector3 segDir = ptB.pos - ptA.pos;
		SVector3 side = segDir.cross(ptB.pos - camPos);
		side.normalize();
		side *= halfWidth;
		SVector3 c1 = side;
		SVector3 c2 = -side;

		// 2 or 4 line corners
		if (i == 1) {
			vb->p = ptA.pos + c1;
			vb->diffuse = ptA.color;
			vb->tu = 0.0f;
			vb->tv = 0.0f;
			++vb;
			vb->p = ptA.pos + c2;
			vb->diffuse = ptA.color;
			vb->tu = 0.0f;
			vb->tv = 1.0f;
			++vb;
		}
		vb->p = ptB.pos + c1;
		vb->diffuse = ptB.color;
		vb->tu = 1.0f; vb->tv = 0.0f;
		++vb;
		vb->p = ptB.pos + c2;
		vb->diffuse = ptB.color;
		vb->tu = 1.0f; vb->tv = 1.0f;
		++vb;
	}

	int offset = dynamic_vb_append(chunk, nverts * sizeof(TVertex));
	delete[] chunk;

	// render
	sg_apply_uniforms(0, { &g_global_u, sizeof(g_global_u) });

	sg_bindings binds = {};
	binds.vertex_buffers[0] = dynamic_vb_get();
	binds.vertex_buffer_offsets[0] = offset;
	binds.index_buffer = mIB;
	binds.views[0] = g_data_tex[DataTexLine1]->view_tex;
	binds.samplers[0] = s_smp_linear_repeat;
	sg_apply_bindings(binds);
	sg_draw(0, ntris * 3, 1);
}

void gComputeTextureProjection(const SMatrix4x4& renderCameraMatrix, const SMatrix4x4& projectorMatrix, SMatrix4x4& dest)
{
	// | -0.5     0        0        0 |
	// | 0        0.5      0        0 |
	// | 0        0        1        0 |
	// | 0.5      0.5      0        1 |
	SMatrix4x4 matTexScale;
	matTexScale.identify();
	matTexScale._11 = 0.5f;
	matTexScale._22 = -0.5f;
	matTexScale.getOrigin().set(0.5f, 0.5f, 0.0f);

	dest = renderCameraMatrix * projectorMatrix * matTexScale;
}

const int BLOOM_PASSES = 4;

void pingPongBlur(int passes)
{
	sokol_texture* pingPong[2] = { &rt_4th_2, &rt_4th_1 };

	int swidth = sapp_width() / 4;
	int sheight = sapp_height() / 4;

	const SVector4 offsetX(1, 1, -1, -1);
	const SVector4 offsetY(1, -1, -1, 1);
	for (int i = 0; i < passes; ++i) {
		const float pixDist = i + 0.5f;
		SVector4 offset(pixDist / swidth, pixDist / sheight, 0, 0);

		sg_pass pass = {};
		pass.label = "blur step";
		pass.attachments.colors[0] = pingPong[i & 1]->view_rt;
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0, 0, 0, 0 };
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = pingPong[(i + 1) & 1]->view_tex;
		binds.samplers[0] = s_smp_linear_clamp;

		pipeline_apply(pip_postBlurStep);

		sg_apply_uniforms(0, { &offset, sizeof(offset) });

		sg_apply_bindings(binds);
		sg_draw(0, 3, 1);

		sg_end_pass();
	}
}

void renderBloom()
{
	// downsample main into smaller RT
	{
		sg_pass pass = {};
		pass.label = "downsample";
		pass.attachments.colors[0] = rt_4th_1.view_rt;
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0, 0, 0, 0 };
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = rt_main_resolved.view_tex;
		binds.samplers[0] = s_smp_linear_clamp;

		pipeline_apply(pip_blit);
		sg_apply_bindings(binds);
		sg_draw(0, 3, 1);

		sg_end_pass();
	}

	// blur
	pingPongBlur(BLOOM_PASSES);

	// add back into resolved
	{
		sg_pass pass = {};
		pass.label = "compose bloom";
		pass.attachments.colors[0] = rt_main_resolved.view_rt;
		pass.attachments.depth_stencil = {};
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_LOAD;
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = !(BLOOM_PASSES & 1) ? rt_4th_1.view_tex : rt_4th_2.view_tex;
		binds.samplers[0] = s_smp_linear_clamp;

		pipeline_apply(pip_postComposeBloom);
		sg_apply_bindings(binds);
		sg_draw(0, 3, 1);
		sg_end_pass();
	}

	// blit into swapchain backbuffer
	{
		sg_pass pass = {};
		pass.swapchain = sglue_swapchain();
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_DONTCARE;
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);
		sg_bindings binds = {};
		binds.views[0] = rt_main_resolved.view_tex;
		binds.samplers[0] = s_smp_linear_clamp;
		pipeline_apply(pip_blitToSwap);
		sg_apply_bindings(binds);
		sg_draw(0, 3, 1);
		// do not end pass; debug text will continue rendering
	}
}

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

static sg_buffer s_dynamic_buffer;

void dynamic_vb_init(size_t capacityBytes)
{
	sg_buffer_desc desc = {};
	desc.usage.vertex_buffer = true;
	desc.size = capacityBytes;
	desc.usage.immutable = false;
	desc.usage.dynamic_update = true;
	s_dynamic_buffer = sg_make_buffer(&desc);
	assert(sg_query_buffer_state(s_dynamic_buffer) == SG_RESOURCESTATE_VALID);
}

void dynamic_vb_shutdown()
{
	sg_destroy_buffer(s_dynamic_buffer);
	s_dynamic_buffer = {};
}

int dynamic_vb_append(const void* data, size_t size)
{
	int offset = sg_append_buffer(s_dynamic_buffer, { data, size });
	assert(!sg_query_buffer_overflow(s_dynamic_buffer));
	return offset;
}

sg_buffer dynamic_vb_get()
{
	return s_dynamic_buffer;
}
