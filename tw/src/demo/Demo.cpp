#include "stdafx.h"

#include <dingus/math/Matrix4x4.h>
#include <dingus/math/Plane.h>
#include <dingus/math/MathUtils.h>

#include <dingus/gfx/geometry/DynamicVBManager.h>

#include "Demo.h"
#include "DemoResources.h"
#include "Effect.h"
#include "Scene.h"
#include "SceneTeeth.h"
#include "SceneOut.h"
#include "Glare.h"

#include "../util/TextureProjector.h"
#include "../util/LineRenderer.h"
#include "../system/MusicPlayer.h"
#include "../external/sokol_app.h"
#include "../external/sokol_glue.h"

//#define WITHMUSIC

#ifdef WITHMUSIC
	CMusicPlayer*	gMusicPlayer;
	#define DELAY_ACTION (0.25)
	#define GET_TIME (gMusicPlayer->getTime() + DELAY_ACTION)
#else
	double gDebugTime = 0.0;
	#define GET_TIME gDebugTime
	#define DELAY_ACTION (0.0)
#endif


// --------------------------------------------------------------------------

GlobalUniforms g_global_u;

enum eSceneMode { SC_OUTER = 0, SC_SCENE };

const int SCENES = 6;
CScene*	gScenes[SCENES];

const float gOutSections[SCENES+1] = { 0, 255, 473, 653, 939, 1150, 1350 };
CSceneOut*	gSceneOut;

eSceneMode	gSceneMode;
int			gSceneIndex; // scene/section
float		gSceneStartTime; // start time of current scene/section

// current scene cuts
struct SSceneCut {
	void	reset() {
		lastDone = -1.0f;
		doneCount = 0;
		curr = -1.0f;
		startTime = -1.0;
		sceneStartTime = -1.0;
		duration = -1.0f;
	}
	bool	isInCut() const { return startTime >= 0; }

	float	lastDone;	// last completed cut ntime 
	int		doneCount;	// completed cuts count
	float	curr;		// current cut ntime, or -1 if no cut
	double	startTime;	// current cut start time, or -1 if no cut
	double	sceneStartTime;
	float	duration;	// current cut dutaion, or -1 if no cut
};
SSceneCut	gCut;


// --------------------------------------------------------------------------

// line renderer, billboards
CLineRenderer*	gLineRenderer;
CRenderableOrderedBillboards*	gBillboardsNormal;
CRenderableOrderedBillboards*	gBillboardsNoDestA;

// camera
CCameraEntity	gCamera;
SMatrix4x4		gCameraViewProjMatrix;
// light
SMatrix4x4		gLightViewProjMatrix;
CCameraEntity	gLightCamera;
// reflective wals
CMeshEntity*	gWallMeshes[CFACE_COUNT];
const char*		gWallNames[CFACE_COUNT] = { "CubePX", "CubeNX", "CubePY", "CubeNY", "CubePZ", "CubeNZ" };
CCameraEntity	gWallCamera;

// post-processing fx
SVector4			gOverlayColor;
SVector4			gOverlayColor2;

// synch anim
CAnim*	gAnimSynch;


// --------------------------------------------------------------------------

static void gPreload()
{
	// tex...
	RGET_TEX("AlphaEdge");
	RGET_TEX("BondigoDuo");
	RGET_TEX("ColorLuts");
	RGET_TEX("CreditsA");
	RGET_TEX("CreditsB");
	RGET_TEX("DentATrepasa");
	RGET_TEX("DentBPokero");
	RGET_TEX("DentCCorenjo");
	RGET_TEX("DentDSmogenias");
	RGET_TEX("DentEChanChan");
	RGET_TEX("DentFPerto");
	RGET_TEX("DentGZetal");
	RGET_TEX("DentHLaVina");
	RGET_TEX("DentICarina");
	RGET_TEX("Greetings");
	RGET_TEX("Line1");
	RGET_TEX("SpotLight");
	// fx...
	effects_init();
	// anims...
	RGET_ANIM("Anim0"); RGET_ANIM("Anim1"); RGET_ANIM("Anim2");
	RGET_ANIM("Anim3"); RGET_ANIM("Anim4"); RGET_ANIM("Anim5");
	RGET_ANIM("Anim6");
	RGET_ANIM("Synch");
	RGET_ANIM("6/Axes");
	RGET_ANIM("6/TeethA"); RGET_ANIM("6/TeethB"); RGET_ANIM("6/TeethC"); RGET_ANIM("6/TeethD");
}

static sg_buffer s_ib_quads;

