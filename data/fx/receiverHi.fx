#include "_lib.fx"

float4x4	mWorld;
float4x4	mWorldView;
float4x4	mWVP;

float4		vShadowID;


// --------------------------------------------------------------------------
//  2.0 - 4 sample robust objectid shadows

#ifdef ENABLE_PS20_PATH


struct VS_OUTPUT20 {
	float4	pos 	: POSITION;
	float3	uvShadow	: TEXCOORD0; // float looks better than half
	half3	hlf			: TEXCOORD1;
	half3	normal		: TEXCOORD2;
	half3	tolight		: TEXCOORD3;
};

VS_OUTPUT20 vsMain20( VS_PN i ) {
	VS_OUTPUT20	o;

	o.pos = mul( i.pos, mWVP );
	float4 uvs = mul( i.pos, mWorldView );
	uvs = mul( uvs, mShadowProj );
	o.uvShadow = uvs.xyz;

	gVSLightTerms( i.pos, i.normal, mWorld, o.normal, o.tolight, o.hlf );

	return o;
}

float4 psMain20( VS_OUTPUT20 i ) : COLOR
{
	const float offset = 1.0 / 1024.0;
	float2 uv = i.uvShadow.xy / i.uvShadow.z;
	float2 uv00 = uv + float2(      0,      0 );
	float2 uv01 = uv + float2(      0, offset );
	float2 uv10 = uv + float2( offset,      0 );
	float2 uv11 = uv + float2( offset, offset );
	float4 ids = float4(
		tex2D(smpShadow, uv00).r,
		tex2D(smpShadow, uv01).r,
		tex2D(smpShadow, uv10).r,
		tex2D(smpShadow, uv11).r );
	half4 compare = (ids == vShadowID.rrrr);
	
	// Robust ObjectID shadows (ShaderX^2)
	half shadow = any( compare );
	
	shadow *= tex2D( smpCookie, uv ).r;

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




#include "receiver-ps14path.fx"

#include "receiver-ps11path.fx"

