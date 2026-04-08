#pragma once

#include "../gfx/Mesh.h"


class CMeshSerializer {
public:
	/// Return true if successful.
	static CMesh* loadMeshFromFile( const char* fileName );
};


