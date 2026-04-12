#include "Anim.h"

CAnim::CAnim(DataAnim animation, const std::string &curve, int anims)
    : mDefaultPos(0, 0, 0),
      mDefaultRot(0, 0, 0, 1),
      mPosAnim(0), mRotAnim(0)
{
    CAnimationBunch *ab = g_data_anim[animation];
    mCurve = ab->getCurveIndexByName(curve);
    if (anims & POSITION)
    {
        mPosAnim = ab->findAnim("pos", TYPE_VECTOR3);
    }
    if (anims & ROTATION)
    {
        mRotAnim = ab->findAnim("rot", TYPE_QUATERNION);
    }
}

void CAnim::sample(float t, SMatrix4x4 &dest) const
{
    SVector3 pos;
    SQuaternion rot;

    if (mPosAnim)
        mPosAnim->sample(t, mCurve, 1, &pos.x);
    else
        pos = mDefaultPos;
    if (mRotAnim)
        mRotAnim->sample(t, mCurve, 1, &rot.x);
    else
        rot = mDefaultRot;

    dest = SMatrix4x4(pos, rot);
}

void CAnim::samplePos(float t, SVector3 &dest) const
{
    if (mPosAnim)
        mPosAnim->sample(t, mCurve, 1, &dest.x);
    else
        dest = mDefaultPos;
}
