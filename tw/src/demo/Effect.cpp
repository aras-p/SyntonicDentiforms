#include "Effect.h"
#include "DemoResources.h"

#include "../fx/caster.glsl.h"
#include "../fx/receiverLo.glsl.h"
#include "../fx/receiverHi.glsl.h"
#include "../fx/reflective.glsl.h"

#include "../external/sokol_gfx.h"

static sg_pipeline s_fx_pipes[fxCount] = {};

void effects_init()
{
	sg_backend backend = sg_query_backend();
	// caster
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(caster_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		s_fx_pipes[fx_caster] = sg_make_pipeline(desc);
	}
	// receiver Lo
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(receiverLo_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[1].offset = 12;
		desc.sample_count = kMainAA;
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
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[1].offset = 12;
		desc.sample_count = kMainAA;
		s_fx_pipes[fx_receiverHi] = sg_make_pipeline(desc);
	}
	// reflective
	{
		sg_pipeline_desc desc = {};
		desc.shader = sg_make_shader(reflective_prog_shader_desc(backend));
		desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
		desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
		desc.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
		desc.index_type = SG_INDEXTYPE_UINT16;
		desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
		desc.layout.attrs[0].offset = 0;
		desc.sample_count = kMainAA;
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

void effects_shutdown()
{
	//@TODO
}

void effect_apply(Effect fx)
{
	ASSERT_MSG(s_fx_pipes[fx].id != 0, "Uninitialized effect pipeline");
	sg_apply_pipeline(s_fx_pipes[fx]);
}
