#pragma once

#include <assert.h>

inline float clamp( float value, float vmin = 0.0f, float vmax = 1.0f ) {
	assert( vmin <= vmax );
	if( value < vmin )
		value = vmin;
	else if( value > vmax )
		value = vmax;
	return value;
};
