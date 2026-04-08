#pragma once

#include "AnimType.h"
#include "../resource/ResourceId.h"
#include "../math/Vector3.h"
#include "../math/Quaternion.h"
#include "../math/Matrix4x4.h"

namespace dingus {

namespace traits {
	template<typename _T> struct anim_type { enum { value = TYPE_CUSTOM }; };
	struct anim_type<float> { enum { value = TYPE_FLOAT }; };
	struct anim_type<int> { enum { value = TYPE_INT }; };
	struct anim_type<SVector3> { enum { value = TYPE_VECTOR3 }; };
	struct anim_type<SMatrix4x4> { enum { value = TYPE_MATRIX }; };
	struct anim_type<CResourceId> { enum { value = TYPE_RESOURCEID }; };
	//struct anim_type<CKeyframe> { enum { value = TYPE_KEYFRAME }; };
	struct anim_type<SQuaternion> { enum { value = TYPE_QUATERNION }; };
};

};

