#include "stdafx.h"

#include "MeshEntity.h"
#include "DemoResources.h"

#include <dingus/utils/StringHelper.h>

int CMeshEntity::mShadowIDGenerator = 0;


// --------------------------------------------------------------------------

CMeshEntity::CMeshEntity( const std::string& clazz )
:	mMeshRecvHi(0), mMeshRecvLo(0), mMeshShadow(0), mMeshReflection(0), mMeshHi(0),
	mShadowID(0), mCubeFace(CFACE_PX)
{
	assert( !clazz.empty() );

	if( clazz == "Box" ) {
		mRenderModes = RM_HI;
		mShadowID = 0;
	} else if( CStringHelper::startsWith( clazz, "Cube" ) ) {
		mRenderModes = RM_RECV_HI | RM_RECV_LO;
		mShadowID = 0;
		if( clazz != "Cube" ) {
			mRenderModes |= RM_REFLECTIVE;
			if( CStringHelper::endsWith(clazz,"PX") ) mCubeFace = CFACE_PX;
			else if( CStringHelper::endsWith(clazz,"NX") ) mCubeFace = CFACE_NX;
			else if( CStringHelper::endsWith(clazz,"PY") ) mCubeFace = CFACE_PY;
			else if( CStringHelper::endsWith(clazz,"NY") ) mCubeFace = CFACE_NY;
			else if( CStringHelper::endsWith(clazz,"PZ") ) mCubeFace = CFACE_PZ;
			else if( CStringHelper::endsWith(clazz,"NZ") ) mCubeFace = CFACE_NZ;
			else ASSERT_FAIL_MSG( "bad Cube* mesh" );
		}
	} else {
		mRenderModes = RM_RECV_HI | RM_RECV_LO | RM_SHADOW;
		++mShadowIDGenerator;
		if( mShadowIDGenerator >= 16 )
			mShadowIDGenerator = 1;
		mShadowID = mShadowIDGenerator;
	}
	
	SVector4 vshadowID;
	vshadowID.x = vshadowID.y = vshadowID.z = vshadowID.w = 
		(mShadowID*16) / 255.0f;

	const char* fxRHi = mShadowID ? "receiverHi" : "recvallHi";
	const char* fxRLo = mShadowID ? "receiverLo" : "recvallLo";
	const char* fxShadow = "caster";
	const char* fxRefl = "reflective";
	const char* fxHi = "noshadowHi";
	
	mMeshRecvHi = new CRenderableMesh( *RGET_MESH(clazz), 0, &mMatrix.getOrigin(), 0 );
	mMeshRecvHi->getParams().setEffect( *RGET_FX(fxRHi) );
	mMeshRecvHi->getParams().addVector4( "vShadowID", vshadowID );
	addMatricesToParams( mMeshRecvHi->getParams() );

	mMeshRecvLo = new CRenderableMesh( *RGET_MESH(clazz), 0, &mMatrix.getOrigin(), 0 );
	mMeshRecvLo->getParams().setEffect( *RGET_FX(fxRLo) );
	mMeshRecvLo->getParams().addVector4( "vShadowID", vshadowID );
	addMatricesToParams( mMeshRecvLo->getParams() );
	
	mMeshShadow = new CRenderableMesh( *RGET_MESH(clazz), 0, &mMatrix.getOrigin(), 0 );
	mMeshShadow->getParams().setEffect( *RGET_FX(fxShadow) );
	mMeshShadow->getParams().addVector4( "vShadowID", vshadowID );
	addMatricesToParams( mMeshShadow->getParams() );
	
	mMeshReflection = new CRenderableMesh( *RGET_MESH(clazz), 0, &mMatrix.getOrigin(), 1 );
	mMeshReflection->getParams().setEffect( *RGET_FX(fxRefl) );
	static const char* texs[CFACE_COUNT] = {
		RT_REFL_PX, RT_REFL_NX, RT_REFL_PY, RT_REFL_NY, RT_REFL_PZ, RT_REFL_NZ };
	mMeshReflection->getParams().addTexture( "tRefl", *RGET_STEX(texs[mCubeFace]) );
	addMatricesToParams( mMeshReflection->getParams() );

	mMeshHi = new CRenderableMesh( *RGET_MESH(clazz), 0, &mMatrix.getOrigin(), 0 );
	mMeshHi->getParams().setEffect( *RGET_FX(fxHi) );
	addMatricesToParams( mMeshHi->getParams() );
}

CMeshEntity::~CMeshEntity()
{
	delete mMeshRecvHi;
	delete mMeshRecvLo;
	delete mMeshShadow;
	delete mMeshReflection;
	delete mMeshHi;
}

void CMeshEntity::render( int renderMode )
{
	if( !(mRenderModes & renderMode) )
		return;
	updateMatrices();
	if( frustumCull() )
		return;
	
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
}
