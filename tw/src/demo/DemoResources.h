#ifndef __MYDEMO_RES_H
#define __MYDEMO_RES_H

#include <dingus/math/Vector4.h>
#include <dingus/gfx/RenderableOrderedBillboards.h>

// --------------------------------------------------------------------------
// misc

#define RID_IB_QUADSTRIP "ibqstrip"
#define RID_IB_QUADS "ibquads"

class CLineRenderer;
extern CLineRenderer*	gLineRenderer;

class CBloomPostProcess;
extern CBloomPostProcess*	gPPSBloom;

extern CRenderableOrderedBillboards*	gBillboardsNormal;
extern CRenderableOrderedBillboards*	gBillboardsNoDestA;
extern SVector4			gScreenFixUVs;


// --------------------------------------------------------------------------
// main rendertargets

// full screen
#define RT_FULLSCREEN_1 "mainRT1"
#define RT_FULLSCREEN_2 "mainRT2"

// 1/4 of the screen
#define RT_4thSCREEN_1 "4thRT1"
#define RT_4thSCREEN_2 "4thRT2"


// --------------------------------------------------------------------------
// cube reflections

const float SZ_REFL_REL = 0.5f;

// textures
#define RT_REFL_PX "reflpx"
#define RT_REFL_NX "reflnx"
#define RT_REFL_PY "reflpy"
#define RT_REFL_NY "reflny"
#define RT_REFL_PZ "reflpz"
#define RT_REFL_NZ "reflnz"
// rendertarget and z/stencil
#define RT_REFLRT "reflRT"
#define RT_REFLZ "reflZ"

// --------------------------------------------------------------------------
// shadow maps

const int SZ_SHADOWMAP = 1024;

#define RT_SHADOWRT "shadowRT"
#define RT_SHADOWZ "shadowZ"


#endif
