// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __SHARED_TEXTURE_BUNDLE_H
#define __SHARED_TEXTURE_BUNDLE_H

#include "BundleSingleton.h"
#include "SharedResourceBundle.h"
#include "TextureCreator.h"
#include "../kernel/Proxies.h"
#include "DeviceResource.h"

namespace dingus {


class CSharedTextureBundle :	public CSharedResourceBundle<CD3DTexture,ITextureCreator::TSharedPtr>,
								public CBundleSingleton<CSharedTextureBundle>,
								public IDeviceResource
{
public:
	static void init() {
		CSharedTextureBundle* bundle = new CSharedTextureBundle();
		assert( bundle );
		assignInstance( *bundle );
	}

	void registerTexture( CResourceId const& id, const ITextureCreator::TSharedPtr& creator );

	virtual void createResource();
	virtual void activateResource();
	virtual void passivateResource();
	virtual void deleteResource();

protected:
	virtual void deleteResource( CD3DTexture& resource ) { delete &resource; }

private:
	CSharedTextureBundle() { };
	virtual ~CSharedTextureBundle() { clear(); };
	friend CBundleSingleton<CSharedTextureBundle>;
};

}; // namespace

#endif
