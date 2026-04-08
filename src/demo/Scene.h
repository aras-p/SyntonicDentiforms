#pragma once

#include "MeshEntity.h"
#include "Anim.h"


class CScene {
public:
	struct SMesh {
		std::string		name;
		std::string		parent;
		CMeshEntity*	mesh;
		SVector3		pos;
		SVector3		rot;
		SVector3		rotVel; // over all animation
		int				parentIdx;
	};
	typedef std::vector<SMesh>	TMeshVector;
	typedef std::vector<int>	TIntVector;
	typedef std::vector<float>	TFloatVector;

public:
	CScene( int number );
	virtual ~CScene();

	virtual void initialize();

	void	evaluate( float t, SMatrix4x4& light, SMatrix4x4& camera );
	void	evaluateCamera( float t, SMatrix4x4& camera ) const;
	void	evaluateLight( float t, SMatrix4x4& light ) const;
	void	render(eRenderMode renderMode, sg_bindings* binds);

	int		getLength() const { return mLength; }

	CMeshEntity* addStaticMesh( const std::string& name );

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
	TMeshVector	mMeshes;	// owns meshes
	TIntVector	mEvalOrder;	// depth-first order for hierarchy
	CAnim*		mAnimLight;
	CAnim*		mAnimCamera;
	int			mLength;	// in max frames

	TFloatVector	mCutTimes; // 0..1
};

