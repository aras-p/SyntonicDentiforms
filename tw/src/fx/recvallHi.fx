#include "_lib.fx"

mat4	mWorld;
mat4	mWorldView;
mat4	mWVP;

struct VS_OUTPUT20 {
	vec4	pos 	: POSITION;
	vec4	uvShadow	: TEXCOORD0;
	vec3	hlf			: TEXCOORD1;
	vec3	normal		: TEXCOORD2;
	vec3	tolight		: TEXCOORD3;
};

VS_OUTPUT20 vsMain20( VS_PN i ) {
	VS_OUTPUT20	o;

	o.pos = mul( i.pos, mWVP );
	vec4 uvs = mul( i.pos, mWorldView );
	uvs = mul( uvs, mShadowProj );
	o.uvShadow = uvs;

	gVSLightTerms( i.pos.xyz, i.normal, mWorld, o.normal, o.tolight, o.hlf );

	return o;
}

vec4 psMain20( VS_OUTPUT20 i ) : COLOR
{
	float shadow = ( tex2Dproj( smpShadow, i.uvShadow ).r > 0 ) ? 0 : 1;
	shadow *= tex2Dproj( smpCookie, i.uvShadow ).r;
	
	// lighting
	vec4 res = gPSLight( i.normal, i.tolight, normalize(i.hlf), shadow );
	return res;
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
