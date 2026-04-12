
#include "external/sokol_app.h"
#include "external/sokol_gfx.h"
#include "external/sokol_log.h"
#include "external/sokol_glue.h"
#include "external/sokol_time.h"
#include "external/sokol_debugtext.h"

#include "external/stb_image.h"

#include "Demo.h"

static void init(void)
{
	{
		sg_desc desc = {};
		desc.environment = sglue_environment();
		desc.logger.func = slog_func;
		sg_setup(&desc);
	}

	{
		sdtx_desc_t desc = {};
		desc.logger.func = slog_func;
		desc.fonts[0] = sdtx_font_cpc();
		sdtx_setup(&desc);
	}

	stm_setup();

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
    app_desc.high_dpi = false;
	app_desc.sample_count = 1;
	app_desc.window_title = "Syntonic Dentiforms";
    app_desc.icon.sokol_default = true;
    app_desc.logger.func = slog_func;
    app_desc.event_cb = onevent;

#if DEMO_PLAY_MODE == PLAY_MODE_DEBUG
    app_desc.fullscreen = false;
    app_desc.swap_interval = 0;
#else
    app_desc.fullscreen = true;
    app_desc.swap_interval = 1;
#endif

	// try to load icon
	int icon_width, icon_height, icon_comp;
	const char* icon_path = "data/tex/icon.png";
	uint8_t* icon_image = stbi_load(icon_path, &icon_width, &icon_height, &icon_comp, 4);
	if (icon_image != NULL) {
		app_desc.icon.sokol_default = false;
		app_desc.icon.images[0].width = icon_width;
		app_desc.icon.images[0].height = icon_height;
		app_desc.icon.images[0].pixels.ptr = icon_image;
		app_desc.icon.images[0].pixels.size = icon_width * icon_height * 4;
	}

    return app_desc;
}
