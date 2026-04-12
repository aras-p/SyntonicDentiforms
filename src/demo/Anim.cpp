#include "Anim.h"

Anim::Anim(DataAnim animation, const std::string &curve, int anims)
    : mDefaultPos(0, 0, 0),
      mDefaultRot(0, 0, 0, 1),
      mPosAnim(0), mRotAnim(0)
{
    AnimationBunch *ab = g_data_anim[animation];
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

void Anim::sample(float t, Matrix4x4 &dest) const
{
    Vector3 pos;
    Quaternion rot;

    if (mPosAnim)
        mPosAnim->sample(t, mCurve, 1, &pos.x);
    else
        pos = mDefaultPos;
    if (mRotAnim)
        mRotAnim->sample(t, mCurve, 1, &rot.x);
    else
        rot = mDefaultRot;

    dest = Matrix4x4(pos, rot);
}

void Anim::samplePos(float t, Vector3 &dest) const
{
    if (mPosAnim)
        mPosAnim->sample(t, mCurve, 1, &dest.x);
    else
        dest = mDefaultPos;
}
