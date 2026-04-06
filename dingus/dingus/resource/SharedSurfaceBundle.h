// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#pragma warning(disable:4786)
#ifndef __SHARED_SURFACE_BUNDLE_H
#define __SHARED_SURFACE_BUNDLE_H

#include "BundleSingleton.h"
#include "SharedResourceBundle.h"
#include "SurfaceCreator.h"
#include "../kernel/Proxies.h"
#include "DeviceResource.h"

namespace dingus {


class CSharedSurfaceBundle :public CSharedResourceBundle<CD3DSurface,ISurfaceCreator::TSharedPtr>,
							public CBundleSingleton<CSharedSurfaceBundle>,
							public IDeviceResource
{
public:
	static void init() {
		CSharedSurfaceBundle* bundle = new CSharedSurfaceBundle();
		assert( bundle );
		assignInstance( *bundle );
	}

	void registerSurface( CResourceId const& id, const ISurfaceCreator::TSharedPtr& creator );

	virtual void createResource();
	virtual void activateResource();
	virtual void passivateResource();
	virtual void deleteResource();

protected:
	virtual void deleteResource( CD3DSurface& resource ) { delete &resource; }

private:
	CSharedSurfaceBundle() { };
	virtual ~CSharedSurfaceBundle() { clear(); };
	friend CBundleSingleton<CSharedSurfaceBundle>;
};

}; // namespace

#endif
