#include "Scene.h"
#include "Pipelines.h"

#include "DemoResources.h"
#include "SceneData.h"

#include <assert.h>

CScene::CScene( int number )
:	mNumber(number)
{
	mAnimLight = new CAnim((DataAnim)(DataAnimAnim0 + number), "Light" );
	mAnimCamera = new CAnim((DataAnim)(DataAnimAnim0 + number), "Camera" );
}

CScene::~CScene()
{
	int n = mMeshes.size();
	for( int i = 0; i < n; ++i )
		delete mMeshes[i].mesh;
}

static const SceneData* kSceneDatas[] = { nullptr, &kScene1, &kScene2, &kScene3, &kScene4, &kScene5, &kScene6 };

void CScene::initialize()
{
	const SceneData& sd = *kSceneDatas[mNumber];
	mLength = sd.length;

	for( int i = 0; i < sd.count; ++i ) {
		const SceneEntityData& e = sd.entities[i];
		SVector3 rot0(e.rot0[0], e.rot0[1], e.rot0[2]);
		SVector3 rot1(e.rot1[0], e.rot1[1], e.rot1[2]);
		SMesh mesh;
		mesh.parentIdx = e.parentIdx;
		mesh.pos = SVector3(e.pos[0], e.pos[1], e.pos[2]);
		mesh.rot = rot0 * (M_PI/180.0f);
		mesh.rotVel = (rot1-rot0) * (mLength * M_PI / 180.0f);
		mesh.mesh = new CMeshEntity(e.mesh);
		mMeshes.push_back( mesh );

		toMatrix( mesh.pos, mesh.rot, mesh.mesh->mMatrix );
	}

	//
	// calculate hierarchy traversal order

	for( int i = 0; i < (int)mMeshes.size(); ++i ) {
		if( mMeshes[i].parentIdx == -1 ) {
			recurseAdd( i );
		}
	}
}


void CScene::recurseAdd( int idx )
{
	mEvalOrder.push_back( idx );
	int n = mMeshes.size();
	for( int i = 0; i < n; ++i ) {
		if( mMeshes[i].parentIdx == idx ) {
			recurseAdd( i );
		}
	}
}

CMeshEntity* CScene::addStaticMesh(DataMesh data)
{
	SMesh mesh;
	mesh.mesh = new CMeshEntity(data);
	mesh.pos.set(0,0,0);
	mesh.rot.set(0,0,0);
	mesh.rotVel.set(0,0,0);
	mesh.parentIdx = -1;
	mEvalOrder.push_back( mMeshes.size() );
	mMeshes.push_back( mesh );
	return mesh.mesh;
}

void CScene::toMatrix( const SVector3& pos, const SVector3& rot, SMatrix4x4& m )
{
	SMatrix4x4 mrx, mry, mrz;
	mrx.rotationX(-rot.x);
	mry.rotationY(-rot.z);
	mrz.rotationZ(-rot.y);
	m = mrx * mrz * mry;
	m.getOrigin() = pos;
}

void CScene::evaluate( float t, SMatrix4x4& light, SMatrix4x4& camera )
{
	// eval light/camera
	evaluateLight( t, light );
	evaluateCamera( t, camera );

	// eval mesh transforms
	evaluateMeshes( t );
}

void CScene::evaluateCamera( float t, SMatrix4x4& camera ) const
{
	SMatrix4x4 mr;
	mAnimCamera->sample( t, camera );
	mr.rotationX(M_PI/2);
	camera = mr * camera;
}

void CScene::evaluateLight( float t, SMatrix4x4& light ) const
{
	SMatrix4x4 mr;
	mAnimLight->sample( t, light );
	mr.rotationX(M_PI / 2);
	light = mr * light;
}

void CScene::evaluateMeshes( float t )
{
	// eval mesh transforms
	int n = mMeshes.size();
	for( int i = 0; i < n; ++i ) {
		int idx = mEvalOrder[i];
		SMesh& mesh = mMeshes[idx];
		// local transform
		SVector3 rot = mesh.rot + mesh.rotVel * t;
		SMatrix4x4& m = mesh.mesh->mMatrix;
		toMatrix( mesh.pos, rot, m );
		// into world space
		if( mesh.parentIdx >= 0 ) {
			m = m * mMeshes[mesh.parentIdx].mesh->mMatrix;
		}
	}
}


void CScene::render(eRenderMode renderMode, sg_bindings* binds)
{
	int n = mMeshes.size();
	if (n == 0)
		return;

	bool ubo0 = true;
	switch (renderMode) {
	case RM_RECV_HI:
		pipeline_apply(pip_renderLitShadowed);
		break;
	case RM_RECV_LO:
		pipeline_apply(pip_renderLitShadowedFlip);
		break;
	case RM_SHADOW:
		pipeline_apply(pip_shadowCaster);
		ubo0 = false;
		break;
	case RM_REFLECTIVE:
		pipeline_apply(pip_renderReflective);
		break;
	case RM_HI:
		pipeline_apply(pip_renderLit);
		break;
	default:
        assert(false);
	}

	if (ubo0)
		sg_apply_uniforms(0, { &g_global_u, sizeof(g_global_u) });

	for( int i = 0; i < n; ++i )
		mMeshes[i].mesh->render(renderMode, binds);
}

void CScene::addCut( float frame )
{
	mCutTimes.push_back( frame / getLength() );
}

float CScene::getPastCut( float t ) const
{
	int n = mCutTimes.size();
	int i;
	for( i = 0; i < n; ++i ) {
		if( mCutTimes[i] > t )
			break;
	}
	if( i==0 )
		return -1.0f;
	else
		return mCutTimes[i-1];
}
