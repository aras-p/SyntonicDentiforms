// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#include "TextureBundle.h"
#include "../utils/Errors.h"

#include "dds_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../tw/src/external/stb_image.h"

using namespace dingus;

CTextureBundle::CTextureBundle( const std::string& predir )
:	CStorageResourceBundle<sokol_texture>(predir)
{
	addExtension( ".dds" );
	addExtension( ".png" );
};

sokol_texture* CTextureBundle::loadResourceById( const CResourceId& id, const CResourceId& fullName )
{
	int width, height, channels;

	// try stb_image
	unsigned char* data = stbi_load(fullName.getUniqueName().c_str(), &width, &height, &channels, 4);
	if (data != nullptr)
	{
		sg_image_desc desc = {};
		desc.width = width;
		desc.height = height;
		desc.data.mip_levels[0].ptr = data;
		desc.data.mip_levels[0].size = width * height * 4;
		sokol_texture* res = new sokol_texture();
		res->create(desc);
		stbi_image_free(data);
		return res;
	}
	
	// try dds loader
	DDSFormat format;
	void* dds_data = nullptr;
	size_t dds_size;
	//@TODO: mipmaps
	if (load_dds(fullName.getUniqueName().c_str(), &width, &height, &format, &dds_data, &dds_size))
	{
		sg_image_desc desc = {};
		desc.width = width;
		desc.height = height;
		desc.data.mip_levels[0].ptr = dds_data;
		desc.data.mip_levels[0].size = dds_size;
		switch (format) {
		case DDSFormat::BC1: desc.pixel_format = SG_PIXELFORMAT_BC1_RGBA; break;
		case DDSFormat::BC2: desc.pixel_format = SG_PIXELFORMAT_BC2_RGBA; break;
		case DDSFormat::BC3: desc.pixel_format = SG_PIXELFORMAT_BC3_RGBA; break;
		case DDSFormat::BC4: desc.pixel_format = SG_PIXELFORMAT_BC4_R; break;
		case DDSFormat::BC5: desc.pixel_format = SG_PIXELFORMAT_BC5_RG; break;
		case DDSFormat::BC6HU: desc.pixel_format = SG_PIXELFORMAT_BC6H_RGBUF; break;
		case DDSFormat::BC6HS: desc.pixel_format = SG_PIXELFORMAT_BC6H_RGBF; break;
		case DDSFormat::BC7: desc.pixel_format = SG_PIXELFORMAT_BC7_RGBA; break;
		}
		sokol_texture* res = new sokol_texture();
		res->create(desc);
		free(dds_data);
		return res;
	}

	return nullptr;
}
