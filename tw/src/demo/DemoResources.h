#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <dingus/math/Vector4.h>
#include <dingus/math/Matrix4x4.h>
#include <dingus/gfx/RenderableOrderedBillboards.h>
#include <dingus/gfx/Texture.h>

// --------------------------------------------------------------------------
// misc

class CLineRenderer;
extern CLineRenderer*	gLineRenderer;

extern CRenderableOrderedBillboards*	gBillboardsNormal;
extern CRenderableOrderedBillboards*	gBillboardsNoDestA;
extern SVector4			gScreenFixUVs;

extern sg_sampler s_smp_linear_repeat;
extern sg_sampler s_smp_linear_clamp;
extern sg_sampler s_smp_point_clamp;


// --------------------------------------------------------------------------
// main rendertargets

constexpr int kMainAA = 4;

extern sokol_texture rt_main_aa, rt_main_z, rt_main_resolved;

// full screen
extern sokol_texture rt_full_toon;

// 1/4 of the screen
extern sokol_texture rt_4th_1, rt_4th_2;


// --------------------------------------------------------------------------
// cube reflections

// textures
extern sokol_texture rt_refl_px, rt_refl_py, rt_refl_pz, rt_refl_nx, rt_refl_ny, rt_refl_nz;
// rendertarget and z/stencil
extern sokol_texture rt_refl_rt, rt_refl_z;

// --------------------------------------------------------------------------
// shadow maps

const int SZ_SHADOWMAP = 1024;

extern sokol_texture rt_shadow_rt;
extern sokol_texture rt_shadow_z;

// match global_uniforms in shaders
struct GlobalUniforms
{
	SMatrix4x4 matView;
	SMatrix4x4 matProjection;
	SMatrix4x4 matViewProj;
	SMatrix4x4 matViewTexProj;
	SMatrix4x4 matShadowProj;
	SVector4 eyePos;
	SVector4 lightPos;
	SVector4 lightDir;
	SVector4 _pad0;
};

extern GlobalUniforms g_global_u;

// match entity_uniforms in shaders
struct EntityUniformsVS
{
	SMatrix4x4	mat;
	SMatrix4x4	matWV;
	SMatrix4x4	matWVP;
};
struct EntityUniformsFS
{
	SVector4 shadowID; // zero shadow ID: receive all shadows
};

