#include "TextureProjector.h"

void gComputeTextureProjection( const SMatrix4x4& renderCameraMatrix, const SMatrix4x4& projectorMatrix, SMatrix4x4& dest )
{
    // | -0.5     0        0        0 |
    // | 0        0.5      0        0 |
    // | 0        0        1        0 |
    // | 0.5      0.5      0        1 |
	SMatrix4x4 matTexScale;
	matTexScale.identify();
	matTexScale._11 = 0.5f;
	matTexScale._22 = -0.5f;
	matTexScale.getOrigin().set( 0.5f, 0.5f, 0.0f );

	dest = renderCameraMatrix * projectorMatrix * matTexScale;
}
