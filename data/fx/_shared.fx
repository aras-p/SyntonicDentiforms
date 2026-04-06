#ifndef __SHARED_FX
#define __SHARED_FX

// various global (not per-object) parameters are here

// --------------------------------------------------------------------------
// camera

shared float4x4	mView;
shared float4x4 mProjection;
shared float4x4 mViewProj;
shared float3	vEye;

// --------------------------------------------------------------------------
// light

shared float3	vLightPos;
shared float3	vLightDir;
static float	fLightDiffuse = 0.5;
static float	fLightK = 16;


// --------------------------------------------------------------------------
// shadows

shared float4x4 mShadowProj;
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


// --------------------------------------------------------------------------


// cull params: CW=2 (should be default), CCW=3 (for reflected, etc.)
shared int		iCull = 2;


// others

shared texture tNormalizer;
samplerCUBE smpNormalizer = sampler_state {
    Texture   = <tNormalizer>;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    //AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
};

shared float4x4	mViewTexProj;

shared static float4x4 mIdentity = float4x4(
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1 );
shared static float4x4 mSphereMap = float4x4(
	0.4,0,  0, 0,
	0, -0.4,0, 0,
	0,  0.0,0, 0,
	0.5,0.5,0, 0 );


#endif
