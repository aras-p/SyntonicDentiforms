// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "../dxutils/D3DFont.h"

#include "D3DConsoleRenderingContext.h"

using namespace dingus;

//---------------------------------------------------------------------------
// CD3DTextBoxConsoleRenderingContext
//---------------------------------------------------------------------------

CD3DTextBoxConsoleRenderingContext::CD3DTextBoxConsoleRenderingContext( CD3DFont& font, int x, int y, D3DCOLOR color )
:	mFont( font ),
	mX( x ),
	mY( y ),
	mColor( color )
{
}

void CD3DTextBoxConsoleRenderingContext::write( const std::string& message )
{
	mBuffer += message;
}

void CD3DTextBoxConsoleRenderingContext::flush()
{
	mFont.drawText( (float)mX, (float)mY, mColor, (char*)mBuffer.c_str() );
	mBuffer.erase();
}
