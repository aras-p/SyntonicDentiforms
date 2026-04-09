#include "SceneTeeth.h"

#include "Billboards.h"
#include "DemoResources.h"
#include "Glare.h"
#include "Pipelines.h"
#include "LineRenderer.h"
#include "DataFiles.h"
#include "SceneData.h"
#include <src/math/MathUtils.h>
#include <stdio.h>

const int TOTAL_FRAMES = 1500;

const int PATH_FRAMES = 200;
const int BLOB_BLUR_PASSES = 5;

// --------------------------------------------------------------------------


CTeethAnim::CTeethAnim(DataAnim animation, int startFrame, int endFrame)
{
	int i;
    mAnim = g_data_anim[animation];
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
	mAnimTeeth[0] = new CTeethAnim(DataAnim6TeethA, 350, 480 );
	mAnimTeeth[1] = new CTeethAnim(DataAnim6TeethB, 490, 590 );
	mAnimTeeth[2] = new CTeethAnim(DataAnim6TeethC, 745, 860 );
	mAnimTeeth[3] = new CTeethAnim(DataAnim6TeethD, 860, 965 );
	mAnimAxes[0] = new CAnim(DataAnim6Axes, "Axis1", CAnim::POSITION | CAnim::ROTATION );
	mAnimAxes[1] = new CAnim(DataAnim6Axes, "Axis2", CAnim::POSITION | CAnim::ROTATION );
}

CSceneTeeth::~CSceneTeeth()
{
	for( int i = 0; i < TEETHPACKS; ++i )
		delete mAnimTeeth[i];
	delete mAnimAxes[0];
	delete mAnimAxes[1];
}

void CSceneTeeth::initialize()
{
	const SceneData& sd = kScene6;
	mLength = sd.length;

	//
	// create axes

	mAxesIdx[0] = mMeshes.size();
	addStaticMesh(DataMesh6Axis1);
	mAxesIdx[1] = mMeshes.size();
	addStaticMesh(DataMesh6Axis2);

	//
	// create meshes

	int tooth1Count = 0, tooth2Count = 0;
	for( int i = 0; i < sd.count; ++i ) {
		const SceneEntityData& e = sd.entities[i];
		SVector3 rot0(e.rot0[0], e.rot0[1], e.rot0[2]);
		SVector3 rot1(e.rot1[0], e.rot1[1], e.rot1[2]);
		SMesh mesh;
		mesh.parentIdx = e.parentIdx;
		mesh.pos = SVector3(e.pos[0], e.pos[1], e.pos[2]);
		mesh.rot = rot0 * (M_PI/180.0f);
		mesh.rotVel = (rot1-rot0) * (mLength * M_PI / 180.0f);

		// teeth?
		if( e.mesh == DataMesh6Tooth1 || e.mesh == DataMesh6Tooth2 ) {
			char toothName[16];
			if( e.mesh == DataMesh6Tooth1 )
				snprintf( toothName, sizeof(toothName), "Tooth1_%d", tooth1Count++ );
			else
				snprintf( toothName, sizeof(toothName), "Tooth2_%d", tooth2Count++ );
			bool found = false;
			for( int j = 0; j < TEETHPACKS; ++j ) {
				found = mAnimTeeth[j]->possiblyAddTooth( toothName, mMeshes.size() );
				if( found )
					break;
			}
			assert( found );
		}
		// gear?
		if( e.mesh == DataMesh6Gear1 ) {
			mGearsIdx[0] = mMeshes.size();
		} else if( e.mesh == DataMesh6Gear2 ) {
			mGearsIdx[1] = mMeshes.size();
		}

		mesh.mesh = new CMeshEntity(e.mesh);
		mMeshes.push_back( mesh );
		toMatrix( mesh.pos, mesh.rot, mesh.mesh->mMatrix );
	}
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
				m = SMatrix4x4(ta.mVectors[j], ta.mQuats[j]);
			}
		}
	}
}

