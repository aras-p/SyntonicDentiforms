#include "Texture.h"

#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../../external/stb_image.h"

void sokol_texture::create(const sg_image_desc &desc)
{
    assert(image.id == 0);

    image = sg_make_image(&desc);
    assert(sg_query_image_state(image) == SG_RESOURCESTATE_VALID);

    if (desc.usage.color_attachment)
    {
        sg_view_desc vdesc = {};
        vdesc.color_attachment.image = image;
        view_rt = sg_make_view(&vdesc);
        assert(sg_query_view_state(view_rt) == SG_RESOURCESTATE_VALID);
    }

    if (desc.usage.depth_stencil_attachment)
    {
        sg_view_desc vdesc = {};
        vdesc.depth_stencil_attachment.image = image;
        view_z = sg_make_view(&vdesc);
        assert(sg_query_view_state(view_z) == SG_RESOURCESTATE_VALID);
    }

    if (desc.usage.resolve_attachment)
    {
        sg_view_desc vdesc = {};
        vdesc.resolve_attachment.image = image;
        view_resolve = sg_make_view(&vdesc);
        assert(sg_query_view_state(view_resolve) == SG_RESOURCESTATE_VALID);
    }

    if (desc.sample_count <= 1) // only allow sampling it if not MSAA'd (mostly WebGL2 limitation)
    {
        sg_view_desc vdesc = {};
        vdesc.texture.image = image;
        view_tex = sg_make_view(&vdesc);
        assert(sg_query_view_state(view_tex) == SG_RESOURCESTATE_VALID);
    }
}

void sokol_texture::destroy()
{
    if (image.id)
        sg_destroy_image(image);
    image = {};
    if (view_rt.id)
        sg_destroy_view(view_rt);
    view_rt = {};
    if (view_z.id)
        sg_destroy_view(view_z);
    view_z = {};
    if (view_resolve.id)
        sg_destroy_view(view_resolve);
    view_resolve = {};
    if (view_tex.id)
        sg_destroy_view(view_tex);
    view_tex = {};
}

sokol_texture *load_texture(const char *path)
{
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 4);
    if (data != nullptr)
    {
        sg_image_desc desc = {};
        desc.width = width;
        desc.height = height;
        desc.data.mip_levels[0].ptr = data;
        desc.data.mip_levels[0].size = width * height * 4;
        sokol_texture *res = new sokol_texture();
        res->create(desc);
        stbi_image_free(data);
        return res;
    }

    return nullptr;
}
