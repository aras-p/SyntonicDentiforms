#pragma once

#include "src/utils/noncopyable.h"

#include "VertexFormat.h"
#include "../math/Vector3.h"
#include "../math/AABox.h"

#include "external/sokol_gfx.h"

#include <assert.h>
#include <vector>



class CMesh : public noncopyable {
public:
	/**
	 *  A subset of a mesh. Mesh can have multiple subsets, eg. when portions
	 *  of it have different materials. Contains AABB of the subset.
	 */
	class CGroup {
	public:
		CGroup( int firstVert, int vertCount, int firstPrim, int primCount )
			: mFirstVertex(firstVert), mVertexCount(vertCount), mFirstPrim(firstPrim), mPrimCount(primCount), mAABB() { }

		int getFirstVertex() const { return mFirstVertex; }
		int getVertexCount() const { return mVertexCount; }
		int getFirstPrim() const { return mFirstPrim; }
		int getPrimCount() const { return mPrimCount; }

		const CAABox& getAABB() const { return mAABB; }
		CAABox& getAABB() { return mAABB; }

	private:
		int		mFirstVertex;
		int		mVertexCount;
		int		mFirstPrim;
		int		mPrimCount;
		CAABox	mAABB;
	};

	typedef std::vector<CGroup>	TGroupVector;

public:
	CMesh(int vertCount, int idxCount, const CVertexFormat& vertFormat, int indexStride, const void* vbData, const void* ibData, int groupCount, const CGroup *groups);
	~CMesh();

	int		getVertexCount() const { return mVertexCount; }
	int		getIndexCount() const { return mIndexCount; }
	
	const CVertexFormat& getVertexFormat() const { return mVertexFormat; }
	int		getVertexStride() const { return mVertexStride; }
	int		getIndexStride() const { return mIndexStride; }

	sg_buffer getVB() { return mVB; }
	sg_buffer getIB() { return mIB; }
	
	const CAABox& getTotalAABB() const { return mTotalAABB; }

	int		getGroupCount() const { return mGroups.size(); }
	const CGroup& getGroup( int i ) const { assert(i>=0&&i<getGroupCount()); return mGroups[i]; }

private:
	// vertex/index count
	int				mVertexCount = 0;
	int				mIndexCount = 0;
	// formats/strides
	CVertexFormat	mVertexFormat = 0;
	int				mVertexStride = 0; // in bytes, depends on format
	int				mIndexStride = 0;
	// VB/IB
	sg_buffer		mVB = {};
	sg_buffer		mIB = {};
	// groups
	TGroupVector	mGroups;
	// total AABB
	CAABox			mTotalAABB = {};
};


CMesh* load_mesh(const char* fileName);