void CSceneTeeth::renderTeethBills( int pack, float t, float relT, float cutAlpha, bool masks, float aspect)
{
	const int BILLSPERPACK = 3;
	static DataTexture texnames[TEETHPACKS][BILLSPERPACK] = {
		{ DataTexDentATrepasa, DataTexDentBPokero, DataTexDentCCorenjo },
		{ DataTexDentDSmogenias, DataTexDentEChanChan, DataTexDentFPerto },
		{ DataTexDentGZetal, DataTexDentHLaVina, DataTexDentICarina },
		{ DataTexDentATrepasa, DataTexDentATrepasa, DataTexDentATrepasa },
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

	SOBillboard* bill;

	if( pack != TEETHPACKS-1 ) {
		for( int i = 0; i < BILLSPERPACK; ++i ) {
			float billAlpha = 3.0f - fabsf(cutAlpha - (i+0.5f)/(BILLSPERPACK))*BILLSPERPACK*6;
			if( masks )
				billAlpha += 0.5f;
			if( billAlpha <= 0.0f )
				continue;
			
			// tooth image
			bill = &billboards_add();
			bill->x1 = 1.0f - BEDGE - BSIZE;
			bill->y1 = -1.0f + BEDGE*aspect;
			bill->x2 = bill->x1 + BSIZE;
			const float toothY = bill->y2 = bill->y1 + BSIZE*aspect / (ratios[pack][i]);
			bill->texture = g_data_tex[texnames[pack][i]]->view_tex;
			uint32_t c = SVector4(1, 1, 1, billAlpha).toRGBA();
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
			bill = &billboards_add();
			bill->x1 = 1.0f - BEDGE*0.5f - BSIZE - (uvs[2]-uvs[0]) * texelScale;
			bill->y1 = -1.0f + BEDGE*aspect*0.2f;
			bill->x2 = 1.0f - BEDGE*0.5f - BSIZE;
			bill->y2 = bill->y1 + (uvs[3]-uvs[1]) * texelScale*aspect;
			/*bill->x1 = 1.0f - BEDGE - (uvs[2]-uvs[0]) * texelScale;
			bill->y1 = toothY + BEDGE*aspect*0.2f;
			bill->x2 = 1.0f - BEDGE;
			bill->y2 = bill->y1 + (uvs[3]-uvs[1]) * texelScale*aspect;*/
			bill->color = c;
			bill->texture = g_data_tex[DataTexGreetings]->view_tex;
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
		bill = &billboards_add();
		bill->x1 = -GRP_WIDTH/2;
		bill->y1 = -GRP_HEIGHT/2;
		bill->x2 = GRP_WIDTH/2;
		bill->y2 = GRP_HEIGHT/2;
		SVector4 c = SVector4( 1, 1, 1, 0.0f );
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
			c.w = relT*3;
		} else {
			c.w = (relT-2);
		}
		bill->color = c.toRGBA();
		bill->texture = g_data_tex[DataTexBondigoDuo]->view_tex;
		bill->setWholeTexture();
	}

	pipeline_apply(masks ? pip_billboards : pip_billboardsClearDestAlpha);
	billboards_render();
	billboards_clear();
}

void CSceneTeeth::renderTeethLines(int pack, float t)
{
	assert(pack >= 0 && pack < TEETHPACKS);
	CTeethAnim& ta = *mAnimTeeth[pack];
	int nteeth = ta.getCount();

	float relT = ta.getRelTime(t);

	const int PATH_SIZE = PATH_FRAMES;
	SLinePoint path[PATH_SIZE];
	for (int i = 0; i < nteeth; ++i) {
		float a = 0.0f;
		float da = 1.0f / PATH_SIZE;
		for (int j = 0; j < PATH_SIZE; ++j, a += da) {
			path[j].pos = ta.mPaths[i][j];
			SVector4 c;
			c.x = 1.0f; c.y = 0.0f; c.z = 0.0f;
			float delta = a - relT;
			if (delta < 0.0f)
				delta *= 4;
			c.w = 0.75f - fabsf(delta) * 4;
			//if( delta>=0 )
			//	c.a = 0.0f;
			path[j].color = c.toRGBA();
		}
		pipeline_apply(pip_lines);
		gLineRenderer->renderStrip(PATH_SIZE, path, 0.05f);
	}
}

void CSceneTeeth::renderTeethStuff(int pack, float t, float cutAlpha, float aspect)
{
	assert(pack >= 0 && pack < TEETHPACKS);
	CTeethAnim& ta = *mAnimTeeth[pack];
	int nteeth = ta.getCount();
	int i;

	float relT = ta.getRelTime(t);

	// end previous pass, apply toon post-processing
	sg_end_pass();
	{
		sg_pass pass = {};
		pass.attachments.colors[0] = rt_full_toon.view_rt;
		pass.attachments.depth_stencil = {};
		pass.action.colors[0].load_action = SG_LOADACTION_DONTCARE;
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = rt_main_resolved.view_tex;
		binds.views[1] = g_data_tex[DataTexColorLuts]->view_tex;
		binds.samplers[0] = s_smp_linear_clamp;

		pipeline_apply(pip_postToon);
		sg_apply_bindings(binds);
		sg_draw(0, 3, 1);

		sg_end_pass();
	}

	// render teeth masks into 1/4th mask RT, to be blurred
	{
		sg_pass pass = {};
		pass.attachments.colors[0] = rt_4th_1.view_rt;
		pass.attachments.depth_stencil = {};
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0,0,0,0 };
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);
	}

	float toothMaskScale = 6.0f;
	const float SCALEUP_ALPHA = 0.03f;
	if (cutAlpha < SCALEUP_ALPHA) {
		toothMaskScale *= cutAlpha / SCALEUP_ALPHA;
	}
	/*if( pack==TEETHPACKS-1 && cutAlpha > 1-SCALEUP_ALPHA*3 ) {
		toothMaskScale *= (1 - cutAlpha)/(SCALEUP_ALPHA*3);
	} else*/ if (pack != TEETHPACKS - 1 && cutAlpha > 1 - SCALEUP_ALPHA) {
		toothMaskScale *= (1 - cutAlpha) / SCALEUP_ALPHA;
	}

	pipeline_apply(pip_renderWhite);
	sg_apply_uniforms(0, { &g_global_u, sizeof(g_global_u) });
	EntityUniformsVS uboVS = {};

	sg_bindings binds = {};

	CMesh* toothMaskMesh = g_data_mesh[DataMesh6Tooth1];

	binds.vertex_buffers[0] = toothMaskMesh->getVB();
	binds.index_buffer = toothMaskMesh->getIB();

	if( toothMaskScale > 0.001f ) {
		if( pack != TEETHPACKS-1 ) {
			for( i = 0; i < nteeth; ++i ) {
				SMesh& mesh = mMeshes[ ta.mTeethIdx[i] ];
				uboVS.mat = mesh.mesh->mMatrix;
				uboVS.mat.getAxisX() *= toothMaskScale;
				uboVS.mat.getAxisY() *= toothMaskScale * toothMaskScale*0.5f;
				uboVS.mat.getAxisZ() *= toothMaskScale;
				sg_apply_uniforms(1, { &uboVS, sizeof(uboVS) });
				sg_apply_bindings(binds);
				sg_draw(0, toothMaskMesh->getIndexCount(), 1);
			}
		} else {
			for( int pk = 0; pk < TEETHPACKS; ++pk ) {
				int ntth = mAnimTeeth[pk]->getCount();
				for( i = 0; i < ntth; ++i ) {
					SMesh& mesh = mMeshes[ mAnimTeeth[pk]->mTeethIdx[i] ];
					uboVS.mat = mesh.mesh->mMatrix;
					uboVS.mat.getAxisX() *= toothMaskScale;
					uboVS.mat.getAxisY() *= toothMaskScale * toothMaskScale*0.5f;
					uboVS.mat.getAxisZ() *= toothMaskScale;
					sg_apply_uniforms(1, { &uboVS, sizeof(uboVS) });
					sg_apply_bindings(binds);
					sg_draw(0, toothMaskMesh->getIndexCount(), 1);
				}
			}
		}
	}
	renderTeethBills( pack, t, relT, cutAlpha, true, aspect );

	sg_end_pass();

	pingPongBlur( BLOB_BLUR_PASSES );

	// composite
	{
		sg_pass pass = {};
		pass.attachments.colors[0] = rt_main_resolved.view_rt;
		pass.attachments.depth_stencil = {};
		pass.action.colors[0].load_action = SG_LOADACTION_LOAD;
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = rt_full_toon.view_tex;
		binds.views[1] = !(BLOB_BLUR_PASSES & 1) ? rt_4th_1.view_tex : rt_4th_2.view_tex;
		binds.views[2] = g_data_tex[DataTexAlphaEdge]->view_tex;
		binds.samplers[0] = s_smp_linear_clamp;

		pipeline_apply(pip_postComposeToon);
		sg_apply_bindings(binds);
		sg_draw(0, 3, 1);
	}

	/*
	// render paths
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
		pipeline_apply(pip_lines);
		gLineRenderer->renderStrip( PATH_SIZE, path, 0.05f );
	}
	*/
}

void CSceneTeeth::renderTeethUI( int pack, float t, float cutAlpha, float aspect)
{
	assert( pack >= 0 && pack < TEETHPACKS );
	CTeethAnim& ta = *mAnimTeeth[pack];
	float relT = ta.getRelTime( t );
	int nteeth = ta.getCount();
	renderTeethBills( pack, t, relT, cutAlpha, false, aspect);
}
