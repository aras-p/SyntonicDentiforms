#pragma once

#include "Scene.h"
#include <src/animator/Animation.h>
#include <src/math/Matrix4x4.h>

// ------------------------------------

class CTeethAnim {
public:
	CTeethAnim(DataAnim animation, int startFrame, int endFrame);
	~CTeethAnim();

	bool evaluate( float t );
	float getRelTime( float t ) const;
	bool possiblyAddTooth( const std::string& name, int index );
	int getCount() const { return mAnim->getCurveCount(); }

private:
	int		mTotalFrames, mStartFrame, mEndFrame;
	CAnimationBunch*	mAnim;
	const CAnimationBunch::TVector3Animation* mPosAnim;
	const CAnimationBunch::TQuatAnimation*	mRotAnim;
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

	void	renderTeethLines(int pack, float t);
	void	renderTeethStuff(int pack, float t, float cutAlpha, float aspect);
	void	renderTeethUI(int pack, float t, float cutAlpha, float aspect);
	
protected:
	virtual void	evaluateMeshes( float t );

	static void renderTeethBills( int pack, float t, float relT, float cutAlpha, bool masks, float aspect);

private:
	CAnim*			mAnimAxes[GEARS];
	int		mAxesIdx[GEARS];
	int		mGearsIdx[GEARS];
	CTeethAnim*	mAnimTeeth[TEETHPACKS];
};

