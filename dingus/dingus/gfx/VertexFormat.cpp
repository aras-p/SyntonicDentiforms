// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "VertexFormat.h"

using namespace dingus;

int CVertexFormat::calcFloat3Size( eFloat3Mode flt3 )
{
	assert( flt3 >= FLT3_NONE && flt3 <= FLT3_DEC3N );
	int sizes[] = { 0, 3*4, 4, 4 };
	return sizes[flt3];
}

//@TODO
//int CVertexFormat::calcFloat3Type( eFloat3Mode flt3 )
//{
//	assert( flt3 > FLT3_NONE && flt3 <= FLT3_DEC3N );
//	int types[] = { 0, D3DDECLTYPE_FLOAT3, D3DDECLTYPE_D3DCOLOR, D3DDECLTYPE_DEC3N };
//	return types[flt3];
//}

int CVertexFormat::calcSkinSize() const
{
	eSkinMode skin = getSkinMode();
	if( skin == SKIN_NONE )
		return 0;
	eFloat3Mode data = getSkinDataMode();
	assert( data != FLT3_NONE );
	int skinsize = 4; // indices
	if( data != FLT3_FLOAT3 )
		skinsize += 4; // non-float case: all weights packed into single DWORD
	else
		skinsize += skin * 4; // float case: (bones-1) floats
	return skinsize;
}

int CVertexFormat::calcVertexSize() const
{
	int size = 0;
	// position
	if( hasPosition() )
		size += 3*4;
	// normals, tangents, binormals
	size += calcFloat3Size( getNormalMode() );
	size += calcFloat3Size( getTangentMode() );
	size += calcFloat3Size( getBinormMode() );
	// skin
	size += calcSkinSize();
	// color
	if( hasColor() )
		size += 4;
	// UVs
	for( int i = 0; i < UV_COUNT; ++i ) {
		size += calcUVSize( getUVMode(i) );
	}
	return size;
}

int CVertexFormat::calcComponentCount() const
{
	int size = 0;
	// position
	if( hasPosition() ) ++size;
	// normals, tangents, binormals
	if( getNormalMode() ) ++size;
	if( getTangentMode() ) ++size;
	if( getBinormMode() ) ++size;
	// skin
	if( getSkinMode() ) size += 2; // weights and indices
	// color
	if( hasColor() ) ++size;
	// UVs
	for( int i = 0; i < UV_COUNT; ++i )
		if( getUVMode(i) ) ++size;
	return size;
}
