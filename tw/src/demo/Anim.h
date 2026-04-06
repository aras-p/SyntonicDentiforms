#ifndef __ANIM_H
#define __ANIM_H

#include <dingus/animator/AnimationBunch.h>
#include <dingus/math/Matrix4x4.h>


class CAnim {
public:
	enum eAnims {
		POSITION	= 1<<0,
		ROTATION	= 1<<1,
		SCALE		= 1<<2,
		ALL			= POSITION | ROTATION | SCALE
	};

public:
	CAnim( const std::string& animation, const std::string& curve, int anims = ALL );

	void	setDefaultPos( const SVector3& pos ) { mDefaultPos = pos; }
	void	setDefaultRot( const SQuaternion& rot ) { mDefaultRot = rot; }
	void	setDefaultScale( float s ) { mDefaultScale = s; }

	void sample( float t, SMatrix4x4& dest ) const;
	void samplePos( float t, SVector3& dest ) const;

private:
	SVector3	mDefaultPos;
	SQuaternion	mDefaultRot;
	float		mDefaultScale;

	CAnimationBunch::TVector3Animation* mPosAnim;
	CAnimationBunch::TQuatAnimation*	mRotAnim;
	CAnimationBunch::TFloatAnimation*	mScaleAnim;
	int mCurve;
};

#endif
