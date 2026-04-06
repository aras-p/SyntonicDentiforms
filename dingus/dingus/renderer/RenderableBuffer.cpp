// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "RenderableBuffer.h"
#include "RenderContext.h"
#include "../kernel/D3DDevice.h"

using namespace dingus;


// --------------------------------------------------------------------------
//  CAbstractRenderableBuffer
// --------------------------------------------------------------------------

CAbstractRenderableBuffer::CAbstractRenderableBuffer( const SVector3* origin, int priority )
:	CRenderable( origin, priority ),
	mPrimType( D3DPT_TRIANGLELIST ),
	mPrimCount( 0 )
{
	mLargestActiveStream = VERTEX_STREAM_COUNT - 1;
	resetVBs();
	mLargestActiveStream = -1;
}

void CAbstractRenderableBuffer::resetVBs()
{
	for( int q = 0; q <= mLargestActiveStream; ++q ) {
		mVB[q] = NULL;
		mStride[q] = 0;
		mByteOffset[q] = 0;
	}
	mLargestActiveStream = -1;
	mVertexDecl = NULL;
}

void CAbstractRenderableBuffer::setVB( CD3DVertexBuffer& vb, int stream )
{
	assert( stream >= 0 && stream < VERTEX_STREAM_COUNT );
	mVB[stream] = &vb;
	if( stream > mLargestActiveStream )
		mLargestActiveStream = stream;
}

void CAbstractRenderableBuffer::applyStreams()
{
	CD3DDevice& device = CD3DDevice::getInstance();

	for( int q = 0; q <= mLargestActiveStream; ++q ) {
		assert( mStride[q] > 0 );
		device.setVertexBuffer( q, mVB[q], mByteOffset[q], mStride[q] );
	}

	if( mVertexDecl )
		device.setDeclaration( *mVertexDecl );
}

void CAbstractRenderableBuffer::unapplyStreams()
{
	/*
	CD3DDevice& device = CD3DDevice::getInstance();
	for( int q = 0; q <= mLargestActiveStream; ++q )
		device.setVertexBuffer( q, NULL, 0, 0 );
		*/
}


// --------------------------------------------------------------------------
//  CRenderableIndexedBuffer
// --------------------------------------------------------------------------

CRenderableIndexedBuffer::CRenderableIndexedBuffer( const SVector3* origin, int priority )
:	CAbstractRenderableBuffer( origin, priority ),
	mIB( NULL ),
	mBaseVertex( 0 ),
	mMinVertex( 0 ),
	mNumVertices( 0 ),
	mStartIndex( 0 )
{
}

void CRenderableIndexedBuffer::render( CRenderContext const& ctx )
{
	if( getLargestActiveStream() < 0 )
		return;
	if( getPrimCount() == 0 )
		return;

	assert( mIB );

	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();

	applyStreams();

	device.setIndexBuffer( mIB );

	dx.DrawIndexedPrimitive(
		getPrimType(), mBaseVertex, mMinVertex, mNumVertices, mStartIndex, getPrimCount() );

	unapplyStreams();

	//device.setIndexBuffer( NULL );
}