sg_sampler s_smp_linear_repeat;
sg_sampler s_smp_linear_clamp;
sg_sampler s_smp_point_clamp;

sokol_texture rt_main_aa, rt_main_z, rt_main_resolved;
sokol_texture rt_full_toon;
sokol_texture rt_4th_1, rt_4th_2;
sokol_texture rt_refl_px, rt_refl_py, rt_refl_pz, rt_refl_nx, rt_refl_ny, rt_refl_nz;
sokol_texture rt_refl_rt, rt_refl_z;
sokol_texture rt_shadow_rt;
sokol_texture rt_shadow_z;

static void ensure_render_targets()
{
	static int width = -1, height = -1;
	int newWidth = sapp_width();
	int newHeight = sapp_height();

	// screen size based textures
	if (width != newWidth || height != newHeight)
	{
		rt_main_aa.destroy();
		rt_main_z.destroy();
		rt_main_resolved.destroy();
		rt_full_toon.destroy();
		rt_4th_1.destroy();
		rt_4th_2.destroy();
		rt_refl_px.destroy();
		rt_refl_py.destroy();
		rt_refl_pz.destroy();
		rt_refl_nx.destroy();
		rt_refl_ny.destroy();
		rt_refl_nz.destroy();
		rt_refl_rt.destroy();
		rt_refl_z.destroy();

		width = newWidth;
		height = newHeight;
	}

	if (!rt_main_aa.valid())
	{
		sg_image_desc desc = {};
		desc.usage.color_attachment = true;

		// full size
		desc.width = width;
		desc.height = height;
		desc.sample_count = kMainAA;
		desc.pixel_format = SG_PIXELFORMAT_RGBA8;
		rt_main_aa.create(desc);
		desc.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.usage.depth_stencil_attachment = true;
		desc.usage.color_attachment = false;
		rt_main_z.create(desc);
		desc.pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.usage.depth_stencil_attachment = false;
		desc.usage.color_attachment = true;

		desc.sample_count = 1;
		desc.usage.resolve_attachment = true;
		rt_main_resolved.create(desc);
		desc.usage.resolve_attachment = false;

		rt_full_toon.create(desc);

		// 1/4th size
		desc.width = width / 4;
		desc.height = height / 4;
		rt_4th_1.create(desc);
		rt_4th_2.create(desc);

		// reflection textures
		desc.usage.color_attachment = false;
		desc.usage.resolve_attachment = true;
		desc.width = width / 2;
		desc.height = height / 2;
		rt_refl_px.create(desc);
		rt_refl_py.create(desc);
		rt_refl_pz.create(desc);
		rt_refl_nx.create(desc);
		rt_refl_ny.create(desc);
		rt_refl_nz.create(desc);
		// reflections color/depth MSAA surfaces
		desc.sample_count = kMainAA;
		desc.usage.color_attachment = true;
		desc.usage.resolve_attachment = false;
		rt_refl_rt.create(desc);
		desc.usage.color_attachment = false;
		desc.usage.depth_stencil_attachment = true;
		desc.pixel_format = SG_PIXELFORMAT_DEPTH;
		rt_refl_z.create(desc);
	}

	// fixed size textures
	if (!rt_shadow_rt.valid())
	{
		sg_image_desc desc = {};
		desc.sample_count = 1;
		desc.width = SZ_SHADOWMAP;
		desc.height = SZ_SHADOWMAP;

		desc.usage.color_attachment = true;
		desc.pixel_format = SG_PIXELFORMAT_RGBA8;
		rt_shadow_rt.create(desc);

		desc.usage.color_attachment = false;
		desc.usage.depth_stencil_attachment = true;
		desc.pixel_format = SG_PIXELFORMAT_DEPTH;
		rt_shadow_z.create(desc);
	}
}

