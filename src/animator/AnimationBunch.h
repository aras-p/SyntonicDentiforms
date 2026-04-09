#pragma once

#include "SampledAnimation.h"
#include "../math/Vector3.h"
#include "../math/Quaternion.h"
#include <string>
#include <vector>
#include <assert.h>


class CAnimationBunch {
public:
    typedef CSampledAnimation<SVector3>	TVector3Animation;
	typedef CSampledAnimation<SQuaternion>	TQuatAnimation;
	typedef CSampledAnimation<float>		TFloatAnimation;

public:
	CAnimationBunch() { };
	~CAnimationBunch();

    TVector3Animation& addVector3Anim()
    {
        mVector3Anims.emplace_back(TVector3Animation());
        return mVector3Anims.back();
    }
    TQuatAnimation& addQuatAnim()
    {
        mQuatAnims.emplace_back(TQuatAnimation());
        return mQuatAnims.back();
    }
    TFloatAnimation& addFloatAnim()
    {
        mFloatAnims.emplace_back(TFloatAnimation());
        return mFloatAnims.back();
    }

	const TVector3Animation* findVector3Anim(const std::string& name)
    {
        for(const TVector3Animation& anim : mVector3Anims)
        {
            if (anim.getName() == name)
                return &anim;
        }
        return nullptr;
    }
	const TQuatAnimation* findQuatAnim(const std::string& name)
    {
        for(const TQuatAnimation& anim : mQuatAnims)
        {
            if (anim.getName() == name)
                return &anim;
        }
        return nullptr;
    }
	const TFloatAnimation* findFloatAnim(const std::string& name)
    {
        for(const TFloatAnimation& anim : mFloatAnims)
        {
            if (anim.getName() == name)
                return &anim;
        }
        return nullptr;
    }

	//
	// curve descriptors

	void	addCurveDesc(const std::string& name);
	/// Get curve count.
    int     getCurveCount() const { return (int)mCurveNames.size(); }
	/// Get curve's name by index.
	const std::string& getCurveName( int curveIdx ) const;
	/// Gets curve's index by it's name, or -1 if no such curve exists.
	int		getCurveIndexByName( const std::string& name ) const;

private:
	std::vector<TVector3Animation>	mVector3Anims;
    std::vector<TQuatAnimation>		mQuatAnims;
    std::vector<TFloatAnimation>	mFloatAnims;
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

