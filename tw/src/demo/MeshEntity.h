#ifndef __DEMO_MESH_ENTITY_H
#define __DEMO_MESH_ENTITY_H

#include "Entity.h"
#include <dingus/renderer/RenderableMesh.h>

// --------------------------------------------------------------------------

enum eRenderMode {
	RM_RECV_HI = (1<<0),	// full-detail (maybe per-pixel lights), receive other shadows
	RM_RECV_LO = (1<<1),	// low-detail (maybe per-vertex lights), receive other shadows
	RM_SHADOW = (1<<2),		// shadow casting
	RM_REFLECTIVE = (1<<3), // receive reflection
	RM_HI = (1<<4),			// just full-detail (no shadows)
};
enum eCubeFaces {
	CFACE_PX = 0, CFACE_NX, CFACE_PY, CFACE_NY, CFACE_PZ, CFACE_NZ, CFACE_COUNT };


class CMeshEntity : public CAbstractEntity {
public:
	CMeshEntity( const std::string& clazz );
	virtual ~CMeshEntity();

	/// Culls with current W matrix and given VP matrix. Returns true if outside frustum.
	bool	frustumCull( const SMatrix4x4& viewProj ) const {
		return mMeshRecvHi->getMesh().getTotalAABB().frustumCull( mMatrix, viewProj );
	}
	/// Culls with current WVP matrix. Returns true if outside frustum.
	bool	frustumCull() const {
		return mMeshRecvHi->getMesh().getTotalAABB().frustumCull( mWVPMatrix );
	}
	/// Updates WVP, adds to render context if not culled.
	void	render( int renderMode );

private:
	CRenderableMesh*	mMeshRecvHi;
	CRenderableMesh*	mMeshRecvLo;
	CRenderableMesh*	mMeshShadow;
	CRenderableMesh*	mMeshReflection;
	CRenderableMesh*	mMeshHi;
	int		mRenderModes;
	int		mShadowID; // shadow casting object ID
	int		mCubeFace; // reflection cube face

	static int	mShadowIDGenerator;
};


#endif