bool demo_init()
{
	// samplers
	{
		sg_sampler_desc desc = {};
		desc.min_filter = desc.mag_filter = SG_FILTER_LINEAR;
		desc.wrap_u = desc.wrap_v = SG_WRAP_REPEAT;
		s_smp_linear_repeat = sg_make_sampler(&desc);
		desc.min_filter = desc.mag_filter = SG_FILTER_LINEAR;
		desc.wrap_u = desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
		s_smp_linear_clamp = sg_make_sampler(&desc);
		desc.min_filter = desc.mag_filter = SG_FILTER_NEAREST;
		s_smp_point_clamp = sg_make_sampler(&desc);
	}
	/*
	state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
		.shader = sg_make_shader(triangle_shader_desc(sg_query_backend())),
			.layout = {
				.attrs = {
					[ATTR_triangle_position] .format = SG_VERTEXFORMAT_FLOAT3,
					[ATTR_triangle_color0].format = SG_VERTEXFORMAT_FLOAT4
				}
		},
	});

	state.pass_action = (sg_pass_action){
		.colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 1.0f } }
	};
	*/

	//
	// resources

	CTextureBundle::init("data/tex/");
	CMeshBundle::init("data/mesh/");
	CAnimationBundle::init("data/anim/");

	dynamic_vb_init(2 * 1024 * 1024);

	// ------------------------------
	//  shared index buffers, etc
	
	{
		// Fills 6 indices for a quad in this way:
		//	0----13
		//	|   / |
		//	| /   |
		//	25----4
		// Here vertex 0 is upper left, 1 is upper right, 2 is lower right, 3 is lower left (so clockwise).
		uint16_t quadIBData[6] = { 0, 1, 3, 1, 2, 3 };
		sg_buffer_desc desc = {};
		desc.usage.index_buffer = true;
		desc.data.ptr = quadIBData;
		desc.data.size = 6 * 2;
		desc.usage.immutable = true;
		s_ib_quads = sg_make_buffer(&desc);
		ASSERT_MSG(sg_query_buffer_state(s_ib_quads) == SG_RESOURCESTATE_VALID, "Failed to create Quad IB");
	}

	gBillboardsNormal = new CRenderableOrderedBillboards(s_ib_quads, s_smp_linear_clamp);
	gBillboardsNoDestA = new CRenderableOrderedBillboards(s_ib_quads, s_smp_linear_clamp);
	gLineRenderer = new CLineRenderer();

	// --------------------------------
	// RTs

	ensure_render_targets();

	// --------------------------------
	// preload

	gPreload();

	// --------------------------------
	// synch

	gAnimSynch = new CAnim( "Synch", "BeatBox", CAnim::POSITION );

	// --------------------------------
	// scenes

	gSceneOut = new CSceneOut(0);
	gSceneOut->initialize();
	for( int s = 0; s < SCENES; ++s ) {
		if( s==SCENES-1 )
			gScenes[s] = new CSceneTeeth(s+1);
		else
			gScenes[s] = new CScene(s+1);
		gScenes[s]->initialize();
		gScenes[s]->addStaticMesh( "Cube" );
		for( int i = 0; i < CFACE_COUNT; ++i )
			gWallMeshes[i] = gScenes[s]->addStaticMesh( gWallNames[i] );
	}

	gSceneOut->addCut( 110 );
	gSceneOut->addCut( 160 );
	gSceneOut->addCut( 220 );

	gScenes[1]->addCut( 218 );
	gScenes[2]->addCut( 188 );
	gScenes[2]->addCut( 311 );
	gScenes[3]->addCut( 259 );
	gScenes[4]->addCut( 154 );
	
	gScenes[5]->addCut( 351 );
	gScenes[5]->addCut( 493 );
	gScenes[5]->addCut( 748 );
	gScenes[5]->addCut( 865 );

	gSceneMode = SC_OUTER;
	gSceneIndex = 0;

#ifdef WITHMUSIC
	gMusicPlayer = new CMusicPlayer();
	gMusicPlayer->play( "data/music.ogg" );
	gSceneStartTime = float(GET_TIME - DELAY_ACTION);
#else
	gSceneMode = SC_SCENE;
	gSceneIndex = 5;
	gSceneStartTime = -15.0f;
#endif

	gCut.reset();

	return true;
}

static void ReflectMatrix(SMatrix4x4& out, const SPlane& plane)
{
	SPlane nplane = plane.normalize();

	out.identify();
	out.m[0][0] = 1.0f - 2.0f * nplane.a * nplane.a;
	out.m[0][1] = -2.0f * nplane.a * nplane.b;
	out.m[0][2] = -2.0f * nplane.a * nplane.c;
	out.m[1][0] = -2.0f * nplane.a * nplane.b;
	out.m[1][1] = 1.0f - 2.0f * nplane.b * nplane.b;
	out.m[1][2] = -2.0f * nplane.b * nplane.c;
	out.m[2][0] = -2.0f * nplane.c * nplane.a;
	out.m[2][1] = -2.0f * nplane.c * nplane.b;
	out.m[2][2] = 1.0f - 2.0f * nplane.c * nplane.c;
	out.m[3][0] = -2.0f * nplane.d * nplane.a;
	out.m[3][1] = -2.0f * nplane.d * nplane.b;
	out.m[3][2] = -2.0f * nplane.d * nplane.c;
}

