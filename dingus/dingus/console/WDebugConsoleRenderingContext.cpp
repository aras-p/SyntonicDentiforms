// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "WDebugConsoleRenderingContext.h"

using namespace dingus;

//---------------------------------------------------------------------------
// WDebugConsoleRenderingContext
//---------------------------------------------------------------------------

void CWDebugConsoleRenderingContext::write( const std::string& message )
{
	OutputDebugString( message.c_str() );
}
