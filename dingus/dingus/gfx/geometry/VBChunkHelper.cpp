// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../../stdafx.h"
#pragma hdrstop

#include "VBChunkHelper.h"

using namespace dingus;



byte* CVBChunkHelper::lock( int count )
{
	// unlock previous chunk?
	if( mCurrentChunkLocked )
		unlock();
	
	mCurrentChunkLocked = true;
	
	// lock new chunk
	TSharedChunk chunk = mSource->lock( count );
	assert( chunk.get() );
	
	// collect chunks for rendering
	mChunks.push_back( chunk );

	return chunk->getData();
} 
