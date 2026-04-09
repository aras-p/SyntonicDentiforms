#pragma once

#include <src/math/Matrix4x4.h>
#include <src/math/Vector3.h>
#include <src/math/Quaternion.h>

#include "DataFiles.h"

class CAnim {
public:
	enum eAnims {
		POSITION	= 1<<0,
		ROTATION	= 1<<1,
		SCALE		= 1<<2,
		ALL			= POSITION | ROTATION | SCALE
	};

public:
    CAnim(DataAnim animation, const std::string& curve, int anims = ALL);

	void	setDefaultPos( const SVector3& pos ) { mDefaultPos = pos; }
	void	setDefaultRot( const SQuaternion& rot ) { mDefaultRot = rot; }
	void	setDefaultScale( float s ) { mDefaultScale = s; }

	void sample( float t, SMatrix4x4& dest ) const;
	void samplePos( float t, SVector3& dest ) const;

private:
	SVector3	mDefaultPos;
	SQuaternion	mDefaultRot;
	float		mDefaultScale;

	const CAnimationBunch::TVector3Animation* mPosAnim;
	const CAnimationBunch::TQuatAnimation*	mRotAnim;
	const CAnimationBunch::TFloatAnimation*	mScaleAnim;
	int mCurve;
};

