// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#pragma once

#if 0 //@TODO

namespace dingus {

class CRenderable;


class CRenderContext {
public:
	CRenderContext();

	void attach( CRenderable& r ) { mRenderables.push_back(&r); };
	void clear() { mRenderables.clear(); };

	void perform();

	//
	// "direct" rendering

	void directBegin();
	void directRender( CRenderable& r );
	void directEnd();
	
private:
	void	renderRenderables();
	void	initGlobalFX();

private:
	typedef std::vector<CRenderable*>	TRenderableVector;
private:
	TRenderableVector	mRenderables;

	// direct rendering part
	bool			mInsideDirect;
	int				mDirectCurrPasses;
};


}; // namespace

#endif
