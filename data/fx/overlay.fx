#include "_lib.fx"
  
float4		vColor;

// x=0.5/texwidth, y=0.5/texheight
float4	vFixUV;


float4 psMain() : COLOR
{
	return vColor;
}

technique tec0 {
	pass P0 {
		VertexShader = compile vs_2_0 vsBillboard(vFixUV);
		PixelShader = compile ps_2_0 psMain();

		ZEnable = False;
		ZWriteEnable = False;
		
		AlphaBlendEnable = True;
		SrcBlend = One;
		DestBlend = One;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
		AlphaBlendEnable = False;
	}
}
