#include "Anim.h"

CAnim::CAnim(DataAnim animation, const std::string& curve, int anims )
:	mDefaultPos(0,0,0),
	mDefaultRot(0,0,0,1),
	mDefaultScale(1),
	mPosAnim(0), mRotAnim(0), mScaleAnim(0)
{
	CAnimationBunch* ab = g_data_anim[animation];
	mCurve = ab->getCurveIndexByName( curve );
	if( anims & POSITION ) {
		mPosAnim = ab->findAnim("pos", TYPE_VECTOR3);
	}
	if( anims & ROTATION ) {
		mRotAnim = ab->findAnim("rot", TYPE_QUATERNION);
	}
	if( anims & SCALE ) {
		mScaleAnim = ab->findAnim("scale", TYPE_FLOAT);
	}
}

void CAnim::sample( float t, SMatrix4x4& dest ) const
{
	SVector3 pos;
	SQuaternion rot;
	float scale;

	if( mPosAnim )
		mPosAnim->sample(t, mCurve, 1, &pos.x);
	else
		pos = mDefaultPos;
	if( mRotAnim )
		mRotAnim->sample(t, mCurve, 1, &rot.x);
	else
		rot = mDefaultRot;
	if( mScaleAnim )
		mScaleAnim->sample(t, mCurve, 1, &scale);
	else
		scale = mDefaultScale;

	dest = SMatrix4x4(pos, rot);
	dest.getAxisX() *= scale;
	dest.getAxisY() *= scale;
	dest.getAxisZ() *= scale;
}

void CAnim::samplePos( float t, SVector3& dest ) const
{
	if( mPosAnim )
		mPosAnim->sample(t, mCurve, 1, &dest.x);
	else
		dest = mDefaultPos;
}
