#include "RenderCamera.h"
#include "DemoResources.h"

using namespace dingus;

CRenderCamera gRenderCam;

CRenderCamera::CRenderCamera()
{
	mCameraMatrix.identify();
	mProjectionMatrix.identify();
	mCameraRotMatrix.identify();
	mViewMatrix.identify();
	mViewMatrix.identify();
}


void CRenderCamera::setCameraMatrix( SMatrix4x4 const& matrix )
{
	mCameraMatrix = matrix;
	mCameraRotMatrix = matrix;
	mCameraRotMatrix.getOrigin().set(0,0,0);

	mViewMatrix = mCameraMatrix;
	mViewMatrix.invert();
	mViewProjMatrix = mViewMatrix * mProjectionMatrix;

	g_global_u.matProjection = mProjectionMatrix;
	g_global_u.matView = mViewMatrix;
	g_global_u.matViewProj = mViewProjMatrix;
	g_global_u.eyePos = mCameraMatrix.getOrigin();
}

void CRenderCamera::setProjectionMatrix( SMatrix4x4 const& matrix )
{
	mProjectionMatrix = matrix;
	mViewProjMatrix = mViewMatrix * mProjectionMatrix;

	g_global_u.matProjection = mProjectionMatrix;
	g_global_u.matViewProj = mViewProjMatrix;
}
