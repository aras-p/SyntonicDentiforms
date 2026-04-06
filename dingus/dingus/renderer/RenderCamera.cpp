// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "../stdafx.h"
#pragma hdrstop

#include "RenderCamera.h"

using namespace dingus;


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
	D3DXMatrixInverse( &mViewMatrix, NULL, &mCameraMatrix );
	mViewProjMatrix = mViewMatrix * mProjectionMatrix;
}

void CRenderCamera::setProjectionMatrix( SMatrix4x4 const& matrix )
{
	mProjectionMatrix = matrix;
	mViewProjMatrix = mViewMatrix * mProjectionMatrix;
}
