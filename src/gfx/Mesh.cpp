#include "Mesh.h"
#include "src/utils/AssertHelper.h"
#include <vector>


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


#define READ_4BYTE(var) fread(&var,1,4,f)
#define READ_2BYTE(var) fread(&var,1,2,f)

CMesh* load_mesh(const char* fileName)
{
	// open file
	FILE* f = fopen(fileName, "rb");
	if (!f) return nullptr;

	//
	// read header

	// magic
	char magic[4];
	READ_4BYTE(magic);
	if (magic[0] != 'D' || magic[1] != 'M' || magic[2] != 'S' || magic[3] != 'H') {
		fclose(f);
		return nullptr;
	}
	// header
	int nverts, ntris, ngroups, vstride, istride, vformat;
	READ_4BYTE(nverts);
	READ_4BYTE(ntris);
	READ_4BYTE(ngroups);
	READ_4BYTE(vstride);
	READ_4BYTE(vformat);
	READ_4BYTE(istride);
	assert(nverts > 0);
	assert(ntris > 0);
	assert(ngroups > 0);
	assert(vstride > 0);
	assert(istride == 2 || istride == 4);
	assert(vformat != 0);

	// read data
	std::vector<uint8_t> vbData(vstride * nverts);
	fread(vbData.data(), vstride, nverts, f);

	std::vector<uint8_t> ibData(istride * ntris * 3);
	fread(ibData.data(), istride, ntris * 3, f);

	std::vector<CMesh::CGroup> groups;
	for (int i = 0; i < ngroups; ++i) {
		int vstart, vcount, fstart, fcount;
		READ_4BYTE(vstart);
		READ_4BYTE(vcount);
		READ_4BYTE(fstart);
		READ_4BYTE(fcount);
		// HACK: TBD: for null groups
		if (ngroups == 1 && vcount == 0 && fcount == 0) {
			vstart = 0;
			vcount = nverts;
			fstart = 0;
			fcount = ntris;
		}
		groups.emplace_back(CMesh::CGroup(vstart, vcount, fstart, fcount));
	}

	// close file
	fclose(f);
	// HACK: some of demo meshes have position + normal + UV but we don't need the UVs.
	// Discard them to simplify pipeline management.
	if (vstride == 32 && vformat == 4099)
	{
		for (int i = 0; i < nverts; ++i)
		{
			memmove(vbData.data() + i * 24, vbData.data() + i * 32, 24);
		}
		vstride = 24;
		vformat = 3;
	}

	// init mesh
	CVertexFormat format(vformat);
	assert(format.calcVertexSize() == vstride);
	CMesh* mesh = new CMesh(nverts, ntris * 3, format, istride, vbData.data(), ibData.data(), ngroups, groups.data());
	return mesh;
}
