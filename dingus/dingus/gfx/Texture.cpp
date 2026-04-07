#include "Texture.h"

void sokol_texture::create(const sg_image_desc& desc)
{
	assert(image.id == 0);

	image = sg_make_image(&desc);
	ASSERT_MSG(sg_query_image_state(image) == SG_RESOURCESTATE_VALID, "Failed to create Image");

	if (desc.usage.color_attachment)
	{
		sg_view_desc vdesc = {};
		vdesc.color_attachment.image = image;
		view_rt = sg_make_view(&vdesc);
		ASSERT_MSG(sg_query_view_state(view_rt) == SG_RESOURCESTATE_VALID, "Failed to create RT View");
	}

	if (desc.usage.depth_stencil_attachment)
	{
		sg_view_desc vdesc = {};
		vdesc.depth_stencil_attachment.image = image;
		view_z = sg_make_view(&vdesc);
		ASSERT_MSG(sg_query_view_state(view_z) == SG_RESOURCESTATE_VALID, "Failed to create Z View");
	}

	if (desc.usage.resolve_attachment)
	{
		sg_view_desc vdesc = {};
		vdesc.resolve_attachment.image = image;
		view_resolve = sg_make_view(&vdesc);
		ASSERT_MSG(sg_query_view_state(view_resolve) == SG_RESOURCESTATE_VALID, "Failed to create Resolve View");
	}

	{
		sg_view_desc vdesc = {};
		vdesc.texture.image = image;
		view_tex = sg_make_view(&vdesc);
		ASSERT_MSG(sg_query_view_state(view_tex) == SG_RESOURCESTATE_VALID, "Failed to create Texture View");
	}
}

void sokol_texture::destroy()
{
	if (image.id) sg_destroy_image(image); image = {};
	if (view_rt.id) sg_destroy_view(view_rt); view_rt = {};
	if (view_z.id) sg_destroy_view(view_z); view_z = {};
	if (view_resolve.id) sg_destroy_view(view_resolve); view_resolve = {};
	if (view_tex.id) sg_destroy_view(view_tex); view_tex = {};
}
