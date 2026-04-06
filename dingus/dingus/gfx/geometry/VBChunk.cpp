// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../../stdafx.h"
#pragma hdrstop

#include "VBChunk.h"

using namespace dingus;



CVBChunk::CVBChunk( CD3DVertexBuffer& vb, byte* data, int offset, int count, int stride )
:	mVB( &vb ),
	mData( data ),
	mOffset( offset ),
	mCount( count ),
	mStride( stride ),
	mValid( false )
{
}

void CVBChunk::unlock( int count )
{
	if( mCount )
		mVB->getObject()->Unlock();
	
	mData = NULL;
	mValid = true;

	if( !count )
		return;

	assert( count > 0 );
	mCount = count;
}

