// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "AnimType.h"

const char* dingus::anim_type_string( dingus::eAnimType type )
{
	// NOTE: change array if eAnimType changed!
	static const char* types[] = {
		"",
		"TYPE_FLOAT",
		"TYPE_INT",
		"TYPE_VEC3",
		"TYPE_QUAT",
		"TYPE_MATRIX",
		"TYPE_RID",
		"TYPE_CUSTOM",
	};
	return types[type];
};
