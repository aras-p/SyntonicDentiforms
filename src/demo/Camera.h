#pragma once

#include "../math/Maths.h"

class CameraEntity
{
  public:
    CameraEntity();

    void setProjectionParams(float fov, float aspect, float znear, float zfar);
    void setOrthoParams(float width, float height, float znear, float zfar);
    const Matrix4x4 &getProjectionMatrix() const { return mProjectionMatrix; }
    void setProjFrom(const CameraEntity &c);

    float getZNear() const { return mZNear; }

    /**
     *  Gets world space ray from camera's viewing plane coordinates.
     *  @param x X coordinate (-1 at left, 1 at right).
     *  @param y Y coordinate (-1 at top, 1 at bottom).
     *  @return Ray in world space (NOT unit length).
     */
    Vector3 getWorldRay(float x, float y) const;
    /**
     *  Gets camera space ray from camera's viewing plane coordinates.
     *  @param x X coordinate (-1 at left, 1 at right).
     *  @param y Y coordinate (-1 at top, 1 at bottom).
     *  @return Ray in camera space (NOT unit length).
     */
    Vector3 getCameraRay(float x, float y) const;

    /**
     *  Set current camera parameters onto rendering context's camera.
     */
    void setOntoRenderContext() const;

  public:
    Matrix4x4 mMatrix;

  private:
    // projection params
    Matrix4x4 mProjectionMatrix;
    bool mOrtho;
    float mFOV;
    float mZNear;
    float mZFar;
    float mViewHalfWidth;  // at distance 1
    float mViewHalfHeight; // at distance 1
};

class RenderCamera
{
  public:
    RenderCamera();

    void setCameraMatrix(const Matrix4x4 &matrix);
    void setProjectionMatrix(const Matrix4x4 &matrix);

    const Matrix4x4 &getCameraMatrix() const { return mCameraMatrix; }
    const Matrix4x4 &getCameraRotMatrix() const { return mCameraRotMatrix; }
    const Matrix4x4 &getViewMatrix() const { return mViewMatrix; }
    const Matrix4x4 &getViewProjMatrix() const { return mViewProjMatrix; }
    const Matrix4x4 &getProjectionMatrix() const { return mProjectionMatrix; };
    const Vector3 &getEye3() const { return mCameraMatrix.getOrigin(); }

  private:
    Matrix4x4 mCameraMatrix;
    Matrix4x4 mProjectionMatrix;

    // calculated basing on camera matrix
    Matrix4x4 mViewMatrix;
    Matrix4x4 mViewProjMatrix;
    Matrix4x4 mCameraRotMatrix;
};

extern RenderCamera gRenderCam;
