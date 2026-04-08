#include "MeshEntity.h"
#include "DemoResources.h"

#include <src/utils/StringHelper.h>

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

	if (renderMode != RM_REFLECTIVE && renderMode != RM_HI)
	{
		EntityUniformsFS uboFS = {};
		uboFS.shadowID.set(mShadowVal, mShadowVal, mShadowVal, mShadowVal);
		sg_apply_uniforms(2, { &uboFS, sizeof(uboFS) });
	}

	binds->vertex_buffers[0] = mMesh->getVB();
	binds->index_buffer = mMesh->getIB();
	sg_apply_bindings(binds);
	sg_draw(0, mMesh->getIndexCount(), 1);
}
