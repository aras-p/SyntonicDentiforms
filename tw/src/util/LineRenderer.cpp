#include "stdafx.h"

#include "LineRenderer.h"
#include <dingus/gfx/geometry/DynamicVBManager.h>



CLineRenderer::CLineRenderer( CD3DIndexBuffer& ib )
{
	mRenderable = new CRenderableIndexedBuffer();
	mRenderable->setIB( ib );
}

CLineRenderer::~CLineRenderer()
{
	delete mRenderable;
}


void CLineRenderer::renderStrip( int npoints, const SLinePoint* points, float halfWidth )
{
	if( npoints < 2 )
		return;
	assert( points );

	const SMatrix4x4& camRotMat = G_RCTX->getCamera().getCameraRotMatrix();
	const SMatrix4x4& camViewMat = G_RCTX->getCamera().getViewMatrix();
	const SVector3& camPos = G_RCTX->getCamera().getEye3();

	int nverts = npoints * 2;
	int ntris = (npoints-1)*2;
	CVBChunk::TSharedPtr chunk = CDynamicVBManager::getInstance().allocateChunk( nverts, sizeof(TVertex) );
	assert( chunk );
	TVertex* vb = reinterpret_cast<TVertex*>( chunk->getData() );

	// fill VB with lines
	for( int i = 1; i < npoints; ++i ) {
		const SLinePoint& ptA = points[i-1];
		const SLinePoint& ptB = points[i];
		//SVector3 camA, camB;
		//D3DXVec3TransformCoord( &camA, &ptA.pos, &camViewMat );
		//D3DXVec3TransformCoord( &camB, &ptB.pos, &camViewMat );

		/*
		// line direction on XZ plane
		D3DXVECTOR2 lineDir( camB.x-camA.x, camB.y-camA.y );
		D3DXVec2Normalize( &lineDir, &lineDir );

		// perpendicular to line direction vector, scaled by halfWidth
		float width2 = -lineDir.y * halfWidth;
		float height2 = lineDir.x * halfWidth;

		// transform by camera
		SVector3 corner1( width2, height2, 0.0f );
		SVector3 corner2( -width2, -height2, 0.0f );
		SVector3 c1, c2;
		D3DXVec3TransformCoord( &c1, &corner1, &camRotMat );
		D3DXVec3TransformCoord( &c2, &corner2, &camRotMat );
		*/
		SVector3 segDir = ptB.pos - ptA.pos;
		SVector3 side = segDir.cross( ptB.pos - camPos );
		side.normalize();
		side *= halfWidth;
		SVector3 c1 = side;
		SVector3 c2 = -side;
		
		// 2 or 4 line corners
		if( i==1 ) {
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
	chunk->unlock();

	// render
	mRenderable->setVB( chunk->getVB(), 0 );
	mRenderable->setStride( chunk->getStride(), 0 );

	mRenderable->setBaseVertex( chunk->getOffset() );
	mRenderable->setMinVertex( 0 );
	mRenderable->setNumVertices( chunk->getSize() );
	mRenderable->setStartIndex( 0 );
	mRenderable->setPrimCount( ntris );
	mRenderable->setPrimType( D3DPT_TRIANGLELIST );

	G_RCTX->directRender( *mRenderable );
}
