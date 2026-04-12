#include "Camera.h"
#include "Rendering.h"

RenderCamera gRenderCam;

CameraEntity::CameraEntity()
{
    mMatrix.identify();
}

void CameraEntity::setProjectionParams(float fov, float aspect, float znear, float zfar)
{
    mOrtho = false;
    mProjectionMatrix.perspectiveFovLH(fov, aspect, znear, zfar);
    mFOV = fov;
    mZNear = znear;
    mZFar = zfar;
    mViewHalfWidth = 1.0f / mProjectionMatrix._11;
    mViewHalfHeight = 1.0f / mProjectionMatrix._22;
}

void CameraEntity::setOrthoParams(float width, float height, float znear, float zfar)
{
    mOrtho = true;
    mProjectionMatrix.orthoLH(width, height, znear, zfar);
    mZNear = znear;
    mZFar = zfar;
    mViewHalfWidth = width * 0.5f;
    mViewHalfHeight = height * 0.5f;
}

void CameraEntity::setProjFrom(const CameraEntity &c)
{
    mOrtho = c.mOrtho;
    mFOV = c.mFOV;
    mZNear = c.mZNear;
    mZFar = c.mZFar;
    mViewHalfWidth = c.mViewHalfWidth;
    mViewHalfHeight = c.mViewHalfHeight;
    mProjectionMatrix = c.mProjectionMatrix;
}

void CameraEntity::setOntoRenderContext() const
{
    // set camera params
    gRenderCam.setCameraMatrix(mMatrix);
    gRenderCam.setProjectionMatrix(mProjectionMatrix);
}

Vector3 CameraEntity::getWorldRay(float x, float y) const
{
    Matrix4x4 m = mMatrix;
    m.getOrigin().set(0, 0, 0);
    Vector3 r = getCameraRay(x, y);
    r = r.transformCoord(m);
    return r;
}

Vector3 CameraEntity::getCameraRay(float x, float y) const
{
    return Vector3(x * mViewHalfWidth, -y * mViewHalfHeight, 1.0f);
}

RenderCamera::RenderCamera()
{
    mCameraMatrix.identify();
    mProjectionMatrix.identify();
    mCameraRotMatrix.identify();
    mViewMatrix.identify();
    mViewMatrix.identify();
}

void RenderCamera::setCameraMatrix(Matrix4x4 const &matrix)
{
    mCameraMatrix = matrix;
    mCameraRotMatrix = matrix;
    mCameraRotMatrix.getOrigin().set(0, 0, 0);

    mViewMatrix = mCameraMatrix;
    mViewMatrix.invert();
    mViewProjMatrix = mViewMatrix * mProjectionMatrix;

    g_global_u.matProjection = mProjectionMatrix;
    g_global_u.matView = mViewMatrix;
    g_global_u.matViewProj = mViewProjMatrix;
    g_global_u.eyePos = mCameraMatrix.getOrigin();
}

void RenderCamera::setProjectionMatrix(Matrix4x4 const &matrix)
{
    mProjectionMatrix = matrix;
    mViewProjMatrix = mViewMatrix * mProjectionMatrix;

    g_global_u.matProjection = mProjectionMatrix;
    g_global_u.matViewProj = mViewProjMatrix;
}
