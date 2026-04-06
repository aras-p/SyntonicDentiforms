// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __RELOADABLE_BUNDLE_H
#define __RELOADABLE_BUNDLE_H

#include "../utils/AbstractNotifier.h"

namespace dingus {


/**
 *  Reloadable resource (usually bundle).
 *
 *  Is able to reload all the resources it contains. The application won't
 *  notice this, because it's dealing with proxies anyway. Reloading should
 *  preserve the resource proxy objects, only change the objects they refer to.
 */
class IReloadableBundle {
public:
	virtual ~IReloadableBundle() = 0 { }

	virtual void reload() = 0;
};


/**
 *  Manager for reloadable bundles.
 */
class CReloadableBundleManager : public IReloadableBundle {
public:
	virtual ~CReloadableBundleManager() { }

	void add( IReloadableBundle& r ) { mListeners.addListener(r); }
	void remove( IReloadableBundle& r ) { mListeners.removeListener(r); }
	void clear() { mListeners.clear(); }


	// IReloadableBundle
	virtual void reload() { mListeners.notify(); }

private:
	enum eNotifyCmd { CMD_ACTIVATE, CMD_PASSIVATE, CMD_CREATE, CMD_DELETE };
	class CReloadableNotifier : public CAbstractNotifier<IReloadableBundle> {
	protected:
		virtual void notifyListener( IReloadableBundle& listener ) {
			listener.reload();
		}
	};
	friend class CReloadableNotifier;

private:
	CReloadableNotifier	mListeners;
};



}; // namespace

#endif
