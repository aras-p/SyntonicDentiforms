#include "_lib.fx"
  
texture		tBase;

// x=0.5/texwidth, y=0.5/texheight, z=1-x*2, w=1-y*2
vec4	vFixUV;
vec4	vTexelsX;
vec4	vTexelsY;


struct VS_OUTPUT {
	vec4 pos		: POSITION;
	vec2 uv[4]	: TEXCOORD0;
};

VS_OUTPUT vsMain( VS_PUV i )
{
    VS_OUTPUT o;
	o.pos = billboardpos( i.pos );
	vec2 uv = i.uv + vFixUV.xy;
	for( int i = 0; i < 4; ++i )
		o.uv[i] = uv + vec2( vTexelsX[i], vTexelsY[i] );
	return o;
}

sampler smpBase = sampler_state {
    Texture   = (tBase);
    MipFilter = None;	MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};

vec4 psMain( VS_OUTPUT i ) : COLOR
{
	int t;
	float c = 0;
	for( t = 0; t < 4; ++t ) {
		float cc = tex2D( smpBase, i.uv[t] ).a;
		c += cc / 4;
	}
	return c * vec4(1,0.97,0.92,1); // a bit of sepia-ish tone
}


technique tec0 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader = compile ps_2_0 psMain();

		ZEnable = False;
		ZWriteEnable = False;
	}
	pass PLast {
		Texture[0] = NULL;
		Texture[1] = NULL;
		Texture[2] = NULL;
		Texture[3] = NULL;
		ZEnable = True;
		ZWriteEnable = True;
	}
}
