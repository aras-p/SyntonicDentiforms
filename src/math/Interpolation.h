#pragma once


/**
 *  Linear interpolates between 2 values.
 *  @param _R Result type
 *  @param _A Value type
 *  @param _T Time type
 */
template<typename _R, typename _A, typename _T>
inline _R const math_lerp( _A const& a1, _A const& a2, _T const& t )
{
	return _R( a1 + (a2-a1)*t );
};



