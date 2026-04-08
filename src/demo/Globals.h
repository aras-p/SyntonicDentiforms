#pragma once

#include "RenderCamera.h"

#include <src/resource/AnimationBundle.h>


extern CRenderCamera gRenderCam;

#define RGET_ANIM(id) CAnimationBundle::getInstance().getResourceById(id)
