#include "stdafx.h"

#include "Demo.h"
#include "DemoResources.h"
#include "Scene.h"
#include "SceneTeeth.h"
#include "SceneOut.h"
#include "Glare.h"

#include "../util/TextureProjector.h"
#include "../util/LineRenderer.h"
#include "../system/MusicPlayer.h"

#include <dingus/renderer/RenderableMesh.h>
#include <dingus/math/Matrix4x4.h>
#include <dingus/math/Plane.h>
#include <dingus/math/MathUtils.h>
#include <dingus/resource/IndexBufferFillers.h>
#include <dingus/resource/CubeTextureBundle.h>


#define WITHMUSIC

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


int			gGlobalCullMode;
SVector4	gScreenFixUVs;

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
SMatrix4x4		gLightShadowMatrix;
CCameraEntity	gLightCamera;
// reflective wals
CMeshEntity*	gWallMeshes[CFACE_COUNT];
const char*		gWallNames[CFACE_COUNT] = { "CubePX", "CubeNX", "CubePY", "CubeNY", "CubePZ", "CubeNZ" };
const char*		gWallTexs[CFACE_COUNT] = { RT_REFL_PX, RT_REFL_NX, RT_REFL_PY, RT_REFL_NY, RT_REFL_PZ, RT_REFL_NZ };
CCameraEntity	gWallCamera;

SMatrix4x4		gViewTexProjMatrix;

// post-processing fx
CBloomPostProcess*	gPPSBloom;
CRenderableMesh*	gPPSOverlayMesh;
CRenderableMesh*	gPPSOverlayMesh2;
SVector4			gOverlayColor;
SVector4			gOverlayColor2;

// synch anim
CAnim*	gAnimSynch;


// --------------------------------------------------------------------------

CDemo::CDemo()
{
}

