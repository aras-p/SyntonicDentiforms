#include "_lib.fx"

float4x4	mWorld;
float4x4	mWorldView;
float4x4	mWVP;

// --------------------------------------------------------------------------
//  2.0

#ifdef ENABLE_PS20_PATH


struct VS_OUTPUT20 {
	float4	pos 	: POSITION;
	half4	uvShadow	: TEXCOORD0;
	half3	hlf			: TEXCOORD1;
	half3	normal		: TEXCOORD2;
	half3	tolight		: TEXCOORD3;
};

VS_OUTPUT20 vsMain20( VS_PN i ) {
	VS_OUTPUT20	o;

	o.pos = mul( i.pos, mWVP );
	float4 uvs = mul( i.pos, mWorldView );
	uvs = mul( uvs, mShadowProj );
	o.uvShadow = uvs;

	gVSLightTerms( i.pos, i.normal, mWorld, o.normal, o.tolight, o.hlf );

	return o;
}

float4 psMain20( VS_OUTPUT20 i ) : COLOR
{
	half shadow = ( tex2Dproj( smpShadow, i.uvShadow ).r > 0 ) ? 0 : 1;
	shadow *= tex2Dproj( smpCookie, i.uvShadow ).r;
	
	// lighting
	half4 res = gPSLight( i.normal, i.tolight, nrnorm(i.hlf), shadow );
	return res;
}

technique tecPS20 {
	pass P0 {
		VertexShader = compile vs_1_1 vsMain20();
		PixelShader = compile ps_2_0 psMain20();
	}
	pass PLast {
		Texture[0] = NULL;
		Texture[1] = NULL;
	}
}

#endif


#include "recvall-ps14path.fx"
#include "recvall-ps11path.fx"