void gRenderWallReflections()
{
	assert( gSceneMode == SC_SCENE );
	SVector3 planePos[CFACE_COUNT] = {
		SVector3(10,0,0), SVector3(-10,0,0),
		SVector3(0,10,0), SVector3(0,-10,0),
		SVector3(0,0,10), SVector3(0,0,-10),
	};
	SVector3 planeNrm[CFACE_COUNT] = {
		SVector3(-1,0,0), SVector3(1,0,0),
		SVector3(0,-1,0), SVector3(0,1,0),
		SVector3(0,0,-1), SVector3(0,0,1),
	};

	for( int currWall = 0; currWall < CFACE_COUNT; ++currWall )
	{
		gWallMeshes[currWall]->updateMatrices();
		if( gWallMeshes[currWall]->frustumCull(gCameraViewProjMatrix) )
			continue;

		SPlane reflPlane( planePos[currWall] + planeNrm[currWall]*0.05f, planeNrm[currWall] );
		SMatrix4x4 reflectMat;
		ReflectMatrix(reflectMat, reflPlane);
		
		gWallCamera.mMatrix = gCamera.mMatrix * reflectMat;
		gWallCamera.setProjFrom( gCamera );
		gWallCamera.setOntoRenderContext();
		gComputeTextureProjection(gRenderCam.getCameraMatrix(), gLightViewProjMatrix, g_global_u.matShadowProj);

		sg_pass pass = {};
		pass.attachments.depth_stencil = rt_refl_z.view_z;
		sg_view resolve_views[CFACE_COUNT] = {
			rt_refl_px.view_resolve, rt_refl_nx.view_resolve,
			rt_refl_py.view_resolve, rt_refl_ny.view_resolve,
			rt_refl_pz.view_resolve, rt_refl_nz.view_resolve,
		};
		pass.attachments.colors[0] = rt_refl_rt.view_rt;
		pass.attachments.resolves[0] = resolve_views[currWall];
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0, 0, 0, 1 };
		pass.action.depth.load_action = SG_LOADACTION_CLEAR;
		pass.action.depth.clear_value = 1.0f;

		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = rt_shadow_rt.view_tex;
		binds.views[1] = RGET_TEX("SpotLight")->view_tex;
		binds.samplers[0] = s_smp_point_clamp;
		binds.samplers[1] = s_smp_linear_clamp;
		gScenes[gSceneIndex]->render(RM_RECV_LO, &binds);

		sg_end_pass();
	}
}

void gRenderShadowMap()
{
	assert( gSceneMode == SC_SCENE );

	gLightCamera.setProjectionParams( M_PI/3, 1.0f, 0.2f, 35.0f );
	gLightCamera.setOntoRenderContext();
	gLightViewProjMatrix = gRenderCam.getViewProjMatrix();

	sg_pass pass = {};
	pass.attachments.colors[0] = rt_shadow_rt.view_rt;
	pass.attachments.depth_stencil = rt_shadow_z.view_z;
	pass.action.colors[0].store_action = SG_STOREACTION_STORE;
	pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
	pass.action.colors[0].clear_value = { 0, 0, 0, 0 };
	pass.action.depth.load_action = SG_LOADACTION_CLEAR;
	pass.action.depth.clear_value = 1.0f;

	sg_begin_pass(&pass);
	sg_apply_viewport(1, 1, SZ_SHADOWMAP-2, SZ_SHADOWMAP-2, true);

	sg_bindings binds = {};
	gScenes[gSceneIndex]->render(RM_SHADOW, &binds);

	sg_end_pass();

}

static float gGetSceneDur()
{
	assert( gSceneIndex >= 0 && gSceneIndex < SCENES );
	if( gSceneMode == SC_OUTER ) {
		const float framesPerSec = (!gSceneIndex) ? 48.0f : 56.0f;
		const float secLen = gOutSections[gSceneIndex+1] - gOutSections[gSceneIndex];
		return secLen / framesPerSec;
	} else {
		const float framesPerSec = 24.0f;
		return gScenes[gSceneIndex]->getLength() / framesPerSec;
	}
}

static bool gIsLastCut()
{
	return
		gSceneMode==SC_SCENE &&
		gSceneIndex==SCENES-1 &&
		gCut.isInCut() &&
		gCut.doneCount == gScenes[gSceneIndex]->getCutCount()-1;
}