bool CDemo::appCheckDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT bbFormat ) const
{
	if( caps.PixelShaderVersion < D3DPS_VERSION(1,1) )
		return false;
	if( caps.PixelShaderVersion < D3DPS_VERSION(2,0) ) {
		if( !(behavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return false;
	}
	return true;
}

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
	RGET_FX("billboards");
	RGET_FX("billboardsNoDestA");
	RGET_FX("caster");
	RGET_FX("compositeAdd");
	RGET_FX("compositeAlpha");
	RGET_FX("filterBloom");
	RGET_FX("filterToon");
	RGET_FX("lines");
	RGET_FX("noshadowHi");
	RGET_FX("overlay2");
	RGET_FX("overlay");
	RGET_FX("receiverHi");
	RGET_FX("receiverLo");
	RGET_FX("reflective");
	// anims...
	RGET_ANIM("Anim0"); RGET_ANIM("Anim1"); RGET_ANIM("Anim2");
	RGET_ANIM("Anim3"); RGET_ANIM("Anim4"); RGET_ANIM("Anim5");
	RGET_ANIM("Anim6");
	RGET_ANIM("Synch");
	RGET_ANIM("6/Axes");
	RGET_ANIM("6/TeethA"); RGET_ANIM("6/TeethB"); RGET_ANIM("6/TeethC"); RGET_ANIM("6/TeethD");
}

void CDemo::appInitialize()
{
	CSharedTextureBundle& stb = CSharedTextureBundle::getInstance();
	CSharedSurfaceBundle& ssb = CSharedSurfaceBundle::getInstance();

	// ------------------------------
	//  shared index buffers, etc

	CIndexBufferBundle::getInstance().registerIB( RID_IB_QUADSTRIP, 6*1000, D3DFMT_INDEX16, *(new CIBFillerQuadStrip()) );
	CIndexBufferBundle::getInstance().registerIB( RID_IB_QUADS, 6*10000, D3DFMT_INDEX16, *(new CIBFillerQuads()) );
	gBillboardsNormal = new CRenderableOrderedBillboards( *CIndexBufferBundle::getInstance().getResourceById(RID_IB_QUADS), "tBase" );
	gBillboardsNormal->getParams().setEffect( *RGET_FX("billboards") );
	gBillboardsNoDestA = new CRenderableOrderedBillboards( *CIndexBufferBundle::getInstance().getResourceById(RID_IB_QUADS), "tBase" );
	gBillboardsNoDestA->getParams().setEffect( *RGET_FX("billboardsNoDestA") );
	gLineRenderer = new CLineRenderer( *CIndexBufferBundle::getInstance().getResourceById(RID_IB_QUADSTRIP) );
	gLineRenderer->getRenderable().getParams().setEffect( *RGET_FX("lines") );
	gLineRenderer->getRenderable().getParams().addTexture( "tBase", *RGET_TEX("Line1") );

	// --------------------------------
	// general use RTs

	ITextureCreator* rtcreatFull = new CScreenBasedTextureCreator(
		1.00f, 1.00f, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT );
	ITextureCreator* rtcreat4th = new CScreenBasedTextureCreator(
		0.25f, 0.25f, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT );
	stb.registerTexture( RT_FULLSCREEN_1, rtcreatFull );
	stb.registerTexture( RT_FULLSCREEN_2, rtcreatFull );
	stb.registerTexture( RT_4thSCREEN_1, rtcreat4th );
	stb.registerTexture( RT_4thSCREEN_2, rtcreat4th );
	ssb.registerSurface( RT_FULLSCREEN_1, new CTextureLevelSurfaceCreator(*RGET_STEX(RT_FULLSCREEN_1),0) );
	ssb.registerSurface( RT_FULLSCREEN_2, new CTextureLevelSurfaceCreator(*RGET_STEX(RT_FULLSCREEN_2),0) );
	ssb.registerSurface( RT_4thSCREEN_1, new CTextureLevelSurfaceCreator(*RGET_STEX(RT_4thSCREEN_1),0) );
	ssb.registerSurface( RT_4thSCREEN_2, new CTextureLevelSurfaceCreator(*RGET_STEX(RT_4thSCREEN_2),0) );

	// --------------------------------
	// reflection RTs

	ITextureCreator* rtcreatRefl = new CScreenBasedTextureCreator(
		SZ_REFL_REL, SZ_REFL_REL, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT );
	ISurfaceCreator* rtcreatReflRT = new CScreenBasedSurfaceCreator(
		SZ_REFL_REL, SZ_REFL_REL, false, D3DFMT_A8R8G8B8, true );
	ISurfaceCreator* rtcreatReflZ = new CScreenBasedSurfaceCreator(
		SZ_REFL_REL, SZ_REFL_REL, true, D3DFMT_D16, true );
	stb.registerTexture( RT_REFL_PX, rtcreatRefl );
	stb.registerTexture( RT_REFL_NX, rtcreatRefl );
	stb.registerTexture( RT_REFL_PY, rtcreatRefl );
	stb.registerTexture( RT_REFL_NY, rtcreatRefl );
	stb.registerTexture( RT_REFL_PZ, rtcreatRefl );
	stb.registerTexture( RT_REFL_NZ, rtcreatRefl );
	ssb.registerSurface( RT_REFLRT, rtcreatReflRT );
	ssb.registerSurface( RT_REFLZ, rtcreatReflZ );

	// --------------------------------
	// shadow map RTs

	ITextureCreator* rtcreatShadow = new CFixedTextureCreator(
		SZ_SHADOWMAP, SZ_SHADOWMAP, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT );
	ISurfaceCreator* rtcreatShadowZ = new CFixedSurfaceCreator(
		SZ_SHADOWMAP, SZ_SHADOWMAP, true, D3DFMT_D16 );
	stb.registerTexture( RT_SHADOWRT, rtcreatShadow );
	ssb.registerSurface( RT_SHADOWRT, new CTextureLevelSurfaceCreator(*RGET_STEX(RT_SHADOWRT),0) );
	ssb.registerSurface( RT_SHADOWZ, rtcreatShadowZ );

	// --------------------------------
	// common params

	gGlobalCullMode = D3DCULL_CW;
	G_RCTX->getGlobalParams().addIntRef( "iCull", &gGlobalCullMode );
	G_RCTX->getGlobalParams().addMatrix4x4Ref( "mViewTexProj", gViewTexProjMatrix );
	G_RCTX->getGlobalParams().addMatrix4x4Ref( "mShadowProj", gLightShadowMatrix );
	G_RCTX->getGlobalParams().addVector3Ref( "vLightPos", gLightCamera.mMatrix.getOrigin() );
	G_RCTX->getGlobalParams().addVector3Ref( "vLightDir", gLightCamera.mMatrix.getAxisZ() );
	G_RCTX->getGlobalParams().addTexture( "tShadow", *RGET_STEX(RT_SHADOWRT) );
	G_RCTX->getGlobalParams().addTexture( "tCookie", *RGET_TEX("SpotLight") );
	//G_RCTX->getGlobalParams().addCubeTexture( "tNormalizer", *CCubeTextureBundle::getInstance().getResourceById("Normalizer") );

	// --------------------------------
	// preload

	gPreload();

	// --------------------------------
	// post process fx

	gPPSBloom = new CBloomPostProcess();
	{
		gPPSOverlayMesh = new CRenderableMesh( *RGET_MESH("billboard"), 0 );
		CEffectParams& ep = gPPSOverlayMesh->getParams();
		ep.setEffect( *RGET_FX("overlay") );
		ep.addVector4Ref( "vFixUV", gScreenFixUVs );
		ep.addVector4Ref( "vColor", gOverlayColor );
	}
	{
		gPPSOverlayMesh2 = new CRenderableMesh( *RGET_MESH("billboard"), 0 );
		CEffectParams& ep = gPPSOverlayMesh2->getParams();
		ep.setEffect( *RGET_FX("overlay2") );
		ep.addVector4Ref( "vFixUV", gScreenFixUVs );
		ep.addVector4Ref( "vColor", gOverlayColor2 );
	}
	
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
	gSceneStartTime = GET_TIME - DELAY_ACTION;
#else
	gSceneMode = SC_SCENE;
	gSceneIndex = 5;
	gSceneStartTime = -15.0f;
#endif

	gCut.reset();
}

void gHackyKeyboard()
{
#ifndef WITHMUSIC
	float dt = DTIME;
	//float dt = 1.0f / 50.0f;
	if( GetAsyncKeyState(VK_LEFT) )		gDebugTime -= dt * 1.0f;
	if( GetAsyncKeyState(VK_RIGHT) )	gDebugTime += dt * 1.0f;
	if( GetAsyncKeyState(VK_PRIOR) )	gDebugTime -= dt * 5.0f;
	if( GetAsyncKeyState(VK_NEXT) )		gDebugTime += dt * 5.0f;
	static bool justPressedSpace = false;
	if( GetAsyncKeyState(VK_SPACE) ) {
		if( !justPressedSpace ) {
			gDebugTime += 1.0f;
		}
		justPressedSpace = true;
	} else
		justPressedSpace = false;
#endif
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

	int oldCull = gGlobalCullMode;
	gGlobalCullMode = D3DCULL_CCW;
	
	for( int currWall = 0; currWall < CFACE_COUNT; ++currWall ) {
		gWallMeshes[currWall]->updateMatrices();
		if( gWallMeshes[currWall]->frustumCull(gCameraViewProjMatrix) )
			continue;
		SPlane reflPlane( planePos[currWall] + planeNrm[currWall]*0.05f, planeNrm[currWall] );
		SMatrix4x4 reflectMat;
		D3DXMatrixReflect( &reflectMat, &reflPlane );
		
		gWallCamera.mMatrix = gCamera.mMatrix * reflectMat;
		gWallCamera.setProjFrom( gCamera );
		gWallCamera.setOntoRenderContext();
		gComputeTextureProjection( G_RCTX->getCamera().getCameraMatrix(), gLightViewProjMatrix, gLightShadowMatrix );

		CD3DDevice& dx = CD3DDevice::getInstance();
		dx.setRenderTarget( RGET_SSURF(RT_REFLRT) );
		dx.setZStencil( RGET_SSURF(RT_REFLZ) );
		dx.clearTargets( true, true, false, 0xFF000000, 1.0f );
		dx.sceneBegin();
		gScenes[gSceneIndex]->render( RM_RECV_LO );
		G_RCTX->perform();
		dx.sceneEnd();

		IDirect3DSurface9* surf;
		RGET_STEX(gWallTexs[currWall])->getObject()->GetSurfaceLevel( 0, &surf );
		D3DSURFACE_DESC dsc;
		RGET_SSURF(RT_REFLRT)->getObject()->GetDesc( &dsc );
		dx.getDevice().StretchRect( RGET_SSURF(RT_REFLRT)->getObject(), NULL, surf, NULL, D3DTEXF_NONE );
		surf->Release();
	}

	gGlobalCullMode = oldCull;
}

void gRenderShadowMap()
{
	assert( gSceneMode == SC_SCENE );

	gLightCamera.setProjectionParams( D3DX_PI/3, 1.0f, 0.2f, 35.0f );
	gLightCamera.setOntoRenderContext();
	gLightViewProjMatrix = G_RCTX->getCamera().getViewProjMatrix();
	
	CD3DDevice& dx = CD3DDevice::getInstance();
	dx.setRenderTarget( RGET_SSURF(RT_SHADOWRT) );
	dx.setZStencil( RGET_SSURF(RT_SHADOWZ) );
	dx.clearTargets( true, true, false, 0x00000000, 1.0f );

	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 1; vp.Height = vp.Width = SZ_SHADOWMAP-2;
	vp.MinZ = 0.0f; vp.MaxZ = 1.0f;
	dx.getDevice().SetViewport( &vp );
	
	dx.sceneBegin();
	gScenes[gSceneIndex]->render( RM_SHADOW );
	G_RCTX->perform();
	dx.sceneEnd();
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

	G_RCTX->directBegin();
	CRenderableOrderedBillboards& bills = *gBillboardsNoDestA;
	bills.clear();

	const float CENTER_X = outer ? -0.02f : -0.2f;
	const float TOP_Y = outer ? 0.6f : 0.7f;
	const float HEIGHT = outer ? (0.09f+gCut.doneCount*0.02f) : 0.15f;
	const float ASPECT = CD3DDevice::getInstance().getBackBufferAspect();
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
		linepts[i].color = D3DXCOLOR(1,0,0,ptA);
	}
	gLineRenderer->renderStrip( LINE_PTS, linepts, lineWidth );

	//
	// bills...

	CD3DTexture* btex = RGET_TEX(outer?"CreditsB":"CreditsA");
	const int BILLCOUNT = 16;
	const D3DCOLOR BILLCOL = 0x18ffffff;
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

		b->texture = btex;
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
		b->texture = btex;
		b->color = BILLCOL;
	}

	G_RCTX->directRender( bills );

	G_RCTX->directEnd();
}

