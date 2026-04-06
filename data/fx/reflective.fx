#include "_lib.fx"

float4x4	mWorld;
float4x4	mWorldView;
float4x4	mWVP;

texture		tRefl;


// --------------------------------------------------------------------------
//  1.1

sampler2D smpRefl = sampler_state {
    Texture   = <tRefl>;
    MipFilter = None; MinFilter = Linear; MagFilter = Linear;
    //MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp;
};

struct VS_OUTPUT {
	float4	pos 	: POSITION;
	float4	uvRefl	: TEXCOORD0;
};

VS_OUTPUT vsMain( VS_P i ) {
	VS_OUTPUT	o;

	o.pos = mul( i.pos, mWVP );
	float4 uv = mul( i.pos, mWorldView );
	uv = mul( uv, mViewTexProj );
	o.uvRefl = uv;

	return o;
}

float4 psMain( VS_OUTPUT i ) : COLOR
{
	float4 crefl = tex2D( smpRefl, i.uvRefl );
	crefl.a = 0.25;
	return crefl;
}


technique tecPS11 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_1_1 psMain();

		TextureTransformFlags[0] = Projected;

		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		ZWriteEnable = False;
	}
	pass PLast {
		TextureTransformFlags[0] = Disable;

		Texture[0] = NULL;

		AlphaBlendEnable = False;
		ZWriteEnable = True;
	}
}
