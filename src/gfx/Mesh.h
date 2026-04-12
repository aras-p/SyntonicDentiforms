#pragma once

#include "../math/Maths.h"

#include "../../external/sokol_gfx.h"

#include <assert.h>

class CMesh {
public:
	struct Vertex
	{
		float x, y, z; // position
		uint32_t n; // normal, 2xSNorm16 octahedral
	};
	static_assert(sizeof(Vertex) == 16, "Mesh Vertex needs to be 16 bytes");

public:
	CMesh(int vertCount, int idxCount, const Vertex* vbData, const uint16_t* ibData);
	~CMesh();

	int		getVertexCount() const { return mVertexCount; }
	int		getIndexCount() const { return mIndexCount; }
	
	sg_buffer getVB() { return mVB; }
	sg_buffer getIB() { return mIB; }
	
	const CAABox& getTotalAABB() const { return mTotalAABB; }

private:
	int				mVertexCount = 0;
	int				mIndexCount = 0;
	sg_buffer		mVB = {};
	sg_buffer		mIB = {};
	CAABox			mTotalAABB = {};
};

CMesh* load_mesh(const char* fileName);
