// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "D3DDevice.h"
#include "../utils/Errors.h"

using namespace dingus;

CD3DDevice CD3DDevice::mSingleInstance;

// --------------------------------------------------------------------------

CD3DDevice::CD3DDevice()
:	mDevice(NULL),
	mBackBuffer(NULL),
	mMainZStencil(NULL)
{
}

void CD3DDevice::activateDevice()
{
	assert( !mMainZStencil );
	assert( !mBackBuffer );

	// backbuffer
	mDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &mBackBuffer );
	assert( mBackBuffer );
	mBackBuffer->GetDesc( &mBackBufferDesc );
	mBackBufferAspect = (float)getBackBufferWidth() / (float)getBackBufferHeight();

	// Z/stencil
	mDevice->GetDepthStencilSurface( &mMainZStencil );
	assert( mMainZStencil );
	mMainZStencil->GetDesc( &mMainZStencilDesc );
}

void CD3DDevice::passivateDevice()
{
	int i;

	// set default rendertargets
	setDefaultRenderTarget();
	for( i = 1; i < getMRTCount(); ++i )
		setRenderTarget( NULL, i );
	setDefaultZStencil();

	// set null streams
	setIndexBuffer( NULL );
	for( i = 0; i < VSTREAM_COUNT; ++i )
		setVertexBuffer( 0, NULL, 0, 0 );
	setDeclarationFVF( D3DFVF_XYZ );

	// release backbuffer/zstencil
	if( mBackBuffer ) {
		mBackBuffer->Release();
		mBackBuffer = NULL;
	}
	if( mMainZStencil ) {
		mMainZStencil->Release();
		mMainZStencil = NULL;
	}
}

void CD3DDevice::setDevice( IDirect3DDevice9* dx )
{
	mDevice = dx;
	if( !mDevice ) {
		memset( &mCaps, 0, sizeof(mCaps) );
		assert( !mBackBuffer );
		assert( !mMainZStencil );
		return;
	}
	assert( mDevice );
	mDevice->GetDeviceCaps( &mCaps );

	assert( !mBackBuffer );
	assert( !mMainZStencil );
}


// --------------------------------------------------------------------------

void CD3DDevice::setRenderTarget( CD3DSurface* rt, int index )
{
	internalSetRenderTarget( rt ? rt->getObject() : NULL, index );
}

void CD3DDevice::setDefaultRenderTarget()
{
	internalSetRenderTarget( mBackBuffer, 0 );
}

void CD3DDevice::internalSetRenderTarget( IDirect3DSurface9* rt, int index )
{
	assert( mDevice );

	// #MRT check
	if( index >= getMRTCount() ) {
		// debug complaint
		ASSERT_FAIL_MSG( "rendertarget index larger than number of MRTs supported!" );
		return;
	}

	// set RT
	HRESULT hr = mDevice->SetRenderTarget( index, rt );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set render target" );
}

void CD3DDevice::setZStencil( CD3DSurface* zs )
{
	internalSetZStencil( zs ? zs->getObject() : NULL );
}

void CD3DDevice::setDefaultZStencil()
{
	internalSetZStencil( mMainZStencil );
}

void CD3DDevice::internalSetZStencil( IDirect3DSurface9* zs )
{
	assert( mDevice );

	HRESULT hr = mDevice->SetDepthStencilSurface( zs );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set z/stencil" );
}

void CD3DDevice::clearTargets( bool clearRT, bool clearZ, bool clearStencil, D3DCOLOR color, float z, DWORD stencil )
{
	assert( mDevice );

	DWORD clrFlags = 0;
	if( clearRT ) clrFlags |= D3DCLEAR_TARGET;
	if( clearZ ) clrFlags |= D3DCLEAR_ZBUFFER;
	if( clearStencil ) clrFlags |= D3DCLEAR_STENCIL;

	HRESULT hr = mDevice->Clear( 0, NULL, clrFlags, color, z, stencil );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to clear render target" );
}


// --------------------------------------------------------------------------

void CD3DDevice::sceneBegin()
{
	assert( mDevice );
	HRESULT hr = mDevice->BeginScene();
	if( FAILED(hr) )
		THROW_DXERROR( hr, "failed to begin scene" );
}

void CD3DDevice::sceneEnd()
{
	assert( mDevice );
	HRESULT hr = mDevice->EndScene();
	if( FAILED(hr) )
		THROW_DXERROR( hr, "failed to end scene" );
}

// --------------------------------------------------------------------------

void CD3DDevice::setIndexBuffer( CD3DIndexBuffer* ib )
{
	assert( mDevice );
	
	HRESULT hr = mDevice->SetIndices( ib ? ib->getObject() : NULL );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set indices" );
}

void CD3DDevice::setVertexBuffer( int stream,  CD3DVertexBuffer* vb, unsigned int offset, unsigned int stride )
{
	assert( mDevice );
	assert( stream >= 0 && stream < VSTREAM_COUNT );
	
	IDirect3DVertexBuffer9* vb9 = vb ? vb->getObject() : NULL;
	HRESULT hr = mDevice->SetStreamSource( stream, vb9, offset, stride );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set vertex buffer" );
}

void CD3DDevice::setDeclaration( CD3DVertexDecl& decl )
{
	assert( mDevice );
	assert( &decl );
	
	HRESULT hr = mDevice->SetVertexDeclaration( &decl ? decl.getObject() : NULL );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set declaration" );
}

void CD3DDevice::setDeclarationFVF( DWORD fvf )
{
	assert( mDevice );
	assert( fvf );
	
	HRESULT hr = mDevice->SetFVF( fvf );
	if( FAILED( hr ) )
		THROW_DXERROR( hr, "failed to set declaration fvf" );
}
