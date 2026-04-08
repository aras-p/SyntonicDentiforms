
#if defined(__APPLE__)
#define SOKOL_METAL
#elif defined(_WIN32)
#define SOKOL_D3D11
#else
#define SOKOL_GLCORE
#endif

#define SOKOL_IMPL
#include "external/sokol_app.h"
#include "external/sokol_gfx.h"
#include "external/sokol_log.h"
#include "external/sokol_glue.h"

#include "../demo/Demo.h"

static void init(void)
{
    {
        sg_desc desc = {};
        desc.environment = sglue_environment();
        desc.logger.func = slog_func;
        sg_setup(&desc);
    }

    if (!demo_init()) {
        sapp_quit();
    }
}

static void frame(void)
{
    if (!demo_update())
        sapp_request_quit();
}

static void cleanup(void)
{
    demo_shutdown();
    sg_shutdown();
}

static void onevent(const sapp_event* evt)
{
	demo_event(evt);
}

sapp_desc sokol_main(int argc, char* argv[])
{
	(void)argc; (void)argv;
	sapp_desc app_desc = {};
	app_desc.init_cb = init;
	app_desc.frame_cb = frame;
	app_desc.cleanup_cb = cleanup;
	app_desc.width = 640;
	app_desc.height = 500;
	app_desc.sample_count = 1;
	app_desc.window_title = "Syntonic Dentiforms";
    app_desc.icon.sokol_default = true;
    app_desc.logger.func = slog_func;
    app_desc.event_cb = onevent;

#ifdef _DEBUG
    app_desc.fullscreen = false;
    app_desc.swap_interval = 0;
#else
    app_desc.fullscreen = true;
    app_desc.swap_interval = 1;
#endif

    return app_desc;
}
