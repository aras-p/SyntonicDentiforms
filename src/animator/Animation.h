#pragma once

#include "src/utils/noncopyable.h"


/**
 *  An animation sampling interface.
 *
 *  Can sample animation state at given time into user provided space.
 *
 *  Animation consists of animation curves (each having the same logical duration).
 *  Curves are primarily for efficient simultaneous animations (eg.
 *  character skeletons).
 *
 *  @param _V Value type.
 */
template<typename value_type>
class IAnimation : public noncopyable {
public:
	virtual ~IAnimation() { };

	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	virtual void sample( float time, int firstCurve, int numCurves, value_type* dest, int destStride = sizeof(value_type) ) const = 0;
};


