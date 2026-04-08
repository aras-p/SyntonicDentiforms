#pragma once

#include <src/math/Matrix4x4.h>

/**
 *  Computes texture projection matrix.
 *  @param renderCameraMatrix Rendering camera's matrix.
 *  @param projectorMatrix Projector's view*projection matrix.
 *  @param dest Destination matrix.
 */
void gComputeTextureProjection(
		const dingus::SMatrix4x4& renderCameraMatrix,
		const dingus::SMatrix4x4& projectorMatrix,
		dingus::SMatrix4x4& dest );