void CDemo::appPerform()
{
	gHackyKeyboard();

	double t = GET_TIME;
	const double musicTime = t - DELAY_ACTION;
	float st = t - gSceneStartTime;
	float sceneDur = gGetSceneDur();

	CD3DDevice& dx = CD3DDevice::getInstance();
	gScreenFixUVs.set( 0.5f/dx.getBackBufferWidth(), 0.5f/dx.getBackBufferHeight(), 0.0f, 0.0f );
	
	gBillboardsNormal->clear();
	gBillboardsNoDestA->clear();
	
	float znear = 0.1f, zfar = 50.0f;
	float camfov = D3DX_PI/4;

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
			float cutAlpha = cutDur / gCut.duration;
			sceneAlpha = gCut.curr + cutAlpha*0.01f;
			gSceneStartTime = gCut.sceneStartTime + cutDur;
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

	//
	// adjust z-ranges in outer scene

	if( gSceneMode == SC_OUTER ) {
		const float ext = 100.0f;
		const SVector3& camo = gCamera.mMatrix.getOrigin();
		float camDist = camo.length();
		znear = (camDist < ext+0.1f) ? 0.1f : (camDist-ext);
		zfar = camDist + ext;
		camfov = D3DX_PI/5;
	}

	//
	// render

	gCamera.setProjectionParams( camfov, dx.getBackBufferAspect(), znear, zfar );

	if( gSceneMode != SC_OUTER ) {
		gRenderShadowMap();
		gRenderWallReflections();
	}

	gCamera.setOntoRenderContext();
	gComputeTextureProjection( G_RCTX->getCamera().getCameraMatrix(), G_RCTX->getCamera().getViewProjMatrix(), gViewTexProjMatrix );
	gComputeTextureProjection( G_RCTX->getCamera().getCameraMatrix(), gLightViewProjMatrix, gLightShadowMatrix );
	gCameraViewProjMatrix = G_RCTX->getCamera().getViewProjMatrix();

	dx.setDefaultRenderTarget();
	dx.setDefaultZStencil();
	dx.clearTargets( true, true, false, 0x00000000, 1.0f, 0L );
	dx.sceneBegin();
	if( gSceneMode != SC_OUTER ) {
		gScenes[gSceneIndex]->render( RM_RECV_HI ); 
		G_RCTX->perform();
		gScenes[gSceneIndex]->render( RM_REFLECTIVE );
		G_RCTX->perform();
	} else {
		gSceneOut->render( RM_HI );
		G_RCTX->perform();
	}

	//
	// render credits

	if( gCut.isInCut() && gSceneIndex!=SCENES-1 ) {
		float cutAlpha = (t - gCut.startTime) / gCut.duration;
		gRenderCredits( cutAlpha );
	}
	
	{
		//
		// overlay for beats...

		const double SYNCH_LEN = 6361.0;
		const double OVERSAMPLE = 1.0 / SYNCH_LEN;
		double synt = musicTime / (SYNCH_LEN/30.0);
		SVector3 beatPos0, beatPos1, beatPos2;
		gAnimSynch->samplePos( synt - OVERSAMPLE, beatPos0 );
		gAnimSynch->samplePos( synt, beatPos1 );
		gAnimSynch->samplePos( synt + OVERSAMPLE, beatPos2 );
		float beat = (beatPos0.y + beatPos1.y + beatPos2.y) * 0.08f;
		beat = clamp( beat, 0.0f, 1.0f );
		gOverlayColor2.set( beat, beat, beat, beat );
		if( beat > 0.001f ) {
			G_RCTX->directBegin();
			G_RCTX->directRender( *gPPSOverlayMesh2 );
			G_RCTX->directEnd();
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
	bool teethCut = false;
	float cutTime = 0.0f;
	if( gCut.isInCut() ) {
		cutTime = t - gCut.startTime;
		if( gSceneIndex == SCENES-1 ) {
			CSceneTeeth* steeth = (CSceneTeeth*)gScenes[gSceneIndex];
			steeth->renderTeethStuff( gCut.doneCount, sceneAlpha, cutTime/gCut.duration );
			steeth->renderTeethUI( gCut.doneCount, sceneAlpha, cutTime/gCut.duration );
			teethCut = true;
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
		G_RCTX->directBegin();
		G_RCTX->directRender( *gPPSOverlayMesh );
		G_RCTX->directEnd();
	}

	dx.sceneEnd();

	//
	// bloom

	gPPSBloom->renderBloom();

	//
	// manage scene transitions

	if( st >= sceneDur ) {
		if( gSceneMode==SC_OUTER ) {
			gSceneMode = SC_SCENE;
		} else {
			gSceneMode = SC_OUTER;
			++gSceneIndex;
			if( gSceneIndex >= SCENES ) {
				//gSceneIndex = 0;
				// end!
				doClose();
			}
		}
		gSceneStartTime = t;
		gCut.reset();
	}
}

void CDemo::appShutdown()
{
#ifdef WITHMUSIC
	delete gMusicPlayer;
#endif
	
	for( int i = 0; i < SCENES; ++i )
		delete gScenes[i];
	delete gSceneOut;
	delete gPPSBloom;
	delete gPPSOverlayMesh;
	delete gPPSOverlayMesh2;
	delete gLineRenderer;
	delete gBillboardsNormal;
	delete gBillboardsNoDestA;
	delete gAnimSynch;
}
