#pragma once

#include "../math/Maths.h"
#include "DataFiles.h"

class CAnim
{
  public:
    enum eAnims
    {
        POSITION = 1 << 0,
        ROTATION = 1 << 1,
        ALL = POSITION | ROTATION
    };

  public:
    CAnim(DataAnim animation, const std::string &curve, int anims = ALL);

    void sample(float t, SMatrix4x4 &dest) const;
    void samplePos(float t, SVector3 &dest) const;

  private:
    SVector3 mDefaultPos;
    SQuaternion mDefaultRot;

    const CSampledAnimation *mPosAnim;
    const CSampledAnimation *mRotAnim;
    int mCurve;
};
