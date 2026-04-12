#pragma once

#include "../math/Maths.h"
#include "DataFiles.h"

class Anim
{
  public:
    enum eAnims
    {
        POSITION = 1 << 0,
        ROTATION = 1 << 1,
        ALL = POSITION | ROTATION
    };

  public:
    Anim(DataAnim animation, const std::string &curve, int anims = ALL);

    void sample(float t, Matrix4x4 &dest) const;
    void samplePos(float t, Vector3 &dest) const;

  private:
    Vector3 mDefaultPos;
    Quaternion mDefaultRot;

    const SampledAnimation *mPosAnim;
    const SampledAnimation *mRotAnim;
    int mCurve;
};
