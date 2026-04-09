#pragma once

#include "Vector3.h"
#include "Constants.h"
#include "Matrix4x4.h"
#include "Plane.h"


/**
 *  Axis aligned box.
 *
 *  Essentially two SVector3's - one for minimum, one for maximum.
 */
class CAABox {
public:
	/// Initializes to "none" AABB - minimum to big numbers, maximum to big negative numbers.
	CAABox()
	:	mMin(DINGUS_BIG_FLOAT,DINGUS_BIG_FLOAT,DINGUS_BIG_FLOAT),
		mMax(-DINGUS_BIG_FLOAT,-DINGUS_BIG_FLOAT,-DINGUS_BIG_FLOAT) { }

	/// Initializes to "none" AABB - minimum to big numbers, maximum to big negative numbers.
	void setNull() {
		mMin.x = mMin.y = mMin.z = DINGUS_BIG_FLOAT;
		mMax.x = mMax.y = mMax.z = -DINGUS_BIG_FLOAT;
	}

	/// Gets minimum vector.
	const SVector3& getMin() const { return mMin; }
	/// Gets minimum vector.
	SVector3& getMin() { return mMin; }
	/// Gets maximum vector.
	const SVector3& getMax() const { return mMax; }
	/// Gets maximum vector.
	SVector3& getMax() { return mMax; }

	/// Extends a box to include point
	void	extend( const SVector3& p ) {
		if( p.x < mMin.x ) mMin.x = p.x;
		if( p.y < mMin.y ) mMin.y = p.y;
		if( p.z < mMin.z ) mMin.z = p.z;
		if( p.x > mMax.x ) mMax.x = p.x;
		if( p.y > mMax.y ) mMax.y = p.y;
		if( p.z > mMax.z ) mMax.z = p.z;
	}
	/// Extends a box to include point and some more space
	void	extend( const SVector3& p, float more ) {
		if( p.x-more < mMin.x ) mMin.x = p.x-more;
		if( p.y-more < mMin.y ) mMin.y = p.y-more;
		if( p.z-more < mMin.z ) mMin.z = p.z-more;
		if( p.x+more > mMax.x ) mMax.x = p.x+more;
		if( p.y+more > mMax.y ) mMax.y = p.y+more;
		if( p.z+more > mMax.z ) mMax.z = p.z+more;
	}
	/// Extends a box to include other box
	void	extend( const CAABox& b ) {
		extend( b.getMin() );
		extend( b.getMax() );
	}

	/// Tests if a point is inside this box
	bool isInside( const SVector3& p ) const {
		return
			p.x>=mMin.x && p.x<=mMax.x &&
			p.y>=mMin.y && p.y<=mMax.y &&
			p.z>=mMin.z && p.z<=mMax.z;
	}

	// Culls this AABB, transformed by the given world matrix, against frustum planes
	// (extracted from view*projection matrix).
	bool frustumCull( const SMatrix4x4& world, const SPlane planes[6] ) const
	{
		// calculate OBB center in world space
		SVector3 center = (mMin + mMax) * 0.5f;
		SVector3 cw(
			center.x*world.m[0][0] + center.y*world.m[1][0] + center.z*world.m[2][0] + world.m[3][0],
			center.x*world.m[0][1] + center.y*world.m[1][1] + center.z*world.m[2][1] + world.m[3][1],
			center.x*world.m[0][2] + center.y*world.m[1][2] + center.z*world.m[2][2] + world.m[3][2]);
		// calculate OBB half extents
		SVector3 h = (mMax - mMin) * 0.5f;
		const SVector3& axX = world.getAxisX();
		const SVector3& axY = world.getAxisY();
		const SVector3& axZ = world.getAxisZ();

		for (int i = 0; i < 6; ++i)
		{
			const SPlane& p = planes[i];
			SVector3 n(p.a, p.b, p.c);
			float dc = n.dot(cw) + p.d;
			// OBB support radius along n
			float r = h.x*fabsf(n.dot(axX)) + h.y*fabsf(n.dot(axY)) + h.z*fabsf(n.dot(axZ));
			if (dc < -r)
				return true;
		}
		return false;
	}

private:
	SVector3	mMin;
	SVector3	mMax;
};


