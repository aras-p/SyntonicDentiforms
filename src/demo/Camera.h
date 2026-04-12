#pragma once

#include "../math/Maths.h"

class CCameraEntity
{
  public:
    CCameraEntity();

    void setProjectionParams(float fov, float aspect, float znear, float zfar);
    void setOrthoParams(float width, float height, float znear, float zfar);
    const SMatrix4x4 &getProjectionMatrix() const { return mProjectionMatrix; }
    void setProjFrom(const CCameraEntity &c);

    float getZNear() const { return mZNear; }

    /**
     *  Gets world space ray from camera's viewing plane coordinates.
     *  @param x X coordinate (-1 at left, 1 at right).
     *  @param y Y coordinate (-1 at top, 1 at bottom).
     *  @return Ray in world space (NOT unit length).
     */
    SVector3 getWorldRay(float x, float y) const;
    /**
     *  Gets camera space ray from camera's viewing plane coordinates.
     *  @param x X coordinate (-1 at left, 1 at right).
     *  @param y Y coordinate (-1 at top, 1 at bottom).
     *  @return Ray in camera space (NOT unit length).
     */
    SVector3 getCameraRay(float x, float y) const;

    /**
     *  Set current camera parameters onto rendering context's camera.
     */
    void setOntoRenderContext() const;

  public:
    SMatrix4x4 mMatrix;

  private:
    // projection params
    SMatrix4x4 mProjectionMatrix;
    bool mOrtho;
    float mFOV;
    float mZNear;
    float mZFar;
    float mViewHalfWidth;  // at distance 1
    float mViewHalfHeight; // at distance 1
};

class CRenderCamera
{
  public:
    CRenderCamera();

    void setCameraMatrix(const SMatrix4x4 &matrix);
    void setProjectionMatrix(const SMatrix4x4 &matrix);

    const SMatrix4x4 &getCameraMatrix() const { return mCameraMatrix; }
    const SMatrix4x4 &getCameraRotMatrix() const { return mCameraRotMatrix; }
    const SMatrix4x4 &getViewMatrix() const { return mViewMatrix; }
    const SMatrix4x4 &getViewProjMatrix() const { return mViewProjMatrix; }
    const SMatrix4x4 &getProjectionMatrix() const { return mProjectionMatrix; };
    const SVector3 &getEye3() const { return mCameraMatrix.getOrigin(); }

  private:
    SMatrix4x4 mCameraMatrix;
    SMatrix4x4 mProjectionMatrix;

    // calculated basing on camera matrix
    SMatrix4x4 mViewMatrix;
    SMatrix4x4 mViewProjMatrix;
    SMatrix4x4 mCameraRotMatrix;
};

extern CRenderCamera gRenderCam;
