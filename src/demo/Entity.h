#pragma once

#include <src/math/Matrix4x4.h>
#include "Globals.h"

// --------------------------------------------------------------------------

class CCameraEntity {
public:
	CCameraEntity();

	void setProjectionParams( float fov, float aspect, float znear, float zfar );
	void setOrthoParams( float width, float height, float znear, float zfar );
	const SMatrix4x4& getProjectionMatrix() const { return mProjectionMatrix; }
	void setProjFrom( const CCameraEntity& c );

	float getZNear() const { return mZNear; }

	/**
	 *  Gets world space ray from camera's viewing plane coordinates.
	 *  @param x X coordinate (-1 at left, 1 at right).
	 *  @param y Y coordinate (-1 at top, 1 at bottom).
	 *  @return Ray in world space (NOT unit length).
	 */
	SVector3 getWorldRay( float x, float y ) const;
	/**
	 *  Gets camera space ray from camera's viewing plane coordinates.
	 *  @param x X coordinate (-1 at left, 1 at right).
	 *  @param y Y coordinate (-1 at top, 1 at bottom).
	 *  @return Ray in camera space (NOT unit length).
	 */
	SVector3 getCameraRay( float x, float y ) const;

	/**
	 *  Set current camera parameters onto rendering context's camera.
	 */
	void	setOntoRenderContext() const;

public:
    SMatrix4x4    mMatrix;

private:

	// projection params
	SMatrix4x4	mProjectionMatrix;
	bool	mOrtho;
	float	mFOV;
	float	mZNear;
	float	mZFar;
	float	mViewHalfWidth; // at distance 1
	float	mViewHalfHeight; // at distance 1
};
