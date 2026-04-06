#ifndef __GLARE_H
#define __GLARE_H

#include <dingus/renderer/RenderableMesh.h>


class CBloomPostProcess {
public:
	CBloomPostProcess();
	~CBloomPostProcess();
	
	void	renderBloom();

	// Ping-pong blurs between 4th screen 1 and 2, starting from 1
	void	pingPongBlur( int passes );

private:
	CRenderableMesh*	mBloomPingPongs[2];
	CRenderableMesh*	mComposite;
	SVector4			mFixUVs4th;
	SVector4			mTexelOffsetsX;
	SVector4			mTexelOffsetsY;
};


#endif
