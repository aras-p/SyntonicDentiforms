#include "Anim.h"
#include "../system/Globals.h"

CAnim::CAnim( const std::string& animation, const std::string& curve, int anims )
:	mDefaultPos(0,0,0),
	mDefaultRot(0,0,0,1),
	mDefaultScale(1),
	mPosAnim(0), mRotAnim(0), mScaleAnim(0)
{
	CAnimationBunch* ab = RGET_ANIM(animation);
	mCurve = ab->getCurveIndexByName( curve );
	if( anims & POSITION ) {
		mPosAnim = ab->findVector3Anim( "pos" );
	}
	if( anims & ROTATION ) {
		mRotAnim = ab->findQuatAnim( "rot" );
	}
	if( anims & SCALE ) {
		mScaleAnim = ab->findFloatAnim( "scale" );
	}
}

void CAnim::sample( float t, SMatrix4x4& dest ) const
{
	SVector3 pos;
	SQuaternion rot;
	float scale;

	if( mPosAnim )
		mPosAnim->sample( t, mCurve, 1, &pos );
	else
		pos = mDefaultPos;
	if( mRotAnim )
		mRotAnim->sample( t, mCurve, 1, &rot );
	else
		rot = mDefaultRot;
	if( mScaleAnim )
		mScaleAnim->sample( t, mCurve, 1, &scale );
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
		mPosAnim->sample( t, mCurve, 1, &dest );
	else
		dest = mDefaultPos;
}