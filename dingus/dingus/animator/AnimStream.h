// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __ANIMATION_STREAM_H
#define __ANIMATION_STREAM_H

#include "AnimTypeTraits.h"
#include "AnimTime.h"


namespace dingus {


// --------------------------------------------------------------------------

/**
 *  Animation stream interface.
 *
 *  Can be paused, unpaused and updated. Can return data type. Is ref counted.
 *
 *  @param _V Value type.
 */
template<typename _V>
class IAnimStream : public CRefCounted {
public:
	typedef _V value_type;
	typedef IAnimStream<_V> this_type;
	typedef intrusive_ptr<this_type> TSharedPtr;
public:
	virtual ~IAnimStream() = 0 { }

	virtual void pause() = 0;
	virtual void unpause() = 0;
	/**
	 *  Update animation stream, write current values into user provided space.
	 *  @param numCurves Number of animation values to sample.
	 *  @param dest Start of destination space.
	 *  @param destStride Stride between adjacent values in bytes.
	 */
	virtual void update( int numCurves, value_type* dest, int destStride ) = 0;

	static eAnimType getType() {
		return static_cast<eAnimType>( traits::anim_type<value_type>::value );
	};
};

// --------------------------------------------------------------------------

template<typename _V>
class CAbstractTimedAnimStream : public IAnimStream<_V> {
public:
	CAbstractTimedAnimStream( float duration, bool repeating, int firstCurveIndex = 0, float startTime = anim_time() );

	virtual void pause();
	virtual void unpause();

	float	getDuration() const { return mDuration; }
	float	getStartTime() const { return mStartTime; }
	bool	isRepeating() const { return mRepeating; }
	bool	isFinished() const { return (!mRepeating) && (anim_time()-mStartTime > mDuration); }
	float	getRelTime() const { return (anim_time()-mStartTime) * mOneOverDuration; }
	float	getOneOverDuration() const { return mOneOverDuration; }

	int		getFirstCurveIndex() const { return mFirstCurveIndex; }

	void	setDuration( float d ) { mDuration = d; mOneOverDuration = 1.0f / d; }

protected:
	bool isPaused() const { return mPaused; }

private:
	float	mDuration;
	float	mOneOverDuration;
	float	mStartTime;
	float	mPausedTime;
	bool	mRepeating;
	bool	mPaused;
	int		mFirstCurveIndex;
};

// --------------------------------------------------------------------------

template<typename _V>
CAbstractTimedAnimStream<_V>::CAbstractTimedAnimStream( float duration, bool repeating, int firstCurveIndex, float startTime )
:	mDuration(duration),
	mOneOverDuration(1.0f/duration),
	mStartTime(startTime),
	mPausedTime(startTime),
	mRepeating(repeating),
	mPaused( false ),
	mFirstCurveIndex( firstCurveIndex )
{
};

template<typename _V>
void CAbstractTimedAnimStream<_V>::pause()
{
	if( mPaused )
		return;
	mPaused = true;
	mPausedTime = anim_time();
};

template<typename _V>
void CAbstractTimedAnimStream<_V>::unpause()
{
	if( !mPaused )
		return;
	mPaused = false;
	mStartTime += anim_time() - mPausedTime; // continue where we paused
};


}; // namespace


#endif
