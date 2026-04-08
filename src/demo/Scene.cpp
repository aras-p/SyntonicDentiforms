#include "Scene.h"
#include "Effect.h"

#include "DemoResources.h"

CScene::CScene( int number )
:	mNumber(number)
{
	//
	// load anims

	char buf[100];
	sprintf( buf, "Anim%i", number );
	mAnimLight = new CAnim( buf, "Light" );
	mAnimCamera = new CAnim( buf, "Camera" );
}

CScene::~CScene()
{
	int n = mMeshes.size();
	for( int i = 0; i < n; ++i )
		delete mMeshes[i].mesh;
}

void CScene::initialize()
{
	int i;
	char buf[100];
	sprintf( buf, "data/scene%i.txt", mNumber );
	FILE* f = fopen( buf, "rt" );
	assert( f );

	int n;
	float flen;
	fscanf( f, "length = %f\n", &flen );
	mLength = flen;
	assert( mLength > 0 );
	fscanf( f, "count = %i\n", &n );
	assert( n > 0 );
	char meshname[100], parentname[100];
	SVector3 pos, rot0, rot1;

	//
	// read meshes

	sprintf( buf, "scene%i/", mNumber );
	std::string meshPrefix = buf;
	for( i = 0; i < n; ++i ) {
		fscanf( f, "name=%s parent=%s\n", meshname, parentname );
		fscanf( f, "pos=%f,%f,%f\n", &pos.x, &pos.y, &pos.z );
		fscanf( f, "rot0=%f,%f,%f ", &rot0.x, &rot0.y, &rot0.z );
		fscanf( f, "rot1=%f,%f,%f\n", &rot1.x, &rot1.y, &rot1.z );
		SMesh mesh;
		mesh.name = meshname;
		mesh.parent = parentname;
		mesh.parentIdx = -1;
		mesh.pos = pos;
		mesh.rot = rot0 * (M_PI/180.0f);
		mesh.rotVel = (rot1-rot0) * (mLength* M_PI /180.0f);
		mesh.mesh = new CMeshEntity( meshPrefix + mesh.name );
		mMeshes.push_back( mesh );

		toMatrix( mesh.pos, mesh.rot, mesh.mesh->mMatrix );
	}

	fclose( f );

	//
	// calculate hierarchy traversal order

	for( i = 0; i < mMeshes.size(); ++i ) {
		if( mMeshes[i].parent == "NONE" ) {
			mMeshes[i].parentIdx = -1;
			recurseAdd( i );
		}
	}
}


void CScene::recurseAdd( int idx )
{
	mEvalOrder.push_back( idx );
	int n = mMeshes.size();
	for( int i = 0; i < n; ++i ) {
		if( mMeshes[i].parent == mMeshes[idx].name ) {
			assert( mMeshes[i].parentIdx == -1 );
			mMeshes[i].parentIdx = idx;
			recurseAdd( i );
		}
	}
}

CMeshEntity* CScene::addStaticMesh( const std::string& name )
{
	SMesh mesh;
	mesh.name = name;
	mesh.parent = "NONE";
	mesh.mesh = new CMeshEntity( name );
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
		effect_apply(fx_receiverHi);
		break;
	case RM_RECV_LO:
		effect_apply(fx_receiverLo);
		break;
	case RM_SHADOW:
		effect_apply(fx_caster);
		ubo0 = false;
		break;
	case RM_REFLECTIVE:
		effect_apply(fx_reflective);
		break;
	case RM_HI:
		effect_apply(fx_noshadowHi);
		break;
	default:
		ASSERT_MSG(false, "Unknown render mode");
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
