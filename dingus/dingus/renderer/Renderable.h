// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// Developed by nesnausk! team: www.nesnausk.org
// --------------------------------------------------------------------------

#ifndef __RENDERABLE_H
#define __RENDERABLE_H


#include "EffectParams.h"
#include "../utils/AbstractNotifier.h"
#include "RenderContext.h"

namespace dingus {


class CEffectSlot;
class CRenderable;


class IRenderListener {
public:
	virtual ~IRenderListener() = 0 {};

	virtual void beforeRender( CRenderable& r ) = 0;
	virtual void afterRender( CRenderable& r ) = 0;
};


class CRenderListenerAdapter : public IRenderListener {
public:
	virtual void beforeRender( CRenderable& r ) { };
	virtual void afterRender( CRenderable& r ) { };
};


/**
 *  Base renderable class.
 *
 *  Contains parameters for the rendering (effect params, priority and origin),
 *  virtual method render() and before/after rendering listeners. Note that
 *  render() may be called multiple times if the effect is multi-pass.
 *
 *  The renderable should generally only render it's geometry. The render states
 *  are to be managed by the effect and corresponding params.
 */
class CRenderable : public CSimpleNotifier<IRenderListener> {
public:
	CRenderable( const SVector3* origin = 0, int priority = 0 )
		: mOrigin(origin), mPriority(priority) { };
	virtual ~CRenderable() = 0 { };

	const CEffectParams& getParams() const { return mParams; }
	CEffectParams& getParams() { return mParams; }

	int getPriority() const { return mPriority; }
	void setPriority( int priority ) { mPriority = priority; }

	const SVector3* getOrigin() const { return mOrigin; }
	void setOrigin( const SVector3* o ) { mOrigin = o; }
	bool isOrigin() const { return mOrigin != NULL; }

	/** Notifies render listeners that the renderable will be rendered. */
	void beforeRender();
	/** Notifies render listeners that the renderable was just rendered. */
	void afterRender();

	/**
	 *  Called for each rendering pass. Here one should render it's geometry.
	 *  Note that this may be called multiple times for a multi-pass effect.
	 *  @param ctx The render context that is rendering.
	 */
	virtual void render( const CRenderContext& ctx ) { };

	/// Return used VB ("main" one, eg. stream zero) - for sorting by VB
	virtual const CD3DVertexBuffer*	getUsedVB() const = 0;
	/// Return used IB - for sorting by IB
	virtual const CD3DIndexBuffer*	getUsedIB() const = 0;

private:
	// Effect and it's params
	CEffectParams	mParams;
	// Render priority. Renderables with lesser priority will be rendered sooner.
	int				mPriority;
	// Origin of the renderable object, used for sorting. May be null.
	const SVector3*	mOrigin;
};


};

#endif
