// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __VB_MANAGER_H
#define __VB_MANAGER_H

#include "VBChunk.h"
#include "DynamicVBManager.h"


namespace dingus {


class CVBManagerSource : public IChunkSource<CVBChunk> {
public:
	CVBManagerSource( int stride ) : mStride(stride) { }

	virtual SHARED_CHUNK lock( int count ) { return CDynamicVBManager::getInstance().allocateChunk( count, mStride ); }

private:
	int					mStride;
};

}; // namespace

#endif
