// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "W32StdConsoleRenderingContext.h"

using namespace dingus;

//---------------------------------------------------------------------------
// CW32StdConsoleRenderingContext
//---------------------------------------------------------------------------

CW32StdConsoleRenderingContext::CW32StdConsoleRenderingContext()
{
	AllocConsole();
	mConsoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	assert( mConsoleHandle );
}

CW32StdConsoleRenderingContext::~CW32StdConsoleRenderingContext()
{
	FreeConsole();
}

void CW32StdConsoleRenderingContext::write( const std::string& message )
{
	DWORD charsWritten;
	WriteConsole( mConsoleHandle, message.c_str(), message.length(), &charsWritten, NULL );
}