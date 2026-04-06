// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "Renderable.h"

using namespace dingus;

void CRenderable::beforeRender()
{
	TListenerVector::iterator it, itEnd = getListeners().end();
	for( it = getListeners().begin(); it != itEnd; ++it ) {
		assert( *it );
		(*it)->beforeRender( *this );
	}
}

void CRenderable::afterRender()
{
	TListenerVector::iterator it, itEnd = getListeners().end();
	for( it = getListeners().begin(); it != itEnd; ++it ) {
		assert( *it );
		(*it)->afterRender( *this );
	}
}

