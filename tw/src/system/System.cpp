#include "stdafx.h"

#include "System.h"

#include <dingus/resource/DeviceResource.h>

#include <dingus/renderer/EffectParamsNotifier.h>

#include <dingus/resource/CubeTextureBundle.h>
#include <dingus/resource/VertexDeclBundle.h>
#include <dingus/resource/IndexBufferBundle.h>

#include <dingus/gfx/geometry/DynamicVBManager.h>

#include <dingus/console/WDebugConsoleRenderingContext.h>


CSystem::CSystem()
:	mStdConsoleCtx(0),
	mAppInited( false )
{
	mCreationWidth			= 640;
	mCreationHeight 		= 480;
	mWindowTitle			= "Syntonic Dentiforms";

	const bool devMode = false;
	mStartFullscreen		= !devMode;
	mVSyncFullscreen		= !devMode;
	mSelectDeviceAtStartup	= !devMode;

	mShowCursorWhenFullscreen	= false;
	mDebugTimer					= false;
	mEnumeration.mUsesDepthBuffer		= true;
	mEnumeration.mMinColorChannelBits	= 8;
	mEnumeration.mMinAlphaChannelBits	= 8;
	mEnumeration.mMinDepthBits			= 16;
	mEnumeration.mMinStencilBits		= 0;
	mEnumeration.mUsesMixedVP			= true;
	mDataPath = "data/";

	// init console
#ifdef _DEBUG
	mStdConsoleCtx = new CWDebugConsoleRenderingContext();
	dingus::CConsole::getInstance().setDefaultRenderingContext( *mStdConsoleCtx );
#endif
};


CSystem::~CSystem()
{
}


/**
 *  Initialization. Paired with shutdown().
 *  The window has been created and the IDirect3D9 interface has been
 *  created, but the device has not been created yet.
 */
HRESULT CSystem::initialize()
{
	//
	// resources

	mDeviceManager = new dingus::CDeviceResourceManager();

	CTextureBundle::init( mDataPath + "tex/" );
	CCubeTextureBundle::init( mDataPath + "tex/" );
	CMeshBundle::init( mDataPath + "mesh/" );
	CEffectBundle::init( mDataPath + "fx/" );
	CAnimationBundle::init( mDataPath + "anim/" );
	CSharedTextureBundle::init();
	CSharedSurfaceBundle::init();
	CVertexDeclBundle::init();
	CIndexBufferBundle::init();

	CDynamicVBManager::initialize( 2 * 1024 * 1024 ); // 2 megabytes
	CDynamicVBManager& vbManager = CDynamicVBManager::getInstance();

	//
	// device dependant resources

	mDeviceManager->add( CSharedTextureBundle::getInstance() );
	mDeviceManager->add( CSharedSurfaceBundle::getInstance() );
	mDeviceManager->add( vbManager );
	mDeviceManager->add( CTextureBundle::getInstance() );
	mDeviceManager->add( CCubeTextureBundle::getInstance() );
	mDeviceManager->add( CMeshBundle::getInstance() );
	mDeviceManager->add( CEffectBundle::getInstance() );
	mDeviceManager->add( CVertexDeclBundle::getInstance() );
	mDeviceManager->add( CIndexBufferBundle::getInstance() );

	//
	// renderer

	assert( !G_RCTX );
	G_RCTX = new CRenderContext();

	return S_OK;
}


/**
 *  Called during device initialization, this code checks the display device
 *  for some minimum set of capabilities.
 */
HRESULT CSystem::checkDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT format )
{
	return appCheckDevice( caps, behavior, format ) ? S_OK : E_FAIL;
}



/**
 *  createDeviceObjects(). Paired with deleteDeviceObjects().
 *  The device has been created.  Resources that are not lost on
 *  Reset() can be created here -- resources in D3DPOOL_MANAGED, D3DPOOL_SCRATCH,
 *  or D3DPOOL_SYSTEMMEM.
 */
HRESULT CSystem::createDeviceObjects()
{
	assert( mD3DDevice );
	dingus::CD3DDevice& dev = dingus::CD3DDevice::getInstance();
	dev.setDevice( mD3DDevice );

	mDeviceManager->createResource();

	dingus::CEffectParamsNotifier::getInstance().notify();

	return S_OK;
}


/**
 *  activateDeviceObjects(). Paired with passivateDeviceObjects().
 *  The device exists, but may have just been Reset().
 */
HRESULT CSystem::activateDeviceObjects()
{
	dingus::CD3DDevice::getInstance().activateDevice();
	mDeviceManager->activateResource();

	if( !mAppInited ) {
		appInitialize();
		mAppInited = true;
	}
	
	return S_OK;
}


/**
 *  Called once per frame.
 */
HRESULT CSystem::performOneTime()
{
	dingus::CD3DDevice& device = dingus::CD3DDevice::getInstance();

	dingus::CSystemClock& c = dingus::CSystemClock::getInstance();
	c.setTimes( mTime, mElapsedTime, c.getPerformCount()+1 );

	//
	// pipeline

	appPerform();

	return S_OK;
}


/**
 *  Overrrides the main WndProc, so the sample can do custom message handling
 *  (e.g. processing mouse, keyboard, or menu commands).
 */
LRESULT CSystem::msgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static bool reloadKeyPressed = false;
	switch( msg ) {
	case WM_KEYDOWN:
		if( wParam == VK_F5 )
			reloadKeyPressed = true;
		break;
	case WM_KEYUP:
		if( wParam == VK_F5 ) {
			if( reloadKeyPressed ) {
				//mReloadableManager->reload();
				dingus::CEffectParamsNotifier::getInstance().notify();
			}
			reloadKeyPressed = false;
		}
	}
	return CD3DApplication::msgProc( hWnd, msg, wParam, lParam );
}


/**
 *  Invalidates device objects.  Paired with activateDeviceObjects().
 */
HRESULT CSystem::passivateDeviceObjects()
{
	mDeviceManager->passivateResource();
	dingus::CD3DDevice::getInstance().passivateDevice();
	return S_OK;
}

/**
 *  Paired with createDeviceObjects().
 *  Called when the app is exiting, or the device is being changed,
 *  this function deletes any device dependent objects.
 */
HRESULT CSystem::deleteDeviceObjects()
{
	mDeviceManager->deleteResource();
	dingus::CD3DDevice::getInstance().setDevice( NULL );
	return S_OK;
}


/**
 *  Paired with initialize().
 *  Called before the app exits, this function gives the app the chance
 *  to cleanup after itself.
 */
HRESULT CSystem::shutdown()
{
	appShutdown();

	mDeviceManager->clear();
	delete mDeviceManager;

	assert( G_RCTX );
	delete G_RCTX;
	G_RCTX = 0;

	CDynamicVBManager::finalize();

	CTextureBundle::finalize();
	CCubeTextureBundle::finalize();
	CMeshBundle::finalize();
	CEffectBundle::finalize();
	CAnimationBundle::finalize();
	CVertexDeclBundle::finalize();
	CSharedTextureBundle::finalize();
	CSharedSurfaceBundle::finalize();
	CIndexBufferBundle::finalize();

	dingus::CSystemClock::finalize();
	if( mStdConsoleCtx )
		delete mStdConsoleCtx;
	dingus::CConsole::finalize();

	return S_OK;
}
