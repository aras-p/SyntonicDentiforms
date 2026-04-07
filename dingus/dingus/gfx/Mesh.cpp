// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "Mesh.h"

using namespace dingus;

CMesh::CMesh(int vertCount, int idxCount, const CVertexFormat& vertFormat, int indexStride, const void* vbData, const void* ibData, int groupCount, const CGroup* groups)
	: mGroups(groups, groups + groupCount)
{
	ASSERT_MSG(vertCount > 0, "bad vertex count");
	ASSERT_MSG(idxCount > 0, "bad index count");
	ASSERT_MSG(indexStride == 2 || indexStride == 4, "bad index stride");
	ASSERT_MSG(groupCount >= 1 && groups != nullptr, "bad group info");
	mVertexCount = vertCount;
	mIndexCount = idxCount;
	mVertexFormat = vertFormat;
	mVertexStride = vertFormat.calcVertexSize();
	ASSERT_MSG(mVertexStride > 0, "bad vertex stride");
	mIndexStride = indexStride;

	// vertex buffer
	{
		sg_buffer_desc desc = {};
		desc.usage.vertex_buffer = true;
		if (vbData)
		{
			desc.data.ptr = vbData;
			desc.data.size = mVertexCount * mVertexStride;
			desc.usage.immutable = true;
		}
		else
		{
			desc.size = mVertexCount * mVertexStride;
			desc.usage.immutable = false;
		}
		mVB = sg_make_buffer(&desc);
		ASSERT_MSG(sg_query_buffer_state(mVB) == SG_RESOURCESTATE_VALID, "Failed to create Mesh VB");
	}

	// index buffer
	{
		sg_buffer_desc desc = {};
		desc.usage.index_buffer = true;
		if (ibData)
		{
			desc.data.ptr = ibData;
			desc.data.size = mIndexCount * mIndexStride;
			desc.usage.immutable = true;
		}
		else
		{
			desc.size = mIndexCount * mIndexStride;
			desc.usage.immutable = false;
		}
		mIB = sg_make_buffer(&desc);
		ASSERT_MSG(sg_query_buffer_state(mIB) == SG_RESOURCESTATE_VALID, "Failed to create Mesh IB");
	}

	// AABBs
	mTotalAABB.setNull();
	if (mVertexFormat.hasPosition() && vbData != nullptr)
	{
		const char* vb = reinterpret_cast<const char*>(vbData);
		for (int i = 0; i < mGroups.size(); ++i)
		{
			CGroup& g = mGroups[i];
			g.getAABB().setNull();
			for (int v = 0; v < g.getVertexCount(); ++v) {
				const char* vtx = vb + mVertexStride * (v + g.getFirstVertex());
				const SVector3& pos = *reinterpret_cast<const SVector3*>(vtx);
				g.getAABB().extend(pos);
			}
			mTotalAABB.extend(g.getAABB());
		}

		// kind of HACK: if we're skinned, inflate total AABB by some amount
		if (mVertexFormat.getSkinMode() != CVertexFormat::SKIN_NONE)
		{
			SVector3 aabbSize = mTotalAABB.getMax() - mTotalAABB.getMin();
			if (aabbSize.x < aabbSize.y) aabbSize.x = aabbSize.y;
			if (aabbSize.z < aabbSize.y) aabbSize.z = aabbSize.y;
			mTotalAABB.getMin() -= aabbSize;
			mTotalAABB.getMax() += aabbSize;
		}
	}
}

CMesh::~CMesh()
{
	sg_destroy_buffer(mVB);
	mVB = {};
	sg_destroy_buffer(mIB);
	mIB = {};
}
