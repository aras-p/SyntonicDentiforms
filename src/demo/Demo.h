#pragma once

#define PLAY_MODE_MUSIC 1   // play music, timing from that
#define PLAY_MODE_CAPTURE 2 // render at 60FPS, timing as if with music, save screenshots
#define PLAY_MODE_DEBUG 3   // no playback, manual frame/time control with keyboard

#ifdef _DEBUG
#define DEMO_PLAY_MODE PLAY_MODE_DEBUG
#else
// #define DEMO_PLAY_MODE PLAY_MODE_CAPTURE
#define DEMO_PLAY_MODE PLAY_MODE_MUSIC
#endif

struct sapp_event;

bool demo_init();
bool demo_update();
void demo_shutdown();
void demo_event(const sapp_event *evt);