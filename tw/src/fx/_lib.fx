
// Match GlobalUniforms in C++
layout(binding=0) uniform global_uniforms {
	mat4 mView;
	mat4 mProjection;
	mat4 mViewProj;
	mat4 mViewTexProj;
	mat4 mShadowProj;
	vec4 vEye;
	vec4 vFixUV; // x=0.5/texwidth, y=0.5/texheight, z=1-x*2, w=1-y*2
	vec4 vLightPos;
	vec4 vLightDir;
};

// Match EntityUniforms in C++
layout(binding=1) uniform entity_uniforms_vs {
	mat4 mWorld;
	mat4 mWorldView;
	mat4 mWVP;
};
layout(binding=2) uniform entity_uniforms_fs {
	vec4 vShadowID;
};

// --------------------------------------------------------------------------
// light

const float	fLightDiffuse = 0.5;
const float	fLightK = 16;


// --------------------------------------------------------------------------
// shadows

/* @TODO

shared texture	tShadow;	// shadow map
shared texture	tCookie;	// shadow cookie

sampler2D smpShadow = sampler_state {
    Texture   = <tShadow>;
    MipFilter = None; MinFilter = Point; MagFilter = Point;
    AddressU = Clamp; AddressV = Clamp;
};
sampler2D smpCookie = sampler_state {
    Texture   = <tCookie>;
    MipFilter = None; MinFilter = Linear; MagFilter = Linear;
    AddressU = Clamp; AddressV = Clamp;
};

*/


// --------------------------------------------------------------------------


// cull params: CW=2 (should be default), CCW=3 (for reflected, etc.)
//shared int		iCull = 2;


// --------------------------------------------------------------------------
//  common structs

/*
struct VS_P {
	vec4	pos		: POSITION;
};

struct VS_PN {
	vec4	pos		: POSITION;
	vec3	normal	: NORMAL;
};

struct VS_PUV {
	vec4 pos		: POSITION;
	vec2 uv		: TEXCOORD0;
};

struct VS_PUV2 {
	vec4 pos		: POSITION;
	vec2 uv[2]	: TEXCOORD0;
};

struct VS_PCUV {
	vec4 pos		: POSITION;
	vec4 color 	: COLOR;
	vec2 uv		: TEXCOORD0;
};

*/


// --------------------------------------------------------------------------
//  common lighting code

void gVSLightTerms(vec3 p, vec3 n, mat4 w, out vec3 on, out vec3 ol, out vec3 oh )
{
	vec3 pos = (w * vec4(p,1.0)).xyz;
	vec3 view = normalize( vEye.xyz - pos );
	vec3 light = normalize( vLightPos.xyz - pos );
	oh = normalize( (view+light)*0.5 );
	ol = light;
	on = mat3(w) * n;
}

vec4 gPSLight( vec3 n, vec3 l, vec3 h, float shadow )
{
	float diff = max(0.0, dot( n, l ) ) * fLightDiffuse;
	float spec = pow( max(0.0, dot( h, n )), fLightK );
	vec4 res;
	res.rgb = vec3(diff + spec);
	res.a = spec;
	return res * shadow;
}


// --------------------------------------------------------------------------
//  stuff

/*
vec4 billboardpos( vec4 p ) {
	return p * vec4( 2, 2, 1, 1 );
}

VS_PUV vsBillboard( VS_PUV i, uniform vec4 fixUV ) {
    VS_PUV o;
	o.pos = billboardpos( i.pos );
	o.uv = i.uv + fixUV.xy;
	return o;
}
*/