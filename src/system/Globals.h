#pragma once

#include "../demo/RenderCamera.h"

#include <src/resource/TextureBundle.h>
#include <src/resource/MeshBundle.h>
#include <src/resource/AnimationBundle.h>

using namespace dingus;

extern CRenderCamera gRenderCam;

#define RGET_TEX(id) dingus::CTextureBundle::getInstance().getResourceById(id)
#define RGET_MESH(id) dingus::CMeshBundle::getInstance().getResourceById(id)
#define RGET_ANIM(id) dingus::CAnimationBundle::getInstance().getResourceById(id)
