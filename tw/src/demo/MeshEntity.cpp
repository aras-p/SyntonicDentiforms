#include "stdafx.h"

#include "MeshEntity.h"
#include "DemoResources.h"

#include <dingus/utils/StringHelper.h>

int CMeshEntity::mShadowIDGenerator = 0;


// --------------------------------------------------------------------------

CMeshEntity::CMeshEntity( const std::string& clazz )
:	mMesh(nullptr),
	mCubeFace(CFACE_PX)
{
	assert( !clazz.empty() );

	int shadowID = 0;
	if( clazz == "Box" ) {
		mRenderModesMask = 1 << RM_HI;
		shadowID = 0;
	} else if( CStringHelper::startsWith( clazz, "Cube" ) ) {
		mRenderModesMask = (1<<RM_RECV_HI) | (1<<RM_RECV_LO);
		shadowID = 0;
		if( clazz != "Cube" ) {
			mRenderModesMask |= 1<<RM_REFLECTIVE;
			if( CStringHelper::endsWith(clazz,"PX") ) mCubeFace = CFACE_PX;
			else if( CStringHelper::endsWith(clazz,"NX") ) mCubeFace = CFACE_NX;
			else if( CStringHelper::endsWith(clazz,"PY") ) mCubeFace = CFACE_PY;
			else if( CStringHelper::endsWith(clazz,"NY") ) mCubeFace = CFACE_NY;
			else if( CStringHelper::endsWith(clazz,"PZ") ) mCubeFace = CFACE_PZ;
			else if( CStringHelper::endsWith(clazz,"NZ") ) mCubeFace = CFACE_NZ;
			else ASSERT_FAIL_MSG( "bad Cube* mesh" );
		}
	} else {
		mRenderModesMask = (1<<RM_RECV_HI) | (1<<RM_RECV_LO) | (1<<RM_SHADOW);
		++mShadowIDGenerator;
		if( mShadowIDGenerator >= 16 )
			mShadowIDGenerator = 1;
		shadowID = mShadowIDGenerator;
	}

	mMesh = RGET_MESH(clazz);

	mShadowVal = (shadowID * 16) / 255.0f;
	
	/* //@TODO
	SVector4 vshadowID;
	vshadowID.x = vshadowID.y = vshadowID.z = vshadowID.w = 
		(mShadowID*16) / 255.0f;

	const char* fxRHi = mShadowID ? "receiverHi" : "recvallHi";
	const char* fxRLo = mShadowID ? "receiverLo" : "recvallLo";
	const char* fxShadow = "caster";
	const char* fxRefl = "reflective";
	const char* fxHi = "noshadowHi";
	
	mMeshRecvHi = new CRenderableMesh(*RGET_MESH(clazz), 0);
	mMeshRecvHi->getParams().setEffect( *RGET_FX(fxRHi) );
	mMeshRecvHi->getParams().addVector4( "vShadowID", vshadowID );
	addMatricesToParams( mMeshRecvHi->getParams() );

	mMeshRecvLo = new CRenderableMesh(*RGET_MESH(clazz), 0);
	mMeshRecvLo->getParams().setEffect( *RGET_FX(fxRLo) );
	mMeshRecvLo->getParams().addVector4( "vShadowID", vshadowID );
	addMatricesToParams( mMeshRecvLo->getParams() );
	
	mMeshShadow = new CRenderableMesh(*RGET_MESH(clazz), 0);
	mMeshShadow->getParams().setEffect( *RGET_FX(fxShadow) );
	mMeshShadow->getParams().addVector4( "vShadowID", vshadowID );
	addMatricesToParams( mMeshShadow->getParams() );
	
	mMeshReflection = new CRenderableMesh(*RGET_MESH(clazz), 0);
	mMeshReflection->getParams().setEffect( *RGET_FX(fxRefl) );
	static const char* texs[CFACE_COUNT] = {
		RT_REFL_PX, RT_REFL_NX, RT_REFL_PY, RT_REFL_NY, RT_REFL_PZ, RT_REFL_NZ };
	mMeshReflection->getParams().addTexture( "tRefl", *RGET_STEX(texs[mCubeFace]) );
	addMatricesToParams( mMeshReflection->getParams() );

	mMeshHi = new CRenderableMesh(*RGET_MESH(clazz), 0);
	mMeshHi->getParams().setEffect( *RGET_FX(fxHi) );
	addMatricesToParams( mMeshHi->getParams() );
	*/
}

CMeshEntity::~CMeshEntity()
{
}

// match entity_uniforms in shaders
struct EntityUniformsVS
{
	SMatrix4x4	mat;
	SMatrix4x4	matWV;
	SMatrix4x4	matWVP;
};
struct EntityUniformsFS
{
	SVector4 shadowID; // zero shadow ID: receive all shadows
};


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

	if (renderMode != RM_REFLECTIVE)
	{
		EntityUniformsFS uboFS = {};
		uboFS.shadowID.set(mShadowVal, mShadowVal, mShadowVal, mShadowVal);
		sg_apply_uniforms(2, { &uboFS, sizeof(uboFS) });
	}

	binds->vertex_buffers[0] = mMesh->getVB();
	binds->index_buffer = mMesh->getIB();
	sg_apply_bindings(binds);
	sg_draw(0, mMesh->getIndexCount(), 1);

	/*
	if( renderMode & RM_RECV_HI )
		G_RCTX->attach( *mMeshRecvHi );
	if( renderMode & RM_RECV_LO )
		G_RCTX->attach( *mMeshRecvLo );
	if( renderMode & RM_SHADOW )
		G_RCTX->attach( *mMeshShadow );
	if( renderMode & RM_REFLECTIVE )
		G_RCTX->attach( *mMeshReflection );
	if( renderMode & RM_HI )
		G_RCTX->attach( *mMeshHi );
		*/
}
