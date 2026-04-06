// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ANIM_TYPE_H
#define __ANIM_TYPE_H

namespace dingus {

// NOTE: change array in anim_type_string if eAnimType changed!
// NOTE: implement corresponding traits if new types added!

enum eAnimType {
	TYPE_FLOAT = 1,
	TYPE_INT,
	TYPE_VECTOR3,
	TYPE_QUATERNION,
	TYPE_MATRIX,
	TYPE_RESOURCEID,
	//TYPE_KEYFRAME,
	TYPE_CUSTOM
};

const char* anim_type_string( eAnimType type );

};

#endif
