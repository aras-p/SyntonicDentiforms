// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __PROXY_EFFECT_H
#define __PROXY_EFFECT_H

#include "Proxies.h"

namespace dingus {


/**
 *  Proxy for ID3DXEffect with some addons.
 *
 *  Additionally has some indicators:
 *		- whether this effect requires strict back-to-front sorting.
 *		- required vertex shader version (0, 1.1, 2.0 or 3.0 currently).
 *	The indicators are read from effect technique annotations.
 */
class CD3DXEffect : public CBaseProxyClass {
public:
	CD3DXEffect( ID3DXEffect* object = 0 );

	/** Begins effect. @return Number of passes. */
	int		beginFx( DWORD flags );
	/** Ends effect. */
	void	endFx();
	/** Begins pass. */
	void	beginPass( int p );
	/** Ends pass. */
	void	endPass();
	/** Commits parameters that are set after begin. */
	void commitParams();
	

	ID3DXEffect* getObject() const { return reinterpret_cast<ID3DXEffect*>(getPtr()); }
	void	setObject( ID3DXEffect* object );

	bool	isBackToFrontSorted() const { return mBackToFrontSorted; }

	/**
	 *  For sorting, computed from other params in ascending order.
	 *  Orders by back-to-front (non-sorted first, then sorted), then
	 *	by vshader version in ascending order.
	 */
	int		getSortValue() const { return mSortValue; }

private:
	void	init();
	bool	tryInit();
	
private:
	int		mSortValue; // value computed from other params, for sorting
	bool	mBackToFrontSorted;
};


inline int CD3DXEffect::beginFx( DWORD flags )
{
	UINT p;
	HRESULT hr = getObject()->Begin( &p, flags );
	assert( SUCCEEDED(hr) );
	return p;
}
inline void	CD3DXEffect::endFx()
{
	HRESULT hr = getObject()->End();
	assert( SUCCEEDED(hr) );
}
inline void	CD3DXEffect::beginPass( int p )
{
	HRESULT hr = getObject()->BeginPass( p );
	assert( SUCCEEDED(hr) );
}
inline void	CD3DXEffect::endPass()
{
	HRESULT hr = getObject()->EndPass();
	assert( SUCCEEDED(hr) );
}
inline void CD3DXEffect::commitParams()
{
	HRESULT hr = getObject()->CommitChanges();
	assert( SUCCEEDED(hr) );
}


}; // namespace

#endif
