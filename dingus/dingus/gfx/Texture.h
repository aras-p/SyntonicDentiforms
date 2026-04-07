#pragma once

#include "../tw/src/external/sokol_gfx.h"

struct sokol_texture
{
	~sokol_texture()
	{
		destroy();
	}

	void create(const sg_image_desc& desc);

	void destroy();

	bool valid() const { return image.id != 0; }

	sg_image image = {};
	sg_view view_rt = {};
	sg_view view_z = {};
	sg_view view_resolve = {};
	sg_view view_tex = {};
};
