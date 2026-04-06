#include "stdafx.h"

#include "SceneTeeth.h"
#include "DemoResources.h"
#include "Glare.h"
#include "../util/LineRenderer.h"
#include <dingus/math/MathUtils.h>
#include <dingus/utils/StringHelper.h>

const int TOTAL_FRAMES = 1500;

const int PATH_FRAMES = 200;
const int BLOB_BLUR_PASSES = 5;

// --------------------------------------------------------------------------


CTeethAnim::CTeethAnim( const std::string& animation, int startFrame, int endFrame )
{
	int i;
	mAnim = RGET_ANIM(animation);
	mPosAnim = mAnim->findVector3Anim( "pos" );
	assert( mPosAnim );
	mRotAnim = mAnim->findQuatAnim( "rot" );
	assert( mRotAnim );

	int ncurves = mAnim->getCurveCount();
	mTeethIdx = new int[ ncurves ];
	for( i = 0; i < ncurves; ++i )
		mTeethIdx[i] = -1;
	mVectors = new SVector3[ ncurves ];
	mQuats = new SQuaternion[ ncurves ];

	mPaths = new TVec3Vector[ ncurves ];

	mTotalFrames = TOTAL_FRAMES;
	mStartFrame = startFrame;
	mEndFrame = endFrame;

	//const float PATH_START = 0.05f;
	//const float PATH_LEN = 0.3f;
	const float PATH_START = 0.0f;
	const float PATH_LEN = 1.0f;
	for( i = 0; i < ncurves; ++i ) {
		mPaths[i].reserve( PATH_FRAMES );
	}
	for( i = 0; i < PATH_FRAMES; ++i ) {
		float a = PATH_START + (float)i / PATH_FRAMES * PATH_LEN;
		mPosAnim->sample( a, 0, ncurves, &mVectors[0] );
		for( int j = 0; j < ncurves; ++j )
			mPaths[j].push_back( mVectors[j] );
	}
}

CTeethAnim::~CTeethAnim()
{
	delete[] mTeethIdx;
	delete[] mQuats;
	delete[] mVectors;
	delete[] mPaths;
}


bool CTeethAnim::evaluate( float t )
{
	float reltime = getRelTime( t );
	bool before = (reltime < 0.0f);
	reltime = clamp( reltime, 0.0f, 1.0f );
	mPosAnim->sample( reltime, 0, mAnim->getCurveCount(), &mVectors[0] );
	mRotAnim->sample( reltime, 0, mAnim->getCurveCount(), &mQuats[0] );
	return before;
}

float CTeethAnim::getRelTime( float t ) const
{
	float tframe = t * mTotalFrames;
	float reltime = (tframe-mStartFrame) / (mEndFrame-mStartFrame);
	return reltime;
}

bool CTeethAnim::possiblyAddTooth( const std::string& name, int index )
{
	int idx = mAnim->getCurveIndexByName( name );
	if( idx < 0 )
		return false;
	assert( mTeethIdx[idx] == -1 );
	mTeethIdx[idx] = index;
	return true;
}

// --------------------------------------------------------------------------

