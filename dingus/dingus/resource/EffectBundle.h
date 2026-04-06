// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __EFFECT_BUNDLE_H
#define __EFFECT_BUNDLE_H

#include "BundleSingleton.h"
#include "StorageResourceBundle.h"
#include "../kernel/Proxies.h"
#include "DeviceResource.h"

namespace dingus {

class CEffectStateManager;


/**
 *  D3DX Effects bundle.
 *
 *  Enables conditional compilation of effects via macros (name-value pairs),
 *  see setMacro() and removeMacro().
 */
class CEffectBundle :	public CStorageResourceBundle<CD3DXEffect>,
						public CBundleSingleton<CEffectBundle>,
						public IDeviceReloadableBundle
{
public:
	static void init( const std::string& predir ) {
		CEffectBundle* bundle = new CEffectBundle( predir );
		assert( bundle );
		assignInstance( *bundle );
	}

	bool	isOptimizingShaders() const { return mOptimizeShaders; }
	void	setOptimizeShaders( bool opt ) { mOptimizeShaders = opt; }
	const std::string& getLastErrors() const { return mLastErrors; }

	virtual void createResource();
	virtual void activateResource();
	virtual void passivateResource();
	virtual void deleteResource();

protected:
	virtual CD3DXEffect* loadResourceById( const CResourceId& id, const CResourceId& fullName );
	virtual void deleteResource( CD3DXEffect& resource ) { delete &resource; }
	
private:
	CEffectBundle( const std::string& predir );
	virtual ~CEffectBundle() { clear(); };
	friend CBundleSingleton<CEffectBundle>;
	
	ID3DXEffect* loadEffect( const CResourceId& id, const CResourceId& fullName ) const;

private:
	CEffectStateManager*	mStateManager;
	ID3DXEffectPool*		mSharedPool;

	bool					mOptimizeShaders;
	mutable std::string		mLastErrors;
};


}; // namespace

#endif
