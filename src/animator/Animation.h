#pragma once

#include "../math/Maths.h"

#include <assert.h>
#include <memory>
#include <string.h>
#include <string>
#include <vector>

enum eAnimType
{
    TYPE_VECTOR3 = 3,
    TYPE_QUATERNION = 4,
};

// Animation curve information.
// This just has first value index (in array of values shared between several curves),
// interpolation type and a collapsed value (if curve is collapsed into single value).
struct AnimCurve
{
    enum eIpol
    {
        NONE = 0,
        LINEAR = 2
    };

    bool isCollapsed() const { return ipol == NONE; }

    int firstSampleIndex = 0;
    eIpol ipol = NONE;
    float collapsedValue[4] = {0, 0, 0, 0};
};

//  A sampled animation.
//  Each curve consists of the same number of samples (but can be collapsed
//  into a single value).
class SampledAnimation
{
  public:
    void init(eAnimType sampleType, const std::string &name, int samplesInCurve)
    {
        mSampleType = sampleType;
        mName = name;
        mSamplesInCurve = samplesInCurve;
    }
    eAnimType getType() const { return mSampleType; }
    const std::string &getName() const { return mName; }

    void resizeSamples(int sampleCount) { mSampleData.resize(sampleCount * getFloatsPerSample()); }
    float *getSampleData() { return mSampleData.data(); }

    void addCurve(const AnimCurve &curve) { mCurves.push_back(curve); }
    void reserveCurves(int curveCount) { mCurves.reserve(curveCount); }
    int getCurveCount() const { return mCurves.size(); }
    const AnimCurve &getCurve(int index) const
    {
        assert(index >= 0 && index < getCurveCount());
        return mCurves[index];
    }

    /**
     *  @param time Relative time (zero is start, one is end).
     */
    void timeToIndex(float time, int &index1, int &index2, float &alpha) const;

    /**
     *  @param time Relative time (zero is start, one is end).
     */
    void sample(float time, int firstCurve, int numCurves, float *dest) const;

  private:
    int getFloatsPerSample() const
    {
        switch (mSampleType)
        {
        case TYPE_VECTOR3:
            return 3;
        case TYPE_QUATERNION:
            return 4;
        }
        return 0;
    }

  private:
    std::string mName;
    std::vector<float> mSampleData;
    std::vector<AnimCurve> mCurves;
    eAnimType mSampleType;
    int mSamplesInCurve;
};

// --------------------------------------------------------------------------

inline void SampledAnimation::timeToIndex(float time, int &index1, int &index2, float &alpha) const
{
    int n = mSamplesInCurve;
    float frame = time * n;
    index1 = int(frame);
    index2 = index1 + 1;
    alpha = frame - float(index1);

    if (index1 < 0)
        index1 = 0;
    else if (index1 >= n)
        index1 = n - 1;
    if (index2 < 0)
        index2 = 0;
    else if (index2 >= n)
        index2 = n - 1;
};

class AnimationBunch
{
  public:
    AnimationBunch() {};
    ~AnimationBunch();

    SampledAnimation &addAnim()
    {
        mAnims.emplace_back(SampledAnimation());
        return mAnims.back();
    }
    const SampledAnimation *findAnim(const std::string &name, eAnimType type)
    {
        for (const SampledAnimation &anim : mAnims)
        {
            if (anim.getType() == type && anim.getName() == name)
                return &anim;
        }
        return nullptr;
    }

    void addCurveDesc(const std::string &name);
    int getCurveCount() const { return (int)mCurveNames.size(); }
    const std::string &getCurveName(int curveIdx) const;
    int getCurveIndexByName(const std::string &name) const;

  private:
    std::vector<SampledAnimation> mAnims;
    std::vector<std::string> mCurveNames;
};

// --------------------------------------------------------------------------

inline void AnimationBunch::addCurveDesc(const std::string &name)
{
    assert(getCurveIndexByName(name) == -1);
    mCurveNames.push_back(name);
}
inline const std::string &AnimationBunch::getCurveName(int curveIdx) const
{
    assert(curveIdx >= 0 && curveIdx < mCurveNames.size());
    return mCurveNames[curveIdx];
}
inline int AnimationBunch::getCurveIndexByName(const std::string &name) const
{
    for (int i = 0; i < mCurveNames.size(); ++i)
    {
        if (mCurveNames[i] == name)
            return i;
    }
    return -1;
}

AnimationBunch *load_animation(const char *path);
