#include "_lib.fx"
  
texture		tBase;

// --------------------------------------------------------------------------
//  vertex shader

VS_PCUV vsMain( VS_PCUV i )
{
    VS_PCUV o;
	o.pos = i.pos;
	o.pos.y = -o.pos.y;
	o.color = i.color;
	o.uv = i.uv;
	return o;
}

// --------------------------------------------------------------------------
//  pixel shader

sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};

vec4 psMain( VS_PCUV i ) : COLOR
{
	vec4 c = tex2D( smpBase, i.uv );
	c *= i.color;
	c.a = c.a;
	return c;
}


// --------------------------------------------------------------------------
//  effect

technique tec0 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader = compile ps_2_0 psMain();

		ZEnable = False;
		ZWriteEnable = False;
		CullMode = None;
		
		AlphaBlendEnable = True;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}

	pass PLast {
		CullMode = <iCull>;
		ZEnable = True;
		ZWriteEnable = True;
		AlphaBlendEnable = False;
	}
}
