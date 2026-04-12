#pragma once

#include "../../external/sokol_gfx.h"

void dynamic_vb_init(size_t capacityBytes = 2 * 1024 * 1024);
void dynamic_vb_shutdown();
int dynamic_vb_append(const void *data, size_t size);
sg_buffer dynamic_vb_get();
