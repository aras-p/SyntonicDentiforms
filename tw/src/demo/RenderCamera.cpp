// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "RenderCamera.h"

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
}

void CRenderCamera::setProjectionMatrix( SMatrix4x4 const& matrix )
{
	mProjectionMatrix = matrix;
	mViewProjMatrix = mViewMatrix * mProjectionMatrix;
}
