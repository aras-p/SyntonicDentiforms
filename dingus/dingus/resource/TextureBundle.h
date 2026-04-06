// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __TEXTURE_BUNDLE_H
#define __TEXTURE_BUNDLE_H

#include "BundleSingleton.h"
#include "StorageResourceBundle.h"
#include "../kernel/Proxies.h"
#include "DeviceResource.h"

namespace dingus {


class CTextureBundle :	public CStorageResourceBundle<CD3DTexture>,
						public CBundleSingleton<CTextureBundle>,
						public IDeviceReloadableBundle
{
public:
	static void init( const std::string& predir ) {
		CTextureBundle* bundle = new CTextureBundle( predir );
		assert( bundle );
		assignInstance( *bundle );
	}

	virtual void createResource();
	virtual void activateResource();
	virtual void passivateResource();
	virtual void deleteResource();

protected:
	virtual CD3DTexture* loadResourceById( const CResourceId& id, const CResourceId& fullName );
	virtual void deleteResource( CD3DTexture& resource ) { delete &resource; }
	
private:
	CTextureBundle( const std::string& predir );
	virtual ~CTextureBundle() { clear(); };
	friend CBundleSingleton<CTextureBundle>;
	
	IDirect3DTexture9* loadTexture( const CResourceId& id, const CResourceId& fullName ) const;
};


}; // namespace


#endif
