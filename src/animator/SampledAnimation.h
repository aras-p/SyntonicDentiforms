#pragma once

#include "AnimCurve.h"
#include "../math/TypeTraits.h"
#include <string>
#include <vector>
#include <assert.h>


/**
 *  A sampled animation.
 *
 *  Each curve consists of the same number of samples (but can be collapsed
 *  into single value).
 */
template<typename value_type>
class CSampledAnimation {
public:
	/**
	 *  Animation loop type.
	 *  CLAMP clamps animation to ends.
	 *  REPEAT loops animation (with interpolation from last sample to first).
	 *  REPEAT_LAST loops animation (no interpolation from last to first,
	 *   instead, the last sample is for transition from pre-last to last).
	 */
	enum eLoopType { CLAMP = 0, REPEAT, REPEAT_LAST };

public:
	CSampledAnimation() { };
    
    void init(const std::string& name, int samplesInCurve, eLoopType loopType)
    {
        mName = name;
        mSamplesInCurve = samplesInCurve;
        mLoopType = loopType;
    }
    const std::string& getName() const { return mName; }

	void	resizeSamples( int sampleCount ) { mSamples.resize(sampleCount); }
	const value_type& getSample( int index ) const
    {
        assert( index>=0 && index < mSamples.size() );
        return mSamples[index];
    }

	void	addCurve( const SAnimCurve& curve ) { mCurves.push_back(curve); }
	void	reserveCurves( int curveCount ) { mCurves.reserve(curveCount); }
	int		getCurveCount() const { return mCurves.size(); }
	const SAnimCurve& getCurve(int index) const
	{
		assert(index >= 0 && index < getCurveCount());
		return mCurves[index];
	}

	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	void	timeToIndex( float time, int& index1, int& index2, float& alpha ) const;
	
	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
    void sample( float time, int firstCurve, int numCurves, value_type* dest, int destStride = sizeof(value_type) ) const;

private:
	typedef std::vector<value_type>	TSampleVector;
	typedef std::vector<SAnimCurve> TCurveVector;

private:
    std::string     mName;
	TSampleVector	mSamples;
	TCurveVector	mCurves;
	int				mSamplesInCurve;
	eLoopType		mLoopType;
};


// --------------------------------------------------------------------------

template<typename _V>
inline void CSampledAnimation<_V>::timeToIndex( float time, int& index1, int& index2, float& alpha ) const
{
	int n = mSamplesInCurve;
	float frame = time * (mLoopType==REPEAT_LAST ? n-1 : n);
	index1 = int(frame);
	index2 = index1 + 1;
	alpha = frame - float(index1);
	
	switch( mLoopType ) {
	case CLAMP:
		if( index1 < 0 ) index1 = 0;
		else if( index1 >= n ) index1 = n-1;
		if( index2 < 0 ) index2 = 0;
		else if( index2 >= n ) index2 = n-1;
		break;
	case REPEAT:
		index1 %= n;
		index2 %= n;
		break;
	case REPEAT_LAST:
		index1 %= (n-1);
		index2 = index1 + 1;
	}
};

template<typename value_type>
void CSampledAnimation<value_type>::sample( float time, int firstCurve, int numCurves, value_type* dest, int destStride ) const
{
	assert( firstCurve >= 0 && firstCurve < getCurveCount() );
	assert( numCurves > 0 && numCurves <= getCurveCount() );
	assert( firstCurve+numCurves <= getCurveCount() );
	assert( dest );
	assert( destStride >= sizeof(value_type) );

	// get sample indices and alpha
	int sampleIdx1, sampleIdx2;
	float alpha;
	timeToIndex( time, sampleIdx1, sampleIdx2, alpha );

	for( int i = 0; i < numCurves; ++i ) {
		const SAnimCurve& curve = getCurve( firstCurve + i );
		switch(curve.ipol) {
		case SAnimCurve::NONE:
			*dest = *(const value_type*)&curve.collapsedValue;
			static_assert(sizeof(curve.collapsedValue) >= sizeof(value_type), "Too large value type");
			break;
		case SAnimCurve::STEP:
			*dest = getSample( curve.firstSampleIndex + sampleIdx1 );
			break;
		case SAnimCurve::LINEAR:
			const value_type& s1 = getSample( curve.firstSampleIndex + sampleIdx1 );
			const value_type& s2 = getSample( curve.firstSampleIndex + sampleIdx2 );
			*dest = math_type_traits<value_type>::interpolate( s1, s2, alpha );
			break;
		};

		((const char*&)dest) += destStride;
	};
};
