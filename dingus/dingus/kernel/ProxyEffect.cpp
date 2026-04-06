// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "ProxyEffect.h"
#include "D3DDevice.h"
#include "../utils/Errors.h"

using namespace dingus;


CD3DXEffect::CD3DXEffect( ID3DXEffect* object )
:	CBaseProxyClass(object)
{
	init();
}

void CD3DXEffect::setObject( ID3DXEffect* object )
{
	setPtr(object);
	if( object )
		init();
}

void CD3DXEffect::init()
{
	CD3DDevice& dx = CD3DDevice::getInstance();
	bool ok = tryInit();
	if( !ok ) {
		// no valid technique found, throw exception
		std::string msg = "no valid techniques found in effect";
		THROW_ERROR( msg );
	}
}

bool CD3DXEffect::tryInit()
{
	HRESULT hres;

	ID3DXEffect* fx = getObject();
	assert( fx );

	//D3DXEFFECT_DESC fxdsc;
	//fx->GetDesc( &fxdsc );
	
	D3DXHANDLE tech = fx->GetTechnique( 0 );
	if( FAILED( fx->ValidateTechnique( tech ) ) )
		hres = fx->FindNextValidTechnique( tech, &tech );
	
	if( tech != NULL )
		fx->SetTechnique( tech );
	else
		return false;
	//D3DXTECHNIQUE_DESC techdsc;
	//fx->GetTechniqueDesc( tech, &techdsc );
	assert( tech != NULL );

	// back to front?
	D3DXHANDLE annot = fx->GetAnnotationByName( tech, "backToFront" );
	BOOL backToFront = FALSE;
	if( annot != NULL )
		fx->GetBool( annot, &backToFront );
	mBackToFrontSorted = backToFront ? true : false;


	// sort value
	mSortValue = 0;
	mSortValue += mBackToFrontSorted ? 0x10 : 0x00;

	return true;
}
