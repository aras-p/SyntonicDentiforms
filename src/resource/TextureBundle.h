#pragma once

#include "BundleSingleton.h"
#include "StorageResourceBundle.h"
#include "../gfx/Texture.h"
#include "external/sokol_gfx.h"
#include <assert.h>


class CTextureBundle :	public CStorageResourceBundle<sokol_texture>,
						public CBundleSingleton<CTextureBundle>
{
public:
	static void init( const std::string& predir ) {
		CTextureBundle* bundle = new CTextureBundle( predir );
		assert( bundle );
		assignInstance( *bundle );
	}

protected:
	virtual sokol_texture* loadResourceById( const CResourceId& id, const CResourceId& fullName );
	virtual void deleteResource(sokol_texture& resource ) { delete &resource; }
	
private:
	CTextureBundle( const std::string& predir );
	virtual ~CTextureBundle() { clear(); };
	friend CBundleSingleton<CTextureBundle>;
};