CSceneTeeth::CSceneTeeth( int number )
:	CScene(number)
{
	mAnimTeeth[0] = new CTeethAnim( "6/TeethA", 350, 480 );
	mAnimTeeth[1] = new CTeethAnim( "6/TeethB", 490, 590 );
	mAnimTeeth[2] = new CTeethAnim( "6/TeethC", 745, 860 );
	mAnimTeeth[3] = new CTeethAnim( "6/TeethD", 860, 965 );
	mAnimAxes[0] = new CAnim( "6/Axes", "Axis1", CAnim::POSITION | CAnim::ROTATION );
	mAnimAxes[1] = new CAnim( "6/Axes", "Axis2", CAnim::POSITION | CAnim::ROTATION );

	{
		mToonQuad = new CRenderableMesh( *RGET_MESH("billboard"), 0 );
		CEffectParams& ep = mToonQuad->getParams();
		ep.setEffect( *RGET_FX("filterToon") );
		ep.addVector4Ref( "vFixUV", gScreenFixUVs );
		ep.addTexture( "tBase", *RGET_STEX(RT_FULLSCREEN_1) );
		ep.addTexture( "tLUT", *RGET_TEX("ColorLuts") );
	}
	{
		mCompositeQuad = new CRenderableMesh( *RGET_MESH("billboard"), 0 );
		CEffectParams& ep = mCompositeQuad->getParams();
		ep.setEffect( *RGET_FX("compositeAlpha") );
		ep.addVector4Ref( "vFixUV", gScreenFixUVs );
		ep.addTexture( "tBase", *RGET_STEX(RT_FULLSCREEN_2) );
		ep.addTexture( "tAlpha", *RGET_STEX( !(BLOB_BLUR_PASSES&1) ? RT_4thSCREEN_1 : RT_4thSCREEN_2 ) );
		ep.addTexture( "tEdgeLUT", *RGET_TEX("AlphaEdge") );
	}
	{
		mToothMaskMesh = new CRenderableMesh( *RGET_MESH("scene6/Tooth1"), 0 );
		CEffectParams& ep = mToothMaskMesh->getParams();
		ep.setEffect( *RGET_FX("caster") );
		ep.addVector4( "vShadowID", SVector4(1.0f, 1.0f, 1.0f, 1.0f) );
		ep.addMatrix4x4Ref( "mWVP", mMaskMeshWVP );
	}
	/*
	{
		mMaskGears[0] = new CRenderableMesh( *RGET_MESH("scene6/Gear1"), 0 );
		CEffectParams& ep = mToothMaskMesh->getParams();
		ep.setEffect( *RGET_FX("caster") );
		ep.addVector4( "vShadowID", SVector4(1.0f, 1.0f, 1.0f, 1.0f) );
		ep.addMatrix4x4Ref( "mWVP", mMaskMeshWVP );
	}
	{
		mMaskGears[1] = new CRenderableMesh( *RGET_MESH("scene6/Gear2"), 0 );
		CEffectParams& ep = mToothMaskMesh->getParams();
		ep.setEffect( *RGET_FX("caster") );
		ep.addVector4( "vShadowID", SVector4(1.0f, 1.0f, 1.0f, 1.0f) );
		ep.addMatrix4x4Ref( "mWVP", mMaskMeshWVP );
	}
	*/
}

CSceneTeeth::~CSceneTeeth()
{
	for( int i = 0; i < TEETHPACKS; ++i )
		delete mAnimTeeth[i];
	delete mAnimAxes[0];
	delete mAnimAxes[1];
	delete mToonQuad;
	delete mCompositeQuad;
	delete mToothMaskMesh;
}

void CSceneTeeth::initialize()
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
	// create axes

	mAxesIdx[0] = mMeshes.size();
	addStaticMesh( "scene6/Axis1" );
	mAxesIdx[1] = mMeshes.size();
	addStaticMesh( "scene6/Axis2" );

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
		mesh.rot = rot0 * (D3DX_PI/180.0f);
		mesh.rotVel = (rot1-rot0) * (mLength*D3DX_PI/180.0f);

		// teeth?
		if( CStringHelper::startsWith( mesh.name, "Tooth" ) ) {
			bool found = false;
			for( int i = 0; i < TEETHPACKS; ++i ) {
				found = mAnimTeeth[i]->possiblyAddTooth( mesh.name, mMeshes.size() );
				if( found )
					break;
			}
			assert( found );
			if( CStringHelper::startsWith( mesh.name, "Tooth1_" ) ) {
				mesh.name = "Tooth1";
				mesh.parentIdx = 0;
			} else {
				mesh.name = "Tooth2";
				mesh.parentIdx = 1;
			}
		}
		// gear?
		if( mesh.name == "Gear1" ) {
			mGearsIdx[0] = mMeshes.size();
		} else if( mesh.name == "Gear2" ) {
			mGearsIdx[1] = mMeshes.size();
		}

		mesh.mesh = new CMeshEntity( meshPrefix + mesh.name );
		mMeshes.push_back( mesh );
		toMatrix( mesh.pos, mesh.rot, mesh.mesh->mMatrix );
	}

	fclose( f );
}

