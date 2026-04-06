// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "RenderStats.h"

using namespace dingus;


void CRenderStats::reset()
{
	// just set to zero...
	memset( this, 0, sizeof(*this) );
}
