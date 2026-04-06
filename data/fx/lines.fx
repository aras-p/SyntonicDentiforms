#include "_lib.fx"

texture		tBase;

// --------------------------------------------------------------------------
//  vertex shader

VS_PCUV vsMain( VS_PCUV i ) {
	VS_PCUV o;
	o.pos = mul( i.pos, mViewProj );
	o.color = i.color;
	o.uv = i.uv;
	return o;
}

technique tec0 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader = NULL;
		Texture[0]   = (tBase);
		
		FVF = Xyz | Diffuse | Tex1;

		AlphaTestEnable = True;
		AlphaFunc = Greater;
		AlphaRef = 10;

		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		ZWriteEnable = False;

		ColorOp[0]	 = Modulate;
		ColorArg1[0] = Texture;
		ColorArg2[0] = Diffuse;
		AlphaOp[0]	 = Modulate;
		AlphaArg1[0] = Texture;
		AlphaArg2[0] = Diffuse;
		
		ColorOp[1]	 = Disable;
		AlphaOp[1]	 = Disable;
		CullMode = None;
	}
	
	pass PLast {
		AlphaBlendEnable = False;
		AlphaTestEnable = False;
		ZWriteEnable = True;
		CullMode = <iCull>;
	}
}
