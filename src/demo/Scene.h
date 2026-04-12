#pragma once

#include "MeshEntity.h"
#include "Anim.h"


class CScene {
public:
	struct SMesh {
		CMeshEntity*	mesh;
		SVector3		pos;
		SVector3		rot;
		SVector3		rotVel; // over all animation
		int				parentIdx;
	};

public:
	CScene( int number );
	virtual ~CScene();

	virtual void initialize();

	void	evaluate( float t, SMatrix4x4& light, SMatrix4x4& camera );
	void	evaluateCamera( float t, SMatrix4x4& camera ) const;
	void	evaluateLight( float t, SMatrix4x4& light ) const;
	void	render(eRenderMode renderMode, sg_bindings* binds);

	int		getLength() const { return mLength; }

	CMeshEntity* addStaticMesh(DataMesh data);

	void	addCut( float frame );
	float	getPastCut( float t ) const;
	int		getCutCount() const { return mCutTimes.size(); }
	
protected:
	virtual void	evaluateMeshes( float t );
	static void	toMatrix( const SVector3& pos, const SVector3& rot, SMatrix4x4& m );

protected:
	void	recurseAdd( int idx );
	
protected:
	int			mNumber;
	std::vector<SMesh>	mMeshes;	// owns meshes
	std::vector<int>	mEvalOrder;	// depth-first order for hierarchy
	CAnim*		mAnimLight;
	CAnim*		mAnimCamera;
	int			mLength;	// in max frames

	std::vector<float>	mCutTimes; // 0..1
};

