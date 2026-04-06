#ifndef __SCENE_TEETH_H
#define __SCENE_TEETH_H

#include "Scene.h"
#include <dingus/animator/AnimationBunch.h>
#include <dingus/math/Matrix4x4.h>

// ------------------------------------

class CTeethAnim {
public:
	CTeethAnim( const std::string& animation, int startFrame, int endFrame );
	~CTeethAnim();

	bool evaluate( float t );
	float getRelTime( float t ) const;
	bool possiblyAddTooth( const std::string& name, int index );
	int getCount() const { return mAnim->getCurveCount(); }

private:
	int		mTotalFrames, mStartFrame, mEndFrame;
	CAnimationBunch*	mAnim;
	CAnimationBunch::TVector3Animation* mPosAnim;
	CAnimationBunch::TQuatAnimation*	mRotAnim;
	typedef std::vector<SVector3>	TVec3Vector;

public:
	SVector3*		mVectors;
	SQuaternion*	mQuats;
	int*			mTeethIdx;
	
	TVec3Vector*		mPaths;
};


// ------------------------------------

class CSceneTeeth : public CScene {
public:
	enum { GEARS = 2 };
	enum { TEETHPACKS = 4 };
public:
	CSceneTeeth( int number );
	virtual ~CSceneTeeth();
	
	virtual void	initialize();

	void	renderTeethStuff( int pack, float t, float cutAlpha );
	void	renderTeethUI( int pack, float t, float cutAlpha );
	
protected:
	virtual void	evaluateMeshes( float t );

	static void renderTeethBills( int pack, float t, float relT, float cutAlpha, bool masks );

private:
	CAnim*			mAnimAxes[GEARS];
	int		mAxesIdx[GEARS];
	int		mGearsIdx[GEARS];
	CTeethAnim*	mAnimTeeth[TEETHPACKS];

	CRenderableMesh*	mToonQuad;
	CRenderableMesh*	mCompositeQuad;
	CRenderableMesh*	mToothMaskMesh;
	CRenderableMesh*	mMaskGears[GEARS];
	SMatrix4x4			mMaskMeshWVP;
};


#endif
