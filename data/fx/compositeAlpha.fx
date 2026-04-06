#include "_lib.fx"
  
texture		tBase;
texture		tAlpha;
texture		tEdgeLUT;

// x=0.5/texwidth, y=0.5/texheight
float4	vFixUV;


// --------------------------------------------------------------------------
//  vertex shader

VS_PUV2 vsMain( VS_PUV i )
{
    VS_PUV2 o;
	o.pos = billboardpos( i.pos );
	o.uv[0] = o.uv[1] = i.uv + vFixUV.xy;
	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpAlpha = sampler_state {
    Texture   = (tAlpha);
    MipFilter = None;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};
sampler smpEdgeLUT = sampler_state {
    Texture   = (tEdgeLUT);
    MipFilter = Point;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};


float4 psMain11( VS_PUV2 i ) : COLOR
{
	float4 rgb = tex2D( smpBase, i.uv[0] );
	float4 alp = tex2D( smpAlpha, i.uv[1] );
	float4 edge = tex2D( smpEdgeLUT, float2(alp.a,alp.r) );
	rgb.rgb *= edge.rgb;
	rgb.a = edge.a;
	return rgb;
}

// --------------------------------------------------------------------------
//  effect

technique tec11 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain();
		PixelShader = compile ps_1_1 psMain11();

		ZEnable = False;
		ZWriteEnable = False;
		
		AlphaBlendEnable = True;
		SrcBlend = InvSrcAlpha;
		DestBlend = SrcAlpha;
	}

	pass PLast {
		ZEnable = True;
		ZWriteEnable = True;
		AlphaBlendEnable = False;
		Texture[0] = NULL;
		Texture[1] = NULL;
		Texture[2] = NULL;
	}
}
