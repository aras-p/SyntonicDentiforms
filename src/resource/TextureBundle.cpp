#include "TextureBundle.h"
#include "../utils/Errors.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "external/stb_image.h"

using namespace dingus;

CTextureBundle::CTextureBundle( const std::string& predir )
:	CStorageResourceBundle<sokol_texture>(predir)
{
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
	
	return nullptr;
}
