// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __PROXIES_H
#define __PROXIES_H

namespace dingus {


class CBaseProxyClass {
public:
	bool	isNull() const { return (mPtr==0); }
	void*	getPtr() const { return mPtr; }

protected:
	CBaseProxyClass( void* u = 0 ) : mPtr(u) { }
	void	setPtr( void* u ) { mPtr = u; }

private:
	void*	mPtr;
};



template<typename T>
class CProxyClass : public CBaseProxyClass {
public:
	CProxyClass( T* object = 0 ) : CBaseProxyClass(object) { }

	T*		getObject() const { return reinterpret_cast<T*>(getPtr()); }
	void	setObject( T* object ) { setPtr(object); }
};

typedef CProxyClass<IDirect3DTexture9>		CD3DTexture;
typedef CProxyClass<IDirect3DSurface9>		CD3DSurface;

typedef CProxyClass<IDirect3DIndexBuffer9>	CD3DIndexBuffer;
typedef CProxyClass<IDirect3DVertexBuffer9>	CD3DVertexBuffer;

typedef CProxyClass<IDirect3DVertexDeclaration9>	CD3DVertexDecl;

}; // namespace


#include "ProxyEffect.h"

#endif
