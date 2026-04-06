// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __CUBE_TEXTURE_BUNDLE_H
#define __CUBE_TEXTURE_BUNDLE_H

#include "BundleSingleton.h"
#include "StorageResourceBundle.h"
#include "../kernel/Proxies.h"
#include "DeviceResource.h"

namespace dingus {


class CCubeTextureBundle :	public CStorageResourceBundle<CD3DCubeTexture>,
						public CBundleSingleton<CCubeTextureBundle>,
						public IDeviceReloadableBundle
{
public:
	static void init( const std::string& predir ) {
		CCubeTextureBundle* bundle = new CCubeTextureBundle( predir );
		assert( bundle );
		assignInstance( *bundle );
	}

	virtual void createResource();
	virtual void activateResource();
	virtual void passivateResource();
	virtual void deleteResource();

protected:
	virtual CD3DCubeTexture* loadResourceById( const CResourceId& id, const CResourceId& fullName );
	virtual void deleteResource( CD3DCubeTexture& resource ) { delete &resource; }
	
private:
	CCubeTextureBundle( const std::string& predir );
	virtual ~CCubeTextureBundle() { clear(); };
	friend CBundleSingleton<CCubeTextureBundle>;
	
	IDirect3DCubeTexture9* loadTexture( const CResourceId& id, const CResourceId& fullName ) const;
};


}; // namespace


#endif
