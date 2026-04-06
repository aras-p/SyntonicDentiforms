// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "ProxyEffect.h"
#include "D3DDevice.h"
#include "../console/Console.h"
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
	mSoftwareVertexProcessed = false;
	if( dx.getVertexProcessing() != CD3DDevice::VP_SW )
		dx.getDevice().SetSoftwareVertexProcessing( FALSE );
	bool okhwvp = tryInit();
	if( !okhwvp ) {
		mSoftwareVertexProcessed = true;
		if( dx.getVertexProcessing() != CD3DDevice::VP_SW )
			dx.getDevice().SetSoftwareVertexProcessing( TRUE );
		bool okswvp = tryInit();
		if( !okswvp ) {
			// no valid technique found, throw exception
			std::string msg = "no valid techniques found in effect";
			CConsole::CON_ERROR.write( msg );
			THROW_ERROR( msg );
		}
		if( dx.getVertexProcessing() != CD3DDevice::VP_SW )
			dx.getDevice().SetSoftwareVertexProcessing( FALSE );
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

	// vertex shader version
	annot = fx->GetAnnotationByName( tech, "vshader" );
	int vsVersion = CD3DDevice::VS_FFP;
	if( annot != NULL ) {
		float vsv;
		fx->GetFloat( annot, &vsv );
		if( vsv < 1.1f )
			vsVersion = CD3DDevice::VS_FFP;
		else if( vsv < 2.0f )
			vsVersion = CD3DDevice::VS_1_1;
		else if( vsv < 3.0f )
			vsVersion = CD3DDevice::VS_2_0;
		else
			vsVersion = CD3DDevice::VS_3_0;
	}
	if( vsVersion > CD3DDevice::getInstance().getVShaderVersion() )
		mSoftwareVertexProcessed = true;

	// sort value
	mSortValue = 0;
	mSortValue += mBackToFrontSorted ? 0x10 : 0x00;
	mSortValue += mSoftwareVertexProcessed ? 0 : 1;
	//mSortValue += vsVersion;

	return true;
}
