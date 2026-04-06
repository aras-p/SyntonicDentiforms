#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <dingus/renderer/RenderContext.h>
#include <dingus/kernel/D3DDevice.h>

#include <dingus/kernel/SystemClock.h>

#include <dingus/resource/TextureBundle.h>
#include <dingus/resource/MeshBundle.h>
#include <dingus/resource/EffectBundle.h>
#include <dingus/resource/SharedTextureBundle.h>
#include <dingus/resource/SharedSurfaceBundle.h>
#include <dingus/resource/AnimationBundle.h>

using namespace dingus;

extern dingus::CRenderContext*	G_RCTX;


// Resource shortcut macros
#define RGET_FX(id) dingus::CEffectBundle::getInstance().getResourceById(id)
#define RGET_TEX(id) dingus::CTextureBundle::getInstance().getResourceById(id)
#define RGET_MESH(id) dingus::CMeshBundle::getInstance().getResourceById(id)
#define RGET_ANIM(id) dingus::CAnimationBundle::getInstance().getResourceById(id)
#define RGET_STEX(id) dingus::CSharedTextureBundle::getInstance().getResourceById(id)
#define RGET_SSURF(id) dingus::CSharedSurfaceBundle::getInstance().getResourceById(id)


#endif
