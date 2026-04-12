
#if defined(__APPLE__)
#define SOKOL_METAL
#elif defined(_WIN32) && !USE_WINDOWS_OPENGL
#define SOKOL_D3D11
#elif defined(__EMSCRIPTEN__)
#define SOKOL_GLES3
#else
#define SOKOL_GLCORE
#endif

#define SOKOL_IMPL
#include "../../external/sokol_app.h"
#include "../../external/sokol_gfx.h"
#include "../../external/sokol_log.h"
#include "../../external/sokol_glue.h"
#include "../../external/sokol_time.h"
#include "../../external/sokol_debugtext.h"

#if !USE_WINDOWS_OPENGL
#define SOKOL_GFX_UTILS_IMPL
#include "../../external/sokol_gfx_utils.h"
#endif
