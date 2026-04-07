#include "_lib.fx"

mat4	mWorld;
mat4	mWorldView;
mat4	mWVP;


// --------------------------------------------------------------------------
//  2.0

struct VS_OUTPUT20 {
	vec4	pos 	: POSITION;
	vec3	hlf			: TEXCOORD0;
	vec3	normal		: TEXCOORD1;
	vec3	tolight		: TEXCOORD2;
};

VS_OUTPUT20 vsMain20( VS_PN i ) {
	VS_OUTPUT20	o;
	o.pos = mul( i.pos, mWVP );
	gVSLightTerms( i.pos.xyz, i.normal, mWorld, o.normal, o.tolight, o.hlf );
	return o;
}

vec4 psMain20( VS_OUTPUT20 i ) : COLOR
{
	return gPSLight( i.normal, i.tolight, normalize(i.hlf), 1.0 );
}

technique tecPS20 {
	pass P0 {
		VertexShader = compile vs_2_0 vsMain20();
		PixelShader = compile ps_2_0 psMain20();
	}
	pass PLast {
		Texture[0] = NULL;
		Texture[1] = NULL;
	}
}
