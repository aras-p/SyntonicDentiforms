#pragma once

#include "AnimCurve.h"
#include "Animation.h"
#include "../math/TypeTraits.h"



/**
 *  A sampled animation.
 *
 *  Each curve consists of the same number of samples (but can be collapsed
 *  into single value).
 */
template<typename _V>
class CSampledAnimation : public IAnimation<_V> {
public:
	/**
	 *  Animation loop type.
	 *  CLAMP clamps animation to ends.
	 *  REPEAT loops animation (with interpolation from last sample to first).
	 *  REPEAT_LAST loops animation (no interpolation from last to first,
	 *   instead, the last sample is for transition from pre-last to last).
	 */
	enum eLoopType { CLAMP = 0, REPEAT, REPEAT_LAST };
	typedef CAnimCurve<value_type>	curve_type;

public:
	CSampledAnimation( int samplesInCurve, eLoopType loopType ) : mSamplesInCurve(samplesInCurve), mLoopType(loopType) { };

	void	addSample( const value_type& sample ) { mSamples.push_back(sample); }
	void	reserveSamples( int sampleCount ) { mSamples.reserve(sampleCount); }
	void	resizeSamples( int sampleCount ) { mSamples.resize(sampleCount); }
	int		getTotalSampleCount() const { return mSamples.size(); }
	const value_type& getSample( int index ) const;

	void	addCurve( const curve_type& curve ) { mCurves.push_back(curve); }
	void	reserveCurves( int curveCount ) { mCurves.reserve(curveCount); }
	int		getCurveCount() const { return mCurves.size(); }
	const curve_type& getCurve( int index ) const;

	int		getSamplesInCurve() const { return mSamplesInCurve; }

	void		setLoopType( eLoopType loopType ) { mLoopType = loopType; }
	eLoopType	getLoopType() const { return mLoopType; }

	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	void	timeToIndex( float time, int& index1, int& index2, float& alpha ) const;
	
	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	virtual void sample( float time, int firstCurve, int numCurves, value_type* dest, int destStride = sizeof(value_type) ) const;

private:
	typedef std::vector<value_type>	TSampleVector;
	typedef std::vector<curve_type> TCurveVector;

private:
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

template<typename _V>
inline const _V& CSampledAnimation<_V>::getSample( int index ) const
{
	assert( index>=0 && index < getTotalSampleCount() );
	return mSamples[index];
};

template<typename _V>
inline const CAnimCurve<_V>& CSampledAnimation<_V>::getCurve( int index ) const
{
	assert( index>=0 && index < getCurveCount() );
	return mCurves[index];
};

template<typename _V>
void CSampledAnimation<_V>::sample( float time, int firstCurve, int numCurves, _V* dest, int destStride ) const
{
	assert( firstCurve >= 0 && firstCurve < getCurveCount() );
	assert( numCurves > 0 && numCurves <= getCurveCount() );
	assert( firstCurve+numCurves <= getCurveCount() );
	assert( dest );
	assert( destStride >= sizeof(_V) );

	// get sample indices and alpha
	int sampleIdx1, sampleIdx2;
	float alpha;
	timeToIndex( time, sampleIdx1, sampleIdx2, alpha );

	for( int i = 0; i < numCurves; ++i ) {
		const curve_type& curve = getCurve( firstCurve + i );
		curve_type::eIpol ipol = curve.getIpol();

		switch( ipol ) {
		case curve_type::NONE:
			*dest = curve.getCollapsedValue();
			break;
		case curve_type::STEP:
			*dest = getSample( curve.getFirstSampleIndex() + sampleIdx1 );
			break;
		case curve_type::LINEAR:
			const value_type& s1 = getSample( curve.getFirstSampleIndex() + sampleIdx1 );
			const value_type& s2 = getSample( curve.getFirstSampleIndex() + sampleIdx2 );
			*dest = math_type_traits<value_type>::interpolate( s1, s2, alpha );
			break;
		};

		((const char*&)dest) += destStride;
	};
};


