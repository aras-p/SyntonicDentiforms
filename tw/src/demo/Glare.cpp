#include "stdafx.h"
#include "Glare.h"
#include "DemoResources.h"

const int BLOOM_PASSES = 4;


CBloomPostProcess::CBloomPostProcess()
{
	/* @TODO
	const char* fx = "filterBloom";
	for( int i = 0; i < 2; ++i ) {
		mBloomPingPongs[i] = new CRenderableMesh( *RGET_MESH("billboard"), 0 );
		CEffectParams& ep = mBloomPingPongs[i]->getParams();
		ep.setEffect( *RGET_FX(fx) );
		ep.addVector4Ref( "vFixUV", mFixUVs4th );
		ep.addVector4Ref( "vTexelsX", mTexelOffsetsX );
		ep.addVector4Ref( "vTexelsY", mTexelOffsetsY );
	}
	mBloomPingPongs[0]->getParams().addTexture( "tBase", *RGET_STEX(RT_4thSCREEN_1) );
	mBloomPingPongs[1]->getParams().addTexture( "tBase", *RGET_STEX(RT_4thSCREEN_2) );

	mComposite = new CRenderableMesh( *RGET_MESH("billboard"), 0 );
	CEffectParams& cep = mComposite->getParams();
	cep.setEffect( *RGET_FX("compositeAdd") );
	cep.addVector4Ref( "vFixUV", mFixUVs4th );
	cep.addTexture( "tBase", *RGET_STEX( !(BLOOM_PASSES&1) ? RT_4thSCREEN_1 : RT_4thSCREEN_2 ) );
	*/
}

CBloomPostProcess::~CBloomPostProcess()
{
	/* @TODO
	delete mBloomPingPongs[0];
	delete mBloomPingPongs[1];
	delete mComposite;
	*/
}

void CBloomPostProcess::pingPongBlur( int passes )
{
	/* @TODO
	CD3DDevice& dx = CD3DDevice::getInstance();

	CD3DSurface* pingPongS[2];
	pingPongS[0] = RGET_SSURF(RT_4thSCREEN_2);
	pingPongS[1] = RGET_SSURF(RT_4thSCREEN_1);

	// fullscreen quad UV fixes
	int swidth = sapp_widthf();
	int sheight = sapp_heightf();
	mFixUVs4th.set( 2.0f/swidth, 2.0f/sheight, 1.0f-4.0f/swidth, 1.0f-4.0f/sheight );

	// ping-pong blur passes
	dx.setZStencil( NULL );

	const SVector4 offsetX( 1, 1,-1,-1);
	const SVector4 offsetY( 1,-1,-1, 1);
	for( int i = 0; i < passes; ++i ) {
		const float pixDist = i+0.5f;
		mTexelOffsetsX = offsetX * (mFixUVs4th.x*2 * pixDist);
		mTexelOffsetsY = offsetY * (mFixUVs4th.y*2 * pixDist);
		dx.setRenderTarget( pingPongS[i&1] );
		dx.sceneBegin();
		G_RCTX->directBegin();
		G_RCTX->directRender( *mBloomPingPongs[i&1] );
		G_RCTX->directEnd();
		dx.sceneEnd();
	}
	*/
}

void CBloomPostProcess::renderBloom()
{
	/* @TODO
	CD3DDevice& dx = CD3DDevice::getInstance();

	// downsample backbuffer into smaller RT
	dx.getDevice().StretchRect(
		dx.getBackBuffer(), NULL,
		RGET_SSURF(RT_4thSCREEN_1)->getObject(), NULL,
		D3DTEXF_NONE
	);

	// blur
	pingPongBlur( BLOOM_PASSES );

	// composite
	dx.setDefaultRenderTarget();
	dx.setDefaultZStencil();
	dx.sceneBegin();
	G_RCTX->directBegin();
	G_RCTX->directRender( *mComposite );
	G_RCTX->directEnd();
	dx.sceneEnd();
	*/

}

