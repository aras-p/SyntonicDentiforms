#include "_lib.fx"
  
texture		tBase;

// x=0.5/texwidth, y=0.5/texheight
float4	vFixUV;


technique tec0 {
	pass P0 {
		VertexShader = compile vs_1_1 vsBillboard(vFixUV);
		PixelShader = NULL;

		ZEnable = False;
		ZWriteEnable = False;
		
		AlphaBlendEnable = True;
		SrcBlend = One;
		DestBlend = One;

		Texture[0] = (tBase);

		ColorOp[0]	 = SelectArg1;
		ColorArg1[0] = Texture;
		AlphaOp[0]	 = SelectArg1;
		AlphaArg1[0] = Texture;

		ColorOp[1]	 = Disable;
		AlphaOp[1]	 = Disable;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
		AlphaBlendEnable = False;
		Texture[0] = NULL;
		Texture[1] = NULL;
	}
}
