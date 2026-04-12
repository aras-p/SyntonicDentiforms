#pragma once

#include "../math/Maths.h"

#include <memory>
#include <string>
#include <vector>
#include <assert.h>
#include <string.h>

enum eAnimType {
    TYPE_FLOAT = 1,
    TYPE_VECTOR3 = 3,
    TYPE_QUATERNION = 4,
};

// Animation curve information.
// This just has first value index (in array of values shared between several curves),
// interpolation type and a collapsed value (if curve is collapsed into single value).
struct SAnimCurve
{
    enum eIpol { NONE = 0, STEP, LINEAR };

    bool isCollapsed() const { return ipol == NONE; }

    int			firstSampleIndex = 0;
    eIpol		ipol = NONE;
    float		collapsedValue[4] = { 0,0,0,0 };
};

//  A sampled animation.
//  Each curve consists of the same number of samples (but can be collapsed
//  into a single value).
class CSampledAnimation {
public:
	// CLAMP clamps animation to ends.
	// REPEAT loops animation (with interpolation from last sample to first).
	// REPEAT_LAST loops animation (no interpolation from last to first,
	//   instead, the last sample is for transition from pre-last to last).
	enum eLoopType { CLAMP = 0, REPEAT, REPEAT_LAST };

public:
	void init(eAnimType sampleType, const std::string& name, int samplesInCurve, eLoopType loopType)
	{
		mSampleType = sampleType;
		mName = name;
		mSamplesInCurve = samplesInCurve;
		mLoopType = loopType;
	}
	eAnimType getType() const { return mSampleType; }
	const std::string& getName() const { return mName; }

	void	resizeSamples(int sampleCount) { mSampleData.resize(sampleCount * getFloatsPerSample()); }
	float* getSampleData() { return mSampleData.data(); }

	void	addCurve(const SAnimCurve& curve) { mCurves.push_back(curve); }
	void	reserveCurves(int curveCount) { mCurves.reserve(curveCount); }
	int		getCurveCount() const { return mCurves.size(); }
	const SAnimCurve& getCurve(int index) const
	{
		assert(index >= 0 && index < getCurveCount());
		return mCurves[index];
	}

	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	void	timeToIndex(float time, int& index1, int& index2, float& alpha) const;

	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	void sample(float time, int firstCurve, int numCurves, float* dest) const;

private:
	int getFloatsPerSample() const {
		switch (mSampleType) {
		case TYPE_FLOAT: return 1;
		case TYPE_VECTOR3: return 3;
		case TYPE_QUATERNION: return 4;
		}
		return 0;
	}

private:
	std::string     mName;
	std::vector<float> mSampleData;
	std::vector<SAnimCurve>	mCurves;
	eAnimType		mSampleType;
	int				mSamplesInCurve;
	eLoopType		mLoopType;
};


// --------------------------------------------------------------------------

inline void CSampledAnimation::timeToIndex(float time, int& index1, int& index2, float& alpha) const
{
	int n = mSamplesInCurve;
	float frame = time * (mLoopType == REPEAT_LAST ? n - 1 : n);
	index1 = int(frame);
	index2 = index1 + 1;
	alpha = frame - float(index1);

	switch (mLoopType) {
	case CLAMP:
		if (index1 < 0) index1 = 0;
		else if (index1 >= n) index1 = n - 1;
		if (index2 < 0) index2 = 0;
		else if (index2 >= n) index2 = n - 1;
		break;
	case REPEAT:
		index1 %= n;
		index2 %= n;
		break;
	case REPEAT_LAST:
		index1 %= (n - 1);
		index2 = index1 + 1;
	}
};

class CAnimationBunch {
public:
	CAnimationBunch() { };
	~CAnimationBunch();

	CSampledAnimation& addAnim()
    {
        mAnims.emplace_back(CSampledAnimation());
        return mAnims.back();
    }
	const CSampledAnimation* findAnim(const std::string& name, eAnimType type)
    {
        for(const CSampledAnimation& anim : mAnims)
        {
            if (anim.getType() == type && anim.getName() == name)
                return &anim;
        }
        return nullptr;
    }

	void	addCurveDesc(const std::string& name);
    int     getCurveCount() const { return (int)mCurveNames.size(); }
	const std::string& getCurveName( int curveIdx ) const;
	int		getCurveIndexByName( const std::string& name ) const;

private:
	std::vector<CSampledAnimation>	mAnims;
    std::vector<std::string>        mCurveNames;
};


// --------------------------------------------------------------------------

inline void CAnimationBunch::addCurveDesc(const std::string& name)
{
	assert( getCurveIndexByName(name) == -1 );
    mCurveNames.push_back(name);
}
inline const std::string& CAnimationBunch::getCurveName( int curveIdx ) const
{
    assert( curveIdx >= 0 && curveIdx < mCurveNames.size() );
	return mCurveNames[curveIdx];
}
inline int CAnimationBunch::getCurveIndexByName( const std::string& name ) const
{
	for (int i = 0; i < mCurveNames.size(); ++i)
    {
        if (mCurveNames[i] == name)
			return i;
	}
	return -1;
}

CAnimationBunch* load_animation(const char* path);

