// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "EffectStateManager.h"
#include "../kernel/D3DDevice.h"

// {79AAB587-6DBC-4fa7-82DE-37FA1781C5CE}
extern "C" const GUID __declspec(selectany) IID_ID3DXEffectStateManager = { 0x79aab587, 0x6dbc, 0x4fa7, 0x82, 0xde, 0x37, 0xfa, 0x17, 0x81, 0xc5, 0xce };


using namespace dingus;

// define for testing only
//#define DISABLE_FILTERING


CEffectStateManager::CEffectStateManager()
{
	reset();
}

void CEffectStateManager::reset()
{
	memset( mLightEnabled, 0, sizeof(mLightEnabled) );
	memset( mGotRenderStates, 0, sizeof(mGotRenderStates) );
	memset( mTextures, 0, sizeof(mTextures) );
	memset( mGotTextureStates, 0, sizeof(mGotTextureStates) );
	memset( mGotSamplerStates, 0, sizeof(mGotSamplerStates) );
	mVShader = 0;
	mPShader = 0;
}

HRESULT CEffectStateManager::SetTransform( D3DTRANSFORMSTATETYPE state, const D3DMATRIX* matrix ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetTransform( state, matrix );
}

HRESULT CEffectStateManager::SetMaterial( const D3DMATERIAL9* material ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetMaterial( material );
}

HRESULT CEffectStateManager::SetLight( DWORD index, const D3DLIGHT9* light ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetLight( index, light );
}

HRESULT CEffectStateManager::LightEnable( DWORD index, BOOL enable ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// check first some
#ifndef DISABLE_FILTERING
	if( index < NUM_LIGHTS_CACHED ) {
		if( enable==mLightEnabled[index] ) {
			return S_OK;
		}
		mLightEnabled[index] = enable;
	}
#endif
	return dx.LightEnable( index, enable );
}

HRESULT CEffectStateManager::SetRenderState( D3DRENDERSTATETYPE state, DWORD value ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// check first some
#ifndef DISABLE_FILTERING
	if( state < NUM_RENDERSTATES_CACHED ) {
		if( value==mRenderStates[state] && mGotRenderStates[state] ) {
			return S_OK;
		}
		mRenderStates[state] = value;
		mGotRenderStates[state] = TRUE;
	}
#endif
	return dx.SetRenderState( state, value );
}

HRESULT CEffectStateManager::SetTexture( DWORD stage, IDirect3DBaseTexture9* texture ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// check first some
#ifndef DISABLE_FILTERING
	if( stage < NUM_TEXTURES_CACHED ) {
		if( texture==mTextures[stage] ) {
			return S_OK;
		}
		mTextures[stage] = texture;
	}
#endif
	return dx.SetTexture( stage, texture );
}

HRESULT CEffectStateManager::SetTextureStageState( DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// check first some
#ifndef DISABLE_FILTERING
	if( stage < NUM_TEXTURES_CACHED && type < NUM_TEXSTATES_CACHED ) {
		if( value==mTextureStates[stage][type] && mGotTextureStates[stage][type] ) {
			return S_OK;
		}
		mTextureStates[stage][type] = value;
		mGotTextureStates[stage][type] = TRUE;
	}
#endif
	return dx.SetTextureStageState( stage, type, value );
}

HRESULT CEffectStateManager::SetSamplerState( DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// check first some
#ifndef DISABLE_FILTERING
	if( sampler < NUM_SAMPLERS_CACHED && type < NUM_SAMPSTATES_CACHED ) {
		if( value==mSamplerStates[sampler][type] && mGotSamplerStates[sampler][type] ) {
			return S_OK;
		}
		mSamplerStates[sampler][type] = value;
		mGotSamplerStates[sampler][type] = TRUE;
	}
#endif
	return dx.SetSamplerState( sampler, type, value );
}

HRESULT CEffectStateManager::SetNPatchMode( float numSegments ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// no checking
	return dx.SetNPatchMode( numSegments );
}

HRESULT CEffectStateManager::SetFVF( DWORD fvf ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	device.setDeclarationFVF( fvf ); // route through device
	return S_OK;
}

HRESULT CEffectStateManager::SetVertexShader( IDirect3DVertexShader9* shader ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// check
#ifndef DISABLE_FILTERING
	if( shader == mVShader ) {
		return S_OK;
	}
#endif
	mVShader = shader;
	return dx.SetVertexShader( shader );
}

HRESULT CEffectStateManager::SetVertexShaderConstantF( UINT registerIdx, const float* data, UINT registerCount ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetVertexShaderConstantF( registerIdx, data, registerCount );
}

HRESULT CEffectStateManager::SetVertexShaderConstantI(THIS_ UINT registerIdx, const int *data, UINT registerCount ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetVertexShaderConstantI( registerIdx, data, registerCount );
}

HRESULT CEffectStateManager::SetVertexShaderConstantB(THIS_ UINT registerIdx, const BOOL *data, UINT registerCount ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetVertexShaderConstantB( registerIdx, data, registerCount );
}

HRESULT CEffectStateManager::SetPixelShader( IDirect3DPixelShader9* shader ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	// check
#ifndef DISABLE_FILTERING
	if( shader == mPShader ) {
		return S_OK;
	}
#endif
	mPShader = shader;
	return dx.SetPixelShader( shader );
}

HRESULT CEffectStateManager::SetPixelShaderConstantF( UINT registerIdx, const float *data, UINT registerCount ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetPixelShaderConstantF( registerIdx, data, registerCount );
}

HRESULT CEffectStateManager::SetPixelShaderConstantI( UINT registerIdx, const int *data, UINT registerCount ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetPixelShaderConstantI( registerIdx, data, registerCount );
}

HRESULT CEffectStateManager::SetPixelShaderConstantB( UINT registerIdx, const BOOL *data, UINT registerCount ) {
	CD3DDevice& device = CD3DDevice::getInstance();
	IDirect3DDevice9& dx = device.getDevice();
	return dx.SetPixelShaderConstantB( registerIdx, data, registerCount );
}
