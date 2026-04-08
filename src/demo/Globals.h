#pragma once

#include "RenderCamera.h"

#include <src/resource/TextureBundle.h>
#include <src/resource/MeshBundle.h>
#include <src/resource/AnimationBundle.h>


extern CRenderCamera gRenderCam;

#define RGET_TEX(id) CTextureBundle::getInstance().getResourceById(id)
#define RGET_MESH(id) CMeshBundle::getInstance().getResourceById(id)
#define RGET_ANIM(id) CAnimationBundle::getInstance().getResourceById(id)
