#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdint.h>

#include "../../external/sokol_gfx.h"
#include "../gfx/Texture.h"
#include "../math/Maths.h"
#include "Pipelines.h"

enum render_pass_flags
{
    RPF_None = 0,
    RPF_LoadRenderTarget = (1 << 0),
    RPF_DoNotEndPass = (1 << 1),
};

void render_fullscreen_pass(
    Pipeline pipe, const char *debugLabel,
    sokol_texture &renderTarget,
    sg_view *inputTextures, size_t inputTexturesCount,
    sg_range uniforms,
    render_pass_flags flags = RPF_None);

// --------------------------------------------------------

struct LinePoint
{
    Vector3 pos;
    uint32_t color;
};

class LineRenderer
{
  public:
    LineRenderer();
    ~LineRenderer();

    void renderStrip(int npoints, const LinePoint *points, float halfWidth);

  private:
    struct TVertex
    {
        Vector3 p;
        uint32_t diffuse;
        float tu, tv;
    };
    sg_buffer mIB;
};

// --------------------------------------------------------

void gComputeTextureProjection(
    const Matrix4x4 &renderCameraMatrix,
    const Matrix4x4 &projectorMatrix,
    Matrix4x4 &dest);

void renderBloom();
void pingPongBlur(int passes);

// --------------------------------------------------------

struct Billboard
{
  public:
    void setWholeTexture()
    {
        tu1 = tv1 = 0;
        tu2 = tv2 = 1;
    }

  public:
    float x1, y1, x2, y2;
    float tu1, tv1, tu2, tv2;
    uint32_t color;
    sg_view texture;
};

Billboard &billboards_add();
void billboards_clear();
void billboards_render();

// --------------------------------------------------------

void dynamic_vb_init(size_t capacityBytes = 2 * 1024 * 1024);
void dynamic_vb_shutdown();
int dynamic_vb_append(const void *data, size_t size);
sg_buffer dynamic_vb_get();

// --------------------------------------------------------

// Multi-pass rendering of reflections causes garbage artifacts on
// the web, but only on Apple systems.
// Both in Chrome / macOS 15.7 and Safari / iOS 18.
// Just bias the reflections for now!
#if defined(__EMSCRIPTEN__)
constexpr bool kReflectionsDepthBias = true;
#else
constexpr bool kReflectionsDepthBias = false;
#endif

class LineRenderer;
extern LineRenderer *gLineRenderer;

extern sg_sampler s_smp_linear_repeat;
extern sg_sampler s_smp_linear_clamp;
extern sg_sampler s_smp_shadow;

// --------------------------------------------------------

constexpr int kMainAA = 4;

extern sokol_texture rt_main_aa, rt_main_z, rt_main_resolved;
extern sokol_texture rt_full_toon;
extern sokol_texture rt_4th_1, rt_4th_2;
extern sokol_texture rt_refl_px, rt_refl_py, rt_refl_pz, rt_refl_nx, rt_refl_ny, rt_refl_nz; // textures
extern sokol_texture rt_refl_rt, rt_refl_z;                                                  // rendertarget and z/stencil

const int SZ_SHADOWMAP = 1024;
extern sokol_texture rt_shadow_z;

// match global_uniforms in shaders
struct GlobalUniforms
{
    Matrix4x4 matView;
    Matrix4x4 matProjection;
    Matrix4x4 matViewProj;
    Matrix4x4 matViewTexProj;
    Matrix4x4 matShadowProj;
    Vector4 eyePos;
    Vector4 lightPos;
    Vector4 lightDir;
    Vector4 _pad0;
};

extern GlobalUniforms g_global_u;

// match entity_uniforms in shaders
struct EntityUniformsVS
{
    Matrix4x4 mat;
};
