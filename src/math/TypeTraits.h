#pragma once

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Interpolation.h"

/*
	Math type traits. Each traits class, math_type_traits<T>, has methods:
	static T identity(); - returns identity.
	static void identify( T& ); - makes given value identity.
	static R interpolate( T& a1, T& a2, TT& t ); - lerps.
*/

template<typename _T>
struct math_type_traits_base {
	template<typename _R, typename _TT>
	static inline _R interpolate( _T const& a1, _T const& a2, _TT const& t ) {
		return math_lerp<_R>( a1, a2, t );
	}
};


// --------------------------------------------------------------------------
//  basic traits

template<typename _T>
struct math_type_traits : public math_type_traits_base<_T> {
};


// --------------------------------------------------------------------------
//  float traits

template<>
struct math_type_traits<float> : public math_type_traits_base<float> {
	static inline float identity() { return 0.0f; }
	static inline void identify( float& val ) { val = identity(); }

	template<typename _TT>
	static inline float interpolate( float const& a1, float const& a2, _TT const& t ) {
		return math_lerp<float>( a1, a2, t );
	};
};


// --------------------------------------------------------------------------
//  int traits

template<>
struct math_type_traits<int> : public math_type_traits_base<int> {
	static inline int identity() { return 0; }
	static inline void identify( int& val ) { val = identity(); }

	template<typename _TT> static inline float interpolate(int const& a1, int const& a2, _TT const& t ) {
		return math_lerp<float>( a1, a2, t );
	};
};


// --------------------------------------------------------------------------
//  SVector3 traits

template<>
struct math_type_traits<SVector3> : public math_type_traits_base<SVector3> {
	static inline SVector3 identity() { return SVector3(0,0,0); }
	static inline void identify( SVector3& val ) { val = identity(); }

	template<typename _TT>
	static inline SVector3 interpolate(SVector3 const& a1, SVector3 const& a2, _TT const& t ) {
		return math_lerp<SVector3>( a1, a2, t );
	};
};


// --------------------------------------------------------------------------
//  SQuaternion traits

template<>
struct math_type_traits<SQuaternion> : public math_type_traits_base<SQuaternion>  {
	static inline SQuaternion identity() { return SQuaternion(0,0,0,1); }
	static inline void identify( SQuaternion& val ) { val = identity(); }

	template<typename _TT>
	static inline SQuaternion interpolate( SQuaternion const& a1, SQuaternion const& a2, _TT const& t ) {
		SQuaternion o;
		o.slerp(a1, a2, t);
		return o;
	};
};