static void gRenderCredits( float cutAlpha )
{
	int i;
	const bool outer = (gSceneMode==SC_OUTER);

	CRenderableOrderedBillboards& bills = *gBillboardsNoDestA;
	bills.clear();

	const float CENTER_X = outer ? -0.02f : -0.2f;
	const float TOP_Y = outer ? 0.6f : 0.7f;
	const float HEIGHT = outer ? (0.09f+gCut.doneCount*0.02f) : 0.15f;
	const float ASPECT = sapp_widthf() / sapp_heightf();
	const float FLY_ALPHA = 0.3f;
	const float UV_HEIGHT = outer ? (80.0f/512.0f) : (96.0f/512.0f);
	const float WIDTH = HEIGHT/ASPECT * (outer ? (850.0f/64.0f) : (512.0f/96.0f));

	int line;
	float lineWidth = gCamera.getZNear() * 0.026f * HEIGHT;
	if( !outer ) {
		line = gSceneIndex-1;
		if( gCut.doneCount > 0 || gSceneIndex > 2 )
			++line;
	} else {
		line = gCut.doneCount;
	}
	
	float edgeDistX = 0.0f;
	if( cutAlpha < FLY_ALPHA )
		edgeDistX = (FLY_ALPHA-cutAlpha);
	else if( cutAlpha > 1-FLY_ALPHA ) {
		edgeDistX = ((1-FLY_ALPHA) - cutAlpha)*1.5f;
	}
	edgeDistX *= 2.0f;
	edgeDistX = edgeDistX * fabsf(edgeDistX);

	//
	// lines...

	const int LINE_PTS = 200;
	const float SLOPEEND_X = CENTER_X - 0.03f/ASPECT;
	const float SLOPE_X = 0.06f/ASPECT;
	const float RISE_Y = HEIGHT*22.0f/96.0f;
	const float LINE_Y = TOP_Y + HEIGHT * ( outer ? (61.0f/80.0f) : (59.0f/96.0f));
	const float LINEFLY_ALPHA = FLY_ALPHA*0.7f;
	SLinePoint linepts[LINE_PTS];
	for( i = 0; i < LINE_PTS; ++i ) {
		float ptX = (float)i / (LINE_PTS-1) * 2 - 1;
		float ptY = LINE_Y;
		if( ptX > SLOPEEND_X ) {
			ptY -= RISE_Y;
		} else if( ptX > SLOPEEND_X-SLOPE_X ) {
			ptY -= RISE_Y * (ptX-SLOPEEND_X+SLOPE_X)/(SLOPE_X);
		}

		linepts[i].pos = gCamera.mMatrix.getOrigin() + gCamera.getWorldRay( ptX, ptY ) * (gCamera.getZNear()*1.01f);
		float ptA = 0.0f;
		if( cutAlpha > LINEFLY_ALPHA && cutAlpha < 1-LINEFLY_ALPHA ) {
			const float subAlpha = (cutAlpha-LINEFLY_ALPHA)/(1-LINEFLY_ALPHA*2);
			float subCenterX = subAlpha*2-1;
			//if( fabsf(subCenterX) > 0.2f )
			//	subCenterX *= 2.0f;
			subCenterX *= fabsf(subCenterX);
			subCenterX -= (CENTER_X+SLOPEEND_X)/2;
			ptA = 1.0f;
			float dist = fabsf(ptX - subCenterX);
			if( dist > 0.6f )
				ptA = 1.0f - (dist-0.6f)*3;
		}
		linepts[i].color = SVector4(1,0,0,ptA).toRGBA();
	}
	effect_apply(fx_linesNoAa);
	gLineRenderer->renderStrip( LINE_PTS, linepts, lineWidth );

	//
	// bills...

	sokol_texture* btex = RGET_TEX(outer?"CreditsB":"CreditsA");
	const int BILLCOUNT = 16;
	const uint32_t BILLCOL = 0x18ffffff;
	const float SECOND_BILL_LOWER = outer ? HEIGHT*0.33f : 0.0f;
	for( i = 0; i < BILLCOUNT; ++i ) {
		const float edgeBill = edgeDistX * (1.0f + (float)i/BILLCOUNT*0.5f );
		SOBillboard* b;
		// left...
		b = &bills.addBill();
		b->x1 = CENTER_X - edgeBill - WIDTH;	b->y1 = TOP_Y;
		b->x2 = b->x1 + WIDTH;					b->y2 = b->y1 + HEIGHT;
		if( outer ) {
			b->tu1 = 0.0f;	b->tv1 = line*2*UV_HEIGHT;
			b->tu2 = 850.0f/1024.0f;	b->tv2 = (line*2+1)*UV_HEIGHT;
		} else {
			b->tu1 = 0.0f;	b->tv1 = line * UV_HEIGHT;
			b->tu2 = 0.5f;	b->tv2 = (line+1) * UV_HEIGHT;
		}

		b->texture = btex->view_tex;
		b->color = BILLCOL;
		// right...
		b = &bills.addBill();
		b->x1 = CENTER_X + edgeBill;			b->y1 = TOP_Y + SECOND_BILL_LOWER;
		b->x2 = b->x1 + WIDTH;					b->y2 = b->y1 + HEIGHT;
		if( outer ) {
			b->tu1 = 0.0f;	b->tv1 = (line*2+1)*UV_HEIGHT;
			b->tu2 = 850.0f/1024.0f;	b->tv2 = (line*2+2)*UV_HEIGHT;
		} else {
			b->tu1 = 0.5f;	b->tv1 = line * UV_HEIGHT;
			b->tu2 = 1.0f;	b->tv2 = (line+1) * UV_HEIGHT;
		}
		b->texture = btex->view_tex;
		b->color = BILLCOL;
	}

	effect_apply(fx_billboards);
	bills.render();
}

