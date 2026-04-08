#pragma once

#include "src/utils/noncopyable.h"

namespace dingus {


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
template<typename _V>
class IAnimation : public noncopyable {
public:
	typedef _V value_type;

public:
	virtual ~IAnimation() = 0 { };

	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	virtual void sample( float time, int firstCurve, int numCurves, value_type* dest, int destStride = sizeof(value_type) ) const = 0;
};


}; // namespace

