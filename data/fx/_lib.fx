#ifndef __LIB_FX
#define __LIB_FX

#include "_shared.fx"

// --------------------------------------------------------------------------
//  common structs

struct VS_P {
	float4	pos		: POSITION;
};

struct VS_PN {
	float4	pos		: POSITION;
	float3	normal	: NORMAL;
};

struct VS_PUV {
	float4 pos		: POSITION;
	float2 uv		: TEXCOORD0;
};

struct VS_PUV2 {
	float4 pos		: POSITION;
	float2 uv[2]	: TEXCOORD0;
};

struct VS_PCUV {
	float4 pos		: POSITION;
	float4 color 	: COLOR;
	float2 uv		: TEXCOORD0;
};


// --------------------------------------------------------------------------
//  newton-rhapson normalization

// v is -1..1
inline half3 nrnorm( half3 v ) {
	float t = dot( v, v );
	return v * (0.5*(1-t)) + v;
}

// v is 0..1
inline half3 nrnorm01( half3 v ) {
	float t = dot( v*2-1, v*2-1 );
	return (v-0.5) * (1-t) + (v*2-1);
}


// --------------------------------------------------------------------------
//  common lighting code

inline void gVSLightTerms( float4 p, float3 n, float4x4 w,
	out float3 on, out float3 ol, out float3 oh )
{
	float3 pos = mul( p, (float4x3)w );
	float3 view = normalize( vEye - pos );
	float3 light = normalize( vLightPos - pos );
	oh = normalize( (view+light)*0.5 );
	ol = light;
	on = mul( n, (float3x3)w );
}

inline half4 gPSLight( half3 n, half3 l, half3 h, half shadow )
{
	half diff = saturate( dot( n, l ) ) * fLightDiffuse;
	half spec = pow( dot( h, n ), fLightK );
	half4 res;
	res.rgb = diff + spec;
	res.a = spec;
	return res * shadow;
}


// --------------------------------------------------------------------------
//  stuff

inline float4 billboardpos( float4 p ) {
	return p * float4( 2, 2, 1, 1 );
}

VS_PUV vsBillboard( VS_PUV i, uniform float4 fixUV ) {
    VS_PUV o;
	o.pos = billboardpos( i.pos );
	o.uv = i.uv + fixUV.xy;
	return o;
}


#endif
