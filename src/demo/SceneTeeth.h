#pragma once

#include "../animator/Animation.h"
#include "../math/Maths.h"
#include "Scene.h"

// ------------------------------------

class TeethAnim
{
  public:
    TeethAnim(DataAnim animation, int startFrame, int endFrame);
    ~TeethAnim();

    bool evaluate(float t);
    float getRelTime(float t) const;
    bool possiblyAddTooth(const std::string &name, int index);
    int getCount() const { return mAnim->getCurveCount(); }

  private:
    int mTotalFrames, mStartFrame, mEndFrame;
    AnimationBunch *mAnim;
    const SampledAnimation *mPosAnim;
    const SampledAnimation *mRotAnim;
    typedef std::vector<Vector3> TVec3Vector;

  public:
    Vector3 *mVectors;
    Quaternion *mQuats;
    int *mTeethIdx;

    TVec3Vector *mPaths;
};

// ------------------------------------

class SceneTeeth : public Scene
{
  public:
    enum
    {
        GEARS = 2
    };
    enum
    {
        TEETHPACKS = 4
    };

  public:
    SceneTeeth(int number);
    virtual ~SceneTeeth();

    virtual void initialize();

    void renderTeethLines(int pack, float t);
    void renderTeethStuff(int pack, float t, float cutAlpha, float aspect);
    void renderTeethUI(int pack, float t, float cutAlpha, float aspect);

  protected:
    virtual void evaluateMeshes(float t);

    static void renderTeethBills(int pack, float t, float relT, float cutAlpha, bool masks, float aspect);

  private:
    Anim *mAnimAxes[GEARS];
    int mAxesIdx[GEARS];
    int mGearsIdx[GEARS];
    TeethAnim *mAnimTeeth[TEETHPACKS];
};