bool demo_update()
{
	ensure_render_targets();

	double t = GET_TIME;
	const double musicTime = t - DELAY_ACTION;
	float st = float(t - gSceneStartTime);
	float sceneDur = gGetSceneDur();

	gBillboardsNormal->clear();
	gBillboardsNoDestA->clear();
	
	float znear = 0.1f, zfar = 50.0f;
	float camfov = M_PI/4;

	//
	// current scene and scene's alpha

	CScene* scene = NULL;
	float sceneAlpha = 0.0f;
	if( gSceneMode == SC_OUTER ) {
		scene = gSceneOut;
		float a0 = gOutSections[gSceneIndex] / gSceneOut->getLength();
		float a1 = gOutSections[gSceneIndex+1] / gSceneOut->getLength();
		//sceneAlpha = clamp( st/sceneDur, 0.0f, 1.0f );
		sceneAlpha = st/sceneDur;
		sceneAlpha = a0 + (a1-a0) * sceneAlpha;
	} else {
		scene = gScenes[gSceneIndex];
		sceneAlpha = st/sceneDur;
	}

	//
	// cut-pauses, adjust scene alpha in them

	const float CUT_DUR = 3.75f;
	if( gCut.isInCut() ) {
		if( !gIsLastCut() ) {
			double cutDur = t - gCut.startTime;
			float cutAlpha = float(cutDur / gCut.duration);
			sceneAlpha = gCut.curr + cutAlpha*0.01f;
			gSceneStartTime = float(gCut.sceneStartTime + cutDur);
			if( cutDur >= gCut.duration ) {
				float sdur = sceneDur;
				if( gSceneMode == SC_OUTER )
					sdur /= gOutSections[1] / gSceneOut->getLength();
				gSceneStartTime -= (sceneAlpha-gCut.curr) * sdur;
				++gCut.doneCount;
				gCut.lastDone = gCut.curr;
				gCut.startTime = -1.0;
				gCut.curr = -1.0f;
			}
		}
	} else {
		float pastCut = scene->getPastCut( sceneAlpha );
		if( gSceneMode == SC_OUTER && gSceneIndex != 0 )
			pastCut = -1.0f;
		if( pastCut > gCut.lastDone ) {
			gCut.startTime = t;
			gCut.curr = pastCut;
			gCut.duration = CUT_DUR;
			if( gSceneIndex==SCENES-1 )
				gCut.duration *= 2.0f;
			gCut.sceneStartTime = gSceneStartTime;
			//if( !gIsLastCut() )
			//	gSceneStartTime += gCut.duration;
		}
	}

	//
	// evaluate scene at alpha

	scene->evaluate( sceneAlpha, gLightCamera.mMatrix, gCamera.mMatrix );
	g_global_u.lightPos = gLightCamera.mMatrix.getOrigin();
	g_global_u.lightDir = gLightCamera.mMatrix.getAxisZ();

	//
	// adjust z-ranges in outer scene

	if( gSceneMode == SC_OUTER ) {
		const float ext = 100.0f;
		const SVector3& camo = gCamera.mMatrix.getOrigin();
		float camDist = camo.length();
		znear = (camDist < ext+0.1f) ? 0.1f : (camDist-ext);
		zfar = camDist + ext;
		camfov = M_PI/5;
	}

	//
	// render

	gCamera.setProjectionParams( camfov, sapp_widthf() / sapp_heightf(), znear, zfar );

	if( gSceneMode != SC_OUTER ) {
		gRenderShadowMap();
		gRenderWallReflections();
	}

	gCamera.setOntoRenderContext();
	gComputeTextureProjection(gRenderCam.getCameraMatrix(), gRenderCam.getViewProjMatrix(), g_global_u.matViewTexProj);
	gComputeTextureProjection(gRenderCam.getCameraMatrix(), gLightViewProjMatrix, g_global_u.matShadowProj);
	gCameraViewProjMatrix = gRenderCam.getViewProjMatrix();

	// Begin main MSAA 3D rendering pass
	{
		sg_pass pass = {};
		pass.attachments.colors[0] = rt_main_aa.view_rt;
		pass.attachments.depth_stencil = rt_main_z.view_z;
		pass.attachments.resolves[0] = rt_main_resolved.view_resolve;
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0, 0, 0, 0 };
		pass.action.depth.load_action = SG_LOADACTION_CLEAR;
		pass.action.depth.clear_value = 1.0f;
		sg_begin_pass(&pass);
	}

	sg_bindings binds = {};
	if( gSceneMode != SC_OUTER )
	{
		binds.views[0] = rt_shadow_rt.view_tex;
		binds.views[1] = RGET_TEX("SpotLight")->view_tex;
		binds.samplers[0] = s_smp_point_clamp;
		binds.samplers[1] = s_smp_linear_clamp;
		gScenes[gSceneIndex]->render(RM_RECV_HI, &binds);

		binds.samplers[0] = s_smp_linear_clamp;
		gScenes[gSceneIndex]->render(RM_REFLECTIVE, &binds);

		// last scene teeth lines
		if (gCut.isInCut() && gSceneIndex == SCENES - 1)
		{
			CSceneTeeth* steeth = (CSceneTeeth*)gScenes[gSceneIndex];
			steeth->renderTeethLines(gCut.doneCount, sceneAlpha);
		}
	}
	else
	{
		gSceneOut->render(RM_HI, &binds);
	}

	// End main 3D MSAA rendering; resolves into rt_main_resolved.
	// Start render pass into that, without depth buffer.
	sg_end_pass();
	{
		sg_pass pass = {};
		pass.attachments.colors[0] = rt_main_resolved.view_rt;
		pass.attachments.depth_stencil = {};
		pass.action.colors[0].load_action = SG_LOADACTION_LOAD;
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		pass.action.depth.clear_value = 1.0f;
		sg_begin_pass(&pass);
	}

	//
	// render credits

	if( gCut.isInCut() && gSceneIndex!=SCENES-1 ) {
		float cutAlpha = float((t - gCut.startTime) / gCut.duration);
		gRenderCredits( cutAlpha );
	}
	
	{
		//
		// overlay for beats...

		const double SYNCH_LEN = 6361.0;
		const double OVERSAMPLE = 1.0 / SYNCH_LEN;
		double synt = musicTime / (SYNCH_LEN/30.0);
		SVector3 beatPos0, beatPos1, beatPos2;
		gAnimSynch->samplePos( float(synt - OVERSAMPLE), beatPos0 );
		gAnimSynch->samplePos( float(synt), beatPos1 );
		gAnimSynch->samplePos( float(synt + OVERSAMPLE), beatPos2 );
		float beat = (beatPos0.y + beatPos1.y + beatPos2.y) * 0.08f;
		beat = clamp( beat, 0.0f, 1.0f );
		gOverlayColor2.set( beat, beat, beat, beat );
		if( beat > 0.001f )
		{
			effect_apply(fx_overlay2);
			sg_apply_uniforms(0, { &gOverlayColor2, sizeof(gOverlayColor2) });
			sg_draw(0, 4, 1);
		}
	}

	//
	// overlay for scene/cut transitions

	const float OVERLAY_DUR = 0.3f;
	gOverlayColor.set( 0, 0, 0, 0 );
	if( sceneDur-st < OVERLAY_DUR ) {
		gOverlayColor.w = 1.0f - (sceneDur-st)/OVERLAY_DUR;
	} else if( st < OVERLAY_DUR ) {
		gOverlayColor.w = 1.0f - st/OVERLAY_DUR;
	}

	// overlay for cuts...
	const float OVERLAY_CUT_DUR = 0.17f;
	const float OVERLAY_CUT_HALF = OVERLAY_CUT_DUR * 0.5f;
	if( gCut.isInCut() ) {
		float cutTime = float(t - gCut.startTime);
		if( gSceneIndex == SCENES-1 ) {
			CSceneTeeth* steeth = (CSceneTeeth*)gScenes[gSceneIndex];
			float aspect = sapp_widthf() / sapp_heightf();
			steeth->renderTeethStuff( gCut.doneCount, sceneAlpha, cutTime/gCut.duration, aspect);
			steeth->renderTeethUI( gCut.doneCount, sceneAlpha, cutTime/gCut.duration, aspect);
		}

		if( cutTime < OVERLAY_CUT_DUR ) {
			gOverlayColor.w = 1.0f - fabsf( (cutTime-OVERLAY_CUT_HALF)/OVERLAY_CUT_HALF );
		} else if( gCut.duration - cutTime < OVERLAY_CUT_DUR && !gIsLastCut() ) {
			gOverlayColor.w = 1.0f - fabsf( (gCut.duration-cutTime-OVERLAY_CUT_HALF)/OVERLAY_CUT_HALF );
		}
		gOverlayColor.w *= 0.5f;
	}

	gOverlayColor.w = clamp( gOverlayColor.w, 0.0f, 1.0f );
	if( gOverlayColor.w > 0.001f ) {
		effect_apply(fx_overlay);
		sg_apply_uniforms(0, { &gOverlayColor, sizeof(gOverlayColor) });
		sg_draw(0, 4, 1);
	}

	sg_end_pass();

	//
	// bloom

	renderBloom();

	//
	// manage scene transitions
	bool continue_exec = true;

	if( st >= sceneDur ) {
		if( gSceneMode==SC_OUTER ) {
			gSceneMode = SC_SCENE;
		} else {
			gSceneMode = SC_OUTER;
			++gSceneIndex;
			if( gSceneIndex >= SCENES ) {
				//gSceneIndex = 0;
				// end!
				gSceneIndex = SCENES - 1;
				continue_exec = false;
			}
		}
		gSceneStartTime = float(t);
		gCut.reset();
	}

	sg_commit();

	return continue_exec;
}