void CSceneTeeth::evaluateMeshes( float t )
{
	const float AXES_FRAMES = 1100.0f;
	float axest = t * (TOTAL_FRAMES / AXES_FRAMES);

	int i;

	// eval axes & gears...
	for( i = 0; i < GEARS; ++i ) {
		// axis...
		mAnimAxes[i]->sample( axest, mMeshes[mAxesIdx[i]].mesh->mMatrix );

		// gear...
		int idx = mGearsIdx[i];
		SMesh& mesh = mMeshes[idx];
		// local transform
		SVector3 rot = mesh.rot + mesh.rotVel * t;
		SMatrix4x4& m = mesh.mesh->mMatrix;
		toMatrix( mesh.pos, rot, m );
		// into world space
		m = m * mMeshes[mAxesIdx[i]].mesh->mMatrix;
	}

	// eval teeth...
	for( i = 0; i < TEETHPACKS; ++i ) {
		CTeethAnim& ta = *mAnimTeeth[i];
		bool before = ta.evaluate( t );
		int n = ta.getCount();
		for( int j = 0; j < n; ++j ) {
			SMesh& mesh = mMeshes[ ta.mTeethIdx[j] ];
			SMatrix4x4& m = mesh.mesh->mMatrix;
			if( before ) {
				// attach to parent
				toMatrix( mesh.pos, mesh.rot, m );
				m = m * mMeshes[mGearsIdx[mesh.parentIdx]].mesh->mMatrix;
			} else {
				// animate
				D3DXMatrixRotationQuaternion( &m, ta.mQuats[j] );
				m.getOrigin() = ta.mVectors[j];
			}
		}
	}
}

void CSceneTeeth::renderTeethBills( int pack, float t, float relT, float cutAlpha, bool masks )
{
	const int BILLSPERPACK = 3;
	static const char* names[TEETHPACKS][BILLSPERPACK] = {
		{ "DentATrepasa", "DentBPokero", "DentCCorenjo" },
		{ "DentDSmogenias", "DentEChanChan", "DentFPerto" },
		{ "DentGZetal", "DentHLaVina", "DentICarina" },
		{ "DentATrepasa", "DentATrepasa", "DentATrepasa" },
	};
	static const float ratios[TEETHPACKS][BILLSPERPACK] = {
		{ 576.0f/576.0f, 544.0f/431.0f, 436.0f/394.0f },
		{ 524.0f/514.0f, 462.0f/420.0f, 479.0f/434.0f },
		{ 380.0f/360.0f, 554.0f/524.0f, 396.0f/372.0f },
		{ 1.0f, 1.0f, 1.0f },
	};
	static const float greetUVs[TEETHPACKS][BILLSPERPACK][4] = {
		{
			{ 0, 326, 640, 502 },	// Condense&Mandarine
			{ 742, 0, 924, 154 },	// FAN
			{ 0, 0, 394, 154 },		// Farbrausch
		},
		{
			{ 470, 154, 795, 320 },	// Haujobb
			{ 394, 0, 742, 154 },	// Kewlers
			{ 0, 154, 226, 320 },	// Kolor
		},
		{
			{ 795, 154, 989, 286 },	// MFX
			{ 226, 154, 470, 320 },	// Moppi
			{ 640, 326, 1017, 502 },// All others
		},
		{
			{ 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 },
		}
	};
	const float BSIZE = 0.5f;
	const float BEDGE = 0.12f;
	const float MEDGE = 0.06f;

	CRenderableOrderedBillboards& bills = *(masks ? gBillboardsNormal : gBillboardsNoDestA);
	const float aspect = CD3DDevice::getInstance().getBackBufferAspect();
	SOBillboard* bill;

	if( pack != TEETHPACKS-1 ) {
		for( int i = 0; i < BILLSPERPACK; ++i ) {
			float billAlpha = 3.0f - fabsf(cutAlpha - (i+0.5f)/(BILLSPERPACK))*BILLSPERPACK*6;
			if( masks )
				billAlpha += 0.5f;
			if( billAlpha <= 0.0f )
				continue;
			
			// tooth image
			bill = &bills.addBill();
			bill->x1 = 1.0f - BEDGE - BSIZE;
			bill->y1 = -1.0f + BEDGE*aspect;
			bill->x2 = bill->x1 + BSIZE;
			const float toothY = bill->y2 = bill->y1 + BSIZE*aspect / (ratios[pack][i]);
			bill->texture = RGET_TEX(names[pack][i]);
			D3DXCOLOR c( 1, 1, 1, billAlpha );
			if( masks ) {
				bill->x1 -= MEDGE;
				bill->y1 -= MEDGE*aspect;
				bill->x2 += MEDGE;
				bill->y2 += MEDGE*aspect;
			}
			bill->color = c;
			bill->setWholeTexture();

			// greets image
			const float texelScale = 1.0f / 768.0f;
			const float* uvs = greetUVs[pack][i];
			bill = &bills.addBill();
			bill->x1 = 1.0f - BEDGE*0.5f - BSIZE - (uvs[2]-uvs[0]) * texelScale;
			bill->y1 = -1.0f + BEDGE*aspect*0.2f;
			bill->x2 = 1.0f - BEDGE*0.5f - BSIZE;
			bill->y2 = bill->y1 + (uvs[3]-uvs[1]) * texelScale*aspect;
			/*bill->x1 = 1.0f - BEDGE - (uvs[2]-uvs[0]) * texelScale;
			bill->y1 = toothY + BEDGE*aspect*0.2f;
			bill->x2 = 1.0f - BEDGE;
			bill->y2 = bill->y1 + (uvs[3]-uvs[1]) * texelScale*aspect;*/
			bill->color = c;
			bill->texture = RGET_TEX("Greetings");
			bill->tu1 = uvs[0] / 1024.0f;	bill->tv1 = uvs[1] / 512.0f;
			bill->tu2 = uvs[2] / 1024.0f;	bill->tv2 = uvs[3] / 512.0f;
			if( masks ) {
				bill->x1 -= MEDGE;
				bill->y1 -= MEDGE*aspect;
				bill->x2 += MEDGE;
				bill->y2 += MEDGE*aspect;
			}
		}
	} else {
		const float GRP_WIDTH = 0.8f;
		const float GRP_HEIGHT = GRP_WIDTH*aspect*898.0f/1050.0f;
		bill = &bills.addBill();
		bill->x1 = -GRP_WIDTH/2;
		bill->y1 = -GRP_HEIGHT/2;
		bill->x2 = GRP_WIDTH/2;
		bill->y2 = GRP_HEIGHT/2;
		D3DXCOLOR c( 1, 1, 1, 0.0f );
		if( masks ) {
			bill->x1 -= MEDGE*5;
			bill->y1 -= MEDGE*5*aspect;
			bill->x2 += MEDGE*5;
			bill->y2 += MEDGE*5*aspect;
			float scale = clamp( (relT - 1.6f)*1.5f, 0, 1 );
			bill->x1 *= scale;
			bill->y1 *= scale;
			bill->x2 *= scale;
			bill->y2 *= scale;
			c.a = relT*3;
		} else {
			c.a = (relT-2);
		}
		bill->color = c;
		bill->texture = RGET_TEX("BondigoDuo");
		bill->setWholeTexture();
	}
	G_RCTX->directRender( bills );
}


