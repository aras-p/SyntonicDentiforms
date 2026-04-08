#include "AnimType.h"

const char* anim_type_string( eAnimType type )
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
