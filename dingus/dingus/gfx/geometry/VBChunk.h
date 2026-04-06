// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __VB_CHUNK_H
#define __VB_CHUNK_H

#include "../../kernel/Proxies.h"

namespace dingus {


class CVBChunk : public CRefCounted {
public:
	typedef intrusive_ptr<CVBChunk>	TSharedPtr;

public:
	CVBChunk( CD3DVertexBuffer& vb, byte* data, int offset, int count, int stride );
	
	void unlock( int count = 0 );
	byte* getData() const { return mData; }
	int getOffset() const { return mOffset; }
	int getSize() const { return mCount; }
	bool isValid() const { return mValid; }
	int getStride() const { return mStride; }
	CD3DVertexBuffer& getVB() const { return *mVB; }
	
	void invalidate() { mValid = false; }

private:
	CD3DVertexBuffer*	mVB;
	byte*		mData;
	int			mOffset;
	int			mCount;
	int			mStride;

	bool		mValid;	
};


}; // namespace


IMPLEMENT_INTRUSIVE_PTR(dingus::CVBChunk);

#endif
