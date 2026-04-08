#pragma once

#include "../gfx/Mesh.h"

namespace dingus {


class CMeshSerializer {
public:
	/// Return true if successful.
	static CMesh* loadMeshFromFile( const char* fileName );
};


}; // namespace