void demo_shutdown()
{
#ifdef WITHMUSIC
	delete gMusicPlayer;
#endif

	rt_main_aa.destroy();
	rt_main_z.destroy();
	rt_main_resolved.destroy();
	rt_full_toon.destroy();
	rt_4th_1.destroy();
	rt_4th_2.destroy();
	rt_refl_px.destroy();
	rt_refl_py.destroy();
	rt_refl_pz.destroy();
	rt_refl_nx.destroy();
	rt_refl_ny.destroy();
	rt_refl_nz.destroy();
	rt_refl_rt.destroy();
	rt_refl_z.destroy();
	rt_shadow_rt.destroy();
	rt_shadow_z.destroy();
	
	for( int i = 0; i < SCENES; ++i )
		delete gScenes[i];
	delete gSceneOut;
	delete gLineRenderer;
	delete gBillboardsNormal;
	delete gBillboardsNoDestA;
	delete gAnimSynch;

	dynamic_vb_shutdown();
	sg_destroy_buffer(s_ib_quads);

	CTextureBundle::finalize();
	CMeshBundle::finalize();
	CAnimationBundle::finalize();
}

void demo_event(const sapp_event* evt)
{
	static bool spaceDown = false;
	if (evt->type == SAPP_EVENTTYPE_KEY_DOWN)
	{
#ifndef WITHMUSIC
		float dt = 1.0f / 60.0f;
		if (evt->key_code == SAPP_KEYCODE_LEFT)		gDebugTime -= dt * 1.0f;
		if (evt->key_code == SAPP_KEYCODE_RIGHT)	gDebugTime += dt * 1.0f;
		if (evt->key_code == SAPP_KEYCODE_PAGE_UP)	gDebugTime -= dt * 5.0f;
		if (evt->key_code == SAPP_KEYCODE_PAGE_DOWN)		gDebugTime += dt * 5.0f;
		if (evt->key_code == SAPP_KEYCODE_SPACE && !spaceDown) {
			gDebugTime += 1.0f;
			spaceDown = true;
		}
#endif
		if (evt->key_code == SAPP_KEYCODE_ESCAPE) {
			sapp_quit();
		}
	}
	if (evt->type == SAPP_EVENTTYPE_KEY_UP)
	{
#ifndef WITHMUSIC
		if (evt->key_code == SAPP_KEYCODE_SPACE)
			spaceDown = false;
#endif
	}
}
