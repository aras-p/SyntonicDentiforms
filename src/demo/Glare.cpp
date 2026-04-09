#include "Glare.h"
#include "Pipelines.h"
#include "DemoResources.h"

#include "external/sokol_app.h"
#include "external/sokol_glue.h"

const int BLOOM_PASSES = 4;

void pingPongBlur(int passes)
{
	sokol_texture* pingPong[2] = {&rt_4th_2, &rt_4th_1};

	int swidth = sapp_width() / 4;
	int sheight = sapp_height() / 4;

	const SVector4 offsetX( 1, 1,-1,-1);
	const SVector4 offsetY( 1,-1,-1, 1);
	for( int i = 0; i < passes; ++i ) {
		const float pixDist = i + 0.5f;
		SVector4 offset(pixDist / swidth, pixDist / sheight, 0, 0);

		sg_pass pass = {};
		pass.attachments.colors[0] = pingPong[i&1]->view_rt;
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0, 0, 0, 0 };
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = pingPong[(i+1) & 1]->view_tex;
		binds.samplers[0] = s_smp_linear_clamp;

		pipeline_apply(pip_postBlurStep);

		sg_apply_uniforms(0, { &offset, sizeof(offset) });

		sg_apply_bindings(binds);
		sg_draw(0, 4, 1);

		sg_end_pass();
	}
}

void renderBloom()
{
	// downsample main into smaller RT
	{
		sg_pass pass = {};
		pass.attachments.colors[0] = rt_4th_1.view_rt;
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0, 0, 0, 0 };
		pass.action.depth.load_action = SG_LOADACTION_DONTCARE;
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = rt_main_resolved.view_tex;
		binds.samplers[0] = s_smp_linear_clamp;

		pipeline_apply(pip_blit);
		sg_apply_bindings(binds);
		sg_draw(0, 4, 1);

		sg_end_pass();
	}

	// blur
	pingPongBlur( BLOOM_PASSES );

	// composite into backbuffer
	{
		sg_pass pass = {};
		pass.swapchain = sglue_swapchain();
		pass.action.colors[0].store_action = SG_STOREACTION_STORE;
		pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value = { 0, 0, 0, 0 };
		sg_begin_pass(&pass);

		sg_bindings binds = {};
		binds.views[0] = rt_main_resolved.view_tex;
		binds.views[1] = !(BLOOM_PASSES & 1) ? rt_4th_1.view_tex : rt_4th_2.view_tex;
		binds.samplers[0] = s_smp_linear_clamp;		

		pipeline_apply(pip_postComposeBloom);
		sg_apply_bindings(binds);
		sg_draw(0, 4, 1);
	}
}
