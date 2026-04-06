
#ifndef __KERNEL_SYSTEM_H
#define __KERNEL_SYSTEM_H

#include <dingus/dxutils/D3DApplication.h>

namespace dingus {
	class CDeviceResourceManager;
};


class CSystem : public dingus::CD3DApplication {
public:
	CSystem();
	virtual ~CSystem();

	LRESULT msgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

protected:
	virtual HRESULT initialize();
	virtual HRESULT createDeviceObjects();
	virtual HRESULT activateDeviceObjects();
	virtual HRESULT passivateDeviceObjects();
	virtual HRESULT deleteDeviceObjects();
	virtual HRESULT performOneTime();
	virtual HRESULT shutdown();
	virtual HRESULT checkDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT bbFormat );

protected:
	virtual bool appCheckDevice( const D3DCAPS9& caps, DWORD behavior, D3DFORMAT bbFormat ) const = 0;
	virtual void appInitialize() = 0;
	virtual void appPerform() = 0;
	virtual void appShutdown() = 0;

private:
	std::string 						mDataPath;
	dingus::CDeviceResourceManager*		mDeviceManager;
	bool	mAppInited;
};


#endif
