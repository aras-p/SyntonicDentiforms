#pragma once

#include "Camera.h"
#include "DataFiles.h"

// --------------------------------------------------------------------------

enum eRenderMode {
	RM_RECV_HI,		// full-detail (maybe per-pixel lights), receive other shadows
	RM_RECV_LO,		// low-detail (maybe per-vertex lights), receive other shadows
	RM_SHADOW,		// shadow casting
	RM_REFLECTIVE,	// receive reflection
	RM_HI,			// just full-detail (no shadows)
};
enum eCubeFaces {
	CFACE_PX = 0, CFACE_NX, CFACE_PY, CFACE_NY, CFACE_PZ, CFACE_NZ, CFACE_COUNT };

class CMeshEntity {
public:
	CMeshEntity(DataMesh type);
    ~CMeshEntity();
    
    /// Updates WV/WVP from current matrix and current render camera
    void updateMatrices() {
        mWVMatrix = mMatrix * gRenderCam.getViewMatrix();
        mWVPMatrix = mMatrix * gRenderCam.getViewProjMatrix();
    }

	/// Culls with current W matrix and given VP matrix. Returns true if outside frustum.
	bool	frustumCull( const SMatrix4x4& viewProj ) const {
		return mMesh->getTotalAABB().frustumCull( mMatrix, viewProj );
	}
	/// Culls with current WVP matrix. Returns true if outside frustum.
	bool	frustumCull() const {
		return mMesh->getTotalAABB().frustumCull( mWVPMatrix );
	}
	/// Updates WVP, renders
	void	render(eRenderMode renderMode, sg_bindings *binds);

public:
    SMatrix4x4    mMatrix;
    SMatrix4x4    mWVMatrix;
    SMatrix4x4    mWVPMatrix;

private:
	CMesh*  mMesh;
	int		mRenderModesMask;
	float   mShadowVal;
	int		mCubeFace; // reflection cube face

	static int	mShadowIDGenerator;
};
