#include "Camera.h"
#include "Rendering.h"

CRenderCamera gRenderCam;

CCameraEntity::CCameraEntity()
{
    mMatrix.identify();
}

void CCameraEntity::setProjectionParams(float fov, float aspect, float znear, float zfar)
{
    mOrtho = false;
    mProjectionMatrix.perspectiveFovLH(fov, aspect, znear, zfar);
    mFOV = fov;
    mZNear = znear;
    mZFar = zfar;
    mViewHalfWidth = 1.0f / mProjectionMatrix._11;
    mViewHalfHeight = 1.0f / mProjectionMatrix._22;
}

void CCameraEntity::setOrthoParams(float width, float height, float znear, float zfar)
{
    mOrtho = true;
    mProjectionMatrix.orthoLH(width, height, znear, zfar);
    mZNear = znear;
    mZFar = zfar;
    mViewHalfWidth = width * 0.5f;
    mViewHalfHeight = height * 0.5f;
}

void CCameraEntity::setProjFrom(const CCameraEntity &c)
{
    mOrtho = c.mOrtho;
    mFOV = c.mFOV;
    mZNear = c.mZNear;
    mZFar = c.mZFar;
    mViewHalfWidth = c.mViewHalfWidth;
    mViewHalfHeight = c.mViewHalfHeight;
    mProjectionMatrix = c.mProjectionMatrix;
}

void CCameraEntity::setOntoRenderContext() const
{
    // set camera params
    gRenderCam.setCameraMatrix(mMatrix);
    gRenderCam.setProjectionMatrix(mProjectionMatrix);
}

SVector3 CCameraEntity::getWorldRay(float x, float y) const
{
    SMatrix4x4 m = mMatrix;
    m.getOrigin().set(0, 0, 0);
    SVector3 r = getCameraRay(x, y);
    r = r.transformCoord(m);
    return r;
}

SVector3 CCameraEntity::getCameraRay(float x, float y) const
{
    return SVector3(x * mViewHalfWidth, -y * mViewHalfHeight, 1.0f);
}

CRenderCamera::CRenderCamera()
{
    mCameraMatrix.identify();
    mProjectionMatrix.identify();
    mCameraRotMatrix.identify();
    mViewMatrix.identify();
    mViewMatrix.identify();
}

void CRenderCamera::setCameraMatrix(SMatrix4x4 const &matrix)
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

void CRenderCamera::setProjectionMatrix(SMatrix4x4 const &matrix)
{
    mProjectionMatrix = matrix;
    mViewProjMatrix = mViewMatrix * mProjectionMatrix;

    g_global_u.matProjection = mProjectionMatrix;
    g_global_u.matViewProj = mViewProjMatrix;
}
