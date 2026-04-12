#include "Pipelines.h"
#include "DemoResources.h"
#include <assert.h>

#include "billboards.glsl.h"
#include "blit.glsl.h"
#include "shadowCaster.glsl.h"
#include "postComposeToon.glsl.h"
#include "postBlurStep.glsl.h"
#include "postToon.glsl.h"
#include "lines.glsl.h"
#include "renderLit.glsl.h"
#include "overlay.glsl.h"
#include "renderLitShadowed.glsl.h"
#include "renderReflective.glsl.h"
#include "renderWhite.glsl.h"

#include "../../external/sokol_gfx.h"

static sg_pipeline s_fx_pipes[pipCount] = {};

static void pipe_full_tri(sg_pipeline_desc& desc)
{
	desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
	desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
	desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
	desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
	desc.depth.write_enabled = false;
	desc.cull_mode = SG_CULLMODE_NONE;
}

static void pipe_blend_add(sg_pipeline_desc& desc)
{
	desc.colors[0].blend.enabled = true;
	desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_ONE;
	desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
	desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE;
	desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
}

static void pipe_blend_alpha(sg_pipeline_desc& desc)
{
	desc.colors[0].blend.enabled = true;
	desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
	desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
	desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
}

static void pipe_vertex_colored(sg_pipeline_desc& desc)
{
	desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
	desc.index_type = SG_INDEXTYPE_UINT16;
	desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
	desc.layout.attrs[0].offset = 0;
	desc.layout.attrs[1].format = SG_VERTEXFORMAT_UBYTE4N;
	desc.layout.attrs[1].offset = 12;
	desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;
	desc.layout.attrs[2].offset = 16;
}

static void pipe_vertex_mesh(sg_pipeline_desc& desc)
{
	desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
	desc.index_type = SG_INDEXTYPE_UINT16;
	desc.layout.buffers[0].stride = 16;
	desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
	desc.layout.attrs[0].offset = 0;
	desc.layout.attrs[1].format = SG_VERTEXFORMAT_SHORT2N;
	desc.layout.attrs[1].offset = 12;
}

static void pipe_vertex_mesh_pos(sg_pipeline_desc& desc)
{
	desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
	desc.index_type = SG_INDEXTYPE_UINT16;
	desc.layout.buffers[0].stride = 16;
	desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
	desc.layout.attrs[0].offset = 0;
}

static void pipe_depth_buffer(sg_pipeline_desc& desc)
{
	desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
	desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
	desc.depth.write_enabled = true;
}

void pipelines_init()
{
	sg_backend backend = sg_query_backend();
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(blit_prog_shader_desc(backend));
		desc.label = "blit";
		pipe_full_tri(desc);
		s_fx_pipes[pip_blit] = sg_make_pipeline(desc);

		desc.label = "blitToSwap";
		desc.colors[0].pixel_format = _SG_PIXELFORMAT_DEFAULT;
		desc.depth.pixel_format = _SG_PIXELFORMAT_DEFAULT;
		s_fx_pipes[pip_blitToSwap] = sg_make_pipeline(desc);

		desc.label = "composeBloom";
		pipe_full_tri(desc);
		pipe_blend_add(desc);
		s_fx_pipes[pip_postComposeBloom] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "postComposeToon";
		desc.shader = sg_make_shader(postComposeToon_prog_shader_desc(backend));
		pipe_full_tri(desc);
		desc.colors[0].blend.enabled = true;
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
		s_fx_pipes[pip_postComposeToon] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "postBlurStep";
		desc.shader = sg_make_shader(postBlurStep_prog_shader_desc(backend));
		pipe_full_tri(desc);
		s_fx_pipes[pip_postBlurStep] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "postToon";
		desc.shader = sg_make_shader(postToon_prog_shader_desc(backend));
		pipe_full_tri(desc);
		s_fx_pipes[pip_postToon] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "billboards";
		desc.shader = sg_make_shader(billboards_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;

		pipe_vertex_colored(desc);
		pipe_blend_alpha(desc);
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		desc.cull_mode = SG_CULLMODE_NONE;
		s_fx_pipes[pip_billboards] = sg_make_pipeline(desc);

        desc.label = "billboardsClearDestAlpha";
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
		s_fx_pipes[pip_billboardsClearDestAlpha] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "overlay1";
		desc.shader = sg_make_shader(overlay_prog_shader_desc(backend));
		pipe_full_tri(desc);
		pipe_blend_add(desc);
		s_fx_pipes[pip_overlay1] = sg_make_pipeline(desc);

        desc.label = "overlay2";
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_DST_COLOR;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_DST_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
		s_fx_pipes[pip_overlay2] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "renderWhite";
		desc.shader = sg_make_shader(renderWhite_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		pipe_vertex_mesh_pos(desc);
		desc.cull_mode = SG_CULLMODE_FRONT;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		s_fx_pipes[pip_renderWhite] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "shadowCaster";
		desc.shader = sg_make_shader(shadowCaster_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_NONE;
		pipe_vertex_mesh_pos(desc);
		pipe_depth_buffer(desc);
		desc.cull_mode = SG_CULLMODE_BACK; // render backfaces into the shadowmap (reduces acne)
		s_fx_pipes[pip_shadowCaster] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "lines";
		desc.shader = sg_make_shader(lines_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		pipe_depth_buffer(desc);

		pipe_vertex_colored(desc);
		desc.sample_count = kMainAA;
		// blend, no depth write, no cull
		desc.colors[0].blend.enabled = true;
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.depth.write_enabled = false;
		desc.cull_mode = SG_CULLMODE_NONE;
		s_fx_pipes[pip_lines] = sg_make_pipeline(desc);

        desc.label = "linesNoAA";
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		desc.sample_count = 1;
		s_fx_pipes[pip_linesNoAA] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "renderLitShadowed";
		desc.shader = sg_make_shader(renderLitShadowed_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		pipe_depth_buffer(desc);
		pipe_vertex_mesh(desc);
		desc.sample_count = kMainAA;
		desc.cull_mode = SG_CULLMODE_FRONT;
		s_fx_pipes[pip_renderLitShadowed] = sg_make_pipeline(desc);
        desc.label = "renderLitShadowedFlip";
		desc.cull_mode = SG_CULLMODE_BACK; // reflection; inverted culling
		s_fx_pipes[pip_renderLitShadowedFlip] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "renderLit";
		desc.shader = sg_make_shader(renderLit_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		pipe_depth_buffer(desc);
		pipe_vertex_mesh(desc);
		desc.sample_count = kMainAA;
		desc.cull_mode = SG_CULLMODE_FRONT;
		s_fx_pipes[pip_renderLit] = sg_make_pipeline(desc);
	}
	{
		sg_pipeline_desc desc = {};
        desc.label = "renderReflective";
		desc.shader = sg_make_shader(renderReflective_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		pipe_depth_buffer(desc);
		pipe_vertex_mesh_pos(desc);
		desc.sample_count = kMainAA;
		desc.cull_mode = SG_CULLMODE_FRONT;
		pipe_blend_alpha(desc);
		desc.depth.write_enabled = false;
        if (kReflectionsDepthBias)
            desc.depth.bias = -10.0f;
		s_fx_pipes[pip_renderReflective] = sg_make_pipeline(desc);
	}
}

void pipeline_apply(Pipeline fx)
{
    assert(s_fx_pipes[fx].id != 0);
	sg_apply_pipeline(s_fx_pipes[fx]);
}
