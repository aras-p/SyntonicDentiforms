// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __RENDER_CONTEXT_H
#define __RENDER_CONTEXT_H


#include "RenderCamera.h"
#include "EffectParams.h"


namespace dingus {

class CRenderable;


class CRenderContext {
public:
	CRenderContext();

	void attach( CRenderable& r ) { mRenderables.push_back(&r); };
	void clear() { mRenderables.clear(); };

	void perform();

	const CRenderCamera& getCamera() const { return mRenderCamera; };
	CRenderCamera& getCamera() { return mRenderCamera; };
	
	CEffectParams const& getGlobalParams() const { return mGlobalParams; };
	CEffectParams& getGlobalParams() { return mGlobalParams; };

	//
	// "direct" rendering

	void directBegin();
	void directRender( CRenderable& r );
	void directEnd();
	int directSetFX( CD3DXEffect& fx ); // returns pass count-1
	
private:
	void	sortRenderables();
	void	renderRenderables();
	void	initGlobalFX();

private:
	typedef std::vector<CRenderable*>	TRenderableVector;
private:
	TRenderableVector	mRenderables;
	CRenderCamera		mRenderCamera;

	CEffectParams		mGlobalParams;
	CD3DXEffect*		mGlobalEffect;

	bool mGlobalInited;

	// direct rendering part
	bool			mInsideDirect;
	CD3DXEffect*	mDirectCurrFX;
	int				mDirectCurrPasses;
};


}; // namespace

#endif
