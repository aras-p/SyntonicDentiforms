#include "MeshEntity.h"
#include "DemoResources.h"
#include <assert.h>

CMeshEntity::CMeshEntity(DataMesh type)
:	mMesh(nullptr),
	mCubeFace(CFACE_PX)
{
    mMatrix.identify(); mWVPMatrix.identify();
    
    assert(type < DataMeshCOUNT);
	if (type == DataMeshBox)
	{
		mRenderModesMask = 1 << RM_HI;
	}
	else if (type >= DataMeshCube && type <= DataMeshCubePZ)
	{
		mRenderModesMask = (1<<RM_RECV_HI) | (1<<RM_RECV_LO);
		if (type != DataMeshCube)
		{
			mRenderModesMask |= 1<<RM_REFLECTIVE;
			if     (type == DataMeshCubePX) mCubeFace = CFACE_PX;
			else if(type == DataMeshCubeNX) mCubeFace = CFACE_NX;
			else if(type == DataMeshCubePY) mCubeFace = CFACE_PY;
			else if(type == DataMeshCubeNY) mCubeFace = CFACE_NY;
			else if(type == DataMeshCubePZ) mCubeFace = CFACE_PZ;
			else if(type == DataMeshCubeNZ) mCubeFace = CFACE_NZ;
		}
	} else {
		mRenderModesMask = (1<<RM_RECV_HI) | (1<<RM_RECV_LO) | (1<<RM_SHADOW);
	}

	mMesh = g_data_mesh[type];
}

CMeshEntity::~CMeshEntity()
{
}

void CMeshEntity::render(eRenderMode renderMode, sg_bindings* binds)
{
	if( !(mRenderModesMask & (1<<renderMode)) )
		return;

	updateMatrices();
	if( frustumCull() )
		return;

	if (renderMode == RM_REFLECTIVE)
	{
		sg_view texs[CFACE_COUNT] = {
			rt_refl_px.view_tex, rt_refl_nx.view_tex,
			rt_refl_py.view_tex, rt_refl_ny.view_tex,
			rt_refl_pz.view_tex, rt_refl_nz.view_tex,
		};
		binds->views[0] = texs[mCubeFace];
	}

	EntityUniformsVS uboVS = {};
	uboVS.mat = mMatrix;
	uboVS.matWV = mWVMatrix;
	uboVS.matWVP = mWVPMatrix;
	sg_apply_uniforms(1, {&uboVS, sizeof(uboVS)});

	binds->vertex_buffers[0] = mMesh->getVB();
	binds->index_buffer = mMesh->getIB();
	sg_apply_bindings(binds);
	sg_draw(0, mMesh->getIndexCount(), 1);
}
