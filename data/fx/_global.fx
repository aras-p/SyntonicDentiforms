// global effect as used by the framework

#include "_shared.fx"

technique tec0 {
	pass P0 {
		ViewTransform = (mView);
		ProjectionTransform = (mProjection);

		Lighting = False;

		AlphaBlendEnable = False;

		CullMode = <iCull>;

		ZFunc = LessEqual;

		// Texture params
		MinFilter[0] = Linear;
		MagFilter[0] = Linear;
		MipFilter[0] = Linear;
		MinFilter[1] = Linear;
		MagFilter[1] = Linear;
		MipFilter[1] = Linear;
	}
}
