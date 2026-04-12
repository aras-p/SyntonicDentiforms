#include "Rendering.h"

#include "Camera.h"
#include "DataFiles.h"
#include "Demo.h"
#include "Pipelines.h"

#include <assert.h>
#include <vector>

#include "../../external/sokol_app.h"
#include "../../external/sokol_glue.h"

LineRenderer::LineRenderer()
{
    constexpr int kSegments = 1000;
    constexpr int kQuadSize = 6;

    // index data
    uint16_t *ib = new uint16_t[kSegments * kQuadSize];
    {
        uint16_t *pib = ib;
        for (int i = 0; i < kSegments; ++i)
        {
            short base = i * 2;
            pib[0] = base;
            pib[1] = base + 2;
            pib[2] = base + 1;
            pib[3] = base + 2;
            pib[4] = base + 3;
            pib[5] = base + 1;
            pib += 6;
        }
    }

    // index buffer
    sg_buffer_desc desc = {};
    desc.usage.index_buffer = true;
    desc.data.ptr = ib;
    desc.data.size = kSegments * kQuadSize * 2;
    desc.usage.immutable = true;
    mIB = sg_make_buffer(&desc);
    assert(sg_query_buffer_state(mIB) == SG_RESOURCESTATE_VALID);

    delete[] ib;
}

LineRenderer::~LineRenderer()
{
    sg_destroy_buffer(mIB);
}

void LineRenderer::renderStrip(int npoints, const LinePoint *points, float halfWidth)
{
    if (npoints < 2)
        return;
    assert(points);

    const Matrix4x4 &camRotMat = gRenderCam.getCameraRotMatrix();
    const Matrix4x4 &camViewMat = gRenderCam.getViewMatrix();
    const Vector3 &camPos = gRenderCam.getEye3();

    int nverts = npoints * 2;
    int ntris = (npoints - 1) * 2;

    TVertex *chunk = new TVertex[nverts];
    TVertex *vb = chunk;

    // fill VB with lines
    for (int i = 1; i < npoints; ++i)
    {
        const LinePoint &ptA = points[i - 1];
        const LinePoint &ptB = points[i];
        Vector3 segDir = ptB.pos - ptA.pos;
        Vector3 side = segDir.cross(ptB.pos - camPos);
        side.normalize();
        side *= halfWidth;
        Vector3 c1 = side;
        Vector3 c2 = -side;

        // 2 or 4 line corners
        if (i == 1)
        {
            vb->p = ptA.pos + c1;
            vb->diffuse = ptA.color;
            vb->tu = 0.0f;
            vb->tv = 0.0f;
            ++vb;
            vb->p = ptA.pos + c2;
            vb->diffuse = ptA.color;
            vb->tu = 0.0f;
            vb->tv = 1.0f;
            ++vb;
        }
        vb->p = ptB.pos + c1;
        vb->diffuse = ptB.color;
        vb->tu = 1.0f;
        vb->tv = 0.0f;
        ++vb;
        vb->p = ptB.pos + c2;
        vb->diffuse = ptB.color;
        vb->tu = 1.0f;
        vb->tv = 1.0f;
        ++vb;
    }

    int offset = dynamic_vb_append(chunk, nverts * sizeof(TVertex));
    delete[] chunk;

    // render
    sg_apply_uniforms(0, {&g_global_u, sizeof(g_global_u)});

    sg_bindings binds = {};
    binds.vertex_buffers[0] = dynamic_vb_get();
    binds.vertex_buffer_offsets[0] = offset;
    binds.index_buffer = mIB;
    binds.views[0] = g_data_tex[DataTexLine1]->view_tex;
    binds.samplers[0] = s_smp_linear_repeat;
    sg_apply_bindings(binds);
    sg_draw(0, ntris * 3, 1);
}

void gComputeTextureProjection(const Matrix4x4 &renderCameraMatrix, const Matrix4x4 &projectorMatrix, Matrix4x4 &dest)
{
    // | -0.5     0        0        0 |
    // | 0        0.5      0        0 |
    // | 0        0        1        0 |
    // | 0.5      0.5      0        1 |
    Matrix4x4 matTexScale;
    matTexScale.identify();
    matTexScale._11 = 0.5f;
    matTexScale._22 = -0.5f;
    matTexScale.getOrigin().set(0.5f, 0.5f, 0.0f);

    dest = renderCameraMatrix * projectorMatrix * matTexScale;
}

void render_fullscreen_pass(
    Pipeline pipe, const char *debugLabel,
    sokol_texture &renderTarget,
    sg_view *inputTextures, size_t inputTexturesCount,
    sg_range uniforms,
    render_pass_flags flags)
{
    sg_pass pass = {};
    pass.label = debugLabel;
    if (renderTarget.view_rt.id != 0)
        pass.attachments.colors[0] = renderTarget.view_rt;
    else
        pass.swapchain = sglue_swapchain();
    pass.action.colors[0].store_action = SG_STOREACTION_STORE;
    pass.action.colors[0].load_action = (flags & RPF_LoadRenderTarget) ? SG_LOADACTION_LOAD : SG_LOADACTION_DONTCARE;
    pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
    sg_begin_pass(&pass);

    sg_bindings binds = {};
    for (size_t i = 0; i < inputTexturesCount; ++i)
        binds.views[i] = inputTextures[i];
    binds.samplers[0] = s_smp_linear_clamp;

    pipeline_apply(pipe);

    if (uniforms.ptr)
        sg_apply_uniforms(0, uniforms);

    sg_apply_bindings(binds);
    sg_draw(0, 3, 1);

    if (!(flags & RPF_DoNotEndPass))
        sg_end_pass();
}

