// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __TIMED_ANIMATION_STREAM_H
#define __TIMED_ANIMATION_STREAM_H

#include "AnimStream.h"
#include "Animation.h"

namespace dingus {


// --------------------------------------------------------------------------

template<typename _T>
class CTimedAnimStream : public CAbstractTimedAnimStream<_T> {
public:
	typedef CTimedAnimStream<value_type> this_type;
	typedef IAnimation<value_type> animation_type;
	typedef intrusive_ptr<this_type> TSharedPtr;
public:
	CTimedAnimStream( animation_type& animation, float duration, bool repeating, int firstCurveIndex = 0, float startTime = anim_time() )
		: CAbstractTimedAnimStream<_T>(duration,repeating,firstCurveIndex,startTime), mAnimation(&animation) { }
	
	// IAnimStream
	virtual void update( int numCurves, value_type* dest, int destStride )
	{
		assert( mAnimation );
		float t = getRelTime();
		mAnimation->sample( t, getFirstCurveIndex(), numCurves, dest, destStride );
	}

private:
	animation_type*	mAnimation;
};


// --------------------------------------------------------------------------

typedef CTimedAnimStream<SVector3>		TVectorAnimStream;
typedef CTimedAnimStream<SQuaternion>	TQuatAnimStream;
typedef CTimedAnimStream<float>			TFloatAnimStream;


}; // namespace

#endif
