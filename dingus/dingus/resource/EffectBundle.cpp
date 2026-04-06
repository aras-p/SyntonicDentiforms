// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "EffectBundle.h"
#include "../utils/Errors.h"
#include "../kernel/D3DDevice.h"
#include "../renderer/EffectStateManager.h"

using namespace dingus;

CEffectBundle::CEffectBundle( const std::string& predir )
:	CStorageResourceBundle<CD3DXEffect>(predir),
	mStateManager(0), mSharedPool(0),
	mOptimizeShaders(true), mLastErrors("")
{
	addExtension( ".fx" );
};

ID3DXEffect* CEffectBundle::loadEffect( const CResourceId& id, const CResourceId& fullName ) const
{
	ID3DXEffect* fx = NULL;
	ID3DXBuffer* errors = NULL;

	mLastErrors = "";

	assert( mSharedPool );
	assert( mStateManager );
	HRESULT hres = D3DXCreateEffectFromFile(
		&CD3DDevice::getInstance().getDevice(),
		fullName.getUniqueName().c_str(),
		NULL,
		NULL, // TBD ==> includes
		(mOptimizeShaders ? 0 : D3DXSHADER_SKIPOPTIMIZATION) | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
		mSharedPool,
		&fx,
		&errors );
	if( errors && errors->GetBufferSize() > 1 ) {
		std::string msg = "messages compiling effect '" + fullName.getUniqueName() + "'";
		mLastErrors = (const char*)errors->GetBufferPointer();
		msg += mLastErrors;
		CConsole::CON_ERROR.write( msg );
	}

	if( FAILED( hres ) ) {
		return NULL;
	}
	assert( fx );

	CONSOLE.write( "fx loaded '" + id.getUniqueName() + "'" );

	if( errors )
		errors->Release();

	// set state manager
	fx->SetStateManager( mStateManager );

	return fx;
}

CD3DXEffect* CEffectBundle::loadResourceById( const CResourceId& id, const CResourceId& fullName )
{
	ID3DXEffect* fx = loadEffect( id, fullName );
	if( !fx )
		return NULL;
	return new CD3DXEffect( fx );
}

void CEffectBundle::createResource()
{
	// create state manager
	assert( !mStateManager );
	mStateManager = new CEffectStateManager();
	assert( mStateManager );
	mStateManager->AddRef();

	// create pool
	assert( !mSharedPool );
	D3DXCreateEffectPool( &mSharedPool );
	assert( mSharedPool );

	// reload all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( res.isNull() );
		CD3DXEffect* n = tryLoadResourceById( it->first );
		assert( n );
		res.setObject( n->getObject() );
		delete n;
		assert( !res.isNull() );
	}
}

void CEffectBundle::activateResource()
{
	assert( mStateManager );
	mStateManager->reset();

	// call reset on effects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( !res.isNull() );
		res.getObject()->OnResetDevice();
	}
}

void CEffectBundle::passivateResource()
{
	// call lost on effects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( !res.isNull() );
		res.getObject()->OnLostDevice();
	}
}

void CEffectBundle::deleteResource()
{
	// unload all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DXEffect& res = *it->second;
		assert( !res.isNull() );
		res.getObject()->Release();
		res.setObject( NULL );
	}

	// release pool
	assert( mSharedPool );
	mSharedPool->Release();
	mSharedPool = NULL;

	// release state manager
	assert( mStateManager );
	mStateManager->Release();
	mStateManager = NULL;
}

