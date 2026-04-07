#include "stdafx.h"

#include "LineRenderer.h"
#include <dingus/gfx/geometry/DynamicVBManager.h>

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
	ASSERT_MSG(sg_query_buffer_state(mIB) == SG_RESOURCESTATE_VALID, "Failed to create Lines IB");

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
	//@TODO
	/*
	mRenderable->setVB( chunk->getVB(), 0 );
	mRenderable->setStride( chunk->getStride(), 0 );

	mRenderable->setBaseVertex( chunk->getOffset() );
	mRenderable->setMinVertex( 0 );
	mRenderable->setNumVertices( chunk->getSize() );
	mRenderable->setStartIndex( 0 );
	mRenderable->setPrimCount( ntris );
	mRenderable->setPrimType( D3DPT_TRIANGLELIST );

	G_RCTX->directRender( *mRenderable );
	*/
}
