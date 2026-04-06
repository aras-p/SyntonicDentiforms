#include "_lib.fx"

float4x4	mWVP;
float4		vShadowID;

// --------------------------------------------------------------------------
//  vertex shader

VS_P vsMain( VS_P i ) {
	VS_P o;
	o.pos = mul( i.pos, mWVP );
	return o;
}


// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader = NULL;

		TextureFactor = (vShadowID);
		
		ColorOp[0]	 = SelectArg1;
		ColorArg1[0] = TFactor;
		AlphaOp[0]	 = SelectArg1;
		AlphaArg1[0] = TFactor;
		
		ColorOp[1]	 = Disable;
		AlphaOp[1]	 = Disable;
	}
	pass PLast {
	}
}
