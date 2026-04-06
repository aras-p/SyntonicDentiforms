#ifndef __DEMO_H
#define __DEMO_H

#include "../system/System.h"


class CDemo : public CSystem {
public:
	CDemo();
	
protected:
	virtual bool appCheckDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT bbFormat ) const;
	virtual void appInitialize();
	virtual void appPerform();
	virtual void appShutdown();
};


#endif
