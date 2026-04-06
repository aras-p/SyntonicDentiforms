#ifndef __LINE_RENDERER_H
#define __LINE_RENDERER_H

#include <dingus/gfx/Vertices.h>
#include <dingus/renderer/RenderableBuffer.h>
#include <dingus/resource/IndexBufferBundle.h>


struct SLinePoint {
	SVector3	pos;
	D3DCOLOR	color;
};

class CLineRenderer {
public:
	CLineRenderer( CD3DIndexBuffer& ib );
	~CLineRenderer();

	CRenderable&	getRenderable() { return *mRenderable; }

	void	renderStrip( int npoints, const SLinePoint* points, float halfWidth );

private:
	typedef SVertexXyzDiffuseTex1	TVertex;
	CRenderableIndexedBuffer*		mRenderable;
};


class CIBFillerQuadStrip : public IIndexBufferFiller {
public:
	virtual void fillIB( CD3DIndexBuffer& ib, int elements, D3DFORMAT format ) {
		assert( format == D3DFMT_INDEX16 );
		assert( elements % 6 == 0 );
		int n = elements / 6;
		short* pib = NULL;
		ib.getObject()->Lock( 0, elements*2, (void**)&pib, 0 );
		assert( &ib );
		for( int i = 0; i < n; ++i ) {
			short base = i * 2;
			pib[0] = base;
			pib[1] = base+2;
			pib[2] = base+1;
			pib[3] = base+2;
			pib[4] = base+3;
			pib[5] = base+1;
			pib += 6;
		}
		ib.getObject()->Unlock();
	}
};

#endif
