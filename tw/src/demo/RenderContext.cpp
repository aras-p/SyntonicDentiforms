// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#if 0 //@TODO

#include "RenderContext.h"
#include "Renderable.h"
#include "../kernel/D3DDevice.h"
#include "../resource/EffectBundle.h"
#include "../resource/ResourceId.h"

using namespace dingus;

CRenderContext::CRenderContext()
:	mGlobalEffect(NULL),
	mGlobalInited( false ),
	mInsideDirect( false ),
	mDirectCurrFX( NULL )
{
	// reserve some space in renderables
	mRenderables.reserve( 256 );

	// add-by-ref view & projection matrices to the global params
	mGlobalParams.addMatrix4x4Ref( "mView", mRenderCamera.getViewMatrix() );
	mGlobalParams.addMatrix4x4Ref( "mProjection", mRenderCamera.getProjectionMatrix() );
	mGlobalParams.addMatrix4x4Ref( "mViewProj", mRenderCamera.getViewProjMatrix() );
	mGlobalParams.addVector3Ref( "vEye", mRenderCamera.getCameraMatrix().getOrigin() );
}


// --------------------------------------------------------------------------

void CRenderContext::initGlobalFX()
{
	if( !mGlobalInited ) {
		mGlobalEffect = CEffectBundle::getInstance().getResourceById( "_global" );
		assert( mGlobalEffect );
		mGlobalParams.setEffect( *mGlobalEffect );
		mGlobalInited = true;
	}
}

// --------------------------------------------------------------------------

void CRenderContext::renderRenderables()
{
	CD3DDevice& device = CD3DDevice::getInstance();

	// here we have our renderables attached and sorted by (with the exception
	// of back-to-front):
	//	1. priority,
	//	2. need to sort back-to-front (unsorted, then sorted)
	//	3. vertex shader req. (in ascending order - none, 1.1, 2.0 etc.)
	//	4. effect (in no particular order)

	int sz = mRenderables.size();

	CD3DXEffect*	currFX = NULL;

	//
	// go thru all renderables

	TRenderableVector::iterator itR = mRenderables.begin();
	TRenderableVector::iterator itEnd = mRenderables.end();
	TRenderableVector::iterator itFxStart = itR;
	for( ; itR != itEnd; ++itR ) {
		CRenderable* rend = *itR;
		assert( rend );

		CEffectParams& params = rend->getParams();
		CD3DXEffect* newFX = params.getEffect();
		assert( newFX );

		//
		// check effect change. if changed, render all renderables of the old effect

		if( newFX != currFX ) {
			// render all renderables with old fx
			if( currFX ) {
				assert( currFX );

				// begin
				int passes = currFX->beginFx( FX_PARAMS );
				// all passes, except last
				for( int p = 0; p < passes-1; ++p ) {
					currFX->beginPass( p );
					// all objects
					TRenderableVector::iterator it;
					for( it = itFxStart; it != itR; ++it ) {
						CRenderable* r = *it;
						assert( r );
						r->getParams().applyToEffect();
						currFX->commitParams();
						r->render( *this );
					}
					currFX->endPass();
				}
				// restore
				currFX->beginPass( passes-1 );
				currFX->endPass();
				
				// end old fx
				currFX->endFx();
			}
			// set new fx
			currFX = newFX;
			itFxStart = itR;
		}
	}

	//
	// here we have to render all renderables of the last fx

	if( currFX ) {
		assert( currFX );
		// begin
		int passes = currFX->beginFx( FX_PARAMS );
		// all passes, except last
		for( int p = 0; p < passes-1; ++p ) {
			currFX->beginPass( p );
			// all objects
			TRenderableVector::iterator it;
			for( it = itFxStart; it != itEnd; ++it ) {
				CRenderable* r = *it;
				assert( r );
				r->getParams().applyToEffect();
				currFX->commitParams();
				r->render( *this );
			}
			currFX->endPass();
		}
		// restore
		currFX->beginPass( passes-1 );
		currFX->endPass();
		// end old fx
		currFX->endFx();
	}
}

void CRenderContext::perform()
{
	initGlobalFX();

	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9* dx = &device.getDevice();
	assert( dx );
	
	// global params and effect
	mGlobalParams.applyToEffect();

	int passes = mGlobalEffect->beginFx( FX_PARAMS );
	mGlobalEffect->beginPass( 0 );

	renderRenderables();

	mGlobalEffect->endPass();
	mGlobalEffect->endFx();

	clear();
}


// --------------------------------------------------------------------------

void CRenderContext::directBegin()
{
	assert( !mInsideDirect );
	assert( !mDirectCurrFX );
	mInsideDirect = true;
	mDirectCurrFX = NULL;
	mDirectCurrPasses = 0;

	// global params and effect
	initGlobalFX();

	mGlobalParams.applyToEffect();

	int passes = mGlobalEffect->beginFx( FX_PARAMS );
	mGlobalEffect->beginPass( 0 );
	mGlobalEffect->endPass();
}

void CRenderContext::directEnd()
{
	assert( mInsideDirect );
	if( mDirectCurrFX ) {
		// restore
		mDirectCurrFX->beginPass( mDirectCurrPasses-1 );
		mDirectCurrFX->endPass();
		mDirectCurrFX->endFx();
	}
	mInsideDirect = false;
	mDirectCurrFX = NULL;
	mDirectCurrPasses = 0;

	mGlobalEffect->endFx();
}

void CRenderContext::directRender( CRenderable& r )
{
	CD3DXEffect* fx = r.getParams().getEffect();
	assert( fx );
	
	// effect changed?
	directSetFX( *fx );

	// apply renderable params to fx
	assert( mDirectCurrFX );
	assert( mDirectCurrFX == fx );
	r.getParams().applyToEffect();

	// render all passes
	assert( mDirectCurrPasses > 0 );
	r.beforeRender();
	for( int p = 0; p < mDirectCurrPasses-1; ++p ) {
		mDirectCurrFX->beginPass( p );
		r.render( *this );
		mDirectCurrFX->endPass();
	}

	// restore
	mDirectCurrFX->beginPass( mDirectCurrPasses-1 );
	mDirectCurrFX->endPass();

	r.afterRender();
}
#endif