void CSceneTeeth::renderTeethStuff( int pack, float t, float cutAlpha )
{
	assert( pack >= 0 && pack < TEETHPACKS );
	CTeethAnim& ta = *mAnimTeeth[pack];
	int nteeth = ta.getCount();
	int i;

	float relT = ta.getRelTime( t );

	G_RCTX->directBegin();
	const int PATH_SIZE = PATH_FRAMES;
	SLinePoint path[PATH_SIZE];
	for( i = 0; i < nteeth; ++i ) {
		float a = 0.0f;
		float da = 1.0f / PATH_SIZE;
		for( int j = 0; j < PATH_SIZE; ++j, a += da ) {
			path[j].pos = ta.mPaths[i][j];
			D3DXCOLOR c;
			c.r = 1.0f; c.g = 0.0f; c.b = 0.0f;
			float delta = a-relT;
			if( delta < 0.0f )
				delta *= 4;
			c.a = 0.75f - fabsf( delta ) * 4;
			//if( delta>=0 )
			//	c.a = 0.0f;
			path[j].color = c;
		}
		gLineRenderer->renderStrip( PATH_SIZE, path, 0.05f );
	}
	G_RCTX->directEnd();

	// end scene, copy backbuffer to texture, begin scene again
	// toon-process all into texture
	CD3DDevice& dx = CD3DDevice::getInstance();
	dx.sceneEnd();
	dx.getDevice().StretchRect( dx.getBackBuffer(), NULL, RGET_SSURF(RT_FULLSCREEN_1)->getObject(), NULL, D3DTEXF_NONE );
	dx.setRenderTarget( RGET_SSURF(RT_FULLSCREEN_2), 0 );
	dx.clearTargets( true, false, false, 0x00ff0000 ); // REMOVE
	dx.setZStencil( NULL );
	dx.sceneBegin();
	G_RCTX->directBegin();
	G_RCTX->directRender( *mToonQuad );
	G_RCTX->directEnd();
	dx.sceneEnd();

	// render teeth masks into mask RT
	dx.setRenderTarget( RGET_SSURF(RT_4thSCREEN_1), 0 );
	dx.clearTargets( true, false, false, 0x00000000, 1.0f, 0 );
	dx.sceneBegin();
	G_RCTX->directBegin();
	float toothMaskScale = 6.0f;
	const float SCALEUP_ALPHA = 0.03f;
	if( cutAlpha < SCALEUP_ALPHA ) {
		toothMaskScale *= cutAlpha / SCALEUP_ALPHA;
	}
	/*if( pack==TEETHPACKS-1 && cutAlpha > 1-SCALEUP_ALPHA*3 ) {
		toothMaskScale *= (1 - cutAlpha)/(SCALEUP_ALPHA*3);
	} else*/ if( pack!=TEETHPACKS-1 && cutAlpha > 1-SCALEUP_ALPHA ) {
		toothMaskScale *= (1 - cutAlpha)/SCALEUP_ALPHA;
	}
	if( toothMaskScale > 0.001f ) {
		if( pack != TEETHPACKS-1 ) {
			for( i = 0; i < nteeth; ++i ) {
				SMesh& mesh = mMeshes[ ta.mTeethIdx[i] ];
				mMaskMeshWVP = mesh.mesh->mMatrix;
				mMaskMeshWVP.getAxisX() *= toothMaskScale;
				mMaskMeshWVP.getAxisY() *= toothMaskScale * toothMaskScale*0.5f;
				mMaskMeshWVP.getAxisZ() *= toothMaskScale;
				mMaskMeshWVP *= G_RCTX->getCamera().getViewProjMatrix();
				G_RCTX->directRender( *mToothMaskMesh );
			}
		} else {
			for( int pk = 0; pk < TEETHPACKS; ++pk ) {
				int ntth = mAnimTeeth[pk]->getCount();
				for( i = 0; i < ntth; ++i ) {
					SMesh& mesh = mMeshes[ mAnimTeeth[pk]->mTeethIdx[i] ];
					mMaskMeshWVP = mesh.mesh->mMatrix;
					mMaskMeshWVP.getAxisX() *= toothMaskScale;
					mMaskMeshWVP.getAxisY() *= toothMaskScale * toothMaskScale*0.5f;
					mMaskMeshWVP.getAxisZ() *= toothMaskScale;
					mMaskMeshWVP *= G_RCTX->getCamera().getViewProjMatrix();
					G_RCTX->directRender( *mToothMaskMesh );
				}
			}
		}
	}
	renderTeethBills( pack, t, relT, cutAlpha, true );

	G_RCTX->directEnd();
	dx.sceneEnd();

	gPPSBloom->pingPongBlur( BLOB_BLUR_PASSES );

	// composite, render paths
	dx.setDefaultRenderTarget();
	dx.setDefaultZStencil();
	dx.sceneBegin();

	G_RCTX->directBegin();
	G_RCTX->directRender( *mCompositeQuad );

	/*
	const int PATH_SIZE = PATH_FRAMES;
	SLinePoint path[PATH_SIZE];
	for( i = 0; i < nteeth; ++i ) {
		float a = 0.0f;
		float da = 1.0f / PATH_SIZE;
		for( int j = 0; j < PATH_SIZE; ++j, a += da ) {
			path[j].pos = ta.mPaths[i][j];
			D3DXCOLOR c;
			c.r = 1.0f; c.g = 0.0f; c.b = 0.0f;
			float delta = a-relT;
			if( delta < 0.0f )
				delta *= 4;
			c.a = 0.75f - fabsf( delta ) * 4;
			path[j].color = c;
		}
		gLineRenderer->renderStrip( PATH_SIZE, path, 0.05f );
	}
	*/

	G_RCTX->directEnd();
}

void CSceneTeeth::renderTeethUI( int pack, float t, float cutAlpha )
{
	assert( pack >= 0 && pack < TEETHPACKS );
	CTeethAnim& ta = *mAnimTeeth[pack];
	float relT = ta.getRelTime( t );
	int nteeth = ta.getCount();

	CD3DDevice& dx = CD3DDevice::getInstance();
	G_RCTX->directBegin();
	renderTeethBills( pack, t, relT, cutAlpha, false );
	G_RCTX->directEnd();
}