const int BLOOM_PASSES = 4;

void pingPongBlur(int passes)
{
    sokol_texture *pingPong[2] = {&rt_4th_2, &rt_4th_1};

    int swidth = sapp_width() / 4;
    int sheight = sapp_height() / 4;

    const Vector4 offsetX(1, 1, -1, -1);
    const Vector4 offsetY(1, -1, -1, 1);
    for (int i = 0; i < passes; ++i)
    {
        const float pixDist = i + 0.5f;
        Vector4 offset(pixDist / swidth, pixDist / sheight, 0, 0);
        render_fullscreen_pass(pip_postBlurStep, "blur step", *pingPong[i & 1], &pingPong[(i + 1) & 1]->view_tex, 1, {&offset, sizeof(offset)});
    }
}

void renderBloom()
{
    // downsample main into smaller RT
    render_fullscreen_pass(pip_blit, "downsample", rt_4th_1, &rt_main_resolved.view_tex, 1, {});

    // blur
    pingPongBlur(BLOOM_PASSES);

    // add back into resolved
    render_fullscreen_pass(pip_postComposeBloom, "compose bloom", rt_main_resolved, !(BLOOM_PASSES & 1) ? &rt_4th_1.view_tex : &rt_4th_2.view_tex, 1, {}, RPF_LoadRenderTarget);

    // blit into swapchain backbuffer
    // do not end pass; debug text will continue rendering
    render_fullscreen_pass(pip_blitToSwap, "blit to swapchain", sokol_texture(), &rt_main_resolved.view_tex, 1, {}, RPF_DoNotEndPass);
}

struct TVertex
{
    Vector3 p;
    uint32_t diffuse;
    float tu, tv;
};

static std::vector<Billboard> s_bills;
static std::vector<TVertex> s_vertices;
static sg_buffer s_ib_quads;

Billboard &billboards_add()
{
    s_bills.emplace_back(Billboard());
    return s_bills.back();
}

void billboards_clear()
{
    s_bills.clear();
}

void billboards_render()
{
    if (s_ib_quads.id == 0)
    {
        uint16_t quadIBData[6] = {0, 1, 3, 1, 2, 3};
        sg_buffer_desc desc = {};
        desc.usage.index_buffer = true;
        desc.data.ptr = quadIBData;
        desc.data.size = 6 * 2;
        desc.usage.immutable = true;
        s_ib_quads = sg_make_buffer(&desc);
        assert(sg_query_buffer_state(s_ib_quads) == SG_RESOURCESTATE_VALID);
    }

    if (s_bills.empty())
        return;

    // construct vertices
    size_t vertsNeeded = s_bills.size() * 4;
    if (s_vertices.size() < vertsNeeded)
    {
        s_vertices.resize(vertsNeeded);
    }
    TVertex *vb = s_vertices.data();
    for (const Billboard &bill : s_bills)
    {
        const float z = 0.1f;
        vb->p.x = bill.x1;
        vb->p.y = bill.y1;
        vb->p.z = z;
        vb->diffuse = bill.color;
        vb->tu = bill.tu1;
        vb->tv = bill.tv1;
        ++vb;
        vb->p.x = bill.x2;
        vb->p.y = bill.y1;
        vb->p.z = z;
        vb->diffuse = bill.color;
        vb->tu = bill.tu2;
        vb->tv = bill.tv1;
        ++vb;
        vb->p.x = bill.x2;
        vb->p.y = bill.y2;
        vb->p.z = z;
        vb->diffuse = bill.color;
        vb->tu = bill.tu2;
        vb->tv = bill.tv2;
        ++vb;
        vb->p.x = bill.x1;
        vb->p.y = bill.y2;
        vb->p.z = z;
        vb->diffuse = bill.color;
        vb->tu = bill.tu1;
        vb->tv = bill.tv2;
        ++vb;
    }

    // fill int the GPU buffer
    int vbOffset = dynamic_vb_append(s_vertices.data(), vertsNeeded * sizeof(TVertex));

    // base bindings
    sg_bindings bind = {};
    bind.vertex_buffers[0] = dynamic_vb_get();
    bind.vertex_buffer_offsets[0] = vbOffset;
    bind.index_buffer = s_ib_quads;
    bind.samplers[0] = s_smp_linear_clamp;

    for (int i = 0; i < s_bills.size(); ++i)
    {
        const Billboard &b = s_bills[i];
        bind.views[0] = b.texture;
        bind.vertex_buffer_offsets[0] = vbOffset + i * sizeof(TVertex) * 4;
        sg_apply_bindings(&bind);
        sg_draw(0, 6, 1);
    }
}

static sg_buffer s_dynamic_buffer;

void dynamic_vb_init(size_t capacityBytes)
{
    sg_buffer_desc desc = {};
    desc.usage.vertex_buffer = true;
    desc.size = capacityBytes;
    desc.usage.immutable = false;
    desc.usage.dynamic_update = true;
    s_dynamic_buffer = sg_make_buffer(&desc);
    assert(sg_query_buffer_state(s_dynamic_buffer) == SG_RESOURCESTATE_VALID);
}

void dynamic_vb_shutdown()
{
    sg_destroy_buffer(s_dynamic_buffer);
    s_dynamic_buffer = {};
}

int dynamic_vb_append(const void *data, size_t size)
{
    int offset = sg_append_buffer(s_dynamic_buffer, {data, size});
    assert(!sg_query_buffer_overflow(s_dynamic_buffer));
    return offset;
}

sg_buffer dynamic_vb_get()
{
    return s_dynamic_buffer;
}
