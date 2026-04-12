#include "Mesh.h"

#include <assert.h>
#include <stdio.h>
#include <vector>

CMesh::CMesh(int vertCount, int idxCount, const Vertex *vbData, const uint16_t *ibData)
{
    assert(vertCount > 0);
    assert(idxCount > 0);
    mVertexCount = vertCount;
    mIndexCount = idxCount;

    // vertex buffer
    {
        sg_buffer_desc desc = {};
        desc.usage.vertex_buffer = true;
        desc.data.ptr = vbData;
        desc.data.size = mVertexCount * sizeof(Vertex);
        desc.usage.immutable = true;
        mVB = sg_make_buffer(&desc);
        assert(sg_query_buffer_state(mVB) == SG_RESOURCESTATE_VALID);
    }

    // index buffer
    {
        sg_buffer_desc desc = {};
        desc.usage.index_buffer = true;
        desc.data.ptr = ibData;
        desc.data.size = mIndexCount * sizeof(uint16_t);
        desc.usage.immutable = true;
        mIB = sg_make_buffer(&desc);
        assert(sg_query_buffer_state(mIB) == SG_RESOURCESTATE_VALID);
    }

    // AABB
    mTotalAABB.setNull();
    const char *vb = reinterpret_cast<const char *>(vbData);
    mTotalAABB.setNull();
    for (int v = 0; v < mVertexCount; ++v)
    {
        const Vertex &vert = vbData[v];
        const SVector3 pos(vert.x, vert.y, vert.z);
        mTotalAABB.extend(pos);
    }
}

CMesh::~CMesh()
{
    sg_destroy_buffer(mVB);
    mVB = {};
    sg_destroy_buffer(mIB);
    mIB = {};
}

// file format:
// - 4 bytes "dmsh"
// - 4 bytes nverts
// - 4 bytes ntris
// - 16*nverts vertex data (3x float position, 2xSNorm16 octahedral normal)
// - 2*3*ntris index data (16 bit)
CMesh *load_mesh(const char *fileName)
{
    // open file
    FILE *f = fopen(fileName, "rb");
    if (!f)
        return nullptr;

    // magic
    char magic[4];
    fread(magic, 4, 1, f);
    if (magic[0] != 'd' || magic[1] != 'm' || magic[2] != 's' || magic[3] != 'h')
    {
        fclose(f);
        return nullptr;
    }
    int nverts, ntris;
    fread(&nverts, 4, 1, f);
    fread(&ntris, 4, 1, f);
    assert(nverts > 0);
    assert(ntris > 0);

    std::vector<CMesh::Vertex> vbData(nverts);
    fread(vbData.data(), sizeof(CMesh::Vertex), nverts, f);

    std::vector<uint16_t> ibData(ntris * 3);
    fread(ibData.data(), 2, ntris * 3, f);

    fclose(f);

    // create mesh
    return new CMesh(nverts, ntris * 3, vbData.data(), ibData.data());
}
