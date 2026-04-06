// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __DEV_RELOADABLE_RESOURCE_H
#define __DEV_RELOADABLE_RESOURCE_H

#include "ReloadableBundle.h"


namespace dingus {


/**
 *  D3D device dependant resource.
 */
class IDeviceResource {
public:
	virtual ~IDeviceResource() = 0 {};

	virtual void createResource() = 0;
	virtual void activateResource() = 0;
	virtual void passivateResource() = 0;
	virtual void deleteResource() = 0;
};


class IDeviceReloadableBundle : public IDeviceResource, public IReloadableBundle {
public:
	virtual void reload() {
		passivateResource();
		deleteResource();
		createResource();
		activateResource();
	}
};

class CDeviceResourceManager : public IDeviceResource {
public:
	virtual ~CDeviceResourceManager() { }

	void add( IDeviceResource& r ) { mListeners.addListener(r); }
	void remove( IDeviceResource& r ) { mListeners.removeListener(r); }
	void clear() { mListeners.clear(); }


	// IDeviceResource
	virtual void createResource() { mListeners.notify(CMD_CREATE); }
	virtual void activateResource() { mListeners.notify(CMD_ACTIVATE); }
	virtual void passivateResource() { mListeners.notify(CMD_PASSIVATE); }
	virtual void deleteResource() { mListeners.notify(CMD_DELETE); }

private:
	enum eNotifyCmd { CMD_ACTIVATE, CMD_PASSIVATE, CMD_CREATE, CMD_DELETE };
	class CResourceNotifier : public CAbstractNotifierData<IDeviceResource,eNotifyCmd> {
	protected:
		virtual void notifyListener( IDeviceResource& listener, eNotifyCmd cmd ) {
			switch(cmd) {
			case CMD_ACTIVATE:	listener.activateResource(); return;
			case CMD_PASSIVATE: listener.passivateResource(); return;
			case CMD_CREATE:	listener.createResource(); return;
			case CMD_DELETE:	listener.deleteResource(); return;
			}
		}
	};
	friend class CResourceNotifier;

private:
	CResourceNotifier	mListeners;
};


}; // namespace

#endif
