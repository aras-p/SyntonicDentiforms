#include "_lib.fx"
  
texture		tBase;
texture		tLUT;

// x=0.5/texwidth, y=0.5/texheight
float4	vFixUV;



sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpLUT = sampler_state {
    Texture   = (tLUT);
    MipFilter = Point;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};

struct VS_OUTPUT {
	float4 pos	: POSITION;
	float2 uv	: TEXCOORD0;
	float2 uvE[4] : TEXCOORD1;
};


VS_OUTPUT vsMain( VS_PUV i )
{
    VS_OUTPUT o;
	o.pos = billboardpos( i.pos );
	float2 uv = i.uv + vFixUV.xy;
	o.uv = uv;

	float dx = vFixUV.x*2;
	float dy = vFixUV.y*2;
	o.uvE[0] = uv + float2(0,-dy);
	o.uvE[1] = uv + float2(0, dy);
	o.uvE[2] = uv + float2(-dx,0);
	o.uvE[3] = uv + float2( dx,0);
	return o;
}

half4 psMain20( VS_OUTPUT i ) : COLOR
{
	half4 c = tex2D( smpBase, i.uv );
	half4 clut = tex2D( smpLUT, c.ar );
	clut.a = 0;

	half cU = tex2D( smpBase, i.uvE[0] ).r;
	half cD = tex2D( smpBase, i.uvE[1] ).r;
	half cL = tex2D( smpBase, i.uvE[2] ).r;
	half cR = tex2D( smpBase, i.uvE[3] ).r;
	half dUD = (cU - cD)*4;
	half dLR = (cL - cR)*4;
	dUD = dUD * dUD;
	dLR = dLR * dLR;
	half edge = saturate( 1 - dUD - dLR );

	return clut * edge;
}

technique tec20 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader = compile ps_2_0 psMain20();

		ZEnable = False;
		ZWriteEnable = False;
	}
	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
		Texture[0] = NULL;
	}
}
