#pragma once

#include "Camera.h"
#include "DataFiles.h"

// --------------------------------------------------------------------------

enum eRenderMode {
	RM_LIT_SHADOWED,
	RM_LIT_SHADOWED_FLIP,
	RM_SHADOW_CASTER,
	RM_REFLECTIVE,
	RM_LIT,
};
enum eCubeFaces {
	CFACE_PX = 0, CFACE_NX, CFACE_PY, CFACE_NY, CFACE_PZ, CFACE_NZ, CFACE_COUNT };

class CMeshEntity {
public:
	CMeshEntity(DataMesh type);
    ~CMeshEntity();
    
	bool	frustumCull(const SPlane planes[6]) const
	{
		return mMesh->getTotalAABB().frustumCull(mMatrix, planes);
	}

	void	render(eRenderMode renderMode, sg_bindings *binds, const SPlane planes[6]);

public:
    SMatrix4x4    mMatrix;

private:
	CMesh*  mMesh;
	int		mRenderModesMask;
	int		mCubeFace; // reflection cube face
};
