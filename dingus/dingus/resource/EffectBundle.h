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


class IEffectInitListener {
public:
	virtual ~IEffectInitListener() = 0 { }
	virtual void onInitEffect( ID3DXEffect& fx ) = 0;
};


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

	void setInitListener( IEffectInitListener* l ) { mInitListener = l; }
	bool	isOptimizingShaders() const { return mOptimizeShaders; }
	void	setOptimizeShaders( bool opt ) { mOptimizeShaders = opt; }
	const std::string& getLastErrors() const { return mLastErrors; }

	/**
	 *  Adds macro (or replaces same-named one).
	 *  After changing a bunch of macros, call reload() to actually reload effects.
	 *  NOTE: name/value strings are not copied, so look at their lifetime!
	 */
	void setMacro( const char* name, const char* value );
	/**
	 *  Removes macro.
	 *  After changing a bunch of macros, call reload() to actually reload effects.
	 */
	void removeMacro( const char* name );

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

	/// @return index into macro array, or -1 if not found.
	int		findMacro( const char* name ) const;
	

	typedef std::vector<D3DXMACRO>	TMacroVector;

private:
	CEffectStateManager*	mStateManager;
	ID3DXEffectPool*		mSharedPool;
	IEffectInitListener*	mInitListener;

	bool					mOptimizeShaders;
	mutable std::string		mLastErrors;

	TMacroVector			mMacros;
};


}; // namespace

#endif
