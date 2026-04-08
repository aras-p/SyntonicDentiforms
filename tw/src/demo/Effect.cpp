#include "Effect.h"
#include "DemoResources.h"

#include "../fx/billboards.glsl.h"
#include "../fx/blit.glsl.h"
#include "../fx/caster.glsl.h"
#include "../fx/compositeAdd.glsl.h"
#include "../fx/compositeAlpha.glsl.h"
#include "../fx/filterBloom.glsl.h"
#include "../fx/filterToon.glsl.h"
#include "../fx/lines.glsl.h"
#include "../fx/noshadowHi.glsl.h"
#include "../fx/overlay.glsl.h"
#include "../fx/receiverLo.glsl.h"
#include "../fx/receiverHi.glsl.h"
#include "../fx/reflective.glsl.h"

#include "../external/sokol_gfx.h"

static sg_pipeline s_fx_pipes[fxCount] = {};

void effects_init()
{
	sg_backend backend = sg_query_backend();
	// blit
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(blit_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
		s_fx_pipes[fx_blit] = sg_make_pipeline(desc);
	}
	// compositeAdd
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(compositeAdd_prog_shader_desc(backend));
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
		// Z off
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		s_fx_pipes[fx_compositeAdd] = sg_make_pipeline(desc);
	}
	// compositeAlpha
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(compositeAlpha_prog_shader_desc(backend));
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		// blend, no depth, no cull
		desc.colors[0].blend.enabled = true;
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		desc.cull_mode = SG_CULLMODE_NONE;
		s_fx_pipes[fx_compositeAlpha] = sg_make_pipeline(desc);
	}
	// filterBloom
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(filterBloom_prog_shader_desc(backend));
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		// Z off
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		s_fx_pipes[fx_filterBloom] = sg_make_pipeline(desc);
	}
	// filterToon
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(filterToon_prog_shader_desc(backend));
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		// Z off
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		s_fx_pipes[fx_filterToon] = sg_make_pipeline(desc);
	}
	// billboards
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(billboards_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.layout.attrs[1].format = SG_VERTEXFORMAT_UBYTE4N;
		desc.layout.attrs[1].offset = 12;
		desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;
		desc.layout.attrs[2].offset = 16;
		// blend, no depth, no cull
		desc.colors[0].blend.enabled = true;
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		desc.cull_mode = SG_CULLMODE_NONE;
		s_fx_pipes[fx_billboards] = sg_make_pipeline(desc);
	}
	// overlay /2
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(overlay_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP;
		// additive blend, no depth, no cull
		desc.colors[0].blend.enabled = true;
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_ONE;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		desc.cull_mode = SG_CULLMODE_NONE;
		s_fx_pipes[fx_overlay] = sg_make_pipeline(desc);

		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_DST_COLOR;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_DST_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
		s_fx_pipes[fx_overlay2] = sg_make_pipeline(desc);
	}
	// caster
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(caster_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.buffers[0].stride = 24;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
		desc.depth.write_enabled = true;
		desc.cull_mode = SG_CULLMODE_FRONT;
		s_fx_pipes[fx_caster] = sg_make_pipeline(desc);

		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.write_enabled = false;
		s_fx_pipes[fx_casterNoZ] = sg_make_pipeline(desc);
	}
	// lines
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(lines_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.layout.attrs[1].format = SG_VERTEXFORMAT_UBYTE4N;
		desc.layout.attrs[1].offset = 12;
		desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;
		desc.layout.attrs[2].offset = 16;
		desc.sample_count = kMainAA;
		desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
		// blend, no depth write, no cull
		desc.colors[0].blend.enabled = true;
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.depth.write_enabled = false;
		desc.cull_mode = SG_CULLMODE_NONE;
		s_fx_pipes[fx_lines] = sg_make_pipeline(desc);

		desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
		desc.depth.pixel_format = SG_PIXELFORMAT_NONE;
		desc.sample_count = 1;
		s_fx_pipes[fx_linesNoAa] = sg_make_pipeline(desc);
	}
	// receiver Lo
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(receiverLo_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.buffers[0].stride = 24;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[1].offset = 12;
		desc.sample_count = kMainAA;
		desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
		desc.depth.write_enabled = true;
		desc.cull_mode = SG_CULLMODE_BACK; // reflection; inverted culling
		s_fx_pipes[fx_receiverLo] = sg_make_pipeline(desc);
	}
	// receiver Hi
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(receiverHi_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.buffers[0].stride = 24;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[1].offset = 12;
		desc.sample_count = kMainAA;
		desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
		desc.depth.write_enabled = true;
		desc.cull_mode = SG_CULLMODE_FRONT;
		s_fx_pipes[fx_receiverHi] = sg_make_pipeline(desc);
	}
	// noshadow Hi
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(noshadowHi_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.buffers[0].stride = 24;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[1].offset = 12;
		desc.sample_count = kMainAA;
		desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
		desc.depth.write_enabled = true;
		desc.cull_mode = SG_CULLMODE_FRONT;
		s_fx_pipes[fx_noshadowHi] = sg_make_pipeline(desc);
	}
	// reflective
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(reflective_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.buffers[0].stride = 24;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.sample_count = kMainAA;
		desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
		desc.cull_mode = SG_CULLMODE_FRONT;
		// blend, no depth write
		desc.colors[0].blend.enabled = true;
		desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		desc.depth.write_enabled = false;
		s_fx_pipes[fx_reflective] = sg_make_pipeline(desc);
	}
}

void effect_apply(Effect fx)
{
	ASSERT_MSG(s_fx_pipes[fx].id != 0, "Uninitialized effect pipeline");
	sg_apply_pipeline(s_fx_pipes[fx]);
}
